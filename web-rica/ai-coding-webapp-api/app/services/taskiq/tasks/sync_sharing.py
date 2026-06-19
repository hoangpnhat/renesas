from typing import Annotated, Any, Optional
from bson import ObjectId
from odmantic import query
from loguru import logger
from taskiq import Context, TaskiqDepends

from const.notification import (
    NotificationType, 
    DEFAULT_GROUP_INVITATION, 
    DEFAULT_GROUP_REMOVAL)
from const.common import ResourceType, OwnerType
from crud import share_crud, notification_crud, file_crud
from models.share import Share

async def sync_update_files_to_knowledge(
    modifier_id: str,
    file_ids: list[str],
    knowledge_id: str,
    task_id: str | None = None,
    session: Any = None,
) -> bool:
    """Task to handle downstream processing when knowledge files change.

    This is a lightweight placeholder that should be extended to trigger
    indexing, embedding, or any other file->knowledge sync operations.
    """
    try:
        logger.info(
            "[Task %s] sync_update_files_to_knowledge: modifier=%s knowledge=%s files=%d",
            task_id,
            modifier_id,
            knowledge_id,
            len(file_ids or []),
        )

        # Placeholder: extend with real file->knowledge processing as needed.
        return True

    except Exception as e:
        logger.exception(f"[Task {task_id}] Failed to sync files to knowledge {knowledge_id}: {e}")
        return False

async def sync_delete_knowledge_shares(
    knowledge_id: str,
    context: Annotated[Context, TaskiqDepends()] | None = None
) -> bool:
    
    task_id = context.message.task_id if context else None
    try:
        await share_crud.delete_knowledge_share(knowledge_id)
        logger.info(f"[Task {task_id}] Revoked shares for knowledge {knowledge_id}")
        return True

    except Exception as e:
        logger.exception(f"[Task {task_id}] Failed to revoke shares for knowledge {knowledge_id}: {e}")
        return False

async def sync_file_delete(
    file_id: str,
    context: Annotated[Context, TaskiqDepends()] | None = None
) -> bool:
    """Remove references from knowledge bases + ES index docs"""

    task_id = context.message.task_id if context else None
    try:
        from odmantic.bson import ObjectId
        from models.knowledge import Knowledge as MongoKnowledge
        from crud.crud_knowledge import knowledge_crud

        # Remove references from knowledge bases
        filter_q = {"files.file_id": ObjectId(file_id)}
        affected_knowledge = await knowledge_crud.engine.find(MongoKnowledge, filter_q)

        for kb in affected_knowledge or []:
            original_count = len(kb.files or [])
            kb.files = [f for f in (kb.files or []) if str(f.file_id) != str(file_id)]
            if len(kb.files) != original_count:
                # Overwrite old files
                await knowledge_crud.transaction_wrapper(knowledge_crud.update, db_obj=kb, obj_in={"files": kb.files})
                logger.info(f"[Task {task_id}] Removed file {file_id} from knowledge {str(kb.id)}")


        # TODO: remove file documents from Elasticsearch index
        # try:
        #     # Lazy import to avoid tight coupling
        #     from services.ElasticSearchVector import get_elasticsearch_service
        #     es_service = get_elasticsearch_service()
        #     # Use delete_by_query to remove documents that reference this file_id in metadata
        #     query = {"query": {"term": {"metadata.file_id": file_id}}}
        #     resp = es_service.es.delete_by_query(index=es_service.index_name, body=query)
        #     logger.info(f"[Task {task_id}] Elasticsearch delete_by_query response: {resp}")
        # except Exception as es_exc:
        #     # Best-effort: don't fail the whole task if ES delete fails
        #     logger.exception(f"[Task {task_id}] Failed to delete ES documents for file {file_id}: {es_exc}")

        return True

    except Exception as e:
        logger.exception(f"[Task {task_id}] Failed to delete file {file_id}: {e}")
        return False

async def sync_knowledge_members(
        removed_members: list[str],
        knowledge_id: str
):
    await share_crud.engine.remove(
        Share,
        query.and_(
            query.in_(Share.target_id, [ObjectId(rm) for rm in removed_members]),
            Share.shareable_id == ObjectId(knowledge_id)
        )
    )