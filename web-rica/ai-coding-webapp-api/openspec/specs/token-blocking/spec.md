# Token Blocking

## Purpose

This capability manages temporary blocking of users when their token consumption exceeds configured thresholds. It implements a 2-hour block mechanism with automatic expiry, flag record management, and consumption window reset behavior to give users a "fresh start" after serving their block time.

## Requirements

### Requirement: Block activation when threshold exceeded
The system SHALL activate a 2-hour block for a user-model pair when the user's token consumption meets or exceeds the configured threshold for that model within the consumption range window.

#### Scenario: First time exceeding threshold
- **WHEN** user's total consumption in the rolling window reaches or exceeds the model's threshold AND no active block exists
- **THEN** system creates or updates a flag record with current timestamp and returns blocked=true in API response

#### Scenario: Already blocked when consumption occurs
- **WHEN** user's consumption exceeds threshold AND an active block already exists (flag record within last 2 hours)
- **THEN** system does NOT update the flag record and maintains the existing block expiry time

#### Scenario: Exceeding threshold after block expired
- **WHEN** user's consumption exceeds threshold AND previous flag record is older than 2 hours
- **THEN** system updates the existing flag record's timestamp to current time, restarting the 2-hour block period

### Requirement: Automatic block expiry
The system SHALL automatically expire blocks after 2 hours from the flag record's timestamp without requiring manual intervention.

#### Scenario: Block expires after 2 hours
- **WHEN** checking block status for a user-model pair AND the flag record timestamp is more than 2 hours old
- **THEN** system returns blocked=false and blocked_until=null in API response

#### Scenario: Block remains active within 2-hour window
- **WHEN** checking block status for a user-model pair AND the flag record timestamp is less than 2 hours old
- **THEN** system returns blocked=true and blocked_until=(flag_timestamp + 2 hours) in API response

### Requirement: Flag record management
The system SHALL create a new flag record on every block activation. The system SHALL NOT update an existing flag record in place when a block is re-triggered; each blocking event produces a distinct document so that flag history is preserved for multi-block window computation.

#### Scenario: No existing flag record
- **WHEN** activating a block for a user-model pair that has never been blocked before
- **THEN** system creates a new UserTokenConsumption record with flag=true, token_count=0, and current timestamp

#### Scenario: Existing flag record from previous block
- **WHEN** activating a block for a user-model pair that already has one or more flag records (regardless of age)
- **THEN** system creates a NEW flag record with current timestamp instead of updating any existing record

#### Scenario: Multiple flag records accumulate over time
- **WHEN** a user-model pair is blocked, expires, then blocked again
- **THEN** two separate flag documents exist with distinct timestamps; the older one is not modified

### Requirement: Flag record structure
The system SHALL store flag records as UserTokenConsumption documents with flag=true and token_count=0 to mark block periods without affecting consumption totals.

#### Scenario: Flag record does not affect consumption
- **WHEN** aggregating total consumption for a user-model pair
- **THEN** system excludes flag records (flag=true) from the sum, so they do not contribute to threshold calculations

#### Scenario: Flag record has required fields
- **WHEN** creating or updating a flag record
- **THEN** record SHALL have user_id, model_id, flag=true, token_count=0, and timestamp fields

### Requirement: Consumption aggregation uses flag as window reset point
The system SHALL compute `(window_start, window_end)` according to the following state table, using the two most recent flag records where applicable:

| State | `window_start` | `window_end` |
|---|---|---|
| No flags | `now − range_hours` | `now` |
| flag_01 active | `flag_01.timestamp − range_hours` | `flag_01.timestamp` |
| flag_01 expired | `max(flag_01.timestamp, now − range_hours)` | `now` |
| flag_02 active | `max(flag_01.timestamp, flag_02.timestamp − range_hours)` | `flag_02.timestamp` |
| flag_02 expired | `max(flag_02.timestamp, now − range_hours)` | `now` |

#### Scenario: Aggregation with no flag record
- **WHEN** aggregating consumption for a user-model pair with no flag record
- **THEN** system uses `window_start = now - consumption_range_hours` and `window_end = now`

#### Scenario: Aggregation during active first block
- **WHEN** aggregating consumption AND exactly one flag record exists AND flag timestamp is within the last 2 hours
- **THEN** system uses `window_start = flag_01.timestamp - range_hours` and `window_end = flag_01.timestamp`

