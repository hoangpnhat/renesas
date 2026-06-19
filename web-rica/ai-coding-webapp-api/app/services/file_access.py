from odmantic import ObjectId
from loguru import logger
from const.common import OwnerType
from caching.redis import _redis_client as redis_cli

_CACHE_PREFIX = "file-access-ctx"
_CACHE_TTL = 300


async def get_user_file_context(user_id: ObjectId) -> dict:
    """
    Returns user_id and the list of file IDs accessible to the user:
      1. Files owned by the user (checked separately by callers)
      2. Files from knowledge bases shared with the user's groups
      3. Files from knowledge bases shared directly with the user
    """
    assert redis_cli is not None
    cache_key = f"{_CACHE_PREFIX}:{user_id}"

    cached = await redis_cli.get(cache_key)
    if cached and "accessible_files" in cached:
        cached["accessible_files"] = [ObjectId(fid) for fid in cached.get("accessible_files", [])]
        return cached

    from crud import group_crud, share_crud, knowledge_crud

    accessible_file_ids = []
    try:
        knowledge_ids: set[ObjectId] = set()

        user_groups = await group_crud.get_user_groups(user_id)
        group_ids = [g.id for g in user_groups]

        if group_ids:
            group_kb_shares = await share_crud.get_group_knowledge(group_ids)
            logger.debug(f"Found {len(group_kb_shares)} knowledge shares for user's groups")
            knowledge_ids.update(s.shareable_id for s in group_kb_shares)

        user_kb_shares = await share_crud.get_kb_shares_for_targets(
            target_ids=[user_id], target_type=OwnerType.USER
        )
        logger.debug(f"Found {len(user_kb_shares)} knowledge shares directly for user")
        knowledge_ids.update(s.shareable_id for s in user_kb_shares)

        if knowledge_ids:
            kbs = await knowledge_crud.bulk_lookup(list(knowledge_ids))
            for kb in kbs:
                for fj in kb.files:
                    accessible_file_ids.append(fj.file_id)

    except Exception as e:
        logger.error(f"Error getting user file access context: {e}")

    result = {"user_id": user_id, "accessible_files": accessible_file_ids}

    # Cache result
    await redis_cli.set(
        cache_key,
        {"user_id": str(user_id), "accessible_files": [str(fid) for fid in accessible_file_ids]},
        _CACHE_TTL,
    )

    return result


async def invalidate_user_file_context(user_id: ObjectId | str) -> None:
    """Call this when group membership or KB shares change for a user."""
    assert redis_cli is not None
    await redis_cli.delete(f"{_CACHE_PREFIX}:{user_id}")
