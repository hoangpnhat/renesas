# Databricks notebook source
# MAGIC %pip install -U -qqqq databricks-agents==0.8.0 mlflow mlflow-skinny databricks-sdk
# MAGIC dbutils.library.restartPython()

# COMMAND ----------

# MAGIC %run ./00_eval_config
# MAGIC

# COMMAND ----------

import mlflow

pip_requirements = mlflow.pyfunc.get_model_dependencies(
    f"runs:/{experiment_run_id}/model"
)

# COMMAND ----------

# MAGIC %pip install -r $pip_requirements
# MAGIC dbutils.library.restartPython()

# COMMAND ----------

# MAGIC %run ./00_eval_config

# COMMAND ----------

# MAGIC %md
# MAGIC # Load your evaluation set from the previous step

# COMMAND ----------

df = spark.table(eval_table)
eval_df = df.toPandas()
display(eval_df)

# COMMAND ----------

# MAGIC %md
# MAGIC # Evaluate the POC application

# COMMAND ----------

# MAGIC %md
# MAGIC ## Load the correct Python environment for the POC's app
# MAGIC
# MAGIC TODO: replace this with env_manager=virtualenv once that works
# MAGIC
# MAGIC
# MAGIC
# MAGIC

# COMMAND ----------

import mlflow
import pandas as pd
from databricks import agents

# COMMAND ----------

# MAGIC %md
# MAGIC ## Run evaluation on the POC app

# COMMAND ----------

from mlflow.models import MetricThreshold

thresholds = {
    "response/llm_judged/correctness/rating/percentage": MetricThreshold(
        # accuracy should be >=0.8
        threshold=0.2,
        greater_is_better=True,
    ),
}

# COMMAND ----------

with mlflow.start_run(run_id=experiment_run_id):
    # Evaluate
    eval_results = mlflow.evaluate(
        data=eval_df,
        model=f"runs:/{experiment_run_id}/model",  # replace `model` with artifact_path that you used when calling log_model.  By default, this is `chain`.
        model_type="databricks-agent",
        validation_thresholds=thresholds,
    )

# COMMAND ----------

# MAGIC %md
# MAGIC ## Look at the evaluation results
# MAGIC
# MAGIC You can explore the evaluation results using the above links to the MLflow UI.  If you prefer to use the data directly, see the cells below.

# COMMAND ----------

# Summary metrics across the entire evaluation set
eval_results.metrics
