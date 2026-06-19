from odmantic import ObjectId, query
from typing import Any

from models.group import Group as MongoGroup
from schemas.group import GroupCreate, GroupUpdateGeneral
from .refactor_base.crud_operations import MongoCRUD
from filters.implementations import GroupDynamicQueryBuilder

class CRUDGroup(MongoCRUD[MongoGroup, GroupCreate, GroupUpdateGeneral]):

    async def get_by_id(self, id: str | ObjectId) -> MongoGroup | None:
        return await self.single_complex_get(to_query=(MongoGroup.id == ObjectId(id)))  # type: ignore[arg-type]
    
    async def soft_delete_or_archive(self, group: MongoGroup, is_archived: bool, session: Any) -> MongoGroup:
        obj_in: dict[str, Any] = {"is_archived": True} if is_archived else {"is_deleted": True}
        return await self.update(db_obj=group, obj_in=obj_in, session=session)
    
    async def get_user_groups(self, user_id: str | ObjectId) -> list[MongoGroup]:
        return await group_crud.engine.find(
            self.model,
            {
                "$or": [
                    {"owner_id": ObjectId(user_id)},
                    {"members.user_id": ObjectId(user_id), "is_deleted": False}
                ]
            }
        )

group_crud = CRUDGroup(model=MongoGroup,
                       query_builder=GroupDynamicQueryBuilder)