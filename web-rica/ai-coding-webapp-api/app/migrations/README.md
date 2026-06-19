# Database Migrations

This directory contains database migration scripts for the RICA backend application.

## Structure

```
migrations/
├── __init__.py
├── base.py                  # Base migration class
├── manager.py              # Migration manager
├── cli.py                  # Command-line interface
├── README.md              # This file
└── versions/              # Migration files
    ├── __init__.py
    └── 20260211_152550_7b844e41-db_add_context_length_and_temperature_to_model_management.py
```

## Running Migrations

### Option 1: Using Docker (Recommended for Production)

The migration runs in a separate container, isolated from the main application.

#### Prerequisites
- Docker installed and running
- `.env` file configured with database credentials

#### Using Shell Script (Linux/Mac/WSL)

```bash
cd c:/Project/rica-app/backend

# Apply all pending migrations
./run-migration.sh upgrade

# Check migration status
./run-migration.sh status
```

#### Using Batch Script (Windows)

```cmd
cd c:\Project\rica-app\backend

REM Apply all pending migrations
run-migration.bat upgrade

REM Check migration status
run-migration.bat status
```

#### Using Docker Compose

```bash
cd c:/Project/rica-app/backend

# Apply all pending migrations
docker-compose -f docker-compose.migration.yml up migration

# Check migration status (edit docker-compose.migration.yml to set command: status)
docker-compose -f docker-compose.migration.yml up migration
```

#### Using Docker Directly

```bash
cd c:/Project/rica-app/backend

# Build migration image
docker build -f Dockerfile.migration -t rica-migration .

# Run migration
docker run --rm \
  --env MONGO_URI="your_mongo_uri" \
  --env DATABASE_NAME="your_database" \
  rica-migration upgrade

# Check status
docker run --rm \
  --env MONGO_URI="your_mongo_uri" \
  --env DATABASE_NAME="your_database" \
  rica-migration status
```

### Option 2: Running Locally (Development)

#### Check Migration Status

To see which migrations have been applied:

```bash
cd c:/Project/rica-app/backend
python -m app.migrations.cli status
```

#### Apply All Pending Migrations

To apply all pending migrations:

```bash
cd c:/Project/rica-app/backend
python -m app.migrations.cli upgrade
```

## Available Migrations

### 7b844e41-db: Add context_length and temperature to model_management

**Date**: 2026-02-11

**Description**: Adds `context_length` (default: 4096) and `temperature` (default: 0.7) fields to the `default_completion_options` in the `modelmanagement` collection.

**Changes**:
- Adds `context_length` field with default value 4096
- Adds `temperature` field with default value 0.7
- Creates `default_completion_options` structure if missing
- Validates field types and ranges in post-migration check

**Affected Collections**: `modelmanagement`

## Creating New Migrations

To create a new migration:

1. Create a new file in `migrations/versions/` with the naming pattern:
   ```
   YYYYMMDD_HHMMSS_<revision-id>_<description>.py
   ```

2. Use the following template:

```python
"""
Description of migration

Revision ID: <unique-id>
Revises: <previous-revision-id>
Create Date: YYYY-MM-DD HH:MM:SS
"""

from motor.motor_asyncio import AsyncIOMotorDatabase
from migrations.base import BaseMigration


class Migration<UniqueClassName>(BaseMigration):
    def __init__(self):
        super().__init__(revision="<unique-id>", down_revision="<previous-revision-id>")
        self.collection_name = "<collection-name>"

    async def upgrade(self, db: AsyncIOMotorDatabase) -> bool:
        """Apply the migration"""
        # Implementation here
        return True

    async def downgrade(self, db: AsyncIOMotorDatabase) -> None:
        """Revert the migration"""
        # Implementation here
        pass


migration = Migration<UniqueClassName>()
```

## Migration History

Migrations are tracked in the `migration_history` collection in MongoDB with the following structure:

```json
{
  "revision": "7b844e41-db",
  "down_revision": null,
  "applied_at": "2026-02-11T15:25:50Z"
}
```

## Notes

- Migrations are applied in order based on their `down_revision` chain
- Each migration can only be applied once
- The system automatically creates the `migration_history` collection on first run
- Batch size for processing documents can be configured in the MigrationManager (default: 64)
