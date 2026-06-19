# Running Database Migrations with Docker

This document explains how to run database migrations using a separate Docker container, independent from the main application.

## Overview

The migration system is containerized separately to:
- **Isolate migration logic** from the main application
- **Prevent conflicts** between migration and application runtime
- **Enable safe deployments** with pre-deployment migration checks
- **Support CI/CD pipelines** with dedicated migration steps

## Architecture

```
┌─────────────────────────┐
│  Main Application       │
│  (Dockerfile)           │
│  - FastAPI Server       │
│  - Business Logic       │
└─────────────────────────┘

┌─────────────────────────┐
│  Migration Container    │
│  (Dockerfile.migration) │
│  - Migration Scripts    │
│  - One-time Execution   │
└─────────────────────────┘
         │
         ├──> MongoDB
```

## Files Created

- **Dockerfile.migration** - Lightweight Docker image for migrations only
- **migrate.sh** - Entrypoint script with MongoDB health checks
- **run-migration.sh** - Unix/Linux/Mac runner script
- **run-migration.bat** - Windows runner script
- **docker-compose.migration.yml** - Docker Compose configuration

## Quick Start

### 1. Configure Environment Variables

Create or update `.env` file in the backend directory:

```env
# Required
MONGO_URI=mongodb://username:password@host:port/database
DATABASE_NAME=rica_db

# Optional (for cross-database migrations)
EXTERNAL_MONGO_URI=mongodb://username:password@external-host:port/database
EXTERNAL_DATABASE_NAME=rica_external_db
```

### 2. Run Migrations

#### On Linux/Mac/WSL:

```bash
cd c:/Project/rica-app/backend

# Make scripts executable
chmod +x run-migration.sh migrate.sh

# Run migrations
./run-migration.sh upgrade

# Check status
./run-migration.sh status
```

#### On Windows:

```cmd
cd c:\Project\rica-app\backend

REM Run migrations
run-migration.bat upgrade

REM Check status
run-migration.bat status
```

## Detailed Usage

### Using Standalone Scripts (Recommended)

The `run-migration.sh` (or `.bat` for Windows) scripts handle everything:

```bash
# Apply all pending migrations
./run-migration.sh upgrade

# Check migration status
./run-migration.sh status
```

**What it does:**
1. Loads environment variables from `.env`
2. Builds the migration Docker image
3. Runs the migration in an isolated container
4. Automatically cleans up the container after execution

### Using Docker Compose

Edit `docker-compose.migration.yml` to change the command:

```yaml
services:
  migration:
    command: upgrade  # or 'status'
```

Then run:

```bash
docker-compose -f docker-compose.migration.yml up migration
```

### Using Docker Directly

For more control:

```bash
# Build the migration image
docker build -f Dockerfile.migration -t rica-migration .

# Run upgrade
docker run --rm \
  --env MONGO_URI="mongodb://user:pass@host:port" \
  --env DATABASE_NAME="rica_db" \
  rica-migration upgrade

# Check status
docker run --rm \
  --env MONGO_URI="mongodb://user:pass@host:port" \
  --env DATABASE_NAME="rica_db" \
  rica-migration status
```

## CI/CD Integration

### GitHub Actions Example

```yaml
name: Run Database Migrations

on:
  push:
    branches: [main, develop]

jobs:
  migrate:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3

      - name: Build migration image
        run: |
          cd backend
          docker build -f Dockerfile.migration -t rica-migration .

      - name: Run migrations
        env:
          MONGO_URI: ${{ secrets.MONGO_URI }}
          DATABASE_NAME: ${{ secrets.DATABASE_NAME }}
        run: |
          docker run --rm \
            --env MONGO_URI="$MONGO_URI" \
            --env DATABASE_NAME="$DATABASE_NAME" \
            rica-migration upgrade
```

### GitLab CI Example

```yaml
migrate:
  stage: deploy
  image: docker:latest
  services:
    - docker:dind
  script:
    - cd backend
    - docker build -f Dockerfile.migration -t rica-migration .
    - docker run --rm
        --env MONGO_URI="$MONGO_URI"
        --env DATABASE_NAME="$DATABASE_NAME"
        rica-migration upgrade
  only:
    - main
    - develop
```

## Kubernetes Deployment

### Using Kubernetes Job

