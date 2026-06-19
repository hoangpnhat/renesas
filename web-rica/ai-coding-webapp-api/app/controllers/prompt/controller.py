from typing import Optional, Union, cast

from fastapi import UploadFile as RawFile
from loguru import logger
from odmantic import ObjectId
from starlette.responses import StreamingResponse

from const.common import CommonStatus, PromptType, ResourceType
from controllers.file.file_uploader import BaseFileCreateer
from controllers.prompt.permission import PromptValidator
from controllers.prompt.resolver import PromptResolver
from crud import crud_prompt
from crud.refactor_base.mixins.pagination.models import CommonRequest, PaginationResponse, ComprehensiveRequest
from models.file import Files as MongoFile
from models.prompt import Prompt as MongoPrompt
from schemas import VerifiedUser
from schemas.prompt import PromptResponse, PromptCreate, PromptUpdate, PromptFileInfo, PromptTruncateResponse, \
    PromptShareRequest
from schemas.share import ChildrenResource
from services.storage import DatabricksStorage
from services.taskiq.broker import broker
from services.validator.file import PromptBusinessRuleValidator
from utils.decorators import cache_result, require_user_validator
from utils.handle_exception import ApplicationException


class PromptController:
    def __init__(
            self,
            verified_user: VerifiedUser | None,
            resolver: PromptResolver,
            validator: PromptValidator
    ):
        self.crud = crud_prompt.crud_prompt
        self.verified_user = verified_user

        self.validator = validator
        self.resolver = resolver

    async def fetch_prompt_by_id(self, prompt_id: ObjectId | str) -> MongoPrompt:
        prompt = await self.crud.get_by_id(id=prompt_id)
        if not prompt:
            raise ApplicationException('NOT_EXISTED_ERROR')
        return prompt

    @cache_result(prefix="prompt")
    async def get_prompt_identity(
            self,
            element: Union[MongoPrompt, ObjectId, str],
            **kwargs
    ) -> PromptResponse:
        if not isinstance(element, MongoPrompt):
            element = await self.fetch_prompt_by_id(prompt_id=element)

        return await self.resolver.resolve(
            prompt=element,
            lookups=kwargs.get('lookups'),
            overrides=kwargs.get('overrides', {})
        ) 

    #  CRUD operations
    @require_user_validator(return_fields=['id', 'full_name'])
    async def create_prompt(
            self,
            prompt_file: RawFile,
            prompt_data: PromptCreate,
            **kwargs
    ) -> PromptResponse:
        curr_user_id = kwargs.pop('id')
        curr_user_name = kwargs.pop('full_name')
        prompt_data.owner_id = curr_user_id

        prompt_obj = MongoPrompt(**prompt_data.model_dump(exclude_unset=True))

        uploader = PromptFileCreater(crud=self.crud, broker_service=broker)
        prompt_file_info = await uploader.process_upload(
            prompt_file=prompt_file,
            validator=PromptBusinessRuleValidator,
            entity_id=prompt_obj.id,
        )
        # Update prompt object with file info
        prompt_obj.prompt_file_info = prompt_file_info

        created_prompt = await self.crud.create(prompt_obj)
        overrides = {
            "user": curr_user_name
        }
        return cast(PromptResponse, await self.get_prompt_detail(prompt=created_prompt, overrides=overrides))

    @require_user_validator()
    async def get_prompt_detail(
            self,
            prompt: Union[MongoPrompt, ObjectId, str],
            **kwargs
    ) -> PromptResponse:
        curr_user_id = str(kwargs.pop('id'))

        prompt = await self.get_prompt_identity(element=prompt, **kwargs)
        if isinstance(prompt, dict):  # type: ignore[unreachable]
            prompt = PromptResponse(**prompt)  # type: ignore[unreachable]

        # ensure the user has read permission, enrich with permission info
        if not await self.validator.can_perform(prompt, curr_user_id, "read"):
            raise ApplicationException("PERMISSION_DENIED")

        # user-dependent identity: enrich permissions
        permissions = await self.validator.enrich_permissions(prompt, curr_user_id)

        # Update prompt with all permission fields
        for key, value in permissions.items():
            setattr(prompt, key, value)
        return cast(PromptResponse, prompt)

    @require_user_validator()
    async def update_prompt(
            self,
            prompt_id: Union[ObjectId, str],
            update_data: PromptUpdate,
            prompt_file: Optional[RawFile] = None,
            **kwargs
    ) -> PromptResponse:
        curr_user_id = kwargs.pop('id')

        existing_prompt = await self.fetch_prompt_by_id(prompt_id=prompt_id)
        await self.validator.validate_ownership(existing_prompt, curr_user_id)

        if prompt_file:
            uploader = PromptFileCreater(crud=self.crud, broker_service=broker)
            prompt_file_info = await uploader.process_upload(
                prompt_file=prompt_file,
                validator=PromptBusinessRuleValidator,
                entity_id=existing_prompt.id,
            )
            update_data.prompt_file_info = prompt_file_info

        updated_prompt = await self.crud.update(db_obj=existing_prompt, obj_in=update_data)
        return cast(PromptResponse, await self.get_prompt_detail(prompt=updated_prompt))

    @require_user_validator()
    async def delete_prompt(
            self,
            prompt_id: Union[ObjectId, str],
            **kwargs
    ) -> None:
        curr_user_id = kwargs.pop('id')
        prompt = await self.fetch_prompt_by_id(prompt_id=prompt_id)
        await self.validator.validate_ownership(prompt, curr_user_id)
        await self.crud.update(
            db_obj=prompt,
            obj_in=PromptUpdate(status=CommonStatus.DELETED)
        )
        logger.info(
            f"Prompt deleted | prompt_id={prompt_id} | "
            f"name='{prompt.name}' | type={prompt.type} | "
            f"performed_by={curr_user_id}"
        )

    @require_user_validator()
    async def get_prompts(
            self,
            request: CommonRequest | ComprehensiveRequest,
            prompt_type: PromptType,
            visibility: Optional[str] = None,
            query: Optional[str] = None,
            truncated_form: bool = True,
            **kwargs
    ) -> PaginationResponse:
        curr_user_id = kwargs.pop('id')

        results, _ = await self.crud.paginate(
            user_id=curr_user_id,
            request=request,
            query=query,
            prompt_type=prompt_type,
            visibility=visibility,
        )

        if results.data:
            if not truncated_form:
                lookups = await self.resolver._build_lookups(results.data)
                results.data = [
                    await self.get_prompt_detail(p, lookups=lookups, overrides={})
                    for p in results.data
                ]
            else:
                results.data = [
                    PromptTruncateResponse(
                        id=str(p.id),
                        name=p.name,
                        description=p.description,
                        created_at=p.created_at,
                        last_modified=p.last_modified
                    )
                    for p in results.data
                ]

        return results

    @require_user_validator()
    async def share_prompt(
            self,
            prompt_id: Union[ObjectId, str],
            share_request: list[PromptShareRequest],
            **kwargs
    ):
        curr_user_id = kwargs.pop('id')
        prompt = await self.fetch_prompt_by_id(prompt_id=prompt_id)
        await self.validator.validate_ownership(prompt, curr_user_id)

        shareable_type = (
            ResourceType.RULE if prompt.type == PromptType.RULE
            else ResourceType.PROMPT
        )

        logger.info(f"Shareable type {shareable_type}")

        created_share = await self.resolver._process_share_changes(prompt.id, curr_user_id, share_request,
                                                                   shareable_type=shareable_type)
        return created_share

    @require_user_validator()
    async def download_prompt(
            self,
            prompt_id: Union[ObjectId, str],
            **kwargs
    ) -> StreamingResponse:
        curr_user_id = kwargs.pop("id")

        prompt = await self.fetch_prompt_by_id(prompt_id)
        if not await self.validator.ensure_has_permission(prompt, curr_user_id, "read"):
            raise ApplicationException("PERMISSION_DENIED")

        if not prompt.prompt_file_info or not prompt.prompt_file_info.prompt_file_path:
            raise ApplicationException('FILE_NOT_FOUND')

        storage = DatabricksStorage()

        return StreamingResponse(
            storage.streaming_download(prompt.prompt_file_info.prompt_file_path),
            media_type="application/octet-stream",
            headers={"Content-Disposition": f'attachment; filename="{prompt.prompt_file_info.prompt_file_name}"'}
        )

    async def get_prompt_shares(self, prompt_id: str) -> list[ChildrenResource]:
        """Retrieve all recipients of a prompt"""
        prompt = await self.fetch_prompt_by_id(prompt_id)
        shareable_type = (
            ResourceType.RULE if prompt.type == PromptType.RULE
            else ResourceType.PROMPT
        )
        shares = await self.resolver.get_prompt_share(prompt_id, shareable_type=shareable_type)
        if not shares:
            return []
        resources_map = await self.resolver._resolve_recipients(shares)
        return [ChildrenResource(**v) for v in resources_map.values()]


class PromptFileCreater(BaseFileCreateer):

    async def build_target_directory(self, mongo_file: MongoFile | None = None) -> str:
        return f'{self._storage.BASE_BUCKET}/prompt'

    async def process_upload(  # type: ignore[override]
            self,
            prompt_file: RawFile,
            validator: type[PromptBusinessRuleValidator],
            entity_id: ObjectId,
            upload_name: Optional[str] = None,
            target_dir: Optional[str] = None,
    ) -> PromptFileInfo:
        valid_properties = await self._check_raw_file(validator, prompt_file)
        assert valid_properties is not None

        if target_dir is None:
            target_dir = await self.build_target_directory()

        if not upload_name:
            upload_name = f'{entity_id}.{valid_properties.file_extension}'

        upload_info = await self._upload_to_storage(prompt_file, target_dir, upload_name)
        if not upload_info.status or not upload_info.upload_path:
            raise ApplicationException('STORAGE_UPLOAD_FAILED', desc=upload_info.error)

        return PromptFileInfo(
            prompt_file_name=prompt_file.filename,
            prompt_file_path=upload_info.upload_path,
            prompt_file_extension=valid_properties.file_extension,
            prompt_file_size=valid_properties.size,
        )
