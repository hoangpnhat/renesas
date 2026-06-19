# Capability: AI Model Configuration (Admin)

## Purpose

This capability allows administrators to manage the AI model configurations available in the system — listing, creating, updating, and removing model records — and to query available Databricks serving endpoints and role assignments. All operations require an authenticated administrator identity.

## Requirements

### Requirement: Unauthenticated access is rejected

All model configuration operations are protected. The system SHALL reject any request that does not carry a valid authenticated identity.

#### Scenario: Unauthenticated request is rejected

- **GIVEN** a client that supplies no valid authentication token
- **WHEN** the client attempts any model configuration operation
- **THEN** the system rejects the request with an authentication error

### Requirement: Available Databricks endpoints are filtered to ready, named entries

The system SHALL query Databricks for serving endpoints and return only those that are both in a ready state and have a non-empty name.

#### Scenario: Only ready, named endpoints are returned

- **GIVEN** Databricks returns a mix of endpoints — some ready and some not, some with empty names
- **WHEN** an administrator requests the available Databricks endpoints
- **THEN** the response contains only endpoints that are both ready and have a non-empty name

#### Scenario: Databricks service failure surfaces as an error

- **GIVEN** the Databricks service is unavailable
- **WHEN** an administrator requests the available Databricks endpoints
- **THEN** the system returns an error conveying the upstream failure

### Requirement: RICA configuration exposes only active models

The system SHALL return the current RICA configuration including only models that have not been soft-deleted. The `consumption_limit` field for every model SHALL be `null`.

#### Scenario: Configuration excludes soft-deleted models

- **GIVEN** some models have been soft-deleted
- **WHEN** an administrator retrieves the RICA configuration
- **THEN** the response includes only non-deleted models, and every model entry has `consumption_limit` equal to `null`

### Requirement: Available model roles are enumerable

The system SHALL return the complete set of available model roles as a list of name–value pairs.

#### Scenario: All model roles are returned

- **WHEN** an administrator requests the available model roles
- **THEN** the response is a list covering all defined role values, each entry carrying a `name` and a `value`

### Requirement: Models can be listed with pagination

The system SHALL return a paginated list of model records. When no models exist, the list SHALL be empty.

#### Scenario: Empty list is returned when no models exist

- **GIVEN** no models have been created
- **WHEN** an administrator requests the model list
- **THEN** the response contains an empty `data` array and `total` is `0`

#### Scenario: Paginated list reflects correct totals and page size

- **GIVEN** three models exist
- **WHEN** an administrator requests the first page with a page size of two
- **THEN** `total` is `3`, `data` contains two entries, and the pagination metadata reflects the requested page and page size

### Requirement: Individual model records can be retrieved

The system SHALL return the full record for a model identified by its unique identifier. A request for a non-existent identifier SHALL be rejected with a not-found error.

#### Scenario: Existing model is returned

- **GIVEN** a model with a known identifier exists
- **WHEN** an administrator retrieves that model
- **THEN** the response contains the model's `id` and `name` matching the stored record

#### Scenario: Non-existent model returns a not-found error

- **GIVEN** no model exists for the supplied identifier
- **WHEN** an administrator attempts to retrieve it
- **THEN** the system returns a not-found error

### Requirement: Model creation persists a new record

The system SHALL accept a model creation request, persist the record, and return the persisted data.

#### Scenario: A new model is created and persisted

- **GIVEN** an administrator submits a valid model creation payload
- **WHEN** the model is created
- **THEN** the system persists the record with fields matching the payload and returns the persisted data

#### Scenario: Model creation is rejected when consumption fields are incomplete

The `threshold` and `consumption_range_hours` fields are paired: both must be provided together. Supplying only one SHALL be rejected.

- **GIVEN** an administrator submits a payload that supplies `threshold` but omits `consumption_range_hours`, or vice versa
- **WHEN** the model creation is attempted
- **THEN** the system rejects the request with a validation error

### Requirement: Model fields can be partially updated

The system SHALL apply only the fields supplied in an update request, leaving all other fields at their current values.

#### Scenario: Supplied fields are updated; omitted fields are preserved

- **GIVEN** an existing model record
- **WHEN** an administrator submits an update containing only a new `name`
- **THEN** the persisted record's `name` reflects the update and all other fields remain unchanged

#### Scenario: Optional fields can be cleared by setting them to null

- **GIVEN** a model with `threshold` and `consumption_range_hours` set
- **WHEN** an administrator submits an update setting both to `null`
- **THEN** the persisted record has both `threshold` and `consumption_range_hours` equal to `null`

### Requirement: Model deletion removes or soft-deletes the record

The system SHALL accept a deletion request for an existing model and remove or mark it as deleted. A deletion request for a non-existent identifier SHALL be rejected with a not-found error.

#### Scenario: Existing model is deleted

- **GIVEN** an existing model record
- **WHEN** an administrator deletes it
- **THEN** the record is either absent or has `deleted_at` set

#### Scenario: Deleting a non-existent model returns a not-found error

- **GIVEN** no model exists for the supplied identifier
- **WHEN** an administrator attempts to delete it
- **THEN** the system returns a not-found error
