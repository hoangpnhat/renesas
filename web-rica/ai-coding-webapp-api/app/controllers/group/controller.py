from odmantic.bson import ObjectId
from typing import Union, Any, Optional, cast
from loguru import logger

from const.common import SharingRoles, ShareType
from crud.crud_group import group_crud
from schemas.user import VerifiedUser
from models.group import Group as MongoGroup, Members
from schemas.group import (
    GroupResponse,
    GroupCreate,
    GroupUpdateGeneral,
    GroupUpdateMember,
    GroupUpdateAdvanced
)
from utils.decorators import cache_result, require_user_validator
from utils.handle_exception import ApplicationException
from crud.refactor_base.mixins import ComprehensiveRequest, CommonRequest, PaginationResponse
from utils.helpers import datetime_now_sec, normalize_id

from .permission import GroupPermissionValidator
from .resolver import GroupResolver

CACHED_PREFIX = "group"

class GroupController:

    def __init__(self,
                verified_user: VerifiedUser,
                validator: GroupPermissionValidator,
                resolver: GroupResolver):
        self.crud = group_crud
        self.verified_user = verified_user

        # supporting
        self.validator = validator
        self.resolver= resolver

    async def fetch_group_by_id(self, group_id: str | ObjectId, raise_error: bool=True) -> MongoGroup | None:
        group = await self.crud.get_by_id(id=group_id)
        if not group and raise_error:
            raise ApplicationException('NOT_EXISTED_ERROR')
        else:
            return group

    @require_user_validator(return_fields=['id', 'full_name'])
    async def create_group(self,
                        group: GroupCreate,
                        **kwargs) -> GroupResponse:
        curr_user_id = kwargs.get("id")
        curr_user_name = kwargs.get("full_name")
        if curr_user_id is None:
            raise ApplicationException("PERMISSION_DENIED")
        if curr_user_name is None:
            raise ApplicationException("PERMISSION_DENIED")
        # Members
        members = [
            Members(
                user_id=normalize_id(mem.user_id),
                added_by=curr_user_id,
                role=mem.role or SharingRoles.VIEWER
            ) for mem in (group.members or [])
        ]
        # create group base
        newly_group = MongoGroup(  # type: ignore[call-arg]
            owner_id=normalize_id(curr_user_id),
            name=group.name,
            description=group.description,
            visibility=group.visibility or ShareType.PRIVATE,
            members=members
        )

        async def __inner(newly_group: MongoGroup, session: Any):
            newly_created = await self.crud.create(obj_in=newly_group, session=session)
            return GroupResponse(
                **newly_created.model_dump(),
                owner_name=curr_user_name
            )
        
        newly_created_response = await self.crud.transaction_wrapper(
            __inner,
            newly_group=newly_group)

        # Send task
        await GroupTaskController().queue_group_member_modifications(
            modifier_id=curr_user_id,
            modifier_name=curr_user_name,
            added_member_ids=[str(m.user_id) for m in members],
            group_id=newly_created_response.id,
            group_name=newly_group.name,
        )
        return cast(GroupResponse, newly_created_response)


    @cache_result(prefix=CACHED_PREFIX)
    async def get_group_identity(self,
                            element: Union[ObjectId, MongoGroup, str],
                            **kwargs) -> GroupResponse:
        """Get group base with fixed identity information."""

        mg: MongoGroup | None
        if isinstance(element, (str, ObjectId)):
            mg = cast(MongoGroup, await self.fetch_group_by_id(group_id=str(element)))
        else:
            mg = element

        # Extract group_knowledge_ids from metadata if available
        metadata = kwargs.get("metadata", {})
        group_knowledge_ids = metadata.get("group_knowledge_ids") if metadata else None

        group_response = await self.resolver.resolve(
            group=mg,
            overrides=kwargs.get("overrides"),
            lookups=kwargs.get("lookups"),
            group_knowledge_ids=group_knowledge_ids
        )

        return group_response

    @require_user_validator()
    async def get_group(self, group: Union[str, ObjectId, MongoGroup], **kwargs) -> GroupResponse:
        """Get group base with user-specific information."""

        user_id = kwargs.get('id')
        if user_id is None:
            raise ApplicationException("PERMISSION_DENIED")

        # get cached identities (passing through metadata for resource enrichment)
        resolved_group = await self.get_group_identity(element=group, **kwargs)
        if isinstance(resolved_group, dict):
            resolved_group = GroupResponse(**resolved_group)

        # enrich permission fields
        return self.validator.enrich_permission_fields(group=resolved_group, user_id=user_id)
                                    
    @require_user_validator()
    async def get_list_group(self,
                            request: CommonRequest | ComprehensiveRequest,
                            is_archived: bool,
                            **kwargs) -> PaginationResponse:
        user_id = kwargs.get("id")

        # pagination execute
        results, metadata = await self.crud.paginate(user_id=user_id, request=request, is_archived=is_archived)

        if results.data:
            # Build lookups with knowledge data
            group_knowledge_ids = metadata.get('group_knowledge_ids', {})
            lookups = await self.resolver._build_lookups(groups=results.data, group_knowledge_ids=group_knowledge_ids)
            # Pass metadata and lookups to get_group
            results.data = [await self.get_group(group=gr, lookups=lookups, metadata=metadata, **kwargs) for gr in results.data]

        return results
    
    @require_user_validator()
    async def delete_or_archive_group(self,
                        group_id: str | ObjectId,
                        is_archived: bool = True,
                        **kwargs):
        
        current_user_id = kwargs.get("id")
        target_group = await self.fetch_group_by_id(group_id=group_id)

        # validate some ownership capability:
        self.validator.validate_ownership(
            target_group,
            current_user_id
        )

        # all passed, hide it
        func_ = self.crud.soft_delete_or_archive
        await self.crud.transaction_wrapper(
            func_,
            group=target_group,
            is_archived=is_archived)
        
    @require_user_validator()
    async def restore_group(self,
                        group_id: str | ObjectId,
                        **kwargs):
        
        current_user_id = kwargs.get("id")
        target_group = await self.fetch_group_by_id(group_id=group_id)

        # validate some ownership capability:
        self.validator.validate_ownership(
            target_group,
            current_user_id
        )

        # all passed, hide it
        func_ = self.crud.update
        await self.crud.transaction_wrapper(
            func_,
            db_obj=target_group,
            obj_in=GroupUpdateAdvanced(is_archived=False))

    @require_user_validator(return_fields=['id'])
    async def accept_invitation(self, group_id: str | ObjectId, **kwargs):

        curr_user_id = kwargs.get('id')
        group = cast(MongoGroup, await self.fetch_group_by_id(group_id=group_id))
        if self.validator.is_archived(group):
            raise ApplicationException("INACTIVE_OBJECT")

        # find member
        member = next((m for m in group.members if str(m.user_id) == str(curr_user_id)), None)
        if not member:
            raise ApplicationException('NOT_EXISTED_ERROR', obj=f"Member-{curr_user_id}")
        
        # already joined
        if member.joined_at:
            return
        
        # accept joined
        member.joined_at = datetime_now_sec()
        await self.crud.transaction_wrapper(
            self.crud.update,
            db_obj=group,
            obj_in={"members": group.members}
        )

        # invalidate the notification
        await self.resolver.invalidate_group_notification(str(group.id), str(curr_user_id))

    @require_user_validator(return_fields=['id'])
    async def decline_invitation(self, group_id: str | ObjectId, **kwargs):

        curr_user_id = kwargs.get('id')
        group = cast(MongoGroup, await self.fetch_group_by_id(group_id=group_id))

        # ensure member exists
        if not any(str(m.user_id) == str(curr_user_id) for m in group.members):
            raise ApplicationException('NOT_EXISTED_ERROR', obj=f"Member-{curr_user_id}")

        # hard delete
        group.members = [m for m in group.members if str(m.user_id) != str(curr_user_id)]

        await self.crud.transaction_wrapper(
            self.crud.update,
            db_obj=group,
            obj_in={"members": group.members}
        )
        # invalidate the notification
        await self.resolver.invalidate_group_notification(str(group.id), str(curr_user_id))

    @require_user_validator(return_fields=['id'])
    async def set_group_general(self,
                              group_id: str | ObjectId,
                              update_data: GroupUpdateGeneral,
                              **kwargs) -> GroupResponse:
        
        current_user_id = kwargs.get('id')

        # Validate ownership and archived status
        target_group = await self.fetch_group_by_id(group_id=group_id)
        if self.validator.is_archived(target_group):
            raise ApplicationException("INACTIVE_OBJECT")
        self.validator.validate_ownership(target_group, current_user_id)

        obj_in = update_data.model_dump(exclude_unset=True)
        func_ = self.crud.update
        group = await self.crud.transaction_wrapper(
            func_,
            db_obj = target_group,
            obj_in = obj_in
        )
        return cast(GroupResponse, await self.get_group(group))

    @require_user_validator(return_fields=['id', 'full_name'])
    async def update_group_members(self, group_id: str | ObjectId, members: GroupUpdateMember, **kwargs):

        user_id = kwargs.get('id')
        if user_id is None:
            raise ApplicationException("PERMISSION_DENIED")
        user_name = kwargs.get('full_name')
        if user_name is None:
            raise ApplicationException("PERMISSION_DENIED")
        group = cast(MongoGroup, await self.fetch_group_by_id(group_id=group_id))
        if self.validator.is_archived(group):
            raise ApplicationException("INACTIVE_OBJECT")

        # check invitation permission
        if not self.validator.check_critical_action_capability(group, user_id):
            raise ApplicationException("PERMISSION_DENIED",
                                       detail="User does not have permission to update members.")

        task_supporter = GroupTaskController()

        # determine added/removed ids
        new_members = members.members or []
        added_member_ids, removed_member_ids = task_supporter.check_modify_user(set(group.members or []), members)

        # Override existing members
        updated_members = [
            Members(
                user_id=normalize_id(mem.user_id),
                added_by=user_id,
                role=mem.role or SharingRoles.VIEWER
            ) for mem in new_members
        ]

        # Update
        await self.crud.transaction_wrapper(
            self.crud.update,
            db_obj=group,
            obj_in={"members": updated_members}
        )

        # Queue task
        if added_member_ids or removed_member_ids:
            await task_supporter.queue_group_member_modifications(
                modifier_id=user_id,
                modifier_name=user_name,
                group_id=group_id,
                group_name=group.name,
                added_member_ids=list(added_member_ids) if added_member_ids else None,
                removed_member_ids=list(removed_member_ids) if removed_member_ids else None,
            )

        return await self.get_group(group=group, **kwargs)
    
