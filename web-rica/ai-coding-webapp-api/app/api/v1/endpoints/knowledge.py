from fastapi import APIRouter, Depends
from schemas.knowledge import (
    KnowledgeCreate,
    KnowledgeUpdateGeneral,
    KnowledgeShareRequest,
    KnowledgeResponse,
    KnowledgeRequest,
    KnowledgeComprehensiveRequest
)
from odmantic import ObjectId
from utils.decorators import log_api, invalidate_endpoint_cache
from crud.refactor_base.mixins import PaginationResponse
from dependencies import KnowledgeControllerDep
router = APIRouter()

@router.get("", response_model=PaginationResponse)
@log_api
async def getting_list_knowledge(
    controller: KnowledgeControllerDep,
    request: KnowledgeRequest = Depends(),
    query: str | None = None
):
    return await controller.get_list_knowledge(request=request, query = query)

@router.post("", response_model=KnowledgeResponse)
@log_api
async def creating_knowledge_base(
    knowledge_base: KnowledgeCreate,
    controller: KnowledgeControllerDep,
):
    return await controller.create_knowledge(knowledge_base)

@router.post("/search", response_model=PaginationResponse)
@log_api
async def searching_list_knowledge(
    controller: KnowledgeControllerDep,
    request: KnowledgeComprehensiveRequest,
):
    return await controller.get_list_knowledge(request=request)

@router.get("/{knowledge_base_id}/share")
@log_api
async def getting_knowledge_share(
    knowledge_base_id: ObjectId,
    controller: KnowledgeControllerDep
):
    return await controller.get_knowledge_shares(str(knowledge_base_id))

@router.post("/{knowledge_base_id}/share", status_code=204)
@log_api
@invalidate_endpoint_cache(prefix="knowledge", path_param="knowledge_base_id")
async def share_knowledge_base(
    knowledge_base_id: ObjectId,
    share_request: list[KnowledgeShareRequest],
    controller: KnowledgeControllerDep
):
    return await controller.share_knowledge(
        knowledge_base_id, share_request
    )

@router.put("/{knowledge_base_id}/general", response_model=KnowledgeResponse)
@log_api
@invalidate_endpoint_cache(prefix="knowledge", path_param="knowledge_base_id")
async def setting_knowledge_general(
    knowledge_base_id: ObjectId,
    update_data: KnowledgeUpdateGeneral,
    controller: KnowledgeControllerDep,
):
    return await controller.set_knowledge_general(
        knowledge_base_id, update_data
    )

@router.post("/{knowledge_base_id}/archived", status_code=204)
@log_api
@invalidate_endpoint_cache(prefix="knowledge", path_param="knowledge_base_id")
async def archive_knowledge_base(
    knowledge_base_id: ObjectId,
    controller: KnowledgeControllerDep
):
    return await controller.delete_or_archived_knowledge(knowledge_id=knowledge_base_id,
                                            is_archived=True)

@router.get("/{knowledge_base_id}")
@log_api
async def getting_knowledge(
    knowledge_base_id: ObjectId,
    controller: KnowledgeControllerDep
):
    return await controller.get_knowledge(
        element = knowledge_base_id
    )

@router.delete("/{knowledge_base_id}", status_code=204)
@log_api
@invalidate_endpoint_cache(prefix="knowledge", path_param="knowledge_base_id")
async def delete_knowledge_base(
    knowledge_base_id: ObjectId,
    controller: KnowledgeControllerDep
):
    return await controller.delete_or_archived_knowledge(knowledge_id=knowledge_base_id)
