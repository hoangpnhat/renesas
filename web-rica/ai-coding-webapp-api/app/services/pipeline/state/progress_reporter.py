"""
Progress Reporter

Reports pipeline progress at intervals for real-time updates.
"""

import asyncio
from typing import Callable, Optional, Awaitable, Union
from loguru import logger

from ..models import PipelineProgress
from .tracker import PageStateTracker


class ProgressReporter:
    """Reports pipeline progress at intervals"""

    def __init__(
        self,
        file_id: str,
        state_tracker: PageStateTracker,
        on_progress: Optional[Callable[[PipelineProgress], Union[None, Awaitable[None]]]] = None,
        report_interval: float = 2.0  # Report every 2 seconds
    ):
        self.file_id = file_id
        self.state_tracker = state_tracker
        self.on_progress = on_progress
        self.report_interval = report_interval
        self._running = False
        self._task: Optional[asyncio.Task] = None

    async def start(self):
        """Start progress reporting"""
        self._running = True
        self._task = asyncio.create_task(self._report_loop())
        logger.debug(f"[{self.file_id}] Progress reporter started")

    async def stop(self):
        """Stop progress reporting"""
        self._running = False
        if self._task:
            self._task.cancel()
            try:
                await self._task
            except asyncio.CancelledError:
                pass
        logger.debug(f"[{self.file_id}] Progress reporter stopped")

    async def _report_loop(self):
        """Main reporting loop"""
        while self._running:
            try:
                progress = await self.state_tracker.get_progress()

                if self.on_progress:
                    try:
                        result = self.on_progress(progress)
                        if asyncio.iscoroutine(result):
                            await result
                    except Exception as e:
                        logger.error(f"Progress callback error: {e}")

                # Check if complete
                if progress.total_pages > 0:
                    if progress.completed + progress.failed >= progress.total_pages:
                        logger.info(f"[{self.file_id}] Pipeline complete - stopping reporter")
                        break

                await asyncio.sleep(self.report_interval)

            except asyncio.CancelledError:
                break
            except Exception as e:
                logger.exception(f"Progress reporter error: {e}")
                await asyncio.sleep(self.report_interval)

    async def report_now(self) -> PipelineProgress:
        """Report progress immediately"""
        progress = await self.state_tracker.get_progress()

        if self.on_progress:
            try:
                result = self.on_progress(progress)
                if asyncio.iscoroutine(result):
                    await result
            except Exception as e:
                logger.error(f"Progress callback error: {e}")

        return progress
