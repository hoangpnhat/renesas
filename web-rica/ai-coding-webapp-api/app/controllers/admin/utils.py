from typing import List

from typing_extensions import TypedDict


class UserPermissionType(TypedDict):
    """
    User permission dictionary.
    """

    role: str
    department: str
    permission: str


def check_admin_permission(
        roles: List[str],
        required_permission: UserPermissionType,
) -> bool:
    """
    Check if the user has the required roles to access the admin endpoint.
    """
    for role in roles:
        parts = role.split("_")
        user_is_admin = parts[0] == "Admin"
        parts[1] if len(parts) > 1 else None
        user_permission = parts[2] if len(parts) > 2 else None

        is_satisfy_role = True
        if (
                required_permission["role"]
                and required_permission["role"] == "Admin"
                and not user_is_admin
        ):
            is_satisfy_role = False

        is_satisfy_department = True
        # if (
        #     required_permission["department"] and user_department
        #     and required_permission["department"] != "All"
        #     and user_department != required_permission["department"]
        # ):
        #     is_satisfy_department = False

        is_satisfy_permission = True
        if (
                required_permission["permission"]
                and user_permission
                and required_permission["permission"] != "All"
                and (
                "Read" in user_permission
                and "Write" in required_permission["permission"]
        )
        ):
            is_satisfy_permission = False

        if is_satisfy_role and is_satisfy_department and is_satisfy_permission:
            return True
    return False
