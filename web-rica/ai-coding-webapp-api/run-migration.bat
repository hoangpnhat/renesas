@echo off
REM Windows batch script to run migrations in Docker

setlocal enabledelayedexpansion

echo ========================================
echo RICA Backend - Migration Runner
echo ========================================
echo.

REM Configuration
set IMAGE_NAME=rica-migration
set CONTAINER_NAME=rica-migration-runner

REM Check if .env file exists
if not exist ".env" (
    echo Error: .env file not found
    echo Please create a .env file with the following variables:
    echo   MONGO_URI=mongodb://user:password@host:port
    echo   DATABASE_NAME=your_database
    exit /b 1
)

REM Load environment variables from .env file
for /f "usebackq tokens=1,* delims==" %%a in (".env") do (
    set "%%a=%%b"
)

REM Check required environment variables
if not defined MONGO_URI (
    echo Error: MONGO_URI not set in .env file
    exit /b 1
)
if not defined DATABASE_NAME (
    echo Error: DATABASE_NAME not set in .env file
    exit /b 1
)

REM Default command is upgrade
set COMMAND=%1
if "%COMMAND%"=="" set COMMAND=upgrade

echo Building migration container...
docker build -f Dockerfile.migration -t %IMAGE_NAME% .

echo.
echo Running migration: %COMMAND%
echo Database: %DATABASE_NAME%
echo.

REM Remove old container if exists
docker rm -f %CONTAINER_NAME% 2>nul

REM Set external variables with defaults
if not defined EXTERNAL_MONGO_URI set EXTERNAL_MONGO_URI=%MONGO_URI%
if not defined EXTERNAL_DATABASE_NAME set EXTERNAL_DATABASE_NAME=%DATABASE_NAME%

REM Run migration
docker run ^
    --name %CONTAINER_NAME% ^
    --env MONGO_URI=%MONGO_URI% ^
    --env DATABASE_NAME=%DATABASE_NAME% ^
    --env EXTERNAL_MONGO_URI=%EXTERNAL_MONGO_URI% ^
    --env EXTERNAL_DATABASE_NAME=%EXTERNAL_DATABASE_NAME% ^
    --rm ^
    %IMAGE_NAME% ^
    %COMMAND%

echo.
echo ========================================
echo Migration completed!
echo ========================================
