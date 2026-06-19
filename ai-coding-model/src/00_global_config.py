# Databricks notebook source

# COMMAND ----------

try:
    dbutils.widgets.text("environment", "dev")
    dbutils.widgets.text(
        "dbx_token",
        dbutils.secrets.get(scope="ai-dev-coding-sc", key="databricks_token"),
    )
except:
    print("""Warning: can't set default values for "environment" or "dbx_token""")

import mlflow
import os

# COMMAND ----------

# MAGIC %md
# MAGIC ## Application configuration
# MAGIC
# MAGIC To begin with, we simply need to configure the following:
# MAGIC 1. `RAG_APP_NAME`: The name of the RAG application.  Used to name the app's Unity Catalog model and is prepended to the output Delta Tables + Vector Indexes
# MAGIC 2. `UC_CATALOG` & `UC_SCHEMA`: [Create a Unity Catalog](https://docs.databricks.com/en/data-governance/unity-catalog/create-catalogs.html#create-a-catalog) and a Schema where the output Delta Tables with the parsed/chunked documents and Vector Search indexes are stored
# MAGIC 3. `UC_MODEL_NAME`: Unity Catalog location to log and store the chain's model
# MAGIC 4. `VECTOR_SEARCH_ENDPOINT`: [Create a Vector Search Endpoint](https://docs.databricks.com/en/generative-ai/create-query-vector-search.html#create-a-vector-search-endpoint) to host the resulting vector index
# MAGIC 5. `SOURCE_PATH`: A [UC Volume](https://docs.databricks.com/en/connect/unity-catalog/volumes.html#create-and-work-with-volumes) that contains the source documents for your application.
# MAGIC 6. `MLFLOW_EXPERIMENT_NAME`: MLflow Experiment to track all experiments for this application.  Using the same experiment allows you to track runs across Notebooks and have unified lineage and governance for your application.
# MAGIC 7. `EVALUATION_SET_FQN`: Delta Table where your evaluation set will be stored.  In the POC, we will seed the evaluation set with feedback you collect from your stakeholders.
# MAGIC
# MAGIC After finalizing your configuration, optionally run `01_validate_config` to check that all locations exist.

# COMMAND ----------

# By default, will use the current user name to create a unique UC catalog/schema & vector search endpoint
user_email = spark.sql("SELECT current_user() as username").collect()[0].username
user_name = user_email.split("@")[0].replace(".", "").lower()[:35]

# COMMAND ----------

ENVIRONMENT = dbutils.widgets.get("environment")

# The name of the RAG application.  This is used to name the chain's UC model and prepended to the output Delta Tables + Vector Indexes
RAG_APP_NAME = f"ai-{ENVIRONMENT}-coding-c"
RAG_APP_NAME_UNDERSCORE = RAG_APP_NAME.replace("-", "_")

# UC Catalog & Schema where outputs tables/indexs are saved
# If this catalog/schema does not exist, you need create catalog/schema permissions.
UC_CATALOG = f"ai_{ENVIRONMENT}"
UC_SCHEMA = f"ai_{ENVIRONMENT}_coding"
UC_SCHEMA_BRONZE = f"{UC_SCHEMA}_bronze"
UC_SCHEMA_SILVER = f"{UC_SCHEMA}_silver"
UC_SCHEMA_GOLD = f"{UC_SCHEMA}_gold"

# UC_SCHEMA_OF_MODEL = f'ai_dev_common_models'

## UC Model name where the POC chain is logged
UC_MODEL_NAME = f"{UC_CATALOG}.{UC_SCHEMA_GOLD}.{RAG_APP_NAME_UNDERSCORE}"

# Vector Search endpoint where index is loaded
# If this does not exist, it will be created

VECTOR_SEARCH_ENDPOINT = f"ai-{ENVIRONMENT}-coding-vsc"

