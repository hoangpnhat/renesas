from fastapi import APIRouter, Depends, Path, Query, Request, UploadFile, Form
from fastapi.responses import Response, StreamingResponse
from schemas.file import FileInDB, FilePreviewMetaResponse
from dependencies import FileControllerDep
from utils.decorators import invalidate_endpoint_cache, log_api
from crud.refactor_base.mixins import CommonRequest, ComprehensiveRequest
from schemas.base_schema import CallBack, CallbackResult

router = APIRouter()

@router.post("", response_model=FileInDB)
@log_api
async def incremental_upload(
    controller: FileControllerDep,
    file: UploadFile,
    filename: str | None = Form(None),
):
    return await controller.check_duplicate_with_auto_create(
        file=file,
        filename=filename,
    )

@router.get("")
@log_api
async def getting_files(
                    controller: FileControllerDep,
                    request: CommonRequest = Depends()
                    ):

    return await controller.list_files(request=request)

@router.post("/search")
@log_api
async def search_files(
                    controller: FileControllerDep,
                    request: ComprehensiveRequest
                    ):

    return await controller.list_files(request=request)

@router.get("/{file_id}/preview", response_model=FilePreviewMetaResponse)
@log_api
async def getting_file_preview_meta(
    file_id: str,
    controller: FileControllerDep,
):
    return await controller.get_file_preview_meta(element=file_id)


@router.get("/{file_id}/download")
@log_api
async def downloading_file(file_id: str, controller: FileControllerDep):
    stream, file_name = await controller.download_file(file_id=file_id)
    return StreamingResponse(
        stream,
        media_type="application/octet-stream",
        headers={
            "Content-Disposition": f'attachment; filename="{file_name}"',
        },
    )


@router.get("/{file_id}/preview/{page_num}")
@log_api
async def getting_file_preview_page(
    file_id: str,
    request: Request,
    controller: FileControllerDep,
    page_num: int = Path(..., gt=0)
):
    etag = f'"{file_id}-{page_num}"'
    if request.headers.get("if-none-match") == etag:
        return Response(status_code=304)

    image_bytes, content_type = await controller.get_file_preview_page(
        file_id=file_id,
        page_num=page_num,
    )
    return Response(
        content=image_bytes,
        media_type=content_type,
        headers={
            "ETag": etag,
            "Cache-Control": "private, max-age=3600, immutable",
        },
    )


@router.get("/{file_id}")
@log_api
async def getting_file(
                    file_id: str,
                    controller: FileControllerDep):
    return await controller.get_file_details(element = file_id)


@router.delete("/{file_id}", status_code=204)
@log_api
@invalidate_endpoint_cache(prefix="file", path_param="file_id")
async def deleting_a_file( 
                            file_id: str,
                            controller: FileControllerDep):
    return await controller.delete_file(file_id = file_id)

@router.post("/callback", response_model=CallbackResult)
@log_api
async def calling_back(
                body: CallBack,
                controller: FileControllerDep):
    return await controller.callback(body = body)
