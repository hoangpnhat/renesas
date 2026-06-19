from pydantic import BaseModel, HttpUrl
from typing import Optional, Dict, Any, List,Literal, Tuple
from datetime import datetime
from llama_index.core.bridge.pydantic import BaseModel as llamaIndexBaseModel
from llama_index.core.bridge.pydantic import Field as llamaIndexField
from abc import abstractmethod
class Thread(BaseModel):
    id: str
    thread_id: str
    from_user_id: str
    to_bot_id: str


class UserSchema(BaseModel):
    name: str
    id: str
    avatar: HttpUrl | str
    status_verify: int
    type: str


class Mention(BaseModel):
    target: str
    length: int
    offset: int

class MessageMetadata(BaseModel):
    preview_link: Optional[HttpUrl] = None
    mentions: List[Mention]
    payload: str
    is_markdown_text: bool
    reply_to_message: Dict[str, Any]

class Message(BaseModel):
    id: str
    text: str
    type: str | None
    metadata: MessageMetadata | None
    payload: str | None

class SubThread(BaseModel):
    id: str
    event: str
    thread_id: str
    parent_message_id: str
    message_id: str
    from_id: str
    to_id: str
    message: Message

class LastMessage(BaseModel):
    thread_id: str
    message_id: str
    sender_id: str
    bot_id: str
    message_sent_at: datetime
    survey_sent: bool
    message_type: str
    survey_sent_at: datetime | None
    survey_id: str | None
    reminder_sent: bool | None
    reminder_sent_at: datetime | None

class SurveySchema(BaseModel):
    thread_id: str
    message_id: str
    send_at: datetime | None
    is_completed: bool | None
    completed_at: datetime | None
    question: str
    feedback: str | None
    feedback_id: str | None

# Schema LLAMA INDEX 
class LabelledNode(llamaIndexBaseModel):
    """An entity in a graph."""

    label: str = llamaIndexField(default="node", description="The label of the node.")
    embedding: Optional[List[float]] = llamaIndexField(
        default=None, description="The embeddings of the node."
    )
    properties: Dict[str, Any] = llamaIndexField(default_factory=dict)

    @abstractmethod
    def __str__(self) -> str:
        """Return the string representation of the node."""
        ...

    @property
    @abstractmethod
    def id(self) -> str:
        """Get the node id."""
        ...

class EntityNode(LabelledNode):
    """An entity in a graph."""

    name: str = llamaIndexField(description="The name of the entity.")
    label: List[str] = llamaIndexField(default="entity", description="The labels of the node.")
    properties: Dict[str, Any] = llamaIndexField(default_factory=dict)

    def __str__(self) -> str:
        """Return the string representation of the node."""
        return self.name

    @property
    def id(self) -> str:
        """Get the node id."""
        return self.name.replace('"', " ")
    


from langchain_core.pydantic_v1 import BaseModel as langchainBaseModel 
from langchain_core.pydantic_v1 import Field as langchainField

class SubQuery(langchainBaseModel):
    """Search over a graph database NEO4J of Promotion Program."""

    sub_query: str = langchainField(
        ...,
        description="A very specific query against the database.",
    )