# Databricks notebook source
# MAGIC %md
# MAGIC ## Call FMAPI endpoint with new model

# COMMAND ----------

# MAGIC %pip install transformers openai mlflow -U
# MAGIC dbutils.library.restartPython()

# COMMAND ----------

import json
import os
from openai import OpenAI, AsyncOpenAI
import mlflow
from transformers import AutoTokenizer
import asyncio
from asyncio import Semaphore

# COMMAND ----------

# Make it easier to view the endpoint call results in this notebook
mlflow.openai.autolog()

# COMMAND ----------

# MAGIC %md
# MAGIC ## 1. Create chat message and apply qwen template to it for easier calling
# MAGIC

# COMMAND ----------

qwen_tokenizer = AutoTokenizer.from_pretrained("Qwen/Qwen2.5-Coder-7B", trust_remote_code=True)
chat_message=[
  {
    "role": "system",
    "content": "You are Qwen, a helpful coding assistant. Your job is to autocomplete this code and respond with only the autocompletion."},
  {
    "role": "user",
    "content": "#python\n\ndef foo():"
  }
]
message_with_template = qwen_tokenizer.apply_chat_template(chat_message, tokenize=False, add_generation_prompt=True)
len(qwen_tokenizer(message_with_template)['input_ids'])

# COMMAND ----------

message_with_template

# COMMAND ----------

# MAGIC %md
# MAGIC ## 2. Use message_with_template with the completions.create() method as per usual

# COMMAND ----------

DATABRICKS_TOKEN = dbutils.notebook.entry_point.getDbutils().notebook().getContext().apiToken().get()
MODEL = "qwencoder25-7b-test"

client = OpenAI(
  api_key=DATABRICKS_TOKEN,
  base_url="https://e2-demo-field-eng.cloud.databricks.com/serving-endpoints"
)

# COMMAND ----------

completions = client.completions.create(
  prompt=message_with_template,
  model=MODEL,
  max_tokens=128,
  temperature=0.0,
  stop=["<|endoftext|>"]
)

print(completions.choices[0].text)

# COMMAND ----------

# MAGIC %md
# MAGIC ## Async calls test

# COMMAND ----------

async_client = AsyncOpenAI(
  api_key=DATABRICKS_TOKEN,
  base_url="https://e2-demo-field-eng.cloud.databricks.com/serving-endpoints"
)

# COMMAND ----------

qwen_tokenizer = AutoTokenizer.from_pretrained("Qwen/Qwen2.5-Coder-7B", trust_remote_code=True)
chat_message=[
  {
    "role": "system",
    "content": "You are Qwen, a helpful coding assistant. Your job is to autocomplete this code and respond with only the autocompletion."},
  {
    "role": "user",
    "content": "#python\n\ndef foo():"
  }
]
message_with_template = qwen_tokenizer.apply_chat_template(chat_message, tokenize=False, add_generation_prompt=True)
len(qwen_tokenizer(message_with_template)['input_ids'])

# COMMAND ----------

semaphore = Semaphore(10)

async def fetch_completion(prompt):
    async with semaphore:
        response = await async_client.completions.create(
            prompt=prompt,
            model=MODEL,
            max_tokens=128,
            temperature=0.0,
            stop=["<|endoftext|>"]
        )
        return response.choices[0].text

async def run_async_calls():
    tasks = [fetch_completion(message_with_template) for _ in range(100)]
    completions = await asyncio.gather(*tasks)
    for completion in completions:
        print(completion)

# COMMAND ----------

await run_async_calls()