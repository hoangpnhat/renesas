#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

from fastapi import APIRouter, status

from dependencies import UserModelControllerDep
from schemas.model_management import RICAConfigurationResponse
from services.validator.token_consumption import PostConfigsRequest

router = APIRouter()


@router.post("/configs", status_code=status.HTTP_200_OK, response_model=RICAConfigurationResponse)
async def post_rica_configurations(
        request: PostConfigsRequest,
        controller: UserModelControllerDep,
):
    """
    POST endpoint to record token consumption and return model configurations.

    **Breaking Change**: This endpoint was converted from GET to POST to accept consumption data.

    **Deduplication**: Consumption records are deduplicated based on (user_id, model_id, client_timestamp).
    Duplicate records are silently skipped without failing the request. The client_timestamp field is required
    and must be provided as milliseconds since Unix epoch (integer).

    Args:
        request: Request body containing optional consumption records with client_timestamp
        controller: Injected controller with verified user context

    Returns:
        RICAConfigurationResponse with model configurations and consumption limits
    """
    return await controller.post_configurations(request)
