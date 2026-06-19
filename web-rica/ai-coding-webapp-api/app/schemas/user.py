#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

from typing import List

from odmantic import ObjectId
from pydantic import BaseModel, ConfigDict, EmailStr, SecretStr, field_validator, Field
from typing import Annotated, Literal, Optional
from models.user import UserSetting
from const.common import CommonLang, CommonMode

class UserLogin(BaseModel):
    username: str
    password: str


class UserBase(BaseModel):
    email: EmailStr | None = None
    email_validated: bool | None = False
    is_active: bool | None = False
    is_superuser: bool | None = False
    full_name: str = ""


class UserCreate(UserBase):
    email: EmailStr | str
    password: Annotated[str | None, Field(min_length=8, max_length=64)] = None  # n


class UserSettingsBase(BaseModel):
    preferred_language: Optional[CommonLang] = None
    mode: Optional[CommonMode] = None

class UserSettingsCreate(UserSettingsBase):
    user_id: ObjectId

class UserSettingsUpdate(UserSettingsCreate):
    pass

# Properties to receive via API on update
class UserUpdate(UserBase):
    original: Annotated[str | None, Field(min_length=8, max_length=64)] = None  # noqa
    password: Annotated[str | None, Field(min_length=8, max_length=64)] = None  # noq


class UserInDBBase(UserBase):
    id: ObjectId | None = None
    model_config = ConfigDict(from_attributes=True)


# Additional properties to return via API
class User(UserInDBBase):
    """User model returned via any authenticated method, with database checks"""
    hashed_password: bool = Field(default=False)
    totp_secret: bool = Field(default=False)
    model_config = ConfigDict(populate_by_name=True)

    @field_validator("hashed_password", mode="before")
    def evaluate_hashed_password(cls, hashed_password):
        if hashed_password:
            return True
        return False

    @field_validator("totp_secret", mode="before")
    def evaluate_totp_secret(cls, totp_secret):
        if totp_secret:
            return True
        return False


# Additional properties stored in DB
class UserInDB(UserInDBBase):
    hashed_password: SecretStr | None = None
    totp_secret: SecretStr | None = None
    totp_counter: int | None = None


class UserAzureAD(BaseModel):
    id: str
    name: str
    preferred_username: str
    roles: List[str]

class VerifiedUser(BaseModel):
    email: Optional[str] = None
    user_id: Optional[ObjectId] = None
    roles: Optional[list[str]] = None
    name: Optional[str] = None

class UserResponse(User):
    settings: Optional[UserSetting]
    roles: list[str]
