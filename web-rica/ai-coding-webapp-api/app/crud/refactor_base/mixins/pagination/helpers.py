from typing import Any, Generic, Type, Optional, cast
from datetime import datetime
import json
from bson import ObjectId
import base64
from odmantic import query

from filters import DynamicFilterRequest, QueryBuilderType
from utils.handle_exception import ApplicationException
from db.base_class import ModelType

from .models import SortDirection

class CursorCodec:    
    @staticmethod
    def encode_cursor(sort_field: str, sort_value: Any, item_id: str) -> str:
        if isinstance(sort_value, datetime):
            sort_value = sort_value.isoformat()
        elif isinstance(sort_value, ObjectId):
            sort_value = str(sort_value)
        
        cursor_data = {
            "sort_field": sort_field,
            "sort_value": sort_value,
            "item_id": str(item_id),
        }
        
        cursor_json = json.dumps(cursor_data, sort_keys=True)
        return base64.urlsafe_b64encode(cursor_json.encode()).decode()
    
    @staticmethod
    def decode_cursor(cursor: str) -> dict[str, Any]:
        try:
            cursor_json = base64.urlsafe_b64decode(cursor.encode()).decode()
            cursor_data = json.loads(cursor_json)
            
            return cast(dict[str, Any], cursor_data)
        except Exception as e:
            raise ApplicationException("INVALID_CURSOR", obj_1=cursor, obj_2=str(e))
        
    @staticmethod
    def _serialize_value(value: Any) -> str | int | float | bool | None:
        """
        Handles datetime, ObjectId, and other types.
        """
        if value is None:
            return None
        if isinstance(value, datetime):
            return value.isoformat()
        if isinstance(value, ObjectId):
            return str(value)
        if isinstance(value, (int, float, str, bool)):
            return value
        return str(value)
        

    @staticmethod
    def deserialize_value(value_str: str | None, target_type: type) -> Any:
        """
        Convert cursor value string back to its original type.
        """
        if value_str is None:
            return None
        
        # Handle datetime
        if target_type == datetime:
            return datetime.fromisoformat(value_str)
        
        # Handle ObjectId
        if target_type == ObjectId:
            return ObjectId(value_str)
        
        # Handle numeric types
        if target_type == int:
            return int(value_str)
        if target_type == float:
            return float(value_str)
        
        # Handle boolean
        if target_type == bool:
            return value_str.lower() in ('true', '1', 'yes')
        
        # Default: return as string
        return value_str

        
class PaginationSupport(Generic[ModelType, QueryBuilderType]):

    def __init__(self, 
                model: Type[ModelType],
                query_builder: Optional[QueryBuilderType] = None):
        
        self.model = model
        self.query_builder = self.__initialize_query_builder(query_builder=query_builder)

        self.codec = CursorCodec()

    def __initialize_query_builder(self, query_builder):
        if query_builder:
            # Check if it's a class or already an instance
            if isinstance(query_builder, type):
                return query_builder()
            else:
                # Already an instance
                return query_builder
        return None

    def build_sort_expression(
        self,
        sort_field: str,
        sort_direction: str
    ) -> Any:
        """
        Build sort expression (ORDER BY clause).
        """
        field_attr = getattr(self.model, sort_field)
        
        if sort_direction.lower() == 'desc':
            return query.desc(field_attr)
        return query.asc(field_attr)

    def build_cursor_condition(
        self,
        cursor: str,
        sort_field: str,
        sort_direction: SortDirection
    ):
        cursor_data = self.codec.decode_cursor(cursor)
        cursor_sort_field = cursor_data.get("sort_field")
        sort_value = cursor_data.get("sort_value")
        item_id = cursor_data.get("item_id")

        # If cursor is empty or missing required fields, return no filter
        if not cursor_data or not sort_value or not item_id:
            return query.QueryExpression({})
        
        if cursor_sort_field != sort_field:
            raise ApplicationException(f"Cursor sort field {cursor_sort_field} does not match request sort field {sort_field}")
        
        # Convert datatype
        field_attr = getattr(self.model, sort_field)
        cursor_value = self._convert_to_field_type(
            sort_value,
            field_attr
        )

        if sort_direction == SortDirection.DESC:
            return query.or_(
                field_attr < cursor_value,
                query.and_(field_attr == cursor_value, self.model.id < ObjectId(item_id))
            )
        else:
            return query.or_(
                field_attr > cursor_value,
                query.and_(field_attr == cursor_value, self.model.id > ObjectId(item_id))
            )
        
    def generate_next_cursor(
        self,
        items: list[ModelType],
        sort_field: str,
        has_more: bool = False
    ) -> Optional[str]:
        
        if not items or not has_more:
            return None
        
        last_item = items[-1]
        sort_value = getattr(last_item, sort_field)
        return self.codec.encode_cursor(sort_field, sort_value, str(last_item.id))
    
    async def build_completed_query(
            self,
            advanced_filters: Optional[DynamicFilterRequest] = None,
            search_text: Optional[str] = None,
            **kwargs
    ) -> tuple[dict[str, Any], Any]:
        
        return cast(tuple[dict[str, Any], Any], await self.query_builder.build_dynamic_query(
            filters=advanced_filters,
            search_text=search_text,
            **kwargs
        ))
    
    def _convert_to_field_type(self, value: Any, field_attr: Any) -> Any:
        """
        Convert cursor value to the correct type based on field type.
        """
        # Get field type from ODM field info
        field_type = field_attr.field_info.outer_type_
        
        # Use codec to deserialize
        return self.codec.deserialize_value(value, field_type)