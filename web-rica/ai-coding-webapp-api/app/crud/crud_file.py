from typing import Any
from odmantic import ObjectId, query


from models.file import Files as MongoFile
from schemas.file import FileCreate, FileUpdate
from const.common import CommonStatus
from filters.implementations import FileDynamicQueryBuilder
from .refactor_base.crud_operations import MongoCRUD

class CRUDFile(MongoCRUD[MongoFile, FileCreate, FileUpdate]):

    async def get_by_id(self, id: str | ObjectId) -> MongoFile | None:
        to_query = query.and_(MongoFile.id == ObjectId(id),
                            MongoFile.status != CommonStatus.DELETED)
        return await self.engine.find_one(MongoFile, to_query)

    async def delete_file(self, file: MongoFile, session: Any) -> MongoFile:
        return await self.update(
                            db_obj=file,
                            obj_in={"is_hidden": True},
                            session=session)

file_crud = CRUDFile(model=MongoFile,
                    query_builder=FileDynamicQueryBuilder)