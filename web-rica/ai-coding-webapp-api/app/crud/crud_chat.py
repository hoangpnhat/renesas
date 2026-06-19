import asyncio
from typing import Any, Dict, List, Optional

from fastapi.encoders import jsonable_encoder
from odmantic import ObjectId, query

from core.config import settings
from crud.base import CRUDBase
from crud.base import ModelType
from exceptions.database_exception import (
    CanNotFindRecord,
    NotExistedRecord,
)
from models.chat_model import ContentView
from models.chat_model import Dialog
from models.chat_model import MessageContent, SavedChatContent
from schemas.chat import (
    DialogCreate,
    DialogUpdate,
    SavedChatContentCreate,
    SavedChatContentUpdate,
)
from schemas.chat import MessageContentCreate, MessageContentUpdate
from utils.helpers import datetime_now_sec
from utils.helpers import normalize_id


class CRUDChat(CRUDBase[Dialog, DialogCreate, DialogUpdate]):
    async def get_chat_by_user_id(
            self, *, user_id: str, chat_id: str
    ) -> List[ModelType]:
        return await self.engine.find(
            self.model,  # type: ignore[arg-type]
            self.model.user_id == ObjectId(user_id)
            and self.model.id == ObjectId(chat_id),
        )

    async def get_multi_with_user_id(
            self,
            *,
            page: int = 0,
            page_break: bool = False,
            filter_count: Any = {},
            user_id: str,
    ):
        user_oid = normalize_id(user_id)
        if page_break:
            return (
                await self.engine.find(
                    self.model,
                    self.model.user_id == user_oid,
                    sort=query.desc(self.model.last_modified),
                    skip=page * settings.MULTI_MAX,
                    limit=settings.MULTI_MAX,
                ),
                await self.engine.count(self.model, filter_count),
            )
        return (
            await self.engine.find(
                self.model,
                self.model.user_id == user_oid,
                sort=query.desc(self.model.last_modified),
            ),
            await self.engine.count(self.model, filter_count),
        )

    async def add_content_chat(
            self,
            *,
            dialog_id: str | ObjectId,
            message_content_id: str | ObjectId,
    ) -> Dialog:
        if isinstance(dialog_id, str):
            dialog_id = ObjectId(dialog_id)
        dialog = await self.get(dialog_id)
        if dialog is None:
            raise NotExistedRecord(str(dialog_id), "Dialog")

        dialog.content_ids.append(ObjectId(message_content_id))
        dialog.modified = datetime_now_sec()
        return await self.create(obj_in=dialog)

    async def batch_get_dialog_ids(
            self,
            *,
            dialog_ids: List[ObjectId],
            user_id: str | ObjectId,
    ) -> List[Dialog]:
        if user_id:
            return await self.engine.find(
                self.model,
                self.model.id.in_(dialog_ids),  # type: ignore[union-attr]
            )

        return await self.engine.find(self.model, self.model.id.in_(dialog_ids))  # type: ignore[union-attr]

    async def batch_delete_dialogs_with_content_ids(
            self,
            *,
            dialog_ids: List[ObjectId],
            content_ids: List[ObjectId],
    ) -> tuple[int, List[ObjectId]]:
        delete_dialogs = self.engine.remove(Dialog, Dialog.id.in_(dialog_ids))  # type: ignore[union-attr]
        delete_contents = self.engine.remove(MessageContent, MessageContent.id.in_(content_ids))  # type: ignore[union-attr]

        await asyncio.gather(delete_dialogs, delete_contents)

        return len(dialog_ids), dialog_ids


class CRUDSavedChatContent(
    CRUDBase[SavedChatContent, SavedChatContentCreate, SavedChatContentUpdate]
):
    async def get_with_content_id(
            self,
            *,
            content_id: ObjectId,
            raise_error=False,
    ):
        content = await self.engine.find_one(
            self.model, self.model.content_id == content_id
        )
        if not content and raise_error:
            raise NotExistedRecord(str(content_id), "Saved Chat Content")
        return content


