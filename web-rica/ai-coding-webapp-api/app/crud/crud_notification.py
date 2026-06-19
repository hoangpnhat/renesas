from odmantic import ObjectId
from typing import Any, Optional

from models.notification import Notification as MongoNotification
from schemas.notification import NotificationCreate, NotificationUpdate
from const.notification import NotificationStatus, DEFAULT_GROUP_INVITATION, NotificationType
from const.common import ResourceType

from .refactor_base.crud_operations import MongoCRUD


class CRUDNotification(MongoCRUD[MongoNotification, NotificationCreate, NotificationUpdate]):

    async def get_by_id(self, id: str | ObjectId) -> MongoNotification | None:
        return await self.single_complex_get(to_query=(MongoNotification.id == ObjectId(id)))  # type: ignore[arg-type]
    
    async def mark_read(self, noti: MongoNotification, session: Any) -> MongoNotification:
        return await self.update(db_obj=noti, obj_in=NotificationUpdate(status=NotificationStatus.READ), session=session)
    
    async def get_user_latest_notifications(self, user_id: ObjectId | str, limit: int = 20) -> list[MongoNotification]:
        to_query = MongoNotification.user_id == ObjectId(user_id)
        return await self.engine.find(
            self.model,
            to_query,
            limit=limit
        )
    
    async def create_notification(
            self,
            user_id: ObjectId | list[ObjectId],
            notification_type: NotificationType,
            title: str,
            message: str,
            resource_type: ResourceType,
            resource_id: ObjectId,
            actor_id: Optional[ObjectId] = None,
            action_url: Optional[str] = None,
            metadata: Optional[dict] = None,
            session: Any = None
        ) -> MongoNotification | list[MongoNotification]:
            """            
            Args:
                user_id: Single or multiple recipient IDs
                notification_type: Type of notification from NotificationType enum
                title: Notification title
                message: Notification message body
                resource_type: Type of resource (GROUP, FILE, KB, etc.)
                resource_id: ID of the related resource
                actor_id: Optional ID of the user who triggered the notification
                action_url: Optional URL for notification action
                metadata: Optional additional data as dict
                session: Add session for rollback
            """
            user_ids = self._normalize_user_ids(user_id)
            
            notifications: list[MongoNotification] = [
                self._build_notification(
                    uid,
                    notification_type,
                    title,
                    message,
                    resource_type,
                    resource_id,
                    actor_id,
                    action_url,
                    metadata,
                )
                for uid in user_ids
            ]
            
            await self._save_notifications(notifications, session=session)
            return notifications[0] if len(notifications) == 1 else notifications

    def _normalize_user_ids(self, user_id: ObjectId | list[ObjectId]) -> list[ObjectId]:
        """Convert single or list of user IDs to list of ObjectIds."""
        if isinstance(user_id, (list, set)):
            return [ObjectId(uid) for uid in user_id]
        return [ObjectId(user_id)]

    def _build_notification(
        self,
        user_id: ObjectId,
        notification_type: NotificationType,
        title: str,
        message: str,
        resource_type: ResourceType,
        resource_id: ObjectId,
        actor_id: Optional[ObjectId],
        action_url: Optional[str],
        metadata: Optional[dict],
    ) -> MongoNotification:
        
        return MongoNotification(  # type: ignore[call-arg]
            user_id=user_id,
            notification_type=notification_type,
            actor_id=actor_id,
            resource_id=resource_id,
            resource_type=resource_type,
            title=title,
            message=message,
            metadata=metadata or {},
            action_url=action_url,
        )

    async def _save_notifications(self, notifications: list[MongoNotification], session: Any = None):
        if session:
            return await self.engine.save_all(notifications, session=session)
        return await self.transaction_wrapper(self._save_multiple_notifications, notifications)

    async def _save_multiple_notifications(self, notifications: list[MongoNotification], session: Any = None):
        return await self.engine.save_all(notifications, session=session)

    async def update_metadata_flag(
        self,
        resource_id: str | ObjectId,
        user_id: str | ObjectId,
        flag_key: str,
        flag_value: Any,
        session: Any = None
    ) -> int:
        """Update metadata flag for notifications matching resource_id and user_id"""

        query = (MongoNotification.resource_id == ObjectId(resource_id)) & (
            MongoNotification.user_id == ObjectId(user_id)
        )

        notifications = await self.engine.find(self.model, query)

        if not notifications:
            return 0

        for noti in notifications:
            noti.metadata = {**(noti.metadata or {}), flag_key: flag_value}

        await self.engine.save_all(notifications, session=session)
        return len(notifications)

notification_crud = CRUDNotification(model = MongoNotification)