# Databricks Asset Bundle Setup Guide

This repository contains a Databricks Asset Bundle (DAB) that defines notebooks, jobs, clusters, and other resources for deployment to a Databricks workspace.

## 📦 Bundle Structure
```
. 
├── databricks.yml          # Main bundle configuration 
├── variables.yml           # List of global variables (local variables by workspace are defined in databricks.yml)
├── serving_endpoints/      # Serving endpoint definitions
│ └── azure_openai.yml          # GPT models
│ └── claude.yml                # Claude models (not in use)
│ └── managed.yml               # Provisioned models (Qwen-Coder, DeepSeek, etc.)
├── jobs/                   # Job definitions
│ ├── src/
│ │  ├── inference_materialized_view.py     # Script to create aggregated inference tables from serving endpoints
│ └── inference_materialized_view.yml       # Job definition
└── README.md
```

## 🚀 Prerequisites

Before setting up the bundle, ensure you have the following:

- Databricks CLI v0.257.0+ (use `databricks --version` to check)
- Authorized Service Principal's PAT (Personal Access Token) - ask the team members for this token and store it carefully.

## 🔧 Setup Instructions

### 1. Install Databricks CLI

Refer to [Install or update the Databricks CLI | Databricks](https://docs.databricks.com/aws/en/dev-tools/cli/install)
to install the CLI. It is recommended to install using WinGet on Windows.

To check whether the CLI has already been instead, use the command:
```bash
databricks --version
```

### 2. Configurate the token

Open the folder `C:\Users\<DS Number>\` and open `.databrickscfg` file using a text editor. Add the following to
the end of the file.
```text
[ai-uat-coding]
host  = https://adb-893717583776115.15.azuredatabricks.net/
token = <PAT token>

[ai-uat-02-coding]
host  = https://adb-2735645151323815.15.azuredatabricks.net/
token = <PAT token>

[ai-prod-02-coding]
host  = https://adb-4265374015564621.1.azuredatabricks.net/
token = <PAT token>
```

Replace the placeholders `<PAT token>` by the tokens that the team members provided to you.

### 3. Access the bundle folder

Note that this bundle is in a subfolder of the repository. Running `databricks bundle` requires accessing to
the `databricks.yml` file at the current folder. Therefore, run:

```bash
cd deploy
```

### 4. Validate the bundle (optional)

```bash
databricks bundle validate -t <target_name> -p <profile_name>
```
where `target_name` should match one of those in `databricks.yml` file, and `profile_name` should match one of those in
`.databrickscfg` defined in step 2.

For example:
```bash
databricks bundle validate -t ai-uat -p ai-uat-coding
```

### 5. Deploy the bundle

```bash
databricks bundle deploy -t <target_name> -p <profile_name>
```

For example:
```bash
databricks bundle deploy -t ai-uat -p ai-uat-coding
```