from typing import Any, cast, Generic
from odmantic import ObjectId, query

from db.base_class import ModelType

class BulkOperation(Generic[ModelType]):

    model: type[ModelType]
    engine: Any

    async def bulk_lookup(self, ids: list[ObjectId | str]) -> list[ModelType]:
        """
        Retrieves data for multiple objects by their IDs
        """
        # Normalize
        object_ids = [ObjectId(id) if isinstance(id, str) else id for id in ids]

        # Prepare soft delete/hidden condition if exists
        query_conditions = [self.model.id.in_(object_ids)]  # type: ignore[union-attr]
        # Check for standard soft-delete fields
        if hasattr(self.model, "is_deleted"):
            query_conditions.append(getattr(self.model, "is_deleted") == False)  # noqa: E712
        if hasattr(self.model, "is_hidden"):
            query_conditions.append(getattr(self.model, "is_hidden") == False)  # noqa: E712

        # Combine conditions (AND)
        if len(query_conditions) == 1:
            mongo_query = query_conditions[0]
        else:
            # Assumes usage of ODM's .and_ method or construct accordingly
            mongo_query = query.and_(*query_conditions)

        # Async fetch
        return cast(list[ModelType], await self.engine.find(self.model, mongo_query))