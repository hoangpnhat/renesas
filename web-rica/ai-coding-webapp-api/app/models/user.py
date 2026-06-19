#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

from __future__ import annotations

from datetime import datetime
from typing import Any, Optional, List, TYPE_CHECKING, Union

from odmantic import ObjectId, Field, EmbeddedModel, Index, query
from pydantic import BaseModel, EmailStr

from const.common import SharingRoles, CommonMode, CommonLang
from const.user import ENTRA_ROLES
from db.base_class import Base
from utils.helpers import datetime_now_sec

if TYPE_CHECKING:
    pass


class User(Base):
    created: datetime = Field(default_factory=datetime_now_sec)
    modified: datetime = Field(default_factory=datetime_now_sec)
    full_name: str = Field(default="")
    email: EmailStr
    hashed_password: Any = Field(default=None)
    is_active: bool = Field(default=True)
    is_superuser: bool = Field(default=False)
    totp_secret: Any = Field(default=None)
    totp_counter: Optional[int] = Field(default=None)
    email_validated: bool = Field(default=False)
    refresh_tokens: List[ObjectId] = Field(default=[])
    # dialogs_ids: List[ObjectId] = Field(default=[])
    settings: Optional[UserSetting] = Field(default={})

    model_config = {  # type: ignore[typeddict-unknown-key]
        "indexes": lambda: [
                Index(query.desc(User.created)),
                Index(User.email, query.desc(User.modified))
        ]
    }

class UserSettings(Base):
    user_id: ObjectId = Field(default=None)
    preferred_language: str = Field(default="en")

class UserSetting(EmbeddedModel):
    preferred_language: Optional[str] = Field(default=CommonLang.ENG)
    mode: Optional[str] = Field(default=CommonMode.LIGHT_MODE)

class UserAzureAD(BaseModel):
    id: str
    name: str
    preferred_username: str
    roles: List[str]


class UserAzureADInfo(User):
    name: str = Field(default="")
    preferred_username: str = Field(default="")
    roles: List[Union[str, ENTRA_ROLES]] = Field(default=[])


class Members(EmbeddedModel):
    user_id: ObjectId
    added_by: Optional[ObjectId] = None
    joined_at: Optional[datetime] = None
    role: str = Field(default=SharingRoles.VIEWER)