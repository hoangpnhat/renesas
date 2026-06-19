from services.validator import BasePermissionValidator
from schemas.group import GroupResponse
from odmantic.bson import ObjectId

from models.group import Group as MongoGroup
from utils.handle_exception import ApplicationException
from const.common import SharingRoles

class GroupPermissionValidator(BasePermissionValidator):

    async def validate_ownership(self, item, user_id):
        if not self.check_critical_action_capability(item, user_id):
            raise ApplicationException("PERMISSION_DENIED",
                                detail="User does not have permission to perform this action.")

    def check_critical_action_capability(self, group: MongoGroup | GroupResponse, user_id: ObjectId | str) -> bool:
        member_role = next((member.role for member in group.members if member.user_id == str(user_id)), None)
        if member_role == SharingRoles.OWNER:
            return True
        return str(group.owner_id) == str(user_id)
    
    def enrich_permission_fields(self, group: GroupResponse, user_id: ObjectId | str) -> GroupResponse:
        """Enrich GroupResponse with user-specific permission fields."""
        user_id_str = str(user_id)
        
        # Determine user's role and ownership status
        member_role = next((m.role for m in (group.members or []) if str(m.user_id) == user_id_str), None)
        is_owner = group.owner_id == user_id_str
        is_admin = is_owner or member_role == SharingRoles.OWNER
        
        # Set permission fields based on role
        group.is_owner = is_owner
        group.is_admin = is_admin
        group.can_invite = is_admin
        group.can_remove_members = is_admin
        group.can_edit = is_admin
        group.can_delete = is_owner
        group.can_archive = is_admin
        group.can_unarchive = is_admin
        group.can_leave = not is_owner
        group.can_upload_files = is_admin or member_role in [SharingRoles.DEVELOPER, SharingRoles.OWNER]
        group.current_user_role = member_role or (SharingRoles.OWNER if is_owner else None)
        
        return group