from datetime import datetime
from typing import Optional

from odmantic import ObjectId
from pydantic import BaseModel, Field


class ApiKeyCreate(BaseModel):
    name: str = Field(..., min_length=1, max_length=100)
    expires_at: datetime


class ApiKeyResponse(BaseModel):
    id: ObjectId
    name: str
    key_prefix: str
    created_at: datetime
    expires_at: datetime
    last_used_at: Optional[datetime]
    revoked: bool

    model_config = {"from_attributes": True}


class ApiKeyCreatedResponse(ApiKeyResponse):
    key: str  # plaintext — returned once only on creation
