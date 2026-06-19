#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

"""
Integration tests for knowledge endpoints.
Tests complete request-response flow with real database.

Pre-flight findings:
- KnowledgePermissionValidator is minimal; only has validate_ownership with pass.
  No reachable validator methods dereference the CRUD argument passed as None to
  KnowledgeValidator(None) under override_auth_admin.
- KnowledgeResolver._process_share_changes does not perform target-existence lookup.
  Share records are created from provided target_id directly; no real User/Group
  persistence required for sharing tests.
- Route prefix confirmed: /api/knowledge
- Test infrastructure: working (9/9 tests in test_endpoint_file.py pass).
"""

import pytest
from httpx import AsyncClient
from odmantic import ObjectId
from unittest.mock import AsyncMock

from app.models.knowledge import Knowledge
from const.common import OwnerType, SharingRoles


async def _create_knowledge(test_engine, owner_id, name="test_kb", **kwargs):
    knowledge = Knowledge(name=name, owner_id=owner_id, **kwargs)
    return await test_engine.save(knowledge)


@pytest.mark.integration
@pytest.mark.asyncio
async def test_get_knowledge_list_empty(
    async_client: AsyncClient,
    override_auth_admin,
    test_engine,
):
    """Test getting empty knowledge list."""
    response = await async_client.get("/api/knowledge")

    assert response.status_code == 200
    data = response.json()
    assert "data" in data
    assert data["data"] == []


@pytest.mark.integration
@pytest.mark.asyncio
async def test_get_knowledge_list_with_data(
    async_client: AsyncClient,
    user_factory,
    override_auth_admin,
    test_engine,
):
    """Test getting list of knowledge bases with data."""
    user = await user_factory(email=override_auth_admin.email)
    kb1 = await _create_knowledge(test_engine, owner_id=user.id, name="kb1")
    kb2 = await _create_knowledge(test_engine, owner_id=user.id, name="kb2")

    response = await async_client.get("/api/knowledge")

    assert response.status_code == 200
    data = response.json()
    assert "data" in data
    returned_ids = {kb["id"] for kb in data["data"]}
    assert str(kb1.id) in returned_ids
    assert str(kb2.id) in returned_ids


@pytest.mark.integration
@pytest.mark.asyncio
async def test_get_knowledge_by_id(
    async_client: AsyncClient,
    user_factory,
    override_auth_admin,
    test_engine,
):
    """Test getting a specific knowledge base by ID."""
    user = await user_factory(email=override_auth_admin.email)
    kb = await _create_knowledge(test_engine, owner_id=user.id, name="test_kb")

    response = await async_client.get(f"/api/knowledge/{kb.id}")

    assert response.status_code == 200
    data = response.json()
    assert data["id"] == str(kb.id)
    assert data["name"] == "test_kb"
    assert data["is_owner"] is True


@pytest.mark.integration
@pytest.mark.asyncio
async def test_search_knowledge(
    async_client: AsyncClient,
    override_auth_admin,
):
    """Test searching knowledge bases."""
    response = await async_client.post(
        "/api/knowledge/search",
        json={"filters": {}, "page": 1, "page_size": 10},
    )

    assert response.status_code == 200
    data = response.json()
    assert "data" in data
    assert isinstance(data["data"], list)


@pytest.mark.integration
@pytest.mark.asyncio
async def test_get_knowledge_share_empty(
    async_client: AsyncClient,
    user_factory,
    override_auth_admin,
    test_engine,
):
    """Test getting empty share list for a knowledge base."""
    user = await user_factory(email=override_auth_admin.email)
    kb = await _create_knowledge(test_engine, owner_id=user.id, name="test_kb")

    response = await async_client.get(f"/api/knowledge/{kb.id}/share")

    assert response.status_code == 200
    assert response.json() == []


