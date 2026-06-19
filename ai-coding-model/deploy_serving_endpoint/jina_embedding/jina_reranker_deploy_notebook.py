# Databricks notebook source
# MAGIC %md
# MAGIC # Deploy Jina Reranker m0 with Transformers
# MAGIC
# MAGIC This notebook deploys Jina Reranker m0 model - a multilingual multimodal document reranker for ranking visual and textual documents

# COMMAND ----------

# MAGIC %md
# MAGIC ## 1. Install Dependencies

# COMMAND ----------

# Install required packages
# Note: flash-attn is optional but only works on Ampere+ GPUs (A100, H100). T4 uses eager attention.
# Note: transformers>=4.45.0 required for Qwen2VL model compatibility
# MAGIC %pip install -qqqq mlflow==2.19.0 mlflow-skinny==2.19.0 databricks-sql-connector==4.0.3
# MAGIC %pip install "transformers>=4.45.0,<4.54.0" torch torchvision torchaudio accelerate==1.8.1 pandas Pillow requests databricks-sdk==0.58.0 einops qwen-vl-utils
# MAGIC dbutils.library.restartPython()

# COMMAND ----------

# MAGIC %md
# MAGIC ## 2. Import Libraries

# COMMAND ----------

import os
import json
import time
import uuid
import mlflow
import pandas as pd
from typing import List, Dict, Any, Optional
import logging
from datetime import timedelta
from databricks.sdk import WorkspaceClient
from databricks.sdk.runtime import dbutils
from databricks.sdk.service.serving import (
    EndpointCoreConfigInput,
    AutoCaptureConfigInput,
    ServedEntityInput,
    TrafficConfig,
    Route,
    EndpointTag,
    ServingEndpointAccessControlRequest,
    ServingEndpointPermissionLevel,
)
from databricks.sdk.errors import ResourceDoesNotExist

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# COMMAND ----------

# MAGIC %md
# MAGIC ## 3. Configuration

# COMMAND ----------

# Model configuration
MODEL_NAME = "jinaai/jina-reranker-m0" 
REGISTERED_MODEL_NAME = "jina-reranker-m0"

# Environment setup
ENVIRONMENT = "uat"  # Options: dev, uat, prod
USERNAME = "hoang.pham.uh@renesas.com"

# Unity Catalog configuration
CATALOG = "ai_uat"
SCHEMA = "ai_uat_coding_gold"
# SCHEMA = "ai_uat_coding_logs"
FULL_MODEL_NAME = f"{CATALOG}.{SCHEMA}.{REGISTERED_MODEL_NAME}"

# MLflow setup
mlflow.set_registry_uri("databricks-uc")

# COMMAND ----------

# MAGIC %md
# MAGIC ## 4. Prepare PyFunc Wrapper Path

# COMMAND ----------

# Assuming wrapper is in current directory
model_path = os.path.join(os.getcwd(), "./Jina_reranker_wrapper.py")

# Verify file exists
if not os.path.exists(model_path):
    raise FileNotFoundError(f"Wrapper file not found at {model_path}")

print(f"Using wrapper at: {model_path}")

# COMMAND ----------

# MAGIC %md
# MAGIC ## 5. Create Input/Output Examples and Signature

# COMMAND ----------

