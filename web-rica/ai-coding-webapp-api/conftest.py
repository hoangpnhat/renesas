#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

"""
Root conftest for test infrastructure.
Provides shared fixtures for database, FastAPI app, and authentication.
"""
import os
from typing import AsyncGenerator, Generator, List
from unittest.mock import MagicMock, AsyncMock

import pytest
import pytest_asyncio
from fastapi import FastAPI, Depends
from fastapi.testclient import TestClient
from httpx import AsyncClient, ASGITransport
from motor import motor_asyncio
from odmantic import AIOEngine, ObjectId

# Load environment variables from .env file
from dotenv import load_dotenv
load_dotenv()

# Configure logging with custom levels before tests
from core.loguru import configure_logging
configure_logging()


# Validate required test environment variables
def bind_test_environment(vars: List[str]):
    """
    Bind testing enviroment variable to the respective ones.
    Example:
        bind_test_environment([
            "TESTING_MONGO_URI",
            "TESTING_DATABASE_NAME",
            "TESTING_REDIS_URI"
            "TESTING_REDIS_PORT"
        ])
        assert os.environ["MONGO_URI"] == os.environ["TESTING_MONGO_URI"]
        assert os.environ["DATABASE_NAME"] == os.environ["TESTING_DATABASE_NAME"]
        assert os.environ["REDIS_URI"] == os.environ["TESTING_REDIS_URI"]
        assert os.environ["REDIS_PORT"] == os.environ["TESTING_REDIS_PORT"]
    """
    error_message = []

    # Bind variables, throw error if len(error_message) > 0
    for var in vars:
        if var not in os.environ:
            error_message.append(f"Missing required test environment variable: {var}")
        else:
            # Remove "TESTING_" prefix to get the target variable name
            if var.startswith("TESTING_"):
                target_var = var.replace("TESTING_", "", 1)
                os.environ[target_var] = os.environ[var]
            else:
                error_message.append(f"Variable '{var}' does not follow TESTING_* naming convention")

    if error_message:
        raise EnvironmentError("\n".join(error_message))

os.environ["ENV"] = "test"

# Validate environment before proceeding
bind_test_environment([
    "TESTING_MONGO_URI",
    "TESTING_DATABASE_NAME",
    "TESTING_REDIS_URI",
    "TESTING_REDIS_PORT"
])

from core.config import settings
from db.session import TimezoneAwareEngine
from models.user import User
from models.file import Files
from models.group import Group
from models.knowledge import Knowledge
from models.notification import Notification
from models.api_key import ApiKey
from models.chat_model import Dialog, MessageContent
from models.model_management import ModelManagement
from models.token_consumption import UserTokenConsumption
from schemas.user import VerifiedUser


# ============================================================================
# Session-scoped fixtures (setup once per test session)
# ============================================================================

@pytest_asyncio.fixture(scope="session")
async def test_mongo_client():
    """Create test MongoDB client (session-scoped)."""
    client = motor_asyncio.AsyncIOMotorClient(
        os.environ["MONGO_URI"],
        serverSelectionTimeoutMS=5000
    )
    yield client
    client.close()


@pytest_asyncio.fixture(scope="session")
async def test_engine(test_mongo_client) -> TimezoneAwareEngine:
    """Create test database engine (session-scoped)."""
    engine = TimezoneAwareEngine(
        client=test_mongo_client,
        database=settings.mongodb.DATABASE_NAME
    )

    # Configure database with all models
    await engine.configure_database([
        User, Dialog, MessageContent, ModelManagement,
        Group, Files, Notification, Knowledge, UserTokenConsumption, ApiKey
    ], update_existing_indexes=True)

    yield engine


@pytest_asyncio.fixture(scope="session")
async def test_redis_client():
    """
    Create test Redis client (session-scoped).
    Reuses connection across all tests for performance.
    Uses TESTING_REDIS_URI from environment.
    """
    from caching.redis import RedisClient

    # Create client with test Redis URI (already bound by bind_test_environment)
    redis_client = RedisClient(os.environ["REDIS_URI"])

    # Initialize connection (required for async Redis)
    await redis_client.start()

    # Verify connection works
    assert await redis_client.ping(), "Test Redis connection failed"

    yield redis_client

    # Cleanup: close() automatically calls cleanup_keys_by_prefix() + closes connection
    await redis_client.close()


