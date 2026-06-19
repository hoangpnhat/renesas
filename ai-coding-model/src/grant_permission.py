# Databricks notebook source
# MAGIC %pip install databricks-agents
# MAGIC dbutils.library.restartPython()

# COMMAND ----------

# MAGIC %run ./00_config

# COMMAND ----------

from databricks.sdk import WorkspaceClient
from databricks.sdk.service.iam import AccessControlRequest, PermissionLevel

dbutils.widgets.text(
    "access_control_name", "kiet.tran.xm@renesas.com", "access control name"
)
w = WorkspaceClient()

# COMMAND ----------

user_mail = dbutils.widgets.get("access_control_name")

# COMMAND ----------
endpoint_name = RAG_APP_NAME
try:
    endpoint = w.serving_endpoints.get(endpoint_name)
    endpoint_id = endpoint.id

    # Use the endpoint_id to update permissions
    w.permissions.update(
        request_object_type="serving-endpoints",
        request_object_id=endpoint_id,
        access_control_list=[
            AccessControlRequest(
                user_name=user_mail,
                permission_level=PermissionLevel.CAN_MANAGE,
            )
        ],
    )
    print(f"Sucessfully grant permission to user: {endpoint_id}")
except ResourceDoesNotExist:
    print(f"Endpoint does not exist")


# COMMAND ----------
vectorsearch_endpoint_name = VECTOR_SEARCH_ENDPOINT

print("vector search name: ", vectorsearch_endpoint_name)

endpoint = w.vector_search_endpoints.get_endpoint(vectorsearch_endpoint_name)
endpoint_id = endpoint.id

print("vector search endpoint id: ", endpoint_id)

# Use the endpoint_id to update permissions
w.permissions.update(
    request_object_type="serving-endpoints",
    request_object_id=endpoint_id,
    access_control_list=[
        AccessControlRequest(
            user_name=user_mail,
            permission_level=PermissionLevel.CAN_MANAGE,
        )
    ],
)
print(f"Sucessfully grant permission to user: {endpoint_id}")


# COMMAND ----------


def grant_table_permission(user_name, full_path_of_table):
    spark.sql(f"GRANT MANAGE ON TABLE {full_path_of_table} TO `{user_name}`")
    user_list = spark.sql(f"SHOW GRANTS ON TABLE {full_path_of_table}")
    print(user_list.show())


def grant_model_permission(user_name, full_path_of_model):
    spark.sql(f"GRANT MANAGE ON FUNCTION {full_path_of_model} TO `{user_name}`")
    user_list = spark.sql(f"SHOW GRANTS ON FUNCTION {full_path_of_model}")
    print(user_list.show())


# COMMAND ----------

grant_table_permission(
    user_mail, full_path_of_table=destination_tables_config["vectorsearch_index_name"]
)
grant_table_permission(
    user_mail, full_path_of_table=destination_tables_config["raw_files_table_name"]
)
grant_table_permission(
    user_mail, full_path_of_table=destination_tables_config["parsed_docs_table_name"]
)
grant_table_permission(
    user_mail, full_path_of_table=destination_tables_config["chunked_docs_table_name"]
)
grant_model_permission(user_mail, full_path_of_table=UC_MODEL_NAME)
