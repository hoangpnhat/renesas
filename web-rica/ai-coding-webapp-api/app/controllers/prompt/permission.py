from typing import cast
from models.prompt import Prompt as MongoPrompt
from schemas.prompt import PromptResponse
from services.validator import BasePermissionValidator
from utils.handle_exception import ApplicationException


class PromptValidator(BasePermissionValidator[MongoPrompt]):

    async def validate_ownership(self, item: MongoPrompt | PromptResponse, user_id: str) -> MongoPrompt:
        if str(item.owner_id) != str(user_id):
            raise ApplicationException(
                "PERMISSION_DENIED",
            )
        return cast(MongoPrompt, item)
