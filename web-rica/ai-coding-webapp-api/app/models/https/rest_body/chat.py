from typing import List, Literal
from odmantic import ObjectId
from pydantic import BaseModel, Field
from models.chat_model import MessageContent
from models.https.rest_body.base_rest import (
    T,
    BaseBody,
    GetBaseBody,
    GetBaseBodyResponse,
)
from models.chat_model import Dialog, SavedChatContent


class SavedChatContentResponse(BaseBody):
    results: SavedChatContent


class Message:
    content: str
    role: Literal["user", "assistant", "system"]


class MessageRequest(Message):
    categories: List[str]


class RequestBody:
    messages: List[MessageRequest]


class References:
    title: str
    url: str


class MessageResponse(Message):
    refs: List[References]


class ResponseBody:
    results: List[MessageResponse]


class GetMessageContentResponse(GetBaseBody):
    results: List[MessageContent]


class UpdatedDialogResponse(GetBaseBodyResponse[T]):
    pass


class GetSaveContentChatResponse(GetBaseBodyResponse[T]):
    pass


class GetDialogResponse(GetBaseBody):
    results: List[Dialog]

class DeleteDialogResponseBody(BaseBody):
    id: ObjectId | str = Field("", description="Id of delete Item")


class UpdatedMessageContentResponse(BaseModel):
    dialog: Dialog
    message_content: MessageContent


class RetrievedDoc:
    content: str
    doc_uri: str
    name: str

    def __init__(self, content: str, doc_uri: str, name: str) -> None:
        self.content = content
        self.doc_uri = doc_uri
        self.name = name


class DatabricksEndpointResponseElement:
    response: str
    retrieved_docs: List[RetrievedDoc]

    def __init__(self, response: str, retrieved_docs: List[RetrievedDoc]) -> None:
        self.response = response
        self.retrieved_docs = retrieved_docs

class VscodePromptRequest(BaseModel):
    role: Literal["user", "assistant", "system"] = "user" 
    content: str

class VscodeRequestDTO(BaseModel):
    messages: List[VscodePromptRequest] = Field(
        default_factory=list, description="List of messages for the chat"
    )
    max_tokens: int = Field(
        default=128, description="Maximum number of tokens for the chat"
    )
    temperature: float = Field(
        default=0.7, description="Temperature for the chat response"
    )