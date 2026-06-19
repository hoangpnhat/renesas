from typing import Any, Optional
from loguru import logger
from odmantic import ObjectId
from datetime import datetime
from filters.base import BaseQueryBuilder, Operator
from filters.models import FilterCondition, EmptyResultBehavior, DynamicFilterRequest

class GroupDynamicQueryBuilder(BaseQueryBuilder):

    @property
    def searchable_fields(self) -> list[str]:
        return ['name', 'description']

    async def _get_user_context(self, user_id: ObjectId, is_archived: Optional[bool] = None) -> dict[str, Any]:
        from crud import group_crud, share_crud

        accessible_group_ids: list[Any] = []
        group_knowledge_ids: dict[str, list[Any]] = {}

        try:
            # Find all groups where user is a member
            user_groups = await group_crud.get_user_groups(user_id)
            accessible_group_ids = [group.id for group in user_groups]

            # Fetch knowledge shares for all accessible groups
            if accessible_group_ids:
                knowledge_shares = await share_crud.get_group_knowledge(accessible_group_ids)

                # Group knowledge IDs by group_id
                for share in knowledge_shares:
                    group_id_str = str(share.target_id)
                    if group_id_str not in group_knowledge_ids:
                        group_knowledge_ids[group_id_str] = []
                    group_knowledge_ids[group_id_str].append(share.shareable_id)

        except Exception as e:
            logger.error(f"Error getting user context for group access: {e}")

        return {
            'user_id': user_id,
            'is_archived': is_archived,
            'accessible_group_ids': accessible_group_ids,
            'group_knowledge_ids': group_knowledge_ids
            }

    
    async def _transform_field_value(self, field: str, value: Any, user_context: dict[str, Any]) -> Any:
        """Transform field values before adding to query."""

        if field in ('user_ids'):
            if isinstance(value, list):
                return [ObjectId(str(v)) for v in value]
            if field in ('created_at', 'last_modified'):
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
            is_archived = kwargs.get("is_archived")
            filters = self.combine_search_text(filters, search_text)
            if not filters or not filters.filter_groups:
                empty_behavior = filters.empty_result if filters else EmptyResultBehavior.ALL_ROWS

                return await self._handle_empty_filters(empty_behavior, user_id, is_archived=is_archived)
            user_context = await self._get_user_context(user_id, is_archived)
            group_conditions = []
            for group in filters.filter_groups:
                group_condition = await self._build_filter_group(group, user_context=user_context)
                if group_condition:
                    group_conditions.append(group_condition)

            if not group_conditions:
                return await self._handle_empty_filters(filters.empty_result, user_id, is_archived=is_archived)

            final_conditions = self._combine_conditions(group_conditions, filters.match_type)

            # Apply authorization constraints
            authorized_conditions = await self._apply_authorization_constraints(
                final_conditions, user_context, is_archived
            )

            # Return query with metadata containing group knowledge IDs
            metadata = {
                'group_knowledge_ids': user_context.get('group_knowledge_ids', {})
            }

            return (authorized_conditions, metadata)

        except Exception as e:
                logger.error(f'Failed to build user dynamic query: {e}')
                return await self._handle_empty_filters(EmptyResultBehavior.NO_ROWS, user_id=user_id)
        
    async def _validate_condition(self, *, condition: FilterCondition) -> Optional[FilterCondition]:
        """Validate user filter conditions."""
        valid_fields = {'user_ids', 'last_modified', 'knowledge_ids', 'uploaded_at', 'name', 'description'}
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
        is_archived: Optional[bool] = kwargs.get("is_archived")
        if empty_behavior == EmptyResultBehavior.NO_ROWS:
            return ({'_id': {'$exists': False}}, {})
        elif empty_behavior == EmptyResultBehavior.ALL_ROWS:
            assert user_id is not None
            user_context = await self._get_user_context(user_id, is_archived)
            # Apply authorization constraints with no filter conditions
            query_ = await self._apply_authorization_constraints({}, user_context, is_archived)
            metadata = {
                'group_knowledge_ids': user_context.get('group_knowledge_ids', {})
            }
            return (query_, metadata)
        else:
            return ({}, {})

    async def _apply_authorization_constraints(
        self,
        conditions: dict[str, Any],
        user_context: dict[str, Any],
        is_archived: Optional[bool] = None,
    ) -> dict[str, Any]:

        user_id = user_context["user_id"]
        is_archived = user_context["is_archived"]
        accessible_group_ids = user_context['accessible_group_ids']

        constraint_query = {
            "is_deleted": {"$ne": True}
        }

        # Fundamental query: user owns the group OR is a member with joined_at not null
        auth_conditions = [
            {"owner_id": ObjectId(user_id)},
            {
                "members": {
                    "$elemMatch": {
                        "user_id": ObjectId(user_id),
                        "joined_at": {"$ne": None}
                    }
                }
            }
        ]

        if accessible_group_ids:
            auth_conditions.append({"_id": {"$in": accessible_group_ids}})

        # Archived filter
        if is_archived:
            archived_filter = {"is_archived": {"$eq": True}}
        else:
            archived_filter = {"is_archived": {"$ne": True}}

        # Combine
        base_filter = {
            "$and": [
                {"$or": auth_conditions},
                archived_filter,
                constraint_query,
            ]
        }

        if conditions:
            return {
                "$and": [
                    conditions,
                    base_filter
                ]
            }

        return base_filter