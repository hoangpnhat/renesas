#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
import os
import shutil
from pathlib import Path
from typing import List, Optional

from pydantic import Field, model_validator
from pydantic_settings import BaseSettings, SettingsConfigDict

ROOT = Path(__file__).resolve().parents[2]
ENV_FILE = ROOT / ".env"


class CommonConfig(BaseSettings):
    model_config = SettingsConfigDict(
        env_file=ENV_FILE,
        extra="ignore",
        env_ignore_empty=True)


class DataBricksEndpoint(CommonConfig):
    DATABRICKS_URL: Optional[str] = "test_databricks_url"
    DATABRICKS_ENDPOINT: Optional[str] = "test-serving-{endpoint_name}"
    DATABRICKS_ENDPOINT_NAME: Optional[str] = "test_databricks_endpoint_name"
    DATABRICKS_TOKEN: Optional[str] = None
    DATABRICKS_SERVER_HOSTNAME: str = "adb-893717583776115.15.azuredatabricks.net"
    DATABRICKS_CATALOG: Optional[str] = "ai_uat"
    DATABRICKS_SCHEMA: Optional[str] = "ai_uat_coding_gold"
    DATABRICKS_BUCKET: Optional[str] = "user_documents"
    FILE_TIMEOUT_MINUTES: int = 720
    FILE_MANAGEMENT_JOB: str = "test-file-job-{}"
    DATABRICKS_SERVING_ENDPOINT: str = "https://adb-379144824042062.2.azuredatabricks.net/serving-endpoints/{DATABRICKS_ENDPOINT_NAME}/invocations"
    DATABRICKS_MODEL_LIST_ENDPOINT: str = "api/2.0/serving-endpoints"
    DATABRICKS_EMBEDDING_ENDPOINT: Optional[str] = "jina-embeddings-v4-query"
    FORCE_REDUCE_EMBEDDING_SIZE: Optional[int] = 1024


class AzureSettings(CommonConfig):
    API_CLIENT_ID: Optional[str] = "test_client_id"
    AAD_TENANT_ID: Optional[str] = "test_tenant_id"


class MongoDBSettings(CommonConfig):
    MONGO_URI: Optional[
        str] = "mongodb+srv://haila257:Latruonghai2507@cluster0.0lmzs.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0"
    DATABASE_NAME: Optional[str] = "rica_ai"
    TLS: Optional[bool] = False
    AUTH_MECHANISM: Optional[str] = None
    RETRY_WRITE: Optional[bool] = True
    MAXIDLETIMEMS: Optional[int] = 10000


class AppAPISettings(CommonConfig):
    APP_NAME: Optional[str] = "Default App"
    VERSION: Optional[str] = "1.0"
    ENV: Optional[str] = "test"
    API_V1_URL: Optional[str] = "/api"
    EXPOSE_PORT: int | None = 8000
    TIMEZONE: Optional[str] = 'UTC'


class PipelineSettings(CommonConfig):
    # Batch sizes
    PIPELINE_SPLIT_BATCH_SIZE: int = 5
    PIPELINE_CONVERT_BATCH_SIZE: int = 5
    PIPELINE_UPLOAD_BATCH_SIZE: int = 10
    PIPELINE_EMBED_BATCH_SIZE: int = 1
    PIPELINE_INDEX_BATCH_SIZE: int = 50

    # Concurrency limits
    PIPELINE_CONVERT_WORKERS: int = 2
    PIPELINE_UPLOAD_WORKERS: int = 5
    PIPELINE_EMBED_WORKERS: int = 4
    PIPELINE_INDEX_WORKERS: int = 2
    PIPELINE_EMBED_MAX_CONCURRENT: int = 4

    # Queue sizes
    PIPELINE_CONVERT_QUEUE_SIZE: int = 10
    PIPELINE_UPLOAD_QUEUE_SIZE: int = 20
    PIPELINE_EMBED_QUEUE_SIZE: int = 30
    PIPELINE_INDEX_QUEUE_SIZE: int = 50

    # Timeouts (seconds)
    PIPELINE_CONVERT_TIMEOUT: int = 120
    PIPELINE_UPLOAD_TIMEOUT: int = 60
    PIPELINE_EMBED_TIMEOUT: int = 180
    PIPELINE_INDEX_TIMEOUT: int = 60

    # Retry configuration
    PIPELINE_MAX_RETRIES: int = 3
    PIPELINE_RETRY_DELAY_BASE: float = 1.0
    PIPELINE_RETRY_DELAY_MULTIPLIER: float = 2.0
    PIPELINE_RETRY_DELAY_MAX: float = 30.0

    # Image settings
    PIPELINE_IMAGE_DPI: int = 200
    PIPELINE_IMAGE_FORMAT: str = "PNG"


