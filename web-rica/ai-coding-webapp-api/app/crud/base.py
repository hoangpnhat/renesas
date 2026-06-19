#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
import re
from datetime import datetime
from typing import Any, Dict, Generic, Type, TypeVar, Union, List, Optional, cast

from fastapi.encoders import jsonable_encoder
from odmantic import AIOEngine, ObjectId, query
from pydantic import BaseModel
from odmantic.query import asc, desc

from core.loguru import logger
from const.enums import SortOrder
from db.base_class import Base
from db.session import get_engine
from exceptions.database_exception import NotExistedRecord
from schemas import SortFieldDict
from utils.helpers import datetime_now_sec, normalize_id

ModelType = TypeVar("ModelType", bound=Base)
CreateSchemaType = TypeVar("CreateSchemaType", bound=BaseModel)
UpdateSchemaType = TypeVar("UpdateSchemaType", bound=BaseModel)


def sort_generate(model, sort: SortFieldDict):
    if not sort:
        raise ValueError("Sort dictionary is empty")

    field = getattr(model, sort["field"])
    if sort["direction"] == SortOrder.asc:
        return asc(field)
    return desc(field)


async def soft_delete(model: ModelType, engine: AIOEngine) -> ModelType:
    """
    Soft delete a model instance by setting a 'deleted' flag.
    This function should be implemented in the model class.
    """
    if hasattr(model, "deleted_at"):
        new_model = model.model_copy(
            deep=True, update={"deleted_at": datetime_now_sec()}
        )
        return await engine.save(new_model)
    await engine.delete(model)
    return model


