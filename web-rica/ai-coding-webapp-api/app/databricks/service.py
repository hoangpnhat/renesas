from typing import List, Optional
import httpx
from schemas.chat import PromptRequestDataBricks, PromptResponseDataBricks
from .session import ChatPromptDatabricks, DownloadFileDatabricks
from injector import singleton, inject
from injection import AppDepends, injector
from core.config import settings
from const.chat import DBX_DOWNLOAD_ENDPOINT_API
import time


@singleton
class DataBricksService:
    # INFO: multiple endpoints support
    prompt_dbx: ChatPromptDatabricks
    download_dbx: DownloadFileDatabricks

    # title_dbx: DatabricksConnection
    # other_dbx: DatabricksConnection
    def __init__(
        self,
        prompt_dbx: ChatPromptDatabricks = AppDepends(ChatPromptDatabricks),
        download_dbx: DownloadFileDatabricks = AppDepends(DownloadFileDatabricks),
    ):
        self.prompt_dbx = prompt_dbx
        self.download_dbx = download_dbx

    async def prompt_message(
        self, input_prompt: PromptRequestDataBricks, url: Optional[str]=None
    ) -> PromptResponseDataBricks:
        response = await self.prompt_dbx.prompt(input_prompt, url)
        return response

    async def download_file(self, file_path: str):
        async for chunk in self.download_dbx.download_file(file_path):
            yield chunk


injector.create_object(DataBricksService)
