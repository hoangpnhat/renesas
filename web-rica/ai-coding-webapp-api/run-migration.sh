#!/bin/bash

# Standalone migration runner script
# This script runs migrations in a Docker container without docker-compose

set -e

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Configuration
IMAGE_NAME="rica-migration"
CONTAINER_NAME="rica-migration-runner"

echo -e "${BLUE}========================================"
echo "RICA Backend - Migration Runner"
echo -e "========================================${NC}"
echo ""

# Check if .env file exists
if [ ! -f ".env" ]; then
    echo -e "${RED}Error: .env file not found${NC}"
    echo "Please create a .env file with the following variables:"
    echo "  MONGO_URI=mongodb://user:password@host:port"
    echo "  DATABASE_NAME=your_database"
    exit 1
fi

# Load environment variables
export $(cat .env | grep -v '^#' | xargs)

# Check required environment variables
if [ -z "$MONGO_URI" ] || [ -z "$DATABASE_NAME" ]; then
    echo -e "${RED}Error: Required environment variables not set${NC}"
    echo "Please ensure .env contains:"
    echo "  MONGO_URI=mongodb://user:password@host:port"
    echo "  DATABASE_NAME=your_database"
    exit 1
fi

# Default command is upgrade
COMMAND=${1:-upgrade}

echo -e "${YELLOW}Building migration container...${NC}"
docker build -f Dockerfile.migration -t $IMAGE_NAME .

echo ""
echo -e "${YELLOW}Running migration: $COMMAND${NC}"
echo "Database: $DATABASE_NAME"
echo ""

# Remove old container if exists
docker rm -f $CONTAINER_NAME 2>/dev/null || true

# Run migration
docker run \
    --name $CONTAINER_NAME \
    --env MONGO_URI="$MONGO_URI" \
    --env DATABASE_NAME="$DATABASE_NAME" \
    --env EXTERNAL_MONGO_URI="${EXTERNAL_MONGO_URI:-$MONGO_URI}" \
    --env EXTERNAL_DATABASE_NAME="${EXTERNAL_DATABASE_NAME:-$DATABASE_NAME}" \
    --rm \
    $IMAGE_NAME \
    $COMMAND

echo ""
echo -e "${GREEN}========================================"
echo "Migration completed!"
echo -e "========================================${NC}"
