from abc import ABC, abstractmethod
from typing import Generic, TypeVar, Any, Optional

from utils.handle_exception import ApplicationException
from crud.crud_share import share_crud, MongoShare
from const.common import SharingRoles, can

T = TypeVar('T')

class BaseValidator(ABC, Generic[T]):
    def __init__(self, crud: Any = None):
        self.crud = crud or share_crud
    
    @abstractmethod
    async def validate_ownership(self, item: T, user_id: str) -> T:
        pass

    # Role/permission advanced check
    async def get_share_record(self, item: Any, user_id: str) -> Optional[Any]:
        getter = getattr(self.crud, "get_share_for_user", None)
        if getter:
            try:
                return await getter(user_id, item.id)
            except Exception:
                return None
        return None

    async def get_user_role(self, item: Any, user_id: str) -> Optional[SharingRoles]:
        if hasattr(item, "owner_id") and str(getattr(item, "owner_id")) == str(user_id):
            return SharingRoles.OWNER

        share: MongoShare | None = await self.get_share_record(item, user_id)
        if share and hasattr(share, "role"):
            return share.role

        return None

    async def ensure_has_permission(self, item: Any, user_id: str, action: str) -> bool:
        role = await self.get_user_role(item, user_id)
        if not role or not can(role, action):
            raise ApplicationException("PERMISSION_DENIED", detail=f"User does not have permission to perform {action} action.")
        return True
    
    async def can_perform(self, item: Any, user_id: str, action: str) -> bool:
        role = await self.get_user_role(item, user_id)
        return bool(role and can(role, action))

    async def validate_can_delete(self, item: Any, user_id: str) -> bool:
        return await self.ensure_has_permission(item, user_id, "delete")

    async def validate_can_edit(self, item: Any, user_id: str) -> bool:
        return await self.ensure_has_permission(item, user_id, "write")

    async def validate_can_invite(self, item: Any, user_id: str) -> bool:
        return await self.ensure_has_permission(item, user_id, "invite")

    async def validate_can_transfer(self, item: Any, user_id: str) -> bool:
        return await self.ensure_has_permission(item, user_id, "transfer")

    async def validate_update_permission(
        self,
        item: Any,
        user_id: str,
        update_data: Any,
        owner_only_fields: set[str]
    ) -> bool:

        role = await self.get_user_role(item, user_id)
        if not role:
            raise ApplicationException("PERMISSION_DENIED", detail="User has no access to this resource.")

        # Check if any owner-only field is being updated
        updated_fields = set(update_data.model_dump(exclude_unset=True).keys())
        requires_owner = bool(updated_fields & owner_only_fields)

        if requires_owner:
            if role != SharingRoles.OWNER:
                return False
        else:
            if not can(role, "write"):
                raise ApplicationException("PERMISSION_DENIED", detail="User does not have write permission.")
        return True

    def is_archived(self, item: Any) -> bool:
        if hasattr(item, "is_archived") and getattr(item, "is_archived") == True:
            return True
        return False

    async def enrich_permissions(self, item: Any, user_id: str) -> dict:
        role = await self.get_user_role(item, user_id)

        # Determine if user is the owner
        is_owner = role == SharingRoles.OWNER if role else False

        # Determine specific permissions based on role
        can_view = bool(role and can(role, "read"))
        can_add_files = bool(role and can(role, "write"))
        can_remove_files = bool(role and can(role, "write"))
        can_edit = bool(role and can(role, "write"))
        can_delete = bool(role and can(role, "delete"))
        can_share = bool(role and can(role, "invite"))
        can_archive = bool(role and can(role, "delete"))

        return {
            "current_user_role": role.value if role else None,
            "is_owner": is_owner,
            "can_view": can_view,
            "can_add_files": can_add_files,
            "can_remove_files": can_remove_files,
            "can_edit": can_edit,
            "can_delete": can_delete,
            "can_share": can_share,
            "can_archive": can_archive,
        }