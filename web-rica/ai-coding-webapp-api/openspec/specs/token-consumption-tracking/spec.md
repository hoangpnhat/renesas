# Capability: Token Consumption Tracking

## Purpose

This capability records and validates token consumption data submitted by clients, persisting records with automatic expiration and indexing for efficient aggregation queries.

## Requirements

### Requirement: Record token consumption per user per model
The system SHALL accept token consumption data from clients via POST request body with separate input and output token counts, convert them using the formula `tokens = 5 * tokens_out + tokens_in`, and persist to the UserTokenConsumption collection with server-assigned timestamps in UTC.

#### Scenario: Client submits consumption data with input and output tokens
- **WHEN** client sends POST request to `/api/user-model/configs` with consumption array containing model identifier, tokens_in, and tokens_out
- **THEN** system creates UserTokenConsumption record with user_id, model_id, tokens_in, tokens_out, token_count (computed as 5 * tokens_out + tokens_in), and server-assigned timestamp

#### Scenario: Empty consumption data
- **WHEN** client sends POST request with empty consumption array
- **THEN** system accepts request and returns model configurations without creating consumption records

#### Scenario: Batch consumption submission
- **WHEN** client sends POST request with multiple consumption records for different models
- **THEN** system creates separate UserTokenConsumption record for each entry with same timestamp

### Requirement: Validate consumption data before persisting
The system SHALL validate each consumption record in the request body, ensuring both tokens_in and tokens_out are positive integers within allowed range, and reject invalid data with appropriate error messages.

#### Scenario: Invalid model identifier
- **WHEN** client submits consumption record with model_id that does not exist in ModelManagement
- **THEN** logs errors for invalid ones, and returns model configurations

#### Scenario: Missing required token fields
- **WHEN** client submits consumption record without tokens_in field OR without tokens_out field
- **THEN** system returns validation error indicating both tokens_in and tokens_out are required

#### Scenario: Invalid tokens_in - zero or negative
- **WHEN** client submits consumption record with tokens_in less than or equal to zero
- **THEN** system returns validation error indicating tokens_in must be positive integer

#### Scenario: Invalid tokens_out - zero or negative
- **WHEN** client submits consumption record with tokens_out less than or equal to zero
- **THEN** system returns validation error indicating tokens_out must be positive integer

#### Scenario: Invalid tokens_in - exceeds maximum
- **WHEN** client submits consumption record with tokens_in exceeding 1,000,000
- **THEN** system returns validation error indicating tokens_in exceeds maximum allowed value

#### Scenario: Invalid tokens_out - exceeds maximum
- **WHEN** client submits consumption record with tokens_out exceeding 1,000,000
- **THEN** system returns validation error indicating tokens_out exceeds maximum allowed value

#### Scenario: Batch size limit exceeded
- **WHEN** client submits consumption array with more than 1,000 records
- **THEN** system returns validation error indicating batch size limit exceeded

### Requirement: Use server-side timestamps for all consumption records
The system SHALL assign timestamps to consumption records using server time in UTC, ignoring any client-provided timestamp values.

#### Scenario: Server assigns timestamp
- **WHEN** client submits consumption data without timestamp field
- **THEN** system creates record with current server timestamp in UTC

#### Scenario: Client-provided timestamp ignored
- **WHEN** client submits consumption data with timestamp field in request body
- **THEN** system ignores client timestamp and assigns current server timestamp in UTC

### Requirement: Automatically expire consumption records after 7 days
The system SHALL configure MongoDB TTL index on timestamp field to automatically delete consumption records older than 7 days.

#### Scenario: Old records automatically deleted
- **WHEN** consumption record timestamp is older than 7 days
- **THEN** MongoDB TTL process automatically removes the record

#### Scenario: Recent records retained
- **WHEN** consumption record timestamp is within last 7 days
- **THEN** record remains in database and available for aggregation queries

### Requirement: Index consumption records for efficient querying
The system SHALL create compound index on user_id and model_id fields to optimize aggregation queries.

#### Scenario: Query optimization for user and model
- **WHEN** system queries consumption records filtered by user_id and model_id
- **THEN** MongoDB uses compound index for efficient record retrieval

### Requirement: Handle consumption recording failures gracefully
The system SHALL treat consumption recording as best-effort operation and continue returning model configurations even if recording fails.

#### Scenario: Recording failure does not block response
- **WHEN** consumption record persistence fails due to database error
- **THEN** system logs error and returns model configurations successfully

#### Scenario: Partial batch failure
- **WHEN** some consumption records in batch fail validation while others are valid
- **THEN** system persists valid records, logs errors for invalid ones, and returns model configurations
