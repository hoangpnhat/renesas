"""
Volume Uploader Stage

Stage 3: Upload images to Databricks Volume.
I/O bound - can have more workers.
"""

import asyncio
from loguru import logger

from services.storage import DatabricksStorage
from .base import BasePipelineStage, StageResult
from ..models import ImageBatch, UploadedBatch, UploadedPage, PageStatus


class VolumeUploaderStage(BasePipelineStage[ImageBatch, UploadedBatch]):
    """
    Stage 3: Upload images to Databricks Volume.
    I/O bound - can have more workers.
    """

    stage_name = "uploader"
    input_status = PageStatus.UPLOADING
    output_status = PageStatus.EMBEDDING

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._storage = None

    @property
    def storage(self):
        if self._storage is None:
            self._storage = DatabricksStorage()
        return self._storage

    @property
    def num_workers(self) -> int:
        return self.config.upload_workers

    @property
    def timeout(self) -> int:
        return self.config.upload_timeout

    async def process_batch(self, batch: ImageBatch) -> StageResult:
        """Upload batch of images to volume"""
        try:
            page_nums = [img.page_num for img in batch.images]
            logger.info(f"[{batch.file_id}] Uploading {len(batch)} images: pages {page_nums}")

            base_path = f"{self.storage.BASE_BUCKET}/document_files/{batch.file_id}"
            uploaded_pages = []
            failed_pages = []

            # Upload concurrently within batch
            async def upload_single(image_data):
                try:
                    upload_path = f"{base_path}/{image_data.filename}"
                    logger.debug(f"[{batch.file_id}] Uploading page {image_data.page_num} to {upload_path}")
                    result = await self.storage.upload(
                        upload_file_path=upload_path,
                        file_body=image_data.image_bytes
                    )
                    if result.status:
                        logger.debug(f"[{batch.file_id}] Uploaded page {image_data.page_num} successfully")
                        return UploadedPage(
                            page_num=image_data.page_num,
                            volume_path=upload_path
                        )
                    else:
                        logger.error(
                            f"[{batch.file_id}] Upload failed for page {image_data.page_num}: {result.error}"
                        )
                        return None
                except Exception as e:
                    logger.error(f"[{batch.file_id}] Upload exception for page {image_data.page_num}: {e}")
                    return None

            results = await asyncio.gather(
                *[upload_single(img) for img in batch.images],
                return_exceptions=True
            )

            for img, result in zip(batch.images, results):
                if isinstance(result, BaseException) or result is None:
                    failed_pages.append(img.page_num)
                else:
                    uploaded_pages.append(result)

            if failed_pages:
                logger.warning(f"[{batch.file_id}] Failed to upload pages: {failed_pages}")

            if not uploaded_pages:
                return StageResult(
                    success=False,
                    error="All uploads failed",
                    failed_pages=[img.page_num for img in batch.images]
                )

            output = UploadedBatch(
                file_id=batch.file_id,
                batch_id=batch.batch_id,
                pages=uploaded_pages
            )

            logger.info(f"[{batch.file_id}] Uploaded {len(uploaded_pages)} images")

            return StageResult(
                success=True,
                output=output,
                failed_pages=failed_pages
            )

        except Exception as e:
            logger.exception(f"[{batch.file_id}] Upload batch failed: {e}")
            return StageResult(
                success=False,
                error=str(e),
                failed_pages=[img.page_num for img in batch.images]
            )
