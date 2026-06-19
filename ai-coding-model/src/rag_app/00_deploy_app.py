# Databricks notebook source
# MAGIC %pip install -q --upgrade pip
# MAGIC %pip install -U -qqqq databricks-agents mlflow mlflow-skinny databricks-vectorsearch langchain==0.2.11 langchain_core==0.2.23 langchain_community==0.2.10
# MAGIC %pip install -q ydata-profiling==4.11.0
# MAGIC %pip install -q llama-index-core==0.12.2
# MAGIC %pip install -q llama_index==0.12.2
# MAGIC %pip install -q llama-index-llms-openai==0.3.2
# MAGIC %pip install -q llama-index-graph-stores-neo4j
# MAGIC %pip install -q llama-index-embeddings-databricks
# MAGIC %pip install -q neo4j
# MAGIC %pip install -q python-dotenv
# MAGIC %pip install -q llama-index-llms-databricks==0.2.0
# MAGIC %pip install -q python-snappy==0.7.1
# MAGIC dbutils.library.restartPython()

# COMMAND ----------

# MAGIC %md
# MAGIC te: you may need to restart the kernel using %restart_python or dbutils.library.restartPython() to use updated packages.
# MAGIC Hello

# COMMAND ----------

dbutils.library.restartPython()

# COMMAND ----------

import os
import mlflow
from mlflow.models import infer_signature
import time
from datetime import timedelta


# COMMAND ----------

# MAGIC %run ../00_config

# COMMAND ----------

# MAGIC %md ## Log the chain to MLflow & test the RAG chain locally
# MAGIC
# MAGIC This will save the chain using MLflow's code-based logging and invoke it locally to test it.
# MAGIC
# MAGIC **MLflow Tracing** allows you to inspect what happens inside the chain.  This same tracing data will be logged from your deployed chain along with feedback that your stakeholders provide to a Delta Table.
# MAGIC
# MAGIC `# TODO: link docs for code-based logging`

# COMMAND ----------

import sys
from utils.parent_retriver import ParentVectorContextRetriever
from utils.custom_neo4j_graph import CustomNeo4jPropertyGraphStore

# COMMAND ----------

signature = infer_signature(
    rag_chain_config["input_example"], rag_chain_config["output_example"]
)

# COMMAND ----------

# Log the model to MLflow
# TODO: remove example_no_conversion once this papercut is fixed
with mlflow.start_run(run_name=CHAIN_RUN_NAME):
    # Tag to differentiate from the data pipeline runs
    mlflow.set_tag("type", "chain")
    mlflow.set_tag("ai_project_id", "coding")
    logged_chain_info = mlflow.langchain.log_model(
        lc_model=os.path.join(
            os.getcwd(), CHAIN_CODE_FILE
        ),  # Chain code file e.g., /path/to/the/chain.py
        model_config=rag_chain_config,  # Chain configuration set in 00_config
        artifact_path="model",  # Required by MLflow
        input_example=rag_chain_config[
            "input_example"
        ],  # Save the chain's input schema.  MLflow will execute the chain before logging & capture it's output schema.
        example_no_conversion=True,  # Required by MLflow to use the input_example as the chain's schema
        pip_requirements=[
            "python-snappy==0.7.1",
            "databricks-agents==0.10.0",
            "mlflow==2.17.2",
            "mlflow-skinny==2.17.2",
            "databricks-vectorsearch==0.43",
            "langchain==0.2.11",
            "langchain_core==0.2.23",
            "langchain_community==0.2.10",
            "llama_index==0.11.22",
            "llama-index-core==0.11.22",
            "llama-index-llms-openai==0.2.16",
            "llama-index-graph-stores-neo4j==0.3.5",
            "llama-index-embeddings-databricks==0.2.1",
            "python-dotenv==1.0.1",
            "llama-index-llms-databricks==0.2.0",
        ],
        code_paths=["utils"],
        signature=signature,
    )

    #  code_paths =[os.path.join(os.getcwd(),"utils","custom_neo4j_graph.py"), os.path.join(os.getcwd(),"utils","parent_retriver.py")]

    # Attach the data pipeline's configuration as parameters
    mlflow.log_params(_flatten_nested_params({"data_pipeline": data_pipeline_config}))

    # Attach the data pipeline configuration
    mlflow.log_dict(data_pipeline_config, "data_pipeline_config.json")


# COMMAND ----------

# MAGIC %md
# MAGIC ## Test the chain locally

# COMMAND ----------

logged_chain_info.model_uri

# COMMAND ----------

# a = logged_chain_info.model_uri
chain = mlflow.langchain.load_model(logged_chain_info.model_uri)

# COMMAND ----------

chain_input = {
    "messages": [
        {
            "role": "user",
            "content": "Hello?",  # Replace with a question relevant to your use case
        }
    ],
    "retrieval_place": ["document", "code_base"],
}
# Can you explain function CPvsVol
chain.invoke(chain_input)


# COMMAND ----------

# MAGIC %md
# MAGIC ## Deploy to App
# MAGIC

# COMMAND ----------

from databricks import agents
from databricks.sdk import WorkspaceClient
from databricks.sdk.service.serving import (
    EndpointStateReady,
    EndpointStateConfigUpdate,
    EndpointTag,
)
from databricks.sdk.errors import NotFound, ResourceDoesNotExist
import time

