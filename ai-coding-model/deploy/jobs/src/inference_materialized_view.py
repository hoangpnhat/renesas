# Databricks notebook source
import requests
from databricks.sdk.runtime import dbutils, spark

dbutils.widgets.text("catalog_name", "ai_uat_02")
dbutils.widgets.text("schema_name", "ai_uat_coding_logs")
dbutils.widgets.text("dev_mode", "nonprod")
dbutils.widgets.text("environment", "uat")
dbutils.widgets.text("shared_warehouse_id", " 249321e809ad49be")

catalog_name = dbutils.widgets.get("catalog_name")
schema_name = dbutils.widgets.get("schema_name")
dev_mode = dbutils.widgets.get("dev_mode")
environment = dbutils.widgets.get("environment")
shared_warehouse_id = dbutils.widgets.get("shared_warehouse_id")

# COMMAND ----------

sql = f"""
SELECT table_name 
FROM system.information_schema.table_tags
WHERE
    tag_name = 'product'
AND tag_value = 'ai-gateway'
AND schema_name = '{schema_name}'
AND catalog_name = '{catalog_name}'
"""
tables_df = spark.sql(sql)
display(tables_df)

# COMMAND ----------

import re
from pyspark.sql.functions import col

# Get the list of tables
table_names = [row.table_name for row in tables_df.collect()]

# Initialize the SQL statement
sql_statements = []

tabel_stmt_template = """
SELECT   request_date,
         databricks_request_id,
         client_request_id,
         request_time,
         status_code,
         sampling_fraction,
         execution_duration_ms,
         request,
         response,
         served_entity_id,
         logging_error_codes,
         requester,
         '{model}'   AS endpoint_name,
         '{feature}' AS feature
FROM {catalog_name}.{schema_name}.`{table}`
"""

# Iterate over each table
for table in table_names:
    # Determine endpoint_name and feature based on table name using regex
    mapping = {
        f"ai-{dev_mode}-coding-ais-([a-z0-9-]+)-(chat|autocompletion)_payload": (
            lambda m: m.group(0).replace("_payload", ""), lambda m: m.group(2)),
        f"ai-{environment}-coding-([a-z0-9-]+)-(chat|autocompletion)_payload": (
            lambda m: m.group(0).replace("_payload", ""), lambda m: m.group(2)),
        f"databricks-([a-z0-9-]+)_payload": (lambda m: m.group(0).replace("_payload", ""), lambda m: "chat")
    }

    for pattern, (model_func, feature_func) in mapping.items():
        match = re.match(pattern, table)
        if match:
            # Check if the table has rows
            table_df = spark.sql(f"SELECT COUNT(*) FROM {catalog_name}.{schema_name}.`{table}`")
            if table_df.collect()[0][0] > 0:
                model = model_func(match)
                feature = feature_func(match)
                sql_statements.append(tabel_stmt_template.format(
                    catalog_name=catalog_name,
                    schema_name=schema_name,
                    table=table,
                    model=model,
                    feature=feature
                ))

# Combine all SQL statements with UNION ALL
final_sql = " UNION ALL ".join(sql_statements)

# Create the streaming view
create_mv_sql = f"""
CREATE OR REPLACE MATERIALIZED VIEW {catalog_name}.{schema_name}.ai_{environment}_coding_aggregated_payloads
(
    request_date DATE, databricks_request_id STRING, client_request_id STRING, request_time TIMESTAMP, status_code INT, sampling_fraction DOUBLE, execution_duration_ms BIGINT, request STRING, response STRING, served_entity_id STRING, logging_error_codes ARRAY<STRING>, requester STRING, endpoint_name STRING, feature STRING
)
SCHEDULE EVERY 2 HOURS AS
{final_sql}
"""
print(create_mv_sql)

# COMMAND ----------
# Execute the SQL to create the streaming view
DATABRICKS_TOKEN = dbutils.notebook.entry_point.getDbutils().notebook().getContext().apiToken().get()
workspace_url = spark.conf.get("spark.databricks.workspaceUrl")
headers = {
    "Authorization": f"Bearer {DATABRICKS_TOKEN}"
}
payload = {
    "warehouse_id": shared_warehouse_id,
    "statement": create_mv_sql
}

response = requests.post(
    f"https://{workspace_url}/api/2.0/sql/statements",
    json=payload,
    headers=headers
)

# Print response
response.raise_for_status()
print(response.status_code)
print(response.json())
