# Databricks notebook source
# MAGIC %md
# MAGIC ## Create Unity Catalog Volume for Model Storage
# MAGIC
# MAGIC This notebook creates a managed volume in Unity Catalog to store Qwen models.
# MAGIC
# MAGIC **⚠️ Run this notebook BEFORE running the model provision job!**

# COMMAND ----------

# MAGIC %md
# MAGIC ### Step 1: Create Volume using SQL

# COMMAND ----------

# MAGIC %sql
# MAGIC -- Create a managed volume for storing models in ai_uat_coding_logs schema
# MAGIC CREATE VOLUME IF NOT EXISTS ai_uat_02.ai_uat_coding_logs.qwencoder_models
# MAGIC COMMENT 'Volume for storing Qwen coder models and their Llama conversions';

# COMMAND ----------

# MAGIC %md
# MAGIC ### Step 2: Verify Volume Creation

# COMMAND ----------

# MAGIC %sql
# MAGIC -- Verify the volume was created
# MAGIC SHOW VOLUMES IN ai_uat_02.ai_uat_coding_logs;

# COMMAND ----------

# MAGIC %sql
# MAGIC -- Get detailed information about the volume
# MAGIC DESCRIBE VOLUME EXTENDED ai_uat_02.ai_uat_coding_logs.qwencoder_models;

# COMMAND ----------

# MAGIC %md
# MAGIC ### Step 3: Grant Permissions

# COMMAND ----------

# MAGIC %sql
# MAGIC -- Grant permissions to the AI Admin group
# MAGIC GRANT ALL PRIVILEGES ON VOLUME ai_uat_02.ai_uat_coding_logs.qwencoder_models 
# MAGIC TO `04_AI_Admin`;

# COMMAND ----------

# MAGIC %sql
# MAGIC -- Grant permissions to Service Principal
# MAGIC GRANT ALL PRIVILEGES ON VOLUME ai_uat_02.ai_uat_coding_logs.qwencoder_models 
# MAGIC TO `0fe63045-971f-47fd-ac08-314142ab8f0c`;

# COMMAND ----------

# MAGIC %md
# MAGIC ### Step 4: Verify Volume Access with Python

# COMMAND ----------

import os

# Test that we can access the volume
volume_path = "/Volumes/ai_uat_02/ai_uat_coding_logs/qwencoder_models"

try:
    # Try to list contents (should be empty initially)
    if os.path.exists(volume_path):
        contents = os.listdir(volume_path)
        print(f"✅ Volume accessible at: {volume_path}")
        print(f"📁 Current contents: {contents if contents else '(empty)'}")
    else:
        print(f"❌ Volume not accessible at: {volume_path}")
        print(f"   Please verify the volume was created successfully.")
except Exception as e:
    print(f"❌ Error accessing volume: {e}")

# COMMAND ----------

# MAGIC %md
# MAGIC ### Step 5: Test Write Permissions

# COMMAND ----------

import os

volume_path = "/Volumes/ai_uat_02/ai_uat_coding_logs/qwencoder_models"
test_file = os.path.join(volume_path, "test_write.txt")

try:
    # Try to write a test file
    with open(test_file, "w") as f:
        f.write("Test write successful!")
    print(f"✅ Write permission verified. Test file created at: {test_file}")
    
    # Read it back
    with open(test_file, "r") as f:
        content = f.read()
    print(f"✅ Read permission verified. Content: '{content}'")
    
    # Clean up test file
    os.remove(test_file)
    print(f"✅ Test file removed successfully")
    
    print(f"\n🎉 Volume is ready for use!")
except Exception as e:
    print(f"❌ Error writing to volume: {e}")
    print(f"   Please check permissions were granted correctly.")

# COMMAND ----------

# MAGIC %md
# MAGIC ## Alternative: Using Python SDK to Create Volume

# COMMAND ----------

# Uncomment to create volume using Python SDK instead of SQL
"""
from databricks.sdk import WorkspaceClient
from databricks.sdk.service.catalog import VolumeType

w = WorkspaceClient()

try:
    volume = w.volumes.create(
        catalog_name="ai_uat_02",
        schema_name="ai_uat_coding_logs",
        name="qwencoder_models",
        volume_type=VolumeType.MANAGED,
        comment="Volume for storing Qwen coder models"
    )
    print(f"✅ Volume created: {volume.name}")
    print(f"   Full path: {volume.full_name}")
    print(f"   Storage location: {volume.storage_location}")
except Exception as e:
    if "already exists" in str(e).lower():
        print(f"✅ Volume already exists")
    else:
        print(f"❌ Error creating volume: {e}")
"""

# COMMAND ----------

# MAGIC %md
# MAGIC ## Summary
# MAGIC
# MAGIC ✅ **If all steps passed**, you can now:
# MAGIC 1. Deploy the bundle: `databricks bundle deploy --target uat`
# MAGIC 2. Run the provision job: `databricks bundle run selfhost_model_provision --target uat`
# MAGIC
# MAGIC 📁 **Volume path**: `/Volumes/ai_uat_02/ai_uat_coding_logs/qwencoder_models`
# MAGIC
# MAGIC 🔐 **Permissions granted to**:
# MAGIC - Group: `04_AI_Admin`
# MAGIC - Service Principal: `0fe63045-971f-47fd-ac08-314142ab8f0c`

# COMMAND ----------
