ERRORS = {
    "DATABRICKS_EXCEPTION": {
        "status_code": 500,
        "detail": "Databricks ran into this issue: {desc}",
    },
    "INVALID_USER": {
        "status_code": 400,
        "detail": "Invalid user {user} credentials.",
    },
    "REDIS_EXCEPTION": {
        "status_code": 500,
        "detail": "Got Redis exception: {desc}"
    },
    "DATABASE_EXCEPTION": {
        "status_code": 500,
        "detail": "Got database exception: {desc}"
    },
    "PERMISSION_DENIED": {
        "status_code": 403,
        "detail": "{obj_1} does not have permission toward {obj_2}"
    },
    "NOT_EXISTED_ERROR": {
        "status_code": 404,
        "detail": "Could not find resource"
    },
    "EXISTED_ERROR": {
        "status_code": 409,
        "detail": "{obj} already existed"
    },
    "CONSTRAINT_ERROR": {
        "status_code": 400,
        "detail": "Existing constraint between {obj_1} -> {obj_2}"
    },
    "NON_LATIN_CHARACTER": {
        "status_code": 500,
        "detail": "Non Latin1 (ASCII) characters detected: {obj} "
    },
    "INVALID_HEADER": {
        "status_code": 400,
        "detail": "{desc}"
    },
    "FILE_NOT_READY": {
        "status_code": 503,
        "detail": "File is not ready for download"
    },
    "INVALID_CONTENT": {
        "status_code": 400,
        "detail": "Invalid batch file: {desc}"
    },
    "INACTIVE_OBJECT": {
        "status_code": 409,
        "detail": "Cannot access inactive object"
    }
}
