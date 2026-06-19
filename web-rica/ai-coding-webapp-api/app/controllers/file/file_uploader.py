import json
from abc import ABC, abstractmethod
from typing import Optional, Any, Type, Union, cast

from fastapi import UploadFile as RawFile
from odmantic import ObjectId

from const.common import CommonStatus
from models.file import Files as MongoFile
from schemas.file import FileCreate, FileUpdate
from schemas.task import UploadStage
from services.storage import BaseStorageProvider, DatabricksStorage
from services.taskiq.broker import broker
from services.validator import BaseContentValidator, ValidationResult, FileValidateProperties
from utils.handle_exception import ApplicationException

ValidatorType = Union[Type[BaseContentValidator], BaseContentValidator]


class BaseFileCreateer(ABC):

    def __init__(self,
                 crud: Any,
                 storage: BaseStorageProvider | None = None,
                 broker_service: Any = None) -> None:
        self.crud = crud
        self._storage = storage or DatabricksStorage()
        self._broker = broker_service or broker

    @abstractmethod
    async def build_target_directory(self, mongo_file: MongoFile) -> str:
        """Build target directory path in remote volume based on mongo_file"""
        pass

    async def process_upload(self,
                             session: Any,
                             file: RawFile,
                             user_id: ObjectId,
                             target_dir: Optional[str] = None,
                             validator: ValidatorType = BaseContentValidator,
                             skip_process: bool = False,
                             upload_name: Optional[str] = None,
                             existing_file: Optional[MongoFile] = None,
                             **kwargs) -> MongoFile:

        """Create or update file record, upload file to storage and trigger post-processing.

        Args:
            target_dir: Optional target directory. If not provided, build_target_directory will be called.
            existing_file: If provided, updates existing file. Otherwise creates new file.

        If upload fails an ApplicationException is raised so the invoking transaction can rollback.
        """
        try:
            # Validate file and get properties
            valid_properties = await self._check_raw_file(validator, file)
            if valid_properties is None or valid_properties.file_extension is None:
                raise ApplicationException('INVALID_CONTENT', desc='File validation failed')

            # Determine if creating new or updating existing
            # Prepare file object based on operation type
            if existing_file is not None:
                update_data = await self._prepare_file_object_update(
                    file=file,
                    file_size=valid_properties.size,
                    file_extension=valid_properties.file_extension,
                    **kwargs
                )
                mongo_file = await self._update_to_mongo(existing_file, update_data, session=session)
            else:
                create_data = await self._prepare_file_object_newly(
                    file=file,
                    user_id=user_id,
                    file_size=valid_properties.size,
                    file_extension=valid_properties.file_extension,
                    **kwargs
                )
                mongo_file = await self._upload_to_mongo(create_data, session=session)

            # use provided target_dir or build from mongo_file
            if target_dir is None:
                target_dir = await self.build_target_directory(mongo_file)

            # Generate upload name
            if not upload_name:
                upload_name = f'{mongo_file.id}.{valid_properties.file_extension}'

            # Upload to storage
            upload_info = await self._upload_to_storage(file, target_dir, upload_name)
            if not upload_info.status or not upload_info.upload_path:
                raise ApplicationException('STORAGE_UPLOAD_FAILED', detail=upload_info.error)

            # Dispatch post-processing if NOT skipping
            if not skip_process:
                await self._dispatch_post_processing(
                    mongo_file,
                    upload_info.upload_path,
                    **kwargs
                )

            mongo_file = await self._persist_temporary_fields(mongo_file, target_dir, session)
            return mongo_file
        except Exception as e:
            raise e

    async def _upload_to_mongo(self, file: FileCreate, session: Any) -> MongoFile:
        """Create file record in database."""
        return cast(MongoFile, await self.crud.create(obj_in=file, session=session))

    async def _update_to_mongo(self, old_file: MongoFile, new_file: FileUpdate, session: Any) -> MongoFile:
        """Update file record in database."""
        return cast(MongoFile, await self.crud.update(old_file, new_file, session))

    async def _upload_to_storage(self, file: RawFile, target_dir: str, upload_name: str) -> UploadStage:
        """Upload file to storage provider."""
        await file.seek(0)
        body = await file.read()
        dir_name = await self._storage.create_target_directory(target_dir)
        upload_file_path = f'{dir_name}/{upload_name}'
        return await self._storage.upload(upload_file_path=upload_file_path, file_body=body)

    async def _persist_temporary_fields(self, mongo_file: MongoFile, upload_path: str, session: Any) -> MongoFile:
        """Update some temporary fields"""
        try:
            return cast(MongoFile, await self.crud.update(db_obj=mongo_file, obj_in=FileUpdate(
                folder_path=upload_path,
                status=CommonStatus.PROCESSING),
                                          session=session))
        except Exception:
            raise ApplicationException('DB_UPDATE_FAILED', desc='Failed to update folder_path')

    async def _dispatch_post_processing(self, mongo_file: MongoFile, upload_path: str, **kwargs: Any) -> None:
        task_metadata = {
            **mongo_file.model_dump(),
            "id": str(mongo_file.id),
            "author_id": str(mongo_file.author_id),
            "folder_path": upload_path
        }

        from loguru import logger
        from core.config import settings

        logger.info(f"Dispatching background task for file: {mongo_file.file_name}")

        # Use pipeline processing if enabled and not in rollback mode
        use_pipeline = getattr(settings, 'USE_PIPELINE_PROCESSING', False)
        pipeline_rollback = getattr(settings, 'PIPELINE_ROLLBACK', False)

        if use_pipeline and not pipeline_rollback:
            logger.info("Using new streaming pipeline processing")
            await self._broker.send_task(
                'sync_file_pipeline_processing',
                metadata=json.dumps(task_metadata, default=str),
                user_id=str(mongo_file.author_id)
            )
        else:
            logger.info("Using legacy sequential processing")
            await self._broker.send_task(
                'sync_file_conversion_and_indexing',
                metadata=json.dumps(task_metadata, default=str)
            )

        logger.info("Background task dispatched successfully")

    async def _check_raw_file(self, validator: ValidatorType, file: RawFile) -> FileValidateProperties | None:
        """Validate file and return (mime, extension, size)."""
        validator_inst = validator(file) if isinstance(validator, type) else validator  # type: ignore[call-arg]

        if not hasattr(validator_inst, 'run'):
            raise ApplicationException('VALIDATOR_MISSING_RUN',
                                       desc='Validator must implement async run()')

        result: ValidationResult = await validator_inst.run()
        if result.is_valid:
            return cast(FileValidateProperties, result.metadata)
        return None

    async def _prepare_file_object_newly(self,
                                         file: RawFile,
                                         user_id: ObjectId,
                                         file_size: int,
                                         file_extension: str,
                                         **kwargs) -> FileCreate:
        """Prepare file object for creating new file."""
        return FileCreate(
            author_id=user_id,
            file_name=kwargs.get('file_name') or file.filename,
            folder_path=None,
            size=file_size,
            extension=file_extension
        )

    async def _prepare_file_object_update(self,
                                          file: RawFile,
                                          file_size: int,
                                          file_extension: str,
                                          **kwargs) -> FileUpdate:
        """Prepare file object for updating existing file."""
        return FileUpdate(
            file_name=kwargs.get('file_name') or file.filename,
            folder_path=None,
            size=file_size,
            extension=file_extension,
            error=None
        )


class FileCreateer(BaseFileCreateer):

    async def build_target_directory(self, mongo_file: MongoFile) -> str:
        return f'document_files/{mongo_file.id}'


class FileCreaterProvider:

    @staticmethod
    def create(crud, storage=None, broker_service=None) -> BaseFileCreateer:
        return FileCreateer(
            crud=crud,
            storage=storage or DatabricksStorage(),
            broker_service=broker_service
        )


