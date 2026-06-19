# Capability: User Account Management

## Purpose

This capability governs how the system creates user accounts, exposes authenticated user profiles, manages per-user display settings, and supports member search. All operations require a verified identity except where noted.

## Requirements

### Requirement: User registration creates an account on first sign-in

The system SHALL create a new `User` record the first time a verified identity signs in, initialising the account as active with a validated email address.

#### Scenario: New account is created for an unregistered identity

- **GIVEN** no `User` record exists for the verified identity's email address
- **WHEN** the user registers
- **THEN** the system creates a `User` record with `is_active == True` and `email_validated == True`, and returns the new account data including `email` and `full_name`

#### Scenario: Registration is rejected when an account already exists

- **GIVEN** a `User` record already exists for the verified identity's email address
- **WHEN** the user attempts to register
- **THEN** the system rejects the request with a conflict error indicating the username already exists

### Requirement: Authenticated users can retrieve their own profile

The system SHALL return the full profile of the currently authenticated user, including their unique identifier, email address, and display name.

#### Scenario: Profile is returned for an authenticated user

- **GIVEN** a `User` record exists for the authenticated identity
- **WHEN** the user requests their own profile
- **THEN** the system returns `email`, `id`, and `full_name` matching the authenticated identity and the persisted record

### Requirement: User settings expose personalisation preferences

Each `User` record carries an embedded `UserSetting` document. The system SHALL expose these settings and apply sensible defaults when they have not been explicitly configured.

#### Scenario: Default settings are returned when none have been configured

- **GIVEN** a `User` record with no explicitly configured settings
- **WHEN** the user retrieves their settings
- **THEN** the system returns `preferred_language == "en"` and `mode == "light_mode"`

### Requirement: Settings updates are partial merges

The system SHALL update only the fields supplied in a settings change request, leaving all other fields at their current values. After persisting the change, the system SHALL invalidate any cached representation of the user.

#### Scenario: Supplied fields are updated; omitted fields are preserved

- **GIVEN** a `User` with default settings (`preferred_language == "en"`, `mode == "light_mode"`)
- **WHEN** the user submits a settings update containing only `mode == "night_mode"`
- **THEN** the persisted `UserSetting` has `mode == "night_mode"` and `preferred_language == "en"`, and the user's cached entry is invalidated

### Requirement: Member search returns matching users, excluding the requester

The system SHALL support partial-match search over user records. The authenticated requester SHALL never appear in the results regardless of whether their profile matches the query.

#### Scenario: Matching users are returned and the requester is excluded

- **GIVEN** multiple users exist in the system, including at least one whose name or email matches the search term
- **WHEN** an authenticated user searches with a query and a result limit
- **THEN** the response contains a `data` list with at least one entry matching the query, and the authenticated requester's account is not present in the list

### Requirement: Unauthenticated access is rejected

Profile and settings operations are protected. The system SHALL reject any request that does not carry a valid authenticated identity.

#### Scenario: Unauthenticated request is rejected

- **WHEN** a client with no valid identity attempts to access a protected user resource
- **THEN** the system rejects the request with an authentication error
