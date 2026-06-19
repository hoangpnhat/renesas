"""
Embedding Generator Stage

Stage 4: Generate vector embeddings for images.
API bound - limited by endpoint concurrency.
"""

import asyncio
from loguru import logger

from services.storage import DatabricksStorage
from services.VectorEmbedding import get_embedding_service, VectorEmbeddingService
from services.EndpointWarmup import get_warmup_service
from .base import BasePipelineStage, StageResult
from ..models import UploadedBatch, EmbeddedBatch, EmbeddedPage, PageStatus


class EmbeddingGeneratorStage(BasePipelineStage[UploadedBatch, EmbeddedBatch]):
    """
    Stage 4: Generate vector embeddings for images.
    API bound - limited by endpoint concurrency.
    """

    stage_name = "embedder"
    input_status = PageStatus.EMBEDDING
    output_status = PageStatus.INDEXING

    def __init__(self, *args, file_metadata: dict | None = None, **kwargs):
        super().__init__(*args, **kwargs)
        self._embedding_service: VectorEmbeddingService | None = None
        self._storage: DatabricksStorage | None = None
        self.file_metadata = file_metadata or {}

    async def start(self):
        """Start workers after waiting for endpoint warmup"""
        # Wait for endpoint warmup BEFORE starting workers
        # This ensures endpoint is ready before any embedding requests
        warmup_service = get_warmup_service()
        logger.info(f"[{self.stage_name}] Waiting for endpoint warmup before starting workers...")
        warmup_ready = await warmup_service.wait_for_warmup()
        if warmup_ready:
            logger.info(f"[{self.stage_name}] Endpoint warmup successful (status: {warmup_service.status.value}), starting workers")
        else:
            logger.warning(f"[{self.stage_name}] Endpoint warmup not ready (status: {warmup_service.status.value}), starting workers anyway")

        # Now start workers
        await super().start()

    @property
    def embedding_service(self):
        if self._embedding_service is None:
            self._embedding_service = get_embedding_service()
        return self._embedding_service

    @property
    def storage(self):
        if self._storage is None:
            self._storage = DatabricksStorage()
        return self._storage

    @property
    def num_workers(self) -> int:
        return self.config.embed_workers

    @property
    def timeout(self) -> int:
        return self.config.embed_timeout

    async def process_batch(self, batch: UploadedBatch) -> StageResult:
        """Generate embeddings for uploaded images - process all pages concurrently"""
        try:
            logger.info(f"[{batch.file_id}] Starting embedding for {len(batch.pages)} pages: {[p.page_num for p in batch.pages]}")

            embedded_pages = []
            failed_pages = []

            # Download all images concurrently first
            async def download_image(page):
                try:
                    logger.debug(f"[{batch.file_id}] Downloading page {page.page_num} from {page.volume_path}")
                    image_bytes = await self.storage.download(page.volume_path)
                    if image_bytes is None or len(image_bytes) == 0:
                        logger.error(f"[{batch.file_id}] Empty download for page {page.page_num}")
                        return page.page_num, None, "Empty download"
                    logger.debug(f"[{batch.file_id}] Downloaded page {page.page_num}: {len(image_bytes)} bytes")
                    return page.page_num, image_bytes, None
                except Exception as e:
                    logger.error(f"[{batch.file_id}] Download failed for page {page.page_num}: {e}")
                    return page.page_num, None, str(e)

            # Download all images concurrently
            download_results = await asyncio.gather(
                *[download_image(page) for page in batch.pages],
                return_exceptions=True
            )

            # Collect successful downloads
            images_to_embed = []
            page_map = {}  # page_num -> volume_path

            for page, result in zip(batch.pages, download_results):
                if isinstance(result, BaseException):
                    logger.error(f"[{batch.file_id}] Download exception for page {page.page_num}: {result}")
                    failed_pages.append(page.page_num)
                    continue

                page_num, image_bytes, error = result
                if error or image_bytes is None:
                    logger.error(f"[{batch.file_id}] Download failed for page {page_num}: {error}")
                    failed_pages.append(page_num)
                    continue

                images_to_embed.append((page_num, image_bytes))
                page_map[page_num] = page.volume_path

            if not images_to_embed:
                error_msg = f"All {len(batch.pages)} downloads failed"
                logger.error(f"[{batch.file_id}] {error_msg}")
                return StageResult(
                    success=False,
                    error=error_msg,
                    failed_pages=[p.page_num for p in batch.pages]
                )

            logger.info(f"[{batch.file_id}] Downloaded {len(images_to_embed)}/{len(batch.pages)} images, generating embeddings...")

            # Generate embeddings for all downloaded images at once
            image_bytes_list = [img[1] for img in images_to_embed]
            page_nums = [img[0] for img in images_to_embed]

            try:
                result = await self.embedding_service.generate_image_embeddings_batch(
                    image_bytes_list=image_bytes_list,
                    batch_size=self.config.embed_batch_size,
                    max_concurrent=self.config.embed_max_concurrent
                )

                if result.success and result.embeddings:
                    logger.info(f"[{batch.file_id}] Got {len(result.embeddings)} embeddings")
                    for page_num, embedding in zip(page_nums, result.embeddings):
                        if embedding and len(embedding) > 0:
                            embedded_pages.append(EmbeddedPage(
                                page_num=page_num,
                                volume_path=page_map[page_num],
                                embedding=embedding
                            ))
                        else:
                            logger.warning(f"[{batch.file_id}] Empty embedding for page {page_num}")
                            failed_pages.append(page_num)
                else:
                    error_msg = result.error or "Embedding service returned failure"
                    logger.error(f"[{batch.file_id}] Embedding failed: {error_msg}")
                    failed_pages.extend(page_nums)

            except Exception as e:
                logger.exception(f"[{batch.file_id}] Embedding service exception: {e}")
                failed_pages.extend(page_nums)

            if not embedded_pages:
                return StageResult(
                    success=False,
                    error="All embeddings failed",
                    failed_pages=[p.page_num for p in batch.pages]
                )

            output = EmbeddedBatch(
                file_id=batch.file_id,
                batch_id=batch.batch_id,
                pages=embedded_pages,
                file_metadata=self.file_metadata
            )

            logger.info(f"[{batch.file_id}] Embedding complete: {len(embedded_pages)} success, {len(failed_pages)} failed")

            return StageResult(
                success=True,
                output=output,
                failed_pages=failed_pages
            )

        except Exception as e:
            logger.exception(f"[{batch.file_id}] Embedding batch failed: {e}")
            return StageResult(
                success=False,
                error=str(e),
                failed_pages=[p.page_num for p in batch.pages]
            )
