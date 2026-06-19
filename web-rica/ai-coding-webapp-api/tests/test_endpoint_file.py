#  Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.

"""
Integration tests for file endpoints.
Tests complete request-response flow with real database.
"""
import io
import pytest
from httpx import AsyncClient
from odmantic import ObjectId

from app.models.user import User
from app.models.file import Files


@pytest.mark.integration
@pytest.mark.asyncio
async def test_get_files_list_empty(
    async_client: AsyncClient,
    override_auth_admin
):
    """Test getting empty file list."""
    # Act
    response = await async_client.get("/api/file")

    # Assert
    assert response.status_code == 200
    data = response.json()
    assert "data" in data
    assert isinstance(data["data"], list)


@pytest.mark.integration
@pytest.mark.asyncio
async def test_get_files_list_with_data(
    async_client: AsyncClient,
    user_factory,
    file_factory,
    override_auth_admin
):
    """Test getting list of files."""
    # Arrange - create test data owned by the admin user
    user = await user_factory(email=override_auth_admin.email)
    file1 = await file_factory(author_id=user.id, file_name="file1.pdf")
    file2 = await file_factory(author_id=user.id, file_name="file2.pdf")

    # Act
    response = await async_client.get("/api/file")

    # Assert
    assert response.status_code == 200
    data = response.json()
    assert "data" in data
    assert len(data["data"]) == 2
    file_names = [f["file_name"] for f in data["data"]]
    assert "file1.pdf" in file_names
    assert "file2.pdf" in file_names


@pytest.mark.integration
@pytest.mark.asyncio
async def test_get_file_by_id(
    async_client: AsyncClient,
    user_factory,
    file_factory,
    override_auth_admin,
    test_engine
):
    """Test getting a specific file by ID."""
    # Arrange
    user = await user_factory()
    file = await file_factory(author_id=user.id, file_name="test.pdf")

    # Act
    response = await async_client.get(f"/api/file/{file.id}")

    # Assert
    assert response.status_code == 200
    data = response.json()
    assert data["file_name"] == "test.pdf"
    assert data["id"] == str(file.id)


@pytest.mark.integration
@pytest.mark.asyncio
async def test_upload_file_success(
    async_client: AsyncClient,
    user_factory,
    test_engine,
    override_auth_admin,
    mock_databricks_storage,
    mocker
):
    """Test successful file upload."""
    # Arrange - create user in database
    user = await user_factory(email=override_auth_admin.email)

    # Mock Databricks Storage
    mocker.patch(
        'controllers.file.file_uploader.DatabricksStorage',
        return_value=mock_databricks_storage
    )
    mocker.patch(
        'controllers.file.controller.DatabricksStorage',
        return_value=mock_databricks_storage
    )

    # Create file data
    file_content = b"test file content"
    files = {
        "file": ("test.pdf", io.BytesIO(file_content), "application/pdf")
    }
    data = {"filename": "test.pdf"}

    # Act - POST to upload endpoint
    response = await async_client.post(
        "/api/file",
        files=files,
        data=data
    )

    # Assert - check response
    assert response.status_code == 200
    response_data = response.json()
    assert response_data["file_name"] == "test.pdf"
    assert "id" in response_data

    # Assert - verify in database
    file_in_db = await test_engine.find_one(
        Files,
        Files.id == ObjectId(response_data["id"])
    )
    assert file_in_db is not None
    assert file_in_db.file_name == "test.pdf"


@pytest.mark.integration
@pytest.mark.asyncio
async def test_delete_file_success(
    async_client: AsyncClient,
    user_factory,
    file_factory,
    override_auth_admin,
    test_engine
):
    """Test successful file deletion."""
    # Arrange
    user = await user_factory(email=override_auth_admin.email)
    file = await file_factory(author_id=user.id, file_name="to_delete.pdf")

    # Act
    response = await async_client.delete(f"/api/file/{file.id}")

    # Assert - check response
    assert response.status_code == 204

    # Assert - verify file is soft-deleted (status='deleted') or hard-deleted (None)
    file_in_db = await test_engine.find_one(Files, Files.id == file.id)
    # Implementation uses soft delete
    assert file_in_db is None or file_in_db.status == "deleted"


