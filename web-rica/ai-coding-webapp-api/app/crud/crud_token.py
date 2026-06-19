from __future__ import annotations
import __version__
from motor.core import AgnosticDatabase
from crud.base import CRUDBase
from models import User, Token
from schemas import RefreshTokenCreate, RefreshTokenUpdate
from core.config import settings


class CRUDToken(CRUDBase[Token, RefreshTokenCreate, RefreshTokenUpdate]):
    # Everything is user-dependent
    async def create(  # type: ignore[override]
        self,  *, obj_in: str, user_obj: User
    ) -> Token:
        db_obj = None
        try:
            db_obj = await self.engine.find_one(self.model, self.model.token == obj_in)
        except:
            db_obj = None
        if db_obj:
            if db_obj.authenticates_id != user_obj.id:
                raise ValueError("Token mismatch between key and user.")
            return db_obj
        else:
            new_token = self.model(token=obj_in, authenticates_id=user_obj)  # type: ignore[call-arg]
            user_obj.refresh_tokens.append(new_token.id)
            await self.engine.save_all([new_token, user_obj])
            return new_token

    async def get(self, *, user: User, token: str) -> Token | None:  # type: ignore[override]
        return await self.engine.find_one(  # type: ignore[return-value]
            User, ((User.id == user.id) & (User.refresh_tokens == token))
        )

    async def get_multi(  # type: ignore[override]
        self, *, user: User, page: int = 0, page_break: bool = False
    ) -> list[Token]:
        if page_break:
            return await self.engine.find(
                User, (User.refresh_tokens.in_([user.refresh_tokens])),  # type: ignore[attr-defined, arg-type]
                skip=page * settings.MULTI_MAX,
                limit=settings.MULTI_MAX,
            )
        return await self.engine.find(
            User, (User.refresh_tokens.in_([user.refresh_tokens])),  # type: ignore[attr-defined, arg-type]
        )

    async def remove(self,  *, db_obj: Token) -> None:
        users = []
        async for user in self.engine.find(User, User.refresh_tokens.in_([db_obj.id])):  # type: ignore[attr-defined]
            user.refresh_tokens.remove(db_obj.id)
            users.append(user)
        await self.engine.save(users)  # type: ignore[type-var]
        await self.engine.delete(db_obj)


token = CRUDToken(Token)
