#!/bin/bash

# Migration entrypoint script
# Usage:
#   ./migrate.sh upgrade    # Apply all pending migrations
#   ./migrate.sh status     # Show migration status

set -e

echo "========================================"
echo "RICA Backend - Database Migration Tool"
echo "========================================"
echo ""

# Default command is upgrade if not specified
COMMAND=${1:-upgrade}

echo "Command: $COMMAND"
echo "MongoDB URI: ${MONGO_URI:0:30}..."
echo "Database: $DATABASE_NAME"
echo ""

# Wait for MongoDB to be ready
echo "Waiting for MongoDB to be ready..."
until python -c "
from pymongo import MongoClient
import os
import sys
try:
    client = MongoClient(os.environ['MONGO_URI'], serverSelectionTimeoutMS=5000)
    client.admin.command('ping')
    print('MongoDB is ready!')
    sys.exit(0)
except Exception as e:
    print(f'MongoDB not ready: {e}')
    sys.exit(1)
" 2>/dev/null; do
    echo "MongoDB is unavailable - waiting..."
    sleep 2
done

echo ""
echo "Running migration: $COMMAND"
echo "========================================"
echo ""

# Run the migration
python -m app.migrations.cli "$COMMAND"

echo ""
echo "========================================"
echo "Migration completed successfully!"
echo "========================================"
