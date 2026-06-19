# RICA Configuration Adapters

This directory contains RICA-specific configuration management.

## Components

- `env.ts` - Environment detection and endpoint configuration
- `dynamicConfig.ts` - Dynamic configuration loader from RICA backend
- `RicaProfileLoader.ts` - Profile loader with dynamic model injection

## Dynamic Configuration

RICA loads model configuration dynamically from the backend at:

- Production: `https://rica.global.renesas.com/api/user-model/configs`
- Staging: `https://rica-uat.global.renesas.com/api/user-model/configs`

Configuration includes:

- Available LLM models
- Model parameters (context length, temperature limits, etc.)
- Token consumption quotas
- Provider mappings

## Environment Switching

RICA supports multiple methods for environment switching, with the following priority order:

### 1. Dot File Detection (Highest Priority)

Create dot files in the RICA global directory (`~/.rica/`) to switch environments:

| File               | Environment | Description                        |
| ------------------ | ----------- | ---------------------------------- |
| `~/.rica/.local`   | LOCAL_ENV   | Local development (localhost:8082) |
| `~/.rica/.staging` | STAGING_ENV | Non-prod/UAT environment           |

**Note:** `.local` takes precedence over `.staging` if both exist.

**Example:**

```bash
# Switch to staging/non-prod environment
touch ~/.rica/.staging

# Switch to local development
touch ~/.rica/.local

# Return to production (remove dot files)
rm ~/.rica/.staging ~/.rica/.local
```

### 2. VS Code Settings

If no dot files exist, environment is detected via VS Code settings:

- `rica.continueTestEnvironment`: `"production"` | `"staging"` | `"local"` | `"test"` | `"none"`

### 3. Environment Variable (Lowest Priority)

- `CONTROL_PLANE_ENV`: Can be set to override environment selection

## Environment Configuration

Each environment has specific:

- Backend API URLs
- EntraID client IDs (Prod vs Non-Prod)
- Auth endpoints

| Environment | APP_URL                               | EntraID         |
| ----------- | ------------------------------------- | --------------- |
| Production  | `https://rica.global.renesas.com`     | Prod client     |
| Staging     | `https://rica-uat.global.renesas.com` | Non-Prod client |
| Test        | `https://rica-uat.global.renesas.com` | Non-Prod client |
| Local       | `http://localhost:8082`               | Non-Prod client |

## Custom Global Directory

The RICA global directory defaults to `~/.rica/` but can be overridden:

```bash
export RICA_GLOBAL_DIR=/custom/path/to/rica
```
