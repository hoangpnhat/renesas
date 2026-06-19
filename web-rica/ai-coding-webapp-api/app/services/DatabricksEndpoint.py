#  Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
from typing import Dict, Optional

from core.config import settings
from schemas.http.ModelsFromDatabricks import ModelFromDatabricksResponse
from .HttpClient import JsonHttpClient, JsonResponseParser


class DatabricksEndpoint(JsonHttpClient):
    def __init__(self, base_url: Optional[str] = None, custom_token: Optional[str] = None,
                 custom_headers: Optional[Dict[str, str]] = None):
        databricks_headers = {
            "User-Agent": "Databricks-Python-Client/1.0",
            "Accept": "application/json",
        }
        self.model_list_endpoint = settings.databricks.DATABRICKS_MODEL_LIST_ENDPOINT
        if custom_headers:
            databricks_headers.update(custom_headers)
        super().__init__(
            base_url=(settings.databricks.DATABRICKS_URL or "") if not base_url else base_url,
            default_headers=databricks_headers,
            timeout=120,
            response_parser=JsonResponseParser(),
            token=settings.databricks.DATABRICKS_TOKEN if not custom_token else custom_token,
            auth_scheme="Bearer",
        )

    async def get_models(self) -> ModelFromDatabricksResponse:
        response = await self.get(self.model_list_endpoint)
        return ModelFromDatabricksResponse.model_validate(response)
