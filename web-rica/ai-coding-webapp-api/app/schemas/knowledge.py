#  Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
from pydantic import BaseModel, Field
from odmantic.bson import ObjectId
from typing_extensions import Optional

from datetime import datetime
from const.common import ShareType, SharingRoles, OwnerType
from crud.refactor_base.mixins import CommonRequest, ComprehensiveRequest

class FileJunctionResponse(BaseModel):
    file_id: ObjectId
    added_at: datetime
    file_name: Optional[str] = None


class KnowledgeRequest(CommonRequest):
    sort_field: str = Field("last_modified", description="Field to sort by")


class KnowledgeComprehensiveRequest(ComprehensiveRequest):
    sort_field: str = Field("last_modified", description="Field to sort by")


class KnowledgeCreate(BaseModel):
    name: str
    description: Optional[str] = None
    files: list[str | dict] = Field(default_factory=list)
    visibility: Optional[str] = Field(default=ShareType.PRIVATE, description="private or public")
    owner_id: Optional[str] = None
    
class KnowledgeUpdateGeneral(BaseModel):
    name: Optional[str] = None
    description: Optional[str] = None
    files: list[str | dict] = Field(default_factory=list)
    visibility: Optional[str] = None
    owner_id: Optional[str] = None
    is_deleted: Optional[bool] = None
    is_archived: Optional[bool] = None
    
class KnowledgeUpdateFiles(BaseModel):
    files: Optional[list[str]] = None

class KnowledgeShareRequest(BaseModel):
    target_id: ObjectId
    target_type: OwnerType
    role: Optional[SharingRoles] = SharingRoles.VIEWER

class KnowledgeResponse(BaseModel):
    id: str | ObjectId = Field(default_factory=lambda x: str(x))
    name: str
    description: Optional[str] = None

    created_at: datetime | None = None
    last_modified: datetime | None = None

    owner_id: str | ObjectId = Field(default_factory=lambda x: str(x))
    owner: str
    # enrich
    current_user_role: Optional[str] = None
    is_owner: bool = False
    can_view: bool = True
    can_add_files: bool = False
    can_remove_files: bool = False
    can_edit: bool = False
    can_delete: bool = False
    can_share: bool = False
    can_archive: bool = False
    visibility: str

    is_deleted: bool = False
    is_archived: bool = False

    files: list[FileJunctionResponse] = Field(default_factory=list)