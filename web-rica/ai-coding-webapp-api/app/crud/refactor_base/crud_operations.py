from typing import Any, Generic, Type, Union,  Optional
from fastapi.encoders import jsonable_encoder
from odmantic import query

from db.base_class import ModelType, CreateSchemaType, UpdateSchemaType
from odmantic import AIOEngine
from db.session import get_engine
from filters import BaseQueryBuilder
from utils.helpers import datetime_now_sec

from .mixins import PaginationHandler, DuplicateHandler, BulkOperation


class CRUDBase(Generic[ModelType, CreateSchemaType, UpdateSchemaType]):
    def __init__(self, model: Type[ModelType]) -> None:
        self.model: type[ModelType] = model
        self.engine: AIOEngine = get_engine()

    async def transaction_wrapper(self, func, *args, **kwargs) -> Any:
        async with await self.engine.client.start_session() as session:
            async with session.start_transaction():
                kwargs['session'] = session
                return await func(*args, **kwargs)
            
    async def single_plain_get(self, id: Any) -> ModelType | None:
        return await self.engine.find_one(self.model, self.model.id == id)

    async def single_complex_get(
        self,
        to_query: query.QueryExpression | dict) -> ModelType | None:

        if isinstance(to_query, dict):
            to_query = query.QueryExpression(to_query)

        return await self.engine.find_one(self.model, to_query)
 
    async def create(
        self,
        obj_in: CreateSchemaType | ModelType,
        session: Optional[Any] = None) -> ModelType:

        if isinstance(obj_in, self.model):
            db_obj = obj_in
        else:
            obj_in_data = jsonable_encoder(obj_in)
            db_obj = self.model(**obj_in_data)
        return await self.engine.save(db_obj, session=session)

    async def update(
        self,
        db_obj: ModelType,
        obj_in: Union[UpdateSchemaType, dict[str, Any]],
        session: Optional[Any] = None) -> ModelType:

        update_data: UpdateSchemaType | dict[str, Any]
        if isinstance(obj_in, dict):
            update_data = obj_in
        else:
            update_data = obj_in.model_dump(exclude_unset=True)

        # Update only the provided fields
        for field, value in update_data.items():
            setattr(db_obj, field, value)

        # Update last_modified only if it exists in db_obj
        if hasattr(db_obj, "modified"):
            db_obj.modified = datetime_now_sec()
        result = await self.engine.save(db_obj, session=session)
        return result

    async def delete(
        self,
        item: ModelType,
        session: Any) -> ModelType:
        await self.engine.delete(item, session=session)
        return item

class MongoCRUD(  # type: ignore[misc]
    CRUDBase[ModelType, CreateSchemaType, UpdateSchemaType],
    PaginationHandler[ModelType],
    DuplicateHandler[ModelType],
    BulkOperation[ModelType]
):
    def __init__(self, model, query_builder: type[BaseQueryBuilder] | BaseQueryBuilder | None = None):
        super().__init__(model)
        self.query_builder = query_builder
        self._pagination_support = None