# MAGIC %md
# MAGIC ### 🎯 Signature Strategy for Reranker API
# MAGIC
# MAGIC **Purpose:** Accept a query and list of documents to rerank based on relevance.
# MAGIC
# MAGIC **Input Format:**
# MAGIC ```python
# MAGIC {
# MAGIC     "query": "your search query",
# MAGIC     "documents": ["doc1", "doc2", "doc3"],
# MAGIC     "max_length": 1024,  # Optional
# MAGIC     "doc_type": "text",  # Optional: "text" or "image"
# MAGIC     "query_type": "text",  # Optional: "text" or "image"
# MAGIC     "return_documents": False  # Optional
# MAGIC }
# MAGIC ```
# MAGIC
# MAGIC **Supported Use Cases:**
# MAGIC 1. **Text-to-Text**: Query text, rank text documents (default)
# MAGIC 2. **Text-to-Image**: Query text, rank image documents
# MAGIC 3. **Image-to-Text**: Query image, rank text documents  
# MAGIC 4. **Image-to-Image**: Query image, rank image documents
# MAGIC
# MAGIC **Output Format:**
# MAGIC ```python
# MAGIC {
# MAGIC     "model": "jinaai/jina-reranker-m0",
# MAGIC     "results": [
# MAGIC         {"index": 1, "relevance_score": 0.93},
# MAGIC         {"index": 0, "relevance_score": 0.89},
# MAGIC         {"index": 2, "relevance_score": 0.45}
# MAGIC     ],
# MAGIC     "usage": {"total_tokens": 2813}
# MAGIC }
# MAGIC ```
# MAGIC
# MAGIC Results are automatically sorted by relevance_score (highest first).

# COMMAND ----------

# Input examples for reranking
import pandas as pd
from mlflow.models import infer_signature

# Example 1: Text-to-Text reranking (most common use case)
input_example_text = pd.DataFrame({
    "query": ["What is machine learning?"],
    "documents": [[
        "Machine learning is a subset of artificial intelligence that enables systems to learn from data.",
        "Python is a popular programming language.",
        "Deep learning uses neural networks with multiple layers."
    ]]
})

# Example 2: Text-to-Image reranking
input_example_text_to_image = pd.DataFrame({
    "query": ["red sports car"],
    "documents": [[
        "https://example.com/red-ferrari.jpg",
        "https://example.com/blue-sedan.jpg",
        "https://example.com/red-porsche.jpg"
    ]],
    "doc_type": ["image"],
    "max_length": [2048]
})

# Example 3: Image-to-Text reranking
input_example_image_to_text = pd.DataFrame({
    "query": ["https://example.com/chart.png"],
    "documents": [[
        "This document explains data visualization techniques.",
        "A guide to cooking Italian food.",
        "Statistical analysis and chart interpretation."
    ]],
    "query_type": ["image"]
})

# ✅ RECOMMENDED: Use text-to-text for signature (most common)
input_example = input_example_text

print("📋 Selected input example: TEXT-TO-TEXT RERANKING")
print("   - Query: text string")
print("   - Documents: list of text strings")
print("   - Auto-detection: wrapper can detect image inputs automatically")

# Output example (Jina API format)
output_example = {
  "id": "rerank-4f9a12c9e3b1a6d2",
  "results": [
    {
      "index": 3,
      "relevance_score": 0.91
    },
    {
      "index": 0,
      "relevance_score": 0.77
    },
    {
      "index": 1,
      "relevance_score": 0.32
    }
  ],
  "meta": {
    "model": "jinaai/jina-reranker-m0",
    "backend": "transformers",
    "query_type": "text",
    "doc_type": "image",
    "num_documents": 5,
    "latency_ms": 41,
    "max_length": 2048
  }
}


# Infer signature from DataFrame input
signature = infer_signature(input_example, output_example)

print("\n✅ Signature created successfully")
print(f"Input schema: {signature.inputs}")
print(f"\n💡 Usage:")
print(f"   Required: 'query' (string), 'documents' (array)")
print(f"   Optional: 'max_length', 'doc_type', 'query_type', 'return_documents'")
print(f"\n📊 Results are automatically sorted by relevance (highest first)")


# COMMAND ----------

# MAGIC %md
# MAGIC ## 6. Log Model to MLflow

# COMMAND ----------

# Start MLflow run
with mlflow.start_run(run_name=f"{REGISTERED_MODEL_NAME}_run") as run:
    mlflow.set_tag("ai_project_id", "coding")
    mlflow.set_tag("model_type", "reranker")
    
    model_info = mlflow.pyfunc.log_model(
        python_model=model_path,
        registered_model_name=FULL_MODEL_NAME,
        artifact_path=f"{REGISTERED_MODEL_NAME}",
        input_example=input_example,
        signature=signature,
        pip_requirements=[
            "transformers>=4.45.0,<4.54.0",  # Required for Qwen2VL compatibility
            "torch>=2.0.0",
            "accelerate>=1.0.0",
            "pandas>=1.5.0",
            "numpy>=1.23.0",
            "Pillow>=10.0.0",
            "requests>=2.31.0",
            "einops",  # Required by Qwen2VL
            "qwen-vl-utils",  # Utilities for Qwen2VL
        ],
    )
    
