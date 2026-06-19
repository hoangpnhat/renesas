from abc import ABC, abstractmethod
from typing import Optional

import httpx
from fastapi.encoders import jsonable_encoder
from injector import singleton

from core.loguru import logger
from const.chat import DATABRICKS_PROMPT_TIMEOUT
from const.chat import DBX_DOWNLOAD_ENDPOINT_API
from core.config import settings
from exceptions.https_exception import DataBricksException
from schemas.chat import (
    PromptRequestDataBricks,
    PromptResponseDataBricks,
)


# factory pattern and abstract class
class DataBricks(ABC):
    databricks_url: str
    databricks_token: str | None
    http_client: httpx.AsyncClient

    @abstractmethod
    def __init__(self):
        pass


@singleton
class ChatPromptDatabricks(DataBricks):
    http_client: httpx.AsyncClient

    def __init__(self):
        self.databricks_token = settings.databricks.DATABRICKS_TOKEN
        self.databricks_url = f"https://{settings.databricks.DATABRICKS_SERVER_HOSTNAME}/serving-endpoints/{settings.databricks.DATABRICKS_ENDPOINT_NAME}/invocations"
        self.http_client = httpx.AsyncClient(
            headers={
                "Authorization": f"Bearer {settings.databricks.DATABRICKS_TOKEN}",
                "Content-type": "application/json",
                "Connection": "keep-alive",
            }
        )

    async def prompt(
            self, input_prompt: PromptRequestDataBricks, url: Optional[str] = None
    ) -> PromptResponseDataBricks:
        logger.info(f"Input Prompt: {input_prompt}")
        if not url:
            url = self.databricks_url
        result = await self.http_client.post(
            url=url,
            json=jsonable_encoder(input_prompt),
            timeout=DATABRICKS_PROMPT_TIMEOUT,
        )
        if result.is_success:
            json_data = result.json()

            response = PromptResponseDataBricks(
                response=json_data.get("response", ""),
                retrieved_docs=json_data.get("retrieved_docs", []),
            )
            logger.debug(f"Response from databricks: {response}")
            return response
        raise DataBricksException(error_code=result.status_code, message=str(result.json()))


class DownloadFileDatabricks(DataBricks):
    http_client: httpx.AsyncClient

    def __init__(self):
        self.databricks_token = settings.databricks.DATABRICKS_TOKEN
        self.databricks_url = f"https://{settings.databricks.DATABRICKS_SERVER_HOSTNAME}/serving-endpoints/{settings.databricks.DATABRICKS_ENDPOINT_NAME}/invocations"
        self.http_client = httpx.AsyncClient(
            headers={
                "Authorization": f"Bearer {settings.databricks.DATABRICKS_TOKEN}",
                "Content-type": "application/json",
                "Connection": "keep-alive",
            }
        )

    async def download_file(self, file_path: str):
        # time counter
        async with self.http_client.stream(
                "GET",
                f'{settings.databricks.DATABRICKS_URL}{DBX_DOWNLOAD_ENDPOINT_API}{file_path.removeprefix("dbfs:")}',
        ) as response:
            response.raise_for_status()

            async for chunk in response.aiter_bytes():
                yield chunk
