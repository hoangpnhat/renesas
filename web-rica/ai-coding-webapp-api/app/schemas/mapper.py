from typing import Any, List, cast

from redis import Redis

from models import RedisContext
from schemas.chat import (
    PromptResponseDataBricks,
    MessageContentSchema,
    PromptRequestDataBricks, ContextMessageContentSocketChat,
)
from schemas.chat import RetrievedDoc


def map_dbx_response_to_message_content_schema(
    dbx_prompt: PromptResponseDataBricks,
) -> MessageContentSchema:
    prompt_content = dbx_prompt.response
    retrieved_docs = dbx_prompt.retrieved_docs
    # TODO: Update later base on the AI Team
    result = MessageContentSchema(
        retrieved_docs=retrieved_docs, content=prompt_content, role="assistant"
    )

    return result


def map_message_content_schema_to_dbx_request(
    message_content: MessageContentSchema | ContextMessageContentSocketChat,
) -> PromptRequestDataBricks:
    messages: List[Any] = (
        cast(List[Any], message_content.context)
        if isinstance(message_content, ContextMessageContentSocketChat)
        else [{"content": message_content.content, "role": "user"}]
    )
    return {
        "messages": messages,
        "retrieval_place": getattr(message_content, 'retrieval_place', []),
    }

def map_message_sent_schema_to_title_dbx_request(message_content: MessageContentSchema):
    return