print("✅ Model logged successfully to MLflow")
print(f"Model URI: {model_info.model_uri}")
print(f"Run ID: {run.info.run_id}")
print(f"✅ Supports: Text-to-Text, Text-to-Image, Image-to-Text, Image-to-Image reranking")

# COMMAND ----------

# MAGIC %md
# MAGIC ## 7. Register Model to Unity Catalog

# COMMAND ----------

UC_MODEL_NAME = FULL_MODEL_NAME

print(f"Registering model to Unity Catalog: {UC_MODEL_NAME}")

uc_registered_model_info = mlflow.register_model(
    model_uri=model_info.model_uri, 
    name=UC_MODEL_NAME
)

print(f"✅ Model registered successfully")
print(f"Model Name: {uc_registered_model_info.name}")
print(f"Version: {uc_registered_model_info.version}")

# COMMAND ----------

# MAGIC %md
# MAGIC ## 8. Configure Endpoint Settings

# COMMAND ----------

# # Endpoint configuration based on environment
# scale_to_zero_enabled = True if ENVIRONMENT in ("dev", "uat") else False

# ENDPOINT_NAME = "ai-dev-" + UC_MODEL_NAME.split(".")[-1]

# # Workload size based on environment
# if ENVIRONMENT == "dev":
#     workload_size = "Small"
#     print("Using Small workload size for dev environment")
# elif ENVIRONMENT == "uat":
#     workload_size = "Medium"
#     print("Using Medium workload size for UAT environment")
# else:
#     workload_size = "Large"
#     print("Using Large workload size for production")

# from databricks.sdk.service.serving import ServingModelWorkloadType

# # Configure served entities
# # Note: Reranker model is 2.4B params, may need GPU for optimal performance
# served_entities = [
#     ServedEntityInput(
#         name=ENDPOINT_NAME,
#         entity_name=uc_registered_model_info.name,
#         entity_version=uc_registered_model_info.version,
#         workload_type=ServingModelWorkloadType.GPU_SMALL,  # T4 GPU for reranker (2.4B params)
#         workload_size=workload_size,
#         scale_to_zero_enabled=scale_to_zero_enabled
#     )
# ]

# # Traffic configuration
# traffic_config = TrafficConfig([
#     Route(served_model_name=ENDPOINT_NAME, traffic_percentage=100)
# ])

# print(f"Endpoint configuration:")
# print(f"  - Name: {ENDPOINT_NAME}")
# print(f"  - Workload: GPU_SMALL ({workload_size})")
# print(f"  - Scale to Zero: {scale_to_zero_enabled}")
# print(f"  - Model size: 2.4B parameters (Qwen2-VL-2B base)")

# # COMMAND ----------

# # MAGIC %md
# # MAGIC ## 9. Create/Update Serving Endpoint

# # COMMAND ----------


# w = WorkspaceClient()

# endpoint_config = EndpointCoreConfigInput(
#     served_entities=served_entities,
#     traffic_config=traffic_config,
# )

# try:
#     # Check if endpoint exists
#     endpoint = w.serving_endpoints.get(ENDPOINT_NAME)
#     print(f"📝 Endpoint '{ENDPOINT_NAME}' exists. Updating...")
#     print(f"\n🔗 View deployment: https://adb-379144824042062.2.azuredatabricks.net/ml/endpoints/{ENDPOINT_NAME}")
    
#     # Update existing endpoint
#     w.serving_endpoints.update_config_and_wait(
#         name=ENDPOINT_NAME,
#         served_entities=served_entities,
#         traffic_config=traffic_config,
#         timeout=timedelta(minutes=30),
#     )
    
