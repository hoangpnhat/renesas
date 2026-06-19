#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

import pytest
from httpx import AsyncClient
from odmantic import ObjectId

from app.models.user import User as MongoUser
from app.const.common import CommonLang, CommonMode


BASE = "/api/user"


@pytest.mark.integration
@pytest.mark.asyncio
@pytest.mark.auth
async def test_unauthorized_returns_401(async_client: AsyncClient):
    """Test that unauthorized request returns 401."""
    response = await async_client.get(f"{BASE}/me")

    assert response.status_code == 401


@pytest.mark.integration
@pytest.mark.asyncio
async def test_signup_creates_user_when_not_existing(
    async_client: AsyncClient,
    override_auth_admin,
    test_engine,
):
    """Test successful user signup creates a new user record."""
    existing = await test_engine.find_one(MongoUser, MongoUser.email == override_auth_admin.email)
    await test_engine.delete(existing)

    response = await async_client.post(f"{BASE}/")

    assert response.status_code == 200
    body = response.json()
    assert body["email"] == override_auth_admin.email
    assert body["full_name"] == override_auth_admin.name

    user_doc = await test_engine.find_one(MongoUser, MongoUser.email == override_auth_admin.email)
    assert user_doc is not None
    assert user_doc.is_active is True
    assert user_doc.email_validated is True


@pytest.mark.integration
@pytest.mark.asyncio
async def test_signup_conflict_when_user_exists(
    async_client: AsyncClient,
    override_auth_admin,
):
    """Test signup returns 400 when user already exists."""
    response = await async_client.post(f"{BASE}/")

    assert response.status_code == 400
    assert response.json()["detail"] == "The user with this username already exists in the system."


@pytest.mark.integration
@pytest.mark.asyncio
async def test_get_current_user_returns_seeded_admin(
    async_client: AsyncClient,
    override_auth_admin,
):
    """Test GET /me returns current authenticated user."""
    response = await async_client.get(f"{BASE}/me")

    assert response.status_code == 200
    body = response.json()
    assert body["email"] == override_auth_admin.email
    assert body["id"] == str(override_auth_admin.user_id)
    assert body["full_name"] == override_auth_admin.name


@pytest.mark.integration
@pytest.mark.asyncio
async def test_get_user_settings_returns_defaults(
    async_client: AsyncClient,
    override_auth_admin,
):
    """Test GET /me/settings returns default embedded settings."""
    response = await async_client.get(f"{BASE}/me/settings")

    assert response.status_code == 200
    body = response.json()
    assert body["preferred_language"] == CommonLang.ENG.value
    assert body["mode"] == CommonMode.LIGHT_MODE.value


@pytest.mark.integration
@pytest.mark.asyncio
@pytest.mark.xfail(
    reason="TODO: user-endpoint-tests - requires MongoDB replica set for transactions"
)
async def test_patch_user_settings_merges_only_supplied_fields(
    async_client: AsyncClient,
    override_auth_admin,
    test_engine,
    test_redis_client,
):
    """Test PATCH /me/settings merges only fields present in the request body."""
    response = await async_client.patch(f"{BASE}/me/settings", json={"mode": "night_mode"})

    assert response.status_code == 200
    body = response.json()
    assert body["mode"] == CommonMode.NIGHT_MODE.value
    assert body["preferred_language"] == CommonLang.ENG.value

    user_doc = await test_engine.find_one(MongoUser, MongoUser.id == override_auth_admin.user_id)
    assert user_doc.settings.mode == CommonMode.NIGHT_MODE
    assert user_doc.settings.preferred_language == CommonLang.ENG

    assert await test_redis_client.get(override_auth_admin.email) is None


@pytest.mark.integration
@pytest.mark.asyncio
async def test_search_returns_matching_users_excluding_current(
    async_client: AsyncClient,
    override_auth_admin,
    user_factory,
):
    """Test search returns users matching the query, excluding the current user."""
    await user_factory(email="alice@example.com", full_name="Alice Anderson")
    await user_factory(email="bob@example.com", full_name="Bob Brown")

    response = await async_client.get(f"{BASE}/search", params={"query": "alice", "limit": 5})

    assert response.status_code == 200
    body = response.json()
    assert "data" in body
    assert isinstance(body["data"], list)

    emails = {e["email"] for e in body["data"]}
    assert "alice@example.com" in emails
    assert override_auth_admin.email not in emails
