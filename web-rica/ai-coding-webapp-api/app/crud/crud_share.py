from typing import Any

from odmantic import ObjectId, query

from const.common import ResourceType
from models.share import Share as MongoShare
from schemas.share import ShareCreate, ShareUpdate
from .refactor_base.crud_operations import MongoCRUD
from utils.helpers import normalize_id


class CRUDShare(MongoCRUD[MongoShare, ShareCreate, ShareUpdate]):

    async def get_by_id(self, id: str | ObjectId) -> MongoShare | None:
        return await self.single_complex_get(to_query=(
                MongoShare.id == normalize_id(id)  # type: ignore[arg-type]
        ))

    async def _save_multiple_shares(self, shares: list[ShareCreate]):
        return await self.engine.save_all([MongoShare(**share.model_dump()) for share in shares])

    async def _remove_multiple_share(self, remove_lst: list[ObjectId], sharable_id: str,
                                     shareable_type: ResourceType | None = None):
        conditions = [
            query.in_(MongoShare.target_id, remove_lst),
            MongoShare.shareable_id == ObjectId(sharable_id)
        ]
        if shareable_type:
            conditions.append(MongoShare.shareable_type == shareable_type)

        await self.engine.remove(self.model, query.and_(*conditions))  # type: ignore[arg-type]

    async def get_share_for_user(self, user_id: str | ObjectId, resource_id: str | ObjectId) -> MongoShare | None:
        to_query = query.and_(
            MongoShare.target_id == ObjectId(user_id),
            MongoShare.shareable_id == ObjectId(resource_id))
        return await self.single_complex_get(to_query)

    async def get_knowledge_shares(self, knowledge_id: str | ObjectId) -> list[MongoShare]:
        """Get all shares for a knowledge base (both user and group shares)."""
        to_query = query.and_(
            MongoShare.shareable_id == ObjectId(knowledge_id),
            MongoShare.shareable_type == ResourceType.KNOWLEDGE
        )
        return await self.engine.find(self.model, to_query)

    async def get_prompt_shares(self, prompt_id: str | ObjectId) -> list[MongoShare]:
        """Get all shares for a prompt."""
        to_query = query.and_(
            MongoShare.shareable_id == ObjectId(prompt_id),
            MongoShare.shareable_type == ResourceType.PROMPT
        )
        return await self.engine.find(self.model, to_query)

    async def delete_knowledge_share(self, knowledge_id: str | ObjectId, session: Any = None):
        """Delete all shares for a knowledge base."""
        await self.engine.remove(
            self.model,
            query.and_(
                MongoShare.shareable_id == ObjectId(knowledge_id),
                MongoShare.shareable_type == ResourceType.KNOWLEDGE
            ),
            session=session
        )

    async def get_group_knowledge(self, group_ids: list[str | ObjectId]) -> list[MongoShare]:
        return await self.engine.find(
            self.model,
            query.and_(
                MongoShare.target_id.in_(group_ids),  # type: ignore[attr-defined]
                MongoShare.target_type == ResourceType.GROUP,
                MongoShare.shareable_type == ResourceType.KNOWLEDGE
            )
        )

    async def get_kb_shares_for_targets(
            self,
            target_ids: list[ObjectId],
            target_type: str
    ) -> list[MongoShare]:
        from const.common import ResourceType

        query_ = query.and_(
            query.in_(MongoShare.target_id, target_ids),
            MongoShare.target_type == target_type,
            MongoShare.shareable_type == ResourceType.KNOWLEDGE
        )

        shares = await self.engine.find(
            self.model,
            query_
        )

        return list(shares)

    async def get_shares_by_type(
            self,
            shareable_id: str | ObjectId,
            shareable_type: ResourceType
    ) -> list[MongoShare]:
        to_query = query.and_(
            MongoShare.shareable_id == ObjectId(shareable_id),
            MongoShare.shareable_type == shareable_type
        )
        return await self.engine.find(self.model, to_query)

    async def get_prompt_shares_for_groups(self, group_ids: list[str | ObjectId]) -> list[MongoShare]:
        return await self.engine.find(
            self.model,
            query.and_(
                query.in_(MongoShare.target_id, group_ids),
                MongoShare.target_type == ResourceType.GROUP,
                query.or_(
                    MongoShare.shareable_type == ResourceType.PROMPT,
                    MongoShare.shareable_type == ResourceType.RULE
                )
            )
        )

    async def get_prompt_shares_for_targets(
            self,
            target_ids: list[ObjectId],
            target_type: str
    ) -> list[MongoShare]:
        return await self.engine.find(
            self.model,
            query.and_(
                query.in_(MongoShare.target_id, target_ids),
                MongoShare.target_type == target_type,
                query.or_(
                    MongoShare.shareable_type == ResourceType.PROMPT,
                    MongoShare.shareable_type == ResourceType.RULE
                )
            )
        )


share_crud = CRUDShare(model=MongoShare)
