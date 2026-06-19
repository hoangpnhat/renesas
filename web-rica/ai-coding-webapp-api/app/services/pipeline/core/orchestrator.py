"""
Pipeline Orchestrator

Coordinates all pipeline stages, manages queues, and tracks overall progress.
"""

import asyncio
from datetime import datetime
from typing import Optional, Callable, Awaitable, Union, List
from loguru import logger

from ..models import (
    PipelineConfig,
    PipelineProgress,
    PipelineResult,
    PipelineStatus,
    PageBatch,
)
from ..state.tracker import PageStateTracker
from ..state.progress_reporter import ProgressReporter
from ..stages.splitter import PDFSplitterStage
from ..stages.converter import ImageConverterStage
from ..stages.uploader import VolumeUploaderStage
from ..stages.embedder import EmbeddingGeneratorStage
from ..stages.indexer import ElasticsearchIndexerStage
from services.EndpointWarmup import get_warmup_service


class PipelineOrchestrator:
    """
    Coordinates all pipeline stages for processing PDF files.

    Creates async queues between stages and manages the concurrent
    execution of all pipeline stages.
    """

    def __init__(
        self,
        file_id: str,
        config: PipelineConfig,
        file_metadata: dict | None = None,
        on_progress: Optional[Callable[[PipelineProgress], Union[None, Awaitable[None]]]] = None,
    ):
        self.file_id = file_id
        self.config = config
        self.file_metadata = file_metadata or {}
        self.on_progress = on_progress

        # State tracking
        self.state_tracker = PageStateTracker(file_id)
        self.progress_reporter: ProgressReporter | None = None

        # Queues between stages
        self.convert_queue: asyncio.Queue | None = None
        self.upload_queue: asyncio.Queue | None = None
        self.embed_queue: asyncio.Queue | None = None
        self.index_queue: asyncio.Queue | None = None

        # Stages
        self.splitter: PDFSplitterStage | None = None
        self.converter: ImageConverterStage | None = None
        self.uploader: VolumeUploaderStage | None = None
        self.embedder: EmbeddingGeneratorStage | None = None
        self.indexer: ElasticsearchIndexerStage | None = None

        # Status
        self.status = PipelineStatus.INITIALIZING
        self.started_at: Optional[datetime] = None
        self.completed_at: Optional[datetime] = None

    def _create_queues(self):
        """Create async queues with configured sizes"""
        self.convert_queue = asyncio.Queue(maxsize=self.config.convert_queue_size)
        self.upload_queue = asyncio.Queue(maxsize=self.config.upload_queue_size)
        self.embed_queue = asyncio.Queue(maxsize=self.config.embed_queue_size)
        self.index_queue = asyncio.Queue(maxsize=self.config.index_queue_size)

    def _create_stages(self):
        """Create all pipeline stages"""
        assert self.convert_queue is not None and self.upload_queue is not None
        assert self.embed_queue is not None and self.index_queue is not None
        # Splitter doesn't use queues directly - it yields batches
        self.splitter = PDFSplitterStage(
            config=self.config,
            state_tracker=self.state_tracker,
            input_queue=asyncio.Queue(),  # Placeholder
            output_queue=self.convert_queue,
        )

        self.converter = ImageConverterStage(
            config=self.config,
            state_tracker=self.state_tracker,
            input_queue=self.convert_queue,
            output_queue=self.upload_queue,
        )

        self.uploader = VolumeUploaderStage(
            config=self.config,
            state_tracker=self.state_tracker,
            input_queue=self.upload_queue,
            output_queue=self.embed_queue,
        )

        self.embedder = EmbeddingGeneratorStage(
            config=self.config,
            state_tracker=self.state_tracker,
            input_queue=self.embed_queue,
            output_queue=self.index_queue,
            file_metadata=self.file_metadata,
        )

        self.indexer = ElasticsearchIndexerStage(
            config=self.config,
            state_tracker=self.state_tracker,
            input_queue=self.index_queue,
            output_queue=None,  # Final stage
        )

    async def run(self, pdf_bytes: bytes, strip_last_pages: int = 0) -> PipelineResult:
        """
        Run the complete pipeline.

        Args:
            pdf_bytes: PDF file content

        Returns:
            PipelineResult with processing stats
        """
        self.started_at = datetime.utcnow()
        self.status = PipelineStatus.RUNNING
        embedder_start_task = None

        try:
            logger.info(f"[{self.file_id}] Starting pipeline")

            # Trigger endpoint warmup early (embedder will wait for it)
            # Note: trigger_warmup() handles concurrent uploads safely
            # - If WARM: returns immediately
            # - If WARMING: waits for existing warmup
            # - If COLD/ERROR: triggers new warmup
            warmup_service = get_warmup_service()
            await warmup_service.trigger_warmup()

            # Initialize queues and stages
            self._create_queues()
            self._create_stages()

            # Start progress reporter
            if self.on_progress:
                self.progress_reporter = ProgressReporter(
                    file_id=self.file_id,
                    state_tracker=self.state_tracker,
                    on_progress=self.on_progress,
                    report_interval=2.0,
                )
                await self.progress_reporter.start()

            # Start non-blocking stages first (converter, uploader, indexer)
            # These can start processing immediately without waiting for warmup
            await self._start_non_blocking_stages()

            # Start embedder in background (will wait for warmup before starting workers)
            # This allows converter/uploader to process while warmup is in progress
            assert self.embedder is not None and self.splitter is not None and self.convert_queue is not None
            embedder_start_task = asyncio.create_task(
                self.embedder.start(),
                name=f"{self.file_id}_embedder_start"
            )

            # Feed batches from splitter to converter queue
            # Converter/uploader can process batches while embedder waits for warmup
            total_batches = 0
            async for batch in self.splitter.split_pdf(self.file_id, pdf_bytes, strip_last_pages=strip_last_pages):
                await self.convert_queue.put(batch)
                total_batches += 1
                logger.debug(f"[{self.file_id}] Queued batch {total_batches}")

            # Ensure embedder workers are started before waiting for completion
            # This blocks until warmup completes and embedder workers are running
            # By this time, converter/uploader have processed most/all batches
            logger.info(f"[{self.file_id}] Waiting for embedder workers to start (warmup may be in progress)...")
            await embedder_start_task
            logger.info(f"[{self.file_id}] Embedder workers started, proceeding to queue completion")

            # Wait for all queues to be processed
            await self._wait_for_completion()

            # Stop stages
            await self._stop_stages()

            # Stop progress reporter
            if self.progress_reporter:
                await self.progress_reporter.stop()

            # Get final progress
            final_progress = await self.state_tracker.get_progress()

            self.completed_at = datetime.utcnow()
            self.status = PipelineStatus.COMPLETED

            # Build result
            result = PipelineResult(
                file_id=self.file_id,
                success=final_progress.failed == 0,
                total_pages=final_progress.total_pages,
                indexed_pages=final_progress.completed,
                failed_pages=final_progress.failed,
                failed_page_numbers=await self.state_tracker.get_failed_pages(self.file_id),
                started_at=self.started_at,
                completed_at=self.completed_at,
                duration_seconds=(self.completed_at - self.started_at).total_seconds(),
            )

            logger.info(
                f"[{self.file_id}] Pipeline completed: "
                f"{result.indexed_pages}/{result.total_pages} indexed, "
                f"{result.failed_pages} failed"
            )

            return result

        except Exception as e:
            logger.exception(f"[{self.file_id}] Pipeline failed: {e}")
            self.status = PipelineStatus.FAILED

            # Cancel embedder start task if it's still running
            if embedder_start_task and not embedder_start_task.done():
                embedder_start_task.cancel()
                try:
                    await embedder_start_task
                except asyncio.CancelledError:
                    pass

            # Stop stages on error
            await self._stop_stages()

            if self.progress_reporter:
                await self.progress_reporter.stop()

            return PipelineResult(
                file_id=self.file_id,
                success=False,
                total_pages=0,
                indexed_pages=0,
                failed_pages=0,
                errors=[{"error": str(e), "stage": "orchestrator"}],
                started_at=self.started_at,
                completed_at=datetime.utcnow(),
            )

    async def _start_stages(self):
        """Start all pipeline stage workers (legacy - blocks on embedder warmup)"""
        assert self.converter is not None and self.uploader is not None
        assert self.embedder is not None and self.indexer is not None
        await self.converter.start()
        await self.uploader.start()
        await self.embedder.start()
        await self.indexer.start()
        logger.info(f"[{self.file_id}] All stages started")

    async def _start_non_blocking_stages(self):
        """
        Start stages that don't need to wait for warmup.

        Starts converter, uploader, and indexer immediately.
        Embedder is started separately to allow parallel processing
        while warmup is in progress.
        """
        assert self.converter is not None and self.uploader is not None and self.indexer is not None
        await self.converter.start()
        await self.uploader.start()
        await self.indexer.start()
        logger.info(f"[{self.file_id}] Non-blocking stages started (converter, uploader, indexer)")

    async def _stop_stages(self):
        """Stop all pipeline stage workers"""
        assert self.indexer is not None and self.embedder is not None
        assert self.uploader is not None and self.converter is not None
        # Stop in reverse order
        await self.indexer.stop()
        await self.embedder.stop()
        await self.uploader.stop()
        await self.converter.stop()
        logger.info(f"[{self.file_id}] All stages stopped")

    async def _wait_for_completion(self):
        """Wait for all queues to be processed"""
        assert self.convert_queue is not None and self.upload_queue is not None
        assert self.embed_queue is not None and self.index_queue is not None
        logger.info(f"[{self.file_id}] Waiting for pipeline completion...")

        # Wait for convert queue to be processed
        await self.convert_queue.join()
        logger.debug(f"[{self.file_id}] Convert queue drained")

        # Signal converter workers to stop
        for _ in range(self.config.convert_workers):
            await self.convert_queue.put(None)

        # Wait for upload queue
        await self.upload_queue.join()
        logger.debug(f"[{self.file_id}] Upload queue drained")

        for _ in range(self.config.upload_workers):
            await self.upload_queue.put(None)

        # Wait for embed queue
        await self.embed_queue.join()
        logger.debug(f"[{self.file_id}] Embed queue drained")

        for _ in range(self.config.embed_workers):
            await self.embed_queue.put(None)

        # Wait for index queue
        await self.index_queue.join()
        logger.debug(f"[{self.file_id}] Index queue drained")

        for _ in range(self.config.index_workers):
            await self.index_queue.put(None)

        logger.info(f"[{self.file_id}] All queues processed")

    async def retry_pages(self, page_numbers: List[int]) -> PipelineResult:
        """
        Retry processing for specific pages.

        Args:
            page_numbers: List of page numbers to retry

        Returns:
            PipelineResult with retry stats
        """
        logger.info(f"[{self.file_id}] Retrying {len(page_numbers)} pages")

        # This would need access to the original PDF bytes
        # For now, return a placeholder result
        # In practice, you'd need to download the PDF again or
        # implement per-stage retry based on what stage failed

        return PipelineResult(
            file_id=self.file_id,
            success=False,
            total_pages=len(page_numbers),
            indexed_pages=0,
            failed_pages=len(page_numbers),
            errors=[{"error": "Retry not fully implemented", "stage": "orchestrator"}],
        )

    async def cancel(self):
        """Cancel the pipeline execution"""
        logger.warning(f"[{self.file_id}] Pipeline cancelled")
        self.status = PipelineStatus.CANCELLED
        await self._stop_stages()

        if self.progress_reporter:
            await self.progress_reporter.stop()
