from enum import Enum
from typing import Any, Optional, Generic

from pydantic import BaseModel, Field

from db.base_class import ModelType
from filters import DynamicFilterRequest


class SortDirection(str, Enum):
    ASC = "asc"
    DESC = "desc"


class CommonRequest(BaseModel):
    # Skip-based
    page: Optional[int] = Field(None, ge=0, description="Page size")

    # Common
    limit: int = Field(20, ge=1, description="Items per page")
    sort_field: str = Field("modified", description="Field to sort by")
    sort_direction: SortDirection = Field(SortDirection.DESC, description="Sort direction")

    # legacy:
    query: Optional[str] = None


class ComprehensiveRequest(CommonRequest):
    # Cursor-based
    cursor: Optional[str] = Field(None, description="Cursor for infinite scroll")

    # Enhanced filtering
    advanced_filters: Optional[DynamicFilterRequest] = Field(None, description="Advanced filter conditions")

    # Search text
    search_text: Optional[str] = None


class PaginationInfo(BaseModel):
    total: int = Field(..., description="Total number of items")
    page: Optional[int] = Field(None, ge=0, description="Current page number")
    limit: int = Field(..., ge=1, description="Items per page")


class PaginationResponse(BaseModel, Generic[ModelType]):
    data: list[Any]

    system_data: Optional[Any] = None

    # Cursor-based
    next_cursor: Optional[str] = Field(None, description="Cursor for next page")
    has_more: Optional[bool] = Field(None, description="Whether more items exist")

    # Skip-based
    pagination: Optional[PaginationInfo] = Field(None, description="Pagination info")
