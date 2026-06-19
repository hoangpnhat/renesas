#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

import pytest
from httpx import AsyncClient
from odmantic import ObjectId

from app.models.model_management import ModelManagement


@pytest.mark.asyncio
@pytest.mark.integration
class TestAdminModelManagement:
    """Integration tests for admin model management endpoints with consumption limits."""

    async def test_create_model_with_consumption_limits(
        self,
        async_client: AsyncClient,
        test_engine,
        override_auth_admin
    ):
        """Test creating model with threshold and consumption_range_hours."""
        # Act
        response = await async_client.post(
            "/api/admin/model-management",
            json={
                "name": "admin-test-model",
                "model_path": "databricks/admin-test",
                "roles": ["chat", "edit"],
                "threshold": 1000,
                "consumption_range_hours": 24
            }
        )

        # Assert
        assert response.status_code == 201
        data = response.json()
        assert data["name"] == "admin-test-model"
        assert data.get("threshold") == 1000
        assert data.get("consumption_range_hours") == 24

        # Verify persisted to database
        model = await test_engine.find_one(
            ModelManagement,
            ModelManagement.name == "admin-test-model"
        )
        assert model is not None
        assert model.threshold == 1000
        assert model.consumption_range_hours == 24

    async def test_update_model_add_consumption_limits(
        self,
        async_client: AsyncClient,
        test_engine,
        model_management_factory,
        override_auth_admin
    ):
        """Test adding consumption limits to existing model."""
        # Arrange - create model without limits
        model = await model_management_factory(
            name="admin-existing-model",
            model_path="databricks/existing",
            threshold=None,
            consumption_range_hours=None
        )

        # Act - add consumption limits
        response = await async_client.patch(
            f"/api/admin/model-management/{model.id}",
            json={
                "name": "admin-existing-model",
                "model_path": "databricks/existing",
                "roles": ["chat"],
                "threshold": 500,
                "consumption_range_hours": 1
            }
        )

        # Assert
        assert response.status_code == 200
        data = response.json()
        assert data.get("threshold") == 500
        assert data.get("consumption_range_hours") == 1

        # Verify in database
        updated_model = await test_engine.find_one(
            ModelManagement,
            ModelManagement.id == model.id
        )
        assert updated_model.threshold == 500
        assert updated_model.consumption_range_hours == 1

    async def test_update_model_remove_consumption_limits(
        self,
        async_client: AsyncClient,
        test_engine,
        model_management_factory,
        override_auth_admin
    ):
        """Test removing consumption limits by setting both to null."""
        # Arrange
        model = await model_management_factory(
            name="admin-model-to-disable",
            model_path="databricks/model",
            threshold=1000,
            consumption_range_hours=24
        )

        # Act - set both to null
        response = await async_client.patch(
            f"/api/admin/model-management/{model.id}",
            json={
                "name": "admin-model-to-disable",
                "model_path": "databricks/model",
                "roles": ["chat"],
                "threshold": None,
                "consumption_range_hours": None
            }
        )

        # Assert
        assert response.status_code == 200
        data = response.json()
        assert data.get("threshold") is None
        assert data.get("consumption_range_hours") is None

        # Verify in database
        updated_model = await test_engine.find_one(
            ModelManagement,
            ModelManagement.id == model.id
        )
        assert updated_model.threshold is None
        assert updated_model.consumption_range_hours is None

    async def test_create_model_reject_partial_config(
        self,
        async_client: AsyncClient,
        override_auth_admin
    ):
        """Test 422 error when only threshold provided."""
        # Act - only threshold, no consumption_range_hours
        response = await async_client.post(
            "/api/admin/model-management",
            json={
                "name": "admin-partial-model",
                "model_path": "databricks/partial",
                "roles": ["chat"],
                "threshold": 1000
                # Missing consumption_range_hours
            }
        )

        # Assert
        assert response.status_code == 422
        error_detail = response.json()["detail"]
        assert any(
            "threshold and consumption_range_hours must be set together" in str(err)
            for err in error_detail
        )

    async def test_create_model_reject_negative_threshold(
        self,
        async_client: AsyncClient,
        override_auth_admin
    ):
        """Test 422 error for threshold <= 0."""
        # Act - threshold must be > 0
        response = await async_client.post(
            "/api/admin/model-management",
            json={
                "name": "admin-zero-threshold",
                "model_path": "databricks/zero",
                "roles": ["chat"],
                "threshold": 0,
                "consumption_range_hours": 24
            }
        )

        # Assert
        assert response.status_code == 422

    async def test_create_model_reject_excessive_range(
        self,
        async_client: AsyncClient,
        override_auth_admin
    ):
        """Test 422 error for consumption_range_hours > 168."""
        # Act
        response = await async_client.post(
            "/api/admin/model-management",
            json={
                "name": "admin-excessive-range",
                "model_path": "databricks/excessive",
                "roles": ["chat"],
                "threshold": 1000,
                "consumption_range_hours": 200  # > 168
            }
        )

        # Assert
        assert response.status_code == 422

    async def test_create_model_without_consumption_config(
        self,
        async_client: AsyncClient,
        test_engine,
        override_auth_admin
    ):
        """Test creating model without consumption limits."""
        # Act - no threshold or consumption_range_hours
        response = await async_client.post(
            "/api/admin/model-management",
            json={
                "name": "admin-no-limits-model",
                "model_path": "databricks/no-limits",
                "roles": ["chat"]
                # threshold and consumption_range_hours omitted
            }
        )

        # Assert
        assert response.status_code == 201
        data = response.json()
        assert data.get("threshold") is None
        assert data.get("consumption_range_hours") is None

        # Verify in database
        model = await test_engine.find_one(
            ModelManagement,
            ModelManagement.name == "admin-no-limits-model"
        )
        assert model.threshold is None
        assert model.consumption_range_hours is None
