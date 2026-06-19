from datetime import date
from typing import Dict, Any, Optional, TypeVar, Literal
from uuid import UUID

from odmantic import ObjectId
from pydantic import BaseModel, Field, ConfigDict
from typing_extensions import TypedDict

from const.enums import SortOrder


class SortFieldDict(TypedDict):
    field: str
    direction: SortOrder

class DbxError(BaseModel):
    code: str
    detail: Optional[str] = None

class BaseSchema(BaseModel):
    @property
    def as_db_dict(self) -> Dict[str, Any]:
        to_db: Dict[str, Any] = self.model_dump(
            exclude_defaults=True, exclude_none=True, exclude={"identifier", "id"}
        )
        for key in ["identifier", "id"]:
            if key in self.model_dump().keys():
                to_db[key] = self.model_dump()[key].hex
        return to_db


class MetadataBaseSchema(BaseSchema):
    title: str | None = Field(
        None, description="A human-readable title given to the resource."
    )  # noqa
    description: str | None = Field(
        None,
        description="A short description of the resource.",
    )
    isActive: bool | None = Field(
        default=True, description="Whether the resource is still actively maintained."
    )  # noqa
    isPrivate: bool | None = Field(
        default=True,
        description="Whether the resource is private to team members with appropriate authorization.",  # noqa
    )


class MetadataBaseCreate(MetadataBaseSchema):
    pass


class MetadataBaseUpdate(MetadataBaseSchema):
    identifier: UUID = Field(
        ..., description="Automatically generated unique identify for the resource"
    )


class MetadataBaseInDBBase(MetadataBaseSchema):
    identifier: UUID = Field(
        ..., description="Automatically generated unique identity for the resource."
    )
    created: date = Field(
        ..., description="Automatically generated date resource was created."
    )
    isActive: bool = Field(
        ..., description="Whether the resource is still actively maintained."
    )
    isPrivate: bool = Field(
        ...,
        description="Whether the resource is private to team members with appropriate authorization.",
    )
    model_config = ConfigDict(from_attributes=True)


class BatchIdsDTO(BaseSchema):
    ids: list[ObjectId] = Field(
        min_length=1, description="List of unique identifiers for the resources."
    )


class DeletedMultipleResponseDTO(BaseSchema):
    deleted_count: int = Field(
        description="Number of resources successfully deleted."
    )
    deleted_ids: list[ObjectId] = Field(
        description="List of unique identifiers for the resources successfully deleted."
    )

class CallBack(BaseModel):
    status: Literal['failed', 'success']
    id: str
    result_path: Optional[str] = None
    error: Optional[DbxError] = None

class CallbackResult(BaseModel):
    is_callback_ok: bool = True
    error: Optional[str] = None

class PaginationDetailSchema(BaseModel):
    page: int = Field(
        1, description="The current page number."
    )
    limit: int = Field(
        10, description="The number of items per page."
    )
    total: int = Field(
        0, description="The total number of items in the collection."
    )

class DBXCheck(BaseModel):
    is_connected: bool
    jobs: list[str]
    error: str

class PagingResponseDTO(BaseSchema):
    data: list[Any]
    pagination: PaginationDetailSchema


class PaginationSchema(BaseSchema):
    page: Optional[int] = Field(
        default=None,
        ge=0,
        title="Page query for page break loading",
    )
    entries: Optional[int] = Field(
        default=None,
        title="Number of Items for each page",
    )
    total: int = Field(
        default=0,
        title="Total number of items",
        ge=0,
    )

    data: Any = Field(
        default=[],
        title="Data to be paginated",
    )


class NameValuePair(BaseSchema):
    name: str = Field(..., description="Name of the item")
    value: Any = Field(..., description="Value of the item")

BodyX = TypeVar('BodyX')

class DuplicateCheckResponse(BaseModel):
    check_id: str
    duplicate_exists: bool
    document_id: Optional[str] = None
    document_info: Optional[Any] = None

# for advanced create
class CheckDuplicateParams(BaseModel):
    object_field: str
    user_field: str
    delete_field: Optional[str] = None