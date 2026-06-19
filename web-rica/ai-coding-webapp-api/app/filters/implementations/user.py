from typing import Any, Optional
from loguru import logger
from bson import ObjectId

from filters.base import BaseQueryBuilder
from filters.models import FilterCondition, EmptyResultBehavior, DynamicFilterRequest


class UserDynamicQueryBuilder(BaseQueryBuilder):

    @property
    def searchable_fields(self) -> list[str]:
        return ['full_name', 'email']
    
    @property
    def soft_delete_field(self) -> str:
        return "is_active"

    async def build_dynamic_query(
        self,
        user_id: Optional[ObjectId] = None,
        filters: Optional[DynamicFilterRequest] = None,
        search_text: Optional[str] = None,
        **kwargs: Any,
    ) -> tuple[dict[str, Any], dict[str, Any]]:

        try:
            if user_id is None:
                user_id = kwargs.get("user_id")
            # Combine filters with text search
            filters = self.combine_search_text(filters, search_text)

            # Handle empty filters
            if not filters or not filters.filter_groups:
                empty_behavior = filters.empty_result if filters else EmptyResultBehavior.ALL_ROWS
                return await self._handle_empty_filters(empty_behavior, user_id)

            # Build conditions from filter groups
            group_conditions = []
            for group in filters.filter_groups:
                group_condition = await self._build_filter_group(group, user_context=None)
                if group_condition:
                    group_conditions.append(group_condition)

            # Handle case where all groups were invalid
            if not group_conditions:
                return await self._handle_empty_filters(filters.empty_result, user_id)

            # Combine groups based on parent match_type
            conditions = self._combine_conditions(group_conditions, filters.match_type)

            # Apply constraints
            assert user_id is not None
            final_conditions = await self._apply_authorization_constraints(
                conditions, {"user_id": user_id}
            )

            return final_conditions, {}

        except Exception as e:
            logger.error(f"Failed to build user dynamic query: {e}")
            return await self._handle_empty_filters(EmptyResultBehavior.NO_ROWS)

    async def _validate_condition(
        self,
        *,
        condition: FilterCondition
    ) -> Optional[FilterCondition]:
        """Validate user filter conditions."""
        valid_fields = {"full_name", "email", "user_id"}

        if condition.field not in valid_fields:
            logger.warning(f"Invalid field for user filtering: {condition.field}")
            return None

        return condition

    async def _handle_empty_filters(
        self,
        empty_behavior: EmptyResultBehavior,
        user_id: Optional[ObjectId] = None,
        **kwargs: Any,
    ) -> tuple[dict[str, Any], dict[str, Any]]:
        """Handle empty filters for user queries."""
        if empty_behavior == EmptyResultBehavior.NO_ROWS:
            return {"_id": {"$exists": False}}, {}
        elif empty_behavior == EmptyResultBehavior.ALL_ROWS:
            assert user_id is not None
            base_query = await self._apply_authorization_constraints({}, {"user_id": user_id})
            return base_query, {}
        else:
            return {}, {}
        
    async def _apply_authorization_constraints(
        self,
        conditions: dict[str, Any],
        user_context: dict[str, Any],
        is_archived: Optional[bool] = None,
    ) -> dict[str, Any]:
        """Apply authorization constraints to the query."""
        user_id = user_context.get("user_id")
        constraint_query = {
            self.soft_delete_field: {"$ne": False},
            '_id': {"$ne": ObjectId(user_id)} # current user cannot find themself
        }

        # Combine with existing conditions
        if conditions:
            return {
                "$and": [
                    conditions,
                    constraint_query
                ]
            }
        return constraint_query