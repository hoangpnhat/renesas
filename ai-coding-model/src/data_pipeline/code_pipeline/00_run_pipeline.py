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

# MAGIC %run ../../00_config

# COMMAND ----------

import os

os.environ["CODE_ELEMENTS_PATH"] = f"{SOURCE_CODE_PATH}/output/code_elements.json"
os.makedirs(os.path.join(SOURCE_CODE_PATH, "output"), exist_ok=True)
graph_embedding_model_name = rag_chain_config["databricks_resources"][
    "graph_embedding_name"
]

# COMMAND ----------

from process_code_data import extract_and_chunk_code
import embed

# COMMAND ----------

# Read source code, extract information and chunk it
extract_and_chunk_code(SOURCE_CODE_PATH, os.getenv("CODE_ELEMENTS_PATH"))

# COMMAND ----------

# Build graph database by Neo4j
embeddings = embed.main(graph_embedding_model_name)
