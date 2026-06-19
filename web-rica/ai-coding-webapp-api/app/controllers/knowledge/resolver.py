from typing import Any, cast
import asyncio
from odmantic import ObjectId
from crud.crud_share import CRUDShare
from models.knowledge import Knowledge as MongoKnowledge
from schemas.knowledge import KnowledgeResponse, FileJunctionResponse, KnowledgeShareRequest
from schemas.share import ShareCreate, ChildrenResource, MongoShare
from services.resolution.entity_resolver import EntityResolver, EntityType
from const.common import ResourceType

class KnowledgeResolver(EntityResolver):

    def get_fixed_name(self) -> str:
        return self.get_empty_name()

    async def _build_user_lookups(self, knowledge_lst: list[MongoKnowledge])-> dict[str, Any]:
        user_ids: list[ObjectId | str] = list({g.owner_id for g in knowledge_lst})
        users: list[Any] = await self.lookup_service.bulk_lookup(entity_type=EntityType.USER, ids=user_ids)
        user_lookup = {str(user.id): user.full_name for user in users} if users else {}
        return user_lookup
    
    async def _process_share_changes(
        self,
        knowledge_id,
        current_user_id,
        share_request_lst: list[KnowledgeShareRequest]
    ):
        """
        Compare share_request_lst with current knowledge shares.
        Add new shares, remove shares that are no longer in the request list.
        """
        share_crud = cast(CRUDShare, self.lookup_service.registry.get(EntityType.SHARE))

        # Get current shares
        current_shares = await self.get_knowledge_share(knowledge_id)

        # Create lookup for current shares using (target_id, target_type) as key
        current_share_map = {
            (str(share.target_id), share.target_type): share
            for share in current_shares
        }

        # Create lookup for requested shares
        requested_share_map = {
            (str(req.target_id), req.target_type): req
            for req in share_request_lst
        }

        shares_to_add = []
        shares_to_remove = []

        for key, share_request in requested_share_map.items():
            if key not in current_share_map:
                # New share
                shares_to_add.append(ShareCreate(                    shareable_id=knowledge_id,
                    shareable_type=ResourceType.KNOWLEDGE,
                    shared_by=current_user_id,
                    target_id=share_request.target_id,
                    target_type=share_request.target_type,
                    role=share_request.role
                ))
            else:
                # Existing share: check if role changed
                current_share = current_share_map[key]
                if current_share.role != share_request.role:
                    # Remove old share and add new one with updated role
                    shares_to_remove.append(current_share)
                    shares_to_add.append(ShareCreate(                        shareable_id=knowledge_id,
                        shareable_type=ResourceType.KNOWLEDGE,
                        shared_by=current_user_id,
                        target_id=share_request.target_id,
                        target_type=share_request.target_type,
                        role=share_request.role
                    ))

        # Identify shares to remove
        for key, share in current_share_map.items():
            if key not in requested_share_map:
                shares_to_remove.append(share)

        # remove first -> then add
        if shares_to_remove:
            remove_lst = [share.target_id for share in shares_to_remove]
            await share_crud._remove_multiple_share(remove_lst, knowledge_id)

        if shares_to_add:
            await share_crud._save_multiple_shares(shares_to_add)

        return {
            "added": len(shares_to_add),
            "removed": len(shares_to_remove)
        }

    async def _resolve_update_sharing(self, knowledge_id, current_user_id, share_request_lst: list[KnowledgeShareRequest]):
        # Prepare share create payload
        share_alls = [ShareCreate(            shareable_id=knowledge_id,
            shareable_type=ResourceType.KNOWLEDGE,
            shared_by=current_user_id,
            target_id=share_request.target_id,
            target_type=share_request.target_type,
            role=share_request.role
        ) for share_request in share_request_lst]

        # Create share record
        share_crud = cast(CRUDShare, self.lookup_service.registry.get(EntityType.SHARE))
        return await share_crud._save_multiple_shares(share_alls)
    
    async def _build_file_lookups(self, knowledge_lst: list[MongoKnowledge])-> dict[str, Any]:
        file_ids: list[ObjectId | str] = list({f.file_id for knowledge in knowledge_lst for f in knowledge.files})
        files: list[Any] = await self.lookup_service.bulk_lookup(entity_type=EntityType.FILE, ids=file_ids)
        file_lookup = {str(user.id): user.file_name for user in files} if files else {}
        return file_lookup
    
    async def _build_lookups(self, knowledge_lst: list[MongoKnowledge]) -> dict[str, Any]:
        user_lookup, file_lookup = await asyncio.gather(
            self._build_user_lookups(knowledge_lst),
            self._build_file_lookups(knowledge_lst)
        )
        return {
            "users": user_lookup,
            "files": file_lookup
        }
    
    async def resolve(
            self,
            knowledge: MongoKnowledge,
            lookups: dict[str, Any] | None,
            overrides: dict[str, Any] | None
    ) -> KnowledgeResponse:
        
        overrides = overrides or {}
        if lookups is None:
            lookups = await self._build_lookups([knowledge])

        user_lookup = lookups.get('users', {})
        file_lookup = lookups.get('files', {})

        owner_name = await self._resolve_entity(
                                            knowledge.owner_id,
                                            EntityType.USER,
                                            user_lookup,
                                            overrides,
                                            'full_name')
        # enrich file file_name
        enriched_files = []
        for file in knowledge.files or []:
            file_name = file_lookup.get(str(file.file_id))
            if not file_name:
                file_name = await self._resolve_entity(file.file_id, EntityType.FILE, file_lookup, overrides, 'file_name')
            enriched_files.append(FileJunctionResponse(
                **file.model_dump(),
                file_name = file_name
            ))

        return KnowledgeResponse(
            **knowledge.model_dump(exclude={'files'}),
            owner=owner_name,
            files=enriched_files
        )
    async def get_knowledge_share(self, knowledge_id) -> list:
        share_crud = cast(CRUDShare, self.lookup_service.registry.get(EntityType.SHARE))
        shares = await share_crud.get_knowledge_shares(knowledge_id)
        if not shares:
            return []
        return cast(list[Any], shares)
    
    async def _resole_recipients(self, shares: list[MongoShare]) -> dict[str, dict]:
        from const.common import OwnerType

        # Collect unique target ids by type
        user_ids = {str(s.target_id) for s in shares if s.target_type == OwnerType.USER}
        group_ids = {str(s.target_id) for s in shares if s.target_type == OwnerType.GROUP}

        # Bulk lookup
        users: list[Any] = await self.lookup_service.bulk_lookup(entity_type=EntityType.USER, ids=list(user_ids)) if user_ids else []
        groups: list[Any] = await self.lookup_service.bulk_lookup(entity_type=EntityType.GROUP, ids=list(group_ids)) if group_ids else []

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
            if not existing or (last_mod and (not existing.get('last_modified') or last_mod > existing.get('last_modified'))):
                resources_map[tid] = {
                    'resource_id': tid,
                    'resource_type': rtype,
                    'resource_name': name,
                    'last_modified': last_mod,
                    'role': s.role
                }
        return resources_map