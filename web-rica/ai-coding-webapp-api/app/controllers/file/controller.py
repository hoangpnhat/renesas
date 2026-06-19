from typing import Optional, cast

from fastapi import UploadFile as RawFile
from loguru import logger
from odmantic import ObjectId

from caching.redis import _redis_client as redis_cli
from const.common import CommonStatus
from controllers.file import FileResolver
from controllers.file import FileValidator
from controllers.file.file_uploader import FileCreaterProvider
from core.config import settings
from crud.crud_file import file_crud
from crud.refactor_base.mixins import CheckDuplicateEntity, ContextDataModel
from crud.refactor_base.mixins import ComprehensiveRequest, PaginationResponse, CommonRequest
from models.file import Files as MongoFile
from schemas.base_schema import CallBack, CallbackResult
from schemas.file import FileInDB, FileCreate, FileUpdate, FilePreviewMetaResponse
from schemas.user import VerifiedUser
from services.storage import DatabricksStorage
from services.taskiq.broker import broker
from services.validator import FileBusinessRuleValidator
from utils.decorators import cache_result, require_user_validator
from utils.handle_exception import ApplicationException
from utils.helpers import datetime_now_sec, build_file_path

PREFIX = 'file'


class FileController:
    def __init__(self,
                 verified_user: VerifiedUser | None,
                 resolver: FileResolver,
                 validator: FileValidator):
        self.verified_user = verified_user
        self.crud = file_crud

        # supporter
        self.validator = validator
        self.resolver = resolver

    async def fetch_file_by_id(self, file_id: str, raise_error: bool = True) -> MongoFile | None:
        file_data = await self.crud.get_by_id(id=file_id)
        if not file_data and raise_error:
            raise ApplicationException('NOT_EXISTED_ERROR')
        else:
            return file_data

    @cache_result(prefix=PREFIX)
    async def get_file_identity(self, element: MongoFile | str | ObjectId, **kwargs) -> FileInDB:
        mf: MongoFile | None
        if isinstance(element, (str, ObjectId)):
            mf = cast(MongoFile, await self.fetch_file_by_id(file_id=str(element)))
        else:
            mf = element
        return await self.resolver.resolve(mf, kwargs.get('lookups'), kwargs.get('overrides'))

    @require_user_validator()
    async def get_file_details(self, element: MongoFile | str | ObjectId, **kwargs) -> FileInDB:
        curr_user_id = kwargs.get('id')
        if curr_user_id is None:
            raise ApplicationException("PERMISSION_DENIED")
        file = await self.get_file_identity(element=element, **kwargs)
        if isinstance(file, dict):
            file = FileInDB(**file)
        # real-time ownership check
        file.is_owner = await self.validator.validate_ownership(file, curr_user_id)
        status, modified = self.resolver.resolve_status(file, datetime_now_sec())

        file.status = status
        file.modified = modified
        return cast(FileInDB, file)

    @require_user_validator()
    @cache_result(prefix=f"{PREFIX}-preview-meta")
    async def get_file_preview_meta(self, element: str, **kwargs) -> FilePreviewMetaResponse:

        file = cast(MongoFile, await self.fetch_file_by_id(file_id=element))

        if file.status != CommonStatus.COMPLETED:
            raise ApplicationException('FILE_NOT_READY')

        result = FilePreviewMetaResponse(
            file_id=element,
            total_pages=file.total_pages or 0,
        )
        return result

    @require_user_validator()
    async def download_file(self, file_id: str, **kwargs):
        curr_user_id = kwargs.get('id')
        if curr_user_id is None:
            raise ApplicationException("PERMISSION_DENIED")
        file = cast(MongoFile, await self.fetch_file_by_id(file_id=file_id))

        if file.status != CommonStatus.COMPLETED:
            raise ApplicationException('FILE_NOT_READY')

        await self.validator.validate_can_access(file=file, user_id=curr_user_id)

        file_path = build_file_path(file)
        storage = DatabricksStorage()

        return storage.streaming_download(file_path), file.file_name

    @require_user_validator()
    async def get_file_preview_page(
            self,
            file_id: str,
            page_num: int,
            **kwargs,
    ) -> tuple[bytes, str]:

        curr_user_id = kwargs.get('id')
        if curr_user_id is None:
            raise ApplicationException("PERMISSION_DENIED")
        file = cast(MongoFile, await self.fetch_file_by_id(file_id=file_id))

        if file.status != CommonStatus.COMPLETED:
            raise ApplicationException(
                'FILE_NOT_READY',
                detail=f"File status is {file.status}, not ready for preview")

        await self.validator.validate_can_access(file=file, user_id=curr_user_id)

        if file.total_pages and page_num > file.total_pages:
            raise ApplicationException('NOT_EXISTED_ERROR')

        img_ext = settings.pipeline.PIPELINE_IMAGE_FORMAT.lower()
        storage = DatabricksStorage()
        image_path = (
            f"{file.folder_path}/"
            f"/page_{page_num}.{img_ext}"
        )

        try:
            image_bytes = await storage.download(image_path)
        except Exception as e:
            logger.warning(f"[preview] page {page_num} of file {file_id} not found: {e}")
            raise ApplicationException('NOT_EXISTED_ERROR')

        return image_bytes, f"image/{img_ext}"

    @require_user_validator()
    async def list_files(self,
                         request: CommonRequest | ComprehensiveRequest,
                         **kwargs) -> PaginationResponse:

        curr_user_id = kwargs.get('id')
        files_page, _ = await self.crud.paginate(user_id=curr_user_id, request=request)
        if not files_page.data:
            return files_page
        else:
            lookups = await self.resolver._build_lookups(files_page.data)
            resolved_files = await self._resolve_files_batch(files_page.data, lookups, **kwargs)
            files_page.data = resolved_files
            return files_page

    async def _resolve_files_batch(self, files: list[MongoFile], lookups: dict, **kwargs) -> list[FileInDB]:
        """Resolve multiple files in batch."""
        resolved = []
        for file in files:
            resolved_file = await self.get_file_details(element=file, lookups=lookups, **kwargs)
            resolved.append(resolved_file)
        return resolved

    @require_user_validator(return_fields=['id', 'email'])
    async def check_duplicate_with_auto_create(self,
                                               file: RawFile,
                                               file_name: Optional[str] = None,
                                               **kwargs) -> FileInDB:
        """Check duplicate file and auto-create incrementally if duplicate."""

        curr_user_id: ObjectId = kwargs.pop('id')
        user_email = kwargs.pop('email', None)
        file_name = file_name or file.filename
        if file_name is None:
            raise ApplicationException('INVALID_CONTENT', desc='File name is required')
        context_data = await self._check_for_duplicate(curr_user_id, file_name)
        duplicate_info = context_data.duplicate_check

        # duplicate found
        if duplicate_info.duplicate_exists:
            # Determine new file name (if creating with unique name)
            process_result = await self.crud.process_duplicate_context(
                context_data=context_data,
                is_replace=False
            )

            # Create new file with unique name
            if process_result.name_changed is None:
                raise ApplicationException('INVALID_CONTENT', desc='Could not derive unique file name')
            newly_file = await self._handle_upload_and_sync(
                file=file,
                user_id=curr_user_id,
                file_name=process_result.name_changed,
                user_email=user_email
            )
            return cast(FileInDB, await self.get_file_details(element=newly_file, id=curr_user_id))

        # No duplicate found
        newly_file = await self._handle_upload_and_sync(
            file=file,
            user_id=curr_user_id,
            file_name=file_name,
            user_email=user_email
        )
        return cast(FileInDB, await self.get_file_details(element=newly_file, id=curr_user_id))

    async def _check_for_duplicate(self, user_id: ObjectId, file_name: str) -> ContextDataModel:
        """Check if file already exists for user."""
        file_data = FileCreate(author_id=user_id,
                               file_name=file_name)

        check_fields = CheckDuplicateEntity(check_field='file_name',
                                            constraint_field='author_id',
                                            delete_field='status')
        return await self.crud.check_duplicate_context(obj_in_data=file_data.model_dump(exclude_none=True),
                                                       check_duplicate_fields=check_fields)

    async def _handle_upload_and_sync(self,
                                      file: RawFile,
                                      user_id: ObjectId,
                                      file_name: str,
                                      existing_file: Optional[MongoFile] = None,
                                      **kwargs) -> MongoFile:
        """Auto-create and upload file."""

        uploader = FileCreaterProvider.create(
            crud=self.crud,
            broker_service=broker
        )
        # return await self.crud.transaction_wrapper(func=uploader.process_upload, 
        #                                            file=file, 
        #                                            user_id=user_id, 
        #                                            file_name=file_name, 
        #                                            validator=FileBusinessRuleValidator,
        #                                            existing_file=existing_file,
        #                                            skip_process=True, # update folder path after storage original file done
        #                                            **kwargs)
        return await uploader.process_upload(file=file,
                                             user_id=user_id,
                                             file_name=file_name,
                                             validator=FileBusinessRuleValidator,
                                             existing_file=existing_file,
                                             skip_process=False,  # False means DO process (dispatch background task)
                                             session=None,  # update folder path after storage original file done
                                             **kwargs)

    def _check_lock_file(self, is_replace: bool, status: str):
        if is_replace and status == CommonStatus.PROCESSING.value:
            raise ApplicationException(
                "VERSION_LOCK",
                status_code=400,
                detail="File replacement is not allowed during active processing."
            )

    @require_user_validator()
    async def delete_file(self,
                          file_id: str | ObjectId,
                          **kwargs):

        curr_user_id = kwargs.get("id")
        if curr_user_id is None:
            raise ApplicationException("PERMISSION_DENIED")

        # get existing file
        file = cast(MongoFile, await self.fetch_file_by_id(file_id=str(file_id)))

        # check permission
        is_owner = await self.validator.validate_ownership(file=file, user_id=curr_user_id)
        if not is_owner:
            raise ApplicationException("PERMISSION_DENIED")

        # update status
        await self.crud.update(
            db_obj=file,
            obj_in=FileUpdate(status=CommonStatus.DELETED)
        )
        await broker.send_task('sync_file_delete',
                               file_id=str(file.id))

    async def callback(self, body: CallBack) -> CallbackResult:
        """Handle callback from Databricks after file processing."""
        try:
            # Validate request
            if not body.status or not body.id:
                raise ValueError("Invalid required field: status and id required")

            if not isinstance(body.id, str):
                raise ValueError(f"Invalid field `id`. Required str, got {type(body.id)}")

            # Extract file ID and status
            file_id = body.id.split('_')[0]
            is_success = body.status == 'success'

            if not is_success and not body.error:
                raise ValueError("Missing `error` description for failed job")

            # Fetch file
            file = cast(MongoFile, await self.fetch_file_by_id(file_id=file_id))

            # Prepare update based on success/failure
            if is_success:
                update_data: dict = {"status": CommonStatus.COMPLETED, "error": None}
            else:
                update_data = {"error": body.error}

            # Update database and clear cache
            await self.crud.update(db_obj=file, obj_in=update_data)
            if redis_cli is not None:
                await redis_cli.delete(f'{PREFIX}-{file_id}')

            return CallbackResult()

        except Exception as e:
            logger.error(f"Callback for file failed: {e}")
            return CallbackResult(is_callback_ok=False, error=str(e))
