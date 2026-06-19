from pydantic import BaseModel
from typing import Any
from enum import Enum

class MatchType(Enum):
    ANY = "any"
    ALL = "all"

class Operator(Enum):
    IN = "in"
    NOT_IN = "not_in"
    CONTAINS = "contains"
    NOT_CONTAINS = "not_contains"
    EQUALS = "eq"
    NOT_EQUALS = "ne"
    GREATER_THAN = "gt"
    LESS_THAN = "lt"
    GREATER_EQUAL = "gte"
    LESS_EQUAL = "lte"

class EmptyResultBehavior(Enum):
    NO_ROWS = "no_rows"
    ALL_ROWS = "all_rows"
    USER_DEFAULT  = "user_default"

class FilterCondition(BaseModel):
    field: str
    operator: Operator
    value: Any

class FilterGroup(BaseModel):
    group_operator: MatchType
    conditions: list[FilterCondition]

class DynamicFilterRequest(BaseModel):
    match_type: MatchType
    filter_groups: list[FilterGroup]
    empty_result: EmptyResultBehavior = EmptyResultBehavior.NO_ROWS