# ============================================================================
# Function-scoped fixtures (reset for each test)
# ============================================================================

@pytest_asyncio.fixture(autouse=True)
async def clean_database(test_engine):
    """
    Clean all collections before each test.
    Autouse=True means this runs automatically for every test.
    """
    # Drop all collections
    database = test_engine.client[settings.mongodb.DATABASE_NAME]
    for collection_name in await database.list_collection_names():
        await database[collection_name].delete_many({})

    yield

    # Optional: clean up after test as well
    for collection_name in await database.list_collection_names():
        await database[collection_name].delete_many({})


@pytest_asyncio.fixture(autouse=True)
async def clean_redis(test_redis_client):
    """
    Clean all Redis keys with test prefix before each test.
    Autouse=True means this runs automatically for every test.

    Uses RedisClient's built-in cleanup_keys_by_prefix() which:
    - Matches pattern: {APP_NAME}-{ENV}:* (e.g., "rica_ai-test:*")
    - Uses atomic Lua script for performance
    - Safe for shared Redis instances
    """
    # Clean before test
    await test_redis_client.cleanup_keys_by_prefix()

    yield

    # Clean after test as well (paranoid cleanup)
    await test_redis_client.cleanup_keys_by_prefix()


@pytest.fixture
def override_get_engine(test_engine):
    """
    Override the get_engine dependency to return test engine.
    Also patches CRUD singletons to use test engine.
    """
    from db import session
    import crud.crud_user
    import crud.crud_file
    import crud.crud_group
    import crud.crud_knowledge
    import crud.crud_notification
    import crud.crud_model_management
    import crud.crud_chat
    import crud.crud_token_consumption

    # Store originals
    original_get_engine = session.get_engine

    # Store original engines from CRUD objects
    original_engines = {}
    crud_objects = [
        ('user', crud.crud_user.user),
        ('file_crud', crud.crud_file.file_crud),
        ('group_crud', crud.crud_group.group_crud),
        ('knowledge_crud', crud.crud_knowledge.knowledge_crud),
        ('notification_crud', crud.crud_notification.notification_crud),
        ('chat', crud.crud_chat.chat),
        ('message_content', crud.crud_chat.message_content),
        ('token_consumption_crud', crud.crud_token_consumption.token_consumption_crud),
        ('model_crud', crud.crud_model_management.model_crud),
    ]

    for name, crud_obj in crud_objects:
        if hasattr(crud_obj, 'engine'):
            original_engines[name] = crud_obj.engine
            # Patch CRUD object to use test engine
            crud_obj.engine = test_engine

    # Override get_engine function
    def _get_test_engine():
        return test_engine

    session.get_engine = _get_test_engine

    # Reload modules that import get_engine to pick up the patched version
    import importlib
    import sys
    modules_to_reload = [
        'controllers.admin.model_management',
        'services.validator.token_consumption',
    ]

    for module_name in modules_to_reload:
        if module_name in sys.modules:
            importlib.reload(sys.modules[module_name])

    yield test_engine

    # Restore get_engine function
    session.get_engine = original_get_engine

    # Restore original engines
    for name, crud_obj in crud_objects:
        if name in original_engines:
            crud_obj.engine = original_engines[name]


