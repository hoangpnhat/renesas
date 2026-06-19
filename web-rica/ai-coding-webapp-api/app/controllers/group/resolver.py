from typing import Any, Union
from loguru import logger
from odmantic import ObjectId

from models.group import Group as MongoGroup
from schemas.group import GroupResponse
from services.resolution.entity_resolver import EntityResolver, EntityType
from services.taskiq.broker import broker


class GroupResolver(EntityResolver):

    def get_fixed_name(self) -> str:
        return self.get_empty_name()
    
    async def _build_lookups(self, groups: list[MongoGroup], group_knowledge_ids: dict[str, list] | None = None) -> dict[str, Any]:
        # include owners and members
        all_user_ids: list[ObjectId | str] = [
            u for u in
            ({g.owner_id for g in groups} | {m.user_id for g in groups for m in (g.members or [])})
            if u is not None
        ]
        users: list[Any] = await self.lookup_service.bulk_lookup(entity_type=EntityType.USER, ids=all_user_ids)
        user_lookup = {str(user.id): user.full_name for user in users} if users else {}

        # Build knowledge lookup if
        knowledge_lookup = {}
        if group_knowledge_ids:
            knowledge_lookup = await self._build_knowledge_lookups(group_knowledge_ids)

        return {
            'users': user_lookup,
            'knowledge': knowledge_lookup
        }

    async def _build_knowledge_lookups(self, group_knowledge_ids: dict[str, list]) -> dict[str, dict]:

        all_knowledge_ids = set()
        for knowledge_ids in group_knowledge_ids.values():
            all_knowledge_ids.update(knowledge_ids)

        if not all_knowledge_ids:
            return {}

        # Bulk fetch all knowledge entities
        knowledge_ids_list: list[ObjectId | str] = list(all_knowledge_ids)
        knowledge_lst: list[Any] = await self.lookup_service.bulk_lookup(entity_type=EntityType.KNOWLEDGE, ids=knowledge_ids_list)

        knowledge_lookup = {k.id: {
                        'resource_id': str(k.id),
                        'resource_type': 'knowledge',
                        'resource_name': k.name,
                        'last_modified': k.last_modified
                    } for k in knowledge_lst}

        return knowledge_lookup
    
    async def resolve(self, group: MongoGroup, lookups: dict[str, dict] | None = None, overrides: dict | None = None, group_knowledge_ids: dict[str, list] | None = None) -> GroupResponse:
        from schemas.group import ChildrenResource
        overrides = overrides or {}

        if lookups is None:
            lookups = await self._build_lookups([group], group_knowledge_ids)

        user_lookup = lookups.get('users', {})
        owner_name = await self._resolve_entity(
                                            group.owner_id,
                                            EntityType.USER,
                                            user_lookup,
                                            overrides,
                                            'full_name')
        # enrich members full_name
        enriched_members = []
        for member in group.members or []:
            full_name = user_lookup.get(str(member.user_id))
            if not full_name:
                full_name = await self._resolve_entity(member.user_id, EntityType.USER, user_lookup, overrides, 'full_name')
            enriched_members.append({
                **member.model_dump(exclude_none=False),
                'user_id': str(member.user_id),
                'full_name': full_name
            })

        # Enrich resources from knowledge lookup
        resources = []
        knowledge_lookup = lookups.get('knowledge', {})
        if group_knowledge_ids and knowledge_lookup:
            group_id_str = str(group.id)
            knowledge_ids = group_knowledge_ids.get(group_id_str, [])
            for kb_id in knowledge_ids:
                kb_data = knowledge_lookup.get(kb_id)
                if kb_data:
                    resources.append(ChildrenResource(**kb_data))

        group_data = group.model_dump()
        group_data['members'] = enriched_members

        return GroupResponse(
                        **group_data,
                        owner_name=owner_name,
                        resources=resources)
    
    async def invalidate_group_notification(self, resource_id, user_id):
        await broker.send_task(
            "sync_action_completed",
            resource_id=resource_id,
            user_id=user_id
        )