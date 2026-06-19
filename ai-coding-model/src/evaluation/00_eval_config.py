# Databricks notebook source
# MAGIC %md
# MAGIC # Eval configuration
# MAGIC
# MAGIC This notebook stores the evaluation-specific configuration for your agent application, like location of your evaluation datasets.

# COMMAND ----------

# MAGIC %run ../00_config

# COMMAND ----------

from pyspark.sql.types import StructType, StructField, StringType, MapType

schema = StructType(
    [
        StructField("request", StringType(), True),
        StructField("expected_response", StringType(), True),
        StructField("request_id", StringType(), True),
    ]
)

# COMMAND ----------

eval_set_path = "/Volumes/ai_dev/ai_dev_coding_bronze/versa_source/evaluation/Documents_evaluation.csv"
eval_table = f"{UC_CATALOG}.{UC_SCHEMA_GOLD}.document_evaluation_set"
experiment_run_id = "e84ea7bf43394f9dbc784974159c2ff7"


print(f"eval set location: {eval_set_path}")
print(f"eval table: {eval_table}")
print(f"experiment run id (model path): {experiment_run_id}")
