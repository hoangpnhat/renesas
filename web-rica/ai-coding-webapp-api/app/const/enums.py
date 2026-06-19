from enum import Enum


class SortOrder(str, Enum):
    asc = "asc"
    desc = "desc"


class RoleEnum(str, Enum):
    CHAT = "chat"
    EDIT = "edit"
    APPLY = "apply"
    AUTOCOMPLETE = "autocomplete"
    RERANK = "rerank"
    EMBEDDING = "embedding"


class ProviderEnum(str, Enum):
    RICA_PROXY = "rica-proxy"


class ContextProviderEnum(str, Enum):
    CODE = "code"
    DOCS = "docs"
    DIFF = "diff"
    TERMINAL = "terminal"
    PROBLEMS = "problems"
    FOLDER = "folder"
    CODEBASE = "codebase"
    URL = "url"
    RICA_DOCS = "rica-docs"
