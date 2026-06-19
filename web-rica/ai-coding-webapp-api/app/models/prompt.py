from datetime import datetime
from typing import Optional

from odmantic import ObjectId, Field, EmbeddedModel
from pymongo import IndexModel, ASCENDING, DESCENDING

from const.common import CommonStatus, PromptType, ShareType
from db.base_class import Base
from utils.helpers import datetime_now_sec


class PromptFileInfo(EmbeddedModel):
    prompt_file_name: Optional[str] = None
    prompt_file_path: Optional[str] = None
    prompt_file_size: Optional[int] = None
    prompt_file_extension: Optional[str] = None


class Prompt(Base):
    # permission and context
    owner_id: ObjectId = Field(...)

    # core
    name: str
    description: Optional[str] = None

    # status and classification
    status: CommonStatus = Field(default=CommonStatus.ACTIVE)
    visibility: ShareType = Field(default=ShareType.PRIVATE)

    type: PromptType

    # file management
    prompt_file_info: PromptFileInfo = Field(default_factory=PromptFileInfo)

    # time management
    created_at: datetime = Field(default_factory=datetime_now_sec)
    last_modified: datetime = Field(default_factory=datetime_now_sec)

    model_config = {  # type: ignore[typeddict-unknown-key]
        "collection": "prompts",
        "indexes": lambda: [
            IndexModel([
                ("owner_id", ASCENDING),
                ("type", ASCENDING),
                ("status", ASCENDING),
                ("last_modified", DESCENDING)
            ], name="owner_type_status_modified"),

            IndexModel([
                ("visibility", ASCENDING),
                ("status", ASCENDING),
                ("last_modified", DESCENDING)
            ], name="visibility_status_modified"),

            IndexModel([
                ("owner_id", ASCENDING),
                ("visibility", ASCENDING),
                ("status", ASCENDING)
            ], name="owner_visibility_status"),

            IndexModel([("created_at", DESCENDING)]),
            IndexModel([("status", ASCENDING)]),
        ]
    }
