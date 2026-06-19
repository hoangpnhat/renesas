from odmantic import ObjectId
from services.validator import BasePermissionValidator
from services.file_access import get_user_file_context
from schemas.file import FileInDB
from models.file import Files as MongoFile
from utils.handle_exception import ApplicationException


class FileValidator(BasePermissionValidator):

    async def validate_can_access(self, file: MongoFile | FileInDB, user_id: ObjectId | str) -> bool:
        """
        Check if user can access the file (as owner or via shared access).
        
        Returns True if accessible, raises ApplicationException otherwise.
        """
        user_id = ObjectId(str(user_id))

        # Direct owner
        if str(file.author_id) == str(user_id):
            return True

        # Check shared knowledge files
        ctx = await get_user_file_context(user_id)
        accessible = {str(fid) for fid in ctx["accessible_files"]}
        if str(file.id) in accessible:
            return True

        raise ApplicationException("PERMISSION_DENIED")

    async def validate_ownership(self, file: MongoFile | FileInDB, user_id: ObjectId | str) -> bool:
        user_id = ObjectId(str(user_id))
        return str(file.author_id) == str(user_id)