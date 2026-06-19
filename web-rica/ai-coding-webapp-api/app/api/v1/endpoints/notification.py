from fastapi import APIRouter, Depends
from dependencies import NotificationControllerDep
from utils.decorators import log_api
from crud.refactor_base.mixins import CommonRequest


router = APIRouter()

@router.get("")
@log_api
async def list_notifications(controller: NotificationControllerDep,
                            request: CommonRequest = Depends()):
    return await controller.get_user_notifications(request = request)

@router.post("/{notification_id}/read")
@log_api
async def mark_notification_read(notification_id: str, controller: NotificationControllerDep):
    return await controller.mark_read(notification_id=notification_id)
