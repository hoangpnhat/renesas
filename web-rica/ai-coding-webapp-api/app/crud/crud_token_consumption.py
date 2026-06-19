#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

from datetime import datetime, timedelta, timezone
from typing import Any, Optional
from odmantic import ObjectId, query
from odmantic.exceptions import DuplicateKeyError as ODMDuplicateKeyError
from pymongo.errors import BulkWriteError

from models.token_consumption import UserTokenConsumption
from schemas.token_consumption import TokenConsumptionCreate, TokenConsumptionUpdate
from .refactor_base.crud_operations import MongoCRUD
from core.loguru import logger
from utils.helpers import datetime_to_unix_ms, normalize_id


class CRUDTokenConsumption(MongoCRUD[UserTokenConsumption, TokenConsumptionCreate, TokenConsumptionUpdate]):

    async def get_by_id(self, id: str | ObjectId) -> UserTokenConsumption | None:
        return await self.single_complex_get(to_query=(UserTokenConsumption.id == normalize_id(id)))  # type: ignore[arg-type]

    async def bulk_create(
        self,
        consumption_records: list[TokenConsumptionCreate],
        session: Optional[Any] = None
    ) -> list[UserTokenConsumption]:
        """
        Bulk insert consumption records with deduplication support.

        Silently skips duplicate records (same user_id, model_id, client_timestamp)
        and logs them at INFO level for monitoring. Returns successfully inserted records.

        For batches with duplicates, inserts records individually to handle partial success.

        Args:
            consumption_records: List of consumption records to insert
            session: Optional database session for transaction support

        Returns:
            List of created UserTokenConsumption instances (may be fewer than input if duplicates found)
        """
        if not consumption_records:
            return []

        db_objects = [
            UserTokenConsumption(**record.model_dump())
            for record in consumption_records
        ]

        # Try bulk insert first (most efficient for non-duplicate batches)
        try:
            if session:
                result = await self.engine.save_all(db_objects, session=session)
            else:
                result = await self.engine.save_all(db_objects)
            return result

        except (BulkWriteError, ODMDuplicateKeyError):
            # save_all raises odmantic's DuplicateKeyError (not BulkWriteError) when a
            # unique index is violated, because it calls _save() per-instance internally.
            # Insert one by one so non-duplicate records in the batch still succeed.
            successfully_inserted = []
            duplicate_count = 0

            for db_obj in db_objects:
                try:
                    if session:
                        saved_record = await self.engine.save(db_obj, session=session)
                    else:
                        saved_record = await self.engine.save(db_obj)
                    successfully_inserted.append(saved_record)
                except ODMDuplicateKeyError:
                    duplicate_count += 1

            # Log duplicate detection at INFO level
            if duplicate_count > 0:
                logger.info(
                    f"Skipped {duplicate_count} duplicate consumption record(s) "
                    f"from batch of {len(consumption_records)}"
                )

            return successfully_inserted

    async def get_flag_record(
        self,
        user_id: str,
        model_id: ObjectId
    ) -> Optional[UserTokenConsumption]:
        return await self.engine.find_one(
            UserTokenConsumption,
            (UserTokenConsumption.user_id == user_id)
            & (UserTokenConsumption.model_id == model_id)
            & (UserTokenConsumption.flag == True),
            sort=query.desc(UserTokenConsumption.timestamp)
        )

    async def get_flag_records(
        self,
        user_id: str,
        model_id: ObjectId,
        limit: int = 2
    ) -> list[UserTokenConsumption]:
        records = await self.engine.find(
            UserTokenConsumption,
            (UserTokenConsumption.user_id == user_id)
            & (UserTokenConsumption.model_id == model_id)
            & (UserTokenConsumption.flag == True),
            sort=query.desc(UserTokenConsumption.timestamp),
            limit=limit
        )
        return list(records)

    async def aggregate_consumption(
        self,
        user_id: str,
        model_id: ObjectId,
        range_hours: int
    ) -> int:
        """
        Aggregate token consumption for a user-model pair over a state-dependent time window.

        Computes (window_start, window_end) based on one of 5 states derived from the two
        most recent flag records. See CLAUDE.md token consumption section for full state table.
        """
        flag_records = await self.get_flag_records(user_id, model_id, limit=2)
        now = datetime.now(timezone.utc)
        two_hours_ago = now - timedelta(hours=2)

        if len(flag_records) == 0:
            window_start = now - timedelta(hours=range_hours)
            window_end = now
        elif len(flag_records) == 1:
            flag = flag_records[0]
            if flag.timestamp >= two_hours_ago:  # active block — freeze window_end
                window_end = flag.timestamp
                window_start = flag.timestamp - timedelta(hours=range_hours)
            else:  # expired block — rolling window anchored at now, floored by flag
                window_end = now
                window_start = max(flag.timestamp, now - timedelta(hours=range_hours))
        else:  # >= 2 flags
            flag_02 = flag_records[0]  # most recent
            flag_01 = flag_records[1]  # previous
            if flag_02.timestamp >= two_hours_ago:  # active block — freeze window_end
                window_end = flag_02.timestamp
                window_start = max(flag_01.timestamp, flag_02.timestamp - timedelta(hours=range_hours))
            else:  # expired block — rolling window anchored at now, floored by latest flag
                window_end = now
                window_start = max(flag_02.timestamp, now - timedelta(hours=range_hours))

        pipeline = [
            {
                "$match": {
                    "user_id": user_id,
                    "model_id": model_id,
                    "timestamp": {"$gte": window_start, "$lt": window_end},
                    "flag": {"$ne": True}
                }
            },
            {
                "$group": {
                    "_id": None,
                    "total": {"$sum": "$token_count"}
                }
            }
        ]

        collection = self.engine.get_collection(UserTokenConsumption)
        result = await collection.aggregate(pipeline).to_list(length=1)  # type: ignore[arg-type]

        if result:
            return int(result[0].get("total", 0))
        return 0

    async def check_if_blocked(
        self,
        user_id: str,
        model_id: ObjectId,
        flag_record: Optional[UserTokenConsumption] = None
    ) -> tuple[bool, Optional[datetime]]:
        if flag_record is None:
            flag_record = await self.get_flag_record(user_id, model_id)

        if not flag_record:
            return False, None

        now = datetime.now(timezone.utc)
        two_hours_ago = now - timedelta(hours=2)

        if flag_record.timestamp >= two_hours_ago:
            blocked_until = flag_record.timestamp + timedelta(hours=2)
            return True, blocked_until

        return False, None

    async def create_flag_record(
        self,
        user_id: str,
        model_id: ObjectId
    ) -> UserTokenConsumption:
        """Create a new flag record to mark the start of a 2-hour block.

        Always creates a new document so flag history is preserved for
        inter-flag gap window calculation in aggregate_consumption.
        """
        now = datetime.now(timezone.utc)
        flag_record = UserTokenConsumption(  # type: ignore[call-arg]
            user_id=user_id,
            model_id=model_id,
            token_count=0,
            flag=True,
            timestamp=now,
            client_timestamp=datetime_to_unix_ms(now)
        )
        return await self.engine.save(flag_record)


token_consumption_crud = CRUDTokenConsumption(model=UserTokenConsumption)
