"""
Tests for API key management endpoints and authentication.

Coverage:
- Key generation format (rc_ prefix, 32+ chars)
- Hash-only storage (no plaintext in DB)
- Expired key rejection
- Revoked key rejection
- Invalid key rejection
- Ownership check on DELETE (404 for other user's key)
- Valid key resolves to correct DB user via combined dependency
- Per-user limit enforcement
"""
import hashlib
from datetime import timedelta

import pytest
import pytest_asyncio
from httpx import AsyncClient
from odmantic import ObjectId

from app.core.auth.token import generate_api_key, hash_key
from app.models.api_key import ApiKey
from app.models.user import User as MongoUser
from app.utils.helpers import datetime_now_sec

BASE = "/api/keys"


# ---------------------------------------------------------------------------
# Unit tests — key generation and hashing (no DB, no app)
# ---------------------------------------------------------------------------

def test_generate_api_key_format():
    plaintext, key_hash, key_prefix = generate_api_key()
    assert plaintext.startswith("rc_")
    assert len(plaintext) >= 35  # rc_ + 32 chars minimum
    assert key_prefix == plaintext[:8]
    assert key_hash == hashlib.sha256(plaintext.encode()).hexdigest()


def test_hash_key_deterministic():
    plaintext, _, _ = generate_api_key()
    assert hash_key(plaintext) == hash_key(plaintext)


def test_generate_api_key_unique():
    keys = {generate_api_key()[0] for _ in range(50)}
    assert len(keys) == 50


# ---------------------------------------------------------------------------
# Integration tests — management endpoints
# ---------------------------------------------------------------------------

@pytest.mark.integration
@pytest.mark.asyncio
async def test_list_keys_empty(async_client: AsyncClient, override_auth_admin):
    response = await async_client.get(BASE + "/")
    assert response.status_code == 200
    assert response.json() == []


@pytest.mark.integration
@pytest.mark.asyncio
async def test_create_key_returns_plaintext_once(
    async_client: AsyncClient,
    override_auth_admin,
    test_engine,
):
    expires_at = (datetime_now_sec() + timedelta(days=30)).isoformat()
    response = await async_client.post(BASE + "/", json={"name": "my-key", "expires_at": expires_at})

    assert response.status_code == 201
    body = response.json()
    assert "key" in body
    assert body["key"].startswith("rc_")
    assert len(body["key"]) >= 35
    assert body["key_prefix"] == body["key"][:8]

    # Hash stored — plaintext NOT in DB
    all_keys = await test_engine.find(ApiKey, ApiKey.user_id == override_auth_admin.user_id)
    assert len(all_keys) == 1
    doc = all_keys[0]
    assert doc.key_hash != body["key"]  # hash != plaintext
    assert "rc_" not in doc.key_hash    # hash is hex, not raw key
    assert doc.key_hash == hash_key(body["key"])


@pytest.mark.integration
@pytest.mark.asyncio
async def test_list_keys_returns_metadata_only(
    async_client: AsyncClient,
    override_auth_admin,
):
    expires_at = (datetime_now_sec() + timedelta(days=30)).isoformat()
    await async_client.post(BASE + "/", json={"name": "k1", "expires_at": expires_at})

    response = await async_client.get(BASE + "/")
    assert response.status_code == 200
    items = response.json()
    assert len(items) == 1
    assert "key" not in items[0]
    assert "key_hash" not in items[0]
    assert "key_prefix" in items[0]


@pytest.mark.integration
@pytest.mark.asyncio
async def test_create_key_enforces_max_expiry(
    async_client: AsyncClient,
    override_auth_admin,
):
    expires_at = (datetime_now_sec() + timedelta(days=400)).isoformat()
    response = await async_client.post(BASE + "/", json={"name": "too-long", "expires_at": expires_at})
    assert response.status_code == 422


@pytest.mark.integration
@pytest.mark.asyncio
async def test_create_key_enforces_per_user_limit(
    async_client: AsyncClient,
    override_auth_admin,
    test_engine,
):
    from app.api.v1.endpoints.keys import MAX_KEYS_PER_USER

    # Seed keys directly to avoid slow loop
    for i in range(MAX_KEYS_PER_USER):
        _, key_hash, key_prefix = generate_api_key()
        doc = ApiKey(
            user_id=override_auth_admin.user_id,
            name=f"key-{i}",
            key_hash=key_hash,
            key_prefix=key_prefix,
            expires_at=datetime_now_sec() + timedelta(days=30),
        )
        await test_engine.save(doc)

    expires_at = (datetime_now_sec() + timedelta(days=30)).isoformat()
    response = await async_client.post(BASE + "/", json={"name": "over-limit", "expires_at": expires_at})
    assert response.status_code == 422


@pytest.mark.integration
@pytest.mark.asyncio
async def test_delete_key_revokes_it(
    async_client: AsyncClient,
    override_auth_admin,
    test_engine,
):
    expires_at = (datetime_now_sec() + timedelta(days=30)).isoformat()
    create_resp = await async_client.post(BASE + "/", json={"name": "to-revoke", "expires_at": expires_at})
    key_id = create_resp.json()["id"]

    response = await async_client.delete(f"{BASE}/{key_id}")
    assert response.status_code == 204

    doc = await test_engine.find_one(ApiKey, ApiKey.id == ObjectId(key_id))
    assert doc is not None
    assert doc.revoked is True


