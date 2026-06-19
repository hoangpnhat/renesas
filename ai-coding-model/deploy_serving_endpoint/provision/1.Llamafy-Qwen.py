# Databricks notebook source
# MAGIC %md
# MAGIC ## Llamafy Qwen
# MAGIC
# MAGIC This notebook modifies a Qwen QwQ model so that it is consistent with the Llama architecture.

# COMMAND ----------

# MAGIC %md
# MAGIC ## Storage: Unity Catalog Volumes
# MAGIC
# MAGIC Models will be stored in Unity Catalog Volumes for:
# MAGIC - ✅ Persistent storage (survives cluster termination)
# MAGIC - ✅ Shared access across clusters
# MAGIC - ✅ Better governance and access control
# MAGIC - ✅ Integration with Unity Catalog

# COMMAND ----------

# MAGIC %pip install -U mlflow huggingface_hub transformers==4.51.0 torch==2.3.0 accelerate==0.29.0
# MAGIC dbutils.library.restartPython()

# COMMAND ----------

import json
import os
import shutil
from collections import OrderedDict
from pathlib import Path
from tqdm import tqdm

import huggingface_hub
import torch
from safetensors import safe_open
from safetensors.torch import save_file
from huggingface_hub import split_torch_state_dict_into_shards
from transformers.modeling_utils import SAFE_WEIGHTS_INDEX_NAME, SAFE_WEIGHTS_NAME, WEIGHTS_INDEX_NAME, WEIGHTS_NAME
from databricks.sdk import WorkspaceClient
from databricks.sdk.runtime import dbutils

# The model to download from Hugging Face
dbutils.widgets.text("model_name", "Qwen/Qwen2.5-Coder-7B")

# Unity Catalog Volume path where models will be stored
dbutils.widgets.text("uc_volume_path", "/Volumes/ai_uat_02/ai_uat_coding_logs/qwencoder_models")

# COMMAND ----------

# Define a function to convert and save Qwen weights

def save_weight(input_dir: str, output_dir: str, shard_size: str = "2GB") -> None:
    """
    Copies a Qwen model in the input directory to a Llama-compatible version of it in the output directory.
    Injects zeroed-out bias vectors in attention layers where needed in order to make it compatible with the Llama
    architecture.  Also updates configuration as needed.
    """
    # Load Qwen state dict from .safetensors files
    qwen_state_dict = OrderedDict()
    for filepath in os.listdir(input_dir):
        if filepath.endswith(".safetensors"):
            full_path = os.path.join(input_dir, filepath)
            with safe_open(full_path, framework="pt", device="cpu") as sf:
                for key in sf.keys():
                    qwen_state_dict[key] = sf.get_tensor(key)

    # Copy tensors and inject bias where needed to match Llama
    llama_state_dict = OrderedDict()
    for key, value in qwen_state_dict.items():
        llama_state_dict[key] = value
        # Qwen omits bias on attn.o_proj; Llama expects it
        if "attn.o_proj.weight" in key:
            # Each attn.o_proj.weight needs an associated bias in order to be
            # compatible with the Llama architecture. Since Qwen doesn't use this we
            # insert zeroed out vectors.
            bias_key = key.replace("attn.o_proj.weight", "attn.o_proj.bias")
            llama_state_dict[bias_key] = torch.zeros_like(value[:, 0]).squeeze()

    # Save weights using safetensors
    filename_pattern = SAFE_WEIGHTS_NAME.replace(".safetensors", "{suffix}.safetensors")
    state_dict_split = split_torch_state_dict_into_shards(llama_state_dict, 
                                                          filename_pattern=filename_pattern,
                                                          max_shard_size=shard_size)
    for shard_file, tensors in tqdm(state_dict_split.filename_to_tensors.items()):
        shard = {tensor: llama_state_dict[tensor].contiguous() for tensor in tensors}
        save_file(shard, os.path.join(output_dir, shard_file), metadata={"format": "pt"})

    index = {
        "metadata": state_dict_split.metadata,
        "weight_map": state_dict_split.tensor_to_filename,
    }
    with open(os.path.join(output_dir, SAFE_WEIGHTS_INDEX_NAME), "w", encoding="utf-8") as f:
        json.dump(index, f, indent=2, sort_keys=True)

    print(f"Model weights saved in {output_dir}.")

# COMMAND ----------

# Define a function to update the Qwen config and tokenizer files


