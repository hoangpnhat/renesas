# RICA Build Scripts - Quick Reference

This directory contains scripts for building and validating RICA VSIX files.

## Quick Start

```bash
# Build VSIX (automated, recommended)
node rica/scripts/build-vsix.js

# Output: extensions/vscode/build/rica-<version>.vsix
```

## Scripts Overview

| Script                 | Purpose                             | When to Use               |
| ---------------------- | ----------------------------------- | ------------------------- |
| `build-vsix.js`        | **Complete end-to-end build**       | Building VSIX for release |
| `generate-manifest.js` | Apply RICA branding to package.json | After changing brand.json |
| `check-branding.js`    | Validate no Continue branding leaks | Before releasing VSIX     |

## Command Reference

### `build-vsix.js` - Automated Build

**Purpose:** Complete build from source to VSIX in one command.

**Usage:**

```bash
# Standard release
node rica/scripts/build-vsix.js

# Pre-release version
node rica/scripts/build-vsix.js --pre-release

# Platform-specific (Windows x64)
node rica/scripts/build-vsix.js --target win32-x64

# Skip branding validation (dev only)
node rica/scripts/build-vsix.js --skip-validation
```

**Steps performed:**

1. Generate manifest with RICA branding
2. Build GUI assets
3. Build extension (esbuild)
4. TypeScript type check
5. Validate branding
6. Package VSIX

**Output:** `extensions/vscode/build/rica-<version>.vsix`

**Time:** ~3-5 minutes

---

### `generate-manifest.js` - Manifest Generation

**Purpose:** Apply RICA branding to `extensions/vscode/package.json`.

**Usage:**

```bash
node rica/scripts/generate-manifest.js
```

**What it does:**

- Reads `rica/branding/brand.json` (single source of truth)
- Applies branding: name, publisher, URLs, icons
- Normalizes namespaces (`continue.*` → `rica.*`)
- Merges `vscode.manifest.overrides.json`
- Writes to `extensions/vscode/package.json`

**When to run:**

- After modifying `rica/branding/brand.json`
- After modifying `rica/branding/vscode.manifest.overrides.json`
- Before building VSIX (automated in `build-vsix.js`)

**Verify:**

```bash
grep '"name"' extensions/vscode/package.json
# Should output: "name": "rica",
```

---

### `check-branding.js` - Branding Validation

**Purpose:** Ensure no Continue branding leaks to users.

**Usage:**

```bash
node rica/scripts/check-branding.js
```

**What it validates:**

- ✓ `package.json` has RICA name, publisher, URLs
- ✓ No forbidden strings in built artifacts
- ✓ URLs point to Renesas domains
- ✓ No "Continue" references in public files

**Exit codes:**

- `0` - Validation passed ✅
- `1` - Validation failed ❌ (build should be blocked)

**When to run:**

- Before releasing VSIX
- In CI/CD pipeline
- After major code changes
- Before committing (automated in `build-vsix.js`)

**Example output (success):**

```
✅ Branding Validation PASSED

   All public surfaces use RICA branding correctly.
   No Continue branding leaks detected.
```

**Example output (failure):**

```
❌ Branding Validation FAILED

Found 1 error(s):

1. extensions/vscode/package.json:23
   Forbidden: "Continue"
   Line: "displayName": "Continue",

💡 Fix these issues before release!
```

---

## Build Options

### Platform Targets

Build for specific OS/architecture:

```bash
# Windows
--target win32-x64        # Windows x64
--target win32-arm64      # Windows ARM64

# macOS
--target darwin-x64       # macOS Intel
--target darwin-arm64     # macOS Apple Silicon

# Linux
--target linux-x64        # Linux x64
--target linux-arm64      # Linux ARM64

# Alpine (Docker)
--target alpine-x64       # Alpine x64
--target alpine-arm64     # Alpine ARM64
```

**Example:**

```bash
node rica/scripts/build-vsix.js --target darwin-arm64
```

### Pre-Release Builds

For beta/testing versions:

```bash
node rica/scripts/build-vsix.js --pre-release
```

Marks extension as "pre-release" in VS Code.

### Skip Validation (Dev Only)

⚠️ **Development only, never ship without validation!**

```bash
node rica/scripts/build-vsix.js --skip-validation
```

Skips branding validation for faster iteration.

---

## Troubleshooting

### Build fails: "Cannot find module 'rica/adapters/core/RicaCore'"

**Cause:** RICA files not committed/tracked by Git.

**Fix:**

```bash
git status rica/
git add rica/
git commit -m "Add RICA adapters"
```

### Build fails: Branding validation errors

**Cause:** `package.json` has Continue branding.

**Fix:**

```bash
node rica/scripts/generate-manifest.js
```

### Build succeeds but extension doesn't load

**Cause:** Extension build incomplete or TypeScript errors.

**Fix:**

```bash
cd extensions/vscode
npm run tsc:check  # Check for errors
npm run esbuild    # Rebuild extension
cd ../..
```

### VSIX file is too large (>100MB)

**Cause:** `node_modules/` or `.git/` included in VSIX.

**Fix:**

Check `.vscodeignore`:

```bash
cat extensions/vscode/.vscodeignore
```

Should include:

```
node_modules/
.git/
src/
tsconfig.json
*.ts
```

---

## CI/CD Integration

### GitLab CI Example

```yaml
build-vsix:
  stage: build
  image: node:20
  script:
    - npm install
    - cd gui && npm install && cd ..
    - cd extensions/vscode && npm install && cd ../..
    - node rica/scripts/build-vsix.js
  artifacts:
    paths:
      - extensions/vscode/build/*.vsix
    expire_in: 30 days
```

### GitHub Actions Example

```yaml
- name: Build RICA VSIX
  run: |
    npm install
    cd gui && npm install && cd ..
    cd extensions/vscode && npm install && cd ../..
    node rica/scripts/build-vsix.js

- name: Upload VSIX
  uses: actions/upload-artifact@v3
  with:
    name: rica-vsix
    path: extensions/vscode/build/*.vsix
```

---

## Related Documentation

- **[RICA_BUILD_GUIDE.md](../../docs/RICA_BUILD_GUIDE.md)** - Complete build guide with troubleshooting
- **[BRANDING_POLICY.md](../branding/BRANDING_POLICY.md)** - Branding rules and forbidden strings
- **[RICA_REPO_STANDARD.md](../../docs/RICA_REPO_STANDARD.md)** - Architecture guidelines

---

## Support

**Issues:** https://gitlab.global.renesas.com/ids_ai/rica-oss/-/issues
**Contact:** IDS AI Team

---

_Quick reference for RICA build scripts. For detailed guide, see [RICA_BUILD_GUIDE.md](../../docs/RICA_BUILD_GUIDE.md)._