@pytest.fixture
def override_get_redis(test_redis_client):
    """
    Override the get_redis dependency and patch global _redis_client.
    Reloads affected modules to ensure they use the patched client.
    """
    import importlib
    import sys
    from dependencies import get_redis
    import dependencies
    import caching.redis

    # Store originals
    original_get_redis = dependencies.get_redis
    original_redis_client = caching.redis._redis_client

    # Patch the global _redis_client BEFORE any modules use it
    caching.redis._redis_client = test_redis_client

    # Reload modules that imported _redis_client to pick up the patched version
    modules_to_reload = [
        'controllers.user',
        'controllers.file.controller',
        'services.file_access',
        'utils.decorators',
    ]

    for module_name in modules_to_reload:
        if module_name in sys.modules:
            try:
                importlib.reload(sys.modules[module_name])
            except Exception:
                pass  # Module might have complex imports, skip if reload fails

    # Override dependency function
    def _get_test_redis():
        return test_redis_client

    dependencies.get_redis = _get_test_redis

    yield test_redis_client

    # Restore originals
    dependencies.get_redis = original_get_redis
    caching.redis._redis_client = original_redis_client

    # Reload modules again to restore original imports
    for module_name in modules_to_reload:
        if module_name in sys.modules:
            try:
                importlib.reload(sys.modules[module_name])
            except Exception:
                pass


@pytest.fixture
def mock_redis_client():
    """Mock Redis client for tests that don't need real Redis."""
    mock_redis = MagicMock()
    mock_redis.get = AsyncMock(return_value=None)
    mock_redis.set = AsyncMock(return_value=True)
    mock_redis.delete = AsyncMock(return_value=True)
    mock_redis.close = AsyncMock()
    mock_redis.start = AsyncMock()
    mock_redis.exists = AsyncMock(return_value=False)
    mock_redis.expire = AsyncMock(return_value=True)
    mock_redis.incr = AsyncMock(return_value=1)
    return mock_redis


# ============================================================================
# FastAPI App and TestClient fixtures
# ============================================================================

@pytest.fixture
def app(override_get_engine, override_get_redis) -> FastAPI:
    """
    Create FastAPI app instance with test dependencies.

    This fixture overrides:
    - Database engine (to test database) ✓
    - Redis client (to test Redis) ✓
    """
    # Import unwrapped FastAPI app after environment variables are set
    from main import fastapi_app as _app
    from dependencies import get_redis
    from db.session import get_engine

    # Override dependencies
    _app.dependency_overrides[get_engine] = lambda: override_get_engine
    _app.dependency_overrides[get_redis] = lambda: override_get_redis

    yield _app

    # Clean up overrides
    _app.dependency_overrides.clear()


@pytest.fixture
def client(app: FastAPI) -> TestClient:
    """
    Synchronous TestClient for simple endpoint tests.
    Use for tests that don't need async operations.
    """
    return TestClient(app)


@pytest_asyncio.fixture
async def async_client(app: FastAPI) -> AsyncGenerator[AsyncClient, None]:
    """
    Async HTTP client for testing async endpoints.
    Preferred for most endpoint tests.
    """
    async with AsyncClient(
        transport=ASGITransport(app=app),
        base_url="http://test"
    ) as ac:
        yield ac


# ============================================================================
# Authentication fixtures
# ============================================================================

@pytest_asyncio.fixture
async def test_user_admin(test_engine) -> VerifiedUser:
    """Test user with admin role (created in database)."""
    user = User(
        email="test.admin@test.com",
        full_name="Test Admin",
        is_active=True
    )
    await test_engine.save(user)

    return VerifiedUser(
        email=user.email,
        user_id=user.id,
        roles=["admin", "user"],
        name=user.full_name
    )


@pytest_asyncio.fixture
async def test_user_regular(test_engine) -> VerifiedUser:
    """Test user with regular user role (created in database)."""
    user = User(
        email="test.user@test.com",
        full_name="Test User",
        is_active=True
    )
    await test_engine.save(user)

    return VerifiedUser(
        email=user.email,
        user_id=user.id,
        roles=["user"],
        name=user.full_name
    )