class CRUDBase(Generic[ModelType, CreateSchemaType, UpdateSchemaType]):
    def __init__(self, model: Type[ModelType]) -> None:
        self.model: type[ModelType] = model
        self.engine: AIOEngine = get_engine()

    async def find_with_query(
            self,
            *,
            to_query: query.QueryExpression,
            id: ObjectId,
            raise_error: bool = False,
    ) -> ModelType | None:

        content = await self.engine.find_one(self.model, to_query)
        if not content and raise_error:
            raise NotExistedRecord(str(id), self.model.__name__)
        return content

    async def get(self, id: Any) -> ModelType | None:
        model = await self.engine.find_one(self.model, self.model.id == id)
        if not model:
            raise NotExistedRecord(id, instance_name=model.__class__.__name__)
        return model

    async def get_multi(
            self,
            *,
            page: Optional[int] = None,
            entries: Optional[int] = None,
            queries: Optional[Union[query.QueryExpression, Dict[str, Any]]] = None,
            sort: Optional[SortFieldDict] = None,
    ) -> List[ModelType]:
        try:
            if not sort:
                sort = {"field": "created_at", "direction": SortOrder.desc}

            if not queries:
                queries = {"deleted_at": None}

            # Chuyển dict thành SortExpression
            sort_expr = sort_generate(self.model, sort)
            if page is not None and entries is not None:
                return await self.engine.find(
                    self.model, queries, sort=sort_expr,
                    skip=page * entries, limit=max(1, entries)
                )
            return await self.engine.find(
                self.model, queries, sort=sort_expr
            )

        except Exception:
            logger.error("Error in get_multi")
            return []

    async def create(
            self,
            *,
            obj_in: CreateSchemaType | ModelType,
    ) -> ModelType:

        db_obj: ModelType = self.model(**obj_in.model_dump())
        return await self.engine.save(db_obj)

    async def update(
            self,
            *,
            db_obj: ModelType,
            obj_in: Union[UpdateSchemaType, Dict[str, Any]],
    ) -> ModelType:
        obj_data: Any = jsonable_encoder(db_obj)
        update_data: UpdateSchemaType | Dict[str, Any]
        if isinstance(obj_in, dict):
            update_data = obj_in
        else:
            update_data = obj_in.model_dump(exclude_unset=True, exclude_none=True)

        for field in obj_data:
            if field in update_data:
                setattr(db_obj, field, update_data[field])
        result = await self.engine.save(db_obj)
        return result

    async def delete(
            self,
            *,
            item_id: str | ObjectId | None = None,
            item: ModelType | None = None,
    ) -> ModelType:
        if not item:
            assert item_id is not None
            item = await self.get(id=normalize_id(item_id))
        if item:
            return await soft_delete(item, engine=self.engine)
        else:
            raise NotExistedRecord(id=str(item_id), instance_name="Dialog")

    async def count(
            self, queries: Union[query.QueryExpression, Dict[str, Any]] | None = None
    ) -> int:
        if not queries:
            queries = {"$or": [{"deleted_at": None}, {"deleted_at": {"$exists": False}}]}
        return await self.engine.count(self.model, queries)

    def _build_pattern_query(self, search_text: str, search_fields: list[str] | None = None) -> query.QueryExpression:
        """Helper to build pattern search query expression"""

        if not search_text or not search_text.strip():
            return query.QueryExpression({})

        normalized_query = search_text.lower().strip()

        # Get string fields if not specified
        if not search_fields:
            search_fields = [
                field_name for field_name, field in self.model.__odm_fields__.items()
                if (getattr(field, 'outer_type_', None) or getattr(field, 'annotation', None)) == str
            ]

        # Build pattern conditions
        patterns = [
            query.match(getattr(self.model, field), f"(?i){re.escape(normalized_query)}")
            for field in search_fields
        ]

        # Combine with OR
        return query.or_(*patterns) if len(patterns) > 1 else patterns[0]

    async def get_multi_records(
            self,
            *,
            query_extra: Optional[query.QueryExpression | dict] = None,
            sort_logic: Optional[str] = "modified:desc",
            page: int | None = None,
            page_size: int = 6,
            start_date: Optional[str] = None,
            end_date: Optional[str] = None,
            start_field: str = 'created_at',
            search_text: Optional[str] = None,
            search_fields: Optional[list[str]] = None,
            **kwargs
    ) -> dict[str, list[ModelType] | int] | None:
        """
        Regular queries and pattern search
        """
        conditions = []

        # Initial query filter — check QueryExpression first since it's a dict subclass
        if isinstance(query_extra, query.QueryExpression):
            conditions.append(query_extra)
        elif isinstance(query_extra, dict):
            conditions.append(query.QueryExpression(query_extra))

        # Add pattern search if requested
        if search_text:
            pattern_query = self._build_pattern_query(search_text, search_fields)
            conditions.append(pattern_query)

        # Date filter
        if start_date:
            conditions.append(getattr(self.model, start_field) >= datetime.fromisoformat(start_date))
        if end_date:
            conditions.append(getattr(self.model, "modified") <= datetime.fromisoformat(end_date))

        if len(conditions) > 1:
            query_filter = conditions[0]
            for condition in conditions[1:]:
                query_filter = query.and_(query_filter, condition)
        elif len(conditions) == 1:
            query_filter = conditions[0]
        else:
            query_filter = None  # No filter

        # Sorting
        try:
            sort_field, sort_order = (sort_logic or "").split(":")
            sort_expr = query.asc(getattr(self.model, sort_field)) if sort_order == "asc" else query.desc(
                getattr(self.model, sort_field))
        except Exception:
            if hasattr(self.model, "modified"):
                sort_expr = query.desc(getattr(self.model, "modified"))
            else:
                sort_expr = query.desc(getattr(self.model, "created"))

        # Pagination
        skip = (page - 1) * page_size if page is not None else 0

        # Fetch documents
        if query_filter is not None:
            results = await self.engine.find(
                self.model,
                query_filter,
                sort=sort_expr,
                skip=skip,
                limit=page_size
            )
            total = await self.engine.count(self.model, query_filter)
        else:
            results = await self.engine.find(
                self.model,
                sort=sort_expr,
                skip=skip,
                limit=page_size
            )
            total = await self.engine.count(self.model)

        if not results:
            return None

        return {'results': results, "total": total}


    async def bulk_lookup(self, ids: list[str | ObjectId]) -> list[ModelType]:
        """Bulk lookup by IDs with soft-delete check"""
        ids = [ObjectId(id) for id in ids]

        query_expr = [self.model.id.in_(ids)]  # type: ignore[union-attr]

        # check boolean fields: is_hidden or is_deleted or status != DELETED
        if hasattr(self.model, "is_deleted"):
            query_expr.append(getattr(self.model, "is_deleted") == False)  # noqa: E712
        if hasattr(self.model, "is_hidden"):
            query_expr.append(getattr(self.model, "is_hidden") == False)  # noqa: E712
        if hasattr(self.model, "status"):
            query_expr.append(getattr(self.model, "status") != "DELETED")
        final_query = query.and_(*query_expr)
        return await self.engine.find(self.model, final_query)