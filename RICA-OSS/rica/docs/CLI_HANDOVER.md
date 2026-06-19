# RICA CLI — Handover Document

> **Status:** Work-in-progress (TUI functional, auth backend integration pending)  
> **Last updated:** 2026-06-17  
> **Author:** Bruce Pham
> **Branch:** `rica/cli`

---

## 1. Overview

The RICA CLI is a rebranded fork of the Continue CLI (`extensions/cli/`). It follows the same **overlay pattern** used by the VS Code extension: RICA-specific logic lives in `rica/cli/` and is injected at build time via esbuild module resolution — no mass-patching of upstream files.

**Current state:** The TUI renders and accepts keyboard input. Login flow calls the backend but requires the staging environment (`.staging` dot file in `~/.rica/`). Dynamic config from RICA API is **not yet implemented** for CLI (API not ready).

---

## 2. Architecture

### Shim Strategy (Build-Time Module Replacement)

Instead of patching 50+ files that import `auth/workos.ts`, `env.ts`, etc., the build script (`rica/scripts/build-cli.mjs`) uses an esbuild `onResolve` plugin to redirect imports at bundle time:

```
Upstream import                  →  RICA shim
─────────────────────────────────────────────────────────
auth/workos.js                   →  rica/cli/shim/workosShim.ts
auth/workos-types.js             →  rica/cli/shim/workosShim.ts
./env.js (from cli/src/)         →  rica/cli/shim/envShim.ts
asciiArt.js                      →  rica/cli/shim/asciiArtShim.ts
./onboarding.js                  →  rica/cli/shim/onboardingShim.ts
./configLoader.js                →  rica/cli/shim/configLoaderShim.ts
./version.js                     →  rica/cli/shim/versionShim.ts
./config.js (from cli/src/)      →  rica/cli/shim/configShim.ts
./systemMessage.js               →  rica/cli/shim/systemMessageShim.ts
posthogService.js                →  rica/cli/shim/posthogShim.ts
sqlite3 / better-sqlite3         →  rica/cli/shim/sqliteStub.ts
```

### Upstream Patches (Minimal — 3 files)

| File                                                 | Change                                                                                                                                       | Lines |
| ---------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------- | ----- |
| `extensions/cli/src/index.ts`                        | Import `cliStrings`, use for `.name()`, `.description()`, login/logout descriptions. TUI lifecycle (keepalive, SIGINT handler, `parseAsync`) | ~20   |
| `extensions/cli/src/commands/login.ts`               | Import `ricaLogin` from workos shim, use `cliStrings` for messages                                                                           | ~6    |
| `extensions/cli/src/commands/logout.ts`              | Import `ricaLogout` from workos shim                                                                                                         | ~2    |
| `extensions/cli/src/ui/index.ts`                     | Keepalive timer, exitPromise handling                                                                                                        | ~8    |
| `extensions/cli/src/ui/components/ScreenContent.tsx` | Fix `hideScreenContent` to not block during "checking" state                                                                                 | ~4    |

### File Layout

```
rica/
├── cli/
│   ├── auth/
│   │   ├── index.ts              # API key login/logout, token exchange
│   │   ├── tokenRefresh.ts       # In-memory MDP token cache + auto-refresh
│   │   └── tokenStorage.ts       # ~/.rica/config.json (API key), ~/.rica/auth.json
│   ├── config/
│   │   ├── cliEnv.ts             # Environment config (URLs, home dir, env detection)
│   │   └── cliConfigBridge.ts    # Bridges RicaProfileLoader for CLI config loading
│   ├── branding/
│   │   └── cliStrings.ts         # All user-facing strings (from brand.json)
│   ├── integration/
│   │   └── index.ts              # Barrel export for CLI consumers
│   └── shim/
│       ├── workosShim.ts         # Drop-in for auth/workos.ts (21 exports)
│       ├── envShim.ts            # Drop-in for env.ts (RICA URLs)
│       ├── configShim.ts         # Drop-in for config.ts (rica-proxy adapter)
│       ├── configLoaderShim.ts   # Drop-in for configLoader.ts
│       ├── ricaProxyAdapter.ts   # BaseLlmApi for Databricks serving endpoints
│       ├── versionShim.ts        # RICA version (from brand.json)
│       ├── systemMessageShim.ts  # RICA identity in system message
│       ├── asciiArtShim.ts       # RICA ASCII art
│       ├── onboardingShim.ts     # RICA login-only onboarding
│       ├── posthogShim.ts        # PostHog telemetry disabled (no-op)
│       └── sqliteStub.ts         # SQLite stub (not needed in CLI)
│
├── scripts/
│   ├── build-cli.mjs            # esbuild + shim plugin
│   └── generate-cli-manifest.js # Merges branding into package.json
│
├── branding/
│   ├── brand.json               # Single source of truth (name, version, URLs)
│   └── cli.manifest.overrides.json  # CLI-specific package.json fields
│
└── docs/
    └── CLI_HANDOVER.md          # This document
```

