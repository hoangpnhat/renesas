"""
Add context_length and temperature to model_management

Revision ID: 7b844e41-db
Revises: None (Initial migration)
Create Date: 2026-02-11 15:25:50
"""

#  Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.

from motor.motor_asyncio import AsyncIOMotorDatabase
from pymongo import UpdateOne

from migrations.base import BaseMigration


class Migration7B844E41DB(BaseMigration):
    def __init__(self):
        super().__init__(revision="7b844e41-db", down_revision=None)
        self.collection_name = "modelmanagement"

    async def upgrade(self, db: AsyncIOMotorDatabase) -> bool:
        """Apply the migration - Add context_length and temperature fields"""
        try:
            collection = db.get_collection(self.collection_name)

            # Check if collection exists
            if self.collection_name not in await db.list_collection_names():
                print(f"Collection '{self.collection_name}' does not exist. Skipping migration.")
                return True

            # Count total documents
            total_count = await collection.count_documents({})
            print(f"Found {total_count} documents in '{self.collection_name}' collection")

            if total_count == 0:
                print(f"No documents to migrate in '{self.collection_name}'")
                return True

            # Create progress bar
            pbar = await self.create_progress_bar(
                total_count,
                f"Adding context_length and temperature",
                "docs"
            )

            processed = 0
            updated_count = 0

            try:
                # Process documents in batches
                while processed < total_count:
                    # Fetch batch of documents
                    batch = await collection.find({}).skip(processed).limit(self.batch_size).to_list(length=self.batch_size)

                    if not batch:
                        break

                    # Prepare bulk update operations
                    updates = []
                    for doc in batch:
                        # Check if default_completion_options exists
                        if "default_completion_options" in doc and doc["default_completion_options"]:
                            completion_options = doc["default_completion_options"]

                            # Add context_length if missing
                            if "context_length" not in completion_options:
                                completion_options["context_length"] = 4096

                            # Add temperature if missing
                            if "temperature" not in completion_options:
                                completion_options["temperature"] = 0.7

                            # Create update operation
                            updates.append(
                                UpdateOne(
                                    {"_id": doc["_id"]},
                                    {"$set": {"default_completion_options": completion_options}}
                                )
                            )
                        else:
                            # Create default_completion_options if it doesn't exist
                            updates.append(
                                UpdateOne(
                                    {"_id": doc["_id"]},
                                    {
                                        "$set": {
                                            "default_completion_options": {
                                                "max_tokens": 256,
                                                "context_length": 4096,
                                                "temperature": 0.7
                                            }
                                        }
                                    }
                                )
                            )

                    # Execute bulk update if there are operations
                    if updates:
                        result = await self._commit_batch(collection, updates, final=(processed + len(batch) >= total_count))
                        updated_count += result

                    processed += len(batch)
                    pbar.update(len(batch))

                    print(f"Processed {processed}/{total_count} documents")

            finally:
                pbar.close()

            print(
                f"✓ Migration {self.revision} completed successfully. "
                f"Updated {updated_count} documents in '{self.collection_name}'"
            )
            return True

        except Exception as e:
            print(f"Migration {self.revision} failed: {e}")
            import traceback
            traceback.print_exc()
            return False

    async def downgrade(self, db: AsyncIOMotorDatabase) -> None:
        """Revert the migration - Remove context_length and temperature fields"""
        try:
            collection = db.get_collection(self.collection_name)

            # Check if collection exists
            if self.collection_name not in await db.list_collection_names():
                print(f"Collection '{self.collection_name}' does not exist. Nothing to revert.")
                return

            # Count total documents
            total_count = await collection.count_documents({})
            print(f"Reverting migration for {total_count} documents in '{self.collection_name}'")

            if total_count == 0:
                print(f"No documents to revert in '{self.collection_name}'")
                return

            # Create progress bar
            pbar = await self.create_progress_bar(
                total_count,
                f"Removing context_length and temperature",
                "docs"
            )

            processed = 0

            try:
                while processed < total_count:
                    batch = await collection.find({}).skip(processed).limit(self.batch_size).to_list(length=self.batch_size)

                    if not batch:
                        break

                    updates = []
                    for doc in batch:
                        if "default_completion_options" in doc and doc["default_completion_options"]:
                            completion_options = doc["default_completion_options"]

                            # Remove context_length and temperature
                            completion_options.pop("context_length", None)
                            completion_options.pop("temperature", None)

                            updates.append(
                                UpdateOne(
                                    {"_id": doc["_id"]},
                                    {"$set": {"default_completion_options": completion_options}}
                                )
                            )

                    if updates:
                        await self._commit_batch(collection, updates, final=(processed + len(batch) >= total_count))

                    processed += len(batch)
                    pbar.update(len(batch))

            finally:
                pbar.close()

            print(f"✓ Migration {self.revision} reverted successfully")

        except Exception as e:
            print(f"Migration {self.revision} downgrade failed: {e}")
            import traceback
            traceback.print_exc()
            raise

    async def validate_prerequisites(self, db: AsyncIOMotorDatabase) -> bool:
        """Validate prerequisites before migration"""
        # Check if the collection exists (optional - will be created if not)
        collections = await db.list_collection_names()

        if self.collection_name not in collections:
            print(
                f"Collection '{self.collection_name}' does not exist. "
                "This is acceptable - migration will be skipped."
            )
        else:
            print(f"Collection '{self.collection_name}' exists. Ready to migrate.")

        return True

    async def post_migration_check(self, db: AsyncIOMotorDatabase) -> bool:
        """Validate after migration"""
        try:
            collection = db.get_collection(self.collection_name)

            # Check if collection exists
            if self.collection_name not in await db.list_collection_names():
                print(f"Collection '{self.collection_name}' does not exist. Nothing to validate.")
                return True

            # Count documents with new fields
            total_docs = await collection.count_documents({})

            if total_docs == 0:
                print("No documents to validate")
                return True

            # Sample check - verify a few documents have the new fields
            sample_docs = await collection.find({}).limit(5).to_list(length=5)

            for doc in sample_docs:
                if "default_completion_options" in doc and doc["default_completion_options"]:
                    options = doc["default_completion_options"]

                    if "context_length" not in options:
                        print(f"Document {doc['_id']} missing context_length field")
                        return False

                    if "temperature" not in options:
                        print(f"Document {doc['_id']} missing temperature field")
                        return False

                    # Validate field types and ranges
                    if not isinstance(options["context_length"], int) or options["context_length"] < 1:
                        print(f"Document {doc['_id']} has invalid context_length value")
                        return False

                    if not isinstance(options["temperature"], (int, float)) or not (0.0 <= options["temperature"] <= 2.0):
                        print(f"Document {doc['_id']} has invalid temperature value")
                        return False

            print(
                f"Post-migration validation passed. Checked {len(sample_docs)} sample documents."
            )
            return True

        except Exception as e:
            print(f"Post-migration check failed: {e}")
            import traceback
            traceback.print_exc()
            return False


migration = Migration7B844E41DB()
