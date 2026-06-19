from enum import Enum
from typing import Any, Optional, TypeVar

ModelObject = TypeVar('ModelObject')
CrudClass = TypeVar('CrudClass')


class EntityType(str, Enum):
    KNOWLEDGE = 'knowledge'
    USER = 'user'
    FILE = 'file'
    DEFAULT = "default"
    GROUP = "group"
    PROMPT = "prompt"
    NOTIFICATION = "notification"
    SHARE = "share"
    TOKEN_CONSUMPTION = "token_consumption"


class CrudRegistry:

    def __init__(self, crud_mapping: Optional[dict[str, CrudClass]] = None):
        self._registry: dict[str, Any] = {}
        if crud_mapping:
            self._initialize_registry(crud_mapping)
        else:
            # Lazy initialization - only import and register when needed
            self._initialize_default_registry()

    def _initialize_registry(self, crud_mapping: dict[str, CrudClass]) -> None:
        for entity_name, crud_class in crud_mapping.items():
            self._registry[entity_name] = crud_class

    def _initialize_default_registry(self) -> None:
        from .crud_user import user as user_crud
        from .crud_file import file_crud
        from .crud_group import group_crud
        from .crud_notification import notification_crud
        from .crud_knowledge import knowledge_crud
        from .crud_prompt import crud_prompt
        from .crud_share import share_crud
        from .crud_token_consumption import token_consumption_crud

        self._registry = {
            EntityType.USER.value: user_crud,
            EntityType.FILE.value: file_crud,
            EntityType.GROUP.value: group_crud,
            EntityType.NOTIFICATION.value: notification_crud,
            EntityType.KNOWLEDGE.value: knowledge_crud,
            EntityType.PROMPT.value: crud_prompt,
            EntityType.SHARE.value: share_crud,
            EntityType.TOKEN_CONSUMPTION.value: token_consumption_crud
        }

    def get(self, entity_name: str) -> Optional[CrudClass]:
        return self._registry.get(entity_name)

    def register(self, entity_name: str, crud_instance: CrudClass) -> None:
        self._registry[entity_name] = crud_instance
