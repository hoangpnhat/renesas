from typing import Any, cast

from fastapi import HTTPException, status
from odmantic import ObjectId, query

from core.loguru import logger
from crud import notification_crud
from models.notification import Notification as MongoNotification
from schemas.user import VerifiedUser
from utils.handle_exception import ApplicationException
from utils.helpers import empty_response
from utils.decorators import require_user_validator
from crud.refactor_base.mixins import CommonRequest

class NotificationController:
    def __init__(self, verified_user: VerifiedUser | None = None):
        self.crud = notification_crud
        self.verified_user = verified_user

    async def fetch_notification_by_id(self, noti_id: str | ObjectId, raise_error: bool=True) -> MongoNotification | None:
        notification = await self.crud.get_by_id(id=noti_id)
        if not notification and raise_error:
            raise ApplicationException('NOT_EXISTED_ERROR')
        else:
            return cast(MongoNotification | None, notification)

    @require_user_validator()
    async def get_user_notifications(self, request: CommonRequest, **kwargs):
        try:
            user_id = kwargs.get("id")
            results = await self.crud.get_user_latest_notifications(user_id=user_id, limit=request.limit)  # TODO: move it to filters/implement

            if not results:
                return empty_response({})

            data = []
            for r in results:
                data.append({
                    "id": str(r.id),
                    "title": r.title,
                    "message": r.message,
                    "status": r.status,
                    "notification_type": r.notification_type,
                    "resource_type": r.resource_type,
                    "resource_id": str(r.resource_id) if getattr(r, "resource_id", None) else None,
                    "actor_id": str(r.actor_id) if getattr(r, "actor_id", None) else None,
                    "metadata": r.metadata or {},
                    "action_url": r.action_url,
                    "created_at": r.created_at.isoformat() if getattr(r, "created_at", None) else None,
                })

            return {"data": data, "pagination": {"page": 1, "limit": request.limit, "total": len(data)}}

        except ApplicationException:
            raise
        except Exception as e:
            logger.error(f"Error listing notifications for user: {e}")
            raise HTTPException(status_code=status.HTTP_500_INTERNAL_SERVER_ERROR, detail=str(e))

    @require_user_validator()
    async def mark_read(self, notification_id: str, **kwargs):
        curr_user_id = kwargs.get("id")
        noti = cast(MongoNotification, await self.fetch_notification_by_id(noti_id=notification_id))

        if str(noti.user_id) != str(curr_user_id):
            raise ApplicationException("PERMISSION_DENIED")

        return await self.crud.transaction_wrapper(self.crud.mark_read, noti)