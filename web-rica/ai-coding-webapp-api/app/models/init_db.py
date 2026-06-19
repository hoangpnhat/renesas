from fastapi import Depends

import crud
import schemas
from core.config import settings
from db.session import get_db


async def init_db(db=Depends(get_db)) -> None:
    user = await crud.user.get_by_email(db, email=settings.FIRST_SUPERUSER)
    if not user:
        # Create user auth
        user_in = schemas.UserCreate(
            email=settings.FIRST_SUPERUSER,
            password=settings.FIRST_SUPERUSER_PASSWORD,
            is_superuser=True,
            full_name=settings.FIRST_SUPERUSER,
        )
        user = await crud.user.create(db, obj_in=user_in)  # noqa: F841
