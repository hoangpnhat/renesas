# Databricks notebook source
# MAGIC %pip install -qqqq mlflow==2.19.0 mlflow-skinny==2.19.0 databricks-sql-connector==4.0.3
# MAGIC %pip install vllm transformers torch accelerate pandas tiktoken
# MAGIC dbutils.library.restartPython()

# COMMAND ----------

import os
import json
import time
import uuid
import mlflow
import pandas as pd
from vllm import LLM, SamplingParams
from transformers import AutoTokenizer
from typing import List, Dict, Any, Optional
import logging
from datetime import timedelta
from databricks.sdk import WorkspaceClient
from databricks.sdk.runtime import dbutils
from databricks.sdk.service.serving import (
    EndpointCoreConfigInput,
    AutoCaptureConfigInput,
    ServedEntityInput,
    TrafficConfig,
    Route,
    EndpointTag,
    ServingEndpointAccessControlRequest,
    ServingEndpointPermissionLevel,
)

from databricks.sdk.errors import ResourceDoesNotExist

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# COMMAND ----------

dbutils.widgets.text("model_name", "hoangpnhat/qwen2.5-coder-7b-awq", "model_name")
dbutils.widgets.text(
    "registered_model_name", "qwen2_5_coder_7b_awq", "registered_model_name"
)
dbutils.widgets.text("env", "dev", "env")

# COMMAND ----------

MODEL_NAME = dbutils.widgets.get("model_name")
REGISTERED_MODEL_NAME = dbutils.widgets.get("registered_model_name")
ENVIRONMENT = dbutils.widgets.get("env")

# COMMAND ----------

# vLLM configuration
VLLM_CONFIG = {
    "model": MODEL_NAME,
    "quantization": "awq",
    "dtype": "half",
    "max_model_len": 8192,
    "gpu_memory_utilization": 0.8,
    "tensor_parallel_size": 1,
    "trust_remote_code": True,
}
quant_config = {"zero_point": True, "q_group_size": 128, "w_bit": 4, "version": "GEMM"}


# COMMAND ----------

# MAGIC %md
# MAGIC #LOG MODEL
# MAGIC

# COMMAND ----------

import mlflow
from mlflow.models import infer_signature
from mlflow.exceptions import MlflowException

if "qwen" in MODEL_NAME:

    model_path = os.path.join(os.getcwd(), "./qwencoder_wrapper.py")
    # from qwencoder_wrapper import QwenCoderWrapper

    # python_model = QwenCoderWrapper()
else:
    # from qwencoder_wrapper_deepseek import QwenCoderWrapper

    # python_model = QwenCoderWrapper()
    model_path = os.path.join(os.getcwd(), "./qwencoder_wrapper_deepseek.py")


username = "hoang.pham.uh@renesas.com"
model_name = REGISTERED_MODEL_NAME
artifact_path = f"model/{model_name}"

catalog = f"ai_{ENVIRONMENT}"
schema = f"ai_{ENVIRONMENT}_coding_gold"
full_model_name = f"{catalog}.{schema}.{model_name}"
mlflow.set_registry_uri("databricks-uc")

input_example = {
    "messages": [
        {
            "role": "user",
            "content": """<|fim_prefix|>def quicksort(arr):
                            if len(arr) <= 1:
                                return arr
                            pivot = arr[len(arr) // 2]
                            <|fim_suffix|>
                            middle = [x for x in arr if x == pivot]
                            right = [x for x in arr if x > pivot]
                            return quicksort(left) + middle + quicksort(right)<|fim_middle|>""",
        }
    ],
    "max_tokens": 128,
    "temperature": 0.1,
}
# Manually create the output example
output_example = {
    "model": "hoangpnhat/qwen2.5-coder-7b-awq",
    "choices": [
        {
            "message": {
                "role": "assistant",
                "content": "left = [x for x in arr if x < pivot]",
            },
            "index": 0,
            "finish_reason": "stop",
        }
    ],
    "usage": {"prompt_tokens": 75, "completion_tokens": 13, "total_tokens": 88},
    "object": "chat.completion",
    "id": "msg_06388c2930744c40a2ab",
    "created": 1750407569,
}

# Infer the signature
signature = infer_signature(input_example, output_example)

