from typing import Any, Optional
from loguru import logger
from odmantic import ObjectId
from datetime import datetime
from filters.base import BaseQueryBuilder, Operator
from filters.models import FilterCondition, EmptyResultBehavior, DynamicFilterRequest

class FileDynamicQueryBuilder(BaseQueryBuilder):

    @property
    def searchable_fields(self) -> list[str]:
        return ['file_name', 'extension']
    
    @property
    def soft_delete_field(self) -> str:
        return "status"
    
    async def _get_user_context(self, user_id: ObjectId, is_archived: Optional[bool] = None) -> dict[str, Any]:
        from services.file_access import get_user_file_context
        return await get_user_file_context(user_id)
    
    async def _transform_field_value(self, field: str, value: Any, user_context: dict[str, Any]) -> Any:
        """Transform field values before adding to query."""

        if field in ('file_ids', 'user_ids'):
            if isinstance(value, list):
                return [ObjectId(str(v)) for v in value]
            if field in ('uploaded_at', 'modified'):
                value_str = value if 'T' in value else f'{value}T00:00:00.000Z'
                return datetime.strptime(value_str, '%Y-%m-%dT%H:%M:%S.%fZ')
            
        return value

    async def build_dynamic_query(
        self,
        user_id: Optional[ObjectId] = None,
        filters: Optional[DynamicFilterRequest] = None,
        search_text: Optional[str] = None,
        **kwargs: Any,
    ) -> tuple[dict[str, Any], dict[str, Any]]:
        assert user_id is not None
        try:
            filters = self.combine_search_text(filters, search_text)
            if not filters or not filters.filter_groups:
                empty_behavior = filters.empty_result if filters else EmptyResultBehavior.ALL_ROWS

                return await self._handle_empty_filters(empty_behavior, user_id)
            user_context = await self._get_user_context(user_id)
            group_conditions = []
            for group in filters.filter_groups:
                group_condition = await self._build_filter_group(group, user_context=user_context)
                if group_condition:
                    group_conditions.append(group_condition)

            # Handle case where all groups were invalid
            if not group_conditions:
                return await self._handle_empty_filters(filters.empty_result, user_id)

            # Combine groups based on parent match_type
            final_conditions = self._combine_conditions(group_conditions, filters.match_type)

            # Apply authorization constraints
            authorized_conditions = await self._apply_authorization_constraints(
                final_conditions, user_context
            )

            return (authorized_conditions, {})

        except Exception as e:
                logger.error(f'Failed to build user dynamic query: {e}')
                return await self._handle_empty_filters(EmptyResultBehavior.NO_ROWS, user_id=user_id)
        
    async def _validate_condition(self, *, condition: FilterCondition) -> Optional[FilterCondition]:
        """Validate user filter conditions."""
        valid_fields = {'user_ids', 'modified', 'knowledge_ids', 'uploaded_at', 'file_name', 'extension'}
        if condition.field not in valid_fields:
            logger.warning(f'Invalid field for file filtering: {condition.field}')
            return None
        else:
            if condition.field in ('user_ids', 'knowledge_ids') and condition.operator not in (Operator.IN, Operator.NOT_IN):
                logger.warning(f'Invalid operator {condition.operator} for field {condition.field}')
                return None
            else:
                return condition
            
    async def _handle_empty_filters(
        self,
        empty_behavior: EmptyResultBehavior,
        user_id: Optional[ObjectId] = None,
        **kwargs: Any,
    ) -> tuple[dict[str, Any], dict[str, Any]]:
        """Handle empty filters for user queries."""
        if empty_behavior == EmptyResultBehavior.NO_ROWS:
            return ({'_id': {'$exists': False}}, {})
        elif empty_behavior == EmptyResultBehavior.ALL_ROWS:
            assert user_id is not None
            user_context = await self._get_user_context(user_id)
            # Apply authorization constraints with no filter conditions
            query_ = await self._apply_authorization_constraints({}, user_context)
            return (query_, {})
        else:
            return ({}, {})

    async def _apply_authorization_constraints(
        self,
        conditions: dict[str, Any],
        user_context: dict[str, Any],
        is_archived: Optional[bool] = None,
    ) -> dict[str, Any]:
        """Apply authorization constraints to the query."""

        user_id = user_context.get("user_id")
        accessible_files = user_context.get("accessible_files", [])
        constraint_query = {
            self.soft_delete_field: {"$ne": "deleted"}
        }
        # Build authorization conditions: user owns it OR user has file access
        auth_conditions: list[dict[str, Any]] = [{"author_id": ObjectId(user_id)}]

        if accessible_files:
            auth_conditions.append({"_id": {"$in": accessible_files}})

        # Combine with existing conditions
        if conditions:
            return {
                "$and": [
                    conditions,
                    {"$or": auth_conditions},
                    constraint_query
                ]
            }
        return {
            "$and": [
                {"$or": auth_conditions},
                constraint_query
            ]
        }