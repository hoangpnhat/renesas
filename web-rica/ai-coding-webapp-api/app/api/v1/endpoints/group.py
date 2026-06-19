from fastapi import APIRouter, Depends
from schemas.group import (
    GroupResponse, 
    GroupCreate, 
    GroupUpdateGeneral,
    GroupUpdateMember
    )
from dependencies import GroupControllerDep
from utils.decorators import invalidate_endpoint_cache, log_api
from crud.refactor_base.mixins import CommonRequest, ComprehensiveRequest
from crud.refactor_base.mixins import PaginationResponse

router = APIRouter()

@router.post("", response_model=GroupResponse)
@log_api
async def creating_group(
    controller: GroupControllerDep,
    group: GroupCreate,
):
    return await controller.create_group(group=group)

@router.post("/search", response_model=PaginationResponse)
@log_api
async def search_groups(
    controller: GroupControllerDep,
    request: ComprehensiveRequest,
):
    return await controller.get_list_group(request=request,
                                        is_archived=False)

@router.get("/active", response_model=PaginationResponse)
@log_api
async def getting_active_groups(
    controller: GroupControllerDep,
    request: CommonRequest = Depends()
):
    return await controller.get_list_group(request=request,
                                    is_archived = False)

@router.get("/archived", response_model=PaginationResponse)
@log_api
async def getting_archived_groups(
    controller: GroupControllerDep,
    request: CommonRequest = Depends()
):
    return await controller.get_list_group(request=request,
                                    is_archived = True)

@router.post("/{group_id}/members/join", status_code=204)
@log_api
@invalidate_endpoint_cache(prefix="group", path_param="group_id")
async def join_group_member(
    group_id: str,
    controller: GroupControllerDep,
):
    return await controller.accept_invitation(group_id=group_id)

@router.delete("/{group_id}/members/decline", status_code=204)
@log_api
@invalidate_endpoint_cache(prefix="group", path_param="group_id")
async def decline_group_member(
    group_id: str,
    controller: GroupControllerDep,
):
    return await controller.decline_invitation(group_id=group_id)

@router.patch("/{group_id}/general", response_model=GroupResponse)
@log_api
@invalidate_endpoint_cache(prefix="group", path_param="group_id")
async def setting_group_general(
    group_id: str,
    group: GroupUpdateGeneral,
    controller: GroupControllerDep,
):
    return await controller.set_group_general(
        group_id=group_id,
        update_data=group
    )

@router.post("/{group_id}/members", status_code=204)
@log_api
@invalidate_endpoint_cache(prefix="group", path_param="group_id")
async def update_group_member(
    group_id: str,
    members: GroupUpdateMember,
    controller: GroupControllerDep,
):
    return await controller.update_group_members(group_id=group_id,
                                                members=members)

@router.delete("/{group_id}", status_code=204)
@log_api
@invalidate_endpoint_cache(prefix="group", path_param="group_id")
async def delete_group(
    controller: GroupControllerDep,
    group_id: str,
):
    return await controller.delete_or_archive_group(
        group_id=group_id,
        is_archive=False
    )

# @router.post("/{group_id}/archive", status_code=204)
# @log_api
# @invalidate_endpoint_cache(prefix="group", path_param="group_id")
# async def archive_group(
#     controller: GroupControllerDep,
#     group_id: str,
# ):
#     return await controller.delete_or_archive_group(
#         group_id=group_id,
#         is_archive=True
#     )

# @router.post("/{group_id}/restore", status_code=204)
# @log_api
# @invalidate_endpoint_cache(prefix="group", path_param="group_id")
# async def restoring_group(
#     group_id: str,
#     controller: GroupControllerDep,
# ):
#     return await controller.restore_group(
#         group_id=group_id
#     )


@router.get("/{group_id}", response_model=GroupResponse)
@log_api
async def getting_group(
    group_id: str,
    controller: GroupControllerDep,
):
    return await controller.get_group(group=group_id)