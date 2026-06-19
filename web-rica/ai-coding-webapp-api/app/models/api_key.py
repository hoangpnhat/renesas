from __future__ import annotations

from datetime import datetime
from typing import Optional

from odmantic import ObjectId, Field, Index

from db.base_class import Base
from utils.helpers import datetime_now_sec


class ApiKey(Base):
    user_id: ObjectId
    name: str
    key_hash: str
    key_prefix: str  # first 8 chars of plaintext key for display
    created_at: datetime = Field(default_factory=datetime_now_sec)
    expires_at: datetime
    last_used_at: Optional[datetime] = Field(default=None)
    revoked: bool = Field(default=False)

    model_config = {  # type: ignore[typeddict-unknown-key]
        "indexes": lambda: [
            Index(ApiKey.key_hash, unique=True),
            Index(ApiKey.user_id),
        ]
    }
