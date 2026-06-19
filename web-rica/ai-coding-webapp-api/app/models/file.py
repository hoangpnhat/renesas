#  Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
from datetime import datetime
from typing import Optional

from odmantic import ObjectId, Field, Index, EmbeddedModel, query

from db.base_class import Base
from utils.helpers import datetime_now_sec


class Files(Base):
    author_id: ObjectId = Field(default_factory=ObjectId, index=True)
    file_name: str = Field(index=True)
    uploaded_at: datetime = Field(default_factory=datetime_now_sec)
    modified: datetime = Field(default_factory=datetime_now_sec)
    size: Optional[int] = None
    extension: Optional[str] = None
    status: Optional[str] = None
    folder_path: Optional[str] = Field(default=None)
    total_pages: Optional[int] = Field(default=None)
    error: Optional[str] = Field(None)

    model_config = {  # type: ignore[typeddict-unknown-key]
        "indexes": lambda: [
            Index(Files.author_id, Files.file_name),
            Index(query.desc(Files.modified), query.desc(Files.uploaded_at))
        ]
    }


class FileJunction(EmbeddedModel):
    file_id: ObjectId
    added_at: datetime = Field(default_factory=datetime_now_sec)
