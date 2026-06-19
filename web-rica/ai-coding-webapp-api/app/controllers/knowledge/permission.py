from services.validator import BasePermissionValidator

class KnowledgePermissionValidator(BasePermissionValidator):

    async def validate_ownership(self, item, user_id):
        pass