@pytest.mark.integration
@pytest.mark.asyncio
@pytest.mark.auth
async def test_get_files_unauthorized(async_client: AsyncClient):
    """Test accessing files without authentication."""
    # No auth override - should fail
    response = await async_client.get("/api/file")
    assert response.status_code == 401


@pytest.mark.integration
@pytest.mark.asyncio
async def test_search_files(
    async_client: AsyncClient,
    user_factory,
    file_factory,
    override_auth_admin
):
    """Test searching files with filters."""
    # Arrange - create files owned by admin user
    user = await user_factory(email=override_auth_admin.email)
    file1 = await file_factory(author_id=user.id, file_name="report.pdf", status="completed")
    file2 = await file_factory(author_id=user.id, file_name="draft.pdf", status="processing")

    # Act - search with filter
    search_request = {
        "filters": {},
        "page": 1,
        "page_size": 10
    }
    response = await async_client.post("/api/file/search", json=search_request)

    # Assert
    assert response.status_code == 200
    data = response.json()
    assert "data" in data
    # Both files should be returned
    assert len(data["data"]) >= 2


@pytest.mark.integration
@pytest.mark.asyncio
async def test_get_file_preview_meta(
    async_client: AsyncClient,
    user_factory,
    file_factory,
    override_auth_admin
):
    """Test getting file preview metadata."""
    # Arrange - create file owned by admin user
    user = await user_factory(email=override_auth_admin.email)
    file = await file_factory(
        author_id=user.id,
        file_name="preview.pdf",
        status="completed",
        total_pages=10
    )

    # Act
    response = await async_client.get(f"/api/file/{file.id}/preview")

    # Assert
    assert response.status_code == 200
    data = response.json()
    assert "file_name" in data or "total_pages" in data


@pytest.mark.integration
@pytest.mark.asyncio
async def test_file_lifecycle(
    async_client: AsyncClient,
    user_factory,
    test_engine,
    override_auth_admin,
    mock_databricks_storage,
    mocker
):
    """
    Test complete file lifecycle: upload -> get -> delete.
    This is a comprehensive integration test.
    """
    # Arrange - create user
    user = await user_factory(email=override_auth_admin.email)

    # Mock Databricks Storage
    mocker.patch(
        'controllers.file.file_uploader.DatabricksStorage',
        return_value=mock_databricks_storage
    )
    mocker.patch(
        'controllers.file.controller.DatabricksStorage',
        return_value=mock_databricks_storage
    )

    # Step 1: Upload file
    file_content = b"lifecycle test content"
    files = {
        "file": ("lifecycle.pdf", io.BytesIO(file_content), "application/pdf")
    }
    data = {"filename": "lifecycle.pdf"}

    upload_response = await async_client.post(
        "/api/file",
        files=files,
        data=data
    )
    assert upload_response.status_code == 200
    file_id = upload_response.json()["id"]

    # Step 2: Get file
    get_response = await async_client.get(f"/api/file/{file_id}")
    assert get_response.status_code == 200
    assert get_response.json()["file_name"] == "lifecycle.pdf"

    # Step 3: List files (should include our file)
    list_response = await async_client.get("/api/file")
    assert list_response.status_code == 200
    file_ids = [f["id"] for f in list_response.json()["data"]]
    assert file_id in file_ids

    # Step 4: Delete file
    delete_response = await async_client.delete(f"/api/file/{file_id}")
    assert delete_response.status_code == 204

    # Step 5: Verify file is gone (or marked as deleted)
    file_in_db = await test_engine.find_one(Files, Files.id == ObjectId(file_id))
    # File may be soft-deleted (status='deleted') or hard-deleted (None)
    assert file_in_db is None or file_in_db.status == "deleted"
