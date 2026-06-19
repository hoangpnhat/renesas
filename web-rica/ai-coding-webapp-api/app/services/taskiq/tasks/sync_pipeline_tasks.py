import asyncio
import json
from pathlib import Path
from typing import Annotated, Optional
from loguru import logger
from taskiq import Context, TaskiqDepends

from models.file import Files
from services.storage import DatabricksStorage
from services.taskiq.utils import SocketHelper
from services.FileConverter import get_converter_service, SPIRE_AFFECTED_EXTENSIONS
from services.pipeline.core.orchestrator import PipelineOrchestrator
from services.pipeline.models import PipelineConfig, PipelineResult, PipelineProgress
from services.pipeline.state.tracker import PageStateTracker
from services.pipeline.stages.preprocess import ExcelPreprocessStage

# Office file extensions that need conversion to PDF
OFFICE_EXTENSIONS = {'.ppt', '.pptx', '.doc', '.docx', '.xlsx', '.xlsm'}
SPIRE_WATERMARK_PAGE_COUNT = 1
DEFAULT_DPI = 200 # Default DPI for conversion, can be overridden for Excel files to avoid Spire.XLS watermark issues.
SPIRE_DPI = 100 # DPI for Excel files to avoid Spire.XLS watermark issues

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


async def _update_file_status(
    file_id: str,
    status: str,
    user_id: str | None = None,
    **kwargs
):
    """Update file status in database and emit socket event"""
    # Import here to avoid circular imports
    from services.taskiq.tasks.sync_databricks import _update_file_status as update_status
    await update_status(file_id, status, user_id, **kwargs)


async def _convert_to_pdf_if_needed(
    file_bytes: bytes,
    file_name: str,
    task_id: str,
    dpi: int = DEFAULT_DPI,
) -> bytes:
    """
    Convert Office files (docx, pptx, xlsx) to PDF bytes.
    Returns the original bytes if already a PDF.

    Args:
        file_bytes: Original file content
        file_name: File name with extension
        task_id: Task ID for logging
        dpi: DPI to use for conversion

    Returns:
        PDF bytes (either original or converted)
    """
    file_ext = Path(file_name).suffix.lower()
    dpi = DEFAULT_DPI

    # If already a PDF, return as-is
    if file_ext == '.pdf':
        logger.info(f"[Task {task_id}] File is already PDF, no conversion needed")
        return file_bytes

    # Check if it is excel, preprocess it before conversion
    if file_ext in SPIRE_AFFECTED_EXTENSIONS:
        logger.info(f"[Task {task_id}] Preprocessing Excel file before conversion, setting DPI to 100 to avoid Spire.XLS watermark")
        dpi = SPIRE_DPI
        try:
            preprocessor = ExcelPreprocessStage()
            file_bytes = await asyncio.to_thread(
                preprocessor.preprocess,
                file_bytes,
            )
            logger.info(f"[Task {task_id}] Excel preprocessing completed, new byte size: {len(file_bytes)}")
        except Exception as e:
            logger.error(f"[Task {task_id}] Error during Excel preprocessing: {e}")
            raise Exception(f"Error processing diagrams in Excel file: {e}") from e

    # Check if it's an Office file that needs conversion
    if file_ext not in OFFICE_EXTENSIONS:
        logger.warning(f"[Task {task_id}] Unsupported file type: {file_ext}")
        raise ValueError(f"Unsupported file type: {file_ext}")

    logger.info(f"[Task {task_id}] Converting {file_ext} to PDF using LibreOffice")

    converter = get_converter_service(dpi=dpi)

    # Check LibreOffice availability
    if not converter.is_libreoffice_available():
        error_msg = (
            f"LibreOffice is required to convert {file_ext} files. "
            f"Install it with: sudo apt-get install -y libreoffice"
        )
        logger.error(f"[Task {task_id}] {error_msg}")
        raise RuntimeError(error_msg)

    # Convert to PDF in thread (blocking operation)
    pdf_bytes = await asyncio.to_thread(
        converter.convert_office_to_pdf_bytes,
        file_bytes,
        file_ext
    )

    logger.info(
        f"[Task {task_id}] Converted {file_ext} to PDF: "
        f"{len(file_bytes)} bytes -> {len(pdf_bytes)} bytes"
    )

    return pdf_bytes