# Source location for documents
# You need to create this location and add files
SOURCE_PATH = f"/Volumes/{UC_CATALOG}/{UC_SCHEMA_BRONZE}/versa_source/documents"
SOURCE_CODE_PATH = f"/Volumes/{UC_CATALOG}/{UC_SCHEMA_BRONZE}/versa_source/code"

############################
##### We suggest accepting these defaults unless you need to change them. ######
############################

EVALUATION_SET_FQN = (
    f"`{UC_CATALOG}`.`{UC_SCHEMA_GOLD}`.{RAG_APP_NAME_UNDERSCORE}_evaluation_set"
)

# MLflow experiment name
# Using the same MLflow experiment for a single app allows you to compare runs across Notebooks
MLFLOW_EXPERIMENT_NAME = (
    f"/Workspace/Users/{user_email}/{RAG_APP_NAME_UNDERSCORE}_experiment"
)

# Check if the experiment already exists
try:
    mlflow.set_experiment(MLFLOW_EXPERIMENT_NAME)
except:
    print("[Warning] : can not create or experiment is already created")


# MLflow Run Names
# These Runs will store your initial POC application.  They are later used to evaluate the POC model against your experiments to improve quality.

# Data pipeline MLflow run name
POC_DATA_PIPELINE_RUN_NAME = "data_pipeline_poc"
# Chain MLflow run name
CHAIN_RUN_NAME = "poc"

# COMMAND ----------

print(f"RAG_APP_NAME {RAG_APP_NAME}")
print(f"UC_CATALOG {UC_CATALOG}")
print(f"UC_SCHEMA_GOLD {UC_SCHEMA_GOLD}")
print(f"UC_SCHEMA_SILVER {UC_SCHEMA_SILVER}")
print(f"UC_SCHEMA_BRONZE {UC_SCHEMA_BRONZE}")
print(f"UC_MODEL_NAME {UC_MODEL_NAME}")
print(f"VECTOR_SEARCH_ENDPOINT {VECTOR_SEARCH_ENDPOINT}")
print(f"SOURCE_PATH {SOURCE_PATH}")
print(f"EVALUATION_SET_FQN {EVALUATION_SET_FQN}")
print(f"MLFLOW_EXPERIMENT_NAME {MLFLOW_EXPERIMENT_NAME}")
print(f"POC_DATA_PIPELINE_RUN_NAME {POC_DATA_PIPELINE_RUN_NAME}")
print(f"CHAIN_RUN_NAME {CHAIN_RUN_NAME}")


# COMMAND ----------


os.environ["DATABRICKS_TOKEN"] = dbutils.widgets.get("dbx_token")

os.environ["NEO4J_USERNAME"] = dbutils.secrets.get(
    scope=f"ai-{ENVIRONMENT}-coding-ais-sc", key=f"neo4j-ai-{ENVIRONMENT}-coding-usr"
).strip()
os.environ["NEO4J_PASSWORD"] = dbutils.secrets.get(
    scope=f"ai-{ENVIRONMENT}-coding-ais-sc", key=f"neo4j-ai-{ENVIRONMENT}-coding-pwd"
).strip()
os.environ["NEO4J_URI"] = dbutils.secrets.get(
    scope=f"ai-{ENVIRONMENT}-coding-ais-sc", key=f"neo4j-ai-{ENVIRONMENT}-coding-uri"
)
os.environ["DATABRICKS_SERVING_ENDPOINT"] = (
    f"https://{spark.conf.get('spark.databricks.workspaceUrl')}/serving-endpoints"
)

os.environ["ADI_ENDPOINT"] = dbutils.secrets.get(
    scope=f"ai-{ENVIRONMENT}-coding-ais-sc", key=f"di-ai-{ENVIRONMENT}-coding-endpoint"
)
os.environ["ADI_KEY"] = dbutils.secrets.get(
    scope=f"ai-{ENVIRONMENT}-coding-ais-sc", key=f"di-ai-{ENVIRONMENT}-coding-key"
)