#     # Update tags
#     try:
#         w.serving_endpoints.patch(
#             name=ENDPOINT_NAME,
#             add_tags=[
#                 EndpointTag(key="ai_project_id", value="coding"),
#                 EndpointTag(key="model_type", value="reranker"),
#                 EndpointTag(key="backend", value="Transformers")
#             ],
#         )
#     except AttributeError as e:
#         print(f"⚠️  Warning: Tags updated but with warning: {e}")
    
#     print(f"✅ Endpoint '{ENDPOINT_NAME}' updated successfully")
    
# except ResourceDoesNotExist:
#     # Create new endpoint
#     print(f"🆕 Endpoint does not exist. Creating new endpoint '{ENDPOINT_NAME}'...")
#     print(f"\n🔗 View deployment: https://adb-379144824042062.2.azuredatabricks.net/ml/endpoints/{ENDPOINT_NAME}")
    
#     w.serving_endpoints.create_and_wait(
#         name=ENDPOINT_NAME,
#         config=endpoint_config,
#         tags=[
#             EndpointTag(key="ai_project_id", value="coding"),
#             EndpointTag(key="model_type", value="reranker")
#         ],
#         timeout=timedelta(minutes=50),
#     )
    
#     print(f"✅ Endpoint '{ENDPOINT_NAME}' created successfully")


# # COMMAND ----------

# # MAGIC %md
# # MAGIC ## 10. Set Endpoint Permissions

# # COMMAND ----------


# try:
#     # Get endpoint details
#     endpoint = w.serving_endpoints.get(name=ENDPOINT_NAME)
#     serving_endpoint_id = endpoint.id
    
#     # Set permissions for team members
#     w.serving_endpoints.set_permissions(
#         serving_endpoint_id=serving_endpoint_id,
#         access_control_list=[
#             ServingEndpointAccessControlRequest(
#                 user_name="hoang.pham.uh@renesas.com",
#                 permission_level=ServingEndpointPermissionLevel.CAN_MANAGE,
#             ),
#             ServingEndpointAccessControlRequest(
#                 user_name="dung.nguyen.te@renesas.com",
#                 permission_level=ServingEndpointPermissionLevel.CAN_MANAGE,
#             )
#         ],
#     )
    
#     print("✅ Permissions set successfully")
    
# except Exception as e:
#     print(f"⚠️  Error setting permissions: {e}")

# # End MLflow run
# mlflow.end_run()

# print("\n" + "="*60)
# print("🎉 DEPLOYMENT COMPLETED SUCCESSFULLY!")
# print("="*60)
# print(f"\n📊 Endpoint Details:")
# print(f"   - Name: {ENDPOINT_NAME}")
# print(f"   - Model: {FULL_MODEL_NAME}")
# print(f"   - Version: {uc_registered_model_info.version}")
# print(f"   - Environment: {ENVIRONMENT}")
# print(f"\n🔗 Access endpoint at:")
# print(f"   https://adb-379144824042062.2.azuredatabricks.net/ml/endpoints/{ENDPOINT_NAME}")


# COMMAND ----------

# MAGIC %md
# MAGIC ## 11. Test Endpoint (Optional)

# COMMAND ----------


# Test the deployed endpoint
# import requests
# import os

# # Get Databricks token
# token = dbutils.notebook.entry_point.getDbutils().notebook().getContext().apiToken().get()

# # Endpoint URL
# endpoint_url = f"https://adb-379144824042062.2.azuredatabricks.net/serving-endpoints/{ENDPOINT_NAME}/invocations"

# headers = {
#     "Authorization": f"Bearer {token}",
#     "Content-Type": "application/json"
# }

# print("=" * 60)
# print("TESTING RERANKER ENDPOINT")
# print("=" * 60)

# # Test 1: Text-to-Text reranking (most common use case)
# print("\n📝 Test 1: Text-to-Text Reranking")
# test_payload_text = {
#     "dataframe_split": {
#         "columns": ["query", "documents"],
#         "data": [[
#             "What is machine learning?",
#             [
#                 "Machine learning is a subset of artificial intelligence that enables systems to learn from data.",
#                 "Python is a popular programming language for data science.",
#                 "Deep learning uses neural networks with multiple layers to process data.",
#                 "The weather today is sunny and warm."
#             ]
#         ]]
#     }
# }

