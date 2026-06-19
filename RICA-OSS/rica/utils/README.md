# RICA Utilities

This directory contains RICA-specific utility functions that are shared across multiple RICA adapters and components.

## Modules

### `paths.ts`

Path utilities for RICA-specific directories and files.

#### Key Functions

| Function                             | Description                                                |
| ------------------------------------ | ---------------------------------------------------------- |
| `getRicaGlobalPath()`                | Get RICA global directory (`~/.rica` or `RICA_GLOBAL_DIR`) |
| `ensureRicaGlobalPath()`             | Get and create RICA global directory if needed             |
| `getLocalEnvironmentDotFilePath()`   | Path to `~/.rica/.local`                                   |
| `getStagingEnvironmentDotFilePath()` | Path to `~/.rica/.staging`                                 |
| `getRicaLogsPath()`                  | Path to `~/.rica/logs`                                     |
| `getRicaSessionsPath()`              | Path to `~/.rica/sessions`                                 |

#### Environment Switching

RICA uses dot files in `~/.rica/` to control environment:

```bash
# Switch to staging/non-prod
touch ~/.rica/.staging

# Switch to local development
touch ~/.rica/.local

# Return to production
rm ~/.rica/.staging ~/.rica/.local
```

**Priority:** `.local` > `.staging` > IDE settings > `CONTROL_PLANE_ENV`

## Design Principles

Following REPO_STANDARD.md:

1. **RICA-specific utilities live in `rica/`** - Not in `core/` or `packages/`
2. **Reusable across adapters** - Auth, config, control-plane can all use these
3. **Independent from Continue** - `~/.rica` is separate from `~/.continue`
4. **Configurable via environment** - `RICA_GLOBAL_DIR` allows custom paths

## Usage

```typescript
import {
  getRicaGlobalPath,
  getLocalEnvironmentDotFilePath,
  getStagingEnvironmentDotFilePath,
} from "../../utils/paths";

// Check if staging environment is enabled
if (fs.existsSync(getStagingEnvironmentDotFilePath())) {
  // Use staging configuration
}
```
