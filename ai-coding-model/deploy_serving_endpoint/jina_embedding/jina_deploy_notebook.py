# Databricks notebook source
# MAGIC %md
# MAGIC # Deploy Jina Embeddings v4
# MAGIC
# MAGIC This notebook deploys Jina Embeddings v4 model using vLLM for high-performance inference

# COMMAND ----------

# MAGIC %md
# MAGIC ## 1. Install Dependencies

# COMMAND ----------

# Install required packages
# Note: transformers>=4.45.0 is required for SlidingWindowCache support used by Jina v4
# MAGIC %pip install -qqqq mlflow==2.19.0 mlflow-skinny==2.19.0 databricks-sql-connector==4.0.3
# MAGIC %pip install "transformers>=4.45.0,<4.54.0" torch torchvision torchaudio accelerate==1.8.1 pandas tiktoken==0.9.0 Pillow requests peft einops
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
MODEL_NAME = "jinaai/jina-embeddings-v4" 
REGISTERED_MODEL_NAME = "jina-embeddings-v4"

# Environment setup
ENVIRONMENT = "uat"  # Options: dev, uat, prod
USERNAME = "hoang.pham.uh@renesas.com"

# Unity Catalog configuration
CATALOG = "ai_uat"
SCHEMA = "ai_uat_coding_gold"
# SCHEMA = "ai_uat_common_models"
FULL_MODEL_NAME = f"{CATALOG}.{SCHEMA}.{REGISTERED_MODEL_NAME}"

# MLflow setup
mlflow.set_registry_uri("databricks-uc")

# COMMAND ----------

# MAGIC %md
# MAGIC ## 4. Prepare PyFunc Wrapper Path

# COMMAND ----------

# Assuming wrapper is in current directory
model_path = os.path.join(os.getcwd(), "./Jina_wrapper.py")

# Verify file exists
if not os.path.exists(model_path):
    raise FileNotFoundError(f"Wrapper file not found at {model_path}")

print(f"Using wrapper at: {model_path}")

# COMMAND ----------

# MAGIC %md
# MAGIC ## 5. Create Input/Output Examples and Signature

# COMMAND ----------

# MAGIC %md
# MAGIC ### 🎯 Signature Strategy for Flexible Text + Image Support
# MAGIC
# MAGIC **Problem:** MLflow signature validation is strict - column names must match exactly.
# MAGIC
# MAGIC **Solution Options:**
# MAGIC
# MAGIC 1. **Option A (Recommended): Single 'input' column** ✅
# MAGIC    - Signature: `['input': string]`
# MAGIC    - Send text: `{"input": "hello world"}`
# MAGIC    - Send image: `{"input": "data:image/jpeg;base64,..."}`
# MAGIC    - Wrapper auto-detects type based on content
# MAGIC    - **Advantage:** Generic name avoids confusion between "text" and image content
# MAGIC
# MAGIC 2. **Option B: Use 'inputs' format (bypass signature)**
# MAGIC    - No signature validation
# MAGIC    - Send: `{"inputs": {"texts": [...]} }` or `{"inputs": {"images": [...]} }`
# MAGIC    - More flexible but loses type checking
# MAGIC
# MAGIC 3. **Option C: Multimodal signature**
# MAGIC    - Signature: `['texts': array, 'images': array]`
# MAGIC    - Must always provide both fields (can be empty arrays)
# MAGIC    - More complex for clients
# MAGIC
# MAGIC **Current choice: Option A** - Uses generic `'input'` column name for both text and image use cases.

# COMMAND ----------

# Input examples for embeddings
import pandas as pd
from mlflow.models import infer_signature

# Example 1: Flexible input (DataFrame format) - RECOMMENDED
# Using "input" column name to avoid confusion (can be text OR image)
input_example_flexible_df = pd.DataFrame({
    "input": ["Hello world", "Machine learning is amazing"],
})

# Example 2: Text-specific column
input_example_text_df = pd.DataFrame({
    "text": ["Hello world", "Machine learning is amazing"],
})

# Example 3: Image batch (DataFrame format) - Multiple rows, each with 1 image
input_example_image_batch_df = pd.DataFrame({
    "image": [
        "data:image/jpeg;base64,/9j/4AAQSkZJRg...",  # Image 1
        "data:image/jpeg;base64,/9j/4AAQSkZJRg...",  # Image 2
    ]
})

# Example 4: Multiple images in single request (array format)
input_example_images_array_df = pd.DataFrame({
    "images": [[
        "data:image/jpeg;base64,/9j/4AAQSkZJRg...",
        "data:image/jpeg;base64,/9j/4AAQSkZJRg..."
    ]],
})

# Example 5: Multimodal (text + image)
input_example_multimodal_df = pd.DataFrame({
    "texts": [["A beautiful landscape"]],
    "images": [["https://example.com/image.jpg"]],
})

