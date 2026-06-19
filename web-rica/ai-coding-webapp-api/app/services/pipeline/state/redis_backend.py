"""
Redis backend for pipeline state storage

Provides persistent state storage with TTL support for tracking
page processing status across pipeline stages.
"""

import json
from typing import Any, Optional, List, cast
from loguru import logger


class RedisStateBackend:
    """Redis backend for pipeline state storage"""

    def __init__(self):
        self._client = None

    async def _get_client(self):
        if self._client is None:
            from caching.redis import get_redis_client
            self._client = get_redis_client()
            if not self._client.client:
                await self._client.start()
        return self._client

    async def set(self, key: str, value: dict, ttl: int = 86400):
        """
        Set value with optional TTL.

        Args:
            key: Redis key
            value: Dict to store
            ttl: Time to live in seconds (default 24 hours)
        """
        try:
            client = await self._get_client()
            serialized = json.dumps(value, default=str)
            await client.client.setex(key, ttl, serialized)
        except Exception as e:
            logger.error(f"Redis set failed for key {key}: {e}")
            raise

    async def get(self, key: str) -> Optional[dict]:
        """
        Get value by key.

        Args:
            key: Redis key

        Returns:
            Dict or None if not found
        """
        try:
            client = await self._get_client()
            data = await client.client.get(key)
            if data:
                if isinstance(data, bytes):
                    data = data.decode('utf-8')
                return cast(dict[Any, Any], json.loads(data))
            return None
        except Exception as e:
            logger.error(f"Redis get failed for key {key}: {e}")
            return None

    async def delete(self, key: str):
        """Delete a key"""
        try:
            client = await self._get_client()
            await client.client.delete(key)
        except Exception as e:
            logger.error(f"Redis delete failed for key {key}: {e}")

    async def scan(self, pattern: str) -> List[str]:
        """
        Scan keys matching pattern.

        Args:
            pattern: Redis key pattern (e.g., "pipeline:*:page:*")

        Returns:
            List of matching keys
        """
        try:
            client = await self._get_client()
            keys = []
            cursor = 0

            while True:
                cursor, found_keys = await client.client.scan(
                    cursor=cursor,
                    match=pattern,
                    count=100
                )
                keys.extend([
                    k.decode() if isinstance(k, bytes) else k
                    for k in found_keys
                ])
                if cursor == 0:
                    break

            return keys
        except Exception as e:
            logger.error(f"Redis scan failed for pattern {pattern}: {e}")
            return []

    async def delete_pattern(self, pattern: str):
        """Delete all keys matching pattern"""
        try:
            keys = await self.scan(pattern)
            if keys:
                client = await self._get_client()
                await client.client.delete(*keys)
                logger.debug(f"Deleted {len(keys)} keys matching {pattern}")
        except Exception as e:
            logger.error(f"Redis delete_pattern failed for {pattern}: {e}")

    async def incr(self, key: str) -> int:
        """Atomic increment"""
        try:
            client = await self._get_client()
            return int(await client.client.incr(key))
        except Exception as e:
            logger.error(f"Redis incr failed for key {key}: {e}")
            raise

    async def decr(self, key: str) -> int:
        """Atomic decrement"""
        try:
            client = await self._get_client()
            return int(await client.client.decr(key))
        except Exception as e:
            logger.error(f"Redis decr failed for key {key}: {e}")
            raise

    async def expire(self, key: str, ttl: int):
        """Set expiration on a key"""
        try:
            client = await self._get_client()
            await client.client.expire(key, ttl)
        except Exception as e:
            logger.error(f"Redis expire failed for key {key}: {e}")
