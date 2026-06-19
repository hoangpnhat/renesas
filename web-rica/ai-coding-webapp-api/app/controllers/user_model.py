from const.token_consumption import TOKEN_OUTPUT_COST_MULTIPLIER
from controllers.admin.model_management import ModelManagementController
from core.loguru import logger
from crud.crud_token_consumption import token_consumption_crud
from odmantic import ObjectId
from schemas.model_management import RICAConfigurationResponse
from schemas.token_consumption import TokenConsumptionCreate
from schemas.user import VerifiedUser
from services.validator.token_consumption import PostConfigsRequest, validate_model_ids
from utils.handle_exception import ApplicationException
from utils.helpers import datetime_now_sec


class UserModelController:
    def __init__(self, verified_user: VerifiedUser | None = None):
        self.verified_user = verified_user
        self._model_controller = ModelManagementController()

    async def post_configurations(self, request: PostConfigsRequest) -> RICAConfigurationResponse:
        if self.verified_user is None:
            raise ApplicationException("PERMISSION_DENIED")
        if self.verified_user.email is None:
            raise ApplicationException("PERMISSION_DENIED")
        if request.consumption:
            valid_records, invalid_ids = await validate_model_ids(request.consumption)

            if invalid_ids:
                logger.warning(
                    f"Skipping invalid model_ids for user {self.verified_user.email}: {', '.join(invalid_ids)}"
                )

            if valid_records:
                try:
                    server_timestamp = datetime_now_sec()
                    consumption_records = [
                        TokenConsumptionCreate(
                            user_id=self.verified_user.email,
                            model_id=ObjectId(record.model_id),
                            tokens_in=record.tokens_in,
                            tokens_out=record.tokens_out,
                            token_count=TOKEN_OUTPUT_COST_MULTIPLIER * record.tokens_out + record.tokens_in,
                            client_timestamp=record.client_timestamp,
                            timestamp=server_timestamp,
                            created_at=server_timestamp,
                        )
                        for record in valid_records
                    ]
                    await token_consumption_crud.bulk_create(consumption_records)

                except Exception as e:
                    logger.error(f"Failed to record consumption for user {self.verified_user.email}: {str(e)}")

        return await self._model_controller.get_models_configuration(user_id=self.verified_user.email)
