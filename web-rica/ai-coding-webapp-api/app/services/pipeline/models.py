"""
Pipeline data models

Defines all data structures used throughout the pipeline stages.
"""

from dataclasses import dataclass, field
from datetime import datetime
from enum import Enum
from typing import Optional, List, Dict, Any


class PageStatus(str, Enum):
    """Status of individual page processing"""
    PENDING = "pending"
    SPLITTING = "splitting"
    CONVERTING = "converting"
    UPLOADING = "uploading"
    EMBEDDING = "embedding"
    INDEXING = "indexing"
    COMPLETED = "completed"
    FAILED = "failed"
    RETRY_PENDING = "retry_pending"


class PipelineStatus(str, Enum):
    """Overall pipeline status"""
    INITIALIZING = "initializing"
    RUNNING = "running"
    PAUSED = "paused"
    COMPLETING = "completing"
    COMPLETED = "completed"
    FAILED = "failed"
    CANCELLED = "cancelled"


@dataclass
class PageState:
    """Tracks state of individual page through pipeline"""
    page_num: int
    status: PageStatus = PageStatus.PENDING
    retry_count: int = 0
    max_retries: int = 3
    error: Optional[str] = None
    error_stage: Optional[str] = None

    # Stage timestamps
    started_at: Optional[datetime] = None
    converted_at: Optional[datetime] = None
    uploaded_at: Optional[datetime] = None
    embedded_at: Optional[datetime] = None
    indexed_at: Optional[datetime] = None
    completed_at: Optional[datetime] = None

    # Stage outputs (transient, cleared after next stage)
    image_bytes: Optional[bytes] = None
    volume_path: Optional[str] = None
    embedding: Optional[List[float]] = None

    def can_retry(self) -> bool:
        return self.retry_count < self.max_retries

    def mark_failed(self, error: str, stage: str):
        self.status = PageStatus.FAILED
        self.error = error
        self.error_stage = stage
        self.retry_count += 1


@dataclass
class PageBatch:
    """Batch of pages for processing"""
    file_id: str
    batch_id: str
    page_numbers: List[int]
    pdf_bytes: Optional[bytes] = None  # Only set for convert stage

    def __len__(self):
        return len(self.page_numbers)


@dataclass
class ImageData:
    """Single converted image"""
    page_num: int
    image_bytes: bytes
    filename: str


@dataclass
class ImageBatch:
    """Batch of converted images"""
    file_id: str
    batch_id: str
    images: List[ImageData]

    def __len__(self):
        return len(self.images)


@dataclass
class UploadedPage:
    """Single uploaded page info"""
    page_num: int
    volume_path: str


@dataclass
class UploadedBatch:
    """Batch of uploaded pages"""
    file_id: str
    batch_id: str
    pages: List[UploadedPage]

    def __len__(self):
        return len(self.pages)


@dataclass
class EmbeddedPage:
    """Single page with embedding"""
    page_num: int
    volume_path: str
    embedding: List[List[float]]  # ColPali returns multiple vectors per image


@dataclass
class EmbeddedBatch:
    """Batch of embedded pages"""
    file_id: str
    batch_id: str
    pages: List[EmbeddedPage]
    file_metadata: Dict[str, Any]  # For ES document metadata

    def __len__(self):
        return len(self.pages)


