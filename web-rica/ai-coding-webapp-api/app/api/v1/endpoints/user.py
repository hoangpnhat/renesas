#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

from fastapi import APIRouter, Query

from models import UserSetting, User
from schemas import UserSettingsBase

from dependencies import UserControllerDep

router = APIRouter()

@router.post("/")
async def create_user(
    controller: UserControllerDep
) -> User:
    return await controller.signup()


@router.get("/me", response_model=User)
async def get_current_user(controller: UserControllerDep):
    return await controller.login()


@router.get("/me/settings", response_model=UserSetting)
async def get_user_settings(controller: UserControllerDep):
    return await controller.get_user_settings()


@router.patch("/me/settings", response_model=UserSetting)
async def update_user_settings(
    controller: UserControllerDep,
    request_body: UserSettingsBase
):
    return await controller.update_user_settings(obj_in=request_body)


@router.get("/search")
async def searching_users(
    controller: UserControllerDep,
    query: str = Query("hai", description="Partial search in name and email"),
    limit: int = Query(5, description="Limit search")
):
    return await controller.search_members_partial(word=query, limit=limit)