@pytest_asyncio.fixture
async def override_auth_admin(app: FastAPI, test_user_admin, override_get_engine):
    """Override authentication to return admin user."""
    from dependencies import (
        get_file_controller, get_user_controller, get_knowledge_controller,
        get_group_controller, get_notification_controller,
        get_crud_registry, get_model_management_controller, get_user_model_controller,
    )
    from api.v1.endpoints.keys import _get_verified_user as get_keys_verified_user
    from controllers.admin.model_management import ModelManagementController
    from controllers.user_model import UserModelController
    from controllers.file import FileController, FileResolver, FileValidator, file_crud
    from controllers.user import UserController
    from controllers.knowledge import KnowledgeController, KnowledgeResolver, KnowledgeValidator
    from controllers.group import GroupController, GroupResolver, GroupValidator, group_crud
    from controllers.notification import NotificationController
    from services.resolution.entity_resolver import LookupService

    # Create services
    crud_registry = get_crud_registry()
    lookup_service = LookupService(crud_registry)

    # Override controller dependencies to inject test user
    def mock_get_file_controller():
        resolver = FileResolver(lookup_service)
        validator = FileValidator(file_crud)
        return FileController(verified_user=test_user_admin, resolver=resolver, validator=validator)

    def mock_get_user_controller():
        return UserController(verified_user=test_user_admin)

    def mock_get_knowledge_controller():
        resolver = KnowledgeResolver(lookup_service)
        validator = KnowledgeValidator(None)
        return KnowledgeController(verified_user=test_user_admin, resolver=resolver, validator=validator)

    def mock_get_group_controller():
        resolver = GroupResolver(lookup_service)
        validator = GroupValidator(group_crud)
        return GroupController(verified_user=test_user_admin, resolver=resolver, validator=validator)

    def mock_get_notification_controller():
        return NotificationController(verified_user=test_user_admin)

    def mock_get_model_management_controller():
        return ModelManagementController(verified_user=test_user_admin)

    def mock_get_user_model_controller():
        return UserModelController(verified_user=test_user_admin)

    def mock_get_keys_verified_user():
        return test_user_admin

    _original = dict(app.dependency_overrides)
    app.dependency_overrides[get_file_controller] = mock_get_file_controller
    app.dependency_overrides[get_user_controller] = mock_get_user_controller
    app.dependency_overrides[get_knowledge_controller] = mock_get_knowledge_controller
    app.dependency_overrides[get_group_controller] = mock_get_group_controller
    app.dependency_overrides[get_notification_controller] = mock_get_notification_controller
    app.dependency_overrides[get_model_management_controller] = mock_get_model_management_controller
    app.dependency_overrides[get_user_model_controller] = mock_get_user_model_controller
    app.dependency_overrides[get_keys_verified_user] = mock_get_keys_verified_user

    yield test_user_admin

    app.dependency_overrides.clear()
    app.dependency_overrides.update(_original)


@pytest_asyncio.fixture
async def override_auth_user(app: FastAPI, test_user_regular, override_get_engine):
    """Override authentication to return regular user."""
    from dependencies import (
        get_file_controller, get_user_controller, get_knowledge_controller,
        get_group_controller, get_notification_controller,
        get_crud_registry, get_user_model_controller,
    )
    from controllers.user_model import UserModelController
    from controllers.file import FileController, FileResolver, FileValidator, file_crud
    from controllers.user import UserController
    from controllers.knowledge import KnowledgeController, KnowledgeResolver, KnowledgeValidator
    from controllers.group import GroupController, GroupResolver, GroupValidator, group_crud
    from controllers.notification import NotificationController
    from services.resolution.entity_resolver import LookupService

    # Create services
    crud_registry = get_crud_registry()
    lookup_service = LookupService(crud_registry)

    # Override controller dependencies to inject test user
    def mock_get_file_controller():
        resolver = FileResolver(lookup_service)
        validator = FileValidator(file_crud)
        return FileController(verified_user=test_user_regular, resolver=resolver, validator=validator)

    def mock_get_user_controller():
        return UserController(verified_user=test_user_regular)

    def mock_get_knowledge_controller():
        resolver = KnowledgeResolver(lookup_service)
        validator = KnowledgeValidator(None)
        return KnowledgeController(verified_user=test_user_regular, resolver=resolver, validator=validator)

    def mock_get_group_controller():
        resolver = GroupResolver(lookup_service)
        validator = GroupValidator(group_crud)
        return GroupController(verified_user=test_user_regular, resolver=resolver, validator=validator)

    def mock_get_notification_controller():
        return NotificationController(verified_user=test_user_regular)

    def mock_get_user_model_controller():
        return UserModelController(verified_user=test_user_regular)

    _original = dict(app.dependency_overrides)
    app.dependency_overrides[get_file_controller] = mock_get_file_controller
    app.dependency_overrides[get_user_controller] = mock_get_user_controller
    app.dependency_overrides[get_knowledge_controller] = mock_get_knowledge_controller
    app.dependency_overrides[get_group_controller] = mock_get_group_controller
    app.dependency_overrides[get_notification_controller] = mock_get_notification_controller
    app.dependency_overrides[get_user_model_controller] = mock_get_user_model_controller

    yield test_user_regular

    app.dependency_overrides.clear()
    app.dependency_overrides.update(_original)


