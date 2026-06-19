#  Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
from datetime import datetime
from typing import Optional

from odmantic import ObjectId, Field, Index, query

from db.base_class import Base
from utils.helpers import datetime_now_sec
from const.common import ShareType
from models.file import FileJunction


class Knowledge(Base):
    # identity fields
    name: str = Field(...)
    description: Optional[str] = None
    created_at: datetime = Field(default_factory=datetime_now_sec)
    last_modified: datetime = Field(default_factory=datetime_now_sec)

    # ownership fields
    owner_id: ObjectId = Field(...)
    visibility: str = Field(default=ShareType.PRIVATE)

    files: list[FileJunction] = Field(default_factory=list)

    # status fields
    is_deleted: bool = Field(default=False)
    is_archived: bool = Field(default=False)

    model_config = {  # type: ignore[typeddict-unknown-key]
        "indexes": lambda: [
            Index(
                Knowledge.owner_id,
                Knowledge.is_deleted,
                query.desc(Knowledge.last_modified)
            ),
            Index(
                query.desc(Knowledge.last_modified),
                query.desc(Knowledge.created_at)
            )
        ]
    }
