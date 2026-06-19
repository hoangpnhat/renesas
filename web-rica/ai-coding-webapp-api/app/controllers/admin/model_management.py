import asyncio
from datetime import timedelta
from typing import Optional, cast

from fastapi import HTTPException, status
from odmantic import query

from core.loguru import logger
from const.enums import RoleEnum
from core.config import settings
from crud.crud_model_management import model_crud
from exceptions.database_exception import NotExistedRecord
from models.model_management import ModelManagement
from schemas import NameValuePair
from schemas.model_management import (
    ModelManagementCreate,
    ModelManagementUpdate,
    RICAConfigurationResponse,
    Model,
    DefaultCompletionOptions,
    ModelManagementResponse,
    ModelManagementResponseBody,
    ConsumptionLimit
)
from services.DatabricksEndpoint import DatabricksEndpoint
from crud.crud_token_consumption import token_consumption_crud
from schemas.user import VerifiedUser
from utils.helpers import normalize_id
from utils.string_handle import capitalize_first_letter


class ModelManagementController:
    def __init__(self, verified_user: VerifiedUser | None = None):
        self.verified_user = verified_user
        self.model_crud = model_crud
        self.dbx_service = DatabricksEndpoint(base_url=settings.EXT_DATABRICKS_URL,
                                              custom_token=settings.EXT_DATABRICKS_TOKEN)

    async def get_models_list_from_dbx(self) -> list[dict[str, str]]:
        try:
            model_list = await self.dbx_service.get_models()
            return [{"name": model.name, "value": model.name} for model in (model_list.endpoints or []) if
                    getattr(model.state, 'ready', None) == "READY" and model.name]
        except Exception as e:
            raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST,
                                detail=f"Error fetching models from Databricks: {str(e)}")

    async def get_models_configuration(self, user_id: Optional[str] = None) -> RICAConfigurationResponse:
        """
        Get models configuration with optional consumption limit tracking.

        Args:
            user_id: Optional user ID for consumption limit calculation

        Returns:
            RICAConfigurationResponse with model configs and consumption limits
        """
        try:
            default_response = RICAConfigurationResponse()
            models = await self.model_crud.get_multi()

            model_responses = []
            for model in models:
                # Skip deleted models
                if model.deleted_at is not None:
                    continue
                # Build base model response
                model_response = Model(
                    name=model.name,
                    id=model.id,
                    model=f"{settings.EXT_MODEL_PREFIX}{model.model_path}",
                    roles=model.roles,
                    context_length=model.default_completion_options.context_length if model.default_completion_options else None,
                    default_completion_options=DefaultCompletionOptions(
                        **model.default_completion_options.model_dump()) if model.default_completion_options else None
                )

                # Add consumption limit if configured and user_id provided
                if user_id and model.threshold is not None and model.consumption_range_hours is not None:
                    try:
                        consumed = await token_consumption_crud.aggregate_consumption(
                            user_id=user_id,
                            model_id=model.id,
                            range_hours=model.consumption_range_hours
                        )

                        remaining = model.threshold - consumed
                        exceeded = consumed >= model.threshold

                        is_blocked, blocked_until = await token_consumption_crud.check_if_blocked(
                            user_id=user_id,
                            model_id=model.id
                        )

                        if exceeded and not is_blocked:
                            try:
                                flag_record = await token_consumption_crud.create_flag_record(
                                    user_id=user_id,
                                    model_id=model.id
                                )
                                is_blocked = True
                                blocked_until = flag_record.timestamp + timedelta(hours=2)
                            except Exception as flag_error:
                                logger.error(
                                    f"Failed to create flag record for user {user_id}, model {model.id}: {str(flag_error)}"
                                )

                        # Construct ConsumptionLimit object with blocking info
                        model_response.consumption_limit = ConsumptionLimit(
                            enabled=True,
                            threshold=model.threshold,
                            range_hours=model.consumption_range_hours,
                            consumed=consumed,
                            remaining=remaining,
                            blocked=is_blocked,
                            blocked_until=int(blocked_until.timestamp()) if blocked_until else None
                        )
                    except Exception as agg_error:
                        # Log aggregation errors but continue (best-effort)
                        logger.error(
                            f"Failed to aggregate consumption for model {model.id}, user {user_id}: {str(agg_error)}"
                        )
                        # Leave consumption_limit as None

                model_responses.append(model_response)

            default_response.config.models += model_responses
            return default_response
        except Exception as e:
            logger.error(f"Error generating models configuration: {str(e)}")
            raise

    async def get_list_of_models(self, page: Optional[int] = None,
                                 entries: Optional[int] = None) -> ModelManagementResponseBody:
        rsult, total = await asyncio.gather(
            self.model_crud.get_multi(page=page, entries=entries),
            self.model_crud.count()
        )
        return ModelManagementResponseBody(data=[ModelManagementResponse(**model.model_dump()) for model in rsult
                                                 ], page=page, entries=entries, total=total)

    async def get_model_by_id(self, model_id: str) -> ModelManagement | None:
        try:
            return cast(ModelManagement | None, await self.model_crud.get(id=normalize_id(model_id)))
        except NotExistedRecord:
            logger.error(f"Model with id {model_id} does not exist")
            return None
        except Exception as e:
            logger.error(f"Error retrieving model with id {model_id}: {str(e)}")
            return None

    async def create_model(self, model_data: ModelManagementCreate) -> ModelManagementResponse:
        created_data = await self.model_crud.create(obj_in=model_data)
        return ModelManagementResponse(**created_data.model_dump())

    async def update_model(self, model_id: str, model_data: ModelManagementUpdate) -> ModelManagement | None:
        try:
            existing_model = await self.model_crud.get(id=normalize_id(model_id))
            if not existing_model:
                logger.error(f"Model with id {model_id} does not exist")
                return None

            # Handle explicit null for consumption fields
            # When both threshold and consumption_range_hours are explicitly set to None,
            # we need to clear them (not just skip updating them)
            update_dict = model_data.model_dump(exclude_unset=True)

            # Check if both consumption fields are explicitly None in the request
            if "threshold" in update_dict and update_dict["threshold"] is None:
                existing_model.threshold = None
            if "consumption_range_hours" in update_dict and update_dict["consumption_range_hours"] is None:
                existing_model.consumption_range_hours = None

            return cast(ModelManagement, await self.model_crud.update(db_obj=existing_model, obj_in=model_data))
        except NotExistedRecord:
            logger.error(f"Model with id {model_id} does not exist")
            return None
        except Exception as e:
            logger.error(f"Error updating model with id {model_id}: {str(e)}")
            return None

    async def delete_model(self, model_id: str) -> None:
        try:
            model_oid = normalize_id(model_id)
            existing_model = await self.model_crud.find_with_query(id=model_oid, raise_error=True,
                                                                   to_query=query.and_(
                                                                       query.eq(ModelManagement.id, model_oid),
                                                                       query.eq(ModelManagement.deleted_at, None)))

            await self.model_crud.delete(item=existing_model)

        except NotExistedRecord:
            logger.error(f"Model with id {model_id} does not exist")
            raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Model not found")

        except Exception as e:
            logger.error(f"Error deleting model with id {model_id}: {str(e)}")
            raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail=f"Error deleting model: {str(e)}")

    def get_model_roles(self) -> list[NameValuePair]:
        try:
            return [NameValuePair(name=capitalize_first_letter(role.value), value=role.value) for role in RoleEnum]
        except Exception as e:
            logger.error(f"Error retrieving model roles: {str(e)}")
            raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST,
                                detail="Error retrieving model roles")

    def get_model(self, model_id: str):
        try:
            model_oid = normalize_id(model_id)
            return self.model_crud.find_with_query(
                to_query=query.and_(query.eq(ModelManagement.id, model_oid), query.eq(ModelManagement.deleted_at, None)),
                id=model_oid, raise_error=True)
        except NotExistedRecord:
            logger.error(f"Model with id {model_id} does not exist")
            raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Model not found")
        except Exception as e:
            logger.error(f"Error retrieving model with id {model_id}: {str(e)}")
            raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST,
                                detail="Error retrieving model")
