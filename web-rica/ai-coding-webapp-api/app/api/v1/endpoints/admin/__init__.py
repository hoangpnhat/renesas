from fastapi import APIRouter

from .model_management import router as ModelManagementRouter

router = APIRouter()

router.include_router(ModelManagementRouter, prefix="/model-management", tags=["Admin - Model Management"])
