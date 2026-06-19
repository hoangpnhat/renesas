from enum import Enum

class NotificationType(str, Enum):
    GROUP_INVITATION = "group_invitation"
    GROUP_REMOVED = "group_removing"
    FILE_ADDED = "file_added"
    FILE_MODIFIED = "file_modified"
    FILE_DELETED = "file_deleted"
    KB_ADDED = "kb_added"
    KB_MODIFIED = "kb_modified"
    KB_SHARED = "kb_shared"
    KB_DELETED = "kb_deleted"

class NotificationStatus(str, Enum):
    UNREAD = "unread"
    READ = "read"

DEFAULT_GROUP_INVITATION = "You’ve been invited to join a group."
DEFAULT_GROUP_REMOVAL = "You've been removed from a group"