# RICA Authentication Adapters

This directory contains RICA-specific authentication implementation using EntraID.

## Components

- `EntraIDAuthProvider.ts` - Main authentication provider for VS Code
- `SecretStorage.ts` - Encrypted session storage
- `AuthTypes.ts` - EntraID-specific type definitions
- `session.ts` - Session management utilities

## Authentication Flow

1. User initiates login via VS Code command
2. `EntraIDAuthProvider` opens EntraID OAuth flow
3. Exchange authorization code for access token
4. Exchange EntraID token with RICA backend for MDP token
5. Store encrypted session with both tokens
6. Automatically refresh tokens before expiration

## Environment Support

- Production: `rica.global.renesas.com`
- Staging: `rica-uat.global.renesas.com`
- Local: `localhost:8082`

## Token Management

- EntraID access token: Used for backend API calls (documents, config, etc.)
- MDP token: Used for LLM proxy access
- Refresh token: Used for automatic token renewal
- Automatic refresh: Triggered at 2/3 of token lifetime
