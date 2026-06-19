"""
Base Pipeline Stage

Abstract base class for all pipeline stages. Each stage:
- Consumes from an input queue
- Processes batches with configurable workers
- Produces to an output queue
- Tracks state via PageStateTracker
"""

from abc import ABC, abstractmethod
from typing import Any, TypeVar, Generic, Optional, List, cast
from dataclasses import dataclass
import asyncio
from loguru import logger

from ..models import PipelineConfig, PageStatus
from ..state.tracker import PageStateTracker

InputT = TypeVar('InputT')
OutputT = TypeVar('OutputT')


@dataclass
class StageResult:
    """Result of processing a single batch"""
    success: bool
    output: Optional[Any] = None
    error: Optional[str] = None
    failed_pages: List[int] | None = None

    def __post_init__(self):
        if self.failed_pages is None:
            self.failed_pages = []


class BasePipelineStage(ABC, Generic[InputT, OutputT]):
    """
    Abstract base class for pipeline stages.
    Each stage consumes from input queue and produces to output queue.
    """

    stage_name: str = "base"
    input_status: PageStatus = PageStatus.PENDING
    output_status: PageStatus = PageStatus.COMPLETED

    def __init__(
        self,
        config: PipelineConfig,
        state_tracker: PageStateTracker,
        input_queue: asyncio.Queue,
        output_queue: Optional[asyncio.Queue] = None,
    ):
        self.config = config
        self.state_tracker = state_tracker
        self.input_queue = input_queue
        self.output_queue = output_queue
        self._running = False
        self._workers: List[asyncio.Task] = []
        self._processed_count = 0
        self._failed_count = 0

    @property
    @abstractmethod
    def num_workers(self) -> int:
        """Number of concurrent workers for this stage"""
        pass

    @property
    @abstractmethod
    def timeout(self) -> int:
        """Timeout in seconds for processing a single batch"""
        pass

    @abstractmethod
    async def process_batch(self, batch: InputT) -> StageResult:
        """
        Process a single batch of items.
        Must be implemented by each stage.
        """
        pass

    async def start(self):
        """Start worker tasks for this stage"""
        self._running = True
        for i in range(self.num_workers):
            worker = asyncio.create_task(
                self._worker_loop(worker_id=i),
                name=f"{self.stage_name}_worker_{i}"
            )
            self._workers.append(worker)
        logger.info(f"[{self.stage_name}] Started {self.num_workers} workers")

    async def stop(self):
        """Stop all workers gracefully"""
        self._running = False

        # Send poison pills to signal shutdown
        for _ in range(self.num_workers):
            try:
                self.input_queue.put_nowait(None)
            except asyncio.QueueFull:
                pass

        # Wait for workers to finish
        if self._workers:
            await asyncio.gather(*self._workers, return_exceptions=True)
        self._workers.clear()
        logger.info(
            f"[{self.stage_name}] Stopped. Processed: {self._processed_count}, "
            f"Failed: {self._failed_count}"
        )

    async def _worker_loop(self, worker_id: int):
        """Main worker loop - consume from input, process, produce to output"""
        while self._running:
            try:
                # Get batch from input queue with timeout
                try:
                    batch = await asyncio.wait_for(
                        self.input_queue.get(),
                        timeout=1.0  # Check _running flag periodically
                    )
                except asyncio.TimeoutError:
                    continue

                if batch is None:  # Poison pill for shutdown
                    self.input_queue.task_done()
                    break

                # Update state to processing
                await self._update_batch_status(batch, self.input_status)

                # Process batch with timeout
                try:
                    result = await asyncio.wait_for(
                        self.process_batch(batch),
                        timeout=self.timeout
                    )
                except asyncio.TimeoutError:
                    result = StageResult(
                        success=False,
                        error=f"Timeout after {self.timeout}s",
                        failed_pages=self._get_page_numbers(batch)
                    )

                # Handle result
                if result.success:
                    self._processed_count += len(self._get_page_numbers(batch))
                    # Update state and pass to next stage
                    await self._update_batch_status(batch, self.output_status)
                    if self.output_queue and result.output:
                        await self.output_queue.put(result.output)
                else:
                    self._failed_count += len(result.failed_pages or [])
                    # Mark failed pages for retry
                    await self._handle_batch_failure(batch, result)

                self.input_queue.task_done()

            except asyncio.CancelledError:
                break
            except Exception as e:
                logger.exception(f"[{self.stage_name}] Worker {worker_id} error: {e}")

    async def _update_batch_status(self, batch: InputT, status: PageStatus):
        """Update status for all pages in batch"""
        page_numbers = self._get_page_numbers(batch)
        file_id = self._get_file_id(batch)
        for page_num in page_numbers:
            await self.state_tracker.update_status(file_id, page_num, status)

    async def _handle_batch_failure(self, batch: InputT, result: StageResult):
        """Handle failed batch - mark for retry or dead letter"""
        file_id = self._get_file_id(batch)
        for page_num in (result.failed_pages or []):
            await self.state_tracker.mark_failed(
                file_id,
                page_num,
                result.error or "Unknown error",
                self.stage_name
            )

    def _get_page_numbers(self, batch: InputT) -> List[int]:
        """Extract page numbers from batch - override if needed"""
        if hasattr(batch, 'page_numbers'):
            return cast(list[int], batch.page_numbers)
        if hasattr(batch, 'pages'):
            return [p.page_num for p in batch.pages]
        if hasattr(batch, 'images'):
            return [img.page_num for img in batch.images]
        return []

    def _get_file_id(self, batch: InputT) -> str:
        """Extract file_id from batch"""
        return getattr(batch, 'file_id', '')
