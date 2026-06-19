from typing import Union
from odmantic import ObjectId
from caching.redis import get_redis_client, RedisClient
import asyncio
from loguru import logger
from dataclasses import dataclass

redis_cli: RedisClient = get_redis_client()

async def init_redis(service: RedisClient):
    if not service.client:
        await service.start()

@dataclass
class CachedManager:

    async def delete_cached_objects(self, objects: list[Union[str, ObjectId]], prefix: str = 'project') -> bool:
        """Delete associated cached objects: project, knowledge base."""
        try:
            await init_redis(redis_cli)
            delete_operations = [redis_cli.delete(f"{prefix}-{str(obj)}") for obj in objects]
            await asyncio.gather(*delete_operations)
            logger.info(f"Cleared {len(objects)} cached {prefix} objects")
            return True
        except Exception as e:
            logger.error(f"Error clearing cache: {str(e)}")
            return False