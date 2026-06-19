"""
Page State Tracker

Tracks the state of individual pages through the pipeline using Redis
for fast access and persistence.
"""

from datetime import datetime
from typing import Optional, List
from loguru import logger

from ..models import PageState, PageStatus, PipelineProgress
from .redis_backend import RedisStateBackend


class PageStateTracker:
    """
    Tracks state of individual pages through the pipeline.
    Uses Redis for fast access and persistence.
    """

    def __init__(self, file_id: str, redis_backend: RedisStateBackend | None = None):
        self.file_id = file_id
        self.backend = redis_backend or RedisStateBackend()
        self._key_prefix = f"pipeline:{file_id}"
        self._ttl = 86400 * 7  # 7 days TTL

    def _page_key(self, page_num: int) -> str:
        return f"{self._key_prefix}:page:{page_num}"

    def _progress_key(self) -> str:
        return f"{self._key_prefix}:progress"

    async def initialize_page(self, file_id: str, page_num: int):
        """Initialize state for a new page"""
        state = PageState(
            page_num=page_num,
            status=PageStatus.PENDING,
            started_at=datetime.utcnow()
        )
        await self.backend.set(
            self._page_key(page_num),
            self._serialize_state(state),
            ttl=self._ttl
        )

    async def get_page_state(self, file_id: str, page_num: int) -> Optional[PageState]:
        """Get current state of a page"""
        data = await self.backend.get(self._page_key(page_num))
        if data:
            return self._deserialize_state(data)
        return None

    async def update_status(
        self,
        file_id: str,
        page_num: int,
        status: PageStatus,
        **kwargs
    ):
        """Update page status"""
        state = await self.get_page_state(file_id, page_num)

        if state is None:
            state = PageState(page_num=page_num)

        old_status = state.status
        state.status = status

        # Update stage timestamps
        timestamp_fields = {
            PageStatus.CONVERTING: "converted_at",
            PageStatus.UPLOADING: "uploaded_at",
            PageStatus.EMBEDDING: "embedded_at",
            PageStatus.INDEXING: "indexed_at",
            PageStatus.COMPLETED: "completed_at",
        }

        if status in timestamp_fields:
            setattr(state, timestamp_fields[status], datetime.utcnow())

        # Apply additional kwargs
        for key, value in kwargs.items():
            if hasattr(state, key):
                setattr(state, key, value)

        await self.backend.set(
            self._page_key(page_num),
            self._serialize_state(state),
            ttl=self._ttl
        )

        # Update progress counters
        await self._update_progress_counter(old_status, status)

    async def mark_failed(
        self,
        file_id: str,
        page_num: int,
        error: str,
        stage: str
    ):
        """Mark page as failed"""
        state = await self.get_page_state(file_id, page_num)

        if state is None:
            state = PageState(page_num=page_num)

        old_status = state.status
        state.mark_failed(error, stage)

        await self.backend.set(
            self._page_key(page_num),
            self._serialize_state(state),
            ttl=self._ttl
        )

        await self._update_progress_counter(old_status, PageStatus.FAILED)

    async def get_progress(self) -> PipelineProgress:
        """Get overall pipeline progress"""
        data = await self.backend.get(self._progress_key())

        if data:
            return PipelineProgress(
                file_id=self.file_id,
                total_pages=data.get("total_pages", 0),
                pending=data.get("pending", 0),
                converting=data.get("converting", 0),
                uploading=data.get("uploading", 0),
                embedding=data.get("embedding", 0),
                indexing=data.get("indexing", 0),
                completed=data.get("completed", 0),
                failed=data.get("failed", 0),
            )

        return PipelineProgress(file_id=self.file_id, total_pages=0)

    async def initialize_progress(self, total_pages: int):
        """Initialize progress tracking"""
        progress_data = {
            "file_id": self.file_id,
            "total_pages": total_pages,
            "pending": total_pages,
            "converting": 0,
            "uploading": 0,
            "embedding": 0,
            "indexing": 0,
            "completed": 0,
            "failed": 0,
            "started_at": datetime.utcnow().isoformat(),
        }
        await self.backend.set(self._progress_key(), progress_data, ttl=self._ttl)

    async def _update_progress_counter(self, old_status: PageStatus, new_status: PageStatus):
        """Update progress counters"""
        progress = await self.get_progress()

        # Status to attribute mapping
        status_map = {
            PageStatus.PENDING: "pending",
            PageStatus.CONVERTING: "converting",
            PageStatus.UPLOADING: "uploading",
            PageStatus.EMBEDDING: "embedding",
            PageStatus.INDEXING: "indexing",
            PageStatus.COMPLETED: "completed",
            PageStatus.FAILED: "failed",
        }

        # Decrement old status counter (if not pending and tracked)
        if old_status in status_map and old_status != PageStatus.PENDING:
            old_attr = status_map[old_status]
            current = getattr(progress, old_attr, 0)
            if current > 0:
                setattr(progress, old_attr, current - 1)

        # Increment new status counter
        if new_status in status_map:
            new_attr = status_map[new_status]
            current = getattr(progress, new_attr, 0)
            setattr(progress, new_attr, current + 1)

        # Decrement pending if moving from pending
        if old_status == PageStatus.PENDING and new_status != PageStatus.PENDING:
            if progress.pending > 0:
                progress.pending -= 1

        # Save updated progress
        progress_data = {
            "file_id": self.file_id,
            "total_pages": progress.total_pages,
            "pending": progress.pending,
            "converting": progress.converting,
            "uploading": progress.uploading,
            "embedding": progress.embedding,
            "indexing": progress.indexing,
            "completed": progress.completed,
            "failed": progress.failed,
        }
        await self.backend.set(self._progress_key(), progress_data, ttl=self._ttl)

    async def get_failed_pages(self, file_id: str) -> List[int]:
        """Get list of failed page numbers"""
        pattern = f"{self._key_prefix}:page:*"
        keys = await self.backend.scan(pattern)

        failed = []
        for key in keys:
            data = await self.backend.get(key)
            if data and data.get("status") == PageStatus.FAILED.value:
                if (page_num := data.get("page_num")) is not None:
                    failed.append(page_num)

        return sorted(failed)

    async def get_incomplete_pages(self) -> List[int]:
        """Get pages that aren't completed or failed"""
        pattern = f"{self._key_prefix}:page:*"
        keys = await self.backend.scan(pattern)

        incomplete = []
        terminal_states = {PageStatus.COMPLETED.value, PageStatus.FAILED.value}

        for key in keys:
            data = await self.backend.get(key)
            if data and data.get("status") not in terminal_states:
                if (page_num := data.get("page_num")) is not None:
                    incomplete.append(page_num)

        return sorted(incomplete)

    async def reset_retry_count(self, file_id: str, page_num: int):
        """Reset retry count for manual intervention"""
        state = await self.get_page_state(file_id, page_num)
        if state:
            state.retry_count = 0
            state.error = None
            state.error_stage = None
            await self.backend.set(
                self._page_key(page_num),
                self._serialize_state(state),
                ttl=self._ttl
            )

    async def cleanup(self):
        """Clean up all state for this file"""
        pattern = f"{self._key_prefix}:*"
        await self.backend.delete_pattern(pattern)
        logger.info(f"[{self.file_id}] Cleaned up pipeline state")

    def _serialize_state(self, state: PageState) -> dict:
        """Serialize PageState to dict for storage"""
        return {
            "page_num": state.page_num,
            "status": state.status.value if isinstance(state.status, PageStatus) else state.status,
            "retry_count": state.retry_count,
            "max_retries": state.max_retries,
            "error": state.error,
            "error_stage": state.error_stage,
            "started_at": state.started_at.isoformat() if state.started_at else None,
            "converted_at": state.converted_at.isoformat() if state.converted_at else None,
            "uploaded_at": state.uploaded_at.isoformat() if state.uploaded_at else None,
            "embedded_at": state.embedded_at.isoformat() if state.embedded_at else None,
            "indexed_at": state.indexed_at.isoformat() if state.indexed_at else None,
            "completed_at": state.completed_at.isoformat() if state.completed_at else None,
            "volume_path": state.volume_path,
        }

    def _deserialize_state(self, data: dict) -> PageState:
        """Deserialize dict to PageState"""
        status_value = data.get("status", "pending")
        try:
            status = PageStatus(status_value)
        except ValueError:
            status = PageStatus.PENDING

        return PageState(
            page_num=data.get("page_num", 0),
            status=status,
            retry_count=data.get("retry_count", 0),
            max_retries=data.get("max_retries", 3),
            error=data.get("error"),
            error_stage=data.get("error_stage"),
            volume_path=data.get("volume_path"),
        )
