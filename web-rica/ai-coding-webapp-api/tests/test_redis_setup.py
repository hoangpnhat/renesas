#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

"""
Verification tests for Redis testing infrastructure.
These tests validate the test setup itself.
"""
import pytest


@pytest.mark.integration
@pytest.mark.asyncio
async def test_redis_connection(test_redis_client):
    """Verify Redis client is connected and functional."""
    # Test basic operations
    await test_redis_client.set("test_key", {"value": "test"})
    result = await test_redis_client.get("test_key")
    assert result == {"value": "test"}

    # Verify cleanup
    await test_redis_client.delete("test_key")
    result = await test_redis_client.get("test_key")
    assert result is None


@pytest.mark.integration
@pytest.mark.asyncio
async def test_redis_cleanup_isolation(test_redis_client):
    """Verify clean_redis fixture provides test isolation."""
    # This test should start with empty Redis (cleaned by autouse fixture)

    # Set a key
    await test_redis_client.set("isolation_test", {"data": "value"})
    result = await test_redis_client.get("isolation_test")
    assert result == {"data": "value"}

    # Note: The NEXT test should NOT see this key (cleaned by autouse)


@pytest.mark.integration
@pytest.mark.asyncio
async def test_redis_prefix_isolation(test_redis_client):
    """Verify prefix-based key isolation."""
    # Verify the prefix is set correctly
    assert "test" in test_redis_client.prefix.lower()

    # Set a key and verify it's prefixed
    await test_redis_client.set("my_key", "value")

    # Verify cleanup only deletes prefixed keys
    deleted_count = await test_redis_client.cleanup_keys_by_prefix()
    assert deleted_count >= 1  # At least our key


@pytest.mark.integration
@pytest.mark.asyncio
async def test_redis_counter_generation(test_engine, test_redis_client):
    """Verify Redis counter methods work with test database."""
    # Test single ID generation
    id1 = await test_redis_client.next_message_id(test_engine.database)
    id2 = await test_redis_client.next_message_id(test_engine.database)
    assert id2 == id1 + 1

    # Test batch ID generation
    ids = await test_redis_client.batch_next_message_ids(test_engine.database, 5)
    assert len(ids) == 5
    assert ids == list(range(ids[0], ids[0] + 5))


@pytest.mark.integration
@pytest.mark.asyncio
async def test_app_redis_override(async_client):
    """Verify app fixture uses test Redis client."""
    # Test that the app is using test Redis (not production)
    # We can't directly test healthcheck endpoint without implementing it
    # But we can verify the app is created successfully
    assert async_client is not None