class GroupTaskController:

    @staticmethod
    def check_modify_user(current_members: set[Members], new_members: GroupUpdateMember) -> tuple[set, set]:
        # Identify added and removed members
        current_user_ids = {str(m.user_id) for m in current_members}
        new_user_ids = {str(m.user_id) for m in (new_members.members or [])}

        added_member_ids = new_user_ids - current_user_ids
        removed_member_ids = current_user_ids - new_user_ids
        return added_member_ids, removed_member_ids

    @staticmethod
    async def queue_group_member_modifications(
        modifier_id: str | object,
        modifier_name: str,
        group_id: str | object,
        group_name: str,
        added_member_ids: list[str] | None = None,
        removed_member_ids: list[str] | None = None,
    ) -> bool:
        from services.taskiq.broker import broker

        # Convert ids to string cause redis broker does not accept ObjectId
        modifier_s = str(modifier_id) if modifier_id is not None else None
        added_ids = [str(i) for i in (added_member_ids or [])] if added_member_ids else None
        removed_ids = [str(i) for i in (removed_member_ids or [])] if removed_member_ids else None

        if not added_ids and not removed_ids:
            logger.debug("No member modifications to queue for group %s", group_id)
            return False

        await broker.send_task(
            "sync_group_modify_members",
            from_whom=modifier_s,
            from_who=modifier_name,
            group_id=str(group_id),
            group_name=group_name,
            added_whom=added_ids,
            remove_whom=removed_ids,
        )
        logger.info("Enqueued group member modification task for group %s (added=%d, removed=%d)", 
                    group_id, len(added_ids or []), len(removed_ids or []))
        return True

