from typing import Literal, Optional

from pydantic import BaseModel, Field


class ExtensionToken(BaseModel):
    token_type: Literal["Bearer"]
    expires_in: int
    ext_expires_in: int
    access_token: str
    
class ExtensionTokenRequestBody(BaseModel):
    version: Optional[str] = Field(default=None, description="Version of the extension")