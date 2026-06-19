from datetime import datetime
from typing import Optional

from bson import ObjectId
from pydantic import BaseModel, Field, ConfigDict

from const.common import CommonStatus, PromptType, ShareType, OwnerType, SharingRoles
from crud.refactor_base.mixins.pagination.models import CommonRequest
from models.prompt import PromptFileInfo
from utils.helpers import datetime_now_sec


class PromptBase(BaseModel):
    name: str
    description: Optional[str] = None


# Input models
class PromptCreate(PromptBase):
    owner_id: Optional[str | ObjectId] = None
    created_at: datetime = Field(default_factory=datetime_now_sec)
    type: str = Field(default=PromptType.PROMPT.value)
    prompt_file_info: Optional[PromptFileInfo] = None

    model_config = ConfigDict(arbitrary_types_allowed=True)


class PromptUpdate(BaseModel):
    name: Optional[str] = None
    description: Optional[str] = None
    status: Optional[CommonStatus] = None
    visibility: Optional[ShareType] = None
    prompt_file_info: Optional[PromptFileInfo] = None


class PromptShareRequest(BaseModel):
    target_id: str
    target_type: OwnerType
    role: Optional[SharingRoles] = SharingRoles.VIEWER


# Output models
class PromptResponse(PromptUpdate):
    id: Optional[str] = None
    status: CommonStatus
    visibility: ShareType
    type: str = Field(default=PromptType.PROMPT.value)

    owner_id: str
    owner_name: Optional[str] = None
    last_modified: Optional[datetime] = None
    created_at: datetime

    # Linear permissions (as requested)
    current_user_role: Optional[str] = None
    can_view: bool = True
    can_add_files: bool = False
    is_owner: Optional[bool] = None
    can_remove_files: bool = False
    can_edit: bool = False
    can_delete: bool = False
    can_share: bool = False
    can_archive: bool = False

    is_deleted: bool = False
    is_archived: bool = False


class PromptTruncateResponse(PromptBase):
    id: str
    created_at: datetime
    last_modified: Optional[datetime] = None
    type: str = Field(default=PromptType.PROMPT.value)


class PromptCommonRequest(CommonRequest):
    sort_field: str = Field("last_modified", description="Field to sort by")
