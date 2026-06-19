import asyncio
import json
from typing import Annotated, List, Tuple, Dict, Any
from io import BytesIO

from bson import ObjectId
from loguru import logger
from motor.motor_asyncio import AsyncIOMotorClient
from taskiq import Context, TaskiqDepends

from core import settings
from models.file import Files
from services.storage import DatabricksStorage
from services.ElasticSearchVector import get_elasticsearch_service
from services.VectorEmbedding import get_embedding_service
from services.FileConverter import get_converter_service
from services.taskiq.utils import CachedManager, SocketHelper
from caching.redis import normalize_redis_uri
from api.v1.socket import SOCKETIO_CHANNEL

import socketio
assert settings.REDIS_URI is not None
redis_mg = socketio.AsyncRedisManager(
        normalize_redis_uri(settings.REDIS_URI),
        write_only=True,
        channel=SOCKETIO_CHANNEL
    )

def _parse_file_metadata(metadata: str | dict) -> Files:
    if isinstance(metadata, str):
        return Files(**json.loads(metadata))
    return Files(**metadata)

async def _download_original_file(
    task_id: str,
    upload_path: str
) -> bytes:
    logger.info(f"[Task {task_id}] Downloading original file from: {upload_path}")

    volume = DatabricksStorage()

    try:
        file_bytes = await volume.download(upload_path)
        logger.info(f"[Task {task_id}] Downloaded original file: {len(file_bytes)} bytes")
        return file_bytes
    except Exception as e:
        logger.error(f"[Task {task_id}] Failed to download original file: {e}")
        raise


async def _convert_file_to_images(
    task_id: str,
    file_bytes: bytes,
    file_name: str
) -> Dict[str, Any]:
    from pathlib import Path

    logger.info(f"[Task {task_id}] Converting file to images: {file_name}")

    file_ext = Path(file_name).suffix.lower()
    logger.info(f"[Task {task_id}] File extension: {file_ext}")

    converter = get_converter_service()

    if file_ext in ['.ppt', '.pptx', '.doc', '.docx']:
        if not converter.is_libreoffice_available():
            error_msg = (
                f"LibreOffice is required to convert {file_ext} files. "
                f"Install it with: sudo apt-get install -y libreoffice"
            )
            logger.error(f"[Task {task_id}] {error_msg}")
            raise Exception(error_msg)

    result = await asyncio.to_thread(
        converter.convert_file_to_image_bytes,
        file_bytes=file_bytes,
        file_name=file_name,
        pages=None
    )

    if not result["success"]:
        logger.error(f"[Task {task_id}] Conversion failed: {result['error']}")
        raise Exception(f"File conversion failed: {result['error']}")

    logger.info(
        f"[Task {task_id}] Converted {result['total_pages']} pages to images"
    )
    return result


async def _upload_images_to_volume(
    task_id: str,
    images: List[Dict[str, Any]],
    target_path: str,
    batch_size: int = 20
) -> str:
    logger.info(
        f"[Task {task_id}] Uploading {len(images)} images to {target_path} "
        f"(batch size: {batch_size})"
    )

    volume = DatabricksStorage()
    total_uploaded = 0

    for batch_start in range(0, len(images), batch_size):
        batch_end = min(batch_start + batch_size, len(images))
        batch = images[batch_start:batch_end]

        logger.debug(
            f"[Task {task_id}] Uploading batch: images {batch_start + 1}-{batch_end}"
        )

        async def upload_single(img_data: Dict[str, Any]) -> bool:
            try:
                image_bytes = img_data['bytes'].getvalue()
                image_filename = f"page_{img_data['page_num']}.png"
                upload_path = f"{target_path}/{image_filename}"

                result = await volume.upload(
                    upload_file_path=upload_path,
                    file_body=image_bytes
                )

                if result.status:
                    return True
                else:
                    logger.error(
                        f"[Task {task_id}] Failed to upload page "
                        f"{img_data['page_num']}: {result.error}"
                    )
                    return False
            except Exception as e:
                logger.error(
                    f"[Task {task_id}] Upload error for page "
                    f"{img_data['page_num']}: {e}"
                )
                return False

        results = await asyncio.gather(
            *[upload_single(img_data) for img_data in batch],
            return_exceptions=True
        )

        for idx, result in enumerate(results):
            if isinstance(result, Exception):
                raise Exception(
                    f"Failed to upload page {batch[idx]['page_num']}: {str(result)}"
                )
            elif not result:
                raise Exception(
                    f"Failed to upload page {batch[idx]['page_num']}"
                )
            else:
                total_uploaded += 1

        logger.debug(f"[Task {task_id}] Batch complete: {len(batch)} images uploaded")

    logger.info(
        f"[Task {task_id}] All uploads complete: {total_uploaded}/{len(images)} images"
    )
    return target_path


