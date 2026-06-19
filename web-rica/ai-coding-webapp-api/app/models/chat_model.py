#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

from datetime import datetime
from typing import List, Literal, Optional

from odmantic import Field, Index, ObjectId
from odmantic.query import desc
from pydantic import BaseModel

from db.base_class import Base
from utils.helpers import datetime_now_sec


class Dialog(Base, BaseModel):
    user_id: ObjectId = Field(default=None)
    last_modified: datetime = Field(default_factory=datetime_now_sec)
    start_time: datetime = Field(default_factory=datetime_now_sec)
    content_ids: List[ObjectId] = Field(default=[])
    title: str = Field(default="")
    model_config = {"indexes": lambda: [Index(Dialog.user_id, desc(Dialog.last_modified))]}  # type: ignore[typeddict-unknown-key]


class RetrievedDoc(Base, BaseModel):
    content: str
    doc_uri: str
    file_name: Optional[str] = Field(default="")


class RedisContext(BaseModel):
    content: str = Field(default="")
    role: Literal["user", "assistant", "system"] = Field(default="user")


class ContentView(Base, BaseModel):
    content: str = Field(default="")
    role: Literal["user", "assistant", "system"] = Field(default="user")
    model_config = {"collection": "message_content"}  # type: ignore[typeddict-unknown-key]


class MessageContent(Base, BaseModel):
    content: Optional[str] = Field(default="")
    role: Literal["user", "assistant", "system"]
    relate_to: Optional[ObjectId] = None
    dialog_id: ObjectId = Field(..., examples=None)
    timestamp: datetime = Field(default_factory=datetime_now_sec)
    has_error: bool = Field(default=False)
    retrieved_docs: Optional[List[RetrievedDoc]] = Field(default=None)
    # categories: Optional[List[str]] = []
    # vector_search: Optional[str] = ""
    # is_favorite: bool = Field(default=False)
    model_config = {  # type: ignore[typeddict-unknown-key]
        "indexes": lambda: [
            Index(MessageContent.dialog_id, desc(MessageContent.timestamp))
        ]
    }
    is_good_response: Optional[int] = Field(default=-1)
    user_id: Optional[ObjectId] = Field(default=None, examples=None)

class SavedChatContent(Base, BaseModel):
    user_prompt: str
    bot_response: str
    # categories: List[str]
    # vector_search: str
    references: List[RetrievedDoc] = []
    content_id: ObjectId
    dialog_id: ObjectId
    dialog_title: str = ""
    is_disabled: bool = False
    model_config = {  # type: ignore[typeddict-unknown-key]
        "indexes": lambda: [
            Index(SavedChatContent.dialog_id, SavedChatContent.content_id)
        ]
    }
