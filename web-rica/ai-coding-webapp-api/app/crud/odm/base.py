#  Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.

import re
from datetime import datetime
from typing import Any, Generic, Type, Union, Optional, cast

from fastapi.encoders import jsonable_encoder
from odmantic import ObjectId, query

from db.base_class import ModelType, CreateSchemaType, UpdateSchemaType
from db.session import get_engine, TimezoneAwareEngine
from schemas.base_schema import CheckDuplicateParams
from utils.handle_exception import ApplicationException
from utils.helpers import normalize_id, datetime_now_sec, UploadHelper


class CRUDBase(Generic[ModelType, CreateSchemaType, UpdateSchemaType]):
    def __init__(self, model: Type[ModelType]) -> None:
        self.model: type[ModelType] = model
        self.engine: TimezoneAwareEngine = cast(TimezoneAwareEngine, get_engine())

    async def find_with_query(
            self,
            *,
            to_query: query.QueryExpression | dict,
            identity: ObjectId | str | None = None,
            raise_error: bool = False,
    ) -> ModelType | None:

        if isinstance(to_query, dict):
            to_query = query.QueryExpression(to_query)

        content = await self.engine.find_one(self.model, to_query)
        if not content and raise_error:
            raise ApplicationException("NOT_EXISTED_ERROR", obj=f"{str(identity)} in {self.model.__repr_name__}")
        return content

    async def get(self, id: Any) -> ModelType | None:
        model = await self.engine.find_one(self.model, self.model.id == id)
        if not model:
            raise ApplicationException("NOT_EXISTED_ERROR", obj=f"{str(id)} in {self.model.__repr_name__}")
        return model

    async def create(
            self,
            *,
            obj_in: CreateSchemaType | ModelType,
            session: Optional[Any] = None
    ) -> ModelType:
        obj_in_data: Any = jsonable_encoder(obj_in)
        db_obj: ModelType = self.model(**obj_in_data)
        return await self.engine.save(db_obj, session=session)

    async def update(
            self,
            *,
            db_obj: ModelType,
            obj_in: Union[UpdateSchemaType, dict[str, Any]],
            session: Optional[Any] = None
    ) -> ModelType:
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
        result = await self.engine.save(db_obj, session = session)
        return result

    async def _get_duplicate_conflict_doc(
            self,
            name_field: str,
            user_field: str,
            delete_field: str | None,
            name_value: str,
            user_value: Any
    ):
        conflict_query = {
            name_field: name_value,
            user_field: normalize_id(user_value)
        }
        if delete_field:
            conflict_query.update({
                delete_field: False
            })
        return await self.engine.find_one(self.model, conflict_query)

    async def _get_duplicate_conflict_docs(
            self,
            name_field: str,
            user_field: str,
            delete_field: str | None,
            base_name: str,
            extension: str,
            user_value: Any
    ):
        """Get all files matching base name pattern (including versions)"""
        import re
        pattern = rf"^{re.escape(base_name)}( \(\d+\))?{re.escape(extension)}$"

        conflict_query = {
            name_field: {"$regex": pattern},
            user_field: normalize_id(user_value)
        }
        if delete_field:
            conflict_query.update({
                delete_field: False
            })

        return await self.engine.find(self.model, conflict_query)

    async def _generate_non_conflicting_name(
            self,
            name_field: str,
            user_field: str,
            delete_field: str | None,
            base: str,
            ext: str,
            user_value: Any
    ):
        suffix_query = {
            name_field: {
                "$regex": f"^{re.escape(base)}( \\(\\d+\\))*{re.escape(ext)}$",
                "$options": "i"
            },
            user_field: normalize_id(user_value)
        }
        if delete_field:
            suffix_query.update({
                delete_field: False
            })
        existing = await self.engine.find(self.model, suffix_query)
        existing_names = [doc.__getattribute__(name_field) for doc in existing]
        return UploadHelper.generate_incremented_name(base, ext, existing_names)

    async def check_duplicate_context(
            self,
            *,
            obj_in_data: dict,
            check_duplicate_fields: CheckDuplicateParams,
            find_unique_name: bool = False
    ) -> dict[str, Any]:
        """
        Check for duplicate and return a context object for later processing.
        If find_unique_name=True, will find next available version number.
        """
        name_field = check_duplicate_fields.object_field
        user_field = check_duplicate_fields.user_field
        delete_field = check_duplicate_fields.delete_field

        if not name_field or not user_field:
            raise ApplicationException("BAD_REQUEST", detail="Missing required fields to check duplicate")

        name_value = obj_in_data[name_field]
        user_value = obj_in_data[user_field]
        base, ext = UploadHelper.get_base_and_ext(name_value)

        # Find the duplicate (single doc)
        existing_doc = await self._get_duplicate_conflict_doc(
            name_field, user_field, delete_field, name_value, user_value
        )

        # Context object for caching
        context_data = {
            "duplicate_exists": existing_doc is not None,
            "duplicate_id": str(existing_doc.id) if existing_doc else None,
            "existing_doc_data": jsonable_encoder(existing_doc) if existing_doc else None,
            "name_field": name_field,
            "user_field": user_field,
            "delete_field": delete_field,
            "base": base,
            "ext": ext,
            "metadata": {
                "name_value": name_value,
                "user_value": user_value
            }
        }

        if find_unique_name and existing_doc is not None:
            all_versions = await self._get_duplicate_conflict_docs(
                name_field, user_field, delete_field, base, ext, user_value
            )

            existing_names = {doc.file_name for doc in all_versions}

            version = 1
            unique_name = name_value
            while unique_name in existing_names:
                unique_name = f"{base} ({version}){ext}"
                version += 1

                if version > 1000:  # Safety check
                    raise ApplicationException(
                        "TOO_MANY_DUPLICATES",
                        detail=f"File {name_value} has too many versions"
                    )

            context_data["unique_file_name"] = unique_name
            context_data["version_number"] = version - 1 if version > 1 else None
            context_data["all_existing_versions"] = list(existing_names)

        return context_data

    async def process_with_duplicate_context(
            self,
            *,
            obj_in_data: dict | ModelType,
            context_data: dict[str, Any],
            is_replace: bool,
            preserved_fields: Optional[list[str]] = None
    ) -> ModelType:
        """
        Process document using the cached context data from previous check.
        """
        if is_replace is None:
            raise ApplicationException("BAD_REQUEST", detail="is_replace parameter must be explicitly set")

        if not isinstance(obj_in_data, dict):
            obj_in_data = obj_in_data.model_dump(exclude_unset=True)

        # Context object
        name_field = context_data["name_field"]
        user_field = context_data["user_field"]
        delete_field = context_data["delete_field"]
        base = context_data["base"]
        ext = context_data["ext"]
        name_value = context_data["metadata"]["name_value"]
        user_value = context_data["metadata"]["user_value"]

        # Verify the document data is match
        if obj_in_data[name_field] != name_value or obj_in_data[user_field] != ObjectId(user_value):
            raise ApplicationException(
                "NOT_EXISTED_ERROR",
                obj="Document data match what was checked previously"
            )

        # Get existing doc
        existing_doc = None
        if context_data["duplicate_exists"] and context_data["existing_doc_data"]:
            existing_doc = self.model(**context_data["existing_doc_data"])

        if existing_doc:
            if is_replace is True:
                # Replace the old one ---> set the error field to None, preserved the uploaded_field
                if hasattr(obj_in_data, "error"):
                    setattr(obj_in_data, "error", None)
                if preserved_fields:
                    obj_in_data = UploadHelper.strip_immutable_fields(
                        obj_in_data=obj_in_data,
                        fields=preserved_fields
                    )
                return await self.update(db_obj=existing_doc, obj_in=obj_in_data)

            elif is_replace is False:
                # Create new one ---> incremental name
                obj_in_data[name_field] = await self._generate_non_conflicting_name(
                    name_field, user_field, delete_field, base, ext, user_value
                )

        # Create new document
        db_obj: ModelType = self.model(**obj_in_data)
        return await self.engine.save(db_obj)

    async def advanced_create(
            self,
            *,
            obj_in: CreateSchemaType | ModelType,
            check_duplicate_fields: CheckDuplicateParams,
            is_replace: Optional[bool] = None,
            preserved_fields: list[str] | None = None,
            session: Optional[Any] = None
    ) -> ModelType:
        """
        Create or update a document with duplicate checking.
        This combines both check_duplicate and process_with_duplicate.
        """
        obj_in_data = jsonable_encoder(obj_in)

        # Extract fields needed for duplicate checking
        name_field = check_duplicate_fields.object_field
        user_field = check_duplicate_fields.user_field
        delete_field = check_duplicate_fields.delete_field

        if not name_field or not user_field:
            raise ApplicationException("BAD_REQUEST", detail="Missing required fields to check duplicate")

        name_value = obj_in_data[name_field]
        user_value = obj_in_data[user_field]
        base, ext = UploadHelper.get_base_and_ext(name_value)

        # Single database query to find the duplicate
        existing_doc = await self._get_duplicate_conflict_doc(
            name_field, user_field, delete_field, name_value, user_value
        )

        # Handle duplicate based on is_replace
        if existing_doc and is_replace is None:
            raise ApplicationException("EXISTED_ERROR", obj=f"File **{name_value}**")

        # Process with is_replace parameter (defaulting to False if not specified)
        effective_is_replace = is_replace if is_replace is not None else False

        if existing_doc:
            if effective_is_replace is True:
                # Update existing document
                if preserved_fields:
                    obj_in_data = UploadHelper.strip_immutable_fields(
                        obj_in_data=obj_in_data,
                        fields=preserved_fields
                    )
                return await self.update(db_obj=existing_doc, obj_in=obj_in_data, session=session)

            elif effective_is_replace is False:
                # Generate unique name and create new document
                obj_in_data[name_field] = await self._generate_non_conflicting_name(
                    name_field, user_field, delete_field, base, ext, user_value
                )

        # Create new document
        db_obj: ModelType = self.model(**obj_in_data)
        return await self.engine.save(db_obj, session=session)

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
            sort_expr = query.desc(getattr(self.model, "modified"))

        # Pagination
        skip = (page - 1) * page_size if page is not None else 0

        # Fetch documents
        if query_filter is not None:
            results = await self.engine.find(
                self.model,
                query_filter,
                sort=[sort_expr],
                skip=skip,
                limit=page_size
            )
            total = await self.engine.count(self.model, query_filter)
        else:
            results = await self.engine.find(
                self.model,
                sort=[sort_expr],
                skip=skip,
                limit=page_size
            )
            total = await self.engine.count(self.model)

        if not results:
            return None

        return {'results': results, "total": total}

