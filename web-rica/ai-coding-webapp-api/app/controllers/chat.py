import math
from typing import List, Optional, cast

import bson
import bson.errors
import httpx
from fastapi import HTTPException, status
from odmantic import ObjectId, query

from const.http_messages import MessageEnum
from core.config import settings
from core.loguru import logger
from crud import crud_chat
from exceptions.database_exception import CanNotFindRecord
from exceptions.database_exception import NotExistedRecord
from models.chat_model import Dialog, MessageContent, SavedChatContent
from models.https.rest_body.base_rest import MetaData, StatusResponse
from models.https.rest_body.chat import GetDialogResponse
from schemas import VerifiedUser
from schemas.chat import DialogUpdate, MessageContentDTO
from schemas.chat import SavedChatContentCreate, MessageContentCreate
from utils.helpers import datetime_now_sec
from utils.helpers import normalize_id

# TODO: Separate the controller into Dialog and Message


timeout = httpx.Timeout(50.0)


async def get_context(dialog_id: ObjectId | str) -> List[crud_chat.ContentView]:
    dialog_id = normalize_id(dialog_id)
    return await crud_chat.message_content.get_content_by_dialog_id_with_custom(
        dialog_id=str(dialog_id)
    )


async def delete_chat_content(message_content: MessageContent):
    return await crud_chat.message_content.delete(item=message_content)


async def create_dialog_and_content(
        db_in: MessageContent | MessageContentCreate,
        dialog_id: str | ObjectId,
        ref_id: Optional[ObjectId],
) -> tuple[Dialog, MessageContent]:
    try:
        content: MessageContent = (
            await crud_chat.message_content.create_message_content(
                obj_in=db_in, dialog_id=str(dialog_id), ref_id=ref_id
            )
        )

        dialog = await crud_chat.chat.add_content_chat(
            dialog_id=dialog_id,
            message_content_id=content.model_dump()["id"],
        )
    except NotExistedRecord as not_ex:
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail=str(not_ex))

    return (dialog, content)


async def get_all_saved_content(page: int) -> tuple[List[SavedChatContent], int]:
    try:
        saved_chat_content, count_ = await crud_chat.message_content.get_saved_content(
            page=page
        )
        return saved_chat_content, count_
    except CanNotFindRecord as e:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail=str(e))


async def save_chat_content(
        dialog_id: ObjectId, content_id: ObjectId, current_user_id: ObjectId
):
    try:
        saved_chat_content: SavedChatContent | None = (
            await crud_chat.saved_chat_content.find_with_query(
                to_query=query.eq(SavedChatContent.content_id, content_id),
                id=content_id,
            )
        )
        bot_content: MessageContent = cast(MessageContent, await crud_chat.message_content.find_with_query(
            to_query=query.and_(
                query.eq(MessageContent.id, content_id),
                query.eq(MessageContent.dialog_id, dialog_id),
            ),
            raise_error=True,
            content_id=str(content_id),
        ))
        if saved_chat_content and bot_content:
            saved_chat_content.is_disabled = not saved_chat_content.is_disabled
            bot_content = await crud_chat.message_content.update(
                db_obj=bot_content,
                obj_in={"is_favorite": not saved_chat_content.is_disabled},
            )
            return await crud_chat.saved_chat_content.update(
                obj_in={"is_disabled": saved_chat_content.is_disabled},
                db_obj=saved_chat_content,
            )

        bot_content = await crud_chat.message_content.update(
            db_obj=bot_content, obj_in={"is_favorite": True}
        )
        dialog: Dialog = cast(Dialog, await crud_chat.chat.find_with_query(
            to_query=query.eq(Dialog.id, dialog_id),
            raise_error=True,
            id=dialog_id,
        ))
        user_content: MessageContent | None = (
            await crud_chat.message_content.find_with_query(
                to_query=query.and_(
                    MessageContent.id == bot_content.relate_to,
                    MessageContent.dialog_id == dialog_id,
                ),
                content_id=str(bot_content.relate_to),
            )
        )
        if not user_content or not user_content.content or not bot_content.content:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Cannot save chat content: message content is empty",
            )
        return await crud_chat.saved_chat_content.create(
            obj_in=SavedChatContentCreate(
                user_prompt=user_content.content,
                bot_response=bot_content.content,
                # categories=bot_content.categories,
                # vector_search=bot_content.vector_search,
                references=bot_content.retrieved_docs or [],
                content_id=content_id,
                dialog_id=dialog_id,
                dialog_title=dialog.title,
                user_id=current_user_id,
            ),
        )
    except Exception as e:
        logger.error(str(e))
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail=str(e))


