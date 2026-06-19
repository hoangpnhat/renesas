#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

from fastapi import APIRouter, status, Depends

import controllers.extension
from const.common import AppAccessRoles
from core.auth.unified import unified_validator_with_api_key
from schemas.extension import ExtensionToken, ExtensionTokenRequestBody
from schemas.user import VerifiedUser

router = APIRouter()


@router.post("/token", response_model=ExtensionToken, status_code=status.HTTP_200_OK)
async def get_token_for_extension(
        body: ExtensionTokenRequestBody,
        current_user: VerifiedUser = Depends(unified_validator_with_api_key([AppAccessRoles.USER])),
):
    token_payload = await controllers.extension.get_service_principal_access_token(
        version=body.version
    )
    return ExtensionToken(
        token_type=token_payload["token_type"],
        access_token=token_payload["access_token"],
        expires_in=token_payload["expires_in"],
        ext_expires_in=token_payload["ext_expires_in"],
    )