# Start a new MLflow run
with mlflow.start_run(run_name=model_name + "_run_name") as run:
    mlflow.set_tag("ai_project_id", "coding")
    model_info = mlflow.pyfunc.log_model(
        python_model=model_path,
        registered_model_name=full_model_name,
        artifact_path=artifact_path,
        input_example=input_example,
        signature=signature,
        pip_requirements=[
            "vllm>=0.3.0",
            "transformers>=4.36.0",
            "torch>=2.0.0",
            "accelerate>=0.20.0",
            "pandas>=1.5.0",
            "tiktoken>=0.5.0",
        ],
    )
print("Logging model successfully ...")

# COMMAND ----------

UC_MODEL_NAME = f"{catalog}.{schema}.{model_name}"


# COMMAND ----------

# Set up for registering model
print("Registering model ...")

uc_registered_model_info = mlflow.register_model(
    model_uri=model_info.model_uri, name=UC_MODEL_NAME
)


# COMMAND ----------

scale_to_zero_enabled = True if ENVIRONMENT in ("dev", "uat") else False

ENDPOINT_NAME = UC_MODEL_NAME.split(".")[-1]

workload_size = "Medium"
if ENVIRONMENT == "dev":
    workload_size = "Medium"
    print("Using Medium workload size for dev environment")
else:
    workload_size = "Medium"


served_entities = [
    ServedEntityInput(
        name=ENDPOINT_NAME,
        entity_name=uc_registered_model_info.name,
        entity_version=uc_registered_model_info.version,
        workload_type="GPU_LARGE",
        workload_size=workload_size,
        scale_to_zero_enabled=scale_to_zero_enabled,
    )
]

# COMMAND ----------

w = WorkspaceClient()

# COMMAND ----------

traffic_config = TrafficConfig(
    [Route(served_model_name=ENDPOINT_NAME, traffic_percentage=100)]
)

endpoint_config = EndpointCoreConfigInput(
    served_entities=served_entities,
    traffic_config=traffic_config,
)

try:
    endpoint = w.serving_endpoints.get(ENDPOINT_NAME)
    print(f"Endpoint exists. Start updating endpoint '{ENDPOINT_NAME}'...")
    print(
        f"\n\nView deployment status: https://adb-379144824042062.2.azuredatabricks.net/ml/endpoints/{ENDPOINT_NAME}"
    )
    w.serving_endpoints.update_config_and_wait(
        name=ENDPOINT_NAME,
        served_entities=served_entities,
        traffic_config=traffic_config,
        timeout=timedelta(minutes=60),
    )
    try:
        w.serving_endpoints.patch(
            name=ENDPOINT_NAME,
            add_tags=[EndpointTag(key="ai_project_id", value="coding")],
        )
    except AttributeError as a:
        print(f"WARNING: Successfully update tags but warning, error: {a}")
    print(f"Update endpoint '{ENDPOINT_NAME}' successfully")
except ResourceDoesNotExist:
    print(f"Endpoint does not exist. Start creating new endpoint '{ENDPOINT_NAME}'...")
    print(f"\n\nView deployment status: ml/endpoints/{ENDPOINT_NAME}")
    w.serving_endpoints.create_and_wait(
        name=ENDPOINT_NAME,
        config=endpoint_config,
        # budget_policy_id="d01697ef-9452-4a51-987e-9faa7a28b382",
        tags=[EndpointTag(key="ai_project_id", value="coding")],
        timeout=timedelta(minutes=60),
    )
    print(f"Create endpoint '{ENDPOINT_NAME}' successfully")

# COMMAND ----------

try:
    # Get the endpoint
    endpoint = w.serving_endpoints.get(name=ENDPOINT_NAME)

    # Access the serving_endpoint_id
    serving_endpoint_id = endpoint.id

    # Set permissions
    w.serving_endpoints.set_permissions(
        serving_endpoint_id=serving_endpoint_id,
        access_control_list=[
            ServingEndpointAccessControlRequest(
                user_name="hoang.pham.uh@renesas.com",
                permission_level=ServingEndpointPermissionLevel.CAN_MANAGE,
            ),
            ServingEndpointAccessControlRequest(
                user_name="kiet.tran.xm@renesas.com",
                permission_level=ServingEndpointPermissionLevel.CAN_MANAGE,
            ),
        ],
    )
except Exception as e:
    print(f"Error setting permissions: {e}")

mlflow.end_run()

print("Deployment completed")
