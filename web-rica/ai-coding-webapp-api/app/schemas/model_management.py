from datetime import datetime
from typing import List, Optional

from odmantic import ObjectId
from pydantic import BaseModel, Field, ConfigDict, field_validator, model_validator
from pydantic.alias_generators import to_camel

from const.enums import RoleEnum, ProviderEnum, ContextProviderEnum
from schemas import PaginationSchema
from utils.helpers import datetime_now_to_unix
from core.config import settings

class CamelModel(BaseModel):
    model_config = ConfigDict(
        populate_by_name=True,
        alias_generator=to_camel,
        use_enum_values=True
    )


class DefaultCompletionOptionsSchemas(BaseModel):
    max_tokens: int = Field(default=256, description="Maximum tokens for completion")
    context_length: int = Field(default=4096, ge=1, description="Context length for the model (minimum 1)")
    temperature: float = Field(default=0.7, ge=0.0, le=2.0, description="Temperature for model sampling (0.0 to 2.0)")

    @field_validator('context_length')
    @classmethod
    def validate_context_length(cls, v):
        if not isinstance(v, int) or v < 1:
            raise ValueError('context_length must be an integer with minimum value of 1')
        return v

    @field_validator('temperature')
    @classmethod
    def validate_temperature(cls, v):
        if not isinstance(v, float) and not isinstance(v, int):
            raise ValueError('temperature must be a float')
        if not (0.0 <= float(v) <= 2.0):
            raise ValueError('temperature must be between 0.0 and 2.0')
        return float(v)


class DefaultCompletionOptions(CamelModel):
    max_tokens: int = Field(default=256, description="Maximum tokens for completion")
    context_length: int = Field(default=4096, ge=1, description="Context length for the model (minimum 1)")
    temperature: float = Field(default=0.7, ge=0.0, le=2.0, description="Temperature for model sampling (0.0 to 2.0)")

    @field_validator('context_length')
    @classmethod
    def validate_context_length(cls, v):
        if not isinstance(v, int) or v < 1:
            raise ValueError('context_length must be an integer with minimum value of 1')
        return v

    @field_validator('temperature')
    @classmethod
    def validate_temperature(cls, v):
        if not isinstance(v, float) and not isinstance(v, int):
            raise ValueError('temperature must be a float')
        if not (0.0 <= float(v) <= 2.0):
            raise ValueError('temperature must be between 0.0 and 2.0')
        return float(v)


class ModelManagementCore(BaseModel):
    name: str = Field(default="", description="Model name")
    model_path: str = Field(default="unknown", description="Path to the model file")
    roles: List[RoleEnum] = Field(default=[], description="Roles that can access")
    default_completion_options: DefaultCompletionOptionsSchemas = Field(
        default_factory=DefaultCompletionOptionsSchemas,
        description="Default completion options for the model")
    threshold: Optional[int] = Field(
        default=None,
        gt=0,
        description="Token limit for consumption window (must be positive)"
    )
    consumption_range_hours: Optional[int] = Field(
        default=None,
        gt=0,
        le=168,
        description="Hours for rolling window (1-168, max 7 days)"
    )

    @model_validator(mode='after')
    def validate_consumption_config(self):
        """Ensure both threshold and consumption_range_hours are set together or both null."""
        threshold = self.threshold
        range_hours = self.consumption_range_hours

        # Both must be set or both must be None
        if (threshold is None) != (range_hours is None):
            raise ValueError(
                "Both threshold and consumption_range_hours must be set together, "
                "or both must be null to disable consumption tracking"
            )

        return self


class ModelManagementCreate(ModelManagementCore):
    pass


class ModelManagementUpdate(ModelManagementCore):
    pass


class ModelManagementResponse(ModelManagementCore):
    id: ObjectId = Field(..., description="Model ID")
    created_at: datetime = Field(..., description="Creation timestamp")