w = WorkspaceClient()

# COMMAND ----------

# Use Unity Catalog to log the chain
mlflow.set_registry_uri("databricks-uc")

# Register the chain to UC
uc_registered_model_info = mlflow.register_model(
    model_uri=logged_chain_info.model_uri,
    name=UC_MODEL_NAME,
    tags={"ai_project_id": "coding", "ShutdownPolicy": "Never"},
)

# COMMAND ----------

from databricks.sdk.service.serving import (
    EndpointCoreConfigInput,
    AutoCaptureConfigInput,
    ServedEntityInput,
    TrafficConfig,
    Route,
    EndpointTag,
)

# Set up for deploying model
DATABRICKS_HOST = f"https://{spark.conf.get('spark.databricks.workspaceUrl')}"

client = mlflow.deployments.get_deploy_client("databricks")

storage_catalog = UC_CATALOG
storage_schema = UC_SCHEMA_GOLD

entity_name = "coding_assistant_entity"
endpoint_name = RAG_APP_NAME
workload_size = "Small"
scale_to_zero_enabled = True

print(f"Storage_catalog: {storage_catalog}")
print(f"Storage_schema: {storage_schema}")
print(f"Entity_name: {entity_name}")
print(f"Endpoint_name: {endpoint_name}")
print(f"Workload_size: {workload_size}")

served_entities = [
    ServedEntityInput(
        name=entity_name,
        entity_name=uc_registered_model_info.name,
        entity_version=uc_registered_model_info.version,
        workload_size=workload_size,
        scale_to_zero_enabled=scale_to_zero_enabled,
        environment_vars={
            "DATABRICKS_TOKEN": os.getenv("DATABRICKS_TOKEN"),
            "NEO4J_USERNAME": os.getenv("NEO4J_USERNAME"),
            "NEO4J_PASSWORD": os.getenv("NEO4J_PASSWORD"),
            "NEO4J_URI": os.getenv("NEO4J_URI"),
            "DATABRICKS_SERVING_ENDPOINT": os.getenv("DATABRICKS_SERVING_ENDPOINT"),
            "DB_MODEL_SERVING_HOST_URL": DATABRICKS_HOST,
            "ENABLE_LANGCHAIN_STREAMING": True,
            "ENABLE_MLFLOW_TRACING": True,
            "RETURN_REQUEST_ID_IN_RESPONSE": True,
        },
    )
]

auto_capture_config = AutoCaptureConfigInput(
    catalog_name=storage_catalog,
    schema_name=storage_schema,
    enabled=True,
    table_name_prefix=RAG_APP_NAME_UNDERSCORE,
)

traffic_config = TrafficConfig(
    [Route(served_model_name=entity_name, traffic_percentage=100)]
)

endpoint_config = EndpointCoreConfigInput(
    auto_capture_config=auto_capture_config,
    served_entities=served_entities,
    traffic_config=traffic_config,
)

try:
    endpoint = w.serving_endpoints.get(endpoint_name)
    print(f"Endpoint exists. Start updating endpoint '{endpoint_name}'...")
    print(f"\n\nView deployment status: {DATABRICKS_HOST}/ml/endpoints/{endpoint_name}")
    w.serving_endpoints.update_config_and_wait(
        name=endpoint_name,
        auto_capture_config=auto_capture_config,
        served_entities=served_entities,
        traffic_config=traffic_config,
        timeout=timedelta(minutes=30),
    )
    try:
        w.serving_endpoints.patch(
            name=endpoint_name,
            add_tags=[
                EndpointTag(key="ai_project_id", value="coding"),
                EndpointTag(key="ShutdownPolicy", value="Never"),
            ],
        )
    except AttributeError as a:
        print("WARNING: Successfully update tags but warning")
    print(f"Update endpoint '{endpoint_name}' successfully")
except ResourceDoesNotExist:
    print(f"Endpoint does not exist. Start creating new endpoint '{endpoint_name}'...")
    print(f"\n\nView deployment status: {DATABRICKS_HOST}/ml/endpoints/{endpoint_name}")
    w.serving_endpoints.create_and_wait(
        name=endpoint_name,
        config=endpoint_config,
        tags=[
            EndpointTag(key="ai_project_id", value="coding"),
            EndpointTag(key="ShutdownPolicy", value="Never"),
        ],
        timeout=timedelta(minutes=30),
    )
    print(f"Create endpoint '{endpoint_name}' successfully")


# COMMAND ----------

from databricks.sdk.service.iam import AccessControlRequest, PermissionLevel

try:
    endpoint = w.serving_endpoints.get(endpoint_name)
    endpoint_id = endpoint.id

    # Use the endpoint_id to update permissions
    w.permissions.update(
        request_object_type="serving-endpoints",
        request_object_id=endpoint_id,
        access_control_list=[
            AccessControlRequest(
                user_name="kiet.tran.xm@renesas.com",
                permission_level=PermissionLevel.CAN_MANAGE,
            )
        ],
    )
    print(f"Sucessfully grant permission to user: {endpoint_id}")
except ResourceDoesNotExist:
    print(f"Endpoint does not exist")
