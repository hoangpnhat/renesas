from typing import List, Dict, Optional, Union

from pydantic import BaseModel


class Tag(BaseModel):
    key: Optional[str] = None
    value: Optional[str] = None

    class Config:
        extra = "ignore"


class ExternalModel(BaseModel):
    provider: Optional[str] = None
    name: Optional[str] = None
    task: Optional[str] = None

    class Config:
        extra = "ignore"


class FoundationModel(BaseModel):
    name: Optional[str] = None
    display_name: Optional[str] = None
    docs: Optional[str] = None
    description: Optional[str] = None
    price: Optional[str] = None
    input_price: Optional[str] = None
    price_unit: Optional[str] = None
    pricing_model: Optional[str] = None

    class Config:
        extra = "ignore"


class ServedEntityExternal(BaseModel):
    name: Optional[str] = None
    type: Optional[str] = None  # "EXTERNAL_MODEL"
    external_model: Optional[ExternalModel] = None

    class Config:
        extra = "ignore"


class ServedEntityFoundation(BaseModel):
    name: Optional[str] = None
    type: Optional[str] = None  # "FOUNDATION_MODEL"
    foundation_model: Optional[FoundationModel] = None

    class Config:
        extra = "ignore"


class ServedEntityUC(BaseModel):
    name: Optional[str] = None
    entity_name: Optional[str] = None
    entity_version: Optional[str] = None
    type: Optional[str] = None  # "UC_MODEL"

    class Config:
        extra = "ignore"


class ServedEntityBasic(BaseModel):
    name: Optional[str] = None
    entity_version: Optional[str] = None
    entity_name: Optional[str] = None

    class Config:
        extra = "ignore"


ServedEntityType = Union[
    ServedEntityExternal,
    ServedEntityFoundation,
    ServedEntityUC,
    ServedEntityBasic
]


class ConfigModel(BaseModel):
    served_entities: Optional[List[ServedEntityType]] = None
    served_models: Optional[List[Dict]] = None

    class Config:
        extra = "ignore"


class State(BaseModel):
    ready: Optional[str] = None
    config_update: Optional[str] = None
    suspend: Optional[str] = None

    class Config:
        extra = "ignore"


class AiGateway(BaseModel):
    usage_tracking_config: Optional[Dict] = None
    inference_table_config: Optional[Dict] = None

    class Config:
        extra = "ignore"


class Endpoint(BaseModel):
    name: Optional[str] = None
    creator: Optional[str] = None
    creation_timestamp: Optional[int] = None
    last_updated_timestamp: Optional[int] = None
    state: Optional[State] = None
    config: Optional[ConfigModel] = None
    tags: Optional[List[Tag]] = None
    id: Optional[str] = None
    route_optimized: Optional[bool] = None
    task: Optional[str] = None
    endpoint_type: Optional[str] = None
    permission_level: Optional[str] = None
    ai_gateway: Optional[AiGateway] = None
    creator_display_name: Optional[str] = None
    creator_kind: Optional[str] = None
    description: Optional[str] = None

    class Config:
        extra = "ignore"


class ModelFromDatabricksResponse(BaseModel):
    endpoints: Optional[List[Endpoint]] = None

    class Config:
        extra = "ignore"
