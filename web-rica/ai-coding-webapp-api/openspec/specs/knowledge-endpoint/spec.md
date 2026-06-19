# Capability: Knowledge Base Management

## Purpose

This capability governs how the system creates, retrieves, updates, archives, removes, and shares knowledge bases. All operations require a verified identity.

## Requirements

### Requirement: Knowledge base listing returns the authenticated user's records

The system SHALL return a list of knowledge bases owned by the authenticated user. When no records exist, the system SHALL return an empty list.

#### Scenario: Empty list is returned when no knowledge bases exist

- **GIVEN** the authenticated user owns no knowledge base records
- **WHEN** the user requests their knowledge bases
- **THEN** the system returns a response containing an empty `data` array

#### Scenario: Owned knowledge bases are returned

- **GIVEN** the authenticated user owns two persisted knowledge base records
- **WHEN** the user requests their knowledge bases
- **THEN** the response contains a `data` array of length 2 whose entries include the names of the persisted records

### Requirement: Knowledge base creation persists a new record

The system SHALL create a new knowledge base record, associate it with the submitting user, and return the persisted data.

#### Scenario: A new knowledge base is created and persisted

- **GIVEN** an authenticated user submits a valid creation request with a name
- **WHEN** the knowledge base is created
- **THEN** the system persists a `Knowledge` record with the supplied `name`, sets `owner_id` to the authenticated user's identifier, and returns the persisted data

### Requirement: Knowledge bases can be searched

The system SHALL support search queries over knowledge base records and return matching results.

#### Scenario: Search returns matching results

- **GIVEN** an authenticated user submits a search request
- **WHEN** the search is executed
- **THEN** the response contains a `data` array of matching knowledge bases

### Requirement: Knowledge base detail exposes ownership status

When retrieving a single knowledge base, the system SHALL indicate whether the requester is the owner.

#### Scenario: Detail view reflects ownership for the owner

- **GIVEN** a knowledge base owned by the authenticated user
- **WHEN** the user retrieves that knowledge base's detail
- **THEN** the response `id` matches the requested record and `is_owner` is `true`

### Requirement: Knowledge base general settings can be updated

The system SHALL accept updates to a knowledge base's general settings and persist the changes.

#### Scenario: General settings update is persisted

- **GIVEN** an authenticated user owns a persisted knowledge base
- **WHEN** the user updates its description
- **THEN** the persisted record reflects the new description

### Requirement: Knowledge bases can be archived

The system SHALL support archiving a knowledge base, marking it as archived without removing the record.

#### Scenario: Knowledge base is marked as archived

- **GIVEN** an authenticated user owns a persisted knowledge base
- **WHEN** the user archives it
- **THEN** the persisted record has `is_archived == true`

### Requirement: Knowledge bases can be deleted

The system SHALL support deletion of a knowledge base. The record is either removed or marked as deleted.

#### Scenario: Knowledge base is deleted

- **GIVEN** an authenticated user owns a persisted knowledge base
- **WHEN** the user deletes it
- **THEN** the record is either absent or has `is_deleted == true`

### Requirement: Knowledge base sharing state is queryable

The system SHALL return the current list of share recipients for a knowledge base. When no shares exist, the system SHALL return an empty list.

#### Scenario: Empty share list is returned when no shares exist

- **GIVEN** a knowledge base with no share records
- **WHEN** the owner queries its shares
- **THEN** the system returns an empty list

### Requirement: Knowledge bases can be shared

The system SHALL accept share requests specifying one or more recipients and persist the resulting share records.

#### Scenario: Knowledge base is shared with a recipient

- **GIVEN** an authenticated user owns a knowledge base and a valid target recipient exists
- **WHEN** the user shares the knowledge base with the recipient
- **THEN** the system accepts the request and persists the share

### Requirement: Unauthenticated access is rejected

All knowledge base operations are protected. The system SHALL reject any request that does not carry a valid authenticated identity.

#### Scenario: Unauthenticated request is rejected

- **WHEN** a client with no valid identity attempts to access a knowledge base resource
- **THEN** the system rejects the request with an authentication error
