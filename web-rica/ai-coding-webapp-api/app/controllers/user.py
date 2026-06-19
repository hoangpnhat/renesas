#  Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
from typing import Optional, cast

from fastapi import HTTPException, status
from odmantic import ObjectId

from caching.redis import _redis_client as redis_cli
from core.config import settings
from crud import crud_user
from crud.refactor_base.mixins import ComprehensiveRequest, PaginationResponse
from models.user import User as MongoUser
from models.user import UserSetting
from schemas import UserSettingsBase
from schemas.user import UserCreate, VerifiedUser
from utils.handle_exception import ApplicationException


class UserController:
    def __init__(self, verified_user: VerifiedUser | None = None):
        self.crud = crud_user.user
        self.verified_user = verified_user

    async def fetch_user_by_email(self, raise_error: bool = True) -> MongoUser | None:
        if self.verified_user is None:
            raise ApplicationException("PERMISSION_DENIED")
        user_email = self.verified_user.email
        if not user_email:
            if raise_error:
                raise ApplicationException("NOT_EXISTED_ERROR")
            return None
        user = await self.crud.get_by_email(email=user_email)
        if not user and raise_error:
            raise ApplicationException("NOT_EXISTED_ERROR")
        return user

    async def fetch_user_by_id(self, user_id: ObjectId | str, raise_error: bool = True) -> MongoUser | None:
        user = await self.crud.get_by_id(id=user_id)
        if not user and raise_error:
            raise ApplicationException("NOT_EXISTED_ERROR")
        return user

    async def validate_curr_user(self, alert_existing: bool = False) -> MongoUser | None:
        if self.verified_user is None:
            raise ApplicationException("PERMISSION_DENIED")
        key_cached = self.verified_user.email or self.verified_user.user_id
        if not key_cached:
            raise ApplicationException("NOT_EXISTED_ERROR")

        if redis_cli and (cached_data := await redis_cli.get(str(key_cached))):
            return MongoUser(**cached_data)

        if isinstance(key_cached, str):  # email
            user_data = await self.fetch_user_by_email()
        else:  # user_id: ObjectId
            user_data = await self.fetch_user_by_id(user_id=key_cached)

        if not user_data:
            raise ApplicationException("NOT_EXISTED_ERROR")

        if alert_existing:
            raise ApplicationException("EXISTED_ERROR")

        if not user_data.is_active:
            raise ApplicationException("PERMISSION_DENIED", detail="Current user is inactive")

        if redis_cli:
            await redis_cli.set(key=str(key_cached), value=user_data.model_dump())
        return user_data

    async def get_user_settings(self) -> Optional[UserSetting]:
        user = await self.validate_curr_user()
        if user:
            return user.settings
        return None

    async def update_user_settings(self, obj_in: UserSettingsBase) -> UserSetting:
        user = cast(MongoUser, await self.validate_curr_user())
        if user.settings is None:
            raise ApplicationException("NOT_EXISTED_ERROR", detail="User settings not initialised")

        # Invalidate cache
        if redis_cli:
            await redis_cli.delete(user.email)

        # Merge: existing settings + only explicitly set fields from input
        updated_settings = {
            **user.settings.model_dump(),
            **obj_in.model_dump(exclude_unset=True)
        }

        updated_user: MongoUser = await self.crud.transaction_wrapper(
            self.crud.update,
            db_obj=user,
            obj_in={"settings": updated_settings}
        )
        if updated_user.settings is None:
            raise ApplicationException("NOT_EXISTED_ERROR", detail="User settings unavailable after update")
        return updated_user.settings

    async def login(self) -> MongoUser:
        user = await self.fetch_user_by_email(raise_error=False)
        if user:
            return user
        return await self._create_user_record()

    async def signup(self) -> MongoUser:
        user = await self.fetch_user_by_email(raise_error=False)
        if user:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="The user with this username already exists in the system.",
            )
        return await self._create_user_record()

    async def _create_user_record(self) -> MongoUser:
        if self.verified_user is None:
            raise ApplicationException("PERMISSION_DENIED")
        if self.verified_user.name is None:
            raise ApplicationException("PERMISSION_DENIED")
        if self.verified_user.email is None:
            raise ApplicationException("PERMISSION_DENIED")
        return await self.crud.create(
            obj_in=UserCreate(
                email_validated=True,
                is_active=True,
                full_name=self.verified_user.name,
                password=settings.FIRST_SUPERUSER_PASSWORD,
                email=self.verified_user.email,
            )
        )

    async def search_members_partial(self, word: str, limit: int = 10) -> PaginationResponse:
        user = cast(MongoUser, await self.validate_curr_user())
        # constructed params into ComprehensiveRequest
        constructed_request = ComprehensiveRequest(
            limit=limit,
            sort_field="modified",
            search_text=word,
        )
        # call pagination helper
        results, _ = await self.crud.paginate(
            user_id=str(user.id),
            request=constructed_request
        )
        return results

    async def login_with_cache(self) -> MongoUser:
        if self.verified_user is None:
            raise ApplicationException("PERMISSION_DENIED")
        key = self.verified_user.email
        if key is None:
            raise ApplicationException("PERMISSION_DENIED")
        if redis_cli is None:
            raise RuntimeError("Redis client is not initialised — cannot execute login_with_cache")
        if cached := await redis_cli.get(key):
            user = MongoUser(**cached)
            if not user.is_active:
                raise ApplicationException("PERMISSION_DENIED", detail="Current user is inactive")
            return user

        user = await self.login()

        if not user.is_active:
            raise ApplicationException("PERMISSION_DENIED", detail="Current user is inactive")

        await redis_cli.set(key=key, value=user.model_dump(), expire=3600 * 4)
        return user
