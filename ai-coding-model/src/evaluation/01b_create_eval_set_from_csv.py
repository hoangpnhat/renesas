# Databricks notebook source
# MAGIC %pip install -U -qqqq databricks-agents mlflow mlflow-skinny databricks-sdk
# MAGIC dbutils.library.restartPython()

# COMMAND ----------

# MAGIC %run ./00_eval_config

# COMMAND ----------

import pandas as pd

import mlflow

# COMMAND ----------


def read_data_rename_cols(doc_uri: str) -> pd.DataFrame:
    eval_pd = pd.read_csv(doc_uri)
    eval_pd.columns = ["request", "expected_response"]
    eval_pd.head()
    return eval_pd


def create_mosaic_eval_schema(eval_pd: pd.DataFrame) -> pd.DataFrame:
    import uuid

    eval_pd["request_id"] = eval_pd.apply(lambda x: str(uuid.uuid4()), axis=1)
    eval_pd.head()
    return eval_pd


# COMMAND ----------


def _create_update_eval_table(eval_set_location, eval_table, schema):
    print("create eval set from csv ...")
    eval_set = read_data_rename_cols(eval_set_location)
    eval_set = create_mosaic_eval_schema(eval_set)
    print("dropping existing table ...")
    spark.sql(f"DROP TABLE IF EXISTS {eval_table}")
    print("creating new table ...")
    spark.createDataFrame(eval_set, schema=schema).write.mode("overwrite").saveAsTable(
        eval_table
    )
    print("Successfully create new table:")
    spark.table(f"{eval_table}").display()


# COMMAND ----------

_create_update_eval_table(eval_set_path, eval_table, schema)
