import sys
from loguru import logger

def configure_logging():
    """Configure the logging for the application."""
    # Remove default handler
    logger.remove()

    # Custom format configuration: timestamp, log level, and colored output
    logger.add(
        sys.stdout,
        format="{time:YYYY-MM-DD HH:mm:ss.SSS} | {level} | {message}",
        level="INFO",
        colorize=True
    )

    # Custom log levels
    log_levels = [
        "API_INFO",
        "API_ERROR",
        "FUNCTION_INFO",
        "FUNCTION_ERROR"
    ]

    # Set starting level number and apply dynamic log levels
    start_level = 99
    for idx, level_name in enumerate(log_levels):
        color = "<blue>" if level_name.endswith("INFO") else "<red>"
        logger.level(level_name, no=start_level + idx, color=color)
