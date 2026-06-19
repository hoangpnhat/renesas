#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

import pytest
from datetime import datetime, timedelta, timezone
from freezegun import freeze_time
from httpx import AsyncClient
from odmantic import ObjectId
from unittest.mock import patch

from app.models.user import User
from app.models.model_management import ModelManagement
from app.models.token_consumption import UserTokenConsumption
from app.utils.helpers import datetime_now_sec
from app.utils.handle_exception import ApplicationException


@pytest.mark.asyncio
@pytest.mark.integration
class TestPostConfigs:
    """Integration tests for POST /api/user-model/configs endpoint."""

    async def test_post_configs_without_consumption(
        self,
        async_client: AsyncClient,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test POST with empty consumption list - verify model configs returned."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            model_path="test/model",
            threshold=1000,
            consumption_range_hours=24
        )

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        assert response.status_code == 200
        data = response.json()
        assert "config" in data
        assert "models" in data["config"]
        assert len(data["config"]["models"]) >= 1

    async def test_post_configs_with_consumption(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test POST with consumption data - verify records persisted and configs returned."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            model_path="test/model",
            threshold=1000,
            consumption_range_hours=24
        )

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {"model_id": str(model.id), "tokens_in": 100, "tokens_out": 10, "client_timestamp": int(datetime_now_sec().timestamp() * 1000)}
                ]
            }
        )

        # Assert
        assert response.status_code == 200

        # Verify consumption record was persisted
        records = await test_engine.find(
            UserTokenConsumption,
            (UserTokenConsumption.user_id == user.email)
            & (UserTokenConsumption.model_id == model.id)
        )
        assert len(records) == 1
        assert records[0].token_count == 150  # 5 * 10 + 100 = 150

    async def test_post_configs_calculates_remaining_tokens(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test remaining token calculation."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            model_path="test/model",
            threshold=1000,
            consumption_range_hours=24
        )

        # Create existing consumption
        existing_timestamp = datetime_now_sec()
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=600,
            client_timestamp=int(existing_timestamp.timestamp() * 1000),
            timestamp=datetime_now_sec()
        ))

        # Act - submit additional consumption
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {"model_id": str(model.id), "tokens_in": 80, "tokens_out": 4, "client_timestamp": int(datetime_now_sec().timestamp() * 1000)}
                ]
            }
        )

        # Assert
        assert response.status_code == 200
        data = response.json()
        models = data["config"]["models"]
        model_data = next((m for m in models if m["name"] == "test-model"), None)

        assert model_data is not None

        # Verify all required fields present in consumptionLimit
        assert "consumptionLimit" in model_data
        consumption_limit = model_data["consumptionLimit"]

        assert "enabled" in consumption_limit
        assert "threshold" in consumption_limit
        assert "rangeHours" in consumption_limit  # camelCase
        assert "consumed" in consumption_limit
        assert "remaining" in consumption_limit

        # Verify field values
        assert consumption_limit["enabled"] is True
        assert consumption_limit["threshold"] == 1000
        assert consumption_limit["rangeHours"] == 24
        assert consumption_limit["consumed"] == 700
        assert consumption_limit["remaining"] == 300

    async def test_post_configs_enforces_limits(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test consumption over threshold (clients derive exceeded status from remaining < 0)."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            model_path="test/model",
            threshold=1000,
            consumption_range_hours=24
        )

        # Create consumption that exceeds threshold
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=1200,
            client_timestamp=int(datetime_now_sec().timestamp() * 1000),
            timestamp=datetime_now_sec()
        ))

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        assert response.status_code == 200
        data = response.json()
        models = data["config"]["models"]
        model_data = next((m for m in models if m["name"] == "test-model"), None)

        assert model_data is not None
        assert model_data["consumptionLimit"]["consumed"] == 1200
        assert model_data["consumptionLimit"]["remaining"] == -200
        # Clients should check: remaining < 0 to determine exceeded status

    @freeze_time("2026-04-01 12:00:00")
    async def test_post_configs_rolling_window(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test that old records are excluded from rolling window."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            model_path="test/model",
            threshold=1000,
            consumption_range_hours=1  # 1-hour window
        )

        current_time = datetime(2026, 4, 1, 12, 0, 0, tzinfo=timezone.utc)

        # Create old record (outside window)
        old_time = current_time - timedelta(hours=2)
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=500,
            client_timestamp=int(old_time.timestamp() * 1000),
            timestamp=old_time
        ))

        # Create recent record (inside window)
        recent_time = current_time - timedelta(minutes=30)
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=300,
            client_timestamp=int(recent_time.timestamp() * 1000),
            timestamp=recent_time
        ))

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        assert response.status_code == 200
        data = response.json()
        models = data["config"]["models"]
        model_data = next((m for m in models if m["name"] == "test-model"), None)

        # Only recent record (300) should be counted
        assert model_data["consumptionLimit"]["consumed"] == 300

    async def test_post_configs_multiple_models(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test per-model tracking with multiple models."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model1 = await model_management_factory(
            name="model-1",
            model_path="test/model1",
            threshold=1000,
            consumption_range_hours=24
        )
        model2 = await model_management_factory(
            name="model-2",
            model_path="test/model2",
            threshold=500,
            consumption_range_hours=24
        )

        # Create different consumption for each model
        now = datetime_now_sec()
        await test_engine.save_all([
            UserTokenConsumption(
                user_id=user.email,
                model_id=model1.id,
                token_count=400,
                client_timestamp=int(now.timestamp() * 1000),
                timestamp=now
            ),
            UserTokenConsumption(
                user_id=user.email,
                model_id=model2.id,
                token_count=450,
                client_timestamp=int(now.timestamp() * 1000),
                timestamp=now
            )
        ])

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        assert response.status_code == 200
        data = response.json()
        models = data["config"]["models"]

        model1_data = next((m for m in models if m["name"] == "model-1"), None)
        model2_data = next((m for m in models if m["name"] == "model-2"), None)

        assert model1_data["consumptionLimit"]["consumed"] == 400
        assert model1_data["consumptionLimit"]["remaining"] > 0  # Under threshold

        assert model2_data["consumptionLimit"]["consumed"] == 450
        assert model2_data["consumptionLimit"]["remaining"] > 0  # Under threshold

    async def test_post_configs_no_threshold_configured(
        self,
        async_client: AsyncClient,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test that models without threshold have null consumption_limit."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="no-threshold-model",
            model_path="test/model",
            threshold=None,
            consumption_range_hours=None
        )

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        assert response.status_code == 200
        data = response.json()
        models = data["config"]["models"]
        model_data = next((m for m in models if m["name"] == "no-threshold-model"), None)

        assert model_data is not None
        assert "consumptionLimit" in model_data
        assert model_data["consumptionLimit"] is None

    async def test_post_configs_invalid_token_count(
        self,
        async_client: AsyncClient,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test validation error for negative/excessive tokens."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            model_path="test/model"
        )

        # Test negative tokens_in
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {"model_id": str(model.id), "tokens_in": -100, "tokens_out": 10}
                ]
            }
        )
        assert response.status_code == 422

        # Test excessive tokens_in
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {"model_id": str(model.id), "tokens_in": 2_000_000, "tokens_out": 10}
                ]
            }
        )
        assert response.status_code == 422

    async def test_post_configs_batch_size_limit(
        self,
        async_client: AsyncClient,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test batch size validation (max 1,000 records)."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            model_path="test/model"
        )

        # Create 1,001 consumption records
        consumption = [
            {"model_id": str(model.id), "tokens_in": 8, "tokens_out": 1, "client_timestamp": int(datetime_now_sec().timestamp() * 1000)}
            for _ in range(1001)
        ]

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": consumption}
        )

        # Assert
        assert response.status_code == 422

    async def test_post_configs_user_isolation(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test users see only own consumption."""
        # Arrange
        user1 = await user_factory(email=override_auth_admin.email)
        user2 = await user_factory(email="other@test.com")

        model = await model_management_factory(
            name="test-model",
            model_path="test/model",
            threshold=1000,
            consumption_range_hours=24
        )

        # Create consumption for both users
        now = datetime_now_sec()
        await test_engine.save_all([
            UserTokenConsumption(
                user_id=user1.email,
                model_id=model.id,
                token_count=100,
                client_timestamp=int(now.timestamp() * 1000),
                timestamp=now
            ),
            UserTokenConsumption(
                user_id=user2.email,
                model_id=model.id,
                token_count=500,
                client_timestamp=int(now.timestamp() * 1000),
                timestamp=now
            )
        ])

        # Act - Request as user1 (override_auth_admin)
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert - Should only see user1's consumption
        assert response.status_code == 200
        data = response.json()
        models = data["config"]["models"]
        model_data = next((m for m in models if m["name"] == "test-model"), None)

        assert model_data["consumptionLimit"]["consumed"] == 100
        # Should NOT include user2's 500 tokens

    async def test_post_configs_exceeded_at_exact_threshold(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test consumption at exact threshold (clients derive exceeded status from remaining <= 0)."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            model_path="test/model",
            threshold=1000,
            consumption_range_hours=24
        )

        # Create consumption exactly at threshold
        now = datetime_now_sec()
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=1000,
            client_timestamp=int(now.timestamp() * 1000),
            timestamp=now
        ))

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        assert response.status_code == 200
        data = response.json()
        models = data["config"]["models"]
        model_data = next((m for m in models if m["name"] == "test-model"), None)

        assert model_data is not None
        assert model_data["consumptionLimit"]["consumed"] == 1000
        assert model_data["consumptionLimit"]["remaining"] == 0
        # CRITICAL: Clients should check remaining <= 0 to determine threshold reached/exceeded

    async def test_post_configs_excludes_deleted_models(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test that models with deleted_at set are excluded from response."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)

        # Create active model
        active_model = await model_management_factory(
            name="active-model",
            model_path="test/active",
            threshold=1000,
            consumption_range_hours=24
        )

        # Create deleted model
        deleted_model = await model_management_factory(
            name="deleted-model",
            model_path="test/deleted",
            threshold=1000,
            consumption_range_hours=24,
            deleted_at=datetime_now_sec()  # Soft deleted
        )

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        assert response.status_code == 200
        data = response.json()
        models = data["config"]["models"]
        model_names = [m["name"] for m in models]

        # Active model should be present
        assert "active-model" in model_names

        # Deleted model should NOT be present
        assert "deleted-model" not in model_names

    async def test_post_configs_batch_consumption_success(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test successful batch consumption with multiple records."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model1 = await model_management_factory(
            name="model-1",
            model_path="test/model1"
        )
        model2 = await model_management_factory(
            name="model-2",
            model_path="test/model2"
        )
        model3 = await model_management_factory(
            name="model-3",
            model_path="test/model3"
        )

        # Act - submit batch with 3 different models
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {"model_id": str(model1.id), "tokens_in": 80, "tokens_out": 4, "client_timestamp": int(datetime_now_sec().timestamp() * 1000)},
                    {"model_id": str(model2.id), "tokens_in": 160, "tokens_out": 8, "client_timestamp": int(datetime_now_sec().timestamp() * 1000)},
                    {"model_id": str(model3.id), "tokens_in": 250, "tokens_out": 10, "client_timestamp": int(datetime_now_sec().timestamp() * 1000)}
                ]
            }
        )

        # Assert
        assert response.status_code == 200

        # Verify 3 separate records created
        records = await test_engine.find(
            UserTokenConsumption,
            UserTokenConsumption.user_id == user.email
        )
        assert len(records) == 3

        # Verify each model has correct token count (converted: 5 * tokens_out + tokens_in)
        records_by_model = {str(r.model_id): r.token_count for r in records}
        assert records_by_model[str(model1.id)] == 100  # 5*4 + 80
        assert records_by_model[str(model2.id)] == 200  # 5*8 + 160
        assert records_by_model[str(model3.id)] == 300  # 5*10 + 250

        # Verify all have same timestamp (same request)
        timestamps = [r.timestamp for r in records]
        assert len(set(timestamps)) == 1  # All same timestamp

    async def test_post_configs_partial_batch_failure(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test that valid records are persisted when batch contains invalid model_ids."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        valid_model = await model_management_factory(
            name="valid-model",
            model_path="test/valid"
        )
        fake_id = str(ObjectId())  # Non-existent model

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {"model_id": str(valid_model.id), "tokens_in": 80, "tokens_out": 4, "client_timestamp": int(datetime_now_sec().timestamp() * 1000)},  # Valid
                    {"model_id": fake_id, "tokens_in": 160, "tokens_out": 8, "client_timestamp": int(datetime_now_sec().timestamp() * 1000)}  # Invalid - should be skipped
                ]
            }
        )

        # Assert - request succeeds (not rejected)
        assert response.status_code == 200

        # Only valid record persisted
        records = await test_engine.find(
            UserTokenConsumption,
            UserTokenConsumption.user_id == user.email
        )
        assert len(records) == 1
        assert records[0].model_id == valid_model.id
        assert records[0].token_count == 100  # 5*4 + 80

    async def test_post_configs_threshold_change_immediate_effect(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test that threshold changes take immediate effect."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            model_path="test/model",
            threshold=1000,
            consumption_range_hours=24
        )

        # Create consumption at 800 tokens
        now = datetime_now_sec()
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=800,
            client_timestamp=int(now.timestamp() * 1000),
            timestamp=now
        ))

        # Act - First request with threshold=1000 (under threshold)
        response1 = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )
        data1 = response1.json()
        model_data1 = next(m for m in data1["config"]["models"] if m["name"] == "test-model")
        assert model_data1["consumptionLimit"]["remaining"] == 200  # Under threshold (remaining > 0)

        # Admin changes threshold to 500 (lower than consumed)
        model.threshold = 500
        await test_engine.save(model)

        # Act - Second request with new threshold=500 (now exceeded)
        response2 = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        assert response2.status_code == 200
        data2 = response2.json()
        model_data2 = next(m for m in data2["config"]["models"] if m["name"] == "test-model")
        assert model_data2["consumptionLimit"]["remaining"] == -300  # 500 - 800 (exceeded, remaining < 0)


@pytest.mark.asyncio
@pytest.mark.integration
class TestTokenBlocking:
    """Integration tests for 2-hour blocking feature."""

    async def test_blocking_activated_when_threshold_exceeded(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test that blocking is activated when threshold is first exceeded."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            threshold=1000,
            consumption_range_hours=24
        )

        # Create consumption that exceeds threshold
        now = datetime_now_sec()
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=1100,
            client_timestamp=int(now.timestamp() * 1000),
            timestamp=now
        ))

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        assert response.status_code == 200
        data = response.json()
        model_data = next(m for m in data["config"]["models"] if m["name"] == "test-model")

        assert model_data["consumptionLimit"]["remaining"] < 0  # Exceeded threshold
        assert model_data["consumptionLimit"]["blocked"] is True
        assert model_data["consumptionLimit"]["blockedUntil"] is not None

        # Verify flag record was created
        flag_records = await test_engine.find(
            UserTokenConsumption,
            (UserTokenConsumption.user_id == user.email)
            & (UserTokenConsumption.model_id == model.id)
            & (UserTokenConsumption.flag == True)
        )
        assert len(flag_records) == 1
        assert flag_records[0].token_count == 0

    async def test_blocking_not_activated_when_under_threshold(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test that blocking is not activated when under threshold."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            threshold=1000,
            consumption_range_hours=24
        )

        # Create consumption under threshold
        now = datetime_now_sec()
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=500,
            client_timestamp=int(now.timestamp() * 1000),
            timestamp=now
        ))

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        data = response.json()
        model_data = next(m for m in data["config"]["models"] if m["name"] == "test-model")

        assert model_data["consumptionLimit"]["remaining"] > 0  # Under threshold
        assert model_data["consumptionLimit"]["blocked"] is False
        assert model_data["consumptionLimit"]["blockedUntil"] is None

    @freeze_time("2026-04-07 12:00:00")
    async def test_blocking_expires_after_2_hours(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test that blocking automatically expires after 2 hours."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            threshold=1000,
            consumption_range_hours=24
        )

        current_time = datetime(2026, 4, 7, 12, 0, 0, tzinfo=timezone.utc)

        # Create consumption that exceeded threshold (23 hours ago, still in 24h window)
        old_time = current_time - timedelta(hours=23)
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=1100,
            client_timestamp=int(old_time.timestamp() * 1000),
            timestamp=old_time
        ))

        # Create flag record 3 hours ago (expired)
        flag_time = current_time - timedelta(hours=3)
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=0,
            flag=True,
            client_timestamp=int(flag_time.timestamp() * 1000),
            timestamp=flag_time
        ))

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        data = response.json()
        model_data = next(m for m in data["config"]["models"] if m["name"] == "test-model")

        # Not blocked (flag is older than 2 hours)
        assert model_data["consumptionLimit"]["blocked"] is False
        assert model_data["consumptionLimit"]["blockedUntil"] is None

        # But consumption is 0 because aggregation uses flag timestamp as window start
        # and old consumption (23h ago) is BEFORE flag (3h ago)
        assert model_data["consumptionLimit"]["consumed"] == 0
        assert model_data["consumptionLimit"]["remaining"] > 0  # Under threshold after reset

    @freeze_time("2026-04-07 12:00:00")
    async def test_blocking_active_within_2_hours(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """
        Test that blocking remains active within 2-hour window and that consumed
        is frozen at flag.timestamp (only pre-flag records appear in the sum).
        """
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            threshold=1000,
            consumption_range_hours=24
        )

        current_time = datetime(2026, 4, 7, 12, 0, 0, tzinfo=timezone.utc)

        # Create flag record 1 hour ago (still active)
        flag_time = current_time - timedelta(hours=1)
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=0,
            flag=True,
            client_timestamp=int(flag_time.timestamp() * 1000),
            timestamp=flag_time
        ))

        # Consumption BEFORE flag — inside frozen window [flag_time - 24h, flag_time): counted
        pre_flag_time = flag_time - timedelta(hours=1)
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=500,
            client_timestamp=int(pre_flag_time.timestamp() * 1000),
            timestamp=pre_flag_time
        ))

        # Consumption AFTER flag — outside frozen window_end: NOT counted
        post_flag_time = current_time - timedelta(minutes=30)
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=300,
            client_timestamp=int(post_flag_time.timestamp() * 1000),
            timestamp=post_flag_time
        ))

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        data = response.json()
        model_data = next(m for m in data["config"]["models"] if m["name"] == "test-model")

        assert model_data["consumptionLimit"]["blocked"] is True
        expected_blocked_until = int((flag_time + timedelta(hours=2)).timestamp())
        assert model_data["consumptionLimit"]["blockedUntil"] == expected_blocked_until

        # window_end is frozen at flag_time: only the 500 pre-flag tokens appear
        assert model_data["consumptionLimit"]["consumed"] == 500

    async def test_fresh_start_after_unblock(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test that user gets fresh start after block expires - old consumption ignored."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            threshold=1000,
            consumption_range_hours=24
        )

        current_time = datetime_now_sec()

        # Create consumption that exceeded threshold (5 hours ago)
        old_time = current_time - timedelta(hours=5)
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=1100,
            client_timestamp=int(old_time.timestamp() * 1000),
            timestamp=old_time
        ))

        # Create flag record 3 hours ago (expired, but still within 24h rolling window)
        flag_time = current_time - timedelta(hours=3)
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=0,
            flag=True,
            client_timestamp=int(flag_time.timestamp() * 1000),
            timestamp=flag_time
        ))

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        data = response.json()
        model_data = next(m for m in data["config"]["models"] if m["name"] == "test-model")

        # Not blocked (flag expired - more than 2 hours old)
        assert model_data["consumptionLimit"]["blocked"] is False

        # KEY BEHAVIOR: Consumption is 0 because aggregation uses flag timestamp as window start
        # Old consumption (1100 from 5h ago) is BEFORE flag (3h ago), so it's excluded
        assert model_data["consumptionLimit"]["consumed"] == 0
        assert model_data["consumptionLimit"]["remaining"] > 0  # Under threshold after reset

    async def test_consumption_after_flag_counts_toward_threshold(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """
        Records added during an active block are only visible in `consumed` after the
        block expires.  The block window was [flag_time, flag_time+2h).  Now that the
        flag is older than 2h the window shifts to (flag_time, now), so those records
        become visible.
        """
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            threshold=1000,
            consumption_range_hours=24
        )

        current_time = datetime_now_sec()

        # Old consumption before flag — excluded by window_start = flag_time
        old_time = current_time - timedelta(hours=5)
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=1500,
            client_timestamp=int(old_time.timestamp() * 1000),
            timestamp=old_time
        ))

        # Flag record 3 hours ago (now expired)
        flag_time = current_time - timedelta(hours=3)
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=0,
            flag=True,
            client_timestamp=int(flag_time.timestamp() * 1000),
            timestamp=flag_time
        ))

        # Consumption recorded DURING the block period (flag_time to flag_time+2h)
        # while the block was active this record was invisible (frozen window_end);
        # now that the flag is expired it is visible in the window [flag_time, now)
        during_block_time = current_time - timedelta(hours=2, minutes=30)
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=800,
            client_timestamp=int(during_block_time.timestamp() * 1000),
            timestamp=during_block_time
        ))

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        data = response.json()
        model_data = next(m for m in data["config"]["models"] if m["name"] == "test-model")

        # Flag is expired — not blocked
        assert model_data["consumptionLimit"]["blocked"] is False

        # window_start = max(flag_time=3h ago, now-24h) = flag_time, window_end = now
        # during_block_time (2.5h ago) is within [flag_time, now): visible after expiry
        # old_time (5h ago) is before flag_time: excluded
        assert model_data["consumptionLimit"]["consumed"] == 800
        assert model_data["consumptionLimit"]["remaining"] > 0  # Under threshold

    async def test_consumption_still_recorded_when_blocked(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test that consumption is still recorded even when user is blocked."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            threshold=1000,
            consumption_range_hours=24
        )

        # Create flag record to block user
        now = datetime_now_sec()
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=0,
            flag=True,
            client_timestamp=int(now.timestamp() * 1000),
            timestamp=now
        ))

        # Act - try to record consumption while blocked
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {"model_id": str(model.id), "tokens_in": 40, "tokens_out": 2, "client_timestamp": int(datetime_now_sec().timestamp() * 1000)}
                ]
            }
        )

        # Assert - request succeeds
        assert response.status_code == 200

        # Verify consumption was recorded
        records = await test_engine.find(
            UserTokenConsumption,
            (UserTokenConsumption.user_id == user.email)
            & (UserTokenConsumption.model_id == model.id)
            & (UserTokenConsumption.flag != True)
        )
        assert len(records) == 1
        assert records[0].token_count == 50  # 5*2 + 40

    async def test_flag_updated_when_threshold_exceeded_after_unblock(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test that a NEW flag record is created when threshold is exceeded after unblocking."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            threshold=1000,
            consumption_range_hours=24
        )

        current_time = datetime_now_sec()
        old_flag_time = current_time - timedelta(hours=3)

        # Create old flag (3 hours ago, expired)
        old_flag = await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=0,
            flag=True,
            client_timestamp=int(old_flag_time.timestamp() * 1000),
            timestamp=old_flag_time
        ))
        old_flag_id = old_flag.id

        # Create new consumption that exceeds threshold AFTER the old flag
        recent_time = current_time - timedelta(minutes=30)
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=1200,
            client_timestamp=int(recent_time.timestamp() * 1000),
            timestamp=recent_time
        ))

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        assert response.status_code == 200
        data = response.json()
        model_data = next(m for m in data["config"]["models"] if m["name"] == "test-model")

        assert model_data["consumptionLimit"]["blocked"] is True

        # Verify TWO flag records exist: the original and the newly inserted one
        flag_records = await test_engine.find(
            UserTokenConsumption,
            (UserTokenConsumption.user_id == user.email)
            & (UserTokenConsumption.model_id == model.id)
            & (UserTokenConsumption.flag == True)
        )
        assert len(flag_records) == 2

        # Verify the new flag is a distinct document with a recent timestamp
        new_flag = max(flag_records, key=lambda r: r.timestamp)
        assert new_flag.id != old_flag_id          # brand-new document
        assert new_flag.timestamp > old_flag_time  # recent timestamp

    async def test_blocking_is_per_model(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test that blocking is per-user-per-model, not global."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model1 = await model_management_factory(
            name="model-1",
            threshold=1000,
            consumption_range_hours=24
        )
        model2 = await model_management_factory(
            name="model-2",
            threshold=1000,
            consumption_range_hours=24
        )

        # Exceed threshold for model1 only
        now = datetime_now_sec()
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model1.id,
            token_count=1100,
            client_timestamp=int(now.timestamp() * 1000),
            timestamp=now
        ))

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        data = response.json()
        model1_data = next(m for m in data["config"]["models"] if m["name"] == "model-1")
        model2_data = next(m for m in data["config"]["models"] if m["name"] == "model-2")

        # Model1 should be blocked
        assert model1_data["consumptionLimit"]["blocked"] is True

        # Model2 should NOT be blocked
        assert model2_data["consumptionLimit"]["blocked"] is False

    @freeze_time("2026-04-07 12:00:00")
    async def test_very_old_flag_uses_rolling_window(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test that very old flag uses max behavior to prevent unbounded accumulation."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            threshold=1000,
            consumption_range_hours=24  # 24-hour window
        )

        current_time = datetime(2026, 4, 7, 12, 0, 0, tzinfo=timezone.utc)

        # Create very old flag (48 hours ago, way older than 24h rolling window)
        old_flag_time = current_time - timedelta(hours=48)
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=0,
            flag=True,
            client_timestamp=int(old_flag_time.timestamp() * 1000),
            timestamp=old_flag_time
        ))

        # Create consumption 30 hours ago (within flag window but outside rolling window)
        old_consumption_time = current_time - timedelta(hours=30)
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=500,
            client_timestamp=int(old_consumption_time.timestamp() * 1000),
            timestamp=old_consumption_time
        ))

        # Create consumption 12 hours ago (within both windows)
        recent_consumption_time = current_time - timedelta(hours=12)
        await test_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=300,
            client_timestamp=int(recent_consumption_time.timestamp() * 1000),
            timestamp=recent_consumption_time
        ))

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={"consumption": []}
        )

        # Assert
        data = response.json()
        model_data = next(m for m in data["config"]["models"] if m["name"] == "test-model")

        # Not blocked (flag is 48h old)
        assert model_data["consumptionLimit"]["blocked"] is False

        # KEY BEHAVIOR: Uses max(flag_time, rolling_window) = rolling_window (24h ago)
        # So only consumption from last 24h counts: 300 (not 500+300=800)
        assert model_data["consumptionLimit"]["consumed"] == 300
        assert model_data["consumptionLimit"]["remaining"] > 0  # Under threshold


@pytest.mark.asyncio
@pytest.mark.integration
class TestConsumptionDeduplication:
    """Integration tests for consumption deduplication using client_timestamp."""

    async def test_valid_consumption_with_client_timestamp(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test consumption record with valid client_timestamp is accepted."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(name="test-model", threshold=1000)
        client_timestamp = int(datetime_now_sec().timestamp() * 1000)

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {
                        "model_id": str(model.id),
                        "tokens_in": 80,
                        "tokens_out": 4,
                        "client_timestamp": client_timestamp
                    }
                ]
            }
        )

        # Assert
        assert response.status_code == 200
        records = await test_engine.find(
            UserTokenConsumption,
            (UserTokenConsumption.user_id == user.email) & (UserTokenConsumption.model_id == model.id)
        )
        assert len(records) == 1
        assert records[0].token_count == 100  # 5*4 + 80

    async def test_missing_client_timestamp_returns_422(
        self,
        async_client: AsyncClient,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test consumption record without client_timestamp returns 422 validation error."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(name="test-model")

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {"model_id": str(model.id), "tokens_in": 80, "tokens_out": 4}  # Missing client_timestamp
                ]
            }
        )

        # Assert
        assert response.status_code == 422
        assert "client_timestamp" in response.text.lower()

    async def test_duplicate_submission_silently_skipped(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test duplicate consumption record is silently skipped."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(name="test-model")
        client_timestamp = int(datetime_now_sec().timestamp() * 1000)

        consumption_payload = {
            "consumption": [
                {
                    "model_id": str(model.id),
                    "tokens_in": 100,
                    "tokens_out": 10,
                    "client_timestamp": client_timestamp
                }
            ]
        }

        # Act - First submission
        response1 = await async_client.post("/api/user-model/configs", json=consumption_payload)
        assert response1.status_code == 200

        # Act - Duplicate submission
        response2 = await async_client.post("/api/user-model/configs", json=consumption_payload)

        # Assert - Both requests succeed
        assert response2.status_code == 200

        # Only one record should exist
        records = await test_engine.find(
            UserTokenConsumption,
            (UserTokenConsumption.user_id == user.email) & (UserTokenConsumption.model_id == model.id)
        )
        assert len(records) == 1
        assert records[0].token_count == 150  # 5*10 + 100

    async def test_partial_batch_with_duplicates(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test batch with mixed new and duplicate records stores only new records."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(name="test-model")
        # Use past timestamp to avoid future timestamp validation errors due to test delays
        timestamp1 = int((datetime_now_sec() - timedelta(seconds=10)).timestamp() * 1000)
        timestamp2 = timestamp1 + 1000

        # Act - First submission with timestamp1
        await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {
                        "model_id": str(model.id),
                        "tokens_in": 80,
                        "tokens_out": 4,
                        "client_timestamp": timestamp1
                    }
                ]
            }
        )

        # Act - Second submission with one duplicate and one new
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {
                        "model_id": str(model.id),
                        "tokens_in": 80,
                        "tokens_out": 4,
                        "client_timestamp": timestamp1  # Duplicate
                    },
                    {
                        "model_id": str(model.id),
                        "tokens_in": 160,
                        "tokens_out": 8,
                        "client_timestamp": timestamp2  # New
                    }
                ]
            }
        )

        # Assert
        if response.status_code != 200:
            print(f"Second submission error: {response.text}")
        assert response.status_code == 200
        records = await test_engine.find(
            UserTokenConsumption,
            (UserTokenConsumption.user_id == user.email) & (UserTokenConsumption.model_id == model.id)
        )
        assert len(records) == 2  # Only 2 records (first + new from second batch)
        token_counts = sorted([r.token_count for r in records])
        assert token_counts == [100, 200]

    async def test_different_timestamp_creates_new_record(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test consumption record with different client_timestamp creates new record."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(name="test-model")
        # Use past timestamps to avoid future timestamp validation errors due to test delays
        timestamp1 = int((datetime_now_sec() - timedelta(seconds=10)).timestamp() * 1000)
        timestamp2 = timestamp1 + 5000

        # Act - Two submissions with different timestamps
        await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {
                        "model_id": str(model.id),
                        "tokens_in": 80,
                        "tokens_out": 4,
                        "client_timestamp": timestamp1
                    }
                ]
            }
        )

        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {
                        "model_id": str(model.id),
                        "tokens_in": 80,
                        "tokens_out": 4,
                        "client_timestamp": timestamp2
                    }
                ]
            }
        )

        # Assert
        assert response.status_code == 200
        records = await test_engine.find(
            UserTokenConsumption,
            (UserTokenConsumption.user_id == user.email) & (UserTokenConsumption.model_id == model.id)
        )
        assert len(records) == 2  # Two separate records

    async def test_concurrent_duplicate_submissions(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test concurrent duplicate submissions - only one record is stored."""
        import asyncio

        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(name="test-model")
        client_timestamp = int(datetime_now_sec().timestamp() * 1000)

        payload = {
            "consumption": [
                {
                    "model_id": str(model.id),
                    "tokens_in": 80,
                    "tokens_out": 4,
                    "client_timestamp": client_timestamp
                }
            ]
        }

        # Act - Send concurrent requests with same data
        responses = await asyncio.gather(
            async_client.post("/api/user-model/configs", json=payload),
            async_client.post("/api/user-model/configs", json=payload),
            async_client.post("/api/user-model/configs", json=payload),
            return_exceptions=True
        )

        # Assert - All requests should succeed
        for response in responses:
            assert isinstance(response, object) and response.status_code == 200

        # Only one record should exist (database enforces uniqueness)
        records = await test_engine.find(
            UserTokenConsumption,
            (UserTokenConsumption.user_id == user.email) & (UserTokenConsumption.model_id == model.id)
        )
        assert len(records) == 1
        assert records[0].token_count == 100  # 5*4 + 80

    async def test_conversion_formula(
        self,
        async_client: AsyncClient,
        test_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test that token_count is computed correctly using formula: 5 * tokens_out + tokens_in."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(name="test-model")

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {
                        "model_id": str(model.id),
                        "tokens_in": 100,
                        "tokens_out": 20,
                        "client_timestamp": int(datetime_now_sec().timestamp() * 1000)
                    }
                ]
            }
        )

        # Assert
        assert response.status_code == 200
        records = await test_engine.find(
            UserTokenConsumption,
            (UserTokenConsumption.user_id == user.email) & (UserTokenConsumption.model_id == model.id)
        )
        assert len(records) == 1
        # Verify conversion formula: 5 * 20 + 100 = 200
        assert records[0].tokens_in == 100
        assert records[0].tokens_out == 20
        assert records[0].token_count == 200

    async def test_missing_tokens_in_field(
        self,
        async_client: AsyncClient,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test consumption record without tokens_in returns 422 validation error."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(name="test-model")

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {
                        "model_id": str(model.id),
                        "tokens_out": 10,
                        "client_timestamp": int(datetime_now_sec().timestamp() * 1000)
                    }
                ]
            }
        )

        # Assert
        assert response.status_code == 422
        assert "tokens_in" in response.text.lower()

    async def test_missing_tokens_out_field(
        self,
        async_client: AsyncClient,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test consumption record without tokens_out returns 422 validation error."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(name="test-model")

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {
                        "model_id": str(model.id),
                        "tokens_in": 100,
                        "client_timestamp": int(datetime_now_sec().timestamp() * 1000)
                    }
                ]
            }
        )

        # Assert
        assert response.status_code == 422
        assert "tokens_out" in response.text.lower()

    async def test_tokens_in_exceeds_maximum(
        self,
        async_client: AsyncClient,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test tokens_in exceeding 1,000,000 returns 422 validation error."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(name="test-model")

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {
                        "model_id": str(model.id),
                        "tokens_in": 2_000_000,
                        "tokens_out": 10,
                        "client_timestamp": int(datetime_now_sec().timestamp() * 1000)
                    }
                ]
            }
        )

        # Assert
        assert response.status_code == 422

    async def test_tokens_out_exceeds_maximum(
        self,
        async_client: AsyncClient,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test tokens_out exceeding 1,000,000 returns 422 validation error."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(name="test-model")

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {
                        "model_id": str(model.id),
                        "tokens_in": 100,
                        "tokens_out": 2_000_000,
                        "client_timestamp": int(datetime_now_sec().timestamp() * 1000)
                    }
                ]
            }
        )

        # Assert
        assert response.status_code == 422

    async def test_negative_tokens_in(
        self,
        async_client: AsyncClient,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test negative tokens_in returns 422 validation error."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(name="test-model")

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {
                        "model_id": str(model.id),
                        "tokens_in": -100,
                        "tokens_out": 10,
                        "client_timestamp": int(datetime_now_sec().timestamp() * 1000)
                    }
                ]
            }
        )

        # Assert
        assert response.status_code == 422

    async def test_negative_tokens_out(
        self,
        async_client: AsyncClient,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test negative tokens_out returns 422 validation error."""
        # Arrange
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(name="test-model")

        # Act
        response = await async_client.post(
            "/api/user-model/configs",
            json={
                "consumption": [
                    {
                        "model_id": str(model.id),
                        "tokens_in": 100,
                        "tokens_out": -10,
                        "client_timestamp": int(datetime_now_sec().timestamp() * 1000)
                    }
                ]
            }
        )

        # Assert
        assert response.status_code == 422
