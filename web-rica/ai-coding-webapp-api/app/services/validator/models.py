from pydantic import BaseModel
from typing import Any
class FileValidateProperties(BaseModel):
    mime_type: str
    file_extension: str | None
    size: int

class ValidationResult(BaseModel):
    is_valid: bool
    errors: list[str] = []
    metadata: Any