from typing import Any

import httpx

from core.config import settings
from schemas.task import UploadStage
from .base import BaseStorageProvider
from ..HttpClient import JsonHttpClient, NoContentResponseParser


class DatabricksStorage(JsonHttpClient, BaseStorageProvider):
    def __init__(self):
        databricks_base_url = f"https://{settings.databricks.DATABRICKS_SERVER_HOSTNAME}/api/2.0/fs"
        super().__init__(
            base_url=databricks_base_url,
            response_parser=NoContentResponseParser(),
            token=settings.databricks.DATABRICKS_TOKEN,
            auth_scheme="Bearer"
        )
        self.BASE_BUCKET = f"/Volumes/{settings.databricks.DATABRICKS_CATALOG}/{settings.databricks.DATABRICKS_SCHEMA}/{settings.databricks.DATABRICKS_BUCKET}"

    async def create_target_directory(self, dir_name: str = 'input_files') -> str:
        if dir_name.startswith('/Volumes'):
            directory_path = dir_name
        else:
            directory_path = f"{self.BASE_BUCKET}/{dir_name}"
        await self.put(endpoint=f"directories{directory_path}")
        return directory_path

    async def streaming_download(self, file_name: str, chunk_size: int = 64 * 1024):
        url = self._build_url(f"files/{file_name}")
        async with httpx.AsyncClient(timeout=300, follow_redirects=True) as client:
            async with client.stream("GET", url, headers=self.headers) as response:
                response.raise_for_status()
                async for chunk in response.aiter_bytes(chunk_size):
                    yield chunk

    async def download(self, file_name: str) -> bytes:
        endpoint = f"files{file_name}"
        url = self._build_url(endpoint)

        # Download returns binary data
        async with httpx.AsyncClient(timeout=self.timeout) as client:
            response = await client.get(
                url=url,
                headers=self.headers
            )
            response.raise_for_status()
            return response.content

    async def upload(self, file_body: bytes, upload_file_path: str) -> UploadStage:  # type: ignore[override]
        """Upload file to Databricks storage (returns 204 No Content)"""
        try:
            endpoint = f"files{upload_file_path}"
            # Use application/octet-stream
            headers = {"Content-Type": "application/octet-stream"}
            await self.put(
                endpoint=endpoint,
                data=file_body,
                headers=headers
            )
            return UploadStage(
                upload_path=upload_file_path
            )
        except Exception as e:
            return UploadStage(
                status=False,
                error=str(e)
            )

    async def health_check(self) -> Any:
        pass
