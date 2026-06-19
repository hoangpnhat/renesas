import asyncio
from pathlib import Path

from fastapi import UploadFile as RawFile

from const.upload import FILE_ALLOWED_MIME_TYPES, PROMPT_ALLOWED_EXTENSIONS, PROMPT_ALLOWED_MIME_TYPES
from core import settings
from utils.handle_exception import ApplicationException
from .base import BaseValidator, ValidationResult
from .models import FileValidateProperties


class FileBusinessRuleValidator(BaseValidator):
    def __init__(self, file: RawFile):
        self.file = file

    async def validate_mime_type(self) -> tuple[str, str]:
        file_mime = self.file.content_type
        if not file_mime or file_mime not in FILE_ALLOWED_MIME_TYPES:
            raise ApplicationException(
                error_key="INVALID_MIME_TYPE",
                status_code=400,
                detail=f"Invalid file type. Allowed: {', '.join(FILE_ALLOWED_MIME_TYPES.keys())} ---> got {file_mime}"
            )
        return file_mime, FILE_ALLOWED_MIME_TYPES[file_mime]

    async def validate_file_size(self) -> int:
        size = self.file.size

        if size is None or size <= 0:
            raise ApplicationException(
                error_key="INVALID_FILE_SIZE",
                status_code=400,
                detail="File is empty"
            )

        if size > settings.MAX_FILE_SIZE_MB * 1024 * 1024:
            raise ApplicationException(
                error_key="INVALID_FILE_SIZE",
                status_code=400,
                detail=f"File too large, accepted max size is {settings.MAX_FILE_SIZE_MB} MB"
            )
        return size

    async def run(self) -> ValidationResult:
        # extension and file size
        (mime, extension), file_size = await asyncio.gather(
            self.validate_mime_type(),
            self.validate_file_size()
        )
        return ValidationResult(
            is_valid=True,
            metadata=FileValidateProperties(
                mime_type=mime,
                file_extension=extension,
                size=file_size)
        )


class PromptBusinessRuleValidator(FileBusinessRuleValidator):
    def __init__(self, file: RawFile, **kwargs):
        super().__init__(file=file)

    async def validate_mime_type(self) -> tuple[str, str]:
        file_mime = self.file.content_type
        file_name = self.file.filename or ""
        ext = Path(file_name).suffix.lower()

        if ext not in PROMPT_ALLOWED_EXTENSIONS:
            raise ApplicationException(
                error_key="INVALID_FILE_TYPE",
                status_code=400,
                detail=f"Invalid file extension. Allowed: {', '.join(PROMPT_ALLOWED_EXTENSIONS.keys())} ---> got '{ext}'"
            )

        extension = PROMPT_ALLOWED_EXTENSIONS[ext]
        return file_mime or "", extension

    async def validate_file_size(self) -> int:
        size = self.file.size
        if size is None or size <= 0:
            raise ApplicationException(
                error_key="INVALID_FILE_SIZE",
                status_code=400,
                detail="File is empty"
            )
        max_mb = getattr(settings, "MAX_PROMPT_FILE_SIZE_MB", 5)
        if size > max_mb * 1024 * 1024:
            raise ApplicationException(
                error_key="INVALID_FILE_SIZE",
                status_code=400,
                detail=f"Prompt file too large, max allowed is {max_mb} MB"
            )
        return size