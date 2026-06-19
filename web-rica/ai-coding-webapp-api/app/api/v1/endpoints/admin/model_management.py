from fastapi import APIRouter, Query, status, Body, Path

from const.utils import DEFAULT_PAGE_NUMBER, DEFAULT_NUMBER_OF_ITEM_PAGE
from dependencies import ModelManagementControllerDep
from schemas import NameValuePair
from schemas.model_management import ModelManagementResponse, ModelManagementCreate, ModelManagementUpdate, \
    RICAConfigurationResponse, ModelManagementResponseBody

router = APIRouter()


@router.get("/model-databricks", status_code=200)
async def get_databricks_models(controller: ModelManagementControllerDep):
    return await controller.get_models_list_from_dbx()


@router.get("/configs", status_code=200, response_model=RICAConfigurationResponse)
async def get_rica_configurations(controller: ModelManagementControllerDep):
    return await controller.get_models_configuration()


@router.get("/model-roles", status_code=status.HTTP_200_OK, response_model=list[NameValuePair])
def get_model_roles(controller: ModelManagementControllerDep):
    return controller.get_model_roles()


@router.get("/{model_id}", status_code=status.HTTP_200_OK, response_model=ModelManagementResponse)
async def get_model_by_id(
        controller: ModelManagementControllerDep,
        model_id: str = Path(..., description="ID of the model to retrieve"),
):
    return await controller.get_model(model_id=model_id)


@router.get("", status_code=200, response_model=ModelManagementResponseBody)
async def get_all_models(
        controller: ModelManagementControllerDep,
        page: int = Query(
            title="Page query for page break loading",
            default=DEFAULT_PAGE_NUMBER,
            ge=0,
            le=100,
        ),
        entries: int = Query(
            title="Number of Chat for each page",
            default=DEFAULT_NUMBER_OF_ITEM_PAGE,
            gt=0,
            le=100,
        ),
):
    return await controller.get_list_of_models(page=page, entries=entries)


@router.post("", status_code=status.HTTP_201_CREATED, response_model=ModelManagementResponse)
async def create_model(
        controller: ModelManagementControllerDep,
        model_in: ModelManagementCreate = Body(..., description="Model info to create"),
):
    return await controller.create_model(model_data=model_in)


@router.patch("/{model_id}", status_code=status.HTTP_200_OK, response_model=ModelManagementResponse)
async def update_model(
        controller: ModelManagementControllerDep,
        model_id: str = Path(..., description="ID of the model to update"),
        model_in: ModelManagementUpdate = Body(..., description="Updated model info"),
):
    return await controller.update_model(model_id=model_id, model_data=model_in)


@router.delete("/{model_id}", status_code=status.HTTP_204_NO_CONTENT)
async def delete_model(
        controller: ModelManagementControllerDep,
        model_id: str = Path(..., description="ID of the model to delete"),
):
    await controller.delete_model(model_id=model_id)
