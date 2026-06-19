# Databricks notebook source
# MAGIC %md
# MAGIC ## Register Qwen
# MAGIC Takes the modified Qwen model from previous step and registers it in Unity Catalog to prepare for serving.

# COMMAND ----------

# MAGIC %pip install -U mlflow transformers==4.51.0 tensorflow torch torchvision
# MAGIC dbutils.library.restartPython()

# COMMAND ----------

# COMMAND ----------

import json
import os

import mlflow
from databricks.sdk.runtime import dbutils

print(f"✅ MLflow version: {mlflow.__version__}")

# Path to Qwen model converted to Llama architecture in Unity Catalog Volumes
dbutils.widgets.text("model_path", "/Volumes/ai_uat_02/ai_uat_coding_logs/qwencoder_models/Qwen/Qwen2.5-Coder-7B-Llama")

# Where to register the model in Unity Catalog
dbutils.widgets.text("env", "dev", "env")


# The name of the model in Unity Catalog. If unspecified the name will be generated from the
# input path automatically.
dbutils.widgets.text("model_name", "qwencoder25-7b")

# COMMAND ----------

model_path = dbutils.widgets.get("model_path")
model_name = dbutils.widgets.get("model_name")
ENVIRONMENT = dbutils.widgets.get("env")

assert model_path, "model_path is required"

if not model_name:
    model_name = model_path.rstrip("/").split("/")[-1]
    model_name = model_name.replace(".", "").lower()

# Sanitize model_name for Unity Catalog
# UC model names cannot contain: . (dots) - only underscores, hyphens, and alphanumeric
# Replace dots with underscores
uc_model_name = model_name.replace(".", "_")

# Sanitize for artifact_path (cannot contain: / . % :)
artifact_path = model_name.replace(".", "_").replace("/", "_").replace("%", "_").replace(":", "_")

print(f"Model path: {model_path}")
print(f"Original model name: {model_name}")
print(f"UC model name: {uc_model_name}")
print(f"Artifact path: {artifact_path}")

# COMMAND ----------

# Modify model.safetensors.index.json to increase total model size so that model can be correctly loaded to FMAPI. 
# Note that in order to run this script the model size needs to be in model_path.
local_model_path_upper = model_path.upper()
model_size_override = None
if "1.5B" in local_model_path_upper or "3B" in local_model_path_upper:
    size_param = "3b"
elif "7B" in local_model_path_upper:
    size_param = "7b"
    model_size_override = 17378917376
elif "14B" in local_model_path_upper:
    size_param = "14b"
    model_size_override = 71_000_000_000 * 2
elif "32B" in local_model_path_upper:
    size_param = "32b"
    model_size_override = 71_000_000_000 * 2
else:
    raise RuntimeError("Unsupported Qwen model")
if model_size_override:
    with open(os.path.join(model_path, "model.safetensors.index.json"), "r") as f:
        safetensors_index = json.loads(f.read())
    safetensors_index["metadata"]["total_size"] = model_size_override
    print(f"Update safetensors index to use {model_size_override} parameters")
    with open(os.path.join(model_path, "model.safetensors.index.json"), "w") as f:
        f.write(json.dumps(safetensors_index, indent=2))

# COMMAND ----------

# Modify the tokenizer files and model config in the source directory

# Load the tokenizer config
with open(os.path.join(model_path, "tokenizer_config.json")) as f:
    tokenizer_config_obj = json.loads(f.read())

# Remove Qwen’s chat template since it is not recognized by model serving.
if "chat_template" in tokenizer_config_obj:
    del tokenizer_config_obj["chat_template"]

# Update the tokenizer class so that Databricks sees it as "PreTrainedTokenizerFast",
# since Qwen's tokenizer is not recognized, and it derives from PreTrainedTokenizerFast.  
# This also avoids saving additional files during registration that model
# serving would not expect.
tokenizer_config_obj["tokenizer_class"] = "PreTrainedTokenizerFast"

