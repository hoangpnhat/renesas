# Capability: Prompt Management

## Purpose

This capability governs how the system creates, retrieves, updates, removes, and shares prompt records, and controls access to the files attached to them. All operations require a verified identity.

## Requirements

### Requirement: Prompt creation persists a new prompt record

The system SHALL accept a prompt submission containing a file, type, name, and description, store the file in external storage, and associate the new record with the submitting user.

#### Scenario: A new prompt is created and persisted

- **GIVEN** an authenticated user submits a prompt with a file, a type, a name, and a description
- **WHEN** the prompt is created
- **THEN** the system persists a `Prompt` record whose `name`, `description`, `type`, and file metadata match the submission, and whose `owner_id` equals the submitting user's identifier

### Requirement: Prompt listing returns results filtered by type

The system SHALL return a paginated list of prompts matching a requested type. Each entry SHALL include the prompt's identifier, name, description, and timestamps. Prompts of a different type SHALL be excluded.

#### Scenario: Prompts are listed and filtered by type

- **GIVEN** multiple prompts of different types exist for the authenticated user
- **WHEN** the user requests prompts of a specific type
- **THEN** the response contains only prompts of the requested type, each with `id`, `name`, `description`, `created_at`, and `last_modified` fields

### Requirement: Prompt detail exposes ownership and edit permissions

When retrieving a single prompt, the system SHALL indicate whether the requester owns the record and whether they are permitted to modify it.

#### Scenario: Prompt detail includes permission flags for the owner

- **GIVEN** a prompt owned by the authenticated user
- **WHEN** the user retrieves that prompt's detail
- **THEN** the response includes `id` and `owner_id` matching the record, and both `is_owner` and `can_edit` are `true`

### Requirement: Retrieving a non-existent prompt is rejected

The system SHALL reject a detail request when no prompt record matches the supplied identifier.

#### Scenario: Non-existent prompt access returns a not-found error

- **GIVEN** no prompt record exists for the supplied identifier
- **WHEN** a user requests that prompt's detail
- **THEN** the system returns a not-found error

### Requirement: Prompt file download streams the stored content

The system SHALL stream the file associated with a prompt to the requester, identifying the file by its stored name.

#### Scenario: Prompt file is streamed to the requester

- **GIVEN** a prompt with an associated stored file
- **WHEN** the user requests the file download
- **THEN** the system streams the file content and identifies it by its stored filename

### Requirement: Prompt sharing state is queryable

The system SHALL return the current list of share records for a prompt. When no shares have been granted, the system SHALL return an empty list.

#### Scenario: Empty share list is returned when no shares exist

- **GIVEN** a prompt with no share records
- **WHEN** the owner queries the prompt's shares
- **THEN** the system returns an empty list

### Requirement: Prompt fields can be partially updated

The system SHALL accept partial update requests and apply only the supplied fields, leaving all other fields at their current values.

#### Scenario: Supplied fields are updated; omitted fields are preserved

- **GIVEN** a prompt with an existing name and file metadata
- **WHEN** the user submits an update containing only a new name
- **THEN** the persisted prompt's name reflects the update and the file metadata is unchanged

### Requirement: Prompt deletion marks the record as deleted

The system SHALL soft-delete a prompt by marking it as deleted rather than removing the document.

#### Scenario: Prompt is soft-deleted on deletion request

- **GIVEN** a prompt owned by the authenticated user
- **WHEN** the user deletes the prompt
- **THEN** the system marks the prompt as deleted; the document remains in the database

### Requirement: Prompt shares can be applied

The system SHALL accept a share request specifying a target and role, and persist the resulting share record.

#### Scenario: Share is applied and persisted

- **GIVEN** a prompt owned by the authenticated user
- **WHEN** the user shares the prompt with a target user in a specified role
- **THEN** the system persists a share record associating the prompt with the target user in that role

### Requirement: Unauthenticated access is rejected

All prompt operations are protected. The system SHALL reject any request that does not carry a valid authenticated identity.

#### Scenario: Unauthenticated request is rejected

- **WHEN** a client with no valid identity attempts to access a prompt resource
- **THEN** the system rejects the request with an authentication error
