import logging
import os
from time import time
from fastapi import FastAPI, Request
from starlette.middleware.base import BaseHTTPMiddleware

logger = logging.getLogger("LoggingMiddleware")
LOG_FILE = os.path.join("logs", "log")

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
    handlers=[
        # INFO: Disable log file for now
        # logging.FileHandler(LOG_FILE),
        logging.StreamHandler()
    ],
)


class LoggingMiddleware(BaseHTTPMiddleware):
    async def dispatch(self, request: Request, call_next):
        start_time = time()
        # Log request details
        logger.info(f"Incoming request: {request.method} {request.url}")

        # Add custom headers or trace information if needed
        if request.headers.get("User-Agent"):
            logger.info(f"User-Agent: {request.headers['User-Agent']}")

        # Proceed to process the request
        response = await call_next(request)

        # Measure elapsed time
        process_time = time() - start_time
        logger.info(
            f"Response Status: {response.status_code} "
            f"for {request.method} {request.url} "
            f"completed in {process_time:.2f}s"
        )
        return response
