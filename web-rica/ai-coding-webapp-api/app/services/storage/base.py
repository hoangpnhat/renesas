from abc import ABC, abstractmethod
from schemas.task import UploadStage

class BaseStorageProvider(ABC):
    BASE_BUCKET: str

    @abstractmethod
    async def create_target_directory(self, dir_name: str) -> str:
        """Create a target directory and return the full path (optional)"""
        pass
    @abstractmethod
    async def upload(self, upload_file_path: str, file_body: bytes) -> UploadStage:
        """Upload file and return upload status."""
        pass
    @abstractmethod
    async def download(self, file_path: str):
        """Download/stream file."""
        pass