# ✅ RECOMMENDED: Use "input" for flexible signature
# This creates schema: ['input': string (required)]
# Wrapper will detect if input is text or base64/URL and process accordingly
input_example = input_example_flexible_df

print("📋 Selected input example: FLEXIBLE INPUT")
print("   Column name: 'input' (generic, avoids confusion)")
print("   This allows wrapper to auto-detect input type (text vs image)")
print("   Wrapper logic:")
print("   - If value looks like text → process as text embedding")
print("   - If value starts with 'data:image' or 'http' → process as image embedding")

# Output example
output_example = {
    "model": "jinaai/jina-embeddings-v4",
    "object": "list",
    "data": [
        {
            "object": "embedding",
            "embedding": [0.1, 0.2, 0.3],  # Simplified for example
            "index": 0
        },
        {
            "object": "embedding",
            "embedding": [0.4, 0.5, 0.6],
            "index": 1
        }
    ],
    "usage": {
        "prompt_tokens": 8,
        "total_tokens": 8
    },
    "id": "emb_abc123",
    "created": 1234567890,
    "performance": {
        "generation_time_ms": 50,
        "embeddings_per_second": 40.0,
        "tokens_per_second": 160.0,
        "backend": "huggingface"
    },
    "metadata": {
        "embedding_dimensions": 2048,
        "num_embeddings": 2,
        "input_type": "text",
        "num_texts": 2,
        "num_images": 0,
        "task": None,
        "normalized": True
    }
}

# Infer signature from DataFrame input
# This will create schema: ['input': string (required)]
signature = infer_signature(input_example, output_example)

print("\n✅ Signature created successfully")
print(f"Input schema: {signature.inputs}")
print(f"\n💡 Usage:")
print(f"   Text: dataframe_split with 'input' column → text values")
print(f"   Image: dataframe_split with 'input' column → base64/URL values")
print(f"   Or use 'inputs' format with 'texts'/'images' keys to bypass signature")

# COMMAND ----------

# MAGIC %md
# MAGIC ## 6. Log Model to MLflow

# COMMAND ----------

# Start MLflow run
with mlflow.start_run(run_name=f"{REGISTERED_MODEL_NAME}_run") as run:
    mlflow.set_tag("ai_project_id", "coding")
    mlflow.set_tag("model_type", "embedding")
    
    model_info = mlflow.pyfunc.log_model(
        python_model=model_path,
        registered_model_name=FULL_MODEL_NAME,
        artifact_path=f"model/{REGISTERED_MODEL_NAME}",
        input_example=input_example,
        signature=signature,
        pip_requirements=[
            "transformers>=4.45.0,<4.54.0",  # Required for SlidingWindowCache support
            "torch>=2.0.0",
            "accelerate>=1.0.0",
            "pandas>=1.5.0",
            "numpy>=1.23.0",
            "Pillow>=10.0.0",
            "requests>=2.31.0",
            "peft",
            "einops",  # Required by Jina v4
        ],
    )
    
print("✅ Model logged successfully to MLflow")
print(f"Model URI: {model_info.model_uri}")
print(f"Run ID: {run.info.run_id}")
print(f"✅ Supports: Text, Image, and Multimodal embeddings")

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

# ENDPOINT_NAME = UC_MODEL_NAME.split(".")[-1]

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

# # Configure served entities
# served_entities = [
#     ServedEntityInput(
#         name=ENDPOINT_NAME,
#         entity_name=uc_registered_model_info.name,
#         entity_version=uc_registered_model_info.version,
#         workload_type="GPU_LARGE",  # T4 GPU for embeddings
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
#                 EndpointTag(key="ai_project_id", value="ai_coding"),
#                 EndpointTag(key="model_type", value="embedding"),
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
#             EndpointTag(key="model_type", value="embedding")
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
# print("TESTING ENDPOINT")
# print("=" * 60)

# # Test 1: Text embeddings using 'input' column (matches signature)
# print("\n📝 Test 1: Text Embeddings (dataframe_split with 'input' column)")
# test_payload_text = {
#     "dataframe_split": {
#         "columns": ["input"],
#         "data": [
#             ["What is machine learning?"],
#             ["How does AI work?"]
#         ]
#     }
# }

# try:
#     response = requests.post(endpoint_url, json=test_payload_text, headers=headers)
    
#     if response.status_code == 200:
#         result = response.json()
#         print("✅ Text embedding test successful!")
#         print(f"   - Embeddings generated: {result['metadata']['num_embeddings']}")
#         print(f"   - Dimensions: {result['metadata']['embedding_dimensions']}")
#         print(f"   - Input type detected: {result['metadata']['input_type']}")
#         print(f"   - Time: {result['performance']['generation_time_ms']}ms")
#     else:
#         print(f"❌ Test failed with status {response.status_code}")
#         print(f"Error: {response.text}")
        