# Model serving expects model_input_names to be specified for Llama models.
tokenizer_config_obj["model_input_names"] = ["input_ids", "attention_mask"]

# Write the updated configs back
with open(os.path.join(model_path, "tokenizer_config.json"), "w") as f:
    f.write(json.dumps(tokenizer_config_obj, indent=2))

config_path = os.path.join(model_path, "config.json")
with open(config_path) as f:
    config_obj = json.loads(f.read())

with open(config_path, "w") as f:
    f.write(json.dumps(config_obj, indent=2))

# don't need the slow tokenizer files
files_to_delete = ["merges.txt", "vocab.json"]
for file_to_delete in files_to_delete:
    if os.path.exists(os.path.join(model_path, file_to_delete)):
        os.remove(os.path.join(model_path, file_to_delete))

# COMMAND ----------

# Register the model as a Llama model

# We'll create metadata that references Llama so Databricks sees "LlamaForCausalLM".
# Some of the specific versions/sizes referenced here DO NOT MATTER for our purposes.
# The important thing is that model serving considers this a Llama model.
# Note that we register this as a COMPLETION model and removed the chat template. Chat formatting will need to be performed on the client side.
task = "llm/v1/completions"
metadata = {
    "task": task,
    "curation_version": 1,
    "databricks_model_family": "LlamaForCausalLM (llama-3.1)",
    "databricks_model_size_parameters": "8b",
    "databricks_model_source": "genai-fine-tuning",
    "source": "huggingface",
    "source_model_name": "meta-llama/Llama-3.1-8B-Instruct",
    "source_model_revision": "0cb88a4f764b7a12671c53f0838cd831a0843b95",
}

input_example = {"prompt": "def print_hello_world():", "max_tokens": 20, "temperature": 0.05, "stop": ["\n\n"]}

# COMMAND ----------

# Use mlflow.transformers.log_model to register it to Unity Catalog
catalog = f"ai_uat_02"
schema = f"ai_uat_coding_logs"

assert catalog, "catalog is required"
assert schema, "schema is required"
assert uc_model_name, "uc_model_name is required"

# Use sanitized UC model name (without dots)
registered_model_name = ".".join([catalog, schema, uc_model_name])

print(f"📝 Model Registration Configuration:")
print(f"   Model path: {model_path}")
print(f"   Artifact path: {artifact_path}")
print(f"   Registered name: {registered_model_name}")
print(f"   Catalog: {catalog}")
print(f"   Schema: {schema}")
print(f"   Task: {task}")
print(f"\n🚀 Starting model registration to Unity Catalog...")
print(f"   This may take 10-30 minutes depending on model size...")

mlflow.set_registry_uri("databricks-uc")

try:
    with mlflow.start_run() as run:
        print(f"   MLflow Run ID: {run.info.run_id}")
        
        model_info = mlflow.transformers.log_model(
            transformers_model=model_path,
            artifact_path=artifact_path,
            registered_model_name=registered_model_name,
            input_example=input_example,
            metadata=metadata,
            task=task,
            torch_dtype="bfloat16",
        )
        
        print(f"\n✅ Model registered successfully!")
        print(f"   Model URI: {model_info.model_uri}")
        print(f"   Registered Model: {registered_model_name}")
        print(f"   MLflow Run: {run.info.run_id}")
        
except Exception as e:
    print(f"\n❌ Error during model registration: {e}")
    raise

# COMMAND ----------

# MAGIC %md
# MAGIC ## Summary
# MAGIC
# MAGIC Model has been registered in Unity Catalog at: `{registered_model_name}`
# MAGIC
# MAGIC Next steps:
# MAGIC 1. Navigate to Catalog Explorer → `{catalog}` → `{schema}` → Models → `{model_name}`
# MAGIC 2. Review the model details and metadata
# MAGIC 3. Create a Model Serving endpoint to deploy the model
# MAGIC 4. Test the model with inference requests

# COMMAND ----------