# try:
#     response = requests.post(endpoint_url, json=test_payload_text, headers=headers)
    
#     if response.status_code == 200:
#         result = response.json()
#         print("✅ Text-to-Text reranking successful!")
#         print(f"   - Documents reranked: {result['metadata']['num_documents']}")
#         print(f"   - Time: {result['performance']['generation_time_ms']}ms")
#         print(f"\n   Top 3 results:")
#         for i, res in enumerate(result['results'][:3]):
#             print(f"      {i+1}. Document #{res['index']} - Score: {res['relevance_score']:.4f}")
#     else:
#         print(f"❌ Test failed with status {response.status_code}")
#         print(f"Error: {response.text}")
        
# except Exception as e:
#     print(f"❌ Error testing endpoint: {e}")

# # Test 2: With return_documents option
# print("\n📝 Test 2: Reranking with Document Content")
# test_payload_with_docs = {
#     "dataframe_split": {
#         "columns": ["query", "documents", "return_documents"],
#         "data": [[
#             "machine learning",
#             [
#                 "ML is a branch of AI",
#                 "Cooking recipes collection",
#                 "Neural networks and deep learning"
#             ],
#             True
#         ]]
#     }
# }

# try:
#     response = requests.post(endpoint_url, json=test_payload_with_docs, headers=headers)
    
#     if response.status_code == 200:
#         result = response.json()
#         print("✅ Reranking with documents successful!")
#         print(f"\n   Results with content:")
#         for i, res in enumerate(result['results'][:3]):
#             print(f"      {i+1}. Score: {res['relevance_score']:.4f}")
#             if 'document' in res:
#                 print(f"         Content: {res['document']['text'][:50]}...")
#     else:
#         print(f"❌ Test failed: {response.text}")
        
# except Exception as e:
#     print(f"❌ Error: {e}")

# # Test 3: Image reranking example (conceptual)
# print("\n🖼️ Test 3: Text-to-Image Reranking (Example)")
# print("   To test image reranking, use:")
# print("   test_payload_image = {")
# print('       "dataframe_split": {')
# print('           "columns": ["query", "documents", "doc_type", "max_length"],')
# print('           "data": [[')
# print('               "red sports car",')
# print('               [')
# print('                   "https://example.com/red-ferrari.jpg",')
# print('                   "https://example.com/blue-sedan.jpg",')
# print('                   "https://example.com/red-porsche.jpg"')
# print('               ],')
# print('               "image",')
# print('               2048')
# print('           ]]')
# print('       }')
# print("   }")

# print("\n" + "=" * 60)
# print("💡 KEY TAKEAWAYS")
# print("=" * 60)
# print("✅ Query + Documents → Ranked results by relevance")
# print("✅ Supports text, images, and multimodal inputs")
# print("✅ Results automatically sorted (highest score first)")
# print("✅ Use for RAG retrieval, search ranking, document relevance")
# print("=" * 60)


# COMMAND ----------

