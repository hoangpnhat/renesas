import asyncio
import logging
from enum import Enum
from typing import Optional

logger = logging.getLogger(__name__)


class WarmupStatus(str, Enum):
    COLD = "cold"
    WARMING = "warming"
    WARM = "warm"
    ERROR = "error"


class EndpointWarmupService:
    """
    On-demand warmup for embedding endpoint.

    Always triggers warmup on each upload to handle scale-to-zero:
    - If endpoint is warm: ping completes in ~2-5 seconds
    - If endpoint scaled to zero: ping takes longer, with retries

    Concurrent uploads share the same warmup (don't trigger multiple).
    """

    def __init__(
        self,
        warmup_timeout: float = 300.0,  # Total timeout for warmup (5 min)
        max_retries: int = 5,
        retry_delay: float = 5.0,  # Delay between retries
        ping_timeout: float = 90.0,  # Timeout for each ping attempt
    ):
        self.warmup_timeout = warmup_timeout
        self.max_retries = max_retries
        self.retry_delay = retry_delay
        self.ping_timeout = ping_timeout
        self._warmup_event: Optional[asyncio.Event] = None
        self._warmup_in_progress = False
        self._warmup_lock: Optional[asyncio.Lock] = None
        self.status = WarmupStatus.COLD
        self.error_message: Optional[str] = None
        self._attempt_count = 0

    def _ensure_event_loop_objects(self):
        """Lazily create event loop objects to avoid issues with different event loops"""
        if self._warmup_event is None:
            self._warmup_event = asyncio.Event()
        if self._warmup_lock is None:
            self._warmup_lock = asyncio.Lock()

    async def trigger_warmup(self) -> bool:
        """
        Trigger warmup for this upload (thread-safe for concurrent uploads).

        Always triggers a new warmup unless one is already in progress.
        If endpoint is warm, ping completes quickly (~2-5s).
        If endpoint scaled to zero, ping takes longer with retries.

        Concurrent uploads share the same warmup session.

        Returns True if warmup was triggered or is in progress.
        """
        self._ensure_event_loop_objects()
        assert self._warmup_lock is not None and self._warmup_event is not None

        async with self._warmup_lock:
            # Another upload already triggered warmup - share it
            if self._warmup_in_progress:
                logger.info("Warmup already in progress (from another upload), will wait for completion")
                return True

            # Start new warmup
            self._warmup_in_progress = True
            self.status = WarmupStatus.WARMING
            self._warmup_event.clear()
            self.error_message = None
            self._attempt_count = 0

        logger.info("Triggering endpoint warmup...")
        asyncio.create_task(self._do_warmup())
        return True

    async def _do_warmup(self):
        """Perform warmup with retry logic for scale-to-zero scenarios"""
        try:
            for attempt in range(1, self.max_retries + 1):
                self._attempt_count = attempt
                logger.info(f"Warmup attempt {attempt}/{self.max_retries}...")

                try:
                    success = await asyncio.wait_for(
                        self._ping_endpoint(),
                        timeout=self.ping_timeout
                    )

                    if success:
                        logger.info(f"Endpoint warmup completed successfully on attempt {attempt}")
                        return

                except asyncio.TimeoutError:
                    logger.warning(f"Warmup ping timeout on attempt {attempt} (>{self.ping_timeout}s)")
                    self.status = WarmupStatus.WARMING
                    self.error_message = f"Ping timeout on attempt {attempt}"

                # If not last attempt, wait before retry
                if attempt < self.max_retries:
                    logger.info(f"Retrying warmup in {self.retry_delay}s...")
                    await asyncio.sleep(self.retry_delay)

            # All retries exhausted
            logger.error(f"Endpoint warmup failed after {self.max_retries} attempts")
            self.status = WarmupStatus.ERROR
            self.error_message = f"Failed after {self.max_retries} attempts"

        except Exception as e:
            logger.exception(f"Unexpected error during warmup: {e}")
            self.status = WarmupStatus.ERROR
            self.error_message = str(e)
        finally:
            # Signal completion and reset for next upload
            assert self._warmup_event is not None
            self._warmup_event.set()
            self._warmup_in_progress = False

    async def _ping_endpoint(self) -> bool:
        """Send a lightweight ping to warm up endpoint"""
        try:
            from services.VectorEmbedding import get_embedding_service
            from PIL import Image
            from io import BytesIO

            dummy_image = Image.new('RGB', (50, 50), color='white')
            buffer = BytesIO()
            dummy_image.save(buffer, format='JPEG', quality=50)
            buffer.seek(0)

            embedding_service = get_embedding_service()

            result = await embedding_service.generate_image_embeddings_batch(
                image_bytes_list=[buffer.getvalue()],
                batch_size=1
            )
            logger.info(f"Warmup ping completed with embedding of length {len(result.embeddings or [])}")
            if result.success:
                self.status = WarmupStatus.WARM
                self.error_message = None
                return True
            else:
                self.status = WarmupStatus.ERROR
                self.error_message = result.error
                logger.warning(f"Warmup ping failed: {result.error}")
                return False

        except Exception as e:
            self.status = WarmupStatus.ERROR
            self.error_message = str(e)
            logger.error(f"Warmup ping error: {e}")
            return False

    async def wait_for_warmup(self, timeout: Optional[float] = None) -> bool:
        """
        Wait for warmup to complete.

        Args:
            timeout: Max time to wait in seconds. Uses default if not specified.

        Returns:
            True if warmup completed successfully, False otherwise.
        """
        self._ensure_event_loop_objects()
        assert self._warmup_event is not None

        if timeout is None:
            timeout = self.warmup_timeout

        logger.info(f"Waiting for endpoint warmup (timeout: {timeout}s)...")

        try:
            await asyncio.wait_for(self._warmup_event.wait(), timeout=timeout)
            is_warm = self.status == WarmupStatus.WARM
            if is_warm:
                logger.info("Endpoint is warm, proceeding")
            else:
                logger.warning(f"Warmup completed with status: {self.status}")
            return is_warm
        except asyncio.TimeoutError:
            logger.error(f"Warmup timeout after {timeout}s")
            self.status = WarmupStatus.ERROR
            self.error_message = f"Timeout after {timeout}s"
            return False

    def get_status_dict(self):
        """Get status as dict for socket"""
        return {
            "status": self.status.value,
            "error": self.error_message,
            "attempt": self._attempt_count,
            "max_retries": self.max_retries,
        }


# Singleton
_warmup_service = None


def get_warmup_service() -> EndpointWarmupService:
    global _warmup_service
    if _warmup_service is None:
        # Total timeout = max_retries * (ping_timeout + retry_delay) + buffer
        # 5 * (90 + 5) + 30 = 505s (~8.5 min max)
        _warmup_service = EndpointWarmupService(
            warmup_timeout=600.0,  # 10 min total wait timeout
            max_retries=5,
            retry_delay=5.0,
            ping_timeout=90.0,  # Each ping can take up to 90s when scaling
        )
    return _warmup_service