---

## 3. How to Build

### Prerequisites

```bash
cd extensions/cli
npm install          # Only needed once
```

### Build Command

```bash
node rica/scripts/build-cli.mjs            # Minified production build
node rica/scripts/build-cli.mjs --no-minify # Debug build (readable source)
```

### Output

```
extensions/cli/dist/
├── index.js           # Bundled CLI (~14 MB minified, ~28 MB debug)
├── index.js.map       # Sourcemap
├── rica.js            # Wrapper with #!/usr/bin/env node shebang
├── meta.json          # esbuild metafile (for bundle analysis)
└── xhr-sync-worker.js # JSDOM worker (copied from node_modules)
```

### Run

```bash
node extensions/cli/dist/rica.js              # Interactive TUI
node extensions/cli/dist/rica.js -p "hello"   # Headless mode
node extensions/cli/dist/rica.js login        # Authenticate
node extensions/cli/dist/rica.js logout       # Clear credentials
node extensions/cli/dist/rica.js --version    # Show version
```

---

## 4. Authentication Flow

### Login

```
rica login
  → promptForApiKey()        # readline: "Enter your RICA API key: "
  → exchangeApiKeyForToken() # POST {appUrl}/api/extension/token
                             # Headers: { X-API-Key: <api_key> }
                             # Body: { version: "1.2.0" }
  → saveRicaConfig()         # Write ~/.rica/config.json (API key only)
```

### Token Exchange (every session)

```
ensureValidToken()
  → if cachedToken && !expired → return cached
  → else → refreshTokens()
           → loadRicaConfig()        # Read ~/.rica/config.json
           → exchangeApiKeyForToken() # POST /api/extension/token
           → cache in memory          # Never written to disk
```

### LLM Request (via RicaProxyAdapter)

```
getHeaders()
  → ensureValidToken()    # Get/refresh MDP token
  → { Authorization: "Bearer <MDP_TOKEN>" }

POST {apiBase}/serving-endpoints/{modelName}/invocations
  Headers: Authorization: Bearer <MDP_TOKEN>
  Body: OpenAI chat completion format
```

### Security Constraint

**MDP tokens are NEVER persisted to disk.** They exist only in process memory and are exchanged fresh from the API key on each CLI invocation.

---

## 5. Environment Detection

The CLI uses the same dot-file mechanism as the VS Code extension:

| Dot File           | Environment   | APP_URL                               |
| ------------------ | ------------- | ------------------------------------- |
| `~/.rica/.local`   | Local         | `http://localhost:8082`               |
| `~/.rica/.staging` | Staging (UAT) | `https://rica-uat.global.renesas.com` |
| (none)             | Production    | `https://rica.global.renesas.com`     |

Environment variables override dot files:

- `RICA_APP_URL` — Backend URL
- `RICA_API_BASE` — API base (defaults to appUrl)
- `RICA_API_KEY` — API key (bypasses login)

---

## 6. Comparison with VS Code Extension (REFORK_PLAN.md)

