#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

from datetime import datetime
from typing import Any, Literal, Optional, List, TypedDict

from odmantic import ObjectId
from pydantic import BaseModel, Field

from models import RedisContext
from models.chat_model import RetrievedDoc
from utils.helpers import datetime_now_sec

# TODO: break down the schema into smaller pieces

class MessageContentSchema(BaseModel):
    content: Optional[str]=Field(default=None)
    role: Literal["user", "assistant", "system"] = Field(default="user")
    retrieved_docs: List[RetrievedDoc] = Field(default=[])
    language: Literal["en", "ja"] = Field(default="en")
    relate_to: Optional[ObjectId] = Field(default=None)
    is_good_response: Optional[Literal[1, -1, 0]] = Field(default=-1)
class MessageContentSocketChat:
    content: str
    role: Literal["user", "assistant", "system"] = Field(default="user")

class ContextMessageContentSocketChat(BaseModel):
    context: List[RedisContext]
    retrieval_place: List[Literal["code_base", "document"]]
class DialogBaseSchema(BaseModel):
    title: str = Field(default="")
    contents: List[MessageContentSchema] = Field(default=[])


class DialogCreate(BaseModel):
    title: str = Field(default="")
    user_id: ObjectId | None = Field(default=None)


class DialogUpdate(DialogBaseSchema):
    user_id: ObjectId | None = Field(default=None)


class MessageContentDTO(MessageContentSchema):
    # TODO: change content to new format
    user_id: Optional[ObjectId] = Field(default=None)

class MessageContentRequestDTO(MessageContentSchema):
    dialog_id: str
    # vector_search: str
    # categories: List[str]
    # language: Literal["en", "ja"] = Field(default="en")

class MessageContentRequestSocketDTO(MessageContentRequestDTO):
    # token: str
    retrieval_place:List[Literal["code_base", "document"]] = Field(default=[])

class MessageContentCreate(MessageContentDTO):
    dialog_id: ObjectId
    # vector_search: str = Optional
    timestamp: datetime = Field(default_factory=datetime_now_sec)


class MessageContentUpdate(MessageContentDTO):
    dialog_id: str
    timestamp: datetime = Field(default_factory=datetime_now_sec)


class ContentMessageRequestBody(BaseModel):
    messages: List[MessageContentCreate] = Field(default=[])


class SavedChatContentSchema(BaseModel):
    user_prompt: str = Field(default="")
    bot_response: str = Field(default="")
    categories: List[str] = Field(default=[])
    vector_search: str = Field(default="")
    references: List[RetrievedDoc] = Field(default=[])
    content_id: ObjectId | None = Field(default=None)
    dialog_id: ObjectId | None = Field(default=None)
    dialog_title: str = Field(default="")
    user_id: ObjectId | None = Field(default=None)


class SavedChatContentCreate(SavedChatContentSchema):
    pass


class SavedChatContentUpdate(SavedChatContentSchema):
    pass


class DialogRequestBody(BaseModel):
    title: str = Field(default="")

    class Config:
        extra = "forbid"

class ContextPromptDTO(BaseModel):
    content: str
    role: Literal["user", "assistant", "system"]
    retrieval_place: List[Literal["code_base", "document"]] = Field(default=[])

class MessageContentDataBricks(MessageContentSchema):
    role: Literal["user", "assistant", "system"] = Field(default="user")
    content: str = Field(default="")


class PromptRequestDataBricks(TypedDict):
    messages: List[Any]
    retrieval_place: List[Literal["code_base", "document"]]


class PromptResponseDataBricks(BaseModel):
    response: Optional[str] = Field(default="")
    retrieved_docs: List[RetrievedDoc]