@pytest.mark.integration
@pytest.mark.asyncio
async def test_delete_key_404_for_other_user(
    async_client: AsyncClient,
    override_auth_admin,
    test_engine,
):
    """DELETE on a key belonging to another user returns 404 (no existence leak)."""
    other_user = MongoUser(email="other@test.com", full_name="Other", is_active=True)
    await test_engine.save(other_user)

    _, key_hash, key_prefix = generate_api_key()
    doc = ApiKey(
        user_id=other_user.id,
        name="other-key",
        key_hash=key_hash,
        key_prefix=key_prefix,
        expires_at=datetime_now_sec() + timedelta(days=30),
    )
    await test_engine.save(doc)

    response = await async_client.delete(f"{BASE}/{doc.id}")
    assert response.status_code == 404


# ---------------------------------------------------------------------------
# Integration tests — API key authentication (combined dependency)
# ---------------------------------------------------------------------------

@pytest.mark.integration
@pytest.mark.asyncio
async def test_valid_api_key_authenticates_via_x_api_key_header(
    async_client: AsyncClient,
    test_engine,
    app,
    override_get_engine,
    override_get_redis,
):
    """Valid rc_ key in X-API-Key header resolves to correct DB user."""
    # Create user in DB
    user = MongoUser(email="keyuser@test.com", full_name="Key User", is_active=True)
    await test_engine.save(user)

    # Create API key for that user
    plaintext, key_hash, key_prefix = generate_api_key()
    doc = ApiKey(
        user_id=user.id,
        name="test-key",
        key_hash=key_hash,
        key_prefix=key_prefix,
        expires_at=datetime_now_sec() + timedelta(days=30),
    )
    await test_engine.save(doc)

    # Hit /api/user/me with the API key
    response = await async_client.get(
        "/api/user/me",
        headers={"X-API-Key": plaintext},
    )
    assert response.status_code == 200
    assert response.json()["email"] == user.email


@pytest.mark.integration
@pytest.mark.asyncio
async def test_valid_api_key_authenticates_via_bearer_header(
    async_client: AsyncClient,
    test_engine,
    override_get_engine,
    override_get_redis,
):
    """Valid rc_ key in Authorization: Bearer header resolves to correct DB user."""
    user = MongoUser(email="bearerkey@test.com", full_name="Bearer Key User", is_active=True)
    await test_engine.save(user)

    plaintext, key_hash, key_prefix = generate_api_key()
    doc = ApiKey(
        user_id=user.id,
        name="bearer-key",
        key_hash=key_hash,
        key_prefix=key_prefix,
        expires_at=datetime_now_sec() + timedelta(days=30),
    )
    await test_engine.save(doc)

    response = await async_client.get(
        "/api/user/me",
        headers={"Bearer": plaintext},
    )
    assert response.status_code == 200
    assert response.json()["email"] == user.email


@pytest.mark.integration
@pytest.mark.asyncio
async def test_expired_key_rejected(
    async_client: AsyncClient,
    test_engine,
    override_get_engine,
    override_get_redis,
):
    user = MongoUser(email="expiredkey@test.com", full_name="Expired", is_active=True)
    await test_engine.save(user)

    plaintext, key_hash, key_prefix = generate_api_key()
    doc = ApiKey(
        user_id=user.id,
        name="expired",
        key_hash=key_hash,
        key_prefix=key_prefix,
        expires_at=datetime_now_sec() - timedelta(seconds=1),  # already expired
    )
    await test_engine.save(doc)

    response = await async_client.get(
        "/api/user/me",
        headers={"X-API-Key": plaintext},
    )
    assert response.status_code == 401


@pytest.mark.integration
@pytest.mark.asyncio
async def test_revoked_key_rejected(
    async_client: AsyncClient,
    test_engine,
    override_get_engine,
    override_get_redis,
):
    user = MongoUser(email="revokedkey@test.com", full_name="Revoked", is_active=True)
    await test_engine.save(user)

    plaintext, key_hash, key_prefix = generate_api_key()
    doc = ApiKey(
        user_id=user.id,
        name="revoked",
        key_hash=key_hash,
        key_prefix=key_prefix,
        expires_at=datetime_now_sec() + timedelta(days=30),
        revoked=True,
    )
    await test_engine.save(doc)

    response = await async_client.get(
        "/api/user/me",
        headers={"X-API-Key": plaintext},
    )
    assert response.status_code == 401


@pytest.mark.integration
@pytest.mark.asyncio
async def test_unknown_key_rejected(
    async_client: AsyncClient,
    override_get_engine,
    override_get_redis,
):
    response = await async_client.get(
        "/api/user/me",
        headers={"X-API-Key": "rc_nonexistentkey1234567890abcdef"},
    )
    assert response.status_code == 401


@pytest.mark.integration
@pytest.mark.asyncio
async def test_wrong_format_key_rejected(
    async_client: AsyncClient,
    override_get_engine,
    override_get_redis,
):
    response = await async_client.get(
        "/api/user/me",
        headers={"X-API-Key": "notvalid"},
    )
    assert response.status_code == 401