async def update_dialog(request_body: DialogUpdate, dialog_id: str):
    curr_dialog: crud_chat.Dialog = cast(crud_chat.Dialog, await crud_chat.chat.get(
        id=ObjectId(dialog_id)
    ))
    return await crud_chat.chat.update(
        db_obj=curr_dialog,
        obj_in={"title": request_body.title, "last_modified": datetime_now_sec()},
    )


async def update_message_content(
        *,
        dialog_id: ObjectId,
        content_id: ObjectId,
        request_body: MessageContentDTO
):
    try:
        message_content: MessageContent = cast(MessageContent, await crud_chat.message_content.find_with_query(
            to_query=query.and_(
                query.eq(MessageContent.id, content_id),
                query.eq(MessageContent.dialog_id, dialog_id),
            ),
            content_id=str(content_id),
            raise_error=True,
        ))
        dialog: Dialog = cast(Dialog, await crud_chat.chat.get(dialog_id))
        dialog = await crud_chat.chat.update(
            db_obj=dialog,
            obj_in={"last_modified": datetime_now_sec()}
        )
        message_content = await crud_chat.message_content.update(
            db_obj=message_content, obj_in=request_body.model_dump(exclude_unset=True, exclude_none=True)
        )
        return {"dialog": dialog, "message_content": message_content}
    except NotExistedRecord as e:
        logger.error(str(e))
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail=str(e))


async def get_all_dialog(current_user: VerifiedUser):
    try:
        response, count = await crud_chat.chat.get_multi_with_user_id(
            page_break=False, user_id=str(current_user.user_id)
        )
        return GetDialogResponse(
            status=StatusResponse(
                state=MessageEnum.COMPLETED.value,
                message=MessageEnum.GET_DIALOG_SUCCESS.value,
            ),
            results=response,
            metadata=MetaData(
                all_records=count,
                page=0,
                total_page=math.ceil(count / settings.MULTI_MAX),
            ),
        )
    except Exception as e:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail=str(e))


async def delete_dialogs(dialog_ids: List[ObjectId], user_id: ObjectId):
    try:
        dialogs = await crud_chat.chat.batch_get_dialog_ids(
            dialog_ids=dialog_ids,
            user_id=str(user_id),
        )
        if len(dialogs) < len(dialog_ids):
            raise CanNotFindRecord(
                message="Some dialog(s) not found or not belong to the user"
            )
        content_ids = [
            content_id for dialog in dialogs for content_id in dialog.content_ids
        ]

        response = await crud_chat.chat.batch_delete_dialogs_with_content_ids(
            dialog_ids=dialog_ids,
            content_ids=content_ids
        )

        return response

    except Exception as e:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail=str(e))


async def get_content_by_dialog_id(
        dialog_id: ObjectId, page: int, page_break: bool
) -> tuple[List[MessageContent], int]:
    try:
        message_contents, total_num = (
            await crud_chat.message_content.get_content_by_dialog_id(
                dialog_id=dialog_id, page=page, page_break=page_break
            )
        )
        return message_contents, total_num
    except bson.errors.InvalidId:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Id you have typed has wrong.",
        )
    except CanNotFindRecord as e:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail=str(e))
    except Exception as e:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail=str(e))


async def batch_create_message_content(
        message_contents: List[MessageContentCreate],
) -> List[MessageContent]:
    return cast(List[MessageContent], await crud_chat.message_content.batch_create_message_content(message_contents=message_contents))