# except Exception as e:
#     print(f"❌ Error testing endpoint: {e}")

# # Test 2: Text embeddings using 'inputs' format (bypass signature)
# print("\n📝 Test 2: Text Embeddings (inputs format - bypasses signature)")
# test_payload_inputs = {
#     "inputs": {
#         "texts": ["What is machine learning?", "How does AI work?"]
#     }
# }

# try:
#     response = requests.post(endpoint_url, json=test_payload_inputs, headers=headers)
    
#     if response.status_code == 200:
#         result = response.json()
#         print("✅ Text embedding test successful (inputs format)!")
#         print(f"   - Embeddings generated: {result['metadata']['num_embeddings']}")
#         print(f"   - Dimensions: {result['metadata']['embedding_dimensions']}")
#     else:
#         print(f"❌ Test failed: {response.text}")
        
# except Exception as e:
#     print(f"❌ Error: {e}")

# # Test 3: Image embeddings using 'input' column (auto-detection)
# print("\n🖼️ Test 3: Image Embeddings (auto-detection via 'input' column)")
# print("   To test image embedding, use:")
# print("   test_payload_image = {")
# print('       "dataframe_split": {')
# print('           "columns": ["input"],  # Same column name!')
# print('           "data": [[')
# print('               "data:image/jpeg;base64,/9j/4AAQSkZJRg..."')
# print('           ]]')
# print('       }')
# print("   }")
# print("   ✅ Wrapper auto-detects it's an image and processes accordingly")

# # Test 4: Show inputs format for images
# print("\n🖼️ Test 4: Image Embeddings (inputs format)")
# print("   test_payload_image_inputs = {")
# print('       "inputs": {')
# print('           "images": ["https://example.com/image.jpg"]')
# print("       }")
# print("   }")

# print("\n" + "=" * 60)
# print("💡 KEY TAKEAWAYS")
# print("=" * 60)
# print("✅ Single 'input' column works for BOTH text and images")
# print("✅ Wrapper automatically detects content type")
# print("✅ Use 'inputs' format for explicit control (texts/images keys)")
# print("=" * 60)

# COMMAND ----------

# MAGIC %md
# MAGIC ## Summary
# MAGIC
# MAGIC This notebook:
# MAGIC 1. ✅ Installs required dependencies (including Pillow for image support)
# MAGIC 2. ✅ Loads Jina Embeddings v4 multimodal wrapper
# MAGIC 3. ✅ Logs model to MLflow with proper signature
# MAGIC 4. ✅ Registers model to Unity Catalog
# MAGIC 5. ✅ Creates/updates serving endpoint with Huggingface
# MAGIC 6. ✅ Sets appropriate permissions
# MAGIC 7. ✅ Tests the deployed endpoint
# MAGIC
# MAGIC ### Features:
# MAGIC - 🔤 **Text Embeddings**: Generate embeddings for text inputs
# MAGIC - 🖼️ **Image Embeddings**: Generate embeddings for images (base64, URL, file path)
# MAGIC - 🎨 **Multimodal**: Generate embeddings for both text and images together
# MAGIC - ⚡ **Huggingface Optimized**: High-performance inference with GPU acceleration
# MAGIC
# MAGIC ### Usage Examples:
# MAGIC
# MAGIC #### Text Only:
# MAGIC ```python
# MAGIC payload = {
# MAGIC     "inputs": {
# MAGIC         "texts": ["Your text here", "Another text"]
# MAGIC     }
# MAGIC }
# MAGIC ```
# MAGIC
# MAGIC #### Image Only:
# MAGIC ```python
# MAGIC payload = {
# MAGIC     "inputs": {
# MAGIC         "images": [
# MAGIC             "https://example.com/image.jpg",
# MAGIC             "data:image/jpeg;base64,/9j/4AAQSkZJRg..."
# MAGIC         ]
# MAGIC     }
# MAGIC }
# MAGIC ```
# MAGIC
# MAGIC #### Multimodal (Text + Image):
# MAGIC ```python
# MAGIC payload = {
# MAGIC     "inputs": {
# MAGIC         "texts": ["A description of the image"],
# MAGIC         "images": ["https://example.com/image.jpg"]
# MAGIC     }
# MAGIC }
# MAGIC ```
# MAGIC
# MAGIC ### API Call:
# MAGIC ```python
# MAGIC import requests
# MAGIC
# MAGIC response = requests.post(endpoint_url, json=payload, headers=headers)
# MAGIC embeddings = response.json()
# MAGIC ```

# COMMAND ----------

# MAGIC %md
# MAGIC