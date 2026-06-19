from odmantic import ObjectId, Field, query
from typing import Optional
from pymongo import IndexModel, DESCENDING


from db.base_class import Base
from const.notification import NotificationStatus
from utils.helpers import datetime, datetime_now_sec

class Notification(Base):
    user_id: ObjectId = Field(..., description="User receiving the notification")
    notification_type: str = Field(..., description="Type of notification")
    actor_id: Optional[ObjectId] = Field(None, description="User who triggered the notification")
    resource_id: ObjectId = Field(..., description="Related resource ID")
    resource_type: str = Field(..., description="Type: file, knowledge_base, group")
    
    # Message content
    title: str = Field(..., description="Notification title")
    message: str = Field(..., description="Notification message")
    
    # Metadata
    metadata: Optional[dict] = Field(default_factory=dict, description="Additional context")
    status: str = Field(default=NotificationStatus.UNREAD)

    # Redirect
    action_url: Optional[str] = Field(None, description="URL to navigate on click")
    
    created_at: datetime = Field(default_factory=datetime_now_sec)
    
    model_config = {  # type: ignore[typeddict-unknown-key]
        "collection": "notifications",
        "indexes": lambda: [
            IndexModel(
                [
                "user_id",
                ("created_at", DESCENDING)]
                ),
            IndexModel(
                [("created_at", DESCENDING)],
                expireAfterSeconds=7776000,  # 90 days
            ),
        ]
    }