# Databricks notebook source
# MAGIC %pip install -U -qqq databricks-vectorsearch
# MAGIC dbutils.library.restartPython()

# COMMAND ----------

# MAGIC %run ./00_config

# COMMAND ----------

dbutils.widgets.text("reset_table", "no", "reset_table")
dbutils.widgets.text("reset_vector_search_index", "yes", "reset_vector_search_index")
dbutils.widgets.text(
    "reset_vector_search_endpoint", "yes", "reset_vector_search_endpoint"
)
dbutils.widgets.text("reset_payload", "yes", "reset_payload")
dbutils.widgets.text("reset_model", "yes", "reset_model")

# COMMAND ----------

# DBTITLE 1,Delete tables created in 01_load_files
if dbutils.widgets.get("reset_table") == "yes":
    print("removing tables...")
    # delete table with raw documents
    spark.sql(
        f'DROP TABLE IF EXISTS {destination_tables_config.get("raw_files_table_name")}'
    )
    # delete table with parsed documents
    spark.sql(
        f'DROP TABLE IF EXISTS {destination_tables_config.get("chunked_docs_table_name")}'
    )
    # delete table with parsed documents
    spark.sql(
        f'DROP TABLE IF EXISTS {destination_tables_config.get("parsed_docs_table_name")}'
    )


# COMMAND ----------

if dbutils.widgets.get("reset_payload") == "yes":
    try:
        pay_load_table = RAG_APP_NAME + "_payload"
        print(f"Removing payload named {pay_load_table}...")
        # delete payload table
        spark.sql(f"DROP TABLE IF EXISTS {pay_load_table}")
    except:
        print(f"Can't reset payload named {pay_load_table}")

if dbutils.widgets.get("reset_model") == "yes":
    print(f"Removing registered model named {UC_MODEL_NAME}...")
    try:
        # delete payload table
        spark.sql(f"DROP TABLE IF EXISTS {UC_MODEL_NAME}")
    except:
        print(f"Can't reset registered model named {UC_MODEL_NAME}")


# COMMAND ----------

dbutils.widgets.get("reset_vector_search_index")

# COMMAND ----------

# DBTITLE 1,Delete vector search index
from databricks.vector_search.client import VectorSearchClient

# Delete Vector Search Index
if dbutils.widgets.get("reset_vector_search_index") == "yes":
    print(f"removing index: {destination_tables_config.get('vectorsearch_index_name')}")
    client = VectorSearchClient()
    try:
        client.delete_index(
            endpoint_name=VECTOR_SEARCH_ENDPOINT,
            index_name=destination_tables_config.get("vectorsearch_index_name"),
        )

    except Exception as e:
        if "RESOURCE_DOES_NOT_EXIST" in str(e):
            print("Vector Search index doesn't exist, skipping")
            pass  # Handle the case where the resource does not exist, if needed
        else:
            raise e

if dbutils.widgets.get("reset_vector_search_endpoint") == "yes":
    print(f"removing table: {VECTOR_SEARCH_ENDPOINT}")
    # Delete Vector Search Endpoint
    try:
        client.delete_endpoint(name=VECTOR_SEARCH_ENDPOINT)
    except Exception as e:
        if "RESOURCE_DOES_NOT_EXIST" in str(e):
            print("Vector Search endpoint doesn't exist, skipping")
            pass  # Handle the case where the resource does not exist, if needed
        else:
            raise e
