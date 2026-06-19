#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

from typing import Dict, Any, Optional

from bson import ObjectId

from core.security import get_password_hash, verify_password
from crud.refactor_base.crud_operations import MongoCRUD
from models.user import User
from schemas.user import UserCreate, UserUpdate
from filters.implementations import UserDynamicQueryBuilder

class CRUDUser(MongoCRUD[User, UserCreate, UserUpdate]):
    async def get_by_email(self, *, email: str) -> User | None:
        return await self.engine.find_one(User, User.email == email)

    async def get_by_id(self, id: ObjectId | str) -> Optional[User]:
        return await self.single_plain_get(id = id)

    async def authenticate(
            self, *, email: str, password: str
    ) -> User | None:  # noqa
        user = await self.get_by_email(email=email)
        if not user:
            return None
        if not verify_password(
                plain_password=password, hashed_password=user.hashed_password
        ):  # noqa
            return None
        return user

    async def create(self, *, obj_in: UserCreate) -> User:  # type: ignore[override]
        user: Dict[str, Any] = {
            **obj_in.model_dump(),
            "email": obj_in.email,
            "hashed_password": (
                get_password_hash(obj_in.password) if obj_in.password is not None else None
            ),
            "is_superuser": obj_in.is_superuser,
        }

        return await self.engine.save(User(**user))

    @staticmethod
    def is_active(user: User) -> bool:
        return user.is_active

    @staticmethod
    def is_superuser(user: User) -> bool:
        return user.is_superuser


user = CRUDUser(model=User,
                query_builder=UserDynamicQueryBuilder)