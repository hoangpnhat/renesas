from typing import Generator, Annotated

from fastapi import Depends, HTTPException, status
from jose import jwt
from pydantic import ValidationError

import schemas
from caching.redis import _redis_client as redis_cli, RedisClient
from const.common import AppAccessRoles
# Files
from controllers.file import (
    FileController,
    FileResolver,
    FileValidator,
    file_crud
)
# group
from controllers.group import (
    GroupController,
    GroupResolver,
    GroupValidator,
    group_crud
)
# Knowledge
from controllers.knowledge import (
    KnowledgeController,
    KnowledgeResolver,
    KnowledgeValidator
)
# notification
from controllers.notification import NotificationController
# Prompt
from controllers.prompt import (
    PromptController,
    PromptResolver,
    PromptValidator
)
# User
from controllers.user import UserController
# Admin model management
from controllers.admin.model_management import ModelManagementController
# User model
from controllers.user_model import UserModelController
from core.auth.unified import unified_validator
from core.config import settings
from crud import CrudRegistry
from db.session import MongoDatabase
from motor.core import AgnosticDatabase
from schemas.user import VerifiedUser
from services.resolution.entity_resolver import LookupService


# Infrastructure Dependencies
def get_db() -> Generator:
    """Provides database session"""
    try:
        db = MongoDatabase()
        yield db
    finally:
        pass


def get_redis() -> RedisClient:
    """Provides Redis client"""
    if redis_cli is None:
        raise RuntimeError("Redis client is not initialised — check REDIS_URI")
    return redis_cli


def get_token_payload(token: str):
    """Decode and validate JWT token"""
    try:
        payload = jwt.decode(token, settings.SECRET_KEY, algorithms=[settings.JWT_ALGO])
        token_data = schemas.TokenPayload(**payload)

    except (jwt.JWTError, ValidationError):
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Could not validate credentials",
        )
    return token_data


# Controller and Supportings
def get_crud_registry() -> CrudRegistry:
    """Provides singleton CrudRegistry instance"""
    return CrudRegistry()


def get_lookup_service(
        crud_registry: CrudRegistry = Depends(get_crud_registry)
) -> LookupService:
    """Provides LookupService with injected CrudRegistry"""
    return LookupService(crud_registry)


def create_resolver_dependency(resolver_class):
    """Factory to create resolver dependency functions"""

    def get_resolver(lookup_service: LookupService = Depends(get_lookup_service)):
        return resolver_class(lookup_service)

    return get_resolver


def create_validator_dependency(validator_class, crud_instance):
    """Factory to create validator dependency functions"""

    def get_validator():
        return validator_class(crud_instance)

    return get_validator


def create_controller_dependency(controller_class, resolver_dep, validator_dep, roles: list[str] | None = None):
    """Factory to create controller dependency functions

    Args:
        controller_class: The controller class to instantiate
        resolver_dep: Resolver dependency function
        validator_dep: Validator dependency function
        roles: List of allowed roles. Defaults to ["admin", "user"] if None
    """
    roles_list: list[str] = roles or [AppAccessRoles.ADMIN, AppAccessRoles.USER]

    def get_controller(
            verified_user: VerifiedUser = Depends(unified_validator(roles_list)),
            resolver=Depends(resolver_dep),
            validator=Depends(validator_dep)
    ):
        return controller_class(
            verified_user=verified_user,
            resolver=resolver,
            validator=validator
        )

    return get_controller


def create_controller_simple_dependency(controller_class, roles: list[str] | None = None):
    """Factory to create controller dependency functions without resolver/validator

    Args:
        controller_class: The controller class to instantiate
        roles: List of allowed roles. Defaults to ["admin", "user"] if None
    """
    roles_list: list[str] = roles or [AppAccessRoles.ADMIN, AppAccessRoles.USER]

    def get_controller(
            verified_user: VerifiedUser = Depends(unified_validator(roles_list)),
    ):
        return controller_class(
            verified_user=verified_user,
        )

    return get_controller


# Simple Controller
get_user_controller = create_controller_simple_dependency(UserController)
get_notification_controller = create_controller_simple_dependency(NotificationController)

# File Dependencies
get_file_resolver = create_resolver_dependency(FileResolver)
get_file_validator = create_validator_dependency(FileValidator, file_crud)
get_file_controller = create_controller_dependency(
    FileController,
    get_file_resolver,
    get_file_validator
)

# Knowledge Dependencies
get_knowledge_resolver = create_resolver_dependency(KnowledgeResolver)
get_knowledge_validator = create_validator_dependency(KnowledgeValidator, None)
get_knowledge_controller = create_controller_dependency(
    KnowledgeController,
    get_knowledge_resolver,
    get_knowledge_validator
)

# Group Dependencies
get_group_resolver = create_resolver_dependency(GroupResolver)
get_group_validator = create_validator_dependency(GroupValidator, group_crud)
get_group_controller = create_controller_dependency(
    GroupController,
    get_group_resolver,
    get_group_validator
)

# Prompt Dependencies
get_prompt_resolver = create_resolver_dependency(PromptResolver)
get_prompt_validator = create_validator_dependency(PromptValidator, None)
get_prompt_controller = create_controller_dependency(
    PromptController,
    get_prompt_resolver,
    get_prompt_validator
)

# Annotated Types for Convenience
UserControllerDep = Annotated[UserController, Depends(get_user_controller)]
FileControllerDep = Annotated[FileController, Depends(get_file_controller)]
KnowledgeControllerDep = Annotated[KnowledgeController, Depends(get_knowledge_controller)]
GroupControllerDep = Annotated[GroupController, Depends(get_group_controller)]
NotificationControllerDep = Annotated[NotificationController, Depends(get_notification_controller)]
PromptControllerDep = Annotated[PromptController, Depends(get_prompt_controller)]

DatabaseDep = Annotated[AgnosticDatabase, Depends(get_db)]
RedisDep = Annotated[RedisClient, Depends(get_redis)]

# Model Management (admin)
get_model_management_controller = create_controller_simple_dependency(
    ModelManagementController, [AppAccessRoles.ADMIN]
)

# User Model (user)
get_user_model_controller = create_controller_simple_dependency(
    UserModelController, [AppAccessRoles.USER]
)

ModelManagementControllerDep = Annotated[ModelManagementController, Depends(get_model_management_controller)]
UserModelControllerDep = Annotated[UserModelController, Depends(get_user_model_controller)]
