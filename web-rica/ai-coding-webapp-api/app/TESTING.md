# Testing Guide

This document provides comprehensive guidance on testing in the RICA AI backend application.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Test Infrastructure Overview](#test-infrastructure-overview)
3. [Running Tests](#running-tests)
4. [Writing Tests](#writing-tests)
5. [Test Fixtures](#test-fixtures)
6. [Test Patterns](#test-patterns)
7. [Troubleshooting](#troubleshooting)

---

## Quick Start

### Prerequisites

1. **MongoDB running locally**:
   ```bash
   # Check if MongoDB is running
   mongosh --eval "db.adminCommand('ping')"
   
   # Or start MongoDB with Docker
   docker run -d -p 27017:27017 --name mongodb mongo:7
   ```

2. **Install test dependencies**:
   ```bash
   pip install pytest pytest-asyncio pytest-mock faker pytest-cov
   ```

### Run Your First Test

```bash
# Run all tests
pytest

# Run specific test file
pytest app/api/v1/endpoints/test_file.py

# Run with verbose output
pytest -v
```

---

## Test Infrastructure Overview

### Test Organization

Tests are **co-located** with implementation files:

```
app/
├── conftest.py                    # Root fixtures (database, auth, mocks)
├── api/
│   └── v1/
│       └── endpoints/
│           ├── file.py            # Implementation
│           └── test_file.py       # Tests
├── controllers/
│   └── file/
│       ├── controller.py          # Implementation
│       └── test_controller.py     # Tests (if needed)
└── crud/
    ├── crud_file.py               # Implementation
    └── test_crud_file.py          # Tests (if needed)
```

### Test Types

We use **markers** to categorize tests:

- `@pytest.mark.integration` - Full endpoint tests with real database
- `@pytest.mark.unit` - Isolated tests with mocked dependencies
- `@pytest.mark.auth` - Authentication-related tests
- `@pytest.mark.slow` - Long-running tests
- `@pytest.mark.external` - Tests requiring external services

### Test Database

Tests use a **separate test database** (`rica_ai_test`) that is automatically:
- Created when tests start
- Cleaned before each test
- Isolated from production data

---

## Running Tests

### Basic Commands

```bash
# Run all tests
pytest

# Run specific file
pytest app/api/v1/endpoints/test_file.py

# Run specific test function
pytest app/api/v1/endpoints/test_file.py::test_get_files_list_empty

# Verbose output
pytest -v

# Stop on first failure
pytest -x

# Show print statements
pytest -s
```

### Run by Marker

```bash
# Integration tests only
pytest -m integration

# Unit tests only
pytest -m unit

# Exclude slow tests
pytest -m "not slow"

# Auth tests only
pytest -m auth
```

### Coverage Reports

```bash
# Run with coverage
pytest --cov=app

# Generate HTML coverage report
pytest --cov=app --cov-report=html

# View missing lines
pytest --cov=app --cov-report=term-missing

# Coverage report will be in htmlcov/index.html
```

### Parallel Execution (Optional)

```bash
# Install pytest-xdist
pip install pytest-xdist

# Run tests in parallel (4 workers)
pytest -n 4
```

---

## Writing Tests

### Endpoint Integration Test Template

```python
import pytest
from httpx import AsyncClient

@pytest.mark.integration
@pytest.mark.asyncio
async def test_my_endpoint(
    async_client: AsyncClient,
    user_factory,
    override_auth_admin,
    test_engine
):
    """Test my endpoint."""
    # Arrange - create test data
    user = await user_factory(email="test@example.com")
    
    # Act - make HTTP request
    response = await async_client.get("/api/v1/my-endpoint")
    
    # Assert - verify response
    assert response.status_code == 200
    data = response.json()
    assert "expected_field" in data
```

### Controller Unit Test Template

```python
import pytest
from unittest.mock import MagicMock, AsyncMock
from odmantic import ObjectId

@pytest.mark.unit
@pytest.mark.asyncio
async def test_controller_method():
    """Test controller business logic."""
    # Arrange - create mocks
    mock_crud = MagicMock()
    mock_crud.get_by_id = AsyncMock(return_value=mock_object)
    
    # Act - call controller method
    result = await controller.my_method(param="value")
    
    # Assert - verify behavior
    assert result is not None
    mock_crud.get_by_id.assert_called_once()
```

### Test File Structure

```python
#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

"""
Tests for [feature name].
Brief description of what is being tested.
"""
import pytest
from httpx import AsyncClient

# Fixtures (if specific to this module)
@pytest.fixture
def my_fixture():
    return "fixture_value"

# Integration tests
@pytest.mark.integration
@pytest.mark.asyncio
async def test_feature_integration():
    pass

# Unit tests
@pytest.mark.unit
@pytest.mark.asyncio
async def test_feature_unit():
    pass
```

---

## Test Fixtures

### Available Fixtures

Fixtures are defined in `app/conftest.py` and are available to all tests.

#### Database Fixtures

```python
# Test database engine (session-scoped)
async def test_something(test_engine):
    await test_engine.save(model)

# Override get_engine to use test database
def test_something(override_get_engine):
    # Automatically applied when using 'app' fixture
    pass

# Auto-cleanup (runs automatically for all tests)
# clean_database fixture clears all collections before/after each test
```

#### HTTP Client Fixtures

```python
# Async HTTP client (preferred)
async def test_endpoint(async_client: AsyncClient):
    response = await async_client.get("/api/v1/endpoint")

# Synchronous TestClient
def test_endpoint(client: TestClient):
    response = client.get("/api/v1/endpoint")

# FastAPI app with test dependencies
def test_something(app: FastAPI):
    # App has test database and mocked services
    pass
```

#### Authentication Fixtures

```python
# Admin user (with override applied)
async def test_admin_endpoint(async_client, override_auth_admin):
    # All requests will be authenticated as admin
    response = await async_client.get("/api/v1/admin-endpoint")

# Regular user (with override applied)
async def test_user_endpoint(async_client, override_auth_user):
    # All requests will be authenticated as regular user
    response = await async_client.get("/api/v1/user-endpoint")

# Test user objects (without override)
def test_something(test_user_admin, test_user_regular):
    # Use these VerifiedUser objects in your tests
    pass
```

#### Test Data Factories

```python
# Create users in test database
async def test_with_user(user_factory):
    user = await user_factory(
        email="custom@example.com",
        full_name="Custom User",
        is_active=True
    )

# Create files in test database
async def test_with_file(user_factory, file_factory):
    user = await user_factory()
    file = await file_factory(
        author_id=user.id,
        file_name="test.pdf",
        status="completed"
    )

# Create groups in test database
async def test_with_group(group_factory):
    group = await group_factory(
        name="Test Group",
        owner_id=ObjectId()
    )
```

#### Mock Fixtures

```python
# Mock Redis client
def test_with_redis(mock_redis_client):
    # Redis operations will be mocked
    pass

# Mock Databricks storage
def test_with_storage(mock_databricks_storage):
    # Storage operations will be mocked
    pass

# Mock ElasticSearch
def test_with_es(mock_elasticsearch):
    # ES operations will be mocked
    pass
```

---

## Test Patterns

### Pattern 1: Full Endpoint Test

Test complete request-response flow with real database:

```python
@pytest.mark.integration
@pytest.mark.asyncio
async def test_create_and_retrieve(
    async_client,
    user_factory,
    override_auth_admin,
    test_engine
):
    # Create user
    user = await user_factory(email=override_auth_admin.email)
    
    # Create resource via API
    create_response = await async_client.post(
        "/api/v1/resources",
        json={"name": "Test Resource"}
    )
    assert create_response.status_code == 200
    resource_id = create_response.json()["id"]
    
    # Retrieve resource via API
    get_response = await async_client.get(f"/api/v1/resources/{resource_id}")
    assert get_response.status_code == 200
    assert get_response.json()["name"] == "Test Resource"
    
    # Verify in database
    from models.resource import Resource
    resource_in_db = await test_engine.find_one(
        Resource,
        Resource.id == ObjectId(resource_id)
    )
    assert resource_in_db is not None
```

### Pattern 2: Test Unauthorized Access

```python
@pytest.mark.integration
@pytest.mark.asyncio
@pytest.mark.auth
async def test_unauthorized_access(async_client):
    """Test endpoint without authentication."""
    # No override_auth_* fixture - request should fail
    response = await async_client.get("/api/v1/protected-endpoint")
    assert response.status_code == 401
```

### Pattern 3: Parameterized Tests

Test multiple scenarios with one test function:

```python
@pytest.mark.parametrize("status,expected_code", [
    ("completed", 200),
    ("processing", 400),
    ("failed", 400),
])
@pytest.mark.integration
@pytest.mark.asyncio
async def test_download_with_status(
    async_client,
    user_factory,
    file_factory,
    override_auth_admin,
    status,
    expected_code
):
    """Test file download with different statuses."""
    user = await user_factory()
    file = await file_factory(author_id=user.id, status=status)
    
    response = await async_client.get(f"/api/v1/files/{file.id}/download")
    assert response.status_code == expected_code
```

### Pattern 4: Testing Error Cases

```python
@pytest.mark.integration
@pytest.mark.asyncio
async def test_create_with_invalid_data(async_client, override_auth_admin):
    """Test validation error handling."""
    # Send invalid data
    response = await async_client.post(
        "/api/v1/resources",
        json={"invalid_field": "value"}
    )
    
    # Should return validation error
    assert response.status_code == 422
    assert "detail" in response.json()
```

### Pattern 5: Testing Pagination

```python
@pytest.mark.integration
@pytest.mark.asyncio
async def test_pagination(
    async_client,
    user_factory,
    file_factory,
    override_auth_admin
):
    """Test paginated endpoint."""
    # Create multiple resources
    user = await user_factory()
    for i in range(15):
        await file_factory(author_id=user.id, file_name=f"file{i}.pdf")
    
    # Get first page
    response = await async_client.get("/api/v1/files?page=1&page_size=10")
    assert response.status_code == 200
    data = response.json()
    assert len(data["data"]) == 10
    assert data["total"] == 15
    
    # Get second page
    response = await async_client.get("/api/v1/files?page=2&page_size=10")
    assert response.status_code == 200
    data = response.json()
    assert len(data["data"]) == 5
```

---

## Troubleshooting

### MongoDB Connection Issues

**Problem**: Tests fail with "Connection refused" or timeout.

**Solution**:
```bash
# Check MongoDB is running
mongosh --eval "db.adminCommand('ping')"

# Start MongoDB if not running
docker run -d -p 27017:27017 --name mongodb mongo:7

# Check .env file has correct testing configuration
# Edit .env and set:
# TESTING_MONGO_URI=mongodb://localhost:27017
# TESTING_DATABASE_NAME=rica_ai_test

# Or override temporarily
export TESTING_MONGO_URI="mongodb://localhost:27017"
pytest
```

### Test Database Not Isolated

**Problem**: Tests interfere with each other or production data.

**Solution**:
- Verify `DATABASE_NAME` is set to `rica_ai_test` in `conftest.py`
- Check `clean_database` fixture is being applied
- Verify environment variables are set before importing app modules

### Authentication Override Not Working

**Problem**: Tests fail with 401 Unauthorized despite using `override_auth_*` fixture.

**Solution**:
- Ensure fixture is included in test parameters: `async def test(async_client, override_auth_admin):`
- Check that `app` fixture is used (it sets up overrides)
- Verify user exists in database if needed: `await user_factory(email=override_auth_admin.email)`

### Async Fixture Errors

**Problem**: `RuntimeError: Event loop is closed` or similar async errors.

**Solution**:
- Use `@pytest.mark.asyncio` decorator on async tests
- Use `@pytest_asyncio.fixture` for async fixtures
- Check `pytest.ini` has `asyncio_mode = auto`

### Tests Pass Individually But Fail Together

**Problem**: Tests pass when run alone but fail when run with others.

**Solution**:
- This indicates test isolation issues
- Check `clean_database` fixture is working
- Verify no global state is being shared
- Use `pytest -x` to stop on first failure and identify the problematic test

### Import Errors

**Problem**: `ModuleNotFoundError` or import errors.

**Solution**:
```bash
# Run from project root
cd /home/sangdaoxv/workplace/ai-coding-webapp-api

# Ensure PYTHONPATH includes app directory
export PYTHONPATH="${PYTHONPATH}:${PWD}/app"
pytest

# Or run via uv
uv run pytest
```

### Slow Tests

**Problem**: Tests take too long to run.

**Solution**:
```bash
# Run only fast tests
pytest -m "not slow"

# Run tests in parallel
pip install pytest-xdist
pytest -n auto

# Profile slow tests
pytest --durations=10
```

### Coverage Not Working

**Problem**: Coverage report shows 0% or incorrect coverage.

**Solution**:
```bash
# Ensure pytest-cov is installed
pip install pytest-cov

# Run with coverage from project root
cd /home/sangdaoxv/workplace/ai-coding-webapp-api
pytest --cov=app --cov-report=term-missing

# Check .coveragerc if coverage excludes tests
```

---

## Best Practices

1. **Use descriptive test names**: Test names should describe what is being tested
   - Good: `test_file_upload_creates_database_record`
   - Bad: `test_1`, `test_upload`

2. **Follow AAA pattern**: Arrange, Act, Assert
   ```python
   # Arrange - setup test data
   user = await user_factory()
   
   # Act - perform the action
   response = await async_client.get("/endpoint")
   
   # Assert - verify the result
   assert response.status_code == 200
   ```

3. **One assertion per test** (when possible): Easier to debug failures

4. **Test error cases**: Don't just test the happy path

5. **Use factories**: Prefer factories over hardcoded test data

6. **Clean tests**: Tests should be self-contained and not depend on order

7. **Mark tests appropriately**: Use pytest markers for categorization

8. **Keep tests fast**: Use mocks for external services, run slow tests separately

---

## Environment Variables

Tests **require** these environment variables configured in `.env` file:

```bash
# In .env file (REQUIRED)
TESTING_MONGO_URI=mongodb://localhost:27017
TESTING_DATABASE_NAME=rica_ai_test
TESTING_REDIS_URI=redis://127.0.0.1:6379/1  # Optional, has default
```

**Important**: The test infrastructure validates that `TESTING_MONGO_URI` and `TESTING_DATABASE_NAME` are set before running tests. If they're missing, you'll get a clear error message:

```
TESTING CONFIGURATION ERROR
The following required test environment variables are missing:
  - TESTING_MONGO_URI
  - TESTING_DATABASE_NAME

Please add them to your .env file
```

To override for a specific test run:
```bash
export TESTING_MONGO_URI="mongodb://custom:27017"
export TESTING_DATABASE_NAME="custom_test_db"
pytest
```

---

## Next Steps

1. **Write tests for new features**: Add tests when implementing new endpoints
2. **Increase coverage**: Aim for >80% coverage on critical paths
3. **Add more markers**: Create custom markers for your needs
4. **CI/CD integration**: Add GitHub Actions or similar for automated testing

For more information, see:
- `app/conftest.py` - All available fixtures
- `pytest.ini` - Pytest configuration
- `app/api/v1/endpoints/test_file.py` - Example integration tests
