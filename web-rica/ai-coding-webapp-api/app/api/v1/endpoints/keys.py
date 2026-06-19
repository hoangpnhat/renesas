from datetime import timedelta

from fastapi import APIRouter, Depends, HTTPException, status
from odmantic import ObjectId

from core.auth.token import generate_api_key
from core.auth.unified import unified_validator
from db.session import get_engine
from models.api_key import ApiKey
from schemas.api_key import ApiKeyCreate, ApiKeyCreatedResponse, ApiKeyResponse
from schemas.user import VerifiedUser
from utils.helpers import datetime_now_sec

MAX_KEYS_PER_USER = 10
MAX_EXPIRY_DAYS = 365

router = APIRouter()

_verified_user_dep = Depends(unified_validator(["admin", "user"]))


def _get_verified_user(verified_user: VerifiedUser = _verified_user_dep) -> VerifiedUser:
    return verified_user


@router.get("", response_model=list[ApiKeyResponse])
async def list_keys(
    verified_user: VerifiedUser = Depends(_get_verified_user),
):
    engine = get_engine()
    keys = await engine.find(ApiKey, ApiKey.user_id == verified_user.user_id)
    return [
        ApiKeyResponse(
            id=k.id,
            name=k.name,
            key_prefix=k.key_prefix,
            created_at=k.created_at,
            expires_at=k.expires_at,
            last_used_at=k.last_used_at,
            revoked=k.revoked,
        )
        for k in keys
    ]


@router.post("", response_model=ApiKeyCreatedResponse, status_code=status.HTTP_201_CREATED)
async def create_key(
    body: ApiKeyCreate,
    verified_user: VerifiedUser = Depends(_get_verified_user),
):
    engine = get_engine()
    now = datetime_now_sec()

    # Enforce max expiry
    max_expires = now + timedelta(days=MAX_EXPIRY_DAYS)
    if body.expires_at > max_expires:
        raise HTTPException(
            status_code=status.HTTP_422_UNPROCESSABLE_ENTITY,
            detail=f"expires_at must be within {MAX_EXPIRY_DAYS} days from now",
        )
    if body.expires_at <= now:
        raise HTTPException(
            status_code=status.HTTP_422_UNPROCESSABLE_ENTITY,
            detail="expires_at must be in the future",
        )

    # Enforce per-user limit (active keys only)
    active_count = len(
        await engine.find(
            ApiKey,
            (ApiKey.user_id == verified_user.user_id) & (ApiKey.revoked == False),  # noqa: E712
        )
    )
    if active_count >= MAX_KEYS_PER_USER:
        raise HTTPException(
            status_code=status.HTTP_422_UNPROCESSABLE_ENTITY,
            detail=f"Maximum of {MAX_KEYS_PER_USER} active API keys allowed per user",
        )

    plaintext, key_hash, key_prefix = generate_api_key()

    doc = ApiKey(
        user_id=verified_user.user_id,
        name=body.name,
        key_hash=key_hash,
        key_prefix=key_prefix,
        expires_at=body.expires_at,
    )
    await engine.save(doc)

    return ApiKeyCreatedResponse(
        id=doc.id,
        name=doc.name,
        key_prefix=doc.key_prefix,
        created_at=doc.created_at,
        expires_at=doc.expires_at,
        last_used_at=doc.last_used_at,
        revoked=doc.revoked,
        key=plaintext,
    )


@router.delete("/{key_id}", status_code=status.HTTP_204_NO_CONTENT)
async def revoke_key(
    key_id: ObjectId,
    verified_user: VerifiedUser = Depends(_get_verified_user),
):
    engine = get_engine()
    doc = await engine.find_one(
        ApiKey,
        (ApiKey.id == key_id) & (ApiKey.user_id == verified_user.user_id),
    )
    if doc is None:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Not found")

    doc.revoked = True
    await engine.save(doc)