class Settings(CommonConfig):
    LLAMA_ENDPOINT_NAME: str = "databricks-meta-llama-3-1-70b-instruct"
    EXT_MODEL_PREFIX: str = "rica/rica/rica-proxy/"
    SECRET_KEY: str = "RENESAS_AI_TEAM_2024"
    REFRESH_TOKEN_EXPIRE_SECONDS: int = 60 * 60 * 24 * 30
    JWT_ALGO: str = "HS512"
    ACCESS_TOKEN_EXPIRE_SECONDS: int = 60 * 30
    API_PREFIX: str = "/api"
    DEBUG: bool = False
    # Authentication

    MAXIMUM_TITLE_COUNT: int = 0
    MAX_FILE_SIZE_MB: int = 20

    API_CLIENT_ID: str = "5f751fec-ad96-4e85-aa7c-7c3abb7f2e35"
    API_CLIENT_SECRET: str = os.environ.get("API_CLIENT_SECRET", "")
    SWAGGER_UI_CLIENT_ID: str = "5f751fec-ad96-4e85-aa7c-7c3abb7f2e35"
    # AAD_TENANT_ID: str = "53d82571-da19-47e4-9cb4-625a166a4a2a"

    AAD_INSTANCE: str = "https://login.microsoftonline.com"
    API_AUDIENCE: str = f"{API_CLIENT_ID}"
    MULTI_MAX: int = 6
    FIRST_SUPERUSER: str = "hai.la.ud@renesas.com"
    FIRST_SUPERUSER_PASSWORD: str = "Renesas123"
    REDIS_URI: Optional[
        str] = "redis://default:0UFLyjFMq3kV6uIpnS9kWBrfQKlSIu1J@redis-16346.c98.us-east-1-4.ec2.redns.redis-cloud.com:16346"
    REDIS_PASSWORD: Optional[str] = None
    ENVIRONMENT: str = "development"
    ALLOW_CORS: str = (
        "http://localhost:5173,https://globalrulewebuiapp-euh5cvb0hkbya8gz.westus2-01.azurewebsites.net/"
    )

    # Databricks Client ID on Azure
    EXT_AD_TENANT_ID: str = "53d82571-da19-47e4-9cb4-625a166a4a2a"
    EXT_AD_CLIENT_ID: str = "0fe63045-971f-47fd-ac08-314142ab8f0c"
    EXT_AD_CLIENT_SECRET: str = ""
    EXT_DATABRICKS_URL: str = "https://adb-4265374015564621.1.azuredatabricks.net"
    EXT_DATABRICKS_TOKEN: str = ""
    EXT_VERSION: Optional[List[str] | str] = None
    DATABRICKS_CLIENT_ID: str = "2ff814a6-3304-4ab8-85cb-cd0e6f879c1d"

    # File Converter Support
    POPPLER_PATH: Optional[str] = None

    # ElasticSearch Service
    ELASTICSEARCH_API_KEY: str = "bXJWb2ZKb0JKc09MUHJYdEViSk46cldQQ1FHZEpWcHJJSXNGeTdBMG92UQ=="
    ELASTICSEARCH_HOST: str = "https://es-ai-nonprod-colossus-001-bbba6d.es.eastus.azure.elastic.cloud:443"
    INDEX_NAME: str = "multimodal-model"

    # Pipeline Processing Feature Flag
    USE_PIPELINE_PROCESSING: bool = True  # Set to True to use new streaming pipeline
    PIPELINE_ROLLBACK: bool = False  # Emergency rollback to sequential processing
    REDIS_COUNTER_EXPIRE_SECONDS: int = 3600

    # Nested settings as fields
    api_app: AppAPISettings = Field(default_factory=AppAPISettings)
    mongodb: MongoDBSettings = Field(default_factory=MongoDBSettings)
    azure: AzureSettings = Field(default_factory=AzureSettings)
    databricks: DataBricksEndpoint = Field(default_factory=DataBricksEndpoint)
    pipeline: PipelineSettings = Field(default_factory=PipelineSettings)

    @model_validator(mode="after")
    def assign_endpoints(self):
        if type(self.EXT_VERSION) == str:
            ext_version_str = str(self.EXT_VERSION)
            try:
                self.EXT_VERSION = [v.strip() for v in ext_version_str.split(",")]
            except Exception:
                self.EXT_VERSION = [ext_version_str]

        if self.POPPLER_PATH is None:
            pdftoppm = shutil.which('pdftoppm')
            if pdftoppm:
                self.POPPLER_PATH = str(Path(pdftoppm).parent)

        return self


settings = Settings()

# Later, after changing .ment variables
