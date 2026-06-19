from odmantic import ObjectId, query
from typing import Any

from models.knowledge import Knowledge as MongoKnowledge
from schemas.knowledge import KnowledgeCreate, KnowledgeUpdateGeneral
from .refactor_base.crud_operations import MongoCRUD
from filters.implementations import KnowledgeDynamicQueryBuilder

class CRUDKnowledge(MongoCRUD[MongoKnowledge, KnowledgeCreate, KnowledgeUpdateGeneral]):

    async def get_by_id(self, id: str | ObjectId) -> MongoKnowledge | None:
        to_query = query.and_(MongoKnowledge.id == ObjectId(id),
                            MongoKnowledge.is_deleted != True)
        return await self.engine.find_one(MongoKnowledge, to_query)
    
    async def soft_delete_or_archive(self, knowledge: MongoKnowledge, is_archived: bool, session: Any) -> MongoKnowledge:
        obj_in = KnowledgeUpdateGeneral(is_archived=True) if is_archived else KnowledgeUpdateGeneral(is_deleted=True)
        return await self.update(db_obj=knowledge, obj_in=obj_in, session=session)
    
knowledge_crud = CRUDKnowledge(model=MongoKnowledge,
                               query_builder=KnowledgeDynamicQueryBuilder)