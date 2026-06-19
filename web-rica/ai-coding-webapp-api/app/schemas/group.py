from pydantic import BaseModel, Field
from typing import Optional, Union

from odmantic.bson import ObjectId
from const.common import ShareType
from models.user import SharingRoles
from datetime import datetime

from .share import ChildrenResource

class MemberModification(BaseModel):
    user_id: Union[str, ObjectId]
    added_by: Optional[ObjectId] = None
    joined_at: Optional[datetime] = None
    role: Optional[str] = SharingRoles.VIEWER
class MemberResponse(MemberModification):
    full_name: Optional[str] = None
class GroupCreate(BaseModel):
    name: str
    description: Optional[str] = None
    visibility: Optional[str] = Field(default=ShareType.PRIVATE)
    members: Optional[list[MemberModification]] = None
class GroupUpdateGeneral(BaseModel):
    name: Optional[str] = None
    description: Optional[str] = None
    visibility: Optional[str] = None
    members: Optional[list[MemberModification]] = None
class GroupUpdateMember(BaseModel):
    members: Optional[list[MemberModification]] = None
class GroupUpdateAdvanced(BaseModel):
    is_deleted: Optional[bool] = None
    is_archived: Optional[bool] = None
class GroupResponse(BaseModel):
    id: str | ObjectId = Field(default_factory=lambda x: str(x))
    owner_id: str | ObjectId = Field(default_factory=lambda x: str(x))
    name: str
    description: Optional[str] = None
    visibility: str
    is_deleted: bool = False
    is_archived: bool = False
    members: list[MemberResponse] = Field(default_factory=list)
    
    created_at: Optional[datetime] = None
    last_modified: Optional[datetime] = None

    # support UI
    owner_name: Optional[str] = None
    resources: list[ChildrenResource] = []
    current_user_role: Optional[str] = None
    is_owner: bool = Field(False, description="Current user is owner")
    is_admin: bool = Field(False, description="Current user is owner or admin")
    can_invite: bool = Field(False, description="Can invite new members")
    can_remove_members: bool = Field(False, description="Can remove members")
    can_edit: bool = Field(False, description="Can edit group settings")
    can_delete: bool = Field(False, description="Can delete group")
    can_archive: bool = Field(False, description="Can archive group")
    can_unarchive: bool = Field(False, description="Can unarchive group")
    can_leave: bool = Field(True, description="Can leave group")
    can_upload_files: bool = Field(False, description="Can upload files to group")