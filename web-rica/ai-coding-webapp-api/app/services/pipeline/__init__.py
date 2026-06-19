"""
PDF Processing Pipeline

A streaming pipeline architecture for processing PDF files:
- Converts PDFs to images in batches
- Uploads images to storage
- Generates vector embeddings
- Indexes to Elasticsearch

Features:
- Concurrent stage execution
- Per-page retry support
- Real-time progress tracking
- Redis-backed state management
"""

from .models import (
    PageStatus,
    PipelineStatus,
    PageState,
    PageBatch,
    ImageData,
    ImageBatch,
    UploadedPage,
    UploadedBatch,
    EmbeddedPage,
    EmbeddedBatch,
    PipelineConfig,
    PipelineProgress,
    PipelineResult,
)
from .config import DEFAULT_CONFIG, HIGH_THROUGHPUT_CONFIG, LOW_MEMORY_CONFIG

__all__ = [
    # Models
    "PageStatus",
    "PipelineStatus",
    "PageState",
    "PageBatch",
    "ImageData",
    "ImageBatch",
    "UploadedPage",
    "UploadedBatch",
    "EmbeddedPage",
    "EmbeddedBatch",
    "PipelineConfig",
    "PipelineProgress",
    "PipelineResult",
    # Config
    "DEFAULT_CONFIG",
    "HIGH_THROUGHPUT_CONFIG",
    "LOW_MEMORY_CONFIG",
]