@dataclass
class PipelineConfig:
    """Pipeline configuration - values loaded from environment via settings"""
    # Batch sizes
    split_batch_size: int = 5
    convert_batch_size: int = 5
    upload_batch_size: int = 10
    embed_batch_size: int = 3
    index_batch_size: int = 50

    # Concurrency limits
    convert_workers: int = 2
    upload_workers: int = 5
    embed_workers: int = 4
    index_workers: int = 2
    embed_max_concurrent: int = 4

    # Queue sizes (backpressure control)
    convert_queue_size: int = 10
    upload_queue_size: int = 20
    embed_queue_size: int = 30
    index_queue_size: int = 50

    # Retry configuration
    max_retries: int = 3
    retry_delay_base: float = 1.0
    retry_delay_multiplier: float = 2.0
    retry_delay_max: float = 30.0

    # Timeouts (seconds)
    convert_timeout: int = 120
    upload_timeout: int = 60
    embed_timeout: int = 180
    index_timeout: int = 60

    # Image settings
    dpi: int = 200
    image_format: str = "PNG"
    poppler_path: Optional[str] = None

    @classmethod
    def from_settings(cls) -> "PipelineConfig":
        """Create PipelineConfig from application settings"""
        from core.config import settings
        ps = settings.pipeline
        return cls(
            # Batch sizes
            split_batch_size=ps.PIPELINE_SPLIT_BATCH_SIZE,
            convert_batch_size=ps.PIPELINE_CONVERT_BATCH_SIZE,
            upload_batch_size=ps.PIPELINE_UPLOAD_BATCH_SIZE,
            embed_batch_size=ps.PIPELINE_EMBED_BATCH_SIZE,
            index_batch_size=ps.PIPELINE_INDEX_BATCH_SIZE,
            # Concurrency
            convert_workers=ps.PIPELINE_CONVERT_WORKERS,
            upload_workers=ps.PIPELINE_UPLOAD_WORKERS,
            embed_workers=ps.PIPELINE_EMBED_WORKERS,
            index_workers=ps.PIPELINE_INDEX_WORKERS,
            embed_max_concurrent=ps.PIPELINE_EMBED_MAX_CONCURRENT,
            # Queue sizes
            convert_queue_size=ps.PIPELINE_CONVERT_QUEUE_SIZE,
            upload_queue_size=ps.PIPELINE_UPLOAD_QUEUE_SIZE,
            embed_queue_size=ps.PIPELINE_EMBED_QUEUE_SIZE,
            index_queue_size=ps.PIPELINE_INDEX_QUEUE_SIZE,
            # Retry
            max_retries=ps.PIPELINE_MAX_RETRIES,
            retry_delay_base=ps.PIPELINE_RETRY_DELAY_BASE,
            retry_delay_multiplier=ps.PIPELINE_RETRY_DELAY_MULTIPLIER,
            retry_delay_max=ps.PIPELINE_RETRY_DELAY_MAX,
            # Timeouts
            convert_timeout=ps.PIPELINE_CONVERT_TIMEOUT,
            upload_timeout=ps.PIPELINE_UPLOAD_TIMEOUT,
            embed_timeout=ps.PIPELINE_EMBED_TIMEOUT,
            index_timeout=ps.PIPELINE_INDEX_TIMEOUT,
            # Image settings
            dpi=ps.PIPELINE_IMAGE_DPI,
            image_format=ps.PIPELINE_IMAGE_FORMAT,
            poppler_path=settings.POPPLER_PATH,
        )


@dataclass
class PipelineProgress:
    """Real-time pipeline progress"""
    file_id: str
    total_pages: int

    # Per-stage counts
    pending: int = 0
    converting: int = 0
    uploading: int = 0
    embedding: int = 0
    indexing: int = 0
    completed: int = 0
    failed: int = 0

    # Timing
    started_at: Optional[datetime] = None
    estimated_completion: Optional[datetime] = None

    @property
    def progress_percent(self) -> float:
        if self.total_pages == 0:
            return 0.0
        return (self.completed / self.total_pages) * 100

    def to_dict(self) -> Dict[str, Any]:
        return {
            "file_id": self.file_id,
            "total_pages": self.total_pages,
            "completed": self.completed,
            "failed": self.failed,
            "progress_percent": round(self.progress_percent, 1),
            "stages": {
                "pending": self.pending,
                "converting": self.converting,
                "uploading": self.uploading,
                "embedding": self.embedding,
                "indexing": self.indexing,
            }
        }


@dataclass
class PipelineResult:
    """Final pipeline execution result"""
    file_id: str
    success: bool
    total_pages: int
    indexed_pages: int
    failed_pages: int
    failed_page_numbers: List[int] = field(default_factory=list)
    errors: List[Dict[str, Any]] = field(default_factory=list)

    # Timing
    started_at: Optional[datetime] = None
    completed_at: Optional[datetime] = None
    duration_seconds: Optional[float] = None

    # Stage stats
    stage_durations: Dict[str, float] = field(default_factory=dict)
