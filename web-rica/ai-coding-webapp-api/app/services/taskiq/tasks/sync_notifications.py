from typing import Annotated, Optional
from loguru import logger
from bson import ObjectId
from taskiq import Context, TaskiqDepends

from const.notification import (
    NotificationType,
    DEFAULT_GROUP_INVITATION,
    DEFAULT_GROUP_REMOVAL)
from const.common import ResourceType, OwnerType
from utils.helpers import normalize_id

# Lazy import to avoid circular dependency
def _get_notification_crud():
    from crud import crud_notification
    return crud_notification.notification_crud

notification_crud = None  # Will be initialized on first use

async def create_group_invitation_notifications(
        inviter_id: str,
        invitee_ids: list[str],
        group_id: str,
        group_name: str,
        message: str | None = None,
        inviter_name: str | None = None,
        task_id: str | None = None
) -> bool:

    try:

        final_message = message or (
            f"{inviter_name} invited you to join '{group_name}'" if inviter_name
            else DEFAULT_GROUP_INVITATION
        )

        await _get_notification_crud().create_notification(
            user_id=invitee_ids,
            notification_type=NotificationType.GROUP_INVITATION,
            title="Group Invitation",
            message=final_message,
            resource_type=ResourceType.GROUP,
            resource_id=group_id,
            actor_id=inviter_id,
            action_url=f"/groups/{group_id}/invitation",
            metadata={
                "group_name": group_name,
                "inviter_name": inviter_name,
            }
        )

        logger.info(f"[Task {task_id}] Created {len(invitee_ids)} invitations for group {group_id}")
        return True

    except Exception as e:
        logger.exception(f"[Task {task_id}] Failed to create group invitations: {e}")
        return False
    
async def create_group_removal_notifications(
        remover_id: str,
        removed_user_ids: list[str],
        group_id: str,
        group_name: str,
        message: str | None = None,
        remover_name: str | None = None,
        task_id: str | None = None
) -> bool:

    try:

        final_message = message or (
            f"You have been removed from '{group_name}'" if group_name
            else DEFAULT_GROUP_REMOVAL
        )

        await _get_notification_crud().create_notification(
            user_id=removed_user_ids,
            notification_type=NotificationType.GROUP_REMOVED,
            title="Removed from Group",
            message=final_message,
            resource_type=ResourceType.GROUP,
            resource_id=group_id,
            actor_id=remover_id,  # Optional: who removed them
            action_url=f"/groups",
            metadata={
                "group_name": group_name,
                "removed_by": remover_name
            }
        )

        logger.info(f"[Task {task_id}] Created {len(removed_user_ids)} removal notifications for group {group_id}")
        return True

    except Exception as e:
        logger.exception(f"[Task {task_id}] Failed to create group removal notifications: {e}")
        return False
    
async def sync_group_modify_members(
        from_whom: str,
        from_who: str,
        group_id: str,
        group_name: str,
        remove_whom: Optional[list[str]] = None,
        added_whom: Optional[list[str]] = None,
        context: Annotated[Context, TaskiqDepends()] | None = None
) -> bool:
    """
    Handle group member modifications: send invitations for added members or removal notifications for removed members.
    
    Args:
        from_whom: User ID performing the action (remover/inviter)
        from_who: Display name of the user performing the action
        group_id: ID of the group
        group_name: Name of the group
        remove_whom: List of user IDs to remove from group
        added_whom: List of user IDs to add to group
        context: TaskIQ context for task metadata
        
    Returns:
        bool: True if operation succeeded, False otherwise
    """
    try:
        task_id = context.message.task_id if context else None
        
        results = []
        
        # Send invitations for added members
        if added_whom:
            invite_result = await create_group_invitation_notifications(
                inviter_id=from_whom,
                invitee_ids=added_whom,
                group_id=group_id,
                group_name=group_name,
                inviter_name=from_who,
                task_id=task_id
            )
            results.append(invite_result)
        
        # Send removal notifications for removed members
        if remove_whom:
            removal_result = await create_group_removal_notifications(
                remover_id=from_whom,
                removed_user_ids=remove_whom,
                group_id=group_id,
                group_name=group_name,
                remover_name=from_who,
                task_id=task_id
            )
            results.append(removal_result)
        
        return all(results) if results else True
        
    except Exception as e:
        logger.exception(f"[Task {task_id}] Failed to sync group member modifications: {e}")
        return False


