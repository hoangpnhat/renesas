from odmantic import ObjectId
from pydantic import BaseModel, ConfigDict, Field
from typing import Optional, Dict, Any

from utils.helpers import datetime, datetime_now_sec
from const.notification import NotificationStatus


class NotificationBase(BaseModel):
    user_id: Optional[ObjectId] = None
    notification_type: Optional[str] = None
    actor_id: Optional[ObjectId] = None
    resource_id: Optional[ObjectId] = None
    resource_type: Optional[str] = None

    # Message content
    title: Optional[str] = None
    message: Optional[str] = None

    # Metadata
    metadata: Dict[str, Any] = Field(default_factory=dict)
    status: Optional[str] = Field(default=None)

    # Redirect
    action_url: Optional[str] = None


class NotificationCreate(NotificationBase):
    status: Optional[str] = Field(default=NotificationStatus.UNREAD)
    created_at: Optional[datetime] = Field(default_factory=datetime_now_sec)


class NotificationUpdate(NotificationCreate):
    pass

class NotificationResponse(BaseModel):
    id: str
    user_id: str
    notification_type: Optional[str] = None
    actor_id: Optional[str] = None
    resource_id: Optional[str] = None
    resource_type: Optional[str] = None
    title: Optional[str] = None
    message: Optional[str] = None
    metadata: Dict[str, Any] = Field(default_factory=dict)
    status: Optional[str] = Field(default=None)
    action_url: Optional[str] = None
    created_at: Optional[datetime] = Field(default_factory=datetime_now_sec)