#  Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.

import importlib.util
import logging
from pathlib import Path
from typing import Any, Optional

from motor.motor_asyncio import AsyncIOMotorDatabase
from pymongo import ASCENDING
from utils.helpers import datetime_now_sec

from .base import BaseMigration


class MigrationManager:
    def __init__(
        self,
        db: AsyncIOMotorDatabase,
        migrations_path: str = "./app/migrations/versions",
        batch_size: int = 20,
    ):
        self.db = db
        self.migrations_path = Path(migrations_path)
        self.migrations_collection = "migration_history"
        self.batch_size = batch_size

    async def ensure_indexes(self):
        collection = self.db[self.migrations_collection]
        existing = await collection.index_information()

        required = {
            "revision_unique_index": ([("revision", ASCENDING)], True),
            "applied_at_index": ([("applied_at", ASCENDING)], False),
        }

        def find_index_by_keys(keys):
            keys_set = set(keys)
            for idx_name, idx_info in existing.items():
                if set(idx_info["key"]) == keys_set:
                    return idx_name
            return None

        for name, (keys, unique) in required.items():
            if find_index_by_keys(keys):
                logging.info(f"Index on {keys} exists, skipping creation.")
            else:
                logging.warning(f"Creating missing index {name}.")
                await collection.create_index(keys, unique=unique, name=name)

        logging.info("All required indexes ensured.")

    async def initialize_migration_table(self):
        if self.migrations_collection not in await self.db.list_collection_names():
            await self.db.create_collection(self.migrations_collection)

            # Create index
            await self.db[self.migrations_collection].create_index(
                "revision", unique=True
            )
            logging.info("Migration history collection created")
        await self.ensure_indexes()

    def load_migrations(self) -> dict[str, BaseMigration]:
        migrations: dict[str, BaseMigration] = {}

        if not self.migrations_path.exists():
            self.migrations_path.mkdir(parents=True, exist_ok=True)
            return migrations

        for file_path in sorted(self.migrations_path.glob("*.py")):
            if file_path.name.startswith("__"):
                continue

            spec = importlib.util.spec_from_file_location(file_path.stem, file_path)
            if spec is None:
                continue
            module = importlib.util.module_from_spec(spec)
            if spec.loader is None:
                continue
            spec.loader.exec_module(module)

            if hasattr(module, "migration"):
                migration: BaseMigration = module.migration
                migrations[migration.revision] = migration

        return migrations

    async def get_applied_migrations(self) -> list[str]:
        cursor = (
            self.db[self.migrations_collection]
            .find({}, {"revision": 1})
            .sort("applied_at", 1)
        )

        return [doc["revision"] async for doc in cursor]

    async def get_migration_chain(
        self, migrations: dict[str, BaseMigration]
    ) -> list[BaseMigration]:
        """Build the complete migration chain from first to last"""
        chain: list[BaseMigration] = []
        revision_map = {m.revision: m for m in migrations.values()}

        # Find head (migration with no dependents)
        all_down_revisions = {
            m.down_revision for m in migrations.values() if m.down_revision
        }
        heads = [m for m in migrations.values() if m.revision not in all_down_revisions]

        if not heads:
            return chain

        # Build chain from head backwards
        current: BaseMigration | None = heads[0]
        while current:
            chain.insert(0, current)
            current = revision_map.get(current.down_revision) if current.down_revision else None

        return chain

    async def upgrade(self, target_revision: Optional[str] = None):
        """Apply migrations up to target revision"""

        await self.initialize_migration_table()

        migrations = self.load_migrations()
        if not migrations:
            logging.info("No migrations found")
            return

        applied = await self.get_applied_migrations()
        chain = await self.get_migration_chain(migrations)

        # Determine which migrations to apply
        to_apply = []
        for migration in chain:
            if migration.revision not in applied:
                to_apply.append(migration)
                if target_revision and migration.revision == target_revision:
                    break

        if not to_apply:
            logging.info("No pending migrations")
            return

        # Apply migrations
        for migration in to_apply:
            logging.info(f"Applying migration {migration.revision}")
            try:
                # Inject batch_size into migration
                migration.batch_size = self.batch_size

                # Validate prerequisites if method exists
                if hasattr(migration, "validate_prerequisites"):
                    if not await migration.validate_prerequisites(self.db):
                        raise Exception(
                            f"Prerequisites validation failed for {migration.revision}"
                        )

                if not await migration.upgrade(self.db):
                    raise Exception(f"Migration {migration.revision} reported failure")

                # Save migration to db
                await self.db[self.migrations_collection].insert_one(
                    {
                        "revision": migration.revision,
                        "down_revision": migration.down_revision,
                        "applied_at": datetime_now_sec(),
                    }
                )

                # Post-migration check if method exists
                if hasattr(migration, "post_migration_check"):
                    if not await migration.post_migration_check(self.db):
                        logging.warning(
                            f"Post-migration check failed for {migration.revision}"
                        )

                logging.info(f"Migration {migration.revision} applied successfully")

            except Exception as e:
                logging.error(f"Migration {migration.revision} failed: {e}")
                raise

    async def show_status(self):
        """Show detailed migration status"""
        migrations = self.load_migrations()
        applied = await self.get_applied_migrations()
        chain = await self.get_migration_chain(migrations)

        pending = [m.revision for m in chain if m.revision not in applied]
        current_revision = applied[-1] if applied else None

        print("=== Migration Status ===")
        print(f"Current revision: {current_revision or 'None (base)'}")
        print(f"Applied migrations: {len(applied)}/{len(chain)}")

        if pending:
            print(f"Pending migrations: {', '.join(pending)}")
        else:
            print("No pending migrations")