@pytest.mark.integration
@pytest.mark.asyncio
async def test_create_knowledge_success(
    async_client: AsyncClient,
    user_factory,
    override_auth_admin,
    test_engine,
):
    """Test successful knowledge base creation."""
    user = await user_factory(email=override_auth_admin.email)

    response = await async_client.post(
        "/api/knowledge",
        json={"name": "kb1", "description": "Test knowledge base"},
    )

    assert response.status_code == 200
    data = response.json()
    assert data["name"] == "kb1"
    assert data["owner_id"] == str(user.id)

    kb_in_db = await test_engine.find_one(Knowledge, Knowledge.id == ObjectId(data["id"]))
    assert kb_in_db is not None
    assert kb_in_db.name == "kb1"


@pytest.mark.integration
@pytest.mark.asyncio
@pytest.mark.xfail(
    reason="TODO: knowledge-endpoint-tests - requires MongoDB replica set for transactions"
)
async def test_update_knowledge_general_success(
    async_client: AsyncClient,
    user_factory,
    override_auth_admin,
    test_engine,
):
    """Test successful knowledge base general settings update."""
    user = await user_factory(email=override_auth_admin.email)
    kb = await _create_knowledge(test_engine, owner_id=user.id, name="test_kb", description="old")

    response = await async_client.put(
        f"/api/knowledge/{kb.id}/general",
        json={"description": "new"},
    )

    assert response.status_code == 200
    assert response.json()["description"] == "new"

    kb_in_db = await test_engine.find_one(Knowledge, Knowledge.id == kb.id)
    assert kb_in_db.description == "new"


@pytest.mark.integration
@pytest.mark.asyncio
@pytest.mark.xfail(
    reason="TODO: knowledge-endpoint-tests - requires MongoDB replica set for transactions"
)
async def test_archive_knowledge_success(
    async_client: AsyncClient,
    user_factory,
    override_auth_admin,
    test_engine,
    mocker,
):
    """Test successful knowledge base archival."""
    # broker.send_task is called unconditionally by the controller; mock to avoid Redis dependency
    mocker.patch("services.taskiq.broker.broker.send_task", new_callable=AsyncMock)
    user = await user_factory(email=override_auth_admin.email)
    kb = await _create_knowledge(test_engine, owner_id=user.id, name="test_kb")

    response = await async_client.post(f"/api/knowledge/{kb.id}/archived")

    assert response.status_code == 204

    kb_in_db = await test_engine.find_one(Knowledge, Knowledge.id == kb.id)
    assert kb_in_db.is_archived is True


@pytest.mark.integration
@pytest.mark.asyncio
@pytest.mark.xfail(
    reason="TODO: knowledge-endpoint-tests - requires MongoDB replica set for transactions"
)
async def test_delete_knowledge_success(
    async_client: AsyncClient,
    user_factory,
    override_auth_admin,
    test_engine,
    mocker,
):
    """Test successful knowledge base deletion."""
    # broker.send_task is called unconditionally by the controller; mock to avoid Redis dependency
    mocker.patch("services.taskiq.broker.broker.send_task", new_callable=AsyncMock)
    user = await user_factory(email=override_auth_admin.email)
    kb = await _create_knowledge(test_engine, owner_id=user.id, name="test_kb")

    response = await async_client.delete(f"/api/knowledge/{kb.id}")

    assert response.status_code == 204

    kb_in_db = await test_engine.find_one(Knowledge, Knowledge.id == kb.id)
    assert kb_in_db is not None
    assert kb_in_db.is_deleted is True


@pytest.mark.integration
@pytest.mark.asyncio
async def test_share_knowledge_success(
    async_client: AsyncClient,
    user_factory,
    override_auth_admin,
    test_engine,
):
    """Test successful knowledge base sharing."""
    user = await user_factory(email=override_auth_admin.email)
    kb = await _create_knowledge(test_engine, owner_id=user.id, name="test_kb")

    share_payload = [
        {
            "target_id": str(ObjectId()),
            "target_type": OwnerType.USER,
            "role": SharingRoles.VIEWER,
        }
    ]

    response = await async_client.post(
        f"/api/knowledge/{kb.id}/share",
        json=share_payload,
    )

    assert response.status_code == 204


@pytest.mark.integration
@pytest.mark.asyncio
@pytest.mark.auth
async def test_get_knowledge_list_unauthorized(async_client: AsyncClient):
    """Test that unauthorized access returns 401."""
    response = await async_client.get("/api/knowledge")

    assert response.status_code == 401
