# Databricks notebook source
# MAGIC %pip install -q llama_index
# MAGIC %pip install -q llama-index-core
# MAGIC %pip install -q llama-index-llms-openai
# MAGIC %pip install -q llama-index-graph-stores-neo4j
# MAGIC %pip install -q llama-index-embeddings-openai
# MAGIC %pip install -q llama-index-embeddings-databricks
# MAGIC %pip install -q neo4j
# MAGIC %pip install -q python-dotenv
# MAGIC %pip install -q llama-index-llms-databricks==0.2.0

# COMMAND ----------

dbutils.library.restartPython()

# COMMAND ----------

import os

os.environ["DATABRICKS_TOKEN"] = dbutils.secrets.get(
    scope="ai-dev-coding-sc", key="databricks_token"
).strip()
os.environ["NEO4J_USERNAME"] = dbutils.secrets.get(
    scope="ai-dev-coding-sc", key="neo4j_username"
).strip()
os.environ["NEO4J_PASSWORD"] = dbutils.secrets.get(
    scope="ai-dev-coding-sc", key="neo4j_password"
).strip()
os.environ["NEO4J_URI"] = dbutils.secrets.get(scope="ai-dev-coding-sc", key="neo4j_uri")
os.environ["DATABRICKS_SERVING_ENDPOINT"] = (
    "https://adb-379144824042062.2.azuredatabricks.net/serving-endpoints"
)
os.environ["MODEL_EMBEDDING_NAME"] = "text-embedding-3-small"

# COMMAND ----------

import os
from llama_index.embeddings.databricks import DatabricksEmbedding
from llama_index.graph_stores.neo4j import Neo4jPropertyGraphStore
from llama_index.core import PropertyGraphIndex, Settings
from llama_index.core.schema import QueryBundle
from llama_index.llms.databricks import Databricks

from parent_retriver import ParentVectorContextRetriever
from custom_neo4j_graph import CustomNeo4jPropertyGraphStore

# Initialize the embedding model
embed_model = DatabricksEmbedding(model=os.getenv("MODEL_EMBEDDING_NAME"))

llm = Databricks(
    model="gpt4-o",
    api_key=os.environ["DATABRICKS_TOKEN"],
    api_base="https://adb-379144824042062.2.azuredatabricks.net/serving-endpoints",
)


username = os.getenv("NEO4J_USERNAME")
password = os.getenv("NEO4J_PASSWORD")
url = os.getenv("NEO4J_URI")
dim = 1536
graph_store = CustomNeo4jPropertyGraphStore(
    username=username, password=password, url=url
)

property_graph_index = PropertyGraphIndex.from_existing(
    property_graph_store=graph_store, embed_model=embed_model, llm=llm
)
graph_store.close()

# # # Assuming you have an embedding model set up
vector_retriever = ParentVectorContextRetriever(
    property_graph_index.property_graph_store,
    include_text=True,  # Include source chunk text with retrieved paths
    embed_model=embed_model,
)


# COMMAND ----------

query = QueryBundle(query_str="can you write for C")
retriever = vector_retriever.retrieve_from_graph(query)
print(retriever)
