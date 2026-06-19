from enum import Enum


class MessageEnum(Enum):
    GET_CONTENT_DIALOG_SUCCESS = "Get contents dialog successfully!"
    COMPLETED = "Completed."
    GET_DIALOG_SUCCESS = "Get dialogs successfully!"
    UPDATED_DIALOG_SUCCESS = "Updated dialog successfully!"
    DELETED_DIALOG_SUCCESS = "Deleted dialog successfully!"


class MessageHTTPError(Enum):
    INVALID_CONTENT_CHATBOT = "Invalid contents sent. The contents should include message which have role 'user' at the end of context."
    INVALID_ROLE_BODY_CREATE = (
        "Invalid role, role should be 'user', 'assistant' or 'system'"
    )
    TIMEOUT = "The request timed out. Please try again"


DEFAULT_OBJECT_NAME = 'System'
EMPTY_OBJECT_NAME = 'Unknown'