from typing import TypeVar

from .base import BaseQueryBuilder
from .models import *


QueryBuilderType = TypeVar('QueryBuilderType', bound=BaseQueryBuilder)