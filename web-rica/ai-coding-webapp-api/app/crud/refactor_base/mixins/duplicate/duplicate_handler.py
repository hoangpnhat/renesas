from typing import Any, Generic, Optional, cast
from odmantic import ObjectId
import re
from fastapi.encoders import jsonable_encoder

from .models import (CheckDuplicateEntity, DuplicateCheckResponse,
                    ContextDataModel, MetadataModel, ProcessResult)
from db.base_class import ModelType
from utils.helpers import UploadHelper
from utils.handle_exception import ApplicationException
from const.common import CommonStatus



class DuplicateHandler(Generic[ModelType]):

    # Type hint for parent
    model: type[ModelType]
    engine: Any

    async def _get_duplicate_conflict_doc(
        self,
        check_field, 
        constraint_field, 
        delete_field, 
        check_value, 
        constraint_value
    ) -> Optional[ModelType]:
        
        conflict_query = {
            check_field: check_value,
            constraint_field: ObjectId(constraint_value)
        }

        if delete_field:
            # Use False for boolean fields (is_*, deleted), {"$ne": CommonStatus.DELETED} for status fields
            conflict_query[delete_field] = False if (isinstance(delete_field, str) and (delete_field.startswith('is_') or delete_field == 'deleted')) else {"$ne": CommonStatus.DELETED}

        return cast(ModelType | None, await self.engine.find_one(self.model, conflict_query))
    
    async def _generate_non_conflicting_name(self, context_data: ContextDataModel) -> str:
        """Generate a unique name by appending (1), (2), etc."""
        check_field = context_data.check_entity.check_field
        constraint_field = context_data.check_entity.constraint_field
        delete_field = context_data.check_entity.delete_field

        # Extract metadata
        base_value = None
        ext_value = None
        user_value = None

        if context_data.metadata:
            for meta in context_data.metadata:
                if meta.field_name == "base":
                    base_value = meta.field_value
                elif meta.field_name == "ext":
                    ext_value = meta.field_value
                elif meta.field_name == constraint_field:
                    user_value = meta.field_value

        # Query for existing files with similar names
        suffix_query = {
            check_field: {
                "$regex": f"^{re.escape(base_value or '')}( \\(\\d+\\))*{re.escape(ext_value or '')}$",
                "$options": "i"
            },
            constraint_field: ObjectId(user_value)
        }

        if delete_field:
            # Use False for boolean fields (is_*, deleted), {"$ne": CommonStatus.DELETED} for status fields
            suffix_query[delete_field] = False if (isinstance(delete_field, str) and (delete_field.startswith('is_') or delete_field == 'deleted')) else {"$ne": CommonStatus.DELETED}

        existing = await self.engine.find(self.model, suffix_query)
        existing_names = [getattr(doc, check_field) for doc in existing]

        return UploadHelper.generate_incremented_name(base_value or "", ext_value or "", existing_names)
    
    async def check_duplicate_context(
        self,
        obj_in_data: dict | ModelType,
        check_duplicate_fields: CheckDuplicateEntity,
    ) -> ContextDataModel:
        """Check for duplicate and return context for later processing."""
        check_field = check_duplicate_fields.check_field
        constraint_field = check_duplicate_fields.constraint_field
        delete_field = check_duplicate_fields.delete_field

        if not check_field or not constraint_field:
            raise ApplicationException("BAD_REQUEST", detail="Missing required fields to check duplicate")

        if isinstance(obj_in_data, dict):
            obj_in_data = self.model(**obj_in_data)

        check_value = getattr(obj_in_data, check_field)
        constraint_value = getattr(obj_in_data, constraint_field)
        base, ext = UploadHelper.get_base_and_ext(check_value)

        # Find the duplicate
        existing_doc = await self._get_duplicate_conflict_doc(
            check_field, constraint_field, delete_field, check_value, constraint_value
        )

        # Context object for caching
        return ContextDataModel(
            duplicate_check=DuplicateCheckResponse(
                duplicate_exists=existing_doc is not None,
                check_id='',
                document_id=str(existing_doc.id) if existing_doc else None,
                document_info=jsonable_encoder(existing_doc) if existing_doc else None
            ),
            check_entity=check_duplicate_fields,
            metadata=[
                MetadataModel(field_name=check_field, field_value=check_value),
                MetadataModel(field_name=constraint_field, field_value=str(constraint_value)),
                MetadataModel(field_name="base", field_value=base),
                MetadataModel(field_name="ext", field_value=ext)
            ]
        )

    async def process_duplicate_context(
        self,
        context_data: ContextDataModel,
        is_replace: bool = True
    ) -> ProcessResult:
        """Process duplicate context - simplified to only handle name generation.

        Args:
            file_name: The original file name
            context_data: Cached context from check_duplicate_context
            is_replace: If False, generates a new unique name

        Returns:
            ProcessResult with name_changed if new name generated
        """
        if is_replace is None:
            raise ApplicationException("BAD_REQUEST", "is_replace parameter must be explicitly set")

        # If duplicate exists and user chose not to replace, generate new name
        if context_data.duplicate_check.duplicate_exists and not is_replace:
            new_name = await self._generate_non_conflicting_name(context_data)
            return ProcessResult(name_changed=new_name)

        return ProcessResult()