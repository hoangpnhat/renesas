from typing import Any

import jwt
from fastapi_azure_auth.auth import SingleTenantAzureAuthorizationCodeBearer
from fastapi_azure_auth.utils import get_unverified_header, get_unverified_claims, is_guest

from core.config import settings
from core.loguru import logger
from utils.handle_exception import ApplicationException


class AzureADAuthorization(SingleTenantAzureAuthorizationCodeBearer):
    def __init__(self, tenant_id: str, app_client_id: str, auto_error: bool = True):
        super().__init__(
            tenant_id=tenant_id,
            app_client_id=app_client_id,
            scopes={f'api://{app_client_id}/user_impersonation': 'user_impersonation'},
            auto_error=auto_error,
        )

    async def start(self):
        await self.openid_config.load_config()
        logger.debug('AzureAD service initialized')

    async def stop(self):
        logger.debug('AzureAD service shut down')
        pass

    async def verify_token(self, access_token: str) -> tuple[dict[str, Any], bool]:
        """
        Verify and decode the provided JWT token.
        :param access_token: The JWT access token.
        :return: Decoded token and user_is_guest
        """
        try:
            # Load new config if old
            await self.openid_config.load_config()

            # header
            header: dict[str, Any] = get_unverified_header(access_token)
            claims: dict[str, Any] = get_unverified_claims(access_token)

            # Validate token
            decoded_token = jwt.decode(
                access_token,
                key=self.openid_config.signing_keys.get(header.get('kid', '')) or '',
                algorithms=['RS256'],
                audience=self.app_client_id,
                issuer=self.openid_config.issuer,
                options={"verify_exp": True, "verify_aud": True}
            )
            user_is_guest: bool = is_guest(claims=claims)
            return decoded_token, user_is_guest
        except Exception:
            raise ApplicationException("INVALID_USER", user=f"invalid access_token")


authorize = AzureADAuthorization(tenant_id=settings.azure.AAD_TENANT_ID or '',
                                 app_client_id=settings.API_CLIENT_ID)

azure_scheme_allow_unauthenticated = AzureADAuthorization(
    tenant_id=settings.azure.AAD_TENANT_ID or '',
    app_client_id=settings.API_CLIENT_ID,
    auto_error=False,
)
