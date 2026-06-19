from typing import Any, cast

from odmantic import ObjectId
from const.common import ResourceType
from crud import EntityType
from crud.crud_share import CRUDShare
from models.prompt import Prompt as MongoPrompt
from schemas.prompt import PromptResponse, PromptShareRequest
from schemas.share import ShareCreate, MongoShare
from services.resolution.entity_resolver import EntityResolver


class PromptResolver(EntityResolver):

    def get_fixed_name(self) -> str:
        return self.get_empty_name()

    async def _build_user_lookups(
            self,
            prompt_lst: list[MongoPrompt | PromptResponse]
    ) -> dict[str, Any]:
        user_ids: list[ObjectId | str] = list({p.owner_id for p in prompt_lst if p.owner_id is not None})
        users: list[Any] = await self.lookup_service.bulk_lookup(
            entity_type=EntityType.USER, ids=user_ids
        )
        return {str(u.id): u.full_name for u in users} if users else {}

    async def _build_lookups(
            self,
            prompt_lst: list[MongoPrompt | PromptResponse]
    ) -> dict[str, Any]:
        user_lookup = await self._build_user_lookups(prompt_lst)
        return {"users": user_lookup}

    async def _process_share_changes(
            self,
            prompt_id,
            current_user_id,
            share_request_list: list[PromptShareRequest],
            shareable_type: ResourceType = ResourceType.PROMPT
    ):
        """
        Compare share_request_list with current prompt shares.
        Add new shares, remove shares that are no longer in the request list.
        """
        share_crud = cast(CRUDShare, self.lookup_service.registry.get(EntityType.SHARE))

        # Get current shares
        current_shares = await self.get_prompt_share(prompt_id, shareable_type=shareable_type)

        # Create lookup for current shares using (target_id, target_type) as key
        current_share_map = {
            (str(share.target_id), share.target_type): share
            for share in current_shares
        }

        # Create lookup for requested shares
        requested_share_map = {
            (str(req.target_id), req.target_type): req
            for req in share_request_list
        }

        shares_to_add = []
        shares_to_remove = []

        for key, share_request in requested_share_map.items():
            if key not in current_share_map:
                # New share
                shares_to_add.append(ShareCreate(
                    shareable_id=prompt_id,
                    shareable_type=shareable_type,
                    shared_by=current_user_id,
                    target_id=ObjectId(share_request.target_id),
                    target_type=share_request.target_type,
                    role=share_request.role
                ))
            else:
                # Existing share: check if role changed
                current_share = current_share_map[key]
                if current_share.role != share_request.role:
                    # Remove old share and add new one with updated role
                    shares_to_remove.append(current_share)
                    shares_to_add.append(ShareCreate(
                        shareable_id=prompt_id,
                        shareable_type=shareable_type,
                        shared_by=current_user_id,
                        target_id=ObjectId(share_request.target_id),
                        target_type=share_request.target_type,
                        role=share_request.role
                    ))

        # Identify shares to remove
        for key, share in current_share_map.items():
            if key not in requested_share_map:
                shares_to_remove.append(share)

        # remove first -> then add
        if shares_to_remove:
            remove_list = [share.target_id for share in shares_to_remove]
            await share_crud._remove_multiple_share(remove_list, prompt_id, shareable_type=shareable_type)

        if shares_to_add:
            await share_crud._save_multiple_shares(shares_to_add)

        return {
            "added": len(shares_to_add),
            "removed": len(shares_to_remove)
        }

    async def resolve(
            self,
            prompt: MongoPrompt,
            lookups: dict[str, Any] | None,
            overrides: dict[str, Any]
    ) -> PromptResponse:
        overrides = overrides or {}
        if lookups is None:
            lookups = await self._build_lookups([prompt])

        user_lookup = lookups.get("users", {})

        # Resolve owner name
        owner_name = await self._resolve_entity(
            prompt.owner_id,
            EntityType.USER,
            user_lookup,
            overrides,
            "full_name"
        )

        return PromptResponse(
            **prompt.model_dump(exclude={"owner_id", 'id'}),
            id=str(prompt.id),
            owner_id=str(prompt.owner_id),
            owner_name=owner_name
        )

    async def get_prompt_share(self, prompt_id, shareable_type: ResourceType = ResourceType.PROMPT) -> list:
        share_crud = cast(CRUDShare, self.lookup_service.registry.get(EntityType.SHARE))
        shares = await share_crud.get_shares_by_type(prompt_id, shareable_type)
        if not shares:
            return []
        return cast(list[Any], shares)

    async def _resolve_recipients(self, shares: list[MongoShare]) -> dict[str, dict]:
        from const.common import OwnerType

        # Collect unique target ids by type
        user_ids = {str(s.target_id) for s in shares if s.target_type == OwnerType.USER}
        group_ids = {str(s.target_id) for s in shares if s.target_type == OwnerType.GROUP}

        # Bulk lookup
        users: list[Any] = await self.lookup_service.bulk_lookup(entity_type=EntityType.USER,
                                                      ids=list(user_ids)) if user_ids else []
        groups: list[Any] = await self.lookup_service.bulk_lookup(entity_type=EntityType.GROUP,
                                                       ids=list(group_ids)) if group_ids else []

        user_lookup = {str(u.id): u.full_name for u in users} if users else {}
        group_lookup = {str(g.id): g.name for g in groups} if groups else {}

        resources_map: dict[str, dict] = {}
        for s in shares:
            tid = str(s.target_id)
            ttype = s.target_type

            if ttype == OwnerType.USER:
                name = user_lookup.get(tid)
                if not name:
                    name = await self._resolve_entity(s.target_id, EntityType.USER, user_lookup, {}, 'full_name')
                rtype = 'user'
            else:
                name = group_lookup.get(tid)
                if not name:
                    name = await self._resolve_entity(s.target_id, EntityType.GROUP, group_lookup, {}, 'name')
                rtype = 'group'

            last_mod = getattr(s, 'last_modified', None)

            existing = resources_map.get(tid)
            if not existing or (
                    last_mod and (not existing.get('last_modified') or last_mod > existing.get('last_modified'))):
                resources_map[tid] = {
                    'resource_id': tid,
                    'resource_type': rtype,
                    'resource_name': name,
                    'last_modified': last_mod,
                    'role': s.role
                }
        return resources_map
