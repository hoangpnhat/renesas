#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

import math
from typing import Annotated, List, cast

import httpx
from fastapi import APIRouter, Depends, HTTPException, Query, Path, status, Header
from odmantic import ObjectId

import controllers
from const.common import AppAccessRoles
from const.http_messages import MessageEnum
from core.auth.unified import unified_validator
from core.config import settings
from crud import crud_chat
from exceptions.database_exception import NotExistedRecord
from models.chat_model import SavedChatContent
from models.https.rest_body.base_rest import MetaData, StatusResponse
from models.https.rest_body.chat import (
    DeleteDialogResponseBody,
    GetDialogResponse,
    GetMessageContentResponse,
    GetSaveContentChatResponse,
    SavedChatContentResponse,
    UpdatedDialogResponse,
    UpdatedMessageContentResponse,
    VscodeRequestDTO,
)
from schemas.base_schema import BatchIdsDTO, DeletedMultipleResponseDTO
from schemas.chat import DialogUpdate
from schemas.chat import (
    MessageContentDTO,
    DialogCreate,
)
from schemas.user import VerifiedUser
from utils.helpers import normalize_id

router = APIRouter()


# TODO: move to controller
@router.post("/dialog", status_code=status.HTTP_201_CREATED)
async def create_dialog(
        *,
        request_body: DialogCreate,
        current_user: VerifiedUser = Depends(unified_validator([AppAccessRoles.USER])),
) -> crud_chat.Dialog:
    try:
        if current_user:
            request_body.user_id = current_user.user_id
            dialog: crud_chat.Dialog = await crud_chat.chat.create(obj_in=request_body)

            return dialog
        else:
            raise HTTPException(
                status_code=status.HTTP_401_UNAUTHORIZED, detail="Invalid credential"
            )
    except:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST, detail="Invalid request body"
        )


# Get APIs
@router.get("/dialogs", response_model=GetDialogResponse)
async def get_all_dialogs(
        *,
        current_user: VerifiedUser = Depends(unified_validator([AppAccessRoles.USER])),
) -> List[crud_chat.Dialog]:
    return cast(List[crud_chat.Dialog], await controllers.chat.get_all_dialog(current_user=current_user))


@router.get(
    "/dialog/{dialog_id}/contents",
    response_model=GetMessageContentResponse,
    status_code=status.HTTP_200_OK,
)
async def get_dialog_contents(
        *,
        dialog_id: ObjectId = Path(title="Dialog id"),
        page: int = Query(title="Page query for page break loading", default=1),
        current_user: VerifiedUser = Depends(unified_validator([AppAccessRoles.USER])),
) -> GetMessageContentResponse:
    message_contents, total_num = await controllers.chat.get_content_by_dialog_id(
        dialog_id=dialog_id, page=page, page_break=True
    )
    return GetMessageContentResponse(
        status=StatusResponse(
            state=MessageEnum.COMPLETED.value,
            message=MessageEnum.GET_CONTENT_DIALOG_SUCCESS.value,
        ),
        results=message_contents,
        metadata=MetaData(
            all_records=total_num,
            page=page,
            total_page=math.ceil(total_num / settings.MULTI_MAX),
        ),
    )


@router.get("/saved_contents", response_model=GetSaveContentChatResponse)
async def get_all_save_content(
        *,
        page: Annotated[int, Query(title="Page query for page break loading")],
) -> GetSaveContentChatResponse[List[SavedChatContent]]:
    saved_content, count_ = await controllers.chat.get_all_saved_content(page=page)
    return GetSaveContentChatResponse[List[SavedChatContent]](
        metadata=MetaData(
            all_records=count_,
            page=page,
            total_page=math.ceil(count_ / settings.MULTI_MAX),
        ),
        results=saved_content,
        status=StatusResponse(message="Get saved content successfully"),
    )


# Put Methods
@router.put(
    "/dialog/{dialog_id}/content/{content_id}", response_model=SavedChatContentResponse
)
async def save_chat_content(
        *,
        dialog_id: Annotated[str, Path(title="Dialog Id")],
        content_id: Annotated[str, Path(title="Content Id")],
        current_user: VerifiedUser = Depends(unified_validator([AppAccessRoles.USER])),
) -> SavedChatContentResponse:
    if current_user.user_id is None:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="User identity not resolved",
            headers={"WWW-Authenticate": "Bearer"},
        )
    saved_chat_content = await controllers.save_chat_content(
        dialog_id=ObjectId(dialog_id),
        content_id=ObjectId(content_id),
        current_user_id=current_user.user_id,
    )
    return SavedChatContentResponse(
        status=StatusResponse(message="Saved content successfully"),
        results=saved_chat_content
    )


