# Capability: Consumption Deduplication

## Purpose

This capability ensures that token consumption records submitted by clients are deduplicated to prevent duplicate entries from client-side retries or network issues. It requires clients to provide timestamps for consumption events and enforces uniqueness at the database level using a compound index on (user_id, model_id, client_timestamp).

## Requirements

### Requirement: Client timestamp is required for consumption records
The system SHALL require a `client_timestamp` field for all consumption records submitted via the `/api/user-model/configs` endpoint. The client_timestamp SHALL be a valid ISO 8601 datetime value representing when the consumption occurred on the client side.

#### Scenario: Valid consumption record with client_timestamp
- **WHEN** client submits consumption record with valid client_timestamp
- **THEN** system accepts and stores the record with the provided client_timestamp

#### Scenario: Missing client_timestamp
- **WHEN** client submits consumption record without client_timestamp field
- **THEN** system rejects the request with 422 validation error

#### Scenario: Invalid client_timestamp format
- **WHEN** client submits consumption record with non-datetime client_timestamp value
- **THEN** system rejects the request with 422 validation error

### Requirement: Future timestamps are rejected
The system SHALL reject consumption records where the `client_timestamp` is in the future relative to server time. This prevents abuse from clock skew or malicious timestamp manipulation.

#### Scenario: Future timestamp rejection
- **WHEN** client submits consumption record with client_timestamp > server_time
- **THEN** system rejects the request with 422 validation error indicating future timestamp not allowed

#### Scenario: Past timestamp acceptance
- **WHEN** client submits consumption record with client_timestamp <= server_time
- **THEN** system accepts the record regardless of how far in the past

#### Scenario: Current timestamp acceptance
- **WHEN** client submits consumption record with client_timestamp equal to server_time (within reasonable precision)
- **THEN** system accepts the record

### Requirement: Duplicate consumption records are prevented
The system SHALL prevent duplicate consumption records by enforcing a unique constraint on the combination of `(user_id, model_id, client_timestamp)`. When a duplicate record is detected, the system SHALL silently skip it without failing the entire request.

#### Scenario: First submission succeeds
- **WHEN** client submits consumption record for (user_id=U1, model_id=M1, client_timestamp=T1) for the first time
- **THEN** system creates a new consumption record in the database

#### Scenario: Duplicate submission is skipped
- **WHEN** client resubmits identical consumption record with same (user_id=U1, model_id=M1, client_timestamp=T1)
- **THEN** system silently skips the duplicate record without error

#### Scenario: Different timestamp creates new record
- **WHEN** client submits consumption record with same user_id and model_id but different client_timestamp
- **THEN** system creates a new consumption record (not considered duplicate)

#### Scenario: Partial batch with duplicates
- **WHEN** client submits batch of consumption records containing mix of new and duplicate records
- **THEN** system stores all new records and silently skips duplicates without failing the request

### Requirement: Request succeeds despite duplicates
The system SHALL return HTTP 200 OK response for `/api/user-model/configs` endpoint even when all or some consumption records are duplicates. The endpoint SHALL NOT treat duplicate detection as an error condition.

#### Scenario: All duplicates returns success
- **WHEN** client submits batch where all consumption records are duplicates
- **THEN** system returns 200 OK with model configurations

#### Scenario: Mixed batch returns success
- **WHEN** client submits batch with some new records and some duplicates
- **THEN** system returns 200 OK with model configurations after storing new records

#### Scenario: All new records returns success
- **WHEN** client submits batch where all consumption records are new
- **THEN** system returns 200 OK with model configurations after storing all records

### Requirement: Database enforces uniqueness constraint
The system SHALL create a unique compound index on `user_token_consumption` collection with fields `(user_id, model_id, client_timestamp)` to ensure atomicity of duplicate detection at the database level.

#### Scenario: Concurrent duplicate submissions
- **WHEN** two concurrent requests attempt to insert the same (user_id, model_id, client_timestamp)
- **THEN** database rejects the second insert with duplicate key error and only one record is stored

#### Scenario: Index exists after migration
- **WHEN** database migration runs
- **THEN** unique index on (user_id, model_id, client_timestamp) exists in user_token_consumption collection

### Requirement: Duplicate detection is logged for monitoring
The system SHALL log duplicate consumption records at INFO level for monitoring and debugging purposes, without exposing errors to the client.

#### Scenario: Duplicate logged
- **WHEN** system detects duplicate consumption record
- **THEN** system logs message with user_id, model_id, and count of duplicates skipped

#### Scenario: No duplicates, no log spam
- **WHEN** all consumption records are new
- **THEN** system does not log duplicate-related messages
