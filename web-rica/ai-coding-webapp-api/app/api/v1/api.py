from fastapi import APIRouter

from api.v1.endpoints import chat
from api.v1.endpoints import extension
from api.v1.endpoints import healthcheck
from api.v1.endpoints import notification
from api.v1.endpoints import user
from api.v1.endpoints.admin import router as AdminRouter
from api.v1.endpoints.file import router as FileRouter
from api.v1.endpoints.group import router as GroupRouter
from api.v1.endpoints.keys import router as KeysRouter
from api.v1.endpoints.knowledge import router as KnowledgeRouter
from api.v1.endpoints.prompt import router as PromptRouter
from api.v1.endpoints.user_model import router as UserModelRouter

api = APIRouter()
api.include_router(AdminRouter, prefix="/admin", tags=["admin"])
api.include_router(healthcheck.router, prefix="/healthcheck", tags=["healthcheck"])
api.include_router(chat.router, prefix="/chats", tags=["chat"])
api.include_router(UserModelRouter, prefix="/user-model", tags=["user-model"])
api.include_router(FileRouter, prefix="/file", tags=["file"])
api.include_router(GroupRouter, prefix="/group", tags=["group"])
api.include_router(notification.router, prefix="/notification", tags=["notification"])
api.include_router(KnowledgeRouter, prefix="/knowledge", tags=["knowledge"])
api.include_router(PromptRouter, prefix="/prompt", tags=["prompt"])

# TODO: router-level auth DI
api.include_router(user.router, prefix="/user", tags=["user"])
api.include_router(KeysRouter, prefix="/keys", tags=["keys"])
# api.include_router(static.router, prefix="/agnes", tags=["agnes"])

# API that interacts with RICA extension
api.include_router(extension.router, prefix="/extension", tags=["extension"])
