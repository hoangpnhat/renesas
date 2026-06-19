PPT_FILE_FROM_URL_REGEX = r"[^/]+\.pptx\b"

FILE_ALLOWED_MIME_TYPES = {
    "application/pdf": "pdf",
    "application/vnd.openxmlformats-officedocument.presentationml.presentation": "pptx",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet": "xlsx",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.document": "docx",
    "application/msword": "doc"
}

FILE_ALLOWED_EXTENSIONS = {
    '.pdf': 'pdf',
    '.ppt': 'powerpoint_legacy',
    '.pptx': 'powerpoint_openxml',
    '.docx': 'word_openxml',
    '.doc': 'word_legacy',
    '.xlsx': 'excel_openxml',
    '.xlsm': 'excel_macro_enabled'
}

PROMPT_ALLOWED_MIME_TYPES = {
    "text/markdown": "md",
    "text/x-markdown": "md",
    "application/x-yaml": "yaml",
    "text/yaml": "yaml",
    "text/x-yaml": "yaml",
    "application/octet-stream": "md",
}

PROMPT_ALLOWED_EXTENSIONS = {
    ".md": "md",
    ".yaml": "yaml",
    ".yml": "yaml",
}
