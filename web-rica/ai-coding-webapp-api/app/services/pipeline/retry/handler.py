"""
Retry Handler

Handles retry logic for failed pages with exponential backoff.
"""

import asyncio
from typing import List, Dict, Callable, Awaitable
from loguru import logger

from ..models import PageStatus, PipelineConfig
from ..state.tracker import PageStateTracker
from .strategies import RetryStrategy, RetryContext


class RetryHandler:
    """Handles retry logic for failed pages"""

    def __init__(
        self,
        file_id: str,
        config: PipelineConfig,
        state_tracker: PageStateTracker
    ):
        self.file_id = file_id
        self.config = config
        self.state_tracker = state_tracker
        self.strategy = RetryStrategy(
            base_delay=config.retry_delay_base,
            multiplier=config.retry_delay_multiplier,
            max_delay=config.retry_delay_max
        )

    async def should_retry(self, page_num: int) -> bool:
        """Check if page should be retried"""
        state = await self.state_tracker.get_page_state(self.file_id, page_num)

        if state is None:
            return False

        context = RetryContext(
            page_num=page_num,
            stage=state.error_stage or "unknown",
            error=state.error or "unknown",
            attempt=state.retry_count,
            max_attempts=self.config.max_retries
        )

        return context.can_retry and context.is_retriable_error

    async def get_retry_pages(self) -> List[int]:
        """Get list of pages eligible for retry"""
        failed_pages = await self.state_tracker.get_failed_pages(self.file_id)
        retry_pages = []

        for page_num in failed_pages:
            if await self.should_retry(page_num):
                retry_pages.append(page_num)

        return retry_pages

    async def prepare_retry(self, page_num: int) -> float:
        """
        Prepare page for retry.
        Returns delay before retry should be attempted.
        """
        state = await self.state_tracker.get_page_state(self.file_id, page_num)

        if state is None:
            raise ValueError(f"Page {page_num} not found")

        # Calculate delay
        delay = self.strategy.get_delay(state.retry_count)

        # Update state
        await self.state_tracker.update_status(
            self.file_id,
            page_num,
            PageStatus.RETRY_PENDING
        )

        logger.info(
            f"[{self.file_id}] Page {page_num} scheduled for retry "
            f"(attempt {state.retry_count + 1}/{self.config.max_retries}) "
            f"in {delay:.1f}s"
        )

        return delay

    async def execute_retry_batch(
        self,
        page_numbers: List[int],
        retry_func: Callable[[int], Awaitable[bool]]
    ) -> Dict[int, bool]:
        """
        Execute retry for batch of pages with delays.

        Args:
            page_numbers: Pages to retry
            retry_func: Async function to call for each page

        Returns:
            Dict mapping page_num to success status
        """
        results = {}

        for page_num in page_numbers:
            if not await self.should_retry(page_num):
                results[page_num] = False
                continue

            delay = await self.prepare_retry(page_num)
            await asyncio.sleep(delay)

            try:
                success = await retry_func(page_num)
                results[page_num] = success

                if success:
                    await self.state_tracker.update_status(
                        self.file_id,
                        page_num,
                        PageStatus.COMPLETED
                    )
                else:
                    state = await self.state_tracker.get_page_state(
                        self.file_id, page_num
                    )
                    await self.state_tracker.mark_failed(
                        self.file_id,
                        page_num,
                        "Retry failed",
                        (state.error_stage or "unknown") if state else "unknown"
                    )

            except Exception as e:
                logger.exception(f"Retry failed for page {page_num}: {e}")
                results[page_num] = False
                await self.state_tracker.mark_failed(
                    self.file_id,
                    page_num,
                    str(e),
                    "retry"
                )

        return results
