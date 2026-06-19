from typing import Any, cast

from odmantic import ObjectId, query

from const.common import CommonStatus
from crud.refactor_base.crud_operations import MongoCRUD
from filters.implementations import PromptDynamicQueryBuilder
from models.prompt import Prompt as MongoPrompt
from schemas.prompt import PromptCreate, PromptUpdate


class CRUDPrompt(MongoCRUD[MongoPrompt, PromptCreate, PromptUpdate]):

    async def get_by_id(
            self,
            id: ObjectId | str
    ) -> MongoPrompt | None:
        try:
            oid = ObjectId(id)
        except Exception:
            return None
        conditions = [
            MongoPrompt.id == oid,
            MongoPrompt.status != CommonStatus.DELETED
        ]
        return await self.engine.find_one(MongoPrompt, query.and_(*conditions))

    async def soft_delete(self, mongo_prompt: MongoPrompt, session: Any) -> MongoPrompt:
        mongo_prompt.status = CommonStatus.DELETED
        return cast(MongoPrompt, await self.transaction_wrapper(self.update,
                                              db_obj=mongo_prompt,
                                              obj_in=PromptUpdate(status=CommonStatus.DELETED),
                                              session=session))


crud_prompt = CRUDPrompt(model=MongoPrompt,
                         query_builder=PromptDynamicQueryBuilder)
