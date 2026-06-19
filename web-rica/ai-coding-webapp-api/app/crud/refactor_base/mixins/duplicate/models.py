from typing import TypeVar, Optional, Any
from pydantic import BaseModel


BodyX = TypeVar('BodyX')


class MetadataModel(BaseModel):
    field_name: str
    field_value: str
    
class DuplicateCheckResponse(BaseModel):
    check_id: str
    duplicate_exists: bool
    document_id: Optional[str] = None
    document_info: Optional[Any] = None

# for advanced create
class CheckDuplicateEntity(BaseModel):
    check_field: str
    constraint_field: str
    delete_field: Optional[str] = None

class ContextDataModel(BaseModel):
    duplicate_check: DuplicateCheckResponse
    check_entity: CheckDuplicateEntity
    metadata: Optional[list[MetadataModel]] = None

class ProcessResult(BaseModel):
    name_changed: Optional[str] = None
