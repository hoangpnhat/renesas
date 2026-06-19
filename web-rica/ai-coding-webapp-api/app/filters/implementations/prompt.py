from typing import Optional, Any

from loguru import logger
from odmantic import ObjectId

from const.common import CommonStatus, ShareType
from filters import BaseQueryBuilder, EmptyResultBehavior, FilterCondition, Operator, DynamicFilterRequest


class PromptDynamicQueryBuilder(BaseQueryBuilder):

    @property
    def searchable_fields(self) -> list[str]:
        return ['name', 'description']

    @property
    def soft_delete_field(self) -> str:
        return "status"

    async def _validate_condition(
            self,
            *,
            condition: FilterCondition
    ) -> Optional[FilterCondition]:
        valid_fields = {
            'name', 'description', 'visibility',
            'status', 'type', 'created_at', 'last_modified'
        }

        if condition.field not in valid_fields:
            logger.warning(f'Invalid field for prompt filtering: {condition.field}')
            return None

        if condition.field in ('created_at', 'last_modified'):
            valid_date_operators = (
                Operator.EQUALS, Operator.NOT_EQUALS,
                Operator.GREATER_THAN, Operator.GREATER_EQUAL,
                Operator.LESS_THAN, Operator.LESS_EQUAL
            )
            if condition.operator not in valid_date_operators:
                logger.warning(f'Invalid operator {condition.operator} for date field {condition.field}')
                return None

        if condition.field in ('visibility', 'status', 'type'):
            if condition.operator not in (Operator.EQUALS, Operator.NOT_EQUALS):
                logger.warning(f'Invalid operator {condition.operator} for enum field {condition.field}')
                return None

        return condition

    async def _handle_empty_filters(
            self,
            empty_behavior: EmptyResultBehavior,
            user_id: Optional[ObjectId] = None,
            **kwargs: Any,
    ) -> tuple[dict[str, Any], dict[str, Any]]:

        if empty_behavior == EmptyResultBehavior.NO_ROWS:
            return ({'_id': {'$exists': False}}, {})

        elif empty_behavior == EmptyResultBehavior.ALL_ROWS:
            assert user_id is not None
            user_context = await self._get_user_context(user_id)
            user_context.update({
                'prompt_type': kwargs.get('prompt_type'),
                'visibility': kwargs.get('visibility'),
            })
            q = await self._apply_authorization_constraints({}, user_context)
            return (q, {})

        return ({}, {})

    async def _get_user_context(self, user_id: ObjectId, is_archived: Optional[bool] = None) -> dict[str, Any]:
        from crud import group_crud, share_crud
        from const.common import OwnerType

        accessible_prompt_ids = []
        group_ids = []

        try:
            user_groups = await group_crud.get_user_groups(user_id)
            group_ids = [g.id for g in user_groups]

            if group_ids:
                prompt_shares = await share_crud.get_prompt_shares_for_groups(group_ids)
                accessible_prompt_ids.extend([s.shareable_id for s in prompt_shares])

            direct_shares = await share_crud.get_prompt_shares_for_targets(
                target_ids=[user_id],
                target_type=OwnerType.USER
            )
            accessible_prompt_ids.extend([s.shareable_id for s in direct_shares])
            accessible_prompt_ids = list(set(accessible_prompt_ids))

        except Exception as e:
            logger.error(f"Error getting user context for prompt access: {e}")

        return {
            'user_id': user_id,
            'group_ids': group_ids,
            'accessible_prompt_ids': accessible_prompt_ids
        }

    async def build_dynamic_query(
            self,
            user_id: Optional[ObjectId] = None,
            filters: Optional[DynamicFilterRequest] = None,
            search_text: Optional[str] = None,
            **kwargs: Any,
    ) -> tuple[dict[str, Any], dict[str, Any]]:
        if user_id is None:
            user_id = kwargs.get('user_id')
        prompt_type = kwargs.get('prompt_type')
        visibility = kwargs.get('visibility')

        try:
            filters = self.combine_search_text(filters, search_text)

            if not filters or not filters.filter_groups:
                empty_behavior = filters.empty_result if filters else EmptyResultBehavior.ALL_ROWS
                return await self._handle_empty_filters(
                    empty_behavior,
                    user_id=user_id,
                    prompt_type=prompt_type,
                    visibility=visibility
                )

            assert user_id is not None
            user_context = await self._get_user_context(user_id)
            user_context['prompt_type'] = prompt_type
            user_context['visibility'] = visibility

            group_conditions = []
            for group in filters.filter_groups:
                group_condition = await self._build_filter_group(group, user_context=user_context)
                if group_condition:
                    group_conditions.append(group_condition)

            if not group_conditions:
                return await self._handle_empty_filters(
                    filters.empty_result,
                    user_id=user_id,
                    prompt_type=prompt_type,
                    visibility=visibility
                )

            final_conditions = self._combine_conditions(group_conditions, filters.match_type)
            authorized = await self._apply_authorization_constraints(final_conditions, user_context)
            return (authorized, {})

        except Exception as e:
            logger.error(f'Failed to build prompt dynamic query: {e}')
            return await self._handle_empty_filters(EmptyResultBehavior.NO_ROWS, user_id=user_id)

    async def _apply_authorization_constraints(
            self,
            conditions: dict[str, Any],
            user_context: dict[str, Any],
            is_archived: Optional[bool] = None,
    ) -> dict[str, Any]:
        user_id = user_context['user_id']
        accessible_prompt_ids = user_context.get('accessible_prompt_ids', [])
        prompt_type = user_context.get('prompt_type')
        visibility = user_context.get('visibility')

        constraint_query = {"status": {"$ne": CommonStatus.DELETED.value}}

        if prompt_type:
            constraint_query["type"] = prompt_type.value

        auth_conditions: list[dict[str, Any]] = []
        if visibility == "private":
            auth_conditions = [{"owner_id": ObjectId(user_id)}]
        elif visibility == "public":
            auth_conditions = [{"visibility": ShareType.PUBLIC.value}]
        else:
            auth_conditions = [
                {"owner_id": ObjectId(user_id)},
                {"visibility": ShareType.PUBLIC.value},
            ]
            if accessible_prompt_ids:
                auth_conditions.append({"_id": {"$in": accessible_prompt_ids}})

        base = {"$and": [{"$or": auth_conditions}, constraint_query]}

        if conditions:
            return {"$and": [conditions, base]}

        return base
