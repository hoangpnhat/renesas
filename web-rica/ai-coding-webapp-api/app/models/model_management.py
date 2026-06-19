from datetime import datetime
from typing import List, Optional

from odmantic import Field, Index, query
from pydantic import BaseModel, Field as PydanticField

from db.base_class import Base
from utils.helpers import datetime_now_sec


class DefaultCompletionOptions(BaseModel):
    max_tokens: Optional[int] = PydanticField(default=256, description="Maximum tokens for completion")
    context_length: Optional[int] = PydanticField(default=4096, description="Context length for the model")
    temperature: Optional[float] = PydanticField(default=0.7, description="Temperature for model sampling")


class ModelManagement(Base):
    name: str = Field(default="", description="Model name")
    model_path: str = Field(default="", description="Path to the model file")
    roles: List[str] = Field(default_factory=list, description="Roles that can access this model")
    default_completion_options: Optional[DefaultCompletionOptions] = Field(
        default=None,
        description="Default completion options for the model")
    created_at: datetime = Field(default_factory=datetime_now_sec, description="Creation timestamp")
    deleted_at: Optional[datetime] = Field(default=None, description="Deletion timestamp")
    threshold: Optional[int] = Field(default=None, description="Token limit for consumption window")
    consumption_range_hours: Optional[int] = Field(default=None, description="Hours for rolling window (e.g., 1, 24, 168)")
    model_config = {  # type: ignore[typeddict-unknown-key]
        "indexes": lambda: [
            Index(query.desc(ModelManagement.created_at), query.desc(ModelManagement.deleted_at))
        ],
        # "use_enum_values": True,

    }