# MAGIC %md
# MAGIC ## Summary
# MAGIC
# MAGIC This notebook:
# MAGIC 1. ✅ Installs required dependencies (Transformers, Pillow for image support)
# MAGIC 2. ✅ Loads Jina Reranker m0 multimodal wrapper
# MAGIC 3. ✅ Logs model to MLflow with proper signature
# MAGIC 4. ✅ Registers model to Unity Catalog
# MAGIC 5. ✅ Creates/updates serving endpoint with GPU acceleration
# MAGIC 6. ✅ Sets appropriate permissions
# MAGIC 7. ✅ Tests the deployed endpoint
# MAGIC
# MAGIC ### Features:
# MAGIC - 🔤 **Text-to-Text**: Rank text documents by query relevance
# MAGIC - 🖼️ **Text-to-Image**: Rank image documents by text query
# MAGIC - 🔄 **Image-to-Text**: Rank text documents by image query
# MAGIC - 🎨 **Image-to-Image**: Rank image documents by image query
# MAGIC - 🌍 **Multilingual**: 29+ languages supported
# MAGIC - ⚡ **GPU Optimized**: FlashAttention-2 support for faster inference
# MAGIC - 📏 **Long Context**: Up to 10K tokens context length
# MAGIC
# MAGIC ### Model Architecture:
# MAGIC - **Base Model**: Qwen2-VL-2B-Instruct (2.4B parameters)
# MAGIC - **Architecture**: Vision Language Model with LoRA fine-tuning
# MAGIC - **Max Context**: 10,240 tokens (query + document)
# MAGIC - **Image Support**: Dynamic resolution from 56×56 to 4K
# MAGIC
# MAGIC ### Usage Examples:
# MAGIC
# MAGIC #### Text-to-Text Reranking (Default):
# MAGIC ```python
# MAGIC payload = {
# MAGIC     "dataframe_split": {
# MAGIC         "columns": ["query", "documents"],
# MAGIC         "data": [[
# MAGIC             "What is machine learning?",
# MAGIC             [
# MAGIC                 "ML is a subset of AI...",
# MAGIC                 "Python is a programming language...",
# MAGIC                 "Deep learning uses neural networks..."
# MAGIC             ]
# MAGIC         ]]
# MAGIC     }
# MAGIC }
# MAGIC ```
# MAGIC
# MAGIC #### Text-to-Image Reranking:
# MAGIC ```python
# MAGIC payload = {
# MAGIC     "dataframe_split": {
# MAGIC         "columns": ["query", "documents", "doc_type", "max_length"],
# MAGIC         "data": [[
# MAGIC             "red sports car",
# MAGIC             [
# MAGIC                 "https://example.com/ferrari.jpg",
# MAGIC                 "https://example.com/sedan.jpg",
# MAGIC                 "https://example.com/porsche.jpg"
# MAGIC             ],
# MAGIC             "image",
# MAGIC             2048
# MAGIC         ]]
# MAGIC     }
# MAGIC }
# MAGIC ```
# MAGIC
# MAGIC #### Image-to-Text Reranking:
# MAGIC ```python
# MAGIC payload = {
# MAGIC     "dataframe_split": {
# MAGIC         "columns": ["query", "documents", "query_type"],
# MAGIC         "data": [[
# MAGIC             "https://example.com/chart.png",
# MAGIC             [
# MAGIC                 "Data visualization guide",
# MAGIC                 "Cooking recipes",
# MAGIC                 "Statistical analysis"
# MAGIC             ],
# MAGIC             "image"
# MAGIC         ]]
# MAGIC     }
# MAGIC }
# MAGIC ```
# MAGIC
# MAGIC ### Response Format:
# MAGIC ```python
# MAGIC {
# MAGIC     "model": "jinaai/jina-reranker-m0",
# MAGIC     "results": [
# MAGIC         {"index": 0, "relevance_score": 0.95},
# MAGIC         {"index": 2, "relevance_score": 0.78},
# MAGIC         {"index": 1, "relevance_score": 0.12}
# MAGIC     ],
# MAGIC     "usage": {"total_tokens": 150},
# MAGIC     "performance": {
# MAGIC         "generation_time_ms": 150,
# MAGIC         "documents_per_second": 20.0
# MAGIC     }
# MAGIC }
# MAGIC ```
# MAGIC
# MAGIC ### API Call:
# MAGIC ```python
# MAGIC import requests
# MAGIC
# MAGIC response = requests.post(endpoint_url, json=payload, headers=headers)
# MAGIC ranked_results = response.json()
# MAGIC ```
# MAGIC
# MAGIC ### Use Cases:
# MAGIC - 📚 **RAG Systems**: Rerank retrieved documents for better relevance
# MAGIC - 🔍 **Search Engines**: Improve search result ranking
# MAGIC - 📄 **Document Matching**: Find most relevant documents
# MAGIC - 🎯 **Question Answering**: Rank candidate answers
# MAGIC - 🖼️ **Visual Search**: Rank images by text query or image query

# COMMAND ----------

# MAGIC %md
# MAGIC