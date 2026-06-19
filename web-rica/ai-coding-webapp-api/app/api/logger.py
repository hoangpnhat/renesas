from logging import getLogger, basicConfig, INFO, StreamHandler
from core.config import settings

# App Logger
log_format = (
    "%(asctime)s - %(name)s - %(levelname)s - %(message)s [%(filename)s:%(lineno)d]"
)
basicConfig(
    level=INFO,
    format=log_format,
    handlers=[
        # INFO: Disable log file for now
        # logging.FileHandler(LOG_FILE),
        StreamHandler()
    ],
)
logger = getLogger(settings.api_app.APP_NAME)
