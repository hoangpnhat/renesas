#  Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
import asyncio
import hashlib
import json
import re
from typing import Any, Dict, Optional
from redis.asyncio import Redis

import orjson
from fastapi import HTTPException
from motor.core import AgnosticDatabase
from redis import asyncio as aioredis

from core.loguru import logger
from core.config import settings
from utils.data_handle import serialize_data


def is_valid_azure_redis_conn_str(conn_str: str) -> bool:
    """
    Checks whether the provided string is in the expected format for an Azure Redis
    connection string (e.g. "hostname:port,password=...,ssl=...,abortConnect=...").

    Returns:
      True if the string appears valid, False otherwise.
    """
    # Ensure the connection string is not empty
    if not conn_str or not isinstance(conn_str, str):
        return False

    # Split the connection string by commas.
    parts = conn_str.split(',')
    if len(parts) < 2:
        # At least one host:port and one key=value pair is expected.
        return False

    # Check the first part for host:port format.
    host_port = parts[0].strip()
    if ':' not in host_port:
        return False
    hostname, port = host_port.split(':', 1)
    if not hostname:
        return False
    if not port.isdigit():
        return False

    # Check remaining parts are in key=value format.
    for part in parts[1:]:
        part = part.strip()
        if '=' not in part:
            return False
        key, value = part.split('=', 1)
        if not key or not value:
            return False

    return True
def convert_azure_redis_to_uri(valid_conn_str: str) -> str:
    """
    Convert an Azure Redis valid connection string into a URI connection string.

    Example:
      Input: "yourcache.redis.cache.windows.net:6380,password=YourPassword,ssl=True,abortConnect=False"
      Output: "rediss://:YourPassword@yourcache.redis.cache.windows.net:6380"

    Parameters:
      valid_conn_str (str): The connection string from Azure Redis in valid format.

    Returns:
      str: A URI-formatted connection string.

    Raises:
      ValueError: If the input string is not in the expected format.
    """
    # Split the string by commas.
    parts = valid_conn_str.split(',')
    if not parts:
        raise ValueError("The connection string is empty or not properly formatted.")

    # The first part should be the hostname and port in the format "hostname:port"
    host_port = parts[0].strip()
    if ':' not in host_port:
        raise ValueError("Expected 'hostname:port' in the first part of the connection string.")
    hostname, port = host_port.split(':', 1)

    # Process remaining key=value pairs
    params = {}
    for part in parts[1:]:
        if '=' in part:
            key, value = part.split('=', 1)
            params[key.strip().lower()] = value.strip()

    # Extract the password if available
    password = params.get('password', '')

    # Determine if SSL is enabled (default to False if not specified)
    ssl_enabled = params.get('ssl', 'false').lower() == 'true'
    scheme = 'rediss' if ssl_enabled else 'redis'

    # Construct the URI: note that the password segment starts with a colon if password is non-empty.
    password_segment = f":{password}" if password else ""
    uri = f"{scheme}://{password_segment}@{hostname}:{port}"
    logger.info(f"URI: {uri}" )
    return uri

def normalize_redis_uri(redis_uri: str) -> str:
    if is_valid_azure_redis_conn_str(redis_uri):
        return convert_azure_redis_to_uri(redis_uri)
    return redis_uri


def parse_redis_connection_string(conn_str):
    # Extract host and port
    host_port = conn_str.split(",")[0]
    host, port = host_port.split(":")

    # Extract password
    password_match = re.search(r"password=([^,]+)", conn_str)
    password = password_match.group(1) if password_match else None

    return {
        "host": host,
        "port": int(port),
        "password": password,
        "ssl": True,
        "decode_responses": True,
    }


