"""
Test suite for Model Management contextLength field in configuration responses.

This test ensures that the get_models_configuration endpoint properly includes
the top-level contextLength field cloned from defaultCompletionOptions.contextLength.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'app'))

import pytest
from unittest.mock import AsyncMock, MagicMock
from app.schemas.model_management import RICAConfigurationResponse, Model, DefaultCompletionOptions
from app.models.model_management import ModelManagement, DefaultCompletionOptions as ModelDefaultCompletionOptions
from app.controllers.admin.model_management import ModelManagementController
from app.const.enums import RoleEnum


@pytest.fixture
def mock_model_crud():
    """Fixture to create a mocked CRUD instance."""
    return MagicMock()


@pytest.fixture
def controller(mock_model_crud):
    """Fixture to create a ModelManagementController with mocked dependencies."""
    controller = ModelManagementController()
    controller.model_crud = mock_model_crud
    return controller


@pytest.mark.asyncio
async def test_get_models_configuration_includes_context_length(controller, mock_model_crud):
    """
    Test that get_models_configuration returns models with top-level contextLength.

    Given: A model with defaultCompletionOptions.contextLength = 128000
    When: get_models_configuration is called
    Then: The response should include contextLength at the top level with value 128000
    """
    # Arrange
    mock_completion_options = ModelDefaultCompletionOptions(
        max_tokens=256,
        context_length=128000,
        temperature=0.7
    )

    mock_model = ModelManagement(
        name="GPT-5.1",
        model_path="gpt-5.1-turbo",
        roles=[RoleEnum.ADMIN.value, RoleEnum.USER.value],
        default_completion_options=mock_completion_options
    )

    mock_model_crud.get_multi = AsyncMock(return_value=[mock_model])

    # Act
    response = await controller.get_models_configuration()

    # Assert
    assert isinstance(response, RICAConfigurationResponse)
    assert len(response.config.models) == 1

    model_response = response.config.models[0]
    assert model_response.name == "GPT-5.1"
    assert model_response.context_length == 128000
    assert model_response.default_completion_options is not None
    assert model_response.default_completion_options.context_length == 128000
    assert model_response.default_completion_options.max_tokens == 256
    assert model_response.default_completion_options.temperature == 0.7


@pytest.mark.asyncio
async def test_get_models_configuration_with_different_context_lengths(controller, mock_model_crud):
    """
    Test that different models return their respective contextLength values.

    Given: Multiple models with different contextLength values
    When: get_models_configuration is called
    Then: Each model should have its corresponding contextLength at the top level
    """
    # Arrange
    models = [
        ModelManagement(
            name="Model-A",
            model_path="model-a",
            roles=[RoleEnum.USER.value],
            default_completion_options=ModelDefaultCompletionOptions(
                max_tokens=1000,
                context_length=50000,
                temperature=0.5
            )
        ),
        ModelManagement(
            name="Model-B",
            model_path="model-b",
            roles=[RoleEnum.ADMIN.value],
            default_completion_options=ModelDefaultCompletionOptions(
                max_tokens=2000,
                context_length=200000,
                temperature=0.9
            )
        )
    ]

    mock_model_crud.get_multi = AsyncMock(return_value=models)

    # Act
    response = await controller.get_models_configuration()

    # Assert
    assert len(response.config.models) == 2

    model_a = response.config.models[0]
    assert model_a.name == "Model-A"
    assert model_a.context_length == 50000
    assert model_a.default_completion_options.context_length == 50000

    model_b = response.config.models[1]
    assert model_b.name == "Model-B"
    assert model_b.context_length == 200000
    assert model_b.default_completion_options.context_length == 200000


@pytest.mark.asyncio
async def test_get_models_configuration_without_completion_options(controller, mock_model_crud):
    """
    Test handling of models without defaultCompletionOptions.

    Given: A model without defaultCompletionOptions
    When: get_models_configuration is called
    Then: contextLength should be None
    """
    # Arrange
    mock_model = ModelManagement(
        name="Basic-Model",
        model_path="basic-model",
        roles=[RoleEnum.USER.value],
        default_completion_options=None
    )

    mock_model_crud.get_multi = AsyncMock(return_value=[mock_model])

    # Act
    response = await controller.get_models_configuration()

    # Assert
    assert len(response.config.models) == 1
    model_response = response.config.models[0]
    assert model_response.name == "Basic-Model"
    assert model_response.context_length is None
    assert model_response.default_completion_options is None


@pytest.mark.asyncio
async def test_response_schema_format_matches_template(controller, mock_model_crud):
    """
    Test that the response format exactly matches the specified template structure.

    The response should have:
    - name (string)
    - model (string)
    - contextLength (integer)
    - defaultCompletionOptions (object with maxTokens, contextLength, temperature)
    """
    # Arrange
    mock_completion_options = ModelDefaultCompletionOptions(
        max_tokens=128000,
        context_length=200000,
        temperature=0.7
    )

    mock_model = ModelManagement(
        name="GPT-5.1",
        model_path="gpt-5.1-turbo",
        roles=[RoleEnum.ADMIN.value],
        default_completion_options=mock_completion_options
    )

    mock_model_crud.get_multi = AsyncMock(return_value=[mock_model])

    # Act
    response = await controller.get_models_configuration()

    # Assert
    model_dict = response.config.models[0].model_dump(by_alias=True)

    # Verify top-level fields
    assert "name" in model_dict
    assert "model" in model_dict
    assert "contextLength" in model_dict
    assert "defaultCompletionOptions" in model_dict

    # Verify nested fields in defaultCompletionOptions
    completion_opts = model_dict["defaultCompletionOptions"]
    assert "maxTokens" in completion_opts
    assert "contextLength" in completion_opts
    assert "temperature" in completion_opts

    # Verify values match the template structure
    assert model_dict["name"] == "GPT-5.1"
    assert model_dict["contextLength"] == 200000
    assert completion_opts["maxTokens"] == 128000
    assert completion_opts["contextLength"] == 200000
    assert completion_opts["temperature"] == 0.7


@pytest.mark.asyncio
async def test_context_length_consistency(controller, mock_model_crud):
    """
    Test that contextLength is properly synchronized between top-level and nested fields.

    This test verifies that the implementation maintains consistency where:
    - Top-level contextLength is cloned from defaultCompletionOptions.contextLength
    - Both values should be identical in the response
    """
    # Arrange
    test_context_length = 150000

    mock_completion_options = ModelDefaultCompletionOptions(
        max_tokens=64000,
        context_length=test_context_length,
        temperature=0.8
    )

    mock_model = ModelManagement(
        name="Consistency-Test-Model",
        model_path="test-model",
        roles=[RoleEnum.USER.value],
        default_completion_options=mock_completion_options
    )

    mock_model_crud.get_multi = AsyncMock(return_value=[mock_model])

    # Act
    response = await controller.get_models_configuration()

    # Assert
    model_response = response.config.models[0]

    # Both contextLength values should match
    assert model_response.context_length == test_context_length
    assert model_response.default_completion_options.context_length == test_context_length
    assert model_response.context_length == model_response.default_completion_options.context_length
