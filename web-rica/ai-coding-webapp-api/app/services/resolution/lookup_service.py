from typing import Any, cast
from odmantic import ObjectId
from loguru import logger
from crud import CrudRegistry, EntityType
from utils.handle_exception import ApplicationException

class LookupService:
    def __init__(self, crud_registry: CrudRegistry):
        self.registry = crud_registry

    async def bulk_lookup(
        self,
        entity_type: EntityType,
        ids: list[ObjectId | str] | None = None,
        retrieve_fields: list[str] | None = None) -> list[Any]:
        """Generic bulk lookup"""

        if not ids:
            return []

        crud: Any = self.registry.get(entity_type)
        if not crud:
            logger.warning(f"No CRUD registered for entity type: {entity_type}")
            return []

        try:
            return cast(list[Any], await crud.bulk_lookup(ids))
        except ApplicationException as e:
            logger.error(f"Error in bulk lookup for {entity_type}: {e}")
            return []
    
    async def single_lookup(
            self,
            entity_id: ObjectId | str,
            entity_type: EntityType) -> Any:
        crud: Any = self.registry.get(entity_type)
        if not crud:
            logger.warning(f"No CRUD registered for entity type: {entity_type}")
            return {}

        try:
            return await crud.get_by_id(entity_id)
        except ApplicationException as e:
            logger.error(f"Error in single lookup for {entity_type}: {e}")
            return {}

    async def single_lookup_with_strategy(
            self,
            entity_id: ObjectId | str,
            entity_type: EntityType,
            strategy_name: str
    ) -> Any:
        crud: Any = self.registry.get(entity_type)
        if not crud:
            logger.warning(f"No CRUD registered for entity type: {entity_type}")
            return {}

        try:
            strategy = getattr(crud, strategy_name)

            if not strategy:
                logger.warning(f"No strategy registered for entity type: {entity_type}")
                return {}

            return await strategy(entity_id)
        except ApplicationException as e:
            logger.error(f"Error in single lookup for {entity_type}: {e}")
            return {}