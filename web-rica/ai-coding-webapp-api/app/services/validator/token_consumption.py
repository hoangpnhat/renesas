#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

from datetime import datetime, timezone
from pydantic import BaseModel, Field, field_validator
from odmantic import ObjectId
from typing import Optional

from utils.handle_exception import ApplicationException
from db.session import get_engine
from models.model_management import ModelManagement


class ConsumptionRecord(BaseModel):
    """Schema for a single consumption record in the request body."""
    model_id: str = Field(..., description="Model ID from ModelManagement")
    tokens_in: int = Field(..., ge=0, le=1_000_000, description="Number of input tokens consumed (0 to 1,000,000)")
    tokens_out: int = Field(..., ge=0, le=1_000_000, description="Number of output tokens consumed (0 to 1,000,000)")
    client_timestamp: int = Field(..., description="Client-provided timestamp in milliseconds since Unix epoch")

    @field_validator('client_timestamp', mode='before')
    @classmethod
    def parse_client_timestamp(cls, v: int | str) -> int:
        """
        Normalise client_timestamp to milliseconds since Unix epoch.

        Accepted input formats:
        - int: used as-is (assumed milliseconds)
        - numeric string: parsed as int and used as-is (e.g. "1744963200000")
        - ISO 8601 string: converted to milliseconds (e.g. "2026-04-07T10:30:00.000Z")
        """
        if isinstance(v, int):
            return v
        if isinstance(v, str):
            # Try numeric string first (e.g. "1744963200000")
            try:
                return int(v)
            except ValueError:
                pass
            # Try ISO 8601 string (e.g. "2026-04-07T10:30:00Z", "2026-04-07T10:30:00+07:00")
            try:
                dt = datetime.fromisoformat(v.replace("Z", "+00:00"))
                if dt.tzinfo is None:
                    dt = dt.replace(tzinfo=timezone.utc)
                return int(dt.timestamp() * 1000)
            except ValueError:
                pass
        raise ValueError(
            f"client_timestamp must be an integer (ms since epoch) or a valid ISO 8601 string, got: {v!r}"
        )

    @field_validator('model_id')
    @classmethod
    def validate_model_id_format(cls, v: str) -> str:
        """Validate that model_id is a valid ObjectId format."""
        try:
            ObjectId(v)
        except Exception:
            raise ValueError(f"Invalid model_id format: {v}")
        return v


class PostConfigsRequest(BaseModel):
    """Request schema for POST /api/user-model/configs endpoint."""
    consumption: list[ConsumptionRecord] = Field(
        default_factory=list,
        max_length=1000,
        description="Token consumption records (max 1,000 records per request)"
    )

    @field_validator('consumption')
    @classmethod
    def validate_batch_size(cls, v: list[ConsumptionRecord]) -> list[ConsumptionRecord]:
        """Validate batch size does not exceed 1,000 records."""
        if len(v) > 1000:
            raise ValueError(f"Batch size limit exceeded: {len(v)} records (max 1,000)")
        return v


async def validate_model_ids(consumption_records: list[ConsumptionRecord]) -> tuple[list[ConsumptionRecord], list[str]]:
    """
    Validate model_ids and return valid records + list of invalid IDs.

    This implements partial batch failure handling per spec: valid records are accepted
    while invalid ones are filtered out and logged.

    Args:
        consumption_records: List of consumption records to validate

    Returns:
        Tuple of (valid_records, invalid_model_ids)
    """
    if not consumption_records:
        return [], []

    engine = get_engine()
    model_ids = [ObjectId(record.model_id) for record in consumption_records]

    # Query all models at once
    models = await engine.find(
        ModelManagement,
        (ModelManagement.id.in_(model_ids)) & (ModelManagement.deleted_at == None)  # type: ignore[union-attr]
    )

    found_ids = {str(model.id) for model in models}
    provided_ids = {record.model_id for record in consumption_records}

    missing_ids = provided_ids - found_ids

    # Partition records into valid and invalid
    valid_records = [record for record in consumption_records if record.model_id in found_ids]
    invalid_ids = list(missing_ids)

    return valid_records, invalid_ids
