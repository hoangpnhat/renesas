#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

import traceback
from typing import Any, Dict, List, Literal

import httpx
from fastapi import status, HTTPException
from fastapi.encoders import jsonable_encoder
from odmantic import ObjectId
from socketio import AsyncServer
from starlette.responses import StreamingResponse

from core.loguru import logger
from caching.redis import RedisClient
from const.utils import PROMPT_LANGUAGE
from controllers.chat import (
    create_dialog_and_content,
    get_context,
)
from core.config import settings
from crud import crud_chat
from databricks import DataBricksService
from exceptions.database_exception import RedisException
from exceptions.https_exception import DataBricksException
from exceptions.https_exception import DataBricksTitleErrors
from injection import AppDepends
from models.chat_model import ContentView
from models.chat_model import MessageContent
from models.chat_model import RedisContext
from schemas.chat import (
    MessageContentCreate,
    MessageContentRequestSocketDTO,
    MessageContentSchema, ContextMessageContentSocketChat,
)
from schemas.chat import MessageContentRequestDTO
from schemas.mapper import (
    map_message_content_schema_to_dbx_request,
    map_dbx_response_to_message_content_schema,
)
from utils.helpers import count_words
from utils.helpers import get_item_from_redis
from utils.helpers import get_title_from_llama
from utils.helpers import set_key_redis_name, store_item_in_redis

timeout = httpx.Timeout(50.0)


async def get_response_from_endpoint(
        request_body: List[Dict[str, Any]], endpoint_name: str
):
    try:
        header = {
            "Authorization": f"Bearer {settings.databricks.DATABRICKS_TOKEN}",
            "Content-type": "application/json",
            "Connection": "keep-alive",
        }
        payload = {
            "messages": request_body,
        }
        async with httpx.AsyncClient() as client:
            url = (settings.databricks.DATABRICKS_ENDPOINT or "").format(endpoint_name=endpoint_name)
            logger.info(f"DBX URL: {url}")
            result: httpx.Response = await client.post(
                url=url,
                json=payload,
                headers=header,
                timeout=timeout,
            )

            return result

    except httpx.TimeoutException as e:
        logger.error(str(e))
        raise DataBricksException(
            error_code=status.HTTP_400_BAD_REQUEST, message="Timeout"
        )
    except Exception as e:
        raise DataBricksException(
            error_code=status.HTTP_400_BAD_REQUEST, message=str(e)
        )


async def get_dbx_prompt_response(
        input_prompt: MessageContentSchema,
        dbx: DataBricksService = AppDepends(DataBricksService),
) -> MessageContentSchema:
    try:
        databricks_prompt = map_message_content_schema_to_dbx_request(input_prompt)
        message_content = await dbx.prompt_message(databricks_prompt)
        result = map_dbx_response_to_message_content_schema(message_content)

        return result
    except httpx.TimeoutException as e:
        logger.error(str(e))
        raise DataBricksException(
            error_code=status.HTTP_400_BAD_REQUEST, message="Timeout"
        )
    except Exception as e:
        logger.error(str(e))
        logger.debug(traceback.format_exc())
        raise DataBricksException(
            error_code=status.HTTP_400_BAD_REQUEST, message=str(e)
        )


async def create_context_and_prompt(
        *,
        sio: AsyncServer,
        dialog_id: str | ObjectId,
        db_in: MessageContent | MessageContentCreate,
        ref_id: ObjectId | None = None,
        sid: str | None = None,
) -> MessageContent:
    _, message_content = await create_dialog_and_content(
        db_in=db_in,
        dialog_id=dialog_id,
        ref_id=ref_id,
    )
    message_content_to_save = jsonable_encoder(message_content)
    logger.info(f"Message Content: {message_content_to_save}")
    await sio.emit("message", {"data": message_content_to_save}, to=sid)

    return message_content


