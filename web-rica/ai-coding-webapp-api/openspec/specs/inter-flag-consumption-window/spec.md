# Inter-Flag Consumption Window

## Purpose

This capability extends the token blocking system with precise consumption window computation across multiple block events. It freezes the consumed snapshot during active blocks and uses a two-flag formula to compute accurate window boundaries.

## Requirements

### Requirement: Frozen window_end during active block
While a block is active, the system SHALL set `window_end = flag.timestamp` (not `now`) so that `consumed` reflects the snapshot that triggered the block and does not grow while the block is in effect.

#### Scenario: consumed is stable during an active block
- **WHEN** a block is active for a user-model pair AND a new consumption record is added after `flag.timestamp`
- **THEN** the `consumed` value returned by the API is unchanged because the new record falls outside `window_end = flag.timestamp`

#### Scenario: consumed jumps at block expiry
- **WHEN** a block expires (now > flag.timestamp + 2h) AND consumption was recorded during the block period
- **THEN** the `consumed` value includes those records because `window_end` shifts from `flag.timestamp` to `now`

#### Scenario: window_end returns to now after block expires
- **WHEN** checking consumption for a user-model pair AND the most recent flag is older than 2 hours
- **THEN** the system uses `window_end = now` (not the expired flag's timestamp)

### Requirement: Two-flag aggregation window start
When two or more flag records exist for a user-model pair and the most recent block is active, the system SHALL compute `window_start = max(flag_01.timestamp, flag_02.timestamp - consumption_range_hours)`, where flag_02 is the most recent flag and flag_01 is the previous flag.

#### Scenario: flag_01 is more recent than rolling start from flag_02
- **WHEN** flag_02 is active AND flag_01.timestamp > flag_02.timestamp - range_hours
- **THEN** system uses `flag_01.timestamp` as `window_start`, honouring the fresh-start boundary from the previous block

#### Scenario: flag_01 is older than rolling start from flag_02
- **WHEN** flag_02 is active AND flag_01.timestamp <= flag_02.timestamp - range_hours
- **THEN** system uses `flag_02.timestamp - range_hours` as `window_start`; the old flag has no effect

#### Scenario: two-flag formula applies regardless of how long ago flag_01 was set
- **WHEN** flag_02 is active AND flag_01 exists with any age
- **THEN** system uses `max(flag_01.timestamp, flag_02.timestamp - range_hours)` as `window_start`

### Requirement: Flag history retrieval helper
The system SHALL provide a helper that returns the N most recent flag records for a user-model pair sorted by timestamp descending, to support the two-flag window computation.

#### Scenario: Retrieve two most recent flags
- **WHEN** querying flag records with limit=2 for a user-model pair that has two or more flag records
- **THEN** system returns exactly two records with the most recent flag first

#### Scenario: Retrieve flags when fewer than requested exist
- **WHEN** querying flag records with limit=2 for a user-model pair that has zero or one flag record
- **THEN** system returns all available flag records (0 or 1) without error
