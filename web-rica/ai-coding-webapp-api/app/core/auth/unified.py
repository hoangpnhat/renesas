from typing import Any, Callable, Coroutine, Optional

from fastapi import HTTPException, Request, status, Security, Depends
from fastapi.security import APIKeyHeader
from fastapi_azure_auth.user import User

from core.loguru import logger
from schemas.user import VerifiedUser
from utils.handle_exception import ApplicationException
from .azure import azure_scheme_allow_unauthenticated, authorize
from .token import token_strategy

x_api_key_scheme = APIKeyHeader(
    name="X-API-Key",
    auto_error=False,
    description="API key for CLI authentication (format: rc_xxxxx)"
)


def _validate_roles(user_roles: list[str], allowed_roles: list[str]) -> None:
    if not any(role.lower() in [r.lower() for r in allowed_roles] for role in user_roles):
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Insufficient permissions"
        )


async def _handle_api_key_auth(raw_key: str, allowed_roles: list[str]) -> VerifiedUser:
    verified_user, _ = await token_strategy.authenticate(raw_key)
    if verified_user.roles is None:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Invalid credentials",
            headers={"WWW-Authenticate": "Bearer"},
        )
    _validate_roles(verified_user.roles, allowed_roles)
    return verified_user


async def _handle_azure_user(azure_user: User) -> VerifiedUser:
    from controllers import UserController

    verified_user = VerifiedUser(
        email=azure_user.preferred_username,
        roles=azure_user.roles,
        name=azure_user.name,
    )
    try:
        controller = UserController(verified_user=verified_user)
        db_user = await controller.login_with_cache()
    except ApplicationException as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=str(e),
        )
    verified_user.user_id = db_user.id
    return verified_user


async def _jwt_authenticate(
    request: Request,
    azure_user: Optional[User],
    allowed_roles: list[str],
) -> VerifiedUser:
    if azure_user:
        return await _handle_azure_user(azure_user)

    authorization = request.headers.get("Authorization", "")
    if authorization.lower().startswith("bearer "):
        bearer_value = authorization[7:]
        if not bearer_value.startswith("rc_"):
            logger.warning(
                "Bearer JWT present but lenient validation returned None — retrying with strict validation"
            )
            from fastapi.security import SecurityScopes
            strict_user = await authorize(request=request, security_scopes=SecurityScopes())
            if strict_user:
                return await _handle_azure_user(strict_user)

    raise HTTPException(
        status_code=status.HTTP_401_UNAUTHORIZED,
        detail="Invalid or missing credentials. Provide an Authorization Bearer token.",
        headers={"WWW-Authenticate": "Bearer"},
    )


def unified_validator(allowed_roles: list[str]) -> Callable[..., Coroutine[Any, Any, VerifiedUser]]:
    """JWT-only authentication (Azure AD). Use for all endpoints except /extension/token."""

    async def authenticate(
            request: Request,
            azure_user: Optional[User] = Depends(azure_scheme_allow_unauthenticated),
    ) -> VerifiedUser:
        return await _jwt_authenticate(request, azure_user, allowed_roles)

    return authenticate


def unified_validator_with_api_key(allowed_roles: list[str]) -> Callable[..., Coroutine[Any, Any, VerifiedUser]]:
    """JWT + X-API-Key authentication. Use only for /extension/token (CLI access)."""

    async def authenticate(
            request: Request,
            x_api_key: Optional[str] = Security(x_api_key_scheme),
            azure_user: Optional[User] = Depends(azure_scheme_allow_unauthenticated),
    ) -> VerifiedUser:
        if x_api_key and x_api_key.startswith("rc_"):
            return await _handle_api_key_auth(x_api_key, allowed_roles)
        return await _jwt_authenticate(request, azure_user, allowed_roles)

    return authenticate