async def get_context_from_redis(
        *,
        redis: RedisClient,
        dialog_id: str,
        user_prompt: MessageContentRequestDTO,
) -> tuple[List[RedisContext], str]:
    try:
        key_dialog = set_key_redis_name(prefix="dialog", _id=dialog_id)
        context = await get_item_from_redis(item_id=key_dialog, redis=redis)
        if not context:
            context_views: List[ContentView] = await get_context(dialog_id=dialog_id)
            context = jsonable_encoder(context_views, exclude={"id"})

        context.append(RedisContext(**user_prompt.model_dump()))
        return context, key_dialog
    except Exception as e:
        raise RedisException(message=str(e), error_code=status.HTTP_400_BAD_REQUEST)


async def prompt_socket(
        *,
        sio: AsyncServer,
        dialog_id: str,
        user_prompt: MessageContentRequestSocketDTO,
        redis: RedisClient,
        sid: str | None = None,
        dbx: DataBricksService = AppDepends(DataBricksService),
) -> MessageContentSchema | None:
    result_ = None
    try:
        # TODO: Refactor Redis
        # TODO: Redis not await
        # TODO: Redis read, write throughput
        user_prompt_obj = jsonable_encoder(user_prompt)
        context, key_dialog = await get_context_from_redis(
            redis=redis, dialog_id=dialog_id, user_prompt=user_prompt_obj
        )

        dbx_result = await dbx.prompt_message(
            map_message_content_schema_to_dbx_request(
                ContextMessageContentSocketChat(context=context, retrieval_place=user_prompt_obj["retrieval_place"]))
        )
        result = map_dbx_response_to_message_content_schema(dbx_result)
        result_ = result
        if result and result.content:
            context.append(RedisContext(content=result.content, role=result.role))
            await store_item_in_redis(
                redis=redis, item=context, key_name=key_dialog
            )

        return result
    except Exception as e:
        logger.error(str(e))
        await sio.emit(
            "message_error",
            {
                "response": result_,
                "dbx_endpoint": f"https://{settings.databricks.DATABRICKS_SERVER_HOSTNAME}/serving-endpoints/{settings.databricks.DATABRICKS_ENDPOINT_NAME}/invocations",
                "data": jsonable_encoder(user_prompt),
                "message": str(e),
            },
            to=sid,
        )
    return None


async def update_title(
        content: str,
        dialog_id: str,
        language: Literal["en", "ja"]
):
    try:
        to_update_dialog = await crud_chat.chat.get(id=ObjectId(dialog_id))
        if to_update_dialog is None:
            return None
        if to_update_dialog.title == "" or to_update_dialog.title == "New chat":
            if count_words(content) <= settings.MAXIMUM_TITLE_COUNT:
                new_title = content
            # new_title = "Test title"
            else:
                sample_prompt = PROMPT_LANGUAGE[language].format(PROMPT=content)
                new_title_ = await get_response_from_endpoint(
                    request_body=[{"content": sample_prompt, "role": "user"}],
                    endpoint_name=settings.LLAMA_ENDPOINT_NAME,
                )
                new_title = get_title_from_llama(new_title_)
                if not new_title:
                    new_title = new_title_.json()["response"]
                # Remove quotation marks from the new title, if any
                new_title = new_title.replace('"', "")
            if new_title != "":
                updated_dialog = await crud_chat.chat.update(
                    db_obj=to_update_dialog,
                    obj_in={"title": new_title},
                )
                return updated_dialog
            return to_update_dialog
        return None
    except Exception as e:
        raise DataBricksTitleErrors(
            message=f"Cannot update the title. Please follow: {e}", error_code=500
        )


async def download_file(
        file_path: str, dbx: DataBricksService = AppDepends(DataBricksService)
):
    try:
        # Create streaming response directly from Databricks generator
        return StreamingResponse(
            dbx.download_file(file_path),
            media_type="application/octet-stream",
            headers={
                "Content-Disposition": f"attachment; filename={file_path.split('/')[-1]}"
            },
        )
    except Exception as e:
        # Proper error handling
        raise HTTPException(status_code=500, detail=f"Error streaming file: {str(e)}")
