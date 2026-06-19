#  Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.

import base64
import json
import os
import re
import shutil
from datetime import datetime
from pathlib import Path
from typing import Any, Dict, List, TypeVar, Union
from zoneinfo import ZoneInfo

from fastapi import UploadFile, status, HTTPException
from fastapi.encoders import jsonable_encoder
from httpx import Response
from loguru import logger
from odmantic import ObjectId
from pydantic import BaseModel

from caching.redis import RedisClient
from const.enums import SortOrder
from utils.handle_exception import ApplicationException

BaseModelType = TypeVar("BaseModelType", bound=BaseModel)


def datetime_now_sec() -> datetime:
    return datetime.now(ZoneInfo("UTC"))


def datetime_now_to_unix() -> int:
    dt = datetime_now_sec()
    return int(dt.timestamp())


def datetime_to_unix_ms(dt: datetime) -> int:
    """
    Convert a UTC datetime to milliseconds since Unix epoch.
    Naive datetimes are assumed UTC, consistent with datetime_now_sec().
    """
    if dt.tzinfo is None:
        dt = dt.replace(tzinfo=ZoneInfo("UTC"))
    return int(dt.timestamp() * 1000)


def empty_response(filters: dict[str, Any]) -> dict[str, Any]:
    return {
        "data": [],
        "pagination": {
            "page": filters.get('page', 1),
            "limit": filters.get("page_size", 6),
            "total": 0,
        },
    }


def base64Str(original_string: str) -> str:
    # Encode the string to bytes
    bytes_string = original_string.encode("utf-8")

    # Encode bytes to Base64
    base64_encoded = base64.b64encode(bytes_string)

    # Convert Base64 bytes back to string
    base64_string: str = base64_encoded.decode("utf-8")
    return base64_string


def normalize_id(id_: str | ObjectId) -> ObjectId:
    if isinstance(id_, str):
        return ObjectId(id_)
    return id_


def normalize_model_to_dict(
        model: Union[BaseModelType, Dict[str, Any]]
) -> Dict[str, Any]:
    if isinstance(model, dict):
        return model
    else:
        return model.model_dump(exclude_unset=True)


def eliminate_key_from_dict(
        keys_to_exclude: Union[List[str], str], original_dict: Dict[str, Any]
) -> Dict[str, Any]:
    new_dict: Dict[str, Any] = original_dict.copy()
    if isinstance(keys_to_exclude, str):
        new_dict.pop(keys_to_exclude, None)
    else:
        for key in keys_to_exclude:
            if key in new_dict:
                new_dict.pop(key, None)
    return new_dict


def set_key_redis_name(prefix: str, _id: str):
    return f"{prefix}-{_id}"


async def store_item_in_redis(
        redis: RedisClient, *, item: Any, key_name: str
):
    item_data = jsonable_encoder(
        item, exclude={"id"}
    )  # Convert to a JSON-serializable format
    await redis.set(key_name, json.dumps(item_data), expire=3600)


async def get_item_from_redis(item_id: str, redis: RedisClient):
    return await redis.get(item_id)


class UploadHelper:

    @staticmethod
    def get_base_and_ext(file_name: str) -> tuple[str, str]:
        return os.path.splitext(file_name)

    @staticmethod
    def generate_incremented_name(base: str, ext: str, existing_names: list[str]) -> str:
        suffix_pattern = re.compile(rf"^{re.escape(base)}(?: \((\d+)\))*{re.escape(ext)}$")
        max_suffix = 0

        for name in existing_names:
            matches = suffix_pattern.findall(name)
            nums = [int(num) for num in matches if num.isdigit()]
            if nums:
                max_suffix = max(max_suffix, max(nums))

        return f"{base} ({max_suffix + 1}){ext}"

    @staticmethod
    def strip_immutable_fields(obj_in_data: dict[str, Any], fields: list[str]) -> dict[str, Any]:
        for field in fields:
            obj_in_data.pop(field, None)
        return obj_in_data

    @staticmethod
    async def extract_compressed_file(file_zip: UploadFile, extension: str, extract_to: str) -> list[str]:
        import zipfile
        import tarfile
        import gzip
        import io

        extracted_files = []

        try:

            # Read file bytes from UploadFile
            file_bytes = await file_zip.read()

            if extension.endswith('.zip'):
                with zipfile.ZipFile(io.BytesIO(file_bytes)) as zf:
                    for info in zf.infolist():
                        if info.flag_bits & 0x1:
                            raise ApplicationException(
                                error_key="INVALID_CONTENT",
                                status_code=status.HTTP_400_BAD_REQUEST,
                                detail="Password-protected ZIP files are not supported"
                            )
                    zf.extractall(extract_to)
                    extracted_files = [
                        os.path.join(extract_to, name)
                        for name in zf.namelist()
                        if not name.endswith('/')
                    ]

            elif extension.endswith(('.tar', '.tar.gz', '.tar.bz2')):
                mode = 'r:gz' if extension.endswith('.tar.gz') else 'r:bz2' if extension.endswith('.tar.bz2') else 'r'
                with tarfile.open(fileobj=io.BytesIO(file_bytes), mode=mode) as tar_ref:
                    tar_ref.extractall(extract_to)
                    extracted_files = [
                        os.path.join(extract_to, member.name)
                        for member in tar_ref.getmembers()
                        if member.isfile()
                    ]

            elif extension.endswith('.gz') and not extension.endswith('.tar.gz'):
                output_file = os.path.join(extract_to, Path(file_zip.filename or "").stem)
                with gzip.open(io.BytesIO(file_bytes), 'rb') as gz_file:
                    with open(output_file, 'wb') as out_file:
                        shutil.copyfileobj(gz_file, out_file)
                extracted_files = [output_file]

            else:
                raise ApplicationException(
                    error_key="INVALID_MIME_TYPE",
                    status_code=status.HTTP_400_BAD_REQUEST,
                    detail=f"Unsupported file extension: {extension}"
                )

        except (zipfile.BadZipFile, tarfile.ReadError, OSError) as e:
            logger.error(f"Failed to extract compressed file {file_zip.filename}: {str(e)}")
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail=f"Failed to extract compressed file: {str(e)}"
            )
        except ApplicationException:
            raise
        except Exception as e:
            logger.error(f"Unexpected error extracting {file_zip.filename}: {str(e)}")
            raise ApplicationException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail="Unexpected error during file extraction"
            )

        return extracted_files

    @staticmethod
    async def temp_save_uploaded_file(file: UploadFile, temp_dir: str) -> str:
        file_path = os.path.join(temp_dir, file.filename or "")

        try:
            with open(file_path, 'wb') as buffer:
                content = await file.read()
                buffer.write(content)
            return file_path
        except Exception as e:
            logger.error(f"Failed to temp save uploaded file {file.filename}: {str(e)}")
            raise ApplicationException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail="Failed to temp save uploaded file"
            )


def count_words(text):
    # Convert text to lowercase to handle case sensitivity
    text = text.lower()

    # Use regular expressions to remove punctuation and split the text into words
    words = re.findall(r"\b\w+\b", text)

    # Use Counter to count occurrences of each word
    return len(words)


def get_title_from_llama(new_title: Response):
    try:
        return new_title.json()["choices"][0]["message"]["content"]
    except Exception as e:
        return None


def build_file_path(file: Any) -> str:
    """folder_path/file_id.extension: e.g /Volumes/ai_dev/ai_dev_coding_bronze/multimodal_rag/document_files/693bd4618d5382d39724f8b7/693bd4618d5382d39724f8b7.pdf"""

    return (
        f"{file.folder_path}/"
        f"{file.id}.{file.extension}"
    )