class CRUDMessageContent(
    CRUDBase[MessageContent, MessageContentCreate, MessageContentUpdate]
):
    async def find_with_query(  # type: ignore[override]
            self,
            *,
            to_query: query.QueryExpression,
            content_id: str,
            raise_error=False,
    ) -> MessageContent | None:
        content = await self.engine.find_one(self.model, to_query)
        if not content and raise_error:
            raise NotExistedRecord(content_id, "Content Chat")
        return content

    async def get_with_content_id_and_dialog_id(
            self,
            *,
            content_id: ObjectId,
            dialog_id: ObjectId,
            raise_error=False,
    ):
        content = await self.engine.find_one(
            self.model,
            query.and_(
                self.model.id == content_id,
                self.model.dialog_id == dialog_id,
            ),
        )
        if not content and raise_error:
            raise NotExistedRecord(str(content_id), "Content Chat")
        return content

    async def get_with_content_id_and_relate_to(
            self,
            *,
            content_id: ObjectId,
            relate_to: ObjectId,
            raise_error=False,
    ):
        content = await self.engine.find_one(
            self.model,
            query.and_(
                self.model.id == content_id,
                self.model.relate_to == relate_to,
            ),
        )
        if not content and raise_error:
            raise NotExistedRecord(str(content_id), "Content Chat")
        return content

    async def create_message_content(
            self,
            *,
            obj_in: MessageContent | MessageContentCreate,
            dialog_id: str,
            ref_id: Optional[ObjectId] = None,
    ) -> MessageContent:
        obj_dict: Dict[str, Any] = obj_in.model_dump()
        obj_dict.update({
            'dialog_id': ObjectId(dialog_id),  # Replace with the actual `dialog_id`
            'relate_to': ref_id  # Replace with the actual `ref_id`
        })
        return await self.create(obj_in=MessageContent(**obj_dict))

    async def get_saved_content(
            self, page: int, page_break: bool = True
    ) -> tuple[List[SavedChatContent], int]:
        if page_break:
            saved_content_chat: List[SavedChatContent] = await self.engine.find(
                SavedChatContent,
                skip=page * settings.MULTI_MAX,
                limit=settings.MULTI_MAX,
            )
        else:
            saved_content_chat = await self.engine.find(SavedChatContent)
        count_num = await self.engine.count(SavedChatContent)
        if len(saved_content_chat) == 0:
            raise CanNotFindRecord()
        return saved_content_chat, count_num

    async def get_content_by_dialog_id(
            self,
            *,
            dialog_id: ObjectId,
            page: int = 0,
            page_break: bool = False,
    ) -> tuple[List[MessageContent], int]:
        offset = (
            {"skip": page * settings.MULTI_MAX, "limit": settings.MULTI_MAX}
            if page_break and page > -1
            else {}
        )
        count = await self.engine.count(self.model, self.model.dialog_id == dialog_id)

        if page_break and page > -1:
            return (
                await self.engine.find(
                    self.model,
                    query.eq(self.model.dialog_id, dialog_id),
                    sort=query.desc(self.model.timestamp),
                    skip=page * settings.MULTI_MAX,
                    limit=settings.MULTI_MAX,
                ),
                count,
            )
        return (
            await self.engine.find(
                self.model,
                query.eq(self.model.dialog_id, dialog_id),
                sort=query.desc(self.model.timestamp),
            ),
            count,
        )

    async def remove_content_with_dialog(self, *, dialog_id: ObjectId | str) -> int:
        dialog_id = normalize_id(dialog_id)
        dialog = await chat.get(id=dialog_id)
        if dialog is not None:
            return await self.engine.remove(
                self.model, self.model.id.in_(dialog.content_ids)  # type: ignore[union-attr]
            )
        else:
            raise NotExistedRecord(id=str(dialog_id), instance_name="Dialog")

    async def get_content_by_dialog_id_with_custom(
            self,
            *,
            dialog_id: str,
    ) -> List[ContentView]:
        dialog_oid = ObjectId(dialog_id)
        dialog: Dialog | None = await chat.get(dialog_oid)
        return await self.engine.find(
            ContentView,
            (self.model.id.in_(dialog.content_ids)) & (self.model.has_error == False),  # type: ignore[union-attr]
        )

    async def batch_create_message_content(
            self,
            message_contents: List[MessageContentCreate],
    ):
        return await self.engine.save_all(
            [MessageContent(**jsonable_encoder(content)) for content in message_contents]
        )


message_content = CRUDMessageContent(MessageContent)
chat = CRUDChat(Dialog)
saved_chat_content = CRUDSavedChatContent(SavedChatContent)