def save_configs(input_dir: str, output_dir: str) -> None:
    """
    Copies Qwen config and tokenizer files to output_dir, removing Qwen-specific fields
    and making them compatible with the Llama architecture.
    """
    config_name = "config.json"

    # Load Qwen config.json
    with open(os.path.join(input_dir, config_name), encoding="utf-8") as f:
        qwen_config_dict = json.load(f)

    # Modify the Qwen config to look like a Llama model
    llama_config_dict = {**qwen_config_dict}
    llama_config_dict["architectures"] = ["LlamaForCausalLM"]  # now it's Llama 8-)
    llama_config_dict["model_type"] = "llama"
    llama_config_dict["attention_bias"] = True  # Llama-specific
    llama_config_dict["mlp_bias"] = False
    llama_config_dict["pretraining_tp"] = 0

    # Remove Qwen-specific fields related to sliding window
    for del_key in ["sliding_window", "use_sliding_window", "max_window_layers"]:
        if del_key in llama_config_dict:
            del llama_config_dict[del_key]

    # Write updated config to the new directory
    with open(os.path.join(output_dir, config_name), "w", encoding="utf-8") as f:
        json.dump(llama_config_dict, f, indent=2)

    # Copy other relevant files (tokenizer, merges, vocab, and so on)
    additional_files = [
        "generation_config.json",
        "merges.txt",
        "tokenizer.json",
        "tokenizer_config.json",
        "vocab.json",
    ]
    for fname in additional_files:
        src = os.path.join(input_dir, fname)
        dst = os.path.join(output_dir, fname)
        if os.path.exists(src):
            shutil.copyfile(src, dst)


# COMMAND ----------

# Master Function to Orchestrate the Qwen→Llama Conversion

def llamafy_qwen(input_dir: str, output_dir: str) -> None:
    """
    Converts Qwen2.5 into a Llama-like architecture by rewriting weights and configs.
    After this step, the resulting folder can be treated as if it's a Llama model.
    """
    os.makedirs(output_dir, exist_ok=False)

    # Rewrite Qwen weights to add missing biases
    save_weight(input_dir, output_dir)

    # Update config to make it a Llama model and copy other files
    save_configs(input_dir, output_dir)

    print(f"Successfully converted Qwen from '{input_dir}' to Llama format at '{output_dir}'.")


# COMMAND ----------

# Download model files from Hugging Face to Unity Catalog Volume
model_name = dbutils.widgets.get("model_name")
uc_volume_path = dbutils.widgets.get("uc_volume_path")

print(f"Model name: {model_name}")
print(f"UC Volume path: {uc_volume_path}")

assert model_name, "model_name is required"
assert uc_volume_path, "uc_volume_path is required"

# Construct target paths in UC Volumes
target_uc_path = os.path.join(uc_volume_path, model_name)
target_uc_modified_path = f"{target_uc_path}-Llama"

# Verify UC Volume exists and is accessible
if not os.path.exists(uc_volume_path):
    raise RuntimeError(
        f"❌ UC Volume does not exist or is not accessible: {uc_volume_path}\n\n"
        f"Please create the volume first by running:\n"
        f"  CREATE VOLUME IF NOT EXISTS ai_uat_02.ai_uat_coding_logs.qwencoder_models;\n"
        f"  GRANT ALL PRIVILEGES ON VOLUME ai_uat_02.ai_uat_coding_logs.qwencoder_models TO `04_AI_Admin`;\n"
        f"  GRANT ALL PRIVILEGES ON VOLUME ai_uat_02.ai_uat_coding_logs.qwencoder_models TO `0fe63045-971f-47fd-ac08-314142ab8f0c`;"
    )

print(f"✅ UC Volume accessible: {uc_volume_path}")

# Create subdirectory for model if needed (e.g., for nested model names like "org/model")
model_parent_dir = os.path.dirname(target_uc_path)
if model_parent_dir != uc_volume_path and not os.path.exists(model_parent_dir):
    os.makedirs(model_parent_dir, exist_ok=True)
    print(f"✅ Created subdirectory: {model_parent_dir}")

# Download model from Hugging Face to UC Volumes
if not os.path.exists(target_uc_path):
    print(f"📥 Downloading model to UC Volumes: {target_uc_path}")
    print(f"   This may take several minutes depending on model size...")
    huggingface_hub.snapshot_download(
        model_name, 
        local_dir=target_uc_path, 
        max_workers=8
    )
    print(f"✅ Download completed: {target_uc_path}")
else:
    print(f"✅ Model already exists in UC Volumes: {target_uc_path}")

# COMMAND ----------

# Convert Qwen model to Llama format and save to UC Volumes
if not os.path.exists(target_uc_modified_path):
    print(f"🔄 Converting Qwen model to Llama format...")
    print(f"   Input:  {target_uc_path}")
    print(f"   Output: {target_uc_modified_path}")
    print(f"   This may take several minutes...")
    
    llamafy_qwen(target_uc_path, target_uc_modified_path)
    
    print(f"✅ Model conversion completed: {target_uc_modified_path}")
else:
    print(f"✅ Modified model already exists in UC Volumes: {target_uc_modified_path}")

# COMMAND ----------

# MAGIC %md
# MAGIC ## Summary
# MAGIC
# MAGIC Model paths in Unity Catalog Volumes:
# MAGIC - **Original model**: `{target_uc_path}`
# MAGIC - **Llama-converted model**: `{target_uc_modified_path}`
# MAGIC
# MAGIC These paths can be used in the next notebook (2.Register-Llamifed-Qwen-as-UC-Model.py) to register the model in Unity Catalog.

# COMMAND ----------

