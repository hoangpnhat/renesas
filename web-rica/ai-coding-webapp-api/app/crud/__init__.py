from .registry import CrudRegistry, EntityType, ModelObject, CrudClass

def __getattr__(name):
    """Lazy import to avoid circular dependencies."""
    if name == "user_crud":
        from .crud_user import user as user_crud
        return user_crud
    elif name == "file_crud":
        from .crud_file import file_crud
        return file_crud
    elif name == "group_crud":
        from .crud_group import group_crud
        return group_crud
    elif name == "notification_crud":
        from .crud_notification import notification_crud
        return notification_crud
    elif name == "knowledge_crud":
        from .crud_knowledge import knowledge_crud
        return knowledge_crud
    elif name == "share_crud":
        from .crud_share import share_crud
        return share_crud
    elif name == "chat":
        from .crud_chat import chat
        return chat
    elif name == "message_content":
        from .crud_chat import message_content
        return message_content
    elif name == "saved_chat_content":
        from .crud_chat import saved_chat_content
        return saved_chat_content
    elif name == "CRUDBase":
        from .base import CRUDBase
        return CRUDBase
    elif name == "ModelType":
        from .base import ModelType
        return ModelType
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")

__all__ = [
    'CrudRegistry',
    'EntityType',
    'ModelObject',
    'CrudClass',
    'user_crud',
    'file_crud',
    'group_crud',
    'notification_crud',
    'knowledge_crud',
    'share_crud',
    'chat',
    'message_content',
    'saved_chat_content',
    'CRUDBase',
    'ModelType',
]