async def sync_modify_knowledge(
    modifier_id: str,
    modifier_name: str,
    knowledge_id: str,
    knowledge_name: str,
    detail: str = "File updated",
    context: Annotated[Context, TaskiqDepends()] | None = None
) -> bool:

    try:
        from crud.crud_share import share_crud
        from crud.crud_group import group_crud

        task_id = context.message.task_id if context else None

        # Get all shares for this knowledge base
        shares = await share_crud.get_knowledge_shares(knowledge_id)
                
        notified_users = set()
        
        for share in shares:
            if share.target_type == OwnerType.USER:
                # Direct user share
                notified_users.add(share.target_id)
                
            elif share.target_type == OwnerType.GROUP:
                # Group share
                group = await group_crud.get_by_id(share.target_id)
                if group:
                    for member in group.members:
                        if member.joined_at:  # Active member
                            notified_users.add(member.user_id)
        
        # Remove current user
        notified_users.discard(normalize_id(modifier_id))
        
        # Create notifications
        await _get_notification_crud().create_notification(
            user_id=notified_users,
            notification_type=NotificationType.KB_MODIFIED,
            title="Knowledge Base Updated",
            message=f"{modifier_name} updated '{knowledge_name}'",
            resource_type=ResourceType.KNOWLEDGE,
            resource_id=knowledge_id,
            actor_id=modifier_id,
            action_url=f"/knowledge/{knowledge_id}",
            metadata={
                "kb_name": knowledge_name,
                "actor_name": modifier_name,
                "change_type": "kb_modified",
                "details": detail
            }
        )
        return True
    except Exception as e:
        logger.exception(f"[Task {task_id}] Failed to modify knowledge files {knowledge_id}: {e}")
        return False
    
async def sync_share_knowledge(
    shared_by: str,
    shared_by_name: Optional[str],
    knowledge_id: str,
    knowledge_name: str,
    target_type: OwnerType,
    target_id: str,
    role: Optional[str] = None,
    context: Annotated[Context, TaskiqDepends()] | None = None
) -> bool:
    """Notify recipients when a knowledge base is shared to a single user or to a group.

    If the target is a user, notify that user. If the target is a group, notify all active members
    of the group.
    """
    try:
        task_id = context.message.task_id if context else None
        from crud.crud_group import group_crud

        recipients = set()

        if target_type == OwnerType.USER:
            recipients.add(target_id)
        elif target_type == OwnerType.GROUP:
            grp = await group_crud.get_by_id(target_id)
            if grp:
                for member in grp.members:
                    if member.joined_at:
                        recipients.add(str(member.user_id))

        # exclude the sharer
        recipients.discard(shared_by)

        if not recipients:
            logger.info(f"[Task {task_id}] No recipients for share {knowledge_id}; nothing to do")
            return True

        await _get_notification_crud().create_notification(
            user_id=list(recipients),
            notification_type=NotificationType.KB_SHARED,
            title="Knowledge Shared",
            message=(f"{shared_by_name} shared '{knowledge_name}' with you" if shared_by_name else f"'{knowledge_name}' was shared with you"),
            resource_type=ResourceType.KNOWLEDGE,
            resource_id=knowledge_id,
            actor_id=shared_by,
            action_url=f"/knowledge/{knowledge_id}",
            metadata={
                "kb_name": knowledge_name,
                "actor_name": shared_by_name,
                "role": role
            }
        )

        logger.info(f"[Task {task_id}] Created share notifications for {len(recipients)} recipients for knowledge {knowledge_id}")
        return True

    except Exception as e:
        logger.exception(f"[Task {task_id}] Failed to create share notifications for {knowledge_id}: {e}")
        return False


async def sync_action_completed(
    resource_id: str,
    user_id: str,
    context: Annotated[Context, TaskiqDepends()] | None = None
) -> bool:
    try:
        task_id = context.message.task_id if context else None

        await _get_notification_crud().update_metadata_flag(
            resource_id=resource_id,
            user_id=user_id,
            flag_key="action_completed",
            flag_value=True
        )

        logger.info(f"[Task {user_id}] Marked action_completed for resource {resource_id}, user {user_id}")
        return True

    except Exception as e:
        logger.exception(f"[Task {task_id}] Failed to mark action_completed for resource {resource_id}: {e}")
        return False
