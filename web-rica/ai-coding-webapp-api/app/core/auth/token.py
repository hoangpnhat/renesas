import asyncio
import hashlib
import secrets

from fastapi import HTTPException, status

from models.api_key import ApiKey
from models.user import User as MongoUser
from schemas.user import VerifiedUser
from utils.helpers import datetime_now_sec


def generate_api_key() -> tuple[str, str, str]:
    """Return (plaintext_key, key_hash, key_prefix)."""
    raw = secrets.token_urlsafe(32)
    plaintext = f"rc_{raw}"
    key_hash = hashlib.sha256(plaintext.encode()).hexdigest()
    key_prefix = plaintext[:8]
    return plaintext, key_hash, key_prefix


def hash_key(plaintext: str) -> str:
    return hashlib.sha256(plaintext.encode()).hexdigest()


class TokenStrategy:

    async def authenticate(self, access_token: str) -> tuple[VerifiedUser, MongoUser | None]:
        """
        Validate API key (rc_... prefix) via SHA-256 hash lookup in MongoDB.

        Returns (VerifiedUser, MongoUser) on success.
        Raises HTTP 401 on any failure — never reveals why.
        """
        from db.session import get_engine

        # Strip "Bearer " prefix if present
        key = access_token.removeprefix("Bearer ").strip()

        if not key.startswith("rc_"):
            raise HTTPException(
                status_code=status.HTTP_401_UNAUTHORIZED,
                detail="Invalid credentials",
                headers={"WWW-Authenticate": "Bearer"},
            )

        key_hash = hash_key(key)
        engine = get_engine()

        api_key_doc = await engine.find_one(ApiKey, ApiKey.key_hash == key_hash)

        if api_key_doc is None or api_key_doc.revoked:
            raise HTTPException(
                status_code=status.HTTP_401_UNAUTHORIZED,
                detail="Invalid credentials",
                headers={"WWW-Authenticate": "Bearer"},
            )

        now = datetime_now_sec()
        if api_key_doc.expires_at < now:
            raise HTTPException(
                status_code=status.HTTP_401_UNAUTHORIZED,
                detail="Invalid credentials",
                headers={"WWW-Authenticate": "Bearer"},
            )

        mongo_user = await engine.find_one(MongoUser, MongoUser.id == api_key_doc.user_id)
        if mongo_user is None:
            raise HTTPException(
                status_code=status.HTTP_401_UNAUTHORIZED,
                detail="Invalid credentials",
                headers={"WWW-Authenticate": "Bearer"},
            )

        # Fire-and-forget last_used_at update
        asyncio.ensure_future(_update_last_used(api_key_doc.id, engine))

        verified_user = VerifiedUser(
            email=mongo_user.email,
            user_id=mongo_user.id,
            roles=["user"],
            name=mongo_user.full_name,
        )
        return verified_user, mongo_user


async def _update_last_used(key_id, engine) -> None:
    try:
        doc = await engine.find_one(ApiKey, ApiKey.id == key_id)
        if doc:
            doc.last_used_at = datetime_now_sec()
            await engine.save(doc)
    except Exception:
        pass


# Singleton instance
token_strategy = TokenStrategy()
