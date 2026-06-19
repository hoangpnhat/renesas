from odmantic import ObjectId, Field, Index, query
from typing import Optional

from db.base_class import Base
from const.common import ShareType
from utils.helpers import datetime_now_sec, datetime

from .user import Members

class Group(Base):
    # properties field
    name: str = Field(...)
    description: Optional[str] = None

    # ownership fields
    owner_id: ObjectId = Field(...)

    # membership
    members: list[Members] = Field(default_factory=list)
    visibility: str = Field(default=ShareType.PRIVATE)

    # soft-deleted
    is_deleted: bool = Field(default=False)
    is_archived: bool = Field(default=False)

    created_at: datetime = Field(default_factory=datetime_now_sec)
    last_modified: datetime = Field(default_factory=datetime_now_sec)

    model_config = {  # type: ignore[typeddict-unknown-key]
        "collection": "groups",
        "indexes": lambda: [
            Index(Group.owner_id),
            Index(Group.is_deleted),
            Index(Group.visibility),
            Index(
                query.desc(Group.last_modified),
                query.desc(Group.created_at)
            ),
            Index(
                Group.owner_id,
                Group.is_deleted,
                query.desc(Group.last_modified)
            ),
        ]
    }
