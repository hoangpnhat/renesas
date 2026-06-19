#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

from datetime import datetime
from odmantic import ObjectId
from pydantic import BaseModel, Field
from typing import Optional
from utils.helpers import datetime_now_sec
from const.token_consumption import TOKEN_OUTPUT_COST_MULTIPLIER


class TokenConsumptionUpdate(BaseModel):
    pass


class TokenConsumptionCreate(BaseModel):
    """Schema for creating a new consumption record. All fields required."""
    user_id: str = Field(..., description="User ID who consumed tokens")
    model_id: ObjectId = Field(..., description="Model ID reference from ModelManagement")
    tokens_in: int = Field(..., ge=0, description="Number of input tokens consumed")
    tokens_out: int = Field(..., ge=0, description="Number of output tokens consumed")
    token_count: int = Field(..., description=f"Computed total tokens ({TOKEN_OUTPUT_COST_MULTIPLIER} * tokens_out + tokens_in)")
    client_timestamp: int = Field(..., description="Client-provided timestamp in milliseconds since Unix epoch")
    timestamp: Optional[datetime] = Field(default_factory=datetime_now_sec, description="Server-assigned timestamp in UTC")
    created_at: Optional[datetime] = Field(default_factory=datetime_now_sec, description="Creation timestamp")