#### Scenario: Aggregation after first block expires
- **WHEN** aggregating consumption AND exactly one flag record exists AND flag timestamp is older than 2 hours
- **THEN** system uses `window_start = max(flag_01.timestamp, now - range_hours)` and `window_end = now`

#### Scenario: Aggregation during active second block
- **WHEN** aggregating consumption AND two or more flag records exist AND the most recent flag is within the last 2 hours
- **THEN** system uses `window_start = max(flag_01.timestamp, flag_02.timestamp - range_hours)` and `window_end = flag_02.timestamp`

#### Scenario: Aggregation after second block expires
- **WHEN** aggregating consumption AND two or more flag records exist AND the most recent flag is older than 2 hours
- **THEN** system uses `window_start = max(flag_02.timestamp, now - range_hours)` and `window_end = now`

#### Scenario: Flag records excluded from aggregation sum
- **WHEN** aggregating total consumption for a user-model pair
- **THEN** system excludes all flag records (flag=true) from the token_count sum regardless of how many flag records exist

### Requirement: Efficient blocking queries
The system SHALL use an index on the flag field to enable efficient queries for checking block status across user-model pairs.

#### Scenario: Index on flag field exists
- **WHEN** querying for flag records during block status checks
- **THEN** database uses the flag field index to optimize query performance

### Requirement: API response includes block status
The system SHALL include blocked and blocked_until fields in the consumption limit section of the GET /api/user-model/configs response for every model with threshold configuration.

#### Scenario: User is blocked
- **WHEN** user requests model configuration AND user has an active block for a model
- **THEN** response includes blocked=true and blocked_until=<unix_timestamp> for that model's consumptionLimit

#### Scenario: User is not blocked
- **WHEN** user requests model configuration AND user has no active block for a model
- **THEN** response includes blocked=false and blocked_until=null for that model's consumptionLimit

#### Scenario: Model has no threshold configured
- **WHEN** user requests model configuration AND model has no threshold/consumption_range_hours configured
- **THEN** response includes consumptionLimit=null (no blocking applicable)

### Requirement: Consumption recording continues during block
The system SHALL continue to accept and record token consumption even when a user is blocked, without rejecting requests or returning error responses.

#### Scenario: Recording consumption while blocked
- **WHEN** user submits consumption data via POST /api/user-model/configs AND user is currently blocked for one or more models
- **THEN** system accepts the request, persists consumption records, and returns 200 OK with updated configuration including block status

#### Scenario: Consumption adds to total during block
- **WHEN** user records consumption while blocked AND consumption is within the rolling window
- **THEN** new consumption records count toward the user's total consumed tokens for threshold calculation

### Requirement: Per-model blocking isolation
The system SHALL maintain independent block status for each user-model pair, such that blocking one model does not affect the user's access to other models.

#### Scenario: User blocked on one model only
- **WHEN** user exceeds threshold for model A AND user is under threshold for model B
- **THEN** API response shows blocked=true for model A and blocked=false for model B

#### Scenario: Simultaneous blocks on multiple models
- **WHEN** user exceeds thresholds for both model A and model B
- **THEN** each model has its own independent flag record with separate timestamps and expiry times

### Requirement: Best-effort blocking
The system SHALL handle flag record operation failures gracefully by logging errors and continuing to return valid responses without failing the entire request.

#### Scenario: Flag creation fails
- **WHEN** attempting to create or update a flag record AND database operation fails
- **THEN** system logs the error and returns model configuration response without blocking information

#### Scenario: Block check query fails
- **WHEN** checking block status AND database query fails
- **THEN** system logs the error and defaults to blocked=false for the affected model in the response

### Requirement: Backward compatibility with existing records
The system SHALL treat existing UserTokenConsumption records without a flag field as non-flag records (flag=None) to maintain backward compatibility.

#### Scenario: Querying records without flag field
- **WHEN** querying for flag records AND UserTokenConsumption collection contains records created before flag field was added
- **THEN** system correctly identifies records with flag=true and ignores records where flag is None or absent

#### Scenario: New flag field is optional
- **WHEN** reading existing UserTokenConsumption records
- **THEN** system handles missing flag field gracefully with default value of None
