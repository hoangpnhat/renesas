"""
Elasticsearch Indexer Stage

Stage 5: Index embeddings to Elasticsearch.
Final stage - no output queue.
"""

from datetime import datetime
from loguru import logger

from services.ElasticSearchVector import get_elasticsearch_service
from .base import BasePipelineStage, StageResult
from ..models import EmbeddedBatch, PageStatus


class ElasticsearchIndexerStage(BasePipelineStage[EmbeddedBatch, None]):
    """
    Stage 5: Index embeddings to Elasticsearch.
    Final stage - no output queue.
    """

    stage_name = "indexer"
    input_status = PageStatus.INDEXING
    output_status = PageStatus.COMPLETED

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._es_service = None

    @property
    def es_service(self):
        if self._es_service is None:
            self._es_service = get_elasticsearch_service()
        return self._es_service

    @property
    def num_workers(self) -> int:
        return self.config.index_workers

    @property
    def timeout(self) -> int:
        return self.config.index_timeout

    async def process_batch(self, batch: EmbeddedBatch) -> StageResult:
        """Index embedded pages to Elasticsearch"""
        try:
            logger.debug(f"[{batch.file_id}] Indexing {len(batch.pages)} documents")

            # Prepare documents
            documents = []
            for page in batch.pages:
                doc_id = f"{batch.file_id}_page_{page.page_num}"
                metadata = {
                    "file_id": batch.file_id,
                    "file_name": batch.file_metadata.get("file_name", ""),
                    "page_num": page.page_num,
                    "upload_date": datetime.utcnow().isoformat(),
                    "author_id": batch.file_metadata.get("author_id", ""),
                    "volume_path": page.volume_path
                }

                documents.append({
                    "doc_id": doc_id,
                    "vectors": page.embedding,
                    "metadata": metadata
                })

            # Bulk index
            result = await self.es_service.index_documents_bulk(documents)

            if not result.success:
                return StageResult(
                    success=False,
                    error=result.error or "Indexing failed",
                    failed_pages=[p.page_num for p in batch.pages]
                )

            # Determine failed pages based on result counts
            failed_pages: list[int] = []
            if result.failed_count > 0:
                logger.warning(
                    f"[{batch.file_id}] {result.failed_count} documents failed to index"
                )

            logger.info(
                f"[{batch.file_id}] Indexed {result.indexed_count} documents"
            )

            return StageResult(
                success=True,
                output=None,  # Final stage
                failed_pages=failed_pages
            )

        except Exception as e:
            logger.exception(f"[{batch.file_id}] Indexing batch failed: {e}")
            return StageResult(
                success=False,
                error=str(e),
                failed_pages=[p.page_num for p in batch.pages]
            )
