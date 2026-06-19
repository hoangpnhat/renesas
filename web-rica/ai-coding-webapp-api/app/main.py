#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

from contextlib import asynccontextmanager

import socketio
import uvicorn
from fastapi import FastAPI, HTTPException, Request
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse
from fastapi.staticfiles import StaticFiles

from api.v1.api import api
from api.v1.endpoints import static
from api.v1.socket import sio
from caching.redis import _redis_client
from core import (
    settings,
    configure_logging,
    authorize)
from core.auth.azure import azure_scheme_allow_unauthenticated
from core.loguru import logger
from db.session import get_engine
from middlewares.logger import LoggingMiddleware
from models import (
    User,
    Group,
    Files,
    Notification,
    Knowledge,
    Prompt
)
from models.api_key import ApiKey
from models.chat_model import Dialog, MessageContent
from models.model_management import ModelManagement
from models.token_consumption import UserTokenConsumption
from services import broker


# Định nghĩa context manager cho việc khởi tạo ứng dụng
@asynccontextmanager
async def lifespan(app: FastAPI):
    # log setup
    configure_logging()

    # Database setup
    engine = get_engine()
    # Khởi tạo cơ sở dữ liệu và các routes API
    await engine.configure_database(
        [User, Dialog, MessageContent, ModelManagement, UserTokenConsumption,  # type: ignore[arg-type]
         Group, Files, Notification, Knowledge, Prompt, ApiKey],
        update_existing_indexes=True,
    )

    # cache setup
    assert _redis_client is not None
    await _redis_client.start()

    # authorize setup
    await authorize.start()
    await azure_scheme_allow_unauthenticated.start()

    # background task
    await broker.startup()

    yield
    # SHUTDOWN
    await _redis_client.close()
    await authorize.stop()
    await azure_scheme_allow_unauthenticated.stop()
    await broker.shutdown()


# Khởi tạo FastAPI app
_is_production = settings.ENVIRONMENT == "production"
app = FastAPI(
    lifespan=lifespan,
    title=settings.api_app.APP_NAME or "",
    version=settings.api_app.VERSION or "",
    docs_url=None if _is_production else "/docs",
    redoc_url=None if _is_production else "/redoc",
    swagger_ui_oauth2_redirect_url="/oauth2-redirect",
    swagger_ui_init_oauth={
        "usePkceWithAuthorizationCodeGrant": True,
        "clientId": settings.API_CLIENT_ID,
        "scopes": [f"api://{settings.API_CLIENT_ID}/user_impersonation"],
    },
)


# injector = Injector()

# Exception handler
@app.exception_handler(HTTPException)
async def http_exception_handler(request: Request, exc: HTTPException):
    logger.error(
        f"HTTPException: {exc.detail} (status_code={exc.status_code}) - URL: {request.url}."
    )
    return JSONResponse(status_code=exc.status_code, content={"detail": exc.detail})


# CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=settings.ALLOW_CORS.split(","),
    allow_methods=["*"],
    allow_credentials=True,
    allow_headers=["*"],
)

# Universal Middleware Logger
app.add_middleware(LoggingMiddleware)


# Include API router TRƯỚC khi mount static files và SPA routes


class SPAStaticFiles(StaticFiles):
    async def get_response(self, path: str, scope):
        response = await super().get_response(path, scope)
        if path.endswith(".js") or path.endswith(".mjs"):
            response.headers["Content-Type"] = "application/javascript"
        elif path.endswith(".css"):
            response.headers["Content-Type"] = "text/css"
        return response


# mount("/agnes", SPAStaticFiles(directory="static", html=True), name="assets")
# mount("/assets", SPAStaticFiles(directory="static/assets"), name="assets")

app.include_router(api, prefix=settings.api_app.API_V1_URL or "/api")
app.include_router(static.router)

# Keep reference to unwrapped app for testing
fastapi_app = app

# Tích hợp Socket.IO - đặt cuối cùng
app = socketio.ASGIApp(sio, other_asgi_app=fastapi_app)

if __name__ == "__main__":
    uvicorn.run(app, port=8082)
