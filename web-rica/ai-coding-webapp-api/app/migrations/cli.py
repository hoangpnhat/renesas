#!/usr/bin/env python
"""
Migration CLI tool

Usage:
    python -m app.migrations.cli upgrade     # Apply all pending migrations
    python -m app.migrations.cli status      # Show migration status
"""

#  Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.

import asyncio
import sys
from pathlib import Path
from typing import Any

# Add parent directory to path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from motor.motor_asyncio import AsyncIOMotorClient
from core.config import settings
from migrations.manager import MigrationManager


async def main():
    """Main CLI entry point"""
    if len(sys.argv) < 2:
        print("Usage: python -m app.migrations.cli [upgrade|status]")
        sys.exit(1)

    command = sys.argv[1]

    # Connect to database
    client: AsyncIOMotorClient[Any] = AsyncIOMotorClient(settings.mongodb.MONGO_URI)
    db = client.get_database(settings.mongodb.DATABASE_NAME)

    # Create migration manager
    manager = MigrationManager(db, batch_size=64)

    try:
        if command == "upgrade":
            print("Applying migrations...")
            await manager.upgrade()
            print("✓ All migrations applied successfully")

        elif command == "status":
            await manager.show_status()

        else:
            print(f"Unknown command: {command}")
            print("Available commands: upgrade, status")
            sys.exit(1)

    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

    finally:
        client.close()


if __name__ == "__main__":
    asyncio.run(main())
