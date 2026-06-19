# PDF Processing Pipeline - Technical Design Document

## Table of Contents
1. [Executive Summary](#1-executive-summary)
2. [Current Architecture Analysis](#2-current-architecture-analysis)
3. [Proposed Pipeline Architecture](#3-proposed-pipeline-architecture)
4. [Data Models](#4-data-models)
5. [Component Design](#5-component-design)
6. [Background Task Integration](#6-background-task-integration)
7. [Retry & Error Handling](#7-retry--error-handling)
8. [State Management](#8-state-management)
9. [Progress Tracking & Notifications](#9-progress-tracking--notifications)
10. [Migration Strategy](#10-migration-strategy)
11. [Configuration](#11-configuration)
12. [Monitoring & Observability](#12-monitoring--observability)

---

## 1. Executive Summary

### Problem Statement
The current PDF processing flow experiences significant bottlenecks when handling large files (100+ pages). The sequential architecture blocks all downstream processing until the entire PDF is converted to images.

### Proposed Solution
Implement a **streaming pipeline architecture** that:
- Processes pages in small batches (5-10 pages)
- Runs pipeline stages concurrently
- Provides granular retry at page level
- Tracks progress in real-time
- Integrates seamlessly with existing Taskiq background task system

### Key Benefits
| Metric | Current | Proposed | Improvement |
|--------|---------|----------|-------------|
| Memory Peak | O(n) pages | O(batch_size) | ~95% reduction |
| Time to First Index | After all pages converted | After first batch | ~90% faster |
| Retry Granularity | Entire file | Per page | Precise recovery |
| Progress Visibility | Binary (0/100%) | Per-page tracking | Real-time updates |

### Supported File Types

The pipeline supports both PDF files and Office documents:

| File Type | Extension | Conversion Required |
|-----------|-----------|---------------------|
| PDF | `.pdf` | No - processed directly |
| Word | `.doc`, `.docx` | Yes - converted to PDF via LibreOffice |
| PowerPoint | `.ppt`, `.pptx` | Yes - converted to PDF via LibreOffice |
| Excel | `.xls`, `.xlsx` | Yes - converted to PDF via LibreOffice |

**Office File Conversion Flow:**
```
Office File (docx/pptx/xlsx)
        │
        ▼
┌───────────────────────┐
│ LibreOffice Headless  │
│ --convert-to pdf      │
└───────────────────────┘
        │
        ▼
    PDF Bytes
        │
        ▼
 [Pipeline Stages]
```

**Requirements for Office conversion:**
- LibreOffice must be installed: `sudo apt-get install -y libreoffice`
- The `soffice` command must be available in PATH or at standard locations

---

## 2. Current Architecture Analysis

### 2.1 Current Flow Diagram

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                           CURRENT SEQUENTIAL FLOW                               │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│  ┌──────────────┐                                                               │
│  │ File Upload  │                                                               │
│  │ (API Layer)  │                                                               │
│  └──────┬───────┘                                                               │
│         │                                                                       │
│         ▼                                                                       │
│  ┌──────────────┐     ┌──────────────────────────────────────────────────────┐  │
│  │ Upload to    │     │                    TASKIQ WORKER                     │  │
│  │ Databricks   │────▶│  sync_file_conversion_and_indexing                   │  │
│  │ Volume       │     │                                                      │  │
│  └──────────────┘     │  Step 1: Download original file      ✓ Fast         │  │
│                       │          ↓                                           │  │
│                       │  Step 2: Convert ALL pages to images ✗ BOTTLENECK   │  │
│                       │          ↓ (blocks until complete)                   │  │
│                       │  Step 3: Upload ALL images           ~ Batched(20)  │  │
│                       │          ↓                                           │  │
│                       │  Step 4: Download images for embed   ~ Batched(15)  │  │
│                       │          ↓                                           │  │
│                       │  Step 5: Generate embeddings         ~ Concurrent(4)│  │
│                       │          ↓                                           │  │
│                       │  Step 6: Index to Elasticsearch      ~ Chunked(50)  │  │
│                       │          ↓                                           │  │
│                       │  Step 7: Update status → "completed"                 │  │
│                       └──────────────────────────────────────────────────────┘  │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

### 2.2 Code References - Bottleneck Analysis

**Entry Point:** `app/services/taskiq/tasks/sync_databricks.py:358-459`
```python
async def sync_file_conversion_and_indexing(metadata, context, user_id):
    # Step 2: BOTTLENECK - Converts ALL pages before proceeding
    conversion_result = await _convert_file_to_images(task_id, file_bytes, file.file_name)
```

**Blocking Conversion:** `app/services/FileConverter.py:301-307`
```python
# This loads ALL pages into memory at once
pil_images = convert_from_bytes(
    pdf_bytes,
    dpi=self.dpi,
    poppler_path=self.poppler_path
)
```

### 2.3 Current Limitations

| Issue | Impact | Location |
|-------|--------|----------|
| Full PDF load into memory | OOM for large files | `FileConverter.py:301` |
| Sequential stage execution | High latency | `sync_databricks.py:386-440` |
| All-or-nothing retry | Wasted work on failure | `sync_databricks.py:453-459` |
| No progress tracking | Poor UX | No granular status updates |
| Single task handles everything | No parallelism | Monolithic function |

---

## 3. Proposed Pipeline Architecture

### 3.1 High-Level Architecture

```
┌────────────────────────────────────────────────────────────────────────────────────┐
│                         PROPOSED PIPELINE ARCHITECTURE                             │
├────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                    │
│  ┌──────────────────────────────────────────────────────────────────────────────┐  │
│  │                           PIPELINE ORCHESTRATOR                              │  │
│  │                    (Manages stages, queues, and state)                       │  │
│  └──────────────────────────────────────────────────────────────────────────────┘  │
│         │                                                                          │
│         ▼                                                                          │
│  ┌──────────────────────────────────────────────────────────────────────────────┐  │
│  │                              ASYNC QUEUES                                    │  │
│  │  ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐    │  │
│  │  │ Page    │    │Convert  │    │Upload   │    │Embed    │    │Index    │    │  │
│  │  │ Queue   │───▶│ Queue   │───▶│ Queue   │───▶│ Queue   │───▶│ Queue   │    │  │
│  │  │(pending)│    │(images) │    │(paths)  │    │(vectors)│    │(docs)   │    │  │
│  │  └─────────┘    └─────────┘    └─────────┘    └─────────┘    └─────────┘    │  │
│  └──────────────────────────────────────────────────────────────────────────────┘  │
│         │                │                │                │                │      │
│         ▼                ▼                ▼                ▼                ▼      │
│  ┌────────────┐   ┌────────────┐   ┌────────────┐   ┌────────────┐   ┌──────────┐ │
│  │  STAGE 1   │   │  STAGE 2   │   │  STAGE 3   │   │  STAGE 4   │   │ STAGE 5  │ │
│  │  PDF Split │   │  Convert   │   │  Upload    │   │  Embed     │   │  Index   │ │
│  │            │   │  to Image  │   │  to Volume │   │  Vector    │   │  to ES   │ │
│  │ Workers: 1 │   │ Workers: 2 │   │ Workers: 5 │   │ Workers: 4 │   │Workers: 2│ │
│  └────────────┘   └────────────┘   └────────────┘   └────────────┘   └──────────┘ │
│         │                │                │                │                │      │
│         └────────────────┴────────────────┴────────────────┴────────────────┘      │
│                                          │                                         │
│                                          ▼                                         │
│  ┌──────────────────────────────────────────────────────────────────────────────┐  │
│  │                         PAGE STATE TRACKER (Redis)                           │  │
│  │  ┌─────────────────────────────────────────────────────────────────────────┐ │  │
│  │  │ file_id:page_1 → {status: "indexed", retry: 0}                          │ │  │
│  │  │ file_id:page_2 → {status: "embedding", retry: 0}                        │ │  │
│  │  │ file_id:page_3 → {status: "failed", retry: 2, error: "timeout"}         │ │  │
│  │  └─────────────────────────────────────────────────────────────────────────┘ │  │
│  └──────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                    │
└────────────────────────────────────────────────────────────────────────────────────┘
```

### 3.2 Pipeline Stages Detail

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                              STAGE EXECUTION FLOW                                   │
├─────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                     │
│  PDF File (100 pages)                                                               │
│       │                                                                             │
│       ▼                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │ STAGE 1: PDF SPLITTER                                                       │   │
│  │ - Extract page ranges: [1-5], [6-10], [11-15], ...                          │   │
│  │ - Output: PageBatch objects to convert_queue                                │   │
│  │ - Concurrency: 1 (sequential read)                                          │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│       │                                                                             │
│       │  PageBatch(pages=[1,2,3,4,5])                                               │
│       ▼                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │ STAGE 2: IMAGE CONVERTER                                                    │   │
│  │ - Convert batch of pages to images using pdf2image                          │   │
│  │ - Output: ImageBatch objects to upload_queue                                │   │
│  │ - Concurrency: 2 workers (CPU bound)                                        │   │
│  │ - Memory: Only holds current batch in memory                                │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│       │                                                                             │
│       │  ImageBatch(images=[{page:1, bytes:...}, ...])                              │
│       ▼                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │ STAGE 3: VOLUME UPLOADER                                                    │   │
│  │ - Upload images to Databricks Volume                                        │   │
│  │ - Output: UploadedBatch objects to embed_queue                              │   │
│  │ - Concurrency: 5 workers (I/O bound)                                        │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│       │                                                                             │
│       │  UploadedBatch(paths=["/vol/page_1.png", ...])                              │
│       ▼                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │ STAGE 4: EMBEDDING GENERATOR                                                │   │
│  │ - Call Databricks endpoint for vector embeddings                            │   │
│  │ - Output: EmbeddedBatch objects to index_queue                              │   │
│  │ - Concurrency: 4 workers (matches endpoint limit)                           │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│       │                                                                             │
│       │  EmbeddedBatch(embeddings=[{page:1, vector:[...]}, ...])                    │
│       ▼                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │ STAGE 5: ELASTICSEARCH INDEXER                                              │   │
│  │ - Bulk index documents to Elasticsearch                                     │   │
│  │ - Output: IndexResult with success/failure counts                           │   │
│  │ - Concurrency: 2 workers (ES connection pool)                               │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│       │                                                                             │
│       ▼                                                                             │
│  [COMPLETED - All pages indexed]                                                    │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

### 3.3 Concurrent Execution Timeline

```
Time →  0s    5s    10s   15s   20s   25s   30s   35s   40s
        │     │     │     │     │     │     │     │     │
        ▼     ▼     ▼     ▼     ▼     ▼     ▼     ▼     ▼

CURRENT (Sequential):
┌──────────────────────────────────────────────────────────────────────────┐
│ Convert ALL ████████████████████████████████│Upload│Embed│Index│        │
└──────────────────────────────────────────────────────────────────────────┘
Total: ~40s (blocked until convert complete)

PROPOSED (Pipeline):
┌──────────────────────────────────────────────────────────────────────────┐
│ Split   │██│                                                             │
│ Convert │████│████│████│████│████│                                       │
│ Upload  │    │████│████│████│████│████│                                  │
│ Embed   │        │████│████│████│████│████│                              │
│ Index   │            │████│████│████│████│████│                          │
└──────────────────────────────────────────────────────────────────────────┘
Total: ~25s (stages overlap, first batch indexed at ~12s)
```

---

## 4. Data Models

### 4.1 Core Models

```python
# app/services/pipeline/models.py

from dataclasses import dataclass, field
from datetime import datetime
from enum import Enum
from typing import Optional, List, Dict, Any
from odmantic import ObjectId


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


@dataclass
class PipelineConfig:
    """Pipeline configuration"""
    # Batch sizes
    split_batch_size: int = 5          # Pages per batch for splitting
    convert_batch_size: int = 5        # Pages per conversion batch
    upload_batch_size: int = 10        # Images per upload batch
    embed_batch_size: int = 3          # Images per embedding call
    index_batch_size: int = 50         # Documents per ES bulk request

    # Concurrency limits
    convert_workers: int = 2           # CPU bound - limit to cores
    upload_workers: int = 5            # I/O bound - can be higher
    embed_workers: int = 4             # Match endpoint concurrency limit
    index_workers: int = 2             # ES connection pool limit
    embed_max_concurrent: int = 4      # Max concurrent embedding batches

    # Queue sizes (backpressure control)
    convert_queue_size: int = 10       # Max batches waiting for conversion
    upload_queue_size: int = 20        # Max batches waiting for upload
    embed_queue_size: int = 30         # Max batches waiting for embedding
    index_queue_size: int = 50         # Max batches waiting for indexing

    # Retry configuration
    max_retries: int = 3
    retry_delay_base: float = 1.0      # Base delay in seconds
    retry_delay_multiplier: float = 2.0  # Exponential backoff multiplier
    retry_delay_max: float = 30.0      # Maximum retry delay

    # Timeouts
    convert_timeout: int = 60          # Per batch timeout in seconds
    upload_timeout: int = 30
    embed_timeout: int = 60
    index_timeout: int = 30

    # Image settings
    dpi: int = 200
    image_format: str = "PNG"


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
```

### 4.2 Database Models (MongoDB)

```python
# app/models/pipeline_task.py

from datetime import datetime
from typing import Optional, List, Dict, Any
from odmantic import Field, Index, EmbeddedModel
from db.base_class import Base
from utils.helpers import datetime_now_sec


class PageProcessingState(EmbeddedModel):
    """Embedded model for tracking individual page state"""
    page_num: int
    status: str = "pending"
    retry_count: int = 0
    error: Optional[str] = None
    error_stage: Optional[str] = None
    volume_path: Optional[str] = None
    updated_at: datetime = Field(default_factory=datetime_now_sec)


class PipelineTask(Base):
    """
    Persistent task record for pipeline processing.
    Enables resume capability after worker restart.
    """
    # Foreign keys
    file_id: str = Field(index=True)
    author_id: str = Field(index=True)
    taskiq_task_id: Optional[str] = Field(default=None, index=True)

    # File info
    file_name: str
    total_pages: int

    # Status
    status: str = Field(default="pending", index=True)  # pending, running, completed, failed

    # Progress tracking
    pages_completed: int = 0
    pages_failed: int = 0
    page_states: List[PageProcessingState] = Field(default_factory=list)

    # Timestamps
    created_at: datetime = Field(default_factory=datetime_now_sec)
    started_at: Optional[datetime] = None
    completed_at: Optional[datetime] = None

    # Error info
    error: Optional[str] = None

    # Configuration snapshot (for resume)
    config_snapshot: Optional[Dict[str, Any]] = None

    model_config = {
        "collection": "pipeline_tasks",
        "indexes": lambda: [
            Index(PipelineTask.file_id, PipelineTask.status),
            Index(PipelineTask.author_id, PipelineTask.created_at),
        ]
    }
```

---

## 5. Component Design

### 5.1 Directory Structure

```
app/services/pipeline/
├── __init__.py
├── models.py                    # Data models (PageState, PipelineConfig, etc.)
├── config.py                    # Default configurations
│
├── core/
│   ├── __init__.py
│   ├── orchestrator.py          # Main pipeline orchestrator
│   ├── queue_manager.py         # Async queue management
│   └── worker_pool.py           # Worker pool for stages
│
├── stages/
│   ├── __init__.py
│   ├── base.py                  # Abstract base stage
│   ├── splitter.py              # PDF page splitter
│   ├── converter.py             # Image converter
│   ├── uploader.py              # Volume uploader
│   ├── embedder.py              # Vector embedder
│   └── indexer.py               # ES indexer
│
├── state/
│   ├── __init__.py
│   ├── tracker.py               # Page state tracker
│   ├── redis_backend.py         # Redis state storage
│   └── progress_reporter.py     # Progress reporting
│
├── retry/
│   ├── __init__.py
│   ├── handler.py               # Retry orchestration
│   ├── strategies.py            # Retry strategies (exponential, etc.)
│   └── dead_letter.py           # Failed page handling
│
└── tasks/
    ├── __init__.py
    └── pipeline_tasks.py        # Taskiq task definitions
```

### 5.2 Base Stage Interface

```python
# app/services/pipeline/stages/base.py

from abc import ABC, abstractmethod
from typing import TypeVar, Generic, AsyncIterator, Optional
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
    output: Optional[any] = None
    error: Optional[str] = None
    failed_pages: list[int] = None

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
        self._workers: list[asyncio.Task] = []

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
        logger.info(f"Started {self.num_workers} workers for stage: {self.stage_name}")

    async def stop(self):
        """Stop all workers gracefully"""
        self._running = False
        for worker in self._workers:
            worker.cancel()
        await asyncio.gather(*self._workers, return_exceptions=True)
        self._workers.clear()
        logger.info(f"Stopped all workers for stage: {self.stage_name}")

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
                if result.success and result.output:
                    # Update state and pass to next stage
                    await self._update_batch_status(batch, self.output_status)
                    if self.output_queue:
                        await self.output_queue.put(result.output)
                else:
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
        for page_num in result.failed_pages:
            await self.state_tracker.mark_failed(
                file_id,
                page_num,
                result.error,
                self.stage_name
            )

    def _get_page_numbers(self, batch: InputT) -> list[int]:
        """Extract page numbers from batch - override if needed"""
        if hasattr(batch, 'page_numbers'):
            return batch.page_numbers
        if hasattr(batch, 'pages'):
            return [p.page_num for p in batch.pages]
        if hasattr(batch, 'images'):
            return [img.page_num for img in batch.images]
        return []

    def _get_file_id(self, batch: InputT) -> str:
        """Extract file_id from batch"""
        return getattr(batch, 'file_id', '')
```

### 5.3 Stage Implementations

#### 5.3.1 PDF Splitter Stage

```python
# app/services/pipeline/stages/splitter.py

import asyncio
from typing import AsyncIterator
from loguru import logger
from pdf2image.pdf2image import pdfinfo_from_bytes

from .base import BasePipelineStage, StageResult
from ..models import PageBatch, PipelineConfig, PageStatus


class PDFSplitterStage(BasePipelineStage[bytes, PageBatch]):
    """
    Stage 1: Split PDF into page batches.
    Does NOT convert - just creates batches of page numbers for downstream processing.
    """

    stage_name = "splitter"
    input_status = PageStatus.PENDING
    output_status = PageStatus.SPLITTING

    @property
    def num_workers(self) -> int:
        return 1  # Single worker - sequential PDF reading

    @property
    def timeout(self) -> int:
        return 30  # PDF info extraction is fast

    async def split_pdf(
        self,
        file_id: str,
        pdf_bytes: bytes
    ) -> AsyncIterator[PageBatch]:
        """
        Generator that yields page batches.
        Called directly by orchestrator, not through queue.
        """
        try:
            # Get PDF info without loading images
            info = await asyncio.to_thread(
                pdfinfo_from_bytes,
                pdf_bytes,
                userpw=None,
                poppler_path=self.config.poppler_path if hasattr(self.config, 'poppler_path') else None
            )
            total_pages = info.get('Pages', 0)

            logger.info(f"[{file_id}] PDF has {total_pages} pages, splitting into batches of {self.config.split_batch_size}")

            # Initialize all page states
            for page_num in range(1, total_pages + 1):
                await self.state_tracker.initialize_page(file_id, page_num)

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

    async def process_batch(self, batch: bytes) -> StageResult:
        """Not used - splitter uses generator pattern"""
        raise NotImplementedError("Splitter uses split_pdf generator")
```

#### 5.3.2 Image Converter Stage

```python
# app/services/pipeline/stages/converter.py

import asyncio
from io import BytesIO
from loguru import logger
from pdf2image import convert_from_bytes

from .base import BasePipelineStage, StageResult
from ..models import PageBatch, ImageBatch, ImageData, PageStatus


class ImageConverterStage(BasePipelineStage[PageBatch, ImageBatch]):
    """
    Stage 2: Convert PDF pages to images.
    CPU-bound operation - limited workers.
    """

    stage_name = "converter"
    input_status = PageStatus.CONVERTING
    output_status = PageStatus.UPLOADING

    @property
    def num_workers(self) -> int:
        return self.config.convert_workers

    @property
    def timeout(self) -> int:
        return self.config.convert_timeout

    async def process_batch(self, batch: PageBatch) -> StageResult:
        """Convert a batch of PDF pages to images"""
        try:
            logger.debug(f"[{batch.file_id}] Converting pages {batch.page_numbers}")

            if not batch.pdf_bytes:
                return StageResult(
                    success=False,
                    error="No PDF bytes provided",
                    failed_pages=batch.page_numbers
                )

            # Convert pages in thread (CPU bound)
            first_page = min(batch.page_numbers)
            last_page = max(batch.page_numbers)

            pil_images = await asyncio.to_thread(
                convert_from_bytes,
                batch.pdf_bytes,
                dpi=self.config.dpi,
                first_page=first_page,
                last_page=last_page,
                fmt=self.config.image_format.lower(),
            )

            # Build image data list
            images = []
            for idx, (page_num, pil_image) in enumerate(
                zip(range(first_page, last_page + 1), pil_images)
            ):
                if page_num not in batch.page_numbers:
                    continue

                # Convert to bytes
                img_buffer = BytesIO()
                pil_image.save(img_buffer, format=self.config.image_format)
                img_buffer.seek(0)

                images.append(ImageData(
                    page_num=page_num,
                    image_bytes=img_buffer.getvalue(),
                    filename=f"page_{page_num}.{self.config.image_format.lower()}"
                ))

                # Free memory
                pil_image.close()

            output = ImageBatch(
                file_id=batch.file_id,
                batch_id=batch.batch_id,
                images=images
            )

            logger.info(f"[{batch.file_id}] Converted {len(images)} pages")

            return StageResult(success=True, output=output)

        except Exception as e:
            logger.exception(f"[{batch.file_id}] Conversion failed: {e}")
            return StageResult(
                success=False,
                error=str(e),
                failed_pages=batch.page_numbers
            )
```

#### 5.3.3 Volume Uploader Stage

```python
# app/services/pipeline/stages/uploader.py

import asyncio
from loguru import logger

from services.storage import DatabricksStorage
from .base import BasePipelineStage, StageResult
from ..models import ImageBatch, UploadedBatch, UploadedPage, PageStatus


class VolumeUploaderStage(BasePipelineStage[ImageBatch, UploadedBatch]):
    """
    Stage 3: Upload images to Databricks Volume.
    I/O bound - can have more workers.
    """

    stage_name = "uploader"
    input_status = PageStatus.UPLOADING
    output_status = PageStatus.EMBEDDING

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.storage = DatabricksStorage()

    @property
    def num_workers(self) -> int:
        return self.config.upload_workers

    @property
    def timeout(self) -> int:
        return self.config.upload_timeout

    async def process_batch(self, batch: ImageBatch) -> StageResult:
        """Upload batch of images to volume"""
        try:
            logger.debug(f"[{batch.file_id}] Uploading {len(batch)} images")

            base_path = f"{self.storage.BASE_BUCKET}/document_files/{batch.file_id}"
            uploaded_pages = []
            failed_pages = []

            # Upload concurrently within batch
            async def upload_single(image_data):
                try:
                    upload_path = f"{base_path}/{image_data.filename}"
                    result = await self.storage.upload(
                        upload_file_path=upload_path,
                        file_body=image_data.image_bytes
                    )
                    if result.status:
                        return UploadedPage(
                            page_num=image_data.page_num,
                            volume_path=upload_path
                        )
                    else:
                        return None
                except Exception as e:
                    logger.error(f"Upload failed for page {image_data.page_num}: {e}")
                    return None

            results = await asyncio.gather(
                *[upload_single(img) for img in batch.images],
                return_exceptions=True
            )

            for img, result in zip(batch.images, results):
                if isinstance(result, Exception) or result is None:
                    failed_pages.append(img.page_num)
                else:
                    uploaded_pages.append(result)

            if failed_pages:
                logger.warning(f"[{batch.file_id}] Failed to upload pages: {failed_pages}")

            if not uploaded_pages:
                return StageResult(
                    success=False,
                    error="All uploads failed",
                    failed_pages=[img.page_num for img in batch.images]
                )

            output = UploadedBatch(
                file_id=batch.file_id,
                batch_id=batch.batch_id,
                pages=uploaded_pages
            )

            logger.info(f"[{batch.file_id}] Uploaded {len(uploaded_pages)} images")

            return StageResult(
                success=True,
                output=output,
                failed_pages=failed_pages
            )

        except Exception as e:
            logger.exception(f"[{batch.file_id}] Upload batch failed: {e}")
            return StageResult(
                success=False,
                error=str(e),
                failed_pages=[img.page_num for img in batch.images]
            )
```

#### 5.3.4 Embedding Generator Stage

```python
# app/services/pipeline/stages/embedder.py

import asyncio
from loguru import logger

from services.storage import DatabricksStorage
from services.VectorEmbedding import get_embedding_service
from .base import BasePipelineStage, StageResult
from ..models import UploadedBatch, EmbeddedBatch, EmbeddedPage, PageStatus


class EmbeddingGeneratorStage(BasePipelineStage[UploadedBatch, EmbeddedBatch]):
    """
    Stage 4: Generate vector embeddings for images.
    API bound - limited by endpoint concurrency.
    """

    stage_name = "embedder"
    input_status = PageStatus.EMBEDDING
    output_status = PageStatus.INDEXING

    def __init__(self, *args, file_metadata: dict = None, **kwargs):
        super().__init__(*args, **kwargs)
        self.embedding_service = get_embedding_service()
        self.storage = DatabricksStorage()
        self.file_metadata = file_metadata or {}

    @property
    def num_workers(self) -> int:
        return self.config.embed_workers

    @property
    def timeout(self) -> int:
        return self.config.embed_timeout

    async def process_batch(self, batch: UploadedBatch) -> StageResult:
        """Generate embeddings for uploaded images"""
        try:
            logger.debug(f"[{batch.file_id}] Generating embeddings for {len(batch.pages)} pages")

            embedded_pages = []
            failed_pages = []

            # Download images and generate embeddings
            for page in batch.pages:
                try:
                    # Download image from volume
                    image_bytes = await self.storage.download(page.volume_path)

                    if image_bytes is None:
                        failed_pages.append(page.page_num)
                        continue

                    # Generate embedding
                    result = await self.embedding_service.generate_image_embeddings_batch(
                        image_bytes_list=[image_bytes],
                        batch_size=1,
                        max_concurrent=1
                    )

                    if result.success and result.embeddings:
                        embedded_pages.append(EmbeddedPage(
                            page_num=page.page_num,
                            volume_path=page.volume_path,
                            embedding=result.embeddings[0]
                        ))
                    else:
                        failed_pages.append(page.page_num)

                except Exception as e:
                    logger.error(f"Embedding failed for page {page.page_num}: {e}")
                    failed_pages.append(page.page_num)

            if not embedded_pages:
                return StageResult(
                    success=False,
                    error="All embeddings failed",
                    failed_pages=[p.page_num for p in batch.pages]
                )

            output = EmbeddedBatch(
                file_id=batch.file_id,
                batch_id=batch.batch_id,
                pages=embedded_pages,
                file_metadata=self.file_metadata
            )

            logger.info(f"[{batch.file_id}] Generated {len(embedded_pages)} embeddings")

            return StageResult(
                success=True,
                output=output,
                failed_pages=failed_pages
            )

        except Exception as e:
            logger.exception(f"[{batch.file_id}] Embedding batch failed: {e}")
            return StageResult(
                success=False,
                error=str(e),
                failed_pages=[p.page_num for p in batch.pages]
            )
```

#### 5.3.5 Elasticsearch Indexer Stage

```python
# app/services/pipeline/stages/indexer.py

import asyncio
from datetime import datetime
from loguru import logger

from services.ElasticSearchVector import get_elasticsearch_service
from .base import BasePipelineStage, StageResult
from ..models import EmbeddedBatch, PageStatus


class ElasticsearchIndexerStage(BasePipelineStage[EmbeddedBatch, None]):
    """
    Stage 5: Index embeddings to Elasticsearch.
    Final stage - no output queue.
    """

    stage_name = "indexer"
    input_status = PageStatus.INDEXING
    output_status = PageStatus.COMPLETED

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.es_service = get_elasticsearch_service()

    @property
    def num_workers(self) -> int:
        return self.config.index_workers

    @property
    def timeout(self) -> int:
        return self.config.index_timeout

    async def process_batch(self, batch: EmbeddedBatch) -> StageResult:
        """Index embedded pages to Elasticsearch"""
        try:
            logger.debug(f"[{batch.file_id}] Indexing {len(batch.pages)} documents")

            # Prepare documents
            documents = []
            for page in batch.pages:
                doc_id = f"{batch.file_id}_page_{page.page_num}"
                metadata = {
                    "file_id": batch.file_id,
                    "file_name": batch.file_metadata.get("file_name", ""),
                    "page_num": page.page_num,
                    "upload_date": datetime.utcnow().isoformat(),
                    "author_id": batch.file_metadata.get("author_id", ""),
                    "volume_path": page.volume_path
                }

                documents.append({
                    "doc_id": doc_id,
                    "vectors": page.embedding,
                    "metadata": metadata
                })

            # Bulk index
            result = await self.es_service.index_documents_bulk(documents)

            if not result.success:
                return StageResult(
                    success=False,
                    error=result.error or "Indexing failed",
                    failed_pages=[p.page_num for p in batch.pages]
                )

            # Determine failed pages based on result counts
            failed_pages = []
            if result.failed_count > 0:
                # Note: ES bulk doesn't tell us which specific docs failed
                # In production, you'd want more detailed error handling
                logger.warning(f"[{batch.file_id}] {result.failed_count} documents failed to index")

            logger.info(
                f"[{batch.file_id}] Indexed {result.indexed_count} documents"
            )

            return StageResult(
                success=True,
                output=None,  # Final stage
                failed_pages=failed_pages
            )

        except Exception as e:
            logger.exception(f"[{batch.file_id}] Indexing batch failed: {e}")
            return StageResult(
                success=False,
                error=str(e),
                failed_pages=[p.page_num for p in batch.pages]
            )
```

---

## 6. Background Task Integration

### 6.1 Taskiq Task Definitions

```python
# app/services/pipeline/tasks/pipeline_tasks.py

import asyncio
import json
from typing import Annotated, Optional
from loguru import logger
from taskiq import Context, TaskiqDepends

from models.file import Files
from ..core.orchestrator import PipelineOrchestrator
from ..models import PipelineConfig, PipelineResult
from ..state.tracker import PageStateTracker
from services.storage import DatabricksStorage
from services.taskiq.utils import SocketHelper


def _parse_file_metadata(metadata: str | dict) -> Files:
    """Parse file metadata from task payload"""
    if isinstance(metadata, str):
        return Files(**json.loads(metadata))
    return Files(**metadata)


async def _notify_user(user_id: str, event: str, data: dict):
    """Send socket notification to user"""
    try:
        async with SocketHelper() as socket:
            await socket.emit(
                event,
                {"data": data},
                room=f"user_{user_id}"
            )
    except Exception as e:
        logger.error(f"Failed to notify user {user_id}: {e}")


# =============================================================================
# MAIN PIPELINE TASK
# =============================================================================

# Office file extensions that need conversion to PDF
OFFICE_EXTENSIONS = {'.ppt', '.pptx', '.doc', '.docx', '.xls', '.xlsx'}


async def _convert_to_pdf_if_needed(
    file_bytes: bytes,
    file_name: str,
    task_id: str
) -> bytes:
    """
    Convert Office files (docx, pptx, xlsx) to PDF bytes.
    Returns the original bytes if already a PDF.
    """
    from pathlib import Path
    from services.FileConverter import get_converter_service

    file_ext = Path(file_name).suffix.lower()

    # If already a PDF, return as-is
    if file_ext == '.pdf':
        return file_bytes

    # Check if it's an Office file that needs conversion
    if file_ext not in OFFICE_EXTENSIONS:
        raise ValueError(f"Unsupported file type: {file_ext}")

    logger.info(f"[Task {task_id}] Converting {file_ext} to PDF using LibreOffice")

    converter = get_converter_service()
    if not converter.is_libreoffice_available():
        raise RuntimeError(f"LibreOffice is required to convert {file_ext} files")

    # Convert to PDF in thread (blocking operation)
    pdf_bytes = await asyncio.to_thread(
        converter.convert_office_to_pdf_bytes,
        file_bytes,
        file_ext
    )

    return pdf_bytes


async def sync_file_pipeline_processing(
    metadata: str | dict,
    context: Annotated[Context, TaskiqDepends()],
    user_id: Optional[str] = None,
    config_overrides: Optional[dict] = None
) -> bool:
    """
    Main pipeline task for processing files.

    This is the new entry point that replaces sync_file_conversion_and_indexing.
    Supports both PDF files and Office documents (docx, pptx, xlsx).

    Args:
        metadata: File metadata (dict or JSON string)
        context: Taskiq context
        user_id: User ID for notifications (optional, defaults to author_id)
        config_overrides: Optional config overrides

    Returns:
        bool: True if successful
    """
    task_id = context.message.task_id

    try:
        # Parse metadata
        file = _parse_file_metadata(metadata)
        file_id = str(file.id)
        notify_user_id = user_id or str(file.author_id)

        logger.info(f"[Task {task_id}] Starting pipeline for file: {file.file_name}")

        # Build config from environment settings
        config = PipelineConfig.from_settings()
        if config_overrides:
            for key, value in config_overrides.items():
                if hasattr(config, key):
                    setattr(config, key, value)

        # Download original file
        storage = DatabricksStorage()
        dir_name = "document_files"
        directory_path = f"{storage.BASE_BUCKET}/{dir_name}"
        original_path = f"{directory_path}/{file_id}/{file_id}.{file.extension}"

        logger.info(f"[Task {task_id}] Downloading original file from: {original_path}")
        file_bytes = await storage.download(original_path)

        if not file_bytes:
            raise Exception(f"Failed to download file from {original_path}")

        # Convert Office files to PDF if needed (docx, pptx, xlsx -> pdf)
        pdf_bytes = await _convert_to_pdf_if_needed(
            file_bytes=file_bytes,
            file_name=file.file_name,
            task_id=task_id
        )

        # File metadata for ES indexing
        file_metadata = {
            "file_id": file_id,
            "file_name": file.file_name,
            "author_id": str(file.author_id),
        }

        # Progress callback
        async def on_progress(progress: PipelineProgress):
            await _notify_user(notify_user_id, "file_progress", progress.to_dict())

        # Create and run orchestrator
        orchestrator = PipelineOrchestrator(
            file_id=file_id,
            config=config,
            file_metadata=file_metadata,
            on_progress=on_progress
        )

        result: PipelineResult = await orchestrator.run(pdf_bytes)

        # Update file status in database
        from services.taskiq.tasks.sync_databricks import _update_file_status

        if result.success:
            await _update_file_status(
                file_id=file_id,
                status="completed",
                user_id=notify_user_id,
                total_pages=result.total_pages
            )
            logger.info(f"[Task {task_id}] Pipeline completed successfully")
            return True
        else:
            error_msg = f"Pipeline failed: {result.failed_pages} pages failed"
            await _update_file_status(
                file_id=file_id,
                status="failed",
                user_id=notify_user_id,
                error=error_msg
            )
            logger.error(f"[Task {task_id}] {error_msg}")
            return False

    except Exception as e:
        logger.exception(f"[Task {task_id}] Pipeline failed with exception: {e}")
        try:
            await _update_file_status(
                file_id=file_id,
                status="failed",
                user_id=notify_user_id,
                error=str(e)
            )
        except:
            pass
        return False


# =============================================================================
# RETRY TASK
# =============================================================================

async def sync_file_pipeline_retry(
    file_id: str,
    failed_pages: list[int],
    context: Annotated[Context, TaskiqDepends()],
    user_id: Optional[str] = None
) -> bool:
    """
    Retry failed pages for a file.

    Called automatically by pipeline or manually via API.

    Args:
        file_id: File ID to retry
        failed_pages: List of page numbers to retry
        context: Taskiq context
        user_id: User ID for notifications

    Returns:
        bool: True if all retries successful
    """
    task_id = context.message.task_id

    try:
        logger.info(f"[Task {task_id}] Retrying {len(failed_pages)} pages for file {file_id}")

        # Get file info from database
        from crud.crud_file import file_crud
        file = await file_crud.get_by_id(id=file_id)

        if not file:
            raise Exception(f"File not found: {file_id}")

        notify_user_id = user_id or str(file.author_id)

        # Create retry orchestrator
        config = PipelineConfig(max_retries=1)  # Single retry attempt

        orchestrator = PipelineOrchestrator(
            file_id=file_id,
            config=config,
            file_metadata={
                "file_id": file_id,
                "file_name": file.file_name,
                "author_id": str(file.author_id),
            }
        )

        result = await orchestrator.retry_pages(failed_pages)

        if result.success:
            # Check if all pages now complete
            state_tracker = PageStateTracker(file_id)
            progress = await state_tracker.get_progress()

            if progress.failed == 0:
                await _update_file_status(
                    file_id=file_id,
                    status="completed",
                    user_id=notify_user_id
                )

            logger.info(f"[Task {task_id}] Retry completed successfully")
            return True
        else:
            logger.warning(f"[Task {task_id}] Retry completed with failures")
            return False

    except Exception as e:
        logger.exception(f"[Task {task_id}] Retry failed: {e}")
        return False


# =============================================================================
# RESUME TASK
# =============================================================================

async def sync_file_pipeline_resume(
    file_id: str,
    context: Annotated[Context, TaskiqDepends()],
    user_id: Optional[str] = None
) -> bool:
    """
    Resume pipeline from last checkpoint.

    Used after worker restart or manual pause.

    Args:
        file_id: File ID to resume
        context: Taskiq context
        user_id: User ID for notifications

    Returns:
        bool: True if successful
    """
    task_id = context.message.task_id

    try:
        logger.info(f"[Task {task_id}] Resuming pipeline for file {file_id}")

        # Get pipeline state from tracker
        state_tracker = PageStateTracker(file_id)

        # Find pages that need processing
        incomplete_pages = await state_tracker.get_incomplete_pages()

        if not incomplete_pages:
            logger.info(f"[Task {task_id}] No incomplete pages found")
            return True

        logger.info(f"[Task {task_id}] Found {len(incomplete_pages)} incomplete pages")

        # Dispatch retry task for incomplete pages
        from services.taskiq.broker import broker
        await broker.send_task(
            'sync_file_pipeline_retry',
            file_id=file_id,
            failed_pages=incomplete_pages,
            user_id=user_id
        )

        return True

    except Exception as e:
        logger.exception(f"[Task {task_id}] Resume failed: {e}")
        return False
```

### 6.2 Task Registration

```python
# app/services/taskiq/tasks/__init__.py

from .sync_databricks import (
    sync_file_conversion_and_indexing,  # Keep for backward compatibility
    sync_generate_and_index_vectors_from_images
)
from .sync_notifications import *
from .sync_sharing import *

# New pipeline tasks
from services.pipeline.tasks.pipeline_tasks import (
    sync_file_pipeline_processing,
    sync_file_pipeline_retry,
    sync_file_pipeline_resume
)
```

### 6.3 Broker Integration

The existing `CustomBroker` automatically discovers tasks with `sync_` prefix via `load_tasks()` function in [broker.py:20-27](app/services/taskiq/broker.py#L20-L27). New pipeline tasks will be auto-registered.

---

## 7. Retry & Error Handling

### 7.1 Retry Strategy

```python
# app/services/pipeline/retry/strategies.py

from dataclasses import dataclass
from typing import Optional
import asyncio
import random


@dataclass
class RetryStrategy:
    """Exponential backoff with jitter"""
    base_delay: float = 1.0
    multiplier: float = 2.0
    max_delay: float = 30.0
    jitter: float = 0.1  # 10% jitter

    def get_delay(self, attempt: int) -> float:
        """Calculate delay for given attempt number"""
        delay = self.base_delay * (self.multiplier ** attempt)
        delay = min(delay, self.max_delay)

        # Add jitter
        jitter_range = delay * self.jitter
        delay += random.uniform(-jitter_range, jitter_range)

        return max(0, delay)


@dataclass
class RetryContext:
    """Context for retry decisions"""
    page_num: int
    stage: str
    error: str
    attempt: int
    max_attempts: int

    @property
    def can_retry(self) -> bool:
        return self.attempt < self.max_attempts

    @property
    def is_retriable_error(self) -> bool:
        """Check if error is retriable"""
        non_retriable = [
            "unsupported file type",
            "invalid pdf",
            "corrupted file",
            "permission denied",
        ]
        error_lower = self.error.lower()
        return not any(term in error_lower for term in non_retriable)
```

### 7.2 Retry Handler

```python
# app/services/pipeline/retry/handler.py

import asyncio
from typing import List, Dict, Any
from loguru import logger

from ..models import PageState, PageStatus, PipelineConfig
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
        retry_func
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
                        state.error_stage if state else "unknown"
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
```

### 7.3 Dead Letter Queue

```python
# app/services/pipeline/retry/dead_letter.py

from datetime import datetime
from typing import List, Dict, Any
from loguru import logger

from ..models import PageState, PageStatus
from ..state.tracker import PageStateTracker


class DeadLetterHandler:
    """Handles pages that have exhausted all retries"""

    def __init__(self, file_id: str, state_tracker: PageStateTracker):
        self.file_id = file_id
        self.state_tracker = state_tracker

    async def get_dead_letter_pages(self) -> List[Dict[str, Any]]:
        """Get all pages in dead letter state"""
        failed_pages = await self.state_tracker.get_failed_pages(self.file_id)
        dead_letters = []

        for page_num in failed_pages:
            state = await self.state_tracker.get_page_state(self.file_id, page_num)
            if state and not state.can_retry():
                dead_letters.append({
                    "page_num": page_num,
                    "error": state.error,
                    "error_stage": state.error_stage,
                    "retry_count": state.retry_count,
                    "last_attempt": state.completed_at or datetime.utcnow()
                })

        return dead_letters

    async def move_to_dead_letter(self, page_num: int):
        """Move page to dead letter storage"""
        state = await self.state_tracker.get_page_state(self.file_id, page_num)

        if state is None:
            return

        # Log to dead letter storage (could be separate collection)
        logger.error(
            f"[{self.file_id}] Page {page_num} moved to dead letter. "
            f"Stage: {state.error_stage}, Error: {state.error}, "
            f"Retries: {state.retry_count}"
        )

        # Update status
        await self.state_tracker.update_status(
            self.file_id,
            page_num,
            PageStatus.FAILED  # Final failed state
        )

    async def retry_dead_letter(self, page_num: int, reset_retries: bool = True):
        """
        Manually retry a dead letter page.
        Used for admin intervention.
        """
        if reset_retries:
            await self.state_tracker.reset_retry_count(self.file_id, page_num)

        await self.state_tracker.update_status(
            self.file_id,
            page_num,
            PageStatus.RETRY_PENDING
        )

        logger.info(f"[{self.file_id}] Page {page_num} queued for manual retry")
```

---

## 8. State Management

### 8.1 Page State Tracker

```python
# app/services/pipeline/state/tracker.py

import json
from datetime import datetime
from typing import Optional, List, Dict, Any
from loguru import logger

from ..models import PageState, PageStatus, PipelineProgress
from .redis_backend import RedisStateBackend


class PageStateTracker:
    """
    Tracks state of individual pages through the pipeline.
    Uses Redis for fast access and persistence.
    """

    def __init__(self, file_id: str, redis_backend: RedisStateBackend = None):
        self.file_id = file_id
        self.backend = redis_backend or RedisStateBackend()
        self._key_prefix = f"pipeline:{file_id}"

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
            state.__dict__,
            ttl=86400 * 7  # 7 days TTL
        )

    async def get_page_state(self, file_id: str, page_num: int) -> Optional[PageState]:
        """Get current state of a page"""
        data = await self.backend.get(self._page_key(page_num))
        if data:
            return PageState(**data)
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

        await self.backend.set(self._page_key(page_num), state.__dict__)

        # Update progress counters
        await self._update_progress_counter(status)

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

        state.mark_failed(error, stage)
        await self.backend.set(self._page_key(page_num), state.__dict__)

        await self._update_progress_counter(PageStatus.FAILED)

    async def get_progress(self) -> PipelineProgress:
        """Get overall pipeline progress"""
        data = await self.backend.get(self._progress_key())

        if data:
            return PipelineProgress(
                file_id=self.file_id,
                **data
            )

        return PipelineProgress(file_id=self.file_id, total_pages=0)

    async def initialize_progress(self, total_pages: int):
        """Initialize progress tracking"""
        progress = PipelineProgress(
            file_id=self.file_id,
            total_pages=total_pages,
            pending=total_pages,
            started_at=datetime.utcnow()
        )
        await self.backend.set(self._progress_key(), progress.__dict__)

    async def _update_progress_counter(self, new_status: PageStatus):
        """Update progress counters atomically"""
        progress = await self.get_progress()

        # Increment new status counter
        status_map = {
            PageStatus.PENDING: "pending",
            PageStatus.CONVERTING: "converting",
            PageStatus.UPLOADING: "uploading",
            PageStatus.EMBEDDING: "embedding",
            PageStatus.INDEXING: "indexing",
            PageStatus.COMPLETED: "completed",
            PageStatus.FAILED: "failed",
        }

        if new_status in status_map:
            attr = status_map[new_status]
            current = getattr(progress, attr, 0)
            setattr(progress, attr, current + 1)

            # Decrement pending if moving forward
            if new_status != PageStatus.PENDING and progress.pending > 0:
                progress.pending -= 1

        await self.backend.set(self._progress_key(), progress.__dict__)

    async def get_failed_pages(self, file_id: str) -> List[int]:
        """Get list of failed page numbers"""
        # Scan all page keys and filter by status
        pattern = f"{self._key_prefix}:page:*"
        keys = await self.backend.scan(pattern)

        failed = []
        for key in keys:
            data = await self.backend.get(key)
            if data and data.get("status") == PageStatus.FAILED.value:
                failed.append(data.get("page_num"))

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
                incomplete.append(data.get("page_num"))

        return sorted(incomplete)

    async def reset_retry_count(self, file_id: str, page_num: int):
        """Reset retry count for manual intervention"""
        state = await self.get_page_state(file_id, page_num)
        if state:
            state.retry_count = 0
            state.error = None
            state.error_stage = None
            await self.backend.set(self._page_key(page_num), state.__dict__)

    async def cleanup(self):
        """Clean up all state for this file"""
        pattern = f"{self._key_prefix}:*"
        await self.backend.delete_pattern(pattern)
```

### 8.2 Redis Backend

```python
# app/services/pipeline/state/redis_backend.py

import json
from typing import Optional, Any, List
from caching.redis import get_redis_client


class RedisStateBackend:
    """Redis backend for pipeline state storage"""

    def __init__(self):
        self._client = None

    async def _get_client(self):
        if self._client is None:
            self._client = get_redis_client()
            if not self._client.client:
                await self._client.start()
        return self._client

    async def set(self, key: str, value: dict, ttl: int = 86400):
        """Set value with optional TTL"""
        client = await self._get_client()
        await client.set(key, json.dumps(value, default=str), ex=ttl)

    async def get(self, key: str) -> Optional[dict]:
        """Get value by key"""
        client = await self._get_client()
        data = await client.get(key)
        if data:
            return json.loads(data)
        return None

    async def delete(self, key: str):
        """Delete a key"""
        client = await self._get_client()
        await client.delete(key)

    async def scan(self, pattern: str) -> List[str]:
        """Scan keys matching pattern"""
        client = await self._get_client()
        keys = []
        cursor = 0

        while True:
            cursor, found_keys = await client.client.scan(
                cursor=cursor,
                match=pattern,
                count=100
            )
            keys.extend([k.decode() if isinstance(k, bytes) else k for k in found_keys])
            if cursor == 0:
                break

        return keys

    async def delete_pattern(self, pattern: str):
        """Delete all keys matching pattern"""
        keys = await self.scan(pattern)
        if keys:
            client = await self._get_client()
            await client.client.delete(*keys)

    async def incr(self, key: str) -> int:
        """Atomic increment"""
        client = await self._get_client()
        return await client.client.incr(key)

    async def decr(self, key: str) -> int:
        """Atomic decrement"""
        client = await self._get_client()
        return await client.client.decr(key)
```

---

## 9. Progress Tracking & Notifications

### 9.1 Progress Reporter

```python
# app/services/pipeline/state/progress_reporter.py

import asyncio
from typing import Callable, Optional
from datetime import datetime
from loguru import logger

from ..models import PipelineProgress
from .tracker import PageStateTracker


class ProgressReporter:
    """Reports pipeline progress at intervals"""

    def __init__(
        self,
        file_id: str,
        state_tracker: PageStateTracker,
        on_progress: Optional[Callable[[PipelineProgress], None]] = None,
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

    async def stop(self):
        """Stop progress reporting"""
        self._running = False
        if self._task:
            self._task.cancel()
            try:
                await self._task
            except asyncio.CancelledError:
                pass

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
```

### 9.2 Socket Notification Integration

```python
# Integration with existing SocketHelper

async def notify_file_progress(user_id: str, progress: PipelineProgress):
    """Send progress update via socket"""
    from services.taskiq.utils import SocketHelper

    async with SocketHelper() as socket:
        await socket.emit(
            "file_progress",
            {
                "data": {
                    "file_id": progress.file_id,
                    "total_pages": progress.total_pages,
                    "completed": progress.completed,
                    "failed": progress.failed,
                    "progress_percent": progress.progress_percent,
                    "stages": {
                        "converting": progress.converting,
                        "uploading": progress.uploading,
                        "embedding": progress.embedding,
                        "indexing": progress.indexing,
                    }
                }
            },
            room=f"user_{user_id}"
        )
```

---

## 10. Migration Strategy

### 10.1 Phase 1: Parallel Deployment (Week 1-2)

```python
# Keep both implementations, switch via feature flag

async def dispatch_file_processing(file_metadata: dict, user_id: str):
    """Dispatch file processing - uses feature flag to choose implementation"""
    from core.config import settings
    from services.taskiq.broker import broker

    use_pipeline = getattr(settings, 'USE_PIPELINE_PROCESSING', False)

    if use_pipeline:
        await broker.send_task(
            'sync_file_pipeline_processing',
            metadata=file_metadata,
            user_id=user_id
        )
    else:
        await broker.send_task(
            'sync_file_conversion_and_indexing',
            metadata=json.dumps(file_metadata, default=str)
        )
```

### 10.2 Phase 2: Gradual Rollout (Week 3-4)

- Enable pipeline for new files only
- Monitor performance metrics
- Compare success rates

### 10.3 Phase 3: Full Migration (Week 5+)

- Migrate all traffic to pipeline
- Deprecate old implementation
- Remove feature flag

### 10.4 Rollback Plan

```python
# Emergency rollback via config
PIPELINE_ROLLBACK = True  # Set in environment

# In dispatch function:
if settings.PIPELINE_ROLLBACK:
    logger.warning("Pipeline rollback active - using legacy implementation")
    await broker.send_task('sync_file_conversion_and_indexing', ...)
```

---

## 11. Configuration

### 11.1 Default Configuration

```python
# app/services/pipeline/config.py

from .models import PipelineConfig

# Default configuration optimized for typical workloads
DEFAULT_CONFIG = PipelineConfig(
    # Batch sizes
    split_batch_size=5,
    convert_batch_size=5,
    upload_batch_size=10,
    embed_batch_size=3,
    index_batch_size=50,

    # Concurrency
    convert_workers=2,
    upload_workers=5,
    embed_workers=4,
    index_workers=2,
    embed_max_concurrent=4,

    # Queue sizes
    convert_queue_size=10,
    upload_queue_size=20,
    embed_queue_size=30,
    index_queue_size=50,

    # Retry
    max_retries=3,
    retry_delay_base=1.0,
    retry_delay_multiplier=2.0,
    retry_delay_max=30.0,

    # Timeouts
    convert_timeout=120,
    upload_timeout=60,
    embed_timeout=180,
    index_timeout=60,

    # Image
    dpi=200,
    image_format="PNG",
)

# High-throughput configuration for large files
HIGH_THROUGHPUT_CONFIG = PipelineConfig(
    split_batch_size=10,
    convert_batch_size=10,
    upload_batch_size=20,
    embed_batch_size=5,
    index_batch_size=100,

    convert_workers=4,
    upload_workers=10,
    embed_workers=4,
    index_workers=4,
    embed_max_concurrent=6,

    max_retries=5,
)

# Memory-constrained configuration
LOW_MEMORY_CONFIG = PipelineConfig(
    split_batch_size=2,
    convert_batch_size=2,
    upload_batch_size=5,

    convert_workers=1,
    upload_workers=3,

    convert_queue_size=5,
    upload_queue_size=10,
)
```

### 11.2 Environment Variables

```bash
# .env additions for pipeline

# Feature flags
USE_PIPELINE_PROCESSING=true
PIPELINE_ROLLBACK=false

# Batch sizes
PIPELINE_SPLIT_BATCH_SIZE=5
PIPELINE_CONVERT_BATCH_SIZE=5
PIPELINE_UPLOAD_BATCH_SIZE=10
PIPELINE_EMBED_BATCH_SIZE=3
PIPELINE_INDEX_BATCH_SIZE=50

# Concurrency limits
PIPELINE_CONVERT_WORKERS=2
PIPELINE_UPLOAD_WORKERS=5
PIPELINE_EMBED_WORKERS=4
PIPELINE_INDEX_WORKERS=2
PIPELINE_EMBED_MAX_CONCURRENT=4

# Queue sizes
PIPELINE_CONVERT_QUEUE_SIZE=10
PIPELINE_UPLOAD_QUEUE_SIZE=20
PIPELINE_EMBED_QUEUE_SIZE=30
PIPELINE_INDEX_QUEUE_SIZE=50

# Timeouts (seconds)
PIPELINE_CONVERT_TIMEOUT=120
PIPELINE_UPLOAD_TIMEOUT=60
PIPELINE_EMBED_TIMEOUT=180
PIPELINE_INDEX_TIMEOUT=60

# Retry configuration
PIPELINE_MAX_RETRIES=3
PIPELINE_RETRY_DELAY_BASE=1.0
PIPELINE_RETRY_DELAY_MULTIPLIER=2.0
PIPELINE_RETRY_DELAY_MAX=30.0

# Image settings
PIPELINE_IMAGE_DPI=200
PIPELINE_IMAGE_FORMAT=PNG
```

### 11.3 Loading Configuration from Settings

The `PipelineConfig` can be created from environment settings using the `from_settings()` factory method:

```python
# app/services/pipeline/models.py

@classmethod
def from_settings(cls) -> "PipelineConfig":
    """Create PipelineConfig from application settings"""
    from core.config import settings
    ps = settings.pipeline
    return cls(
        split_batch_size=ps.PIPELINE_SPLIT_BATCH_SIZE,
        convert_batch_size=ps.PIPELINE_CONVERT_BATCH_SIZE,
        upload_batch_size=ps.PIPELINE_UPLOAD_BATCH_SIZE,
        embed_batch_size=ps.PIPELINE_EMBED_BATCH_SIZE,
        index_batch_size=ps.PIPELINE_INDEX_BATCH_SIZE,
        convert_workers=ps.PIPELINE_CONVERT_WORKERS,
        upload_workers=ps.PIPELINE_UPLOAD_WORKERS,
        embed_workers=ps.PIPELINE_EMBED_WORKERS,
        index_workers=ps.PIPELINE_INDEX_WORKERS,
        embed_max_concurrent=ps.PIPELINE_EMBED_MAX_CONCURRENT,
        convert_queue_size=ps.PIPELINE_CONVERT_QUEUE_SIZE,
        upload_queue_size=ps.PIPELINE_UPLOAD_QUEUE_SIZE,
        embed_queue_size=ps.PIPELINE_EMBED_QUEUE_SIZE,
        index_queue_size=ps.PIPELINE_INDEX_QUEUE_SIZE,
        max_retries=ps.PIPELINE_MAX_RETRIES,
        retry_delay_base=ps.PIPELINE_RETRY_DELAY_BASE,
        retry_delay_multiplier=ps.PIPELINE_RETRY_DELAY_MULTIPLIER,
        retry_delay_max=ps.PIPELINE_RETRY_DELAY_MAX,
        convert_timeout=ps.PIPELINE_CONVERT_TIMEOUT,
        upload_timeout=ps.PIPELINE_UPLOAD_TIMEOUT,
        embed_timeout=ps.PIPELINE_EMBED_TIMEOUT,
        index_timeout=ps.PIPELINE_INDEX_TIMEOUT,
        dpi=ps.PIPELINE_IMAGE_DPI,
        image_format=ps.PIPELINE_IMAGE_FORMAT,
        poppler_path=settings.POPPLER_PATH,
    )
```

---

## 12. Monitoring & Observability

### 12.1 Metrics to Track

| Metric | Type | Description |
|--------|------|-------------|
| `pipeline_total_files` | Counter | Total files processed |
| `pipeline_total_pages` | Counter | Total pages processed |
| `pipeline_failed_pages` | Counter | Total pages failed |
| `pipeline_stage_duration_seconds` | Histogram | Time per stage |
| `pipeline_queue_size` | Gauge | Current queue sizes |
| `pipeline_active_workers` | Gauge | Active workers per stage |
| `pipeline_retry_count` | Counter | Total retries |

### 12.2 Logging Structure

```python
# Structured logging for pipeline events

logger.info(
    "Pipeline stage completed",
    extra={
        "file_id": file_id,
        "stage": "converter",
        "batch_id": batch_id,
        "pages_processed": 5,
        "duration_ms": 1234,
        "success": True
    }
)

logger.error(
    "Pipeline stage failed",
    extra={
        "file_id": file_id,
        "stage": "embedder",
        "page_num": 42,
        "error": "Timeout",
        "retry_count": 2,
        "will_retry": True
    }
)
```

### 12.3 Health Check Endpoint

```python
# app/api/v1/endpoints/healthcheck.py

@router.get("/pipeline/health")
async def pipeline_health():
    """Check pipeline health"""
    from services.pipeline.state.redis_backend import RedisStateBackend

    backend = RedisStateBackend()

    try:
        # Check Redis connectivity
        await backend.set("health_check", {"status": "ok"}, ttl=10)
        await backend.get("health_check")

        return {
            "status": "healthy",
            "redis": "connected",
            "timestamp": datetime.utcnow().isoformat()
        }
    except Exception as e:
        return {
            "status": "unhealthy",
            "error": str(e),
            "timestamp": datetime.utcnow().isoformat()
        }
```

---

## Appendix A: Sequence Diagrams

### A.1 Happy Path Flow

```
┌────────┐ ┌──────────┐ ┌───────────┐ ┌────────┐ ┌────────┐ ┌────────┐ ┌─────┐
│  API   │ │  Broker  │ │Orchestrator│ │Splitter│ │Converter│ │Embedder│ │  ES │
└───┬────┘ └────┬─────┘ └─────┬─────┘ └───┬────┘ └───┬─────┘ └───┬────┘ └──┬──┘
    │           │             │           │          │           │         │
    │ upload    │             │           │          │           │         │
    │──────────▶│             │           │          │           │         │
    │           │ send_task   │           │          │           │         │
    │           │────────────▶│           │          │           │         │
    │           │             │           │          │           │         │
    │           │             │ split_pdf │          │           │         │
    │           │             │──────────▶│          │           │         │
    │           │             │           │          │           │         │
    │           │             │ batch_1   │          │           │         │
    │           │             │◀──────────│          │           │         │
    │           │             │                      │           │         │
    │           │             │ convert_batch        │           │         │
    │           │             │─────────────────────▶│           │         │
    │           │             │                      │           │         │
    │           │             │ batch_2              │           │         │
    │           │             │◀──────────│          │           │         │
    │           │             │                      │ images    │         │
    │           │             │                      │──────────▶│         │
    │           │             │ convert_batch        │           │         │
    │           │             │─────────────────────▶│           │ embed   │
    │           │             │                      │           │────────▶│
    │           │             │                      │           │         │
    │           │             │                      │           │  index  │
    │           │             │                      │           │────────▶│
    │           │             │                      │           │         │
    │           │             │ completed            │           │         │
    │◀────────────────────────│                      │           │         │
    │           │             │           │          │           │         │
```

### A.2 Retry Flow

```
┌───────────┐ ┌─────────────┐ ┌────────────┐ ┌───────┐
│  Worker   │ │StateTracker │ │RetryHandler│ │ Stage │
└─────┬─────┘ └──────┬──────┘ └─────┬──────┘ └───┬───┘
      │              │              │            │
      │ process_batch│              │            │
      │─────────────────────────────────────────▶│
      │              │              │            │
      │              │              │      error │
      │◀─────────────────────────────────────────│
      │              │              │            │
      │ mark_failed  │              │            │
      │─────────────▶│              │            │
      │              │              │            │
      │ should_retry │              │            │
      │─────────────────────────────▶            │
      │              │              │            │
      │              │  can_retry=true           │
      │◀─────────────────────────────│            │
      │              │              │            │
      │ prepare_retry│              │            │
      │─────────────────────────────▶            │
      │              │              │            │
      │              │     delay=2s │            │
      │◀─────────────────────────────│            │
      │              │              │            │
      │ [wait 2s]    │              │            │
      │              │              │            │
      │ retry_batch  │              │            │
      │─────────────────────────────────────────▶│
      │              │              │            │
      │              │              │    success │
      │◀─────────────────────────────────────────│
      │              │              │            │
      │ update_status│              │            │
      │─────────────▶│              │            │
```

---

## Appendix B: API Changes

### B.1 New Endpoints

```python
# File processing status
GET /api/v1/file/{file_id}/processing-status
Response: {
    "file_id": "...",
    "status": "processing",
    "progress": {
        "total_pages": 100,
        "completed": 45,
        "failed": 2,
        "progress_percent": 45.0,
        "stages": {
            "converting": 5,
            "uploading": 3,
            "embedding": 2,
            "indexing": 0
        }
    }
}

# Retry failed pages
POST /api/v1/file/{file_id}/retry
Request: {
    "pages": [3, 7, 15]  # Optional, retry all failed if not specified
}
Response: {
    "task_id": "...",
    "pages_queued": 3
}

# Get failed pages
GET /api/v1/file/{file_id}/failed-pages
Response: {
    "file_id": "...",
    "failed_pages": [
        {"page_num": 3, "error": "Timeout", "stage": "embedder", "retries": 3},
        {"page_num": 7, "error": "API error", "stage": "embedder", "retries": 2}
    ]
}
```

### B.2 Socket Events

```javascript
// New events for pipeline progress

// Progress update (sent every 2 seconds during processing)
socket.on('file_progress', (data) => {
    // data.file_id
    // data.total_pages
    // data.completed
    // data.failed
    // data.progress_percent
    // data.stages.converting
    // data.stages.uploading
    // data.stages.embedding
    // data.stages.indexing
});

// Stage completion
socket.on('file_stage_complete', (data) => {
    // data.file_id
    // data.stage
    // data.pages_processed
});
```

---

*Document Version: 1.0*
*Last Updated: 2024*
*Author: AI Assistant*