class RedisClient:
    def __init__(self, redis_url: str):
        self.redis_url = redis_url
        self.prefix = f"{settings.api_app.APP_NAME}-{settings.api_app.ENV}"
        self.counter_key = f"{self.prefix}:counter:{hashlib.sha256(self.redis_url.encode()).hexdigest()}"
        self.training_qa_counter_key = f"{self.prefix}:counter-training_qa:{hashlib.sha256(self.redis_url.encode()).hexdigest()}"
        self.client: Redis[Any] | None = None

    async def cleanup_keys_by_prefix(self) -> int:
        """
        Best solution: Atomic cleanup using Lua script with fallback
        Returns number of deleted keys
        """
        if not self.client:
            return 0

        # Lua script for atomic SCAN + DELETE
        lua_cleanup_script = """
        local pattern = ARGV[1]
        local batch_size = tonumber(ARGV[2])
        local cursor = "0"
        local total_deleted = 0
        local iterations = 0
        local max_iterations = 1000

        repeat
            local result = redis.call('SCAN', cursor, 'MATCH', pattern, 'COUNT', batch_size)
            cursor = result[1]
            local keys = result[2]

            if #keys > 0 then
                local deleted = redis.call('DEL', unpack(keys))
                total_deleted = total_deleted + deleted
            end

            iterations = iterations + 1
        until cursor == "0" or iterations >= max_iterations

        return total_deleted
        """

        try:
            # Execute Lua script
            deleted_count = await asyncio.wait_for(
                self.client.eval(
                    lua_cleanup_script,
                    0,  # No keys in KEYS array
                    f"{self.prefix}*",  # Pattern
                    "100",  # Batch size
                ),
                timeout=30.0,
            )

            logger.info(f"Deleted {deleted_count} keys with prefix '{self.prefix}'")
            return int(deleted_count)

        except asyncio.TimeoutError:
            logger.warning("Lua script timeout, using fallback method")
            return await self._fallback_cleanup()
        except Exception as e:
            logger.error(f"Lua script failed: {str(e)}, using fallback")
            return await self._fallback_cleanup()

    async def _fallback_cleanup(self) -> int:
        """Fallback cleanup method"""
        cursor = 0
        total_deleted = 0

        if self.client is None:
            raise RuntimeError("Redis client is not initialised")
        try:
            while True:
                cursor, keys = await asyncio.wait_for(
                    self.client.scan(cursor=cursor, match=f"{self.prefix}*", count=50),
                    timeout=5.0,
                )

                if keys:
                    deleted = await asyncio.wait_for(
                        self.client.delete(*keys), timeout=5.0
                    )
                    total_deleted += deleted

                if cursor == 0:
                    break

            return total_deleted

        except Exception as e:
            logger.error(f"Fallback cleanup failed: {str(e)}")
            return total_deleted

    async def start(self):
        if not self.client:
            try:
                self.client = await aioredis.from_url(
                    self.redis_url, encoding="utf-8", decode_responses=True
                )
            except Exception as e:
                logger.error(str(e))
                redis_params = parse_redis_connection_string(self.redis_url)
                self.client = await aioredis.Redis(**redis_params)

            await self.ping()

    async def close(self):
        if self.client:
            await self.cleanup_keys_by_prefix()
            await self.client.close()
            logger.info(
                f'Deleted all cached keys with prefix "{self.prefix}" and shutdown Redis Client'
            )

    async def get(self, key: str) -> Optional[dict]:
        if self.client is None:
            raise RuntimeError("Redis client is not initialised")
        try:
            key = f"{self.prefix}:{key}"
            value = await self.client.get(key)
            return orjson.loads(value) if value else None

        except (ConnectionError, TimeoutError) as e:
            logger.error(f"Redis operation failed: {str(e)}")
            raise HTTPException(
                status_code=503, detail=f"Redis operation failed: {str(e)}"
            )
        except json.JSONDecodeError as e:
            logger.error(f"Invalid JSON in Redis key {key}: {str(e)}")
            return None  # or raise an exception based on your needs

    async def ping(self) -> bool:
        if self.client is None:
            raise RuntimeError("Redis client is not initialised")
        try:
            return bool(await self.client.ping())
        except (ConnectionError, TimeoutError) as e:
            logger.error(f"Redis connection failed: {str(e)}")
            raise HTTPException(
                status_code=500, detail=f"Redis connection failed: {str(e)}"
            )

    async def set(self, key: str, value: Any, expire: int | None = None) -> None:
        """
        Set a key-value pair in Redis with an optional expiration time.
        :param key: str - The key to set in Redis.
        :param value: Any - The value to associate with the key.
        :param expire: int - Optional expiration time in seconds. If None, the key will not expire.
        :return: None
        """
        if self.client is None:
            raise RuntimeError("Redis client is not initialised")
        key = f"{self.prefix}:{key}"
        value = serialize_data(data=value)
        if expire:
            await self.client.setex(key, expire, value)
        else:
            await self.client.set(key, value)

    async def delete(self, key: str):
        if self.client is None:
            raise RuntimeError("Redis client is not initialised")
        key = f"{self.prefix}:{key}"
        await self.client.delete(key)

    async def clear_cache_by_prefix(self, outer_prefix: str) -> int:
        """
        Clear all Redis keys that match the given prefix.

        Args:
            outer_prefix: The prefix pattern to match keys for deletion (without the client prefix)

        Returns:
            int: Number of keys deleted

        Usage:
            await redis_client.clear_cache_by_prefix("user_data")
            # This will delete keys like: {APP_NAME}-{ENV}:user_data:*
        """
        if self.client is None:
            raise RuntimeError("Redis client is not initialised")
        try:
            pattern = f"{self.prefix}:{outer_prefix}:*"
            cursor = 0
            keys_to_delete = []

            while True:
                cursor, keys = await self.client.scan(cursor, pattern, 100)
                keys_to_delete.extend(keys)
                if cursor == 0:
                    break

            if keys_to_delete:
                await self.client.delete(*keys_to_delete)
                logger.info(
                    f"Cleared {len(keys_to_delete)} cache entries with prefix '{outer_prefix}'"
                )
                return len(keys_to_delete)
            else:
                logger.debug(f"No cache entries found with prefix '{outer_prefix}'")
                return 0

        except (ConnectionError, TimeoutError) as e:
            logger.error(
                f"Redis operation failed while clearing prefix '{outer_prefix}': {str(e)}"
            )
            raise HTTPException(
                status_code=503, detail=f"Redis operation failed: {str(e)}"
            )
        except Exception as e:
            logger.error(f"Error clearing cache with prefix '{outer_prefix}': {str(e)}")
            return 0

    async def _get_max_numbering_id_from_db(
            self, db: AgnosticDatabase, collection_name: str
    ) -> int:
        """
        Retrieve the maximum numbering_id from the specified collection.
        :param db: AgnosticDatabase - The database instance to query.
        :param collection_name: str - The name of the collection to query.
        :return: int - The maximum numbering_id found, or 0 if none exist.
        """
        pipeline = [
            {"$match": {"numbering_id": {"$type": ["int"]}}},
            {"$group": {"_id": None, "max_id": {"$max": "$numbering_id"}}},
        ]
        collection = getattr(db, collection_name)
        result = await collection.aggregate(pipeline).to_list(length=1)
        return int(result[0]["max_id"]) if result else 0

    async def _get_next_ids(
            self, db, counter_key: str, collection_name: str, count: int = 1
    ) -> list[int]:
        """
        Generic method to get next ID(s) for any counter with atomic Redis operations.
        Supports both single and batch operations.
        Only queries database if Redis key doesn't exist.

        Args:
            db: Database connection
            counter_key: Redis key for the counter
            collection_name: Database collection name to get max ID from
            count: Number of IDs to generate (default: 1)

        Returns:
            List of consecutive IDs (length 1 for single operations)
        """
        if count == 1:
            # Single ID operation - check existence first
            lua_script_incr = """
            local key = KEYS[1]
            local ttl = ARGV[1]

            if redis.call('EXISTS', key) == 1 then
                local new_val = redis.call('INCR', key)
                -- Only set expire if it doesn't have one
                if redis.call('TTL', key) == -1 then
                    redis.call('EXPIRE', key, ttl)
                end
                return new_val
            else
                return -1
            end
            """

            lua_script_init = """
            local key = KEYS[1]
            local ttl = ARGV[1]
            local init_value = ARGV[2]

            -- Atomic check-and-set-and-increment
            if redis.call('SETNX', key, init_value) == 1 then
                redis.call('EXPIRE', key, ttl)
                return redis.call('INCR', key)
            else
                -- Someone else initialized, just increment
                local new_val = redis.call('INCR', key)
                if redis.call('TTL', key) == -1 then
                    redis.call('EXPIRE', key, ttl)
                end
                return new_val
            end
            """
        else:
            # Batch operation - check existence first
            lua_script_incr = """
            local key = KEYS[1]
            local ttl = tonumber(ARGV[1])
            local count = tonumber(ARGV[2])

            if redis.call('EXISTS', key) == 1 then
                -- Counter exists, reserve 'count' numbers
                local current_val = tonumber(redis.call('GET', key))
                redis.call('INCRBY', key, count)

                -- Refresh TTL if needed
                if redis.call('TTL', key) <= 60 then
                    redis.call('EXPIRE', key, ttl)
                end

                return current_val
            else
                return -1
            end
            """

            lua_script_init = """
            local key = KEYS[1]
            local ttl = tonumber(ARGV[1])
            local count = tonumber(ARGV[2])
            local init_value = tonumber(ARGV[3])

            -- Atomic check-and-set-and-increment
            if redis.call('SETNX', key, init_value) == 1 then
                redis.call('SET', key, init_value + count)
                redis.call('EXPIRE', key, ttl)
                return init_value
            else
                -- Someone else initialized, just increment
                local current_val = tonumber(redis.call('GET', key))
                redis.call('INCRBY', key, count)
                if redis.call('TTL', key) == -1 then
                    redis.call('EXPIRE', key, ttl)
                end
                return current_val
            end
            """

        if self.client is None:
            raise RuntimeError("Redis client is not initialised")
        try:
            if count == 1:
                # 1️⃣ Try to increment if exists
                result = await self.client.eval(
                    lua_script_incr,
                    1,
                    counter_key,
                    settings.REDIS_COUNTER_EXPIRE_SECONDS,
                )
                if result != -1:
                    return [int(result)]

                # 2️⃣ Initialize atomically (only query DB when needed)
                max_id = await self._get_max_numbering_id_from_db(db, collection_name)
                result = await self.client.eval(
                    lua_script_init,
                    1,
                    counter_key,
                    settings.REDIS_COUNTER_EXPIRE_SECONDS,
                    str(max_id),
                )
                return [int(result)]
            else:
                # 1️⃣ Try batch increment if exists
                result = await self.client.eval(
                    lua_script_incr,
                    1,
                    counter_key,
                    settings.REDIS_COUNTER_EXPIRE_SECONDS,
                    str(count),
                )
                if result != -1:
                    # Generate consecutive IDs
                    starting_id = int(result) + 1
                    ids = list(range(starting_id, starting_id + count))
                    logger.info(
                        f"Generated {count} ID(s) for {collection_name}: {ids[0]}-{ids[-1]}"
                    )
                    return ids

                # 2️⃣ Initialize atomically (only query DB when needed)
                max_id = await self._get_max_numbering_id_from_db(db, collection_name)
                current_counter = await self.client.eval(
                    lua_script_init,
                    1,
                    counter_key,
                    settings.REDIS_COUNTER_EXPIRE_SECONDS,
                    str(count),
                    str(max_id),
                )

                # Generate consecutive IDs
                starting_id = int(current_counter) + 1
                ids = list(range(starting_id, starting_id + count))
                logger.info(
                    f"Generated {count} ID(s) for {collection_name}: {ids[0]}-{ids[-1]}"
                )
                return ids

        except Exception as e:
            logger.error(
                f"ID generation failed for {collection_name} (count={count}): {str(e)}"
            )

            # Fallback: recursive single calls
            if count == 1:
                # For single ID, we can't really fallback further
                raise
            else:
                # For batch, fallback to individual generation
                ids = []
                for _ in range(count):
                    single_id = await self._get_next_ids(
                        db, counter_key, collection_name, 1
                    )
                    ids.extend(single_id)
                return ids

    # Single ID methods (return int for backward compatibility)
    async def next_message_id(self, db) -> int:
        """Get the next message numbering ID."""
        ids = await self._get_next_ids(db, self.counter_key, "message_content", 1)
        return ids[0]

    async def next_training_qa_id(self, db) -> int:
        """Get the next training QA numbering ID."""
        ids = await self._get_next_ids(
            db, self.training_qa_counter_key, "training_qa", 1
        )
        return ids[0]

    # Batch methods (return list[int])
    async def batch_next_message_ids(self, db, count: int) -> list[int]:
        """Generate multiple consecutive message IDs in one atomic operation."""
        return await self._get_next_ids(db, self.counter_key, "message_content", count)

    async def batch_next_training_qa_ids(self, db, count: int) -> list[int]:
        """Generate multiple consecutive training QA IDs in one atomic operation."""
        return await self._get_next_ids(
            db, self.training_qa_counter_key, "training_qa", count
        )