# Patch Methods
@router.patch(
    "/dialog/{dialog_id}",
    status_code=status.HTTP_200_OK,
    response_model=UpdatedDialogResponse[crud_chat.Dialog],
)
async def update_dialog(
        *,
        dialog_id: Annotated[str, Path(title="Dialog id")],
        request_body: DialogUpdate,
        current_user: VerifiedUser = Depends(unified_validator([AppAccessRoles.USER])),
) -> UpdatedDialogResponse[crud_chat.Dialog]:
    new_dialog: crud_chat.Dialog = await controllers.update_dialog(
        request_body=request_body, dialog_id=dialog_id
    )
    return UpdatedDialogResponse[crud_chat.Dialog](
        status=StatusResponse(message="Saved content successfully"),
        results=new_dialog
    )


@router.patch(
    "/dialog/{dialog_id}/content/{content_id}",
    response_model=UpdatedMessageContentResponse,
)
async def update_message_content(
        *,
        dialog_id: Annotated[str, Path(title="Dialog Id")],
        content_id: Annotated[str, Path(title="Content Id")],
        request_body: MessageContentDTO,
        current_user: VerifiedUser = Depends(unified_validator([AppAccessRoles.USER])),
) -> UpdatedMessageContentResponse:
    return cast(UpdatedMessageContentResponse, await controllers.chat.update_message_content(
        dialog_id=normalize_id(dialog_id),
        request_body=request_body,
        content_id=normalize_id(content_id),
    ))


@router.delete("/dialog/{dialog_id}", response_model=DeleteDialogResponseBody)
async def delete_dialog_with_id(
        *,
        dialog_id: Annotated[str, Path(title="Dialog id")],
        current_user: VerifiedUser = Depends(unified_validator([AppAccessRoles.USER])),
) -> DeleteDialogResponseBody:
    try:
        await crud_chat.message_content.remove_content_with_dialog(dialog_id=dialog_id)
        dialog_to_del = await crud_chat.chat.delete(item_id=dialog_id)
        return DeleteDialogResponseBody(
            status=StatusResponse(message=MessageEnum.DELETED_DIALOG_SUCCESS.value),
            id=str(dialog_to_del.id),
        )
    except NotExistedRecord as ex:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail=str(ex))


# create delete route, delete multiple dialogs
@router.delete("/dialogs", response_model=DeletedMultipleResponseDTO)
async def delete_dialogs(
        *,
        request_body: BatchIdsDTO,
        current_user: VerifiedUser = Depends(unified_validator([AppAccessRoles.USER])),
) -> DeletedMultipleResponseDTO:
    if current_user.user_id is None:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="User identity not resolved",
            headers={"WWW-Authenticate": "Bearer"},
        )
    deleted_count, deleted_ids = await controllers.chat.delete_dialogs(
        dialog_ids=request_body.ids,
        user_id=current_user.user_id,
    )
    return DeletedMultipleResponseDTO(
        deleted_count=deleted_count,
        deleted_ids=deleted_ids,
    )


@router.get("/download-file")
async def download_file(
        *, path_file: Annotated[str, Query(title="Path to be downloaded")]
):
    return await controllers.databricks.download_file(file_path=path_file)


@router.post("/dbx-chat")
async def dbx_chat(
        *,
        authorization: str = Header(None),
        request_body: VscodeRequestDTO,
        current_user: VerifiedUser = Depends(unified_validator([AppAccessRoles.USER])),
):
    if not authorization:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Authorization header is required",
        )

    encode_body = request_body.model_dump_json()
    async with httpx.AsyncClient() as client:
        response = await client.post(
            url="https://adb-379144824042062.2.azuredatabricks.net/serving-endpoints/DeepSeek-Coder-67b-awq-vllm/invocations",
            content=encode_body,
            headers={
                "Authorization": f"Bearer {os.environ.get('DATABRICKS_TOKEN', '')}",
                "Content-type": "application/json",
                "Connection": "keep-alive",
            },
        )
        if response.status_code != 200:
            raise HTTPException(
                status_code=response.status_code,
                detail=f"Error from Databricks: {response.text}",
            )
        return response.json()