| Feature           | VS Code Extension                  | CLI                           | Notes                                         |
| ----------------- | ---------------------------------- | ----------------------------- | --------------------------------------------- |
| Authentication    | EntraID SSO (browser flow)         | API key (paste in terminal)   | Different auth methods, same backend endpoint |
| Token exchange    | EntraID access token → MDP         | API key → MDP                 | Same `/api/extension/token` endpoint          |
| Dynamic config    | ✅ via RicaProfileLoader           | ❌ Not implemented            | RICA API not ready for CLI use                |
| Model serving     | rica-proxy provider (OpenAI class) | RicaProxyAdapter (BaseLlmApi) | Different class, same Databricks endpoints    |
| Branding          | GUI bridge pattern                 | cliStrings + ASCII art        | Both source from brand.json                   |
| Telemetry         | PostHog                            | Disabled (posthogShim)        | CLI does not send telemetry                   |
| Build             | webpack (VS Code)                  | esbuild (shim plugin)         | Different bundlers, same shim concept         |
| Online Documents  | ✅ Full multimodal RAG             | ❌ Not implemented            | CLI is chat-only for now                      |
| Multi-Agent       | ✅ Orchestration system            | ❌ Not implemented            | Future work                                   |
| Token Consumption | ✅ Tracking + threshold            | ❌ Not implemented            | Future work                                   |

---

## 7. TUI Fixes Applied

The upstream Continue CLI TUI had several issues when running under RICA. These fixes are in the upstream-patched files:

| Fix | File                                  | Problem                                                   | Solution                                                 |
| --- | ------------------------------------- | --------------------------------------------------------- | -------------------------------------------------------- |
| 1   | `src/index.ts`                        | `program.parse()` returns before async actions complete   | Changed to `program.parseAsync()`                        |
| 2   | `src/ui/index.ts`                     | Node event loop drains before Ink activates stdin         | Added keepalive timer (`setInterval(() => {}, 60_000)`)  |
| 3   | `src/index.ts`                        | `removeAllListeners("SIGINT")` kills signal-exit listener | Changed to `process.off("SIGINT", defaultSigintHandler)` |
| 4   | `src/ui/components/ScreenContent.tsx` | `hideScreenContent` blocks UI during "checking" state     | Removed "checking" from hide conditions                  |

---

## 8. What's NOT Done (Future Work)

### 8.1 Dynamic Configuration (blocked on RICA API)

The VS Code extension loads models dynamically from `GET /api/user-model/configs`. The CLI has the infrastructure (`configLoaderShim.ts`, `cliConfigBridge.ts`) but currently falls back to a hardcoded default model because:

- The RICA API endpoint is not yet ready for CLI consumption
- The CLI currently uses `~/.rica/config.yaml` if present, otherwise defaults

**When API is ready:** Enable `loadDynamicAssistant()` in `configLoaderShim.ts`.

### 8.2 Online Documents / RAG

Not applicable to CLI v1. The VS Code extension has full multimodal RAG via `@rica-docs` context provider.

### 8.3 Multi-Agent Orchestration

The CLI does not support the multi-agent orchestration system (A11 in REFORK_PLAN.md).

### 8.4 Token Consumption Tracking

Not implemented for CLI. The VS Code extension tracks usage and auto-switches models at thresholds.

### 8.5 Production Deployment

- Package publishing (`@renesas/rica-cli` on internal registry)
- npm global install support
- PATH setup instructions for end users

---

## 9. Known Issues

1. **Login 401 on production:** The `/api/extension/token` endpoint may not accept `X-API-Key` header in production. Works on staging. Backend team needs to confirm the API contract for CLI auth.

2. **No config.yaml template:** First-time users have no `~/.rica/config.yaml`. The CLI should either generate a template on login or document the expected format.

3. **Bundle size:** ~14 MB minified. Could be reduced by tree-shaking unused core/ modules (embeddings, indexing, etc.) that are stubbed but still partially bundled.

---

## 10. Development Workflow

### Adding a New Shim

1. Create `rica/cli/shim/myShim.ts` implementing the same exports as the upstream file
2. Add `onResolve` rule in `rica/scripts/build-cli.mjs`
3. Rebuild: `node rica/scripts/build-cli.mjs --no-minify`
4. Test: `node extensions/cli/dist/rica.js`

### Modifying Auth Flow

- Auth logic: `rica/cli/auth/index.ts`
- Token cache: `rica/cli/auth/tokenRefresh.ts`
- Storage format: `rica/cli/auth/tokenStorage.ts`

### Changing Branding

