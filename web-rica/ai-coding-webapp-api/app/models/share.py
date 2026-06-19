from odmantic import ObjectId, Field, Index, query

from db.base_class import Base
from utils.helpers import datetime_now_sec, datetime
from const.common import SharingRoles, can, OwnerType

class Share(Base):
    # What is being shared
    shareable_id: ObjectId = Field(..., description="ID of resource being shared")
    shareable_type: str = Field(..., description="Type: file, knowledge_base, group")
    
    # Who is sharing
    shared_by: ObjectId = Field(..., description="User ID who created the share")
        
    # Who can access
    target_id: ObjectId
    target_type: str = Field(default=OwnerType.USER, description="Being shared as recipient or as group")
    
    # Permissions
    role: SharingRoles = Field(default=SharingRoles.VIEWER)
    
    # Timestamps
    created_at: datetime = Field(default_factory=datetime_now_sec)
    last_modified: datetime = Field(default_factory=datetime_now_sec)

    @property
    def can_write(self) -> bool:
        return can(self.role, "write")

    @property
    def can_read(self) -> bool:
        return can(self.role, "read")

    def has_permission(self, action: str) -> bool:
        return can(self.role, action)
    
    model_config = {  # type: ignore[typeddict-unknown-key]
        "collection": "shares",
        "indexes": lambda: [

            Index(
                query.desc(Share.created_at),
                query.desc(Share.last_modified)
            ),
            # Find shares by target
            Index(
                Share.target_id,
                Share.target_type,
                query.desc(Share.created_at)
            ),
            # Find shares by shareable resource
            Index(
                Share.shareable_id,
                Share.shareable_type,
                Share.target_id,
                Share.target_type
            )
        ]
    }