_redis_client: RedisClient | None = None


def get_redis_client() -> RedisClient:
    global _redis_client
    if _redis_client is None:
        # raise RuntimeError("Redis client is not initialized")
        _redis_client = RedisClient(settings.REDIS_URI or "")
    return _redis_client


def generate_cache_key(prefix: str, params: Dict[str, Any]) -> str:
    """
    Generate a stable cache key based on the prefix and parameters using hashing.
    :param prefix: str - The prefix for the cache key.
    :param params: dict - The parameters to include in the cache key.
    :return: str - The generated cache key.
    """
    if not isinstance(params, dict):
        raise ValueError("params must be a dictionary")

    try:
        # Convert to JSON with sorted keys for consistent ordering
        # This handles nested dicts, lists, and most JSON-serializable types
        json_str = json.dumps(
            params, sort_keys=True, separators=(",", ":"), ensure_ascii=True
        )

        # Create SHA-256 hash of the JSON string
        hash_obj = hashlib.sha256(json_str.encode("utf-8"))
        hash_hex = hash_obj.hexdigest()

        # Return prefix with truncated hash for readability while maintaining uniqueness
        return f"{prefix}:{hash_hex[:16]}"

    except (TypeError, ValueError) as e:
        # Fallback for non-JSON-serializable objects
        # Convert values to strings and sort by keys
        try:
            sorted_items = sorted((str(k), str(v)) for k, v in params.items())
            joined = "&".join(f"{k}={v}" for k, v in sorted_items)

            # Hash the joined string
            hash_obj = hashlib.sha256(joined.encode("utf-8"))
            hash_hex = hash_obj.hexdigest()

            return f"{prefix}:{hash_hex[:16]}"

        except Exception:
            raise ValueError(f"Unable to generate cache key from params: {e}")

_redis_client = RedisClient(settings.REDIS_URI or "")

# @asynccontextmanager
# async def init_redis(app: FastAPI, redis_url: str):
#     global _redis_client
#     _redis_client = RedisClient(redis_url)
#     await _redis_client.init_redis()

#     yield
#     await _redis_client.close()