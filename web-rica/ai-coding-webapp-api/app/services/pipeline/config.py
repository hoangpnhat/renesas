"""
Pipeline configuration presets

Provides different configuration profiles for various workload types.
"""

from .models import PipelineConfig

# Default configuration optimized for typical workloads
DEFAULT_CONFIG = PipelineConfig(
    # Batch sizes
    split_batch_size=5,
    convert_batch_size=5,
    upload_batch_size=10,
    embed_batch_size=1,
    index_batch_size=50,

    # Concurrency
    convert_workers=2,
    upload_workers=5,
    embed_workers=4,
    index_workers=2,

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
    convert_timeout=60,
    upload_timeout=30,
    embed_timeout=60,
    index_timeout=30,

    # Image
    dpi=200,
    image_format="PNG",
)

# High-throughput configuration for large files
HIGH_THROUGHPUT_CONFIG = PipelineConfig(
    split_batch_size=10,
    convert_batch_size=10,
    upload_batch_size=20,
    embed_batch_size=1,
    index_batch_size=100,

    convert_workers=4,
    upload_workers=10,
    embed_workers=4,
    index_workers=4,

    convert_queue_size=20,
    upload_queue_size=40,
    embed_queue_size=60,
    index_queue_size=100,

    max_retries=5,
    retry_delay_base=1.0,
    retry_delay_multiplier=2.0,
    retry_delay_max=30.0,

    convert_timeout=120,
    upload_timeout=60,
    embed_timeout=120,
    index_timeout=60,

    dpi=200,
    image_format="PNG",
)

# Memory-constrained configuration
LOW_MEMORY_CONFIG = PipelineConfig(
    split_batch_size=2,
    convert_batch_size=2,
    upload_batch_size=5,
    embed_batch_size=1,
    index_batch_size=20,

    convert_workers=1,
    upload_workers=3,
    embed_workers=2,
    index_workers=1,

    convert_queue_size=5,
    upload_queue_size=10,
    embed_queue_size=15,
    index_queue_size=20,

    max_retries=3,
    retry_delay_base=1.0,
    retry_delay_multiplier=2.0,
    retry_delay_max=30.0,

    convert_timeout=90,
    upload_timeout=30,
    embed_timeout=60,
    index_timeout=30,

    dpi=150,
    image_format="PNG",
)


def get_config_for_page_count(total_pages: int) -> PipelineConfig:
    """
    Get appropriate config based on page count.

    Args:
        total_pages: Total number of pages in the document

    Returns:
        Appropriate PipelineConfig for the workload
    """
    if total_pages <= 20:
        return DEFAULT_CONFIG
    elif total_pages <= 100:
        return DEFAULT_CONFIG
    else:
        return HIGH_THROUGHPUT_CONFIG
