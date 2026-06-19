from typing import Optional

from odmantic import ObjectId
from pydantic import BaseModel, ConfigDict, Field

from const.common import CommonStatus
from utils.helpers import datetime, datetime_now_sec


class FileBase(BaseModel):
    file_name: Optional[str] = None
    author_id: Optional[str | ObjectId] = Field(None)

    model_config = ConfigDict(from_attributes=True)


class FileCreate(FileBase):
    size: Optional[int] = Field(None)
    extension: str = Field(default='unknown')
    status: Optional[str] = Field(default=None)
    folder_path: Optional[str] = Field(default=None)
    total_pages: Optional[int] = Field(default=None)
    uploaded_at: Optional[datetime] = Field(default_factory=datetime_now_sec)


class FileUpdate(FileCreate):
    modified: Optional[datetime] = Field(default_factory=datetime_now_sec)
    error: Optional[str] = Field(default=None)


class FileInDB(FileUpdate):
    id: str
    author_id: str
    owner: Optional[str] = None
    status: Optional[str] = CommonStatus.PROCESSING

    is_owner: Optional[bool] = False


class FileAccess(BaseModel):
    id: Optional[ObjectId] = None
    user_id: Optional[ObjectId] = None
    role: Optional[str] = "viewer"


class FilePreviewMetaResponse(BaseModel):
    file_id: str
    total_pages: int