class ConsumptionLimit(CamelModel):
    """Token consumption limit information for a model."""
    enabled: bool = Field(..., description="Whether consumption tracking is enabled for this model")
    threshold: int = Field(..., description="Maximum token count allowed in the time window")
    range_hours: int = Field(..., description="Time window in hours for consumption tracking")
    consumed: int = Field(..., description="Total tokens consumed in the current time window")
    remaining: int = Field(..., description="Remaining tokens (threshold - consumed, negative if exceeded). Check remaining < 0 to determine if threshold exceeded")
    blocked: bool = Field(..., description="Whether user is currently blocked from using this model due to exceeding threshold")
    blocked_until: Optional[int] = Field(
        default=None,
        description="Unix timestamp when block expires (null if not blocked)"
    )


class Model(CamelModel):
    # Required fields - must be provided
    id: ObjectId
    name: str
    model: str
    roles: List[RoleEnum]
    context_length: Optional[int] = Field(
        default=None,
        alias="contextLength",
        description="Context length for the model (cloned from defaultCompletionOptions)"
    )
    default_completion_options: Optional[DefaultCompletionOptions] = Field(
        default=None,
        alias="defaultCompletionOptions"
    )

    # All other fields have defaults
    provider: ProviderEnum = Field(default=ProviderEnum.RICA_PROXY)
    org_scope_id: Optional[str] = Field(default=None, alias="orgScopeId")
    on_prem_proxy_url: Optional[str] = Field(default=None, alias="onPremProxyUrl")
    api_base: str = Field(default=f"{settings.EXT_DATABRICKS_URL}serving-endpoints/",
                          alias="apiBase")
    api_key_location: str = Field(default="https://www.google.com", alias="apiKeyLocation")
    consumption_limit: Optional[ConsumptionLimit] = Field(
        default=None,
        alias="consumptionLimit",
        description="Token consumption limit information (null if not configured)"
    )


class ContextProviderParams(CamelModel):
    n_retrieve: Optional[int] = Field(default=None, alias="nRetrieve")
    n_final: Optional[int] = Field(default=None, alias="nFinal")
    use_reranking: Optional[bool] = Field(default=None, alias="useReranking")


class ContextProvider(CamelModel):
    provider: ContextProviderEnum
    params: Optional[ContextProviderParams] = Field(default=None)


class Config(CamelModel):
    # All config fields have defaults
    name: str = Field(default="RICA Test Configuration")
    version: str = Field(default="1.0.0")
    schema: str = Field(default="v1")  # type: ignore[assignment]
    models: List[Model] = Field(default_factory=list)
    context: List[ContextProvider] = Field(
        default_factory=lambda: [
            ContextProvider(provider=ContextProviderEnum.CODE),
            ContextProvider(
                provider=ContextProviderEnum.DOCS,
                params=ContextProviderParams(
                    n_retrieve=30,
                    n_final=8,
                    use_reranking=True
                )
            ),
            ContextProvider(provider=ContextProviderEnum.DIFF),
            ContextProvider(provider=ContextProviderEnum.TERMINAL),
            ContextProvider(provider=ContextProviderEnum.PROBLEMS),
            ContextProvider(provider=ContextProviderEnum.FOLDER),
            ContextProvider(
                provider=ContextProviderEnum.CODEBASE,
                params=ContextProviderParams(
                    n_retrieve=25,
                    n_final=6,
                    use_reranking=True
                )
            ),
            ContextProvider(
                provider=ContextProviderEnum.RICA_DOCS,
                params=ContextProviderParams(
                    n_retrieve=10,
                    n_final=2,
                    use_reranking=True
                )
            ),
            
        ]
    )


class RICAConfigurationResponse(CamelModel):
    # All top-level fields have defaults
    config: Config = Field(default_factory=Config)
    timestamp: int = Field(default_factory=datetime_now_to_unix)  # Default timestamp
    version: str = Field(default="1.0.0")


class ModelManagementResponseBody(PaginationSchema):
    data: List[ModelManagementResponse] = Field(description="List of topics.", default=[])
