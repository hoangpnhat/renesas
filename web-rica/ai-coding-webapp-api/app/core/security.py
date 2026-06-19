from datetime import datetime, timedelta
from passlib.context import CryptContext
from passlib.totp import TOTP
from passlib.exc import TokenError, MalformedTokenError
from core.config import settings
from jose import jwt
from typing import Union, Any, cast

pwd_context = CryptContext(schemes=["argon2", "bcrypt"], deprecated="auto")


def verify_password(*, plain_password: str, hashed_password: str) -> bool:
    return bool(pwd_context.verify(plain_password, hashed_password))


def create_refresh_token(
    *, subject: Union[str, Any], expires_delta: timedelta | None = None
) -> str:
    if expires_delta:
        expire = datetime.now() + expires_delta
    else:
        expire = datetime.now() + timedelta(
            seconds=settings.REFRESH_TOKEN_EXPIRE_SECONDS
        )
    to_encode = {"exp": expire, "sub": str(subject), "refresh": True}
    encoded_jwt = jwt.encode(
        to_encode, settings.SECRET_KEY, algorithm=settings.JWT_ALGO
    )
    return cast(str, encoded_jwt)


def create_access_token(
    *,
    subject: Union[str, Any],
    expires_delta: timedelta | None = None,
    force_totp: bool = False
) -> str:
    if expires_delta:
        expire = datetime.now() + expires_delta
    else:
        expire = datetime.now() + timedelta(
            seconds=settings.ACCESS_TOKEN_EXPIRE_SECONDS
        )
    to_encode = {"exp": expire, "sub": str(subject), "totp": force_totp}
    encoded_jwt = jwt.encode(
        to_encode, settings.SECRET_KEY, algorithm=settings.JWT_ALGO
    )
    return cast(str, encoded_jwt)


def get_password_hash(password: str) -> str:
    return cast(str, pwd_context.hash(password))


if __name__ == "__main__":
    passwd = "HaiLa123"
