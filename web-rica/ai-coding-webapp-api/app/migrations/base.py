#  Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.

import asyncio
from abc import ABC, abstractmethod
from typing import Any, Callable, Optional

from motor.motor_asyncio import AsyncIOMotorDatabase
from pymongo.errors import BulkWriteError, ExecutionTimeout
from utils.helpers import datetime_now_sec


class BaseMigration(ABC):
    def __init__(
        self, revision: str, down_revision: Optional[str] = None, batch_size: int = 64
    ):
        self.revision = revision
        self.down_revision = down_revision
        self.created_at = datetime_now_sec()
        self.batch_size = batch_size

    @abstractmethod
    async def upgrade(self, db: AsyncIOMotorDatabase) -> bool:
        """Apply the migration"""
        pass

    @abstractmethod
    async def downgrade(self, db: AsyncIOMotorDatabase) -> None:
        """Revert the migration"""
        pass

    async def validate_prerequisites(self, db: AsyncIOMotorDatabase) -> bool:
        """Override to add custom validation before migration"""
        return True

    async def post_migration_check(self, db: AsyncIOMotorDatabase) -> bool:
        """Override to add post-migration validation"""
        return True

    async def _commit_batch(self, collection, batch, final: bool = False) -> int:
        """Commit a batch of updates with retry & exponential backoff."""
        label = "Final batch" if final else f"Batch write ({len(batch)} documents)"
        await self.retry_with_exponential_backoff(
            collection.bulk_write,
            batch,
            ordered=False,
            operation_name=label,
            max_retries=3,
        )
        await asyncio.sleep(0.5)
        return len(batch)

    async def retry_with_exponential_backoff(
        self,
        operation: Callable,
        *args,
        max_retries: int = 3,
        base_delay: float = 1.0,
        operation_name: str = "operation",
        **kwargs,
    ) -> Any:
        retries = 0
        while retries < max_retries:
            try:
                result = await operation(*args, **kwargs)
                if retries > 0:
                    print(f"{operation_name} succeeded after {retries} retries")
                return result
            except (BulkWriteError, ExecutionTimeout, Exception) as e:
                retries += 1
                if retries >= max_retries:
                    print(
                        f"{operation_name} failed after {max_retries} attempts. Last error: {e}"
                    )
                    raise RuntimeError(
                        f"Failed to execute {operation_name} after {max_retries} retries"
                    )

                wait_time = base_delay**retries
                print(
                    f"{operation_name} failed (attempt {retries}/{max_retries}): {e}. "
                    f"Retrying in {wait_time} seconds..."
                )
                await asyncio.sleep(wait_time)

    async def create_progress_bar(
        self, total: int, desc: str = "Processing", unit: str = "docs"
    ):
        """Create a simple progress tracker"""
        return SimpleProgressTracker(total, desc, unit)


class SimpleProgressTracker:
    """Simple progress tracker that doesn't require tqdm"""

    def __init__(self, total: int, desc: str, unit: str):
        self.total = total
        self.desc = desc
        self.unit = unit
        self.current = 0

    def update(self, n: int = 1):
        self.current += n
        percentage = (self.current / self.total * 100) if self.total > 0 else 0
        print(f"{self.desc}: {self.current}/{self.total} {self.unit} ({percentage:.1f}%)")

    def close(self):
        print(f"{self.desc}: Complete")
