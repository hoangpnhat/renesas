from abc import ABC, abstractmethod
from typing import Any, Optional, Union
from odmantic import ObjectId
from loguru import logger

from .models import (
    DynamicFilterRequest, FilterGroup, FilterCondition,
    Operator, MatchType, EmptyResultBehavior
)


class BaseQueryBuilder(ABC):

    def __init__(self):
        self.operator_mapping = {
            Operator.IN: self._build_in_condition,
            Operator.NOT_IN: self._build_not_in_condition,
            Operator.CONTAINS: self._build_contains_condition,
            Operator.NOT_CONTAINS: self._build_not_contains_condition,
            Operator.EQUALS: self._build_equals_condition,
            Operator.NOT_EQUALS: self._build_not_equals_condition,
            Operator.GREATER_THAN: self._build_gt_condition,
            Operator.LESS_THAN: self._build_lt_condition,
            Operator.GREATER_EQUAL: self._build_gte_condition,
            Operator.LESS_EQUAL: self._build_lte_condition,
        }

    @property
    def searchable_fields(self) -> list[str]:
        """Fields that can be searched with text search"""
        return ['name', 'description']


    @abstractmethod
    async def build_dynamic_query(
        self,
        user_id: Optional[ObjectId] = None,
        filters: Optional[DynamicFilterRequest] = None,
        search_text: Optional[str] = None,
        **kwargs
    ) -> tuple[dict[str, Any], dict[str, Any]]:
        """
        Build a MongoDB query from filters and search text.
        
        Returns:
            tuple: (query_conditions, metadata)
        """
        pass

    # Filter Processing
    def combine_search_text(
        self,
        filters: Optional[DynamicFilterRequest] = None,
        search_text: Optional[str] = None
    ) -> Optional[DynamicFilterRequest]:
        """Combine existing filters with text search by adding a search filter group."""
        if not filters and not search_text:
            return None
        # Create filters with search group if only search_text exists
        if search_text and not filters:
            return DynamicFilterRequest(
                filter_groups=[self._create_search_filter_group(search_text)],
                match_type=MatchType.ANY
            )

        # Append search group to existing filters
        if filters and search_text:
            filters.filter_groups.append(self._create_search_filter_group(search_text))
            return filters

        # Return filters as-is if no search text
        return filters

    def _create_search_filter_group(self, search_text: str) -> FilterGroup:
        """Create a filter group for text search across searchable fields."""
        return FilterGroup(
            conditions=[
                FilterCondition(
                    field=field,
                    operator=Operator.CONTAINS,
                    value=search_text
                ) for field in self.searchable_fields
            ],
            group_operator=MatchType.ANY
        )

    async def _build_filter_group(
        self,
        group: FilterGroup,
        user_context: Optional[dict[str, Any]] = None
    ) -> Optional[dict[str, Any]]:
        """Build MongoDB query from a single filter group."""
        conditions = []
        special_fields = user_context.get("special_treatments_fields", set()) if user_context else set()

        for condition in group.conditions:
            # Validate condition
            validated_condition = await self._validate_condition(condition=condition)
            if not validated_condition:
                continue

            # Build MongoDB condition
            if validated_condition.field in special_fields:
                mongo_condition = await self._inference_convert(validated_condition)
            else:
                mongo_condition = await self._build_mongo_condition(
                    validated_condition, user_context or {}
                )

            if mongo_condition:
                conditions.append(mongo_condition)

        if not conditions:
            return None

        # Combine conditions based on group operator
        return self._combine_conditions(conditions, group.group_operator)

    def _combine_conditions(
        self,
        conditions: list[dict[str, Any]],
        operator: MatchType
    ) -> dict[str, Any]:
        """Combine multiple conditions with AND or OR logic."""
        if len(conditions) == 1:
            return conditions[0]
        if operator == MatchType.ANY:
            return {"$or": conditions}
        return {"$and": conditions}

    async def _build_mongo_condition(
        self,
        condition: FilterCondition,
        user_context: dict[str, Any]
    ) -> Optional[dict[str, Any]]:
        """Build a single MongoDB condition from a filter condition."""
        try:
            builder_func = self.operator_mapping.get(condition.operator)
            if not builder_func:
                logger.warning(f"Unsupported operator: {condition.operator}")
                return None

            # Transform field name and value
            mongo_field = await self._transform_field_name(condition.field, user_context)
            mongo_value = await self._transform_field_value(
                condition.field, condition.value, user_context
            )

            return builder_func(mongo_field, mongo_value)

        except Exception as e:
            logger.error(f"Failed to build condition for {condition.field}: {e}")
            return None

    def _build_in_condition(self, field: str, value: list[Any]) -> dict[str, Any]:
        return {field: {"$in": value}}

    def _build_not_in_condition(self, field: str, value: list[Any]) -> dict[str, Any]:
        return {field: {"$nin": value}}

    def _build_contains_condition(self, field: str, value: Union[str, list[str]]) -> dict[str, Any]:
        if isinstance(value, list):
            return {field: {"$in": value}}
        return {field: {"$regex": str(value), "$options": "i"}}

    def _build_not_contains_condition(self, field: str, value: Union[str, list[str]]) -> dict[str, Any]:
        if isinstance(value, list):
            return {field: {"$nin": value}}
        return {field: {"$not": {"$regex": str(value), "$options": "i"}}}

    def _build_equals_condition(self, field: str, value: Any) -> dict[str, Any]:
        return {field: value}

    def _build_not_equals_condition(self, field: str, value: Any) -> dict[str, Any]:
        return {field: {"$ne": value}}

    def _build_gt_condition(self, field: str, value: Any) -> dict[str, Any]:
        return {field: {"$gt": value}}

    def _build_lt_condition(self, field: str, value: Any) -> dict[str, Any]:
        return {field: {"$lt": value}}

    def _build_gte_condition(self, field: str, value: Any) -> dict[str, Any]:
        return {field: {"$gte": value}}

    def _build_lte_condition(self, field: str, value: Any) -> dict[str, Any]:
        return {field: {"$lte": value}}


    @abstractmethod
    async def _validate_condition(
        self,
        *,
        condition: FilterCondition
    ) -> Optional[FilterCondition]:
        """Validate a filter condition. Return None if invalid."""
        pass

    @abstractmethod
    async def _handle_empty_filters(
        self,
        empty_behavior: EmptyResultBehavior,
        user_id: Optional[ObjectId] = None,
        **kwargs
    ) -> tuple[dict[str, Any], dict[str, Any]]:
        """Handle cases where no filters are provided or all filters are invalid."""
        pass


    # utilities
    async def _get_user_context(self, user_id: ObjectId, is_archived: Optional[bool] = None) -> dict[str, Any]:
        """Get user context for authorization and field transformation."""
        return {"user_id": user_id}

    async def _apply_authorization_constraints(
        self,
        conditions: dict[str, Any],
        user_context: dict[str, Any],
        is_archived: Optional[bool] = None,
    ) -> dict[str, Any]:
        """Apply authorization constraints to the query."""
        return conditions

    async def _build_metadata(
        self,
        user_context: dict[str, Any],
        filters: Optional[DynamicFilterRequest]
    ) -> dict[str, Any]:
        """Build metadata to return alongside the query."""
        return {}

    async def _transform_field_name(
        self,
        field: str,
        user_context: dict[str, Any]
    ) -> str:
        """Transform API field names to database field names."""
        return field

    async def _transform_field_value(
        self,
        field: str,
        value: Any,
        user_context: dict[str, Any]
    ) -> Any:
        """Transform field values before adding to query."""
        return value

    async def _inference_convert(self, condition: FilterCondition) -> Optional[dict[str, Any]]:
        """
        Convert conditions that require special inference logic.
        
        E.g., status == 'failed' might include both failed records and 
        processing records that exceeded time limits.
        """
        return None