from typing import Optional

from fastapi import APIRouter, UploadFile, Form, Depends, Query, status, File

from const.common import PromptType
from crud.refactor_base.mixins.pagination.models import PaginationResponse
from dependencies import PromptControllerDep
from schemas.prompt import PromptCreate, PromptUpdate, PromptShareRequest, PromptResponse, PromptCommonRequest
from utils.decorators import log_api, invalidate_endpoint_cache

router = APIRouter()


@router.post("", response_model=PromptResponse)
@log_api
async def creating_prompt(
        controller: PromptControllerDep,
        prompt_file: UploadFile,
        prompt_type: PromptType = Form(...),
        name: str = Form(...),
        description: str | None = Form(None),
):
    return await controller.create_prompt(
        prompt_file=prompt_file,
        prompt_data=PromptCreate(name=name, description=description, type=prompt_type)
    )


@router.get("", response_model=PaginationResponse)
@log_api
async def getting_prompts(
        controller: PromptControllerDep,
        request: PromptCommonRequest = Depends(),
        prompt_type: PromptType = Query(...),
        visibility: Optional[str] = Query(None),
        query: Optional[str] = Query(None)
):
    return await controller.get_prompts(request=request, prompt_type=prompt_type, visibility=visibility, query=query)


@router.get("/download/{prompt_id}", response_model=None)
@log_api
async def downloading_prompt(
        prompt_id: str,
        controller: PromptControllerDep,
):
    return await controller.download_prompt(prompt_id=prompt_id)


@router.get("/{prompt_id}", response_model=PromptResponse)
@log_api
async def getting_prompt(
        prompt_id: str,
        controller: PromptControllerDep,
):
    return await controller.get_prompt_detail(prompt=prompt_id)


@router.get("/{prompt_id}/share")
@log_api
async def getting_prompt_share(
        prompt_id: str,
        controller: PromptControllerDep
):
    return await controller.get_prompt_shares(
        prompt_id
    )


@router.patch("/{prompt_id}")
@log_api
@invalidate_endpoint_cache(prefix="prompt", path_param="prompt_id")
async def updating_prompt(
        prompt_id: str,
        controller: PromptControllerDep,
        prompt_file: Optional[UploadFile] = File(None),
        name: str | None = Form(None),
        description: str | None = Form(None),
):
    return await controller.update_prompt(
        prompt_id=prompt_id,
        prompt_file=prompt_file,
        update_data=PromptUpdate(
            name=name,
            description=description,
        )
    )


@router.delete("/{prompt_id}", status_code=status.HTTP_204_NO_CONTENT)
@log_api
@invalidate_endpoint_cache(prefix="prompt", path_param="prompt_id")
async def deleting_prompt(
        prompt_id: str,
        controller: PromptControllerDep,
):
    return await controller.delete_prompt(prompt_id=prompt_id)


@router.patch("/{prompt_id}/share", status_code=status.HTTP_204_NO_CONTENT)
@log_api
@invalidate_endpoint_cache(prefix="prompt", path_param="prompt_id")
async def sharing_prompt(
        prompt_id: str,
        share_request: list[PromptShareRequest],
        controller: PromptControllerDep,
):
    return await controller.share_prompt(prompt_id, share_request)
