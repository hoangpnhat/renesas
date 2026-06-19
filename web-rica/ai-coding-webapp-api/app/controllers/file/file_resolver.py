from typing import Any, Union
from datetime import datetime, timedelta
from odmantic import ObjectId
from models.file import Files as MongoFile
from schemas.file import FileInDB
from services.resolution.entity_resolver import EntityResolver, EntityType
from const.common import CommonStatus
from core.config import settings
class FileResolver(EntityResolver):

    def get_fixed_name(self) -> str:
        return self.get_empty_name()
    
    def resolve_status(self, file: FileInDB, marked: datetime) -> tuple[str | None, datetime | None]:
        status = file.status

        if status != CommonStatus.PROCESSING:
            return (status, file.modified)

        last_modified = file.modified if status == CommonStatus.COMPLETED else None
        return (status, last_modified)
    
    async def _build_lookups(self, files: list[MongoFile]) -> dict[str, Any]:
        """Build all required lookups: users"""
        
        user_ids: list[ObjectId | str] = [u for u in {file.author_id for file in files} if u is not None]
        users: list[Any] = await self.lookup_service.bulk_lookup(entity_type=EntityType.USER, ids=user_ids)
        user_lookup = {str(user.id): user.full_name for user in users} if users else {}
        return {'users': user_lookup}
    
    async def resolve(self, file: MongoFile, lookups: dict[str, dict] | None = None, overrides: dict | None = None) -> FileInDB:
        overrides = overrides or {}

        if lookups is None:
            lookups = await self._build_lookups([file])

        user_lookup = lookups.get('users', {})
        owner_name = await self._resolve_entity(
                                            file.author_id,
                                            EntityType.USER,
                                            user_lookup,
                                            overrides,
                                            'full_name')
        return FileInDB(id=str(file.id),
                        file_name=file.file_name,
                        owner=owner_name,
                        total_pages=file.total_pages,
                        author_id=str(file.author_id),
                        uploaded_at=file.uploaded_at,
                        folder_path=file.folder_path,
                        size=file.size,
                        modified=file.modified,
                        extension=file.extension or 'unknown',
                        status=file.status)