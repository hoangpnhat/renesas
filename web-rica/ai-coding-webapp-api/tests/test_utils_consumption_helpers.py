#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

import pytest
from datetime import datetime, timedelta, timezone
from odmantic import ObjectId

from models.token_consumption import UserTokenConsumption
from crud.crud_token_consumption import token_consumption_crud
from crud.crud_model_management import model_crud


@pytest.mark.asyncio
@pytest.mark.integration
class TestAggregateConsumption:
    """Test cases for CRUDTokenConsumption.aggregate_consumption."""

    async def test_aggregate_with_records_in_window(
        self,
        override_get_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test aggregation with records within time window."""
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            threshold=1000,
            consumption_range_hours=24
        )

        from utils.helpers import datetime_now_sec

        base_time = datetime_now_sec() - timedelta(minutes=5)
        base_time_ms = int(base_time.timestamp() * 1000)
        records = [
            UserTokenConsumption(
                user_id=user.email,
                model_id=model.id,
                token_count=100,
                client_timestamp=base_time_ms,
                timestamp=base_time
            ),
            UserTokenConsumption(
                user_id=user.email,
                model_id=model.id,
                token_count=200,
                client_timestamp=base_time_ms + 1000,
                timestamp=base_time + timedelta(seconds=1)
            ),
            UserTokenConsumption(
                user_id=user.email,
                model_id=model.id,
                token_count=300,
                client_timestamp=base_time_ms + 2000,
                timestamp=base_time + timedelta(seconds=2)
            )
        ]
        await override_get_engine.save_all(records)

        total = await token_consumption_crud.aggregate_consumption(
            user_id=user.email,
            model_id=model.id,
            range_hours=24
        )

        assert total == 600

    async def test_aggregate_no_records_in_window(
        self,
        override_get_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """Test aggregation with no records in time window."""
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            threshold=1000,
            consumption_range_hours=1
        )

        old_timestamp = datetime.now(timezone.utc) - timedelta(hours=2)
        old_record = UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=500,
            client_timestamp=int(old_timestamp.timestamp() * 1000),
            timestamp=old_timestamp
        )
        await override_get_engine.save(old_record)

        total = await token_consumption_crud.aggregate_consumption(
            user_id=user.email,
            model_id=model.id,
            range_hours=1
        )

        assert total == 0


@pytest.mark.asyncio
@pytest.mark.integration
class TestGetModelsWithConsumptionConfig:
    """Test cases for CRUDModelManagement.get_with_consumption_config."""

    async def test_returns_only_models_with_config(
        self,
        override_get_engine,
        model_management_factory
    ):
        """Test that only models with threshold and range_hours are returned."""
        model_with_config = await model_management_factory(
            name="model-with-config",
            threshold=1000,
            consumption_range_hours=24
        )

        await model_management_factory(
            name="model-without-config",
            threshold=None,
            consumption_range_hours=None
        )

        await model_management_factory(
            name="model-partial-config",
            threshold=1000,
            consumption_range_hours=None
        )

        models = await model_crud.get_with_consumption_config()

        assert len(models) == 1
        assert models[0].id == model_with_config.id

    async def test_excludes_deleted_models(
        self,
        override_get_engine,
        model_management_factory
    ):
        """Test that deleted models are excluded."""
        from utils.helpers import datetime_now_sec

        await model_management_factory(
            name="active-model",
            threshold=1000,
            consumption_range_hours=24
        )

        await model_management_factory(
            name="deleted-model",
            threshold=1000,
            consumption_range_hours=24,
            deleted_at=datetime_now_sec()
        )

        models = await model_crud.get_with_consumption_config()

        assert len(models) == 1
        assert models[0].name == "active-model"


@pytest.mark.asyncio
@pytest.mark.integration
class TestAggregateConsumptionWindowStates:
    """Tests for the 5-state window logic in aggregate_consumption."""

    async def test_active_block_freezes_window_end(
        self,
        override_get_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """
        An active flag freezes window_end at flag.timestamp.
        Consumption added after the flag is excluded from the sum.
        """
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            threshold=10000,
            consumption_range_hours=24
        )

        now = datetime.now(timezone.utc)
        flag_time = now - timedelta(hours=1)  # active block (< 2h ago)

        await override_get_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=0,
            flag=True,
            client_timestamp=int(flag_time.timestamp() * 1000),
            timestamp=flag_time
        ))

        pre_flag_time = flag_time - timedelta(hours=1)
        await override_get_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=400,
            client_timestamp=int(pre_flag_time.timestamp() * 1000),
            timestamp=pre_flag_time
        ))

        post_flag_time = now - timedelta(minutes=30)
        await override_get_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=300,
            client_timestamp=int(post_flag_time.timestamp() * 1000),
            timestamp=post_flag_time
        ))

        total = await token_consumption_crud.aggregate_consumption(
            user_id=user.email,
            model_id=model.id,
            range_hours=24
        )

        assert total == 400

    async def test_two_flags_active_t1_is_floor(
        self,
        override_get_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """
        Two flags, flag_02 active, flag_01 is more recent than rolling start from flag_02.

        flag_01 = 6h ago, flag_02 = 30min ago (active), range=24h
        window_start = max(6h ago, flag_02 - 24h ≈ 24.5h ago) = 6h ago  (T1 wins)
        window_end   = flag_02.timestamp (frozen)
        consumption at 4h ago  → within [6h ago, flag_02.t) → included
        consumption at 8h ago  → before 6h ago → excluded
        """
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            threshold=10000,
            consumption_range_hours=24
        )

        now = datetime.now(timezone.utc)
        flag_01_time = now - timedelta(hours=6)
        flag_02_time = now - timedelta(minutes=30)

        for flag_time in (flag_01_time, flag_02_time):
            await override_get_engine.save(UserTokenConsumption(
                user_id=user.email,
                model_id=model.id,
                token_count=0,
                flag=True,
                client_timestamp=int(flag_time.timestamp() * 1000),
                timestamp=flag_time
            ))

        included_time = now - timedelta(hours=4)
        await override_get_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=500,
            client_timestamp=int(included_time.timestamp() * 1000),
            timestamp=included_time
        ))

        excluded_time = now - timedelta(hours=8)
        await override_get_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=700,
            client_timestamp=int(excluded_time.timestamp() * 1000),
            timestamp=excluded_time
        ))

        total = await token_consumption_crud.aggregate_consumption(
            user_id=user.email,
            model_id=model.id,
            range_hours=24
        )

        assert total == 500

    async def test_two_flags_active_rolling_dominates(
        self,
        override_get_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """
        Two flags, flag_02 active, rolling start from flag_02 is more recent than flag_01.

        flag_01 = 30h ago, flag_02 = 30min ago (active), range=24h
        window_start = max(30h ago, flag_02 - 24h ≈ 24.5h ago) = 24.5h ago  (rolling dominates)
        window_end   = flag_02.timestamp (frozen)
        consumption at 23h ago → within [24.5h ago, flag_02.t) → included
        consumption at 26h ago → before 24.5h ago → excluded
        """
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            threshold=10000,
            consumption_range_hours=24
        )

        now = datetime.now(timezone.utc)
        flag_01_time = now - timedelta(hours=30)
        flag_02_time = now - timedelta(minutes=30)

        for flag_time in (flag_01_time, flag_02_time):
            await override_get_engine.save(UserTokenConsumption(
                user_id=user.email,
                model_id=model.id,
                token_count=0,
                flag=True,
                client_timestamp=int(flag_time.timestamp() * 1000),
                timestamp=flag_time
            ))

        included_time = now - timedelta(hours=23)
        await override_get_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=600,
            client_timestamp=int(included_time.timestamp() * 1000),
            timestamp=included_time
        ))

        excluded_time = now - timedelta(hours=26)
        await override_get_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=800,
            client_timestamp=int(excluded_time.timestamp() * 1000),
            timestamp=excluded_time
        ))

        total = await token_consumption_crud.aggregate_consumption(
            user_id=user.email,
            model_id=model.id,
            range_hours=24
        )

        assert total == 600

    async def test_two_flags_expired_uses_latest_flag_as_floor(
        self,
        override_get_engine,
        user_factory,
        model_management_factory,
        override_auth_admin
    ):
        """
        Two flags, flag_02 expired, window_start = max(flag_02.timestamp, now - range_hours).

        flag_01 = 10h ago, flag_02 = 4h ago (expired), range=24h
        window_start = max(4h ago, now - 24h) = 4h ago  (flag_02 is floor)
        window_end   = now
        consumption at 2h ago → within [4h ago, now) → included
        consumption at 6h ago → before 4h ago → excluded
        """
        user = await user_factory(email=override_auth_admin.email)
        model = await model_management_factory(
            name="test-model",
            threshold=10000,
            consumption_range_hours=24
        )

        now = datetime.now(timezone.utc)
        flag_01_time = now - timedelta(hours=10)
        flag_02_time = now - timedelta(hours=4)  # expired (> 2h ago)

        for flag_time in (flag_01_time, flag_02_time):
            await override_get_engine.save(UserTokenConsumption(
                user_id=user.email,
                model_id=model.id,
                token_count=0,
                flag=True,
                client_timestamp=int(flag_time.timestamp() * 1000),
                timestamp=flag_time
            ))

        included_time = now - timedelta(hours=2)
        await override_get_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=450,
            client_timestamp=int(included_time.timestamp() * 1000),
            timestamp=included_time
        ))

        excluded_time = now - timedelta(hours=6)
        await override_get_engine.save(UserTokenConsumption(
            user_id=user.email,
            model_id=model.id,
            token_count=900,
            client_timestamp=int(excluded_time.timestamp() * 1000),
            timestamp=excluded_time
        ))

        total = await token_consumption_crud.aggregate_consumption(
            user_id=user.email,
            model_id=model.id,
            range_hours=24
        )

        assert total == 450