- Update `rica/branding/brand.json` (single source of truth)
- User-facing CLI strings: `rica/cli/branding/cliStrings.ts`
- Package metadata: `rica/branding/cli.manifest.overrides.json`

### Debugging

```bash
# Build without minification for readable stack traces
node rica/scripts/build-cli.mjs --no-minify

# Run with verbose logging
node extensions/cli/dist/rica.js --verbose

# Check logs
# Windows: %USERPROFILE%\.rica\logs\
# Linux: ~/.rica/logs/
```

---

## 11. Build, Link & Release for End Users

### 11.1 Full Build (from scratch)

```bash
# 1. Install CLI dependencies
cd extensions/cli
npm install

# 2. Generate RICA-branded package.json (optional — for publishing)
node rica/scripts/generate-cli-manifest.js

# 3. Build the bundle
node rica/scripts/build-cli.mjs          # Production (minified, ~14 MB)
```

### 11.2 Link for Local Development (npm link)

Make `rica` available as a global command on your machine:

```bash
cd extensions/cli

# Generate RICA manifest first (sets bin.rica = dist/rica.js)
node ../../rica/scripts/generate-cli-manifest.js

# Build
node ../../rica/scripts/build-cli.mjs

# Link globally — creates symlink so `rica` command works anywhere
npm link
```

After linking:

```bash
rica --version        # Should show version from brand.json
rica login            # Authenticate
rica                  # Start interactive TUI
rica -p "hello"      # Headless mode
```

To unlink:

```bash
cd extensions/cli
npm unlink -g
```

### 11.3 Release / Distribution

#### Option A: Internal npm registry

```bash
cd extensions/cli

# 1. Generate RICA manifest
node ../../rica/scripts/generate-cli-manifest.js

# 2. Build production bundle
node ../../rica/scripts/build-cli.mjs

# 3. Publish to internal registry
npm publish --registry https://your-internal-registry.renesas.com/
```

End users install with:

```bash
npm install -g @renesas/rica-cli --registry https://your-internal-registry.renesas.com/
rica --version
```

#### Option B: Standalone binary (no Node.js required)

Package with `pkg` or `sea` (Node.js Single Executable Application):

```bash
# Using pkg (third-party)
npx pkg extensions/cli/dist/index.js --target node20-win-x64 --output rica.exe

# Or using Node.js SEA (built-in, Node 20+)
# See: https://nodejs.org/api/single-executable-applications.html
```

#### Option C: Direct distribution (zip/tar)

```bash
cd extensions/cli

# Build
node ../../rica/scripts/build-cli.mjs

# Package dist/ folder
tar -czf rica-cli-v1.2.0-win-x64.tar.gz dist/

# Users extract and add to PATH:
#   Windows: set PATH=%PATH%;C:\path\to\rica-cli\dist
#   Linux:   export PATH=$PATH:/path/to/rica-cli/dist
#   Then:    node rica.js  (or create alias: alias rica="node /path/to/dist/rica.js")
```

### 11.4 User Setup (after install)

```bash
# 1. Switch to correct environment (if not production)
#    Create dot file in ~/.rica/ for staging:
mkdir -p ~/.rica && touch ~/.rica/.staging

# 2. Login
rica login
# Enter API key: rc_xxxxx

# 3. Use
rica                  # Interactive chat
rica -p "explain this code"  # One-shot headless
```

### 11.5 Updating

When releasing a new version:

1. Update version in `rica/branding/brand.json`
2. Run `node rica/scripts/generate-cli-manifest.js`
3. Build: `node rica/scripts/build-cli.mjs`
4. Publish or redistribute the new bundle

---

## 12. Relationship to Upstream

The RICA CLI tracks Continue CLI upstream. When upstream updates `extensions/cli/`:

1. **Shim files** — Check if upstream changed the exports of shimmed modules. If so, update the corresponding shim.
2. **Patched files** — Re-apply the 5 minimal patches (see Section 7 + upstream patches table in Section 2).
3. **New features** — Evaluate if they need RICA-specific handling or work as-is through the shim layer.

The esbuild plugin approach means **most upstream changes are automatically incorporated** without conflicts — only changes to the shimmed module interfaces require attention.
