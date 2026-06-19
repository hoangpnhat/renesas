from zoneinfo import ZoneInfo

from pymongo.driver_info import DriverInfo

from __version__ import __version__
from motor import motor_asyncio, core
from odmantic import AIOEngine, Model, EmbeddedModel
from core.config import settings
import certifi
from core.loguru import logger
from datetime import datetime
from typing import TypeVar, Optional, Type, Any, cast

DRIVER_INFO = DriverInfo(name=settings.mongodb.DATABASE_NAME or "", version=__version__)

T = TypeVar("T", bound=Model)

class TimezoneAwareEngine(AIOEngine):
    def __init__(self, *args, timezone: str = "UTC", **kwargs):
        super().__init__(*args, **kwargs)
        self._tzinfo = ZoneInfo(timezone)

    def _localize_model(self, obj: Any):
        if isinstance(obj, datetime):
            return obj.replace(tzinfo=self._tzinfo) if obj.tzinfo is None else obj.astimezone(self._tzinfo)

        elif isinstance(obj, (Model, EmbeddedModel)):
            for field_name, field_value in obj.__dict__.items():
                # Avoid modifying primary key
                if field_name in ("id", "_id"):
                    continue
                localized = self._localize_model(field_value)
                # Only set if changed
                if localized is not field_value:
                    setattr(obj, field_name, localized)
            return obj

        elif isinstance(obj, list):
            return [self._localize_model(item) for item in obj]

        elif isinstance(obj, dict):
            return {k: self._localize_model(v) for k, v in obj.items()}

        return obj

    async def find_one(  # type: ignore[override]
            self,
            model_type: Type[T],
            query: Any = None,
            *,
            sort: Optional[list] = None,
            session=None,
            **kwargs
    ) -> Optional[T]:
        result = await super().find_one(model_type, query, sort=sort, session=session)
        if result:
            return cast(T, self._localize_model(result))
        return result

    async def find(  # type: ignore[override]
            self,
            model_type: Type[T],
            query: Any = None,
            *,
            sort: Optional[list] = None,
            limit: Optional[int] = None,
            skip: int = 0,
            session=None,
            **kwargs
    ) -> list[T]:
        results = await super().find(
            model_type,
            query,
            sort=sort,
            limit=limit,
            skip=skip,
            session=session
        )
        return [self._localize_model(r) for r in results]


class _MongoClientSingleton:
    mongo_client: motor_asyncio.AsyncIOMotorClient | None
    engine: TimezoneAwareEngine

    def __new__(cls):
        if not hasattr(cls, "instance"):
            cls.instance = super(_MongoClientSingleton, cls).__new__(cls)
            cls.instance.mongo_client = motor_asyncio.AsyncIOMotorClient(
                settings.mongodb.MONGO_URI, driver=DRIVER_INFO, tlsCAFile=certifi.where()
            )
            cls.instance.engine = TimezoneAwareEngine(
                client=cls.instance.mongo_client, database=settings.mongodb.DATABASE_NAME
            )

        return cls.instance


def MongoDatabase() -> core.AgnosticDatabase:
    client = _MongoClientSingleton().mongo_client
    assert client is not None
    return client[settings.mongodb.DATABASE_NAME or ""]


def get_engine() -> AIOEngine | TimezoneAwareEngine:
    return _MongoClientSingleton().engine


async def ping():
    await MongoDatabase().command("ping")


def get_db():
    return db


async def check_health_db():
    try:
        engine = get_engine()
        # Perform a simple operation to check responsiveness
        await engine.client.server_info()
        logger.info("Database connection successful and responsive.")

        return "OK"
    except Exception as e:
        logger.error(f"Database health check failed: {str(e)}")
        return "Failed"


db = MongoDatabase()
__all__ = ["MongoDatabase", "ping"]
