from odmantic import ObjectId
from typing import Union, Any, cast
import bson
from loguru import logger

from crud.crud_knowledge import knowledge_crud
from schemas.user import VerifiedUser
from models.knowledge import Knowledge as MongoKnowledge
from schemas.knowledge import (
    KnowledgeResponse,
    KnowledgeCreate,
    KnowledgeUpdateGeneral,
    KnowledgeUpdateFiles,
    KnowledgeShareRequest
)
from schemas.share import ShareCreate, ChildrenResource
from utils.decorators import cache_result, require_user_validator
from utils.handle_exception import ApplicationException
from crud.refactor_base.mixins import ComprehensiveRequest, CommonRequest, PaginationResponse
from schemas.share import ShareCreate
from services.taskiq.broker import broker

from .permission import KnowledgePermissionValidator
from .resolver import KnowledgeResolver


CACHED_PREFIX = "knowledge"
KNOWLEDGE_FILES_CACHE_PREFIX = "knowledge_files"

class KnowledgeController:

    def __init__(self,
                verified_user: VerifiedUser,
                validator: KnowledgePermissionValidator,
                resolver: KnowledgeResolver):
        self.crud = knowledge_crud
        self.verified_user = verified_user

        # supporting
        self.validator = validator
        self.resolver= resolver

    async def fetch_knowledge_by_id(self, knowledge_id: str | ObjectId, raise_error: bool=True) -> MongoKnowledge | None:
        knowledge = await self.crud.get_by_id(id=knowledge_id)
        if not knowledge and raise_error:
            raise ApplicationException('NOT_EXISTED_ERROR')
        else:
            return knowledge

    @require_user_validator(return_fields=['id'])
    async def create_knowledge(self,
                              knowledge_base: KnowledgeCreate,
                              **kwargs) -> KnowledgeResponse:
        """Create a new knowledge base."""
        current_user_id = kwargs.get('id')

        # Prepare knowledge base data with owner_id
        knowledge_base.owner_id = current_user_id

        # Create knowledge base
        kb = await self.crud.create(obj_in=MongoKnowledge(**knowledge_base.model_dump(exclude_unset=True)))

        return cast(KnowledgeResponse, await self.get_knowledge(element=kb, id=current_user_id))

    @cache_result(prefix=CACHED_PREFIX)
    async def get_knowledge_identity(self, 
                                    element: Union[ObjectId, MongoKnowledge, str], 
                                    **kwargs) -> KnowledgeResponse:
        """Get knowledge base with fixed identity information."""

        kb: MongoKnowledge | None
        if isinstance(element, (str, bson.ObjectId)):
            kb = cast(MongoKnowledge, await self.fetch_knowledge_by_id(knowledge_id=str(element)))
        else:
            kb = element

        return await self.resolver.resolve(
            knowledge=kb,
            overrides=kwargs.get("overrides"),
            lookups=kwargs.get("lookups")
        )

    @require_user_validator()
    async def get_knowledge(self,
                            element: Union[str, ObjectId],
                            **kwargs) -> KnowledgeResponse:
        """Get knowledge base with user-specific information."""

        user_id = kwargs.get('id')
        if user_id is None:
            raise ApplicationException("PERMISSION_DENIED")

        # get cached identities
        kb = await self.get_knowledge_identity(element=element, **kwargs)
        kb = KnowledgeResponse(**kb) if isinstance(kb, dict) else kb

        # user-dependent identity: enrich permissions
        permissions = await self.validator.enrich_permissions(kb, user_id)

        # Update kb with all permission fields
        for key, value in permissions.items():
            setattr(kb, key, value)

        return cast(KnowledgeResponse, kb)

    @require_user_validator()
    async def get_list_knowledge(self,
                                request: CommonRequest | ComprehensiveRequest,
                                query: str | None = None,
                                **kwargs) -> PaginationResponse:
        user_id = kwargs.get("id")

        # pagination execute
        results, _ = await self.crud.paginate(user_id=user_id, request=request, query = query)

        if results.data:
            lookups = await self.resolver._build_lookups(results.data)
            results.data = [await self.get_knowledge(element=kb, lookups = lookups) for kb in results.data]

        return results
    
    @require_user_validator(return_fields=['id'])
    async def delete_or_archived_knowledge(self,
                            knowledge_id: str | ObjectId,
                            is_archived: bool = False,
                            **kwargs):

        current_user_id = kwargs.get("id")
        if current_user_id is None:
            raise ApplicationException("PERMISSION_DENIED")

        target_knowledge = await self.fetch_knowledge_by_id(knowledge_id=knowledge_id)

        # validate delete capability:
        await self.validator.validate_can_delete(
            target_knowledge,
            current_user_id
        )

        # execute delete
        func_ = self.crud.soft_delete_or_archive
        updated_kb = await self.crud.transaction_wrapper(
                                        func_,
                                        knowledge=target_knowledge,
                                        is_archived=is_archived)

        # Move share revocation to background task (best-effort)
        try:
            await broker.send_task("sync_delete_knowledge_shares", str(updated_kb.id))
        except Exception:
            # best-effort: don't fail the operation if queuing the task fails
            logger.exception("Failed to enqueue share revocation for knowledge %s", str(knowledge_id))

        return updated_kb


    @require_user_validator(return_fields=['id'])
    async def share_knowledge(self,
                            knowledge_id: str | ObjectId,
                            share_request: list[KnowledgeShareRequest],
                            **kwargs):
        """Share this knowledge base to a single user or group """

        current_user_id = kwargs.get('id')
        if current_user_id is None:
            raise ApplicationException("PERMISSION_DENIED")

        # Fetch and validate permission to invite/share
        knowledge = cast(MongoKnowledge, await self.fetch_knowledge_by_id(knowledge_id=knowledge_id))
        await self.validator.validate_can_invite(knowledge, current_user_id)

        created_share = await self.resolver._process_share_changes(knowledge.id, current_user_id, share_request)

        # # notification sync
        # await broker.send_task(
        #     "sync_notifications",
        #     knowledge_id=str(knowledge_id)
        # )

        return created_share

    @require_user_validator(return_fields=['id', 'full_name'])
    async def set_knowledge_general(self,
                              knowledge_id: str | ObjectId,
                              update_data: KnowledgeUpdateGeneral,
                              **kwargs) -> KnowledgeResponse:
        """Update knowledge base general"""
        current_user_id = kwargs.get('id')
        if current_user_id is None:
            raise ApplicationException("PERMISSION_DENIED")
        current_user_name = kwargs.get('full_name')

        knowledge = await self.fetch_knowledge_by_id(knowledge_id=knowledge_id)

        # Validate permission: owner-only fields require owner role
        owner_only_fields = {"name", "description", "visibility"}
        valid_update_permission = await self.validator.validate_update_permission(
            knowledge, current_user_id, update_data, owner_only_fields
        )

        update_payload = (
            update_data.model_dump(exclude=owner_only_fields, exclude_unset=True)
            if not valid_update_permission
            else update_data
        )

        updated_kb: MongoKnowledge = await self.crud.transaction_wrapper(
            self.crud.update,
            db_obj=knowledge,
            obj_in=update_payload
        )
        await broker.send_task(
            "sync_modify_knowledge",
            modifier_id=str(current_user_id),
            modifier_name=current_user_name,
            knowledge_id=str(updated_kb.id),
            knowledge_name=updated_kb.name,
            detail="General settings updated"
        )
        return cast(KnowledgeResponse, await self.get_knowledge(element=updated_kb, id=current_user_id))
    
    async def get_knowledge_shares(self, knowledge_id: str) -> list[ChildrenResource]:
        """Retrieve all recipients of a knowledge base"""
        
        shares = await self.resolver.get_knowledge_share(knowledge_id)
        if not shares:
            return []
        resources_map = await self.resolver._resole_recipients(shares)
        return [ChildrenResource(**v) for v in resources_map.values()]