# =============================================================================
# MAIN PIPELINE TASK
# =============================================================================

async def sync_file_pipeline_processing(
    metadata: str | dict,
    context: Annotated[Context, TaskiqDepends()],
    user_id: Optional[str] = None,
    config_overrides: Optional[dict] = None
) -> bool:
    """
    Main pipeline task for processing files.

    This is the new entry point that replaces sync_file_conversion_and_indexing.

    Args:
        metadata: File metadata (dict or JSON string)
        context: Taskiq context
        user_id: User ID for notifications (optional, defaults to author_id)
        config_overrides: Optional config overrides

    Returns:
        bool: True if successful
    """
    task_id = context.message.task_id
    file_id = None
    notify_user_id = None

    try:
        # Parse metadata
        file = _parse_file_metadata(metadata)
        file_id = str(file.id)
        notify_user_id = user_id or str(file.author_id)
        file_ext = Path(file.file_name).suffix.lower()

        logger.info(f"[Task {task_id}] Starting pipeline for file: {file.file_name}")
        
        if config_overrides is None:
            config_overrides = {}
        if file_ext in SPIRE_AFFECTED_EXTENSIONS:
            config_overrides.update({"dpi": SPIRE_DPI})

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
        logger.info(f"[Task {task_id}] File extension: {file.extension}, checking if conversion needed")
        pdf_bytes = await _convert_to_pdf_if_needed(
            file_bytes=file_bytes,
            file_name=file.file_name,
            task_id=task_id,
            dpi=config.dpi,
        )
        logger.info(f"[Task {task_id}] After conversion: {len(pdf_bytes)} bytes, first bytes: {pdf_bytes[:20]!r}")

        # File metadata for ES indexing
        file_metadata = {
            "file_id": file_id,
            "file_name": file.file_name,
            "author_id": str(file.author_id),
        }

        # Progress callback
        async def on_progress(progress: PipelineProgress):
            await _notify_user(
                notify_user_id,
                "file_progress",
                progress.to_dict()
            )

        # Create and run orchestrator
        orchestrator = PipelineOrchestrator(
            file_id=file_id,
            config=config,
            file_metadata=file_metadata,
            on_progress=on_progress
        )

        # Exclude the Spire.XLS watermark sheet when
        # saving the preprocessed Excel workbook.
        strip_last_pages = SPIRE_WATERMARK_PAGE_COUNT if file_ext in SPIRE_AFFECTED_EXTENSIONS else 0

        result: PipelineResult = await orchestrator.run(pdf_bytes, strip_last_pages=strip_last_pages)

        # Update file status in database
        if result.success:
            await _update_file_status(
                file_id=file_id,
                status="completed",
                user_id=notify_user_id,
                total_pages=result.total_pages,
                folder_path=f"{directory_path}/{file_id}"
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
            if file_id and notify_user_id:
                await _update_file_status(
                    file_id=file_id,
                    status="failed",
                    user_id=notify_user_id,
                    error=str(e)
                )
        except Exception:
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
    notify_user_id = user_id

    try:
        logger.info(f"[Task {task_id}] Retrying {len(failed_pages)} pages for file {file_id}")

        # Get file info from database
        from crud.crud_file import file_crud
        file = await file_crud.get_by_id(id=file_id)

        if not file:
            raise Exception(f"File not found: {file_id}")

        notify_user_id = user_id or str(file.author_id)

        # Create retry orchestrator from settings with single retry
        config = PipelineConfig.from_settings()
        config.max_retries = 1  # Single retry attempt

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

        # For now, log the incomplete pages
        # A full implementation would re-run those specific pages
        logger.warning(
            f"[Task {task_id}] Resume functionality requires re-processing "
            f"pages: {incomplete_pages}"
        )

        return True

    except Exception as e:
        logger.exception(f"[Task {task_id}] Resume failed: {e}")
        return False
