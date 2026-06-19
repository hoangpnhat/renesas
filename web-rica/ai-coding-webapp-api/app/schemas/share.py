from pydantic import BaseModel, Field
from typing import Optional
from odmantic import ObjectId
from models.share import Share as MongoShare
from utils.helpers import datetime_now_sec, datetime
from const.common import SharingRoles, OwnerType


class ShareCreate(BaseModel):
    shareable_id: ObjectId
    shareable_type: str
    shared_by: ObjectId
    target_id: ObjectId
    target_type: str = OwnerType.USER
    role: Optional[SharingRoles] = SharingRoles.VIEWER
    created_at: datetime = Field(default_factory=datetime_now_sec)
    last_modified: datetime = Field(default_factory=datetime_now_sec)


class ChildrenResource(BaseModel):
    resource_id: str
    resource_type: str = "knowledge"
    resource_name: str | None = None
    last_modified: datetime | None = None
    role: Optional[str] = None
class ShareUpdate(BaseModel):
    is_revoked: bool