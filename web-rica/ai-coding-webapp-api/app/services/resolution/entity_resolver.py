from typing import Any, Optional, Callable
from abc import abstractmethod, ABC

from core.loguru import logger
from const.http_messages import DEFAULT_OBJECT_NAME, EMPTY_OBJECT_NAME

from .lookup_service import LookupService, ObjectId, EntityType

class EntityResolver(ABC):
    
    def __init__(self, 
                 lookup_service: LookupService):
        self.lookup_service = lookup_service
    
    @abstractmethod
    def get_fixed_name(self) -> str:
        pass

    def get_default_name(self) -> str:
        return DEFAULT_OBJECT_NAME

    def get_empty_name(self) -> str:
        return EMPTY_OBJECT_NAME
    
    def _get_display_name(
        self, 
        obj: Any,
        display_field: str, 
        transform_func: Optional[Callable] = None,
        **kwargs
    ) -> str:
        """Extract display name with nested field support"""
        try:
            value = obj
            # Handle nested fields (e.g., 'user.display_name')
            for field_part in display_field.split('.'):
                value = getattr(value, field_part)
            
            # Apply transformation if provided
            if transform_func:
                value = transform_func(value)
            
            return str(value) if value is not None else ""
            
        except (AttributeError, TypeError) as e:
            logger.debug(f"Display name extraction failed: {e}")
            return self.get_fixed_name(**kwargs)
        
    async def _resolve_entity(self,
                              entity_id: ObjectId | str,
                              entity_type: EntityType,
                              lookup: dict[str, Any],
                              overrides: dict[str, Any],
                              retrieve_field: str,
                              **kwargs):
        
        # Check if overrides already exists (e.g author_name)
        if name := overrides.get(entity_type):
            return name
        
        # Check if there is existing lookup dict
        if name := lookup.get(str(entity_id)):
            return name
        
        # If not entity_id, it might be system default or deactivated entity
        if not entity_id:
            return self.get_fixed_name(**kwargs)
        
        # Manually retrieve entity
        entity_data: Any = await self.lookup_service.single_lookup(entity_id, entity_type)
        if entity_data:
            return getattr(entity_data, retrieve_field)
        return self.get_fixed_name(**kwargs)
        


            
