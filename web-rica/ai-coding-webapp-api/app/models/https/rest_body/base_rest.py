from typing import Generic, Optional, TypeVar
from pydantic import BaseModel
from core.config import settings

T = TypeVar("T")


class StatusResponse(BaseModel):
    state: str = "Success"
    message: str = "Task completed"


class MetaData(BaseModel):
    all_records: int
    limit: int = settings.MULTI_MAX
    page: int
    total_page: int


class BaseBody(BaseModel):
    status: StatusResponse


class GetBaseBody(BaseBody):
    metadata: Optional[MetaData] = None


class GetBaseBodyResponse(GetBaseBody, Generic[T]):
    results: T
