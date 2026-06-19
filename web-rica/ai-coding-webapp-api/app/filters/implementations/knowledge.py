from typing import Any, Optional
from loguru import logger
from odmantic import ObjectId
from datetime import datetime
from filters.base import BaseQueryBuilder, Operator
from filters.models import FilterCondition, EmptyResultBehavior, DynamicFilterRequest

class KnowledgeDynamicQueryBuilder(BaseQueryBuilder):

    @property
    def searchable_fields(self) -> list[str]:
        """Fields that can be searched with text search"""
        return ['name', 'description']
    
    @property
    def soft_delete_field(self) -> str:
        """Field used for soft delete"""
        return "is_deleted"
    
    async def _get_user_context(self, user_id: ObjectId, is_archived: Optional[bool] = None) -> dict[str, Any]:
        """
        1. Knowledge bases owned by the user (owner_id = user_id)
        2. Knowledge bases directly shared with user
        3. Knowledge bases shared with user's groups
        4. Public knowledge bases (visibility = "public")
        """
        from crud import group_crud, share_crud
        from const.common import ShareType, ResourceType, OwnerType
        
        accessible_kb_ids = []
        
        try:
            # Find all groups where user is a member
            user_groups = await group_crud.get_user_groups(user_id)
            group_ids = [group.id for group in user_groups]
            
            if group_ids:
                kb_shares = await share_crud.get_group_knowledge(group_ids)
                accessible_kb_ids.extend([share.shareable_id for share in kb_shares])
                logger.debug(f"Found {len(kb_shares)} KB shares through groups")
            
            # Direct shares to user
            direct_shares = await share_crud.get_kb_shares_for_targets(
                target_ids=[user_id],
                target_type=OwnerType.USER
            )
            accessible_kb_ids.extend([share.shareable_id for share in direct_shares])
            logger.debug(f"Found {len(direct_shares)} direct KB shares")
            
            # Remove duplicates
            accessible_kb_ids = list(set(accessible_kb_ids))
            
        except Exception as e:
            logger.error(f"Error getting user context for KB access: {e}")
        
        return {
            'user_id': user_id,
            'group_ids': group_ids,
            'accessible_kb_ids': accessible_kb_ids
        }
    
    async def _transform_field_value(
        self, 
        field: str, 
        value: Any, 
        user_context: dict[str, Any]
    ) -> Any:
        """Transform field values before adding to query."""
        
        # Handle ObjectId fields
        if field in ('kb_ids', 'owner_ids', 'group_ids'):
            if isinstance(value, list):
                return [ObjectId(str(v)) for v in value]
            return ObjectId(str(value))
        
        # Handle datetime fields
        if field in ('created_at', 'last_modified', 'archived_at'):
            if isinstance(value, str):
                value_str = value if 'T' in value else f'{value}T00:00:00.000Z'
                return datetime.strptime(value_str, '%Y-%m-%dT%H:%M:%S.%fZ')
        
        # Handle boolean fields
        if field in ('is_archived', 'is_deleted'):
            if isinstance(value, str):
                return value.lower() in ('true', '1', 'yes')
            return bool(value)
        
        # Handle visibility enum
        if field == 'visibility':
            from const.common import ShareType
            valid_values = [ShareType.PRIVATE, ShareType.PUBLIC]
            if value not in valid_values:
                logger.warning(f"Invalid visibility value: {value}")
                return None
        
        return value
    
    async def build_dynamic_query(
        self,
        user_id: Optional[ObjectId] = None,
        filters: Optional[DynamicFilterRequest] = None,
        search_text: Optional[str] = None,
        **kwargs
    ) -> tuple[dict[str, Any], dict[str, Any]]:
        """Build dynamic query with authorization constraints."""
        assert user_id is not None

        try:
            # Combine search text into filters
            filters = self.combine_search_text(filters, search_text)

            # Handle empty filters
            if not filters or not filters.filter_groups:
                empty_behavior = filters.empty_result if filters else EmptyResultBehavior.ALL_ROWS
                return await self._handle_empty_filters(empty_behavior, user_id)

            # Get user context
            user_context = await self._get_user_context(user_id)

            # Build filter group conditions
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
            logger.error(f'Failed to build knowledge dynamic query: {e}')
            return await self._handle_empty_filters(EmptyResultBehavior.NO_ROWS, user_id=user_id)
    
    async def _validate_condition(
        self, 
        *, 
        condition: FilterCondition
    ) -> Optional[FilterCondition]:
        """Validate knowledge base filter conditions."""
        
        valid_fields = {
            'owner_ids',
            'kb_ids',
            'name',
            'description',
            'visibility',
            'is_archived',
            'created_at',
            'last_modified',
            'group_ids'
        }
        
        if condition.field not in valid_fields:
            logger.warning(f'Invalid field for knowledge filtering: {condition.field}')
            return None
        
        # Validate operators for specific fields
        if condition.field in ('owner_ids', 'kb_ids', 'group_ids'):
            if condition.operator not in (Operator.IN, Operator.NOT_IN):
                logger.warning(
                    f'Invalid operator {condition.operator} for field {condition.field}. '
                    f'Only IN and NOT_IN are supported.'
                )
                return None
        
        if condition.field in ('is_archived', 'is_deleted'):
            if condition.operator not in (Operator.EQUALS, Operator.NOT_EQUALS):
                logger.warning(
                    f'Invalid operator {condition.operator} for boolean field {condition.field}'
                )
                return None
        
        if condition.field in ('created_at', 'last_modified', 'archived_at'):
            valid_date_operators = (
                Operator.EQUALS, 
                Operator.NOT_EQUALS,
                Operator.GREATER_THAN,
                Operator.GREATER_EQUAL,
                Operator.LESS_THAN,
                Operator.LESS_EQUAL
            )
            if condition.operator not in valid_date_operators:
                logger.warning(
                    f'Invalid operator {condition.operator} for date field {condition.field}'
                )
                return None
        
        return condition
    
    async def _handle_empty_filters(
        self,
        empty_behavior: EmptyResultBehavior,
        user_id: Optional[ObjectId] = None,
        **kwargs: Any,
    ) -> tuple[dict[str, Any], dict[str, Any]]:
        """Handle empty filters for knowledge base queries."""

        if empty_behavior == EmptyResultBehavior.NO_ROWS:
            # Return impossible condition
            return ({'_id': {'$exists': False}}, {})

        elif empty_behavior == EmptyResultBehavior.ALL_ROWS:
            # Return all accessible KBs for user
            assert user_id is not None
            user_context = await self._get_user_context(user_id)
            query_ = await self._apply_authorization_constraints({}, user_context)
            return (query_, {})

        else:
            # Return empty query (no filters)
            return ({}, {})
    
    async def _apply_authorization_constraints(
        self,
        conditions: dict[str, Any],
        user_context: dict[str, Any],
        is_archived: Optional[bool] = None,
    ) -> dict[str, Any]:
        from const.common import ShareType
        
        user_id = user_context["user_id"]
        accessible_kb_ids = user_context.get("accessible_kb_ids", [])
        
        # Base constraint: not deleted
        constraint_query = {
            self.soft_delete_field: False
        }
        
        # Build authorization conditions
        auth_conditions = [
            {"owner_id": ObjectId(user_id)},
            {"visibility": ShareType.PUBLIC}
        ]
        
        # 3. KB is in accessible list (shared directly or via group)
        if accessible_kb_ids:
            auth_conditions.append({"_id": {"$in": accessible_kb_ids}})
        
        # Combine with existing filter conditions
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