async def _download_images_batch(
    task_id: str,
    upload_path: str,
    total_pages: int,
    max_concurrent: int = 15
) -> List[bytes | None]:
    logger.info(f"[Task {task_id}] Downloading {total_pages} images from Volume")

    volume = DatabricksStorage()

    async def download_single(page_num: int) -> bytes | None:
        image_path = f"{upload_path}/page_{page_num}.png"
        try:
            return await volume.download(image_path)
        except Exception as e:
            logger.error(f"[Task {task_id}] Failed to download page {page_num}: {e}")
            return None

    page_numbers = list(range(1, total_pages + 1))
    image_bytes_list = []

    for i in range(0, len(page_numbers), max_concurrent):
        chunk = page_numbers[i:i + max_concurrent]
        logger.debug(f"[Task {task_id}] Downloading pages {chunk[0]}-{chunk[-1]}")

        chunk_results = await asyncio.gather(
            *[download_single(page_num) for page_num in chunk]
        )
        image_bytes_list.extend(chunk_results)

    successful = sum(1 for img in image_bytes_list if img is not None)
    logger.info(f"[Task {task_id}] Downloaded {successful}/{total_pages} images")

    return image_bytes_list


async def _generate_embeddings(
    task_id: str,
    valid_images: List[Tuple[int, bytes]]
) -> Any:
    logger.info(
        f"[Task {task_id}] Generating embeddings for {len(valid_images)} images"
    )

    embedding_service = get_embedding_service()

    result = await embedding_service.generate_image_embeddings_batch(
        image_bytes_list=[img for _, img in valid_images],
        batch_size=1,
        max_concurrent=4
    )

    if result.success:
        logger.info(f"[Task {task_id}] Generated {len(result.embeddings or [])} embeddings")

    return result


def _prepare_elasticsearch_documents(
    file: Files,
    valid_images: List[Tuple[int, bytes]],
    embeddings: List
) -> List[dict]:
    documents = []

    dir_name = "document_files"
    directory_path = f"{DatabricksStorage().BASE_BUCKET}/{dir_name}"
    doc_path = f"{directory_path}/{file.id}"

    for idx, (original_idx, _) in enumerate(valid_images):
        page_num = original_idx + 1
        vectors = embeddings[idx]
        doc_id = f"{file.id}_page_{page_num}"
        volume_path = f"{doc_path}/page_{page_num}.png"

        doc_metadata = {
            "file_id": str(file.id),
            "file_name": file.file_name,
            "page_num": page_num,
            "upload_date": file.uploaded_at.isoformat() if hasattr(file, 'uploaded_at') else None,
            "author_id": str(file.author_id),
            "volume_path": volume_path
        }

        documents.append({
            "doc_id": doc_id,
            "vectors": vectors,
            "metadata": doc_metadata
        })

    return documents


async def _index_to_elasticsearch(task_id: str, documents: List[dict]) -> Any:
    logger.info(
        f"[Task {task_id}] Indexing {len(documents)} documents to Elasticsearch"
    )

    es_service = get_elasticsearch_service()
    result = await es_service.index_documents_bulk(documents)

    if result.success:
        logger.info(f"[Task {task_id}] ES indexed: {result.indexed_count} documents")

    return result


async def _update_file_status(
    file_id: str,
    status: str,
    user_id: str | None = None,
    **kwargs
):
    """
    Update file status in database and emit socket event to client.

    Args:
        file_id: The file ID to update
        status: New status (processing, completed, failed)
        user_id: User ID to emit to their room (optional)
        **kwargs: Additional fields to update (total_pages, folder_path, error, etc.)
    """
    mongo_client: AsyncIOMotorClient[Any] = AsyncIOMotorClient(settings.mongodb.MONGO_URI)
    db = mongo_client[settings.mongodb.DATABASE_NAME or "rica_ai"]

    update_data = {"status": status}
    update_data.update(kwargs)

    logger.info(f"Updating file status - file_id: {file_id}, status: {status}")

    try:
        object_id = ObjectId(file_id)
    except Exception as e:
        logger.error(f"Invalid ObjectId: {file_id}, error: {e}")
        mongo_client.close()
        return

    existing_doc = await db.files.find_one({"_id": object_id})
    logger.info(f"Found existing document: {existing_doc}")

    result = await db.files.update_one(
        {"_id": object_id},
        {"$set": update_data}
    )

    # Clear old cached
    logger.info(f"clear cached for key: {file_id}")
    cached_invalidator = CachedManager()
    await cached_invalidator.delete_cached_objects(prefix="file", objects=[file_id])

    logger.info(
        f"Update result - matched: {result.matched_count}, "
        f"modified: {result.modified_count}"
    )

    mongo_client.close()

    # Emit socket event to user's room
    if user_id and status != "processing":
        user_room = f"user_{user_id}"

        # Prepare emit data with file details
        emit_data = {
            "file_id": file_id,
            "file_name": (existing_doc or {}).get("file_name"),
            "status": status,
            "total_pages": (existing_doc or {}).get("total_pages"),
            "size": (existing_doc or {}).get("size"),
            "extension": (existing_doc or {}).get("extension"),
            "folder_path": (existing_doc or {}).get("folder_path"),
        }
        emit_data.update(kwargs)

        async with SocketHelper() as socket:
            await socket.emit(
                "file_status",
                {"data": emit_data},
                room=user_room
            )

    # await call_webhook(
    #     file_id,
    #     status,
    #     user_id
    # )

async def sync_file_conversion_and_indexing(
    metadata: str | dict,
    context: Annotated[Context, TaskiqDepends()],
    user_id: str | None = None
) -> bool:
    """
    Process file: convert to images, generate embeddings, and index to Elasticsearch.

    Args:
        metadata: File metadata (dict or JSON string)
        context: Taskiq context
        user_id: User ID for real-time status updates via socket room (optional)
    """
    task_id = context.message.task_id

    try:
        file = _parse_file_metadata(metadata)
        file_id = str(file.id)
        # Use provided user_id or fall back to file's author_id
        notify_user_id = user_id or str(file.author_id)

        logger.info(f"[Task {task_id}] Starting file processing: {file.file_name}")

        dir_name = "document_files"
        directory_path = f"{DatabricksStorage().BASE_BUCKET}/{dir_name}"
        images_target_path = f"{directory_path}/{file_id}"
        original_file_path = f"{directory_path}/{file_id}/{file_id}.{file.extension}"

        logger.info(f"[Task {task_id}] Step 1: Downloading original file")
        file_bytes = await _download_original_file(task_id, original_file_path)

        logger.info(f"[Task {task_id}] Step 2: Converting file to images")
        conversion_result = await _convert_file_to_images(
            task_id, file_bytes, file.file_name
        )

        logger.info(f"[Task {task_id}] Step 3: Uploading images to volume")
        upload_path = await _upload_images_to_volume(
            task_id,
            conversion_result["images"],
            images_target_path
        )

        total_pages = conversion_result["total_pages"]
        await _update_file_status(
            file_id,
            "processing",
            user_id=notify_user_id,
            total_pages=total_pages,
            folder_path=upload_path
        )

        logger.info(f"[Task {task_id}] Step 4: Downloading images for embedding")
        image_bytes_list = await _download_images_batch(
            task_id, upload_path, total_pages
        )

        valid_images = [
            (i, img) for i, img in enumerate(image_bytes_list) if img is not None
        ]

        if not valid_images:
            await _update_file_status(file_id, "failed", user_id=notify_user_id, error="No valid images")
            return False

        logger.info(
            f"[Task {task_id}] Step 5: Generating embeddings for "
            f"{len(valid_images)} valid images"
        )
        embedding_result = await _generate_embeddings(task_id, valid_images)

        if not embedding_result.success:
            await _update_file_status(
                file_id, "failed", user_id=notify_user_id, error="Embedding generation failed"
            )
            return False

        logger.info(f"[Task {task_id}] Step 6: Indexing to Elasticsearch")
        documents = _prepare_elasticsearch_documents(
            file, valid_images, embedding_result.embeddings
        )

        index_result = await _index_to_elasticsearch(task_id, documents)

        if not index_result.success:
            await _update_file_status(
                file_id, "failed", user_id=notify_user_id, error="Elasticsearch indexing failed"
            )
            return False

        await _update_file_status(file_id, "completed", user_id=notify_user_id)

        logger.info(f"[Task {task_id}] Successfully completed processing")
        return True

    except Exception as e:
        logger.exception(f"[Task {task_id}] Failed to process file: {e}")
        try:
            await _update_file_status(file_id, "failed", user_id=notify_user_id, error=str(e))
        except Exception:
            pass
        return False


async def sync_generate_and_index_vectors_from_images(
    metadata: str | dict,
    context: Annotated[Context, TaskiqDepends()],
    user_id: str | None = None
) -> bool:
    """
    Generate embeddings from existing images and index to Elasticsearch.

    Args:
        metadata: File metadata (dict or JSON string)
        context: Taskiq context
        user_id: User ID for real-time status updates via socket room (optional)
    """
    task_id = context.message.task_id

    try:
        file = _parse_file_metadata(metadata)
        file_id = str(file.id)
        # Use provided user_id or fall back to file's author_id
        notify_user_id = user_id or str(file.author_id)

        logger.info(f"[Task {task_id}] Processing file: {file.file_name}")

        try:
            assert file.folder_path is not None and file.total_pages is not None
            image_bytes_list = await _download_images_batch(
                task_id=task_id,
                upload_path=file.folder_path,
                total_pages=file.total_pages
            )
        except Exception as e:
            await _update_file_status(file_id, "failed", user_id=notify_user_id, error=str(e))
            return False

        valid_images = [
            (i, img) for i, img in enumerate(image_bytes_list) if img is not None
        ]

        if not valid_images:
            await _update_file_status(file_id, "failed", user_id=notify_user_id, error="No valid images")
            return False

        logger.info(
            f"[Task {task_id}] Processing "
            f"{len(valid_images)}/{file.total_pages} valid images"
        )

        embedding_result = await _generate_embeddings(task_id, valid_images)

        if not embedding_result.success:
            await _update_file_status(file_id, "failed", user_id=notify_user_id, error="Embedding failed")
            return False

        documents = _prepare_elasticsearch_documents(
            file, valid_images, embedding_result.embeddings
        )

        index_result = await _index_to_elasticsearch(task_id, documents)

        if not index_result.success:
            await _update_file_status(file_id, "failed", user_id=notify_user_id, error="Indexing failed")
            return False

        await _update_file_status(file_id, "completed", user_id=notify_user_id)

        logger.info(f"[Task {task_id}] Successfully completed processing")
        return True

    except Exception as e:
        logger.exception(f"[Task {task_id}] Failed to generate and index vectors: {e}")
        try:
            await _update_file_status(file_id, "failed", user_id=notify_user_id, error=str(e))
        except Exception:
            pass
        return False
