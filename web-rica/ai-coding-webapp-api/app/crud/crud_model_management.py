from typing import Type

from crud import CRUDBase, ModelType
from models.model_management import ModelManagement
from schemas.model_management import ModelManagementCreate, ModelManagementUpdate


class CRUDModelManagement(CRUDBase[ModelManagement, ModelManagementCreate, ModelManagementUpdate]):
    def __init__(self, model: Type[ModelType]):
        super().__init__(model)

    async def get_with_consumption_config(self) -> list[ModelManagement]:
        models = await self.engine.find(
            ModelManagement,
            (ModelManagement.threshold != None)
            & (ModelManagement.consumption_range_hours != None)
            & (ModelManagement.deleted_at == None)
        )
        return list(models)


# Singleton instance
model_crud = CRUDModelManagement(ModelManagement)
