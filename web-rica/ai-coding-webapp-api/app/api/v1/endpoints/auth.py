from fastapi import APIRouter, Depends, HTTPException, status

from fastapi.security import OAuth2PasswordRequestForm
from core import security
import schemas
import models
import crud

router = APIRouter()


@router.post("/oauth", response_model=schemas.Token)
async def auth_verify(
    form_data: OAuth2PasswordRequestForm = Depends(),
):
    # TODO: move to controller
    user: models.User = await crud.user.authenticate(
        email=form_data.username, password=form_data.password
    )

    if not form_data.password or not user or not crud.user.is_active(user):
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Login failed; incorrect email or password",
        )
    refresh_token = None
    force_totp = True

    if not user.totp_secret:
        force_totp = False
        refresh_token = security.create_refresh_token(subject=user.id)
        await crud.token.create(obj_in=refresh_token, user_obj=user)
    return {
        "access_token": security.create_access_token(
            subject=user.id, force_totp=force_totp
        ),
        "refresh_token": refresh_token,
        "token_type": "bearer",
    }