Create `migration-job.yaml`:

```yaml
apiVersion: batch/v1
kind: Job
metadata:
  name: rica-migration
spec:
  template:
    spec:
      containers:
      - name: migration
        image: your-registry/rica-migration:latest
        command: ["/migrate.sh", "upgrade"]
        env:
        - name: MONGO_URI
          valueFrom:
            secretKeyRef:
              name: mongodb-secret
              key: uri
        - name: DATABASE_NAME
          value: "rica_db"
      restartPolicy: OnFailure
  backoffLimit: 3
```

Deploy:

```bash
kubectl apply -f migration-job.yaml
kubectl logs job/rica-migration -f
```

## Docker Image Details

### Dockerfile.migration

The migration image is lightweight and contains only:
- Python runtime
- Required Python packages
- Migration scripts
- Core configuration modules
- Utility functions

**Size:** ~200MB (vs ~800MB for full application image)

### Entrypoint Script (migrate.sh)

Features:
- **MongoDB health check** - Waits for database to be ready
- **Clear logging** - Shows migration progress
- **Error handling** - Exits with proper codes on failure
- **Flexible commands** - Supports `upgrade`, `status`, etc.

## Troubleshooting

### Migration Container Exits Immediately

**Symptom:** Container starts and stops without running migrations

**Solution:** Check environment variables are set correctly:

```bash
docker run --rm \
  --env MONGO_URI="mongodb://..." \
  --env DATABASE_NAME="..." \
  rica-migration upgrade
```

### Cannot Connect to MongoDB

**Symptom:** "MongoDB is unavailable - waiting..."

**Solutions:**
1. Verify MongoDB is running
2. Check MONGO_URI is correct
3. Ensure network connectivity
4. If using Docker networks, add `--network` flag:

```bash
docker run --rm \
  --network rica-network \
  --env MONGO_URI="mongodb://mongodb:27017" \
  --env DATABASE_NAME="rica_db" \
  rica-migration upgrade
```

### Permission Denied on Shell Scripts

**Symptom:** `bash: ./run-migration.sh: Permission denied`

**Solution:**

```bash
chmod +x run-migration.sh migrate.sh
```

### Windows Line Ending Issues

**Symptom:** `/bin/bash^M: bad interpreter`

**Solution:** Convert line endings to Unix format:

```bash
dos2unix run-migration.sh migrate.sh
```

Or in Git:

```bash
git config core.autocrlf false
git rm --cached -r .
git reset --hard
```

## Best Practices

### 1. Pre-Deployment Migrations

Always run migrations before deploying new application versions:

```bash
# 1. Run migrations
./run-migration.sh upgrade

# 2. Verify status
./run-migration.sh status

# 3. Deploy application
docker-compose up -d app
```

### 2. Backup Before Migration

Create a database backup before running migrations:

```bash
# MongoDB backup
mongodump --uri="mongodb://..." --out=/backup/$(date +%Y%m%d_%H%M%S)

# Run migration
./run-migration.sh upgrade
```

### 3. Test in Staging First

Never run migrations directly in production without testing:

```bash
# Test in staging environment
export MONGO_URI="mongodb://staging-host..."
./run-migration.sh upgrade

# Verify in staging
# Then promote to production
```

### 4. Monitor Migration Progress

For large datasets, monitor migration logs:

```bash
# Run in foreground to see logs
./run-migration.sh upgrade

# Or with Docker
docker logs -f rica-migration-runner
```

## Environment Variables Reference

| Variable | Required | Description | Default |
|----------|----------|-------------|---------|
| `MONGO_URI` | Yes | MongoDB connection URI | - |
| `DATABASE_NAME` | Yes | Target database name | - |
| `EXTERNAL_MONGO_URI` | No | External MongoDB URI (for cross-DB migrations) | `MONGO_URI` |
| `EXTERNAL_DATABASE_NAME` | No | External database name | `DATABASE_NAME` |

## Migration Commands

| Command | Description |
|---------|-------------|
| `upgrade` | Apply all pending migrations |
| `status` | Show current migration status |

## Support

For issues or questions:
1. Check logs: `docker logs rica-migration-runner`
2. Verify environment variables in `.env`
3. Review migration history in MongoDB: `migration_history` collection
4. Check [MIGRATION README](app/migrations/README.md) for migration details
