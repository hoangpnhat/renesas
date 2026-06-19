# Capability: Model Configuration Management

## Purpose

This capability manages per-model token consumption configuration, allowing administrators to set and validate consumption limits (threshold and time range) for each AI model in the system.

## Requirements

### Requirement: Store token consumption configuration per model
The system SHALL extend ModelManagement model with optional threshold and consumption_range_hours fields to configure per-model consumption limits.

#### Scenario: Model with consumption limits configured
- **WHEN** administrator sets threshold to 1000 and consumption_range_hours to 24 for a model
- **THEN** ModelManagement record contains threshold=1000 and consumption_range_hours=24

#### Scenario: Model without consumption limits
- **WHEN** administrator does not set threshold and consumption_range_hours for a model
- **THEN** ModelManagement record has threshold=None and consumption_range_hours=None

### Requirement: Validate consumption configuration values
The system SHALL validate threshold and consumption_range_hours fields to ensure they meet business constraints.

#### Scenario: Threshold must be positive
- **WHEN** administrator attempts to set threshold to zero or negative value
- **THEN** system rejects configuration with validation error

#### Scenario: Consumption range must not exceed retention period
- **WHEN** administrator attempts to set consumption_range_hours greater than 168 (7 days)
- **THEN** system rejects configuration with error indicating range exceeds maximum retention period

#### Scenario: Both fields required for consumption tracking
- **WHEN** administrator sets threshold without consumption_range_hours
- **THEN** system rejects configuration indicating both fields required for consumption tracking

### Requirement: Admin can configure model consumption limits via API
The system SHALL allow administrators to set threshold and consumption_range_hours fields when creating or updating models through the admin API endpoint.

#### Scenario: Create model with consumption limits
- **WHEN** administrator creates a new model with threshold=1000 and consumption_range_hours=24
- **THEN** system creates ModelManagement record with those values
- **AND** validates that both fields meet business constraints

#### Scenario: Update model to add consumption limits
- **WHEN** administrator updates existing model to add threshold=500 and consumption_range_hours=1
- **THEN** system updates ModelManagement record with new values
- **AND** consumption tracking becomes enabled for that model

#### Scenario: Update model to change consumption limits
- **WHEN** administrator updates model changing threshold from 1000 to 2000
- **THEN** system updates ModelManagement record with new threshold
- **AND** maintains existing consumption_range_hours value

#### Scenario: Update model to remove consumption limits
- **WHEN** administrator sets both threshold=null and consumption_range_hours=null
- **THEN** system updates ModelManagement record clearing both fields
- **AND** consumption tracking becomes disabled for that model

#### Scenario: Reject partial consumption configuration on create
- **WHEN** administrator creates model with threshold but without consumption_range_hours
- **THEN** system returns 422 validation error
- **AND** error message indicates both fields required together

#### Scenario: Reject partial consumption configuration on update
- **WHEN** administrator updates model setting consumption_range_hours but leaving threshold as null
- **THEN** system returns 422 validation error
- **AND** model record remains unchanged

#### Scenario: Admin endpoint accepts optional consumption fields
- **WHEN** administrator creates or updates model without providing threshold or consumption_range_hours
- **THEN** system accepts request and leaves those fields as null
- **AND** model operates without consumption tracking

### Requirement: Return model configurations with consumption settings
The system SHALL include consumption limit configuration in API response for models that have threshold and consumption_range_hours set.

#### Scenario: Model with consumption configuration
- **WHEN** client requests model configurations and model has threshold and consumption_range_hours set
- **THEN** response includes consumption_limit object with enabled=true, threshold, and range_hours fields

#### Scenario: Model without consumption configuration
- **WHEN** client requests model configurations and model does not have threshold or consumption_range_hours set
- **THEN** response includes consumption_limit=null for that model

### Requirement: Exclude deleted models from configuration response
The system SHALL filter out models with deleted_at field set when returning configurations to clients.

#### Scenario: Deleted model excluded
- **WHEN** client requests model configurations and model has deleted_at timestamp
- **THEN** system excludes that model from response

#### Scenario: Active model included
- **WHEN** client requests model configurations and model has deleted_at=null
- **THEN** system includes that model in response

### Requirement: Query models with consumption configuration efficiently
The system SHALL retrieve only models that have both threshold and consumption_range_hours fields set when calculating consumption limits.

#### Scenario: Filter models with consumption config
- **WHEN** system needs to calculate consumption limits
- **THEN** database query filters for models where threshold IS NOT NULL AND consumption_range_hours IS NOT NULL

### Requirement: Support gradual rollout of consumption tracking
The system SHALL allow models to operate without consumption limits by leaving threshold and consumption_range_hours unset.

#### Scenario: Mixed model configuration
- **WHEN** some models have consumption limits configured and others do not
- **THEN** system tracks and enforces limits only for configured models

#### Scenario: Enable consumption tracking for existing model
- **WHEN** administrator adds threshold and consumption_range_hours to previously unconfigured model
- **THEN** system immediately begins calculating consumption limits for that model using existing historical records
