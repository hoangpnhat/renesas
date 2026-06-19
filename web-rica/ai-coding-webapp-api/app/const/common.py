from enum import Enum

UNKNOWN_RESOLVER = "unknown"
DBX_TIMEOUT_CODE = "TIME_EXCEED"


class CommonStatus(str, Enum):
    ACTIVE = "active"
    INACTIVE = "inactive"
    DRAFT = "draft"
    DELETED = "deleted"
    FAILED = "failed"
    PROCESSING = "processing"
    COMPLETED = "completed"


class AppAccessRoles(str, Enum):
    ADMIN = "admin"
    USER = "user"


class ShareType(str, Enum):
    PRIVATE = "private"
    PUBLIC = "public"


class OwnerType(str, Enum):
    USER = "user"
    GROUP = "group"


class ResourceType(str, Enum):
    FILE = "file"
    KNOWLEDGE = "knowledge"
    PROMPT = "prompt"
    GROUP = "group"
    RULE = "rule"


class SharingRoles(str, Enum):
    VIEWER = "viewer"
    OWNER = "owner"
    DEVELOPER = "developer"


PERMISSIONS = {
    SharingRoles.OWNER: {"read", "write", "delete", "transfer", "manage_permissions", "invite"},
    SharingRoles.DEVELOPER: {"read", "write"},
    SharingRoles.VIEWER: {"read"},
}


def can(role: SharingRoles, action: str) -> bool:
    return action in PERMISSIONS.get(role, set())


class CommonMode(str, Enum):
    LIGHT_MODE = "light_mode"
    NIGHT_MODE = "night_mode"


class CommonLang(str, Enum):
    ENG = "en"
    JAN = "ja"


class PromptType(str, Enum):
    RULE = "rule"
    PROMPT = "prompt"
