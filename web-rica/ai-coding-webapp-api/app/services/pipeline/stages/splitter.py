"""
PDF Splitter Stage

Stage 1: Split PDF into page batches.
Does NOT convert - just creates batches of page numbers for downstream processing.
"""

import asyncio
from typing import AsyncIterator
from loguru import logger
from pdf2image.pdf2image import pdfinfo_from_bytes

from .base import BasePipelineStage, StageResult
from ..models import PageBatch, PipelineConfig, PageStatus
from ..state.tracker import PageStateTracker


class PDFSplitterStage(BasePipelineStage[bytes, PageBatch]):
    """
    Stage 1: Split PDF into page batches.
    Does NOT convert - just creates batches of page numbers for downstream processing.
    """

    stage_name = "splitter"
    input_status = PageStatus.PENDING
    output_status = PageStatus.SPLITTING

    def __init__(
        self,
        config: PipelineConfig,
        state_tracker: PageStateTracker,
        input_queue: asyncio.Queue,
        output_queue: asyncio.Queue | None = None,
    ):
        super().__init__(config, state_tracker, input_queue, output_queue)

    @property
    def num_workers(self) -> int:
        return 1  # Single worker - sequential PDF reading

    @property
    def timeout(self) -> int:
        return 30  # PDF info extraction is fast

    async def split_pdf(
        self,
        file_id: str,
        pdf_bytes: bytes,
        strip_last_pages: int = 0,
    ) -> AsyncIterator[PageBatch]:
        """
        Generator that yields page batches.
        Called directly by orchestrator, not through queue.

        Args:
            strip_last_pages: Number of trailing PDF pages to exclude.
                Set to 1 for Spire-preprocessed Excel files to drop the
                evaluation watermark page appended by Spire.XLS free edition.
        """
        try:
            # Get PDF info without loading images
            poppler_path = self.config.poppler_path
            info = await asyncio.to_thread(
                pdfinfo_from_bytes,
                pdf_bytes,
                poppler_path=poppler_path  # type: ignore[arg-type]
            )
            raw_pages = info.get('Pages', 0)
            total_pages = max(0, raw_pages - strip_last_pages)
            if total_pages == 0:
                logger.warning(
                    f"[{file_id}] No pages remain after stripping {strip_last_pages} page(s) "
                    f"from a {raw_pages}-page PDF. Aborting split."
                )
                return
            logger.info(
                f"[{file_id}] PDF has {total_pages} pages (strip_last_pages={strip_last_pages}), "
                f"splitting into batches of {self.config.split_batch_size}"
            )

            # Initialize all page states
            for page_num in range(1, total_pages + 1):
                await self.state_tracker.initialize_page(file_id, page_num)

            # Initialize progress tracking
            await self.state_tracker.initialize_progress(total_pages)

            # Yield batches
            batch_num = 0
            for start in range(1, total_pages + 1, self.config.split_batch_size):
                end = min(start + self.config.split_batch_size, total_pages + 1)
                page_numbers = list(range(start, end))

                batch = PageBatch(
                    file_id=file_id,
                    batch_id=f"{file_id}_batch_{batch_num}",
                    page_numbers=page_numbers,
                    pdf_bytes=pdf_bytes  # Pass PDF bytes for conversion
                )

                batch_num += 1
                yield batch

            logger.info(f"[{file_id}] Created {batch_num} batches")

        except Exception as e:
            logger.exception(f"[{file_id}] Failed to split PDF: {e}")
            raise

    async def get_total_pages(self, pdf_bytes: bytes) -> int:
        """Get total page count from PDF without full conversion"""
        try:
            poppler_path = self.config.poppler_path
            info = await asyncio.to_thread(
                pdfinfo_from_bytes,
                pdf_bytes,
                poppler_path=poppler_path  # type: ignore[arg-type]
            )
            return int(info.get('Pages', 0))
        except Exception as e:
            logger.error(f"Failed to get PDF page count: {e}")
            return 0

    async def process_batch(self, batch: bytes) -> StageResult:
        """Not used - splitter uses generator pattern"""
        raise NotImplementedError("Splitter uses split_pdf generator")