# ============================================================================
# Mock external services
# ============================================================================

@pytest.fixture
def mock_databricks_storage():
    """Mock DatabricksStorage for tests that don't need real Databricks."""
    mock = MagicMock()
    mock.upload = AsyncMock(return_value=MagicMock(upload_path="/test/path"))
    mock.download = AsyncMock(return_value=b"test file content")
    mock.streaming_download = AsyncMock()
    mock.create_target_directory = AsyncMock(return_value="/test/dir")
    return mock


@pytest.fixture
def mock_elasticsearch():
    """Mock ElasticSearch service."""
    mock = MagicMock()
    mock.index = AsyncMock(return_value={"success": True})
    mock.search = AsyncMock(return_value={"hits": {"hits": []}})
    mock.delete = AsyncMock(return_value={"success": True})
    return mock


# ============================================================================
# Test data factories
# ============================================================================

@pytest_asyncio.fixture
async def user_factory(test_engine):
    """Factory to create test users in database."""
    async def _create_user(
        email: str = "test@example.com",
        full_name: str = "Test User",
        is_active: bool = True,
        **kwargs
    ) -> User:
        user = User(
            email=email,
            full_name=full_name,
            is_active=is_active,
            **kwargs
        )
        await test_engine.save(user)
        return user

    return _create_user


@pytest_asyncio.fixture
async def file_factory(test_engine):
    """Factory to create test files in database."""
    async def _create_file(
        author_id: ObjectId,
        file_name: str = "test.pdf",
        status: str = "completed",
        **kwargs
    ) -> Files:
        file = Files(
            author_id=author_id,
            file_name=file_name,
            status=status,
            extension="pdf",
            size=1024,
            **kwargs
        )
        await test_engine.save(file)
        return file

    return _create_file


@pytest_asyncio.fixture
async def group_factory(test_engine):
    """Factory to create test groups in database."""
    async def _create_group(
        name: str = "Test Group",
        owner_id: ObjectId = None,
        **kwargs
    ) -> Group:
        group = Group(
            name=name,
            owner_id=owner_id or ObjectId(),
            **kwargs
        )
        await test_engine.save(group)
        return group

    return _create_group


@pytest_asyncio.fixture
async def model_management_factory(test_engine):
    """Factory to create test models with consumption config in database."""
    async def _create_model(
        name: str = "test-model",
        model_path: str = "test/model",
        roles: list = None,
        threshold: int = None,
        consumption_range_hours: int = None,
        **kwargs
    ) -> ModelManagement:
        from models.model_management import DefaultCompletionOptions
        from const.enums import RoleEnum

        model = ModelManagement(
            name=name,
            model_path=model_path,
            roles=roles or [RoleEnum.CHAT.value],
            default_completion_options=DefaultCompletionOptions(
                max_tokens=256,
                context_length=4096,
                temperature=0.7
            ),
            threshold=threshold,
            consumption_range_hours=consumption_range_hours,
            **kwargs
        )
        await test_engine.save(model)
        return model

    return _create_model
