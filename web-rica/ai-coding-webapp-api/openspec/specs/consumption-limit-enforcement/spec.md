# Capability: Consumption Limit Enforcement

## Purpose

This capability aggregates token consumption over rolling time windows, calculates remaining tokens against configured thresholds, and enforces consumption limits for each user-model pair in real-time.

## Requirements

### Requirement: Aggregate consumption over rolling time window
The system SHALL calculate total token consumption for each user-model pair over a rolling time window defined by consumption_range_hours.

#### Scenario: Calculate consumption within time window
- **WHEN** model has consumption_range_hours=24 and user has consumption records within last 24 hours
- **THEN** system sums all token_count values from records where timestamp >= (now - 24 hours)

#### Scenario: Exclude consumption outside time window
- **WHEN** model has consumption_range_hours=1 and user has consumption records older than 1 hour
- **THEN** system excludes records with timestamp < (now - 1 hour) from aggregation

#### Scenario: No consumption records in window
- **WHEN** user has no consumption records within rolling time window
- **THEN** system returns consumed=0 for that user-model pair

### Requirement: Calculate remaining tokens against threshold
The system SHALL compute remaining token count by subtracting consumed tokens from model threshold.

#### Scenario: Tokens remaining under threshold
- **WHEN** model threshold is 1000 and user consumed 600 tokens in time window
- **THEN** system calculates remaining=400

#### Scenario: Tokens exhausted - at threshold
- **WHEN** model threshold is 1000 and user consumed exactly 1000 tokens in time window
- **THEN** system calculates remaining=0

#### Scenario: Tokens exceeded - over threshold
- **WHEN** model threshold is 1000 and user consumed 1200 tokens in time window
- **THEN** system calculates remaining=-200

### Requirement: Include consumption data in API response
The system SHALL return consumption_limit object in model configuration response containing enabled, threshold, rangeHours, consumed, remaining, blocked, and blocked_until fields.

#### Scenario: Response with consumption limit data
- **WHEN** client requests model configurations for model with consumption tracking enabled
- **THEN** response includes consumption_limit with enabled, threshold, rangeHours, consumed, remaining, blocked, and blocked_until fields

#### Scenario: Multiple models with different consumption states
- **WHEN** client requests configurations and user has different consumption levels across models
- **THEN** response includes accurate per-model consumption data with independent blocked flags

#### Scenario: Client derives exceeded status from remaining field
- **WHEN** client receives consumption_limit response
- **THEN** client calculates exceeded status as `remaining < 0` to determine if threshold was surpassed

### Requirement: Isolate consumption data by user
The system SHALL ensure users can only see their own consumption data and cannot access other users' consumption records.

#### Scenario: User sees only own consumption
- **WHEN** authenticated user requests model configurations
- **THEN** consumption_limit data reflects only that user's token consumption

#### Scenario: Different users different consumption
- **WHEN** multiple users consume tokens for same model
- **THEN** each user's response shows only their own consumed, remaining, and blocked values

### Requirement: Use UTC for all time window calculations
The system SHALL perform rolling time window calculations using server time in UTC to ensure consistency across time zones.

#### Scenario: Time window calculated in UTC
- **WHEN** system calculates rolling window with consumption_range_hours=24
- **THEN** window_start = current_utc_time - timedelta(hours=24) and window_end = current_utc_time

#### Scenario: Consistent across time zones
- **WHEN** clients in different time zones request configurations at same UTC moment
- **THEN** all receive consumption data calculated using same UTC-based time window

### Requirement: Recalculate limits on every request
The system SHALL aggregate consumption and calculate limits dynamically on each API request to ensure real-time accuracy.

#### Scenario: Fresh calculation per request
- **WHEN** client makes consecutive requests with consumption data in between
- **THEN** each response reflects updated consumption totals including newly recorded data

#### Scenario: Configuration change takes immediate effect
- **WHEN** administrator changes model threshold and user subsequently requests configurations
- **THEN** response shows consumption evaluated against new threshold

### Requirement: Handle models without consumption configuration
The system SHALL skip consumption limit calculation for models that do not have both threshold and consumption_range_hours set.

#### Scenario: Model without consumption config
- **WHEN** model does not have threshold or consumption_range_hours configured
- **THEN** response includes consumption_limit=null and system does not perform aggregation

### Requirement: Optimize aggregation queries for performance
The system SHALL use MongoDB aggregation pipeline with compound index on user_id and model_id to ensure sub-100ms query latency.

#### Scenario: Efficient aggregation
- **WHEN** system aggregates consumption for user-model pair
- **THEN** MongoDB uses compound index to narrow records before time range filtering

#### Scenario: Multiple models aggregated concurrently
- **WHEN** user has consumption records for 5 models with consumption tracking
- **THEN** system completes all 5 aggregation queries within acceptable latency budget
