#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

from datetime import datetime
from typing import Optional
from odmantic import ObjectId, Field
from pymongo import IndexModel, DESCENDING

from db.base_class import Base
from utils.helpers import datetime_now_sec
from const.token_consumption import TOKEN_OUTPUT_COST_MULTIPLIER


class UserTokenConsumption(Base):
    """
    User token consumption tracking with deduplication support.

    Uses client_timestamp along with user_id and model_id to prevent duplicate
    consumption records from client retries. The unique compound index on
    (user_id, model_id, client_timestamp) ensures atomic deduplication at the
    database level.
    """
    user_id: str = Field(..., description="User ID who consumed tokens")
    model_id: ObjectId = Field(..., description="Model ID reference from ModelManagement")
    tokens_in: Optional[int] = Field(default=None, description="Number of input tokens consumed")
    tokens_out: Optional[int] = Field(default=None, description="Number of output tokens consumed")
    token_count: int = Field(..., description=f"Computed total tokens ({TOKEN_OUTPUT_COST_MULTIPLIER} * tokens_out + tokens_in) for backward compatibility")
    client_timestamp: int = Field(..., description="Client-provided timestamp in milliseconds since Unix epoch, used for deduplication")
    timestamp: datetime = Field(default_factory=datetime_now_sec, description="Server-assigned timestamp in UTC")
    created_at: datetime = Field(default_factory=datetime_now_sec, description="Creation timestamp")
    flag: Optional[bool] = Field(default=None, description="Marks when user was blocked due to exceeding threshold. When true, this record serves as a blocking marker with timestamp indicating when the block started.")

    model_config = {  # type: ignore[typeddict-unknown-key]
        "collection": "user_token_consumption",
        "indexes": lambda: [
            # Unique index for deduplication on (user_id, model_id, client_timestamp)
            IndexModel(
                [("user_id", 1), ("model_id", 1), ("client_timestamp", 1)],
                unique=True
            ),
            # TTL index for automatic 7-day expiration
            IndexModel(
                [("timestamp", DESCENDING)],
                expireAfterSeconds=604800,  # 7 days
            ),
            # Compound index for flag queries sorted by timestamp (covers get_flag_record / get_flag_records hot path)
            IndexModel([("user_id", 1), ("model_id", 1), ("flag", 1), ("timestamp", DESCENDING)]),
            # Compound index for aggregation pipeline timestamp range scan
            IndexModel([("user_id", 1), ("model_id", 1), ("timestamp", DESCENDING)]),
        ]
    }
