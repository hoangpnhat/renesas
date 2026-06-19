# Build and Release

> **Status:** canonical &nbsp;·&nbsp; **Last updated:** 2026-05-21
>
> Canonical build & release guide for RICA. This document **replaces and absorbs**:
>
> - `docs/RICA_BUILD_GUIDE.md`
> - `rica/scripts/README_BUILD.md`
> - `rica/scripts/README.md`
> - build- and release-relevant portions of `rica/branding/BRANDING_POLICY.md`
>
> Long-form branding rules (allowed/forbidden strings, asset guidelines, attribution) live in [`REPO_STANDARD.md`](./REPO_STANDARD.md) §6. This document keeps only the parts that gate the build and release pipeline.

---

## 1. Overview

RICA ships as a VS Code extension VSIX produced from this repository. The build:

1. Generates the VS Code manifest from a single branding source (`rica/branding/brand.json`).
2. Builds the GUI (React + Vite) and the extension (esbuild + TypeScript).
3. Validates branding (no Continue leaks).
4. Packages a `.vsix` using `@vscode/vsce`.

The whole pipeline is wrapped by `rica/scripts/build-vsix.js`. Every manual step is reproducible and idempotent.

**Build output:** `extensions/vscode/build/rica-<version>.vsix`
**Typical time:** ~3–5 minutes cold, ~1–2 minutes warm/CI.

---

## 2. Prerequisites

### 2.1 Required tools

| Tool       | Version  | Purpose                          |
| ---------- | -------- | -------------------------------- |
| Node.js    | ≥20.20.1 | Runtime for build scripts        |
| npm        | ≥10.x    | Package manager                  |
| Git        | latest   | Version control                  |
| VS Code    | ≥1.70.0  | Testing the extension            |
| TypeScript | ^5.6.3   | Type checking (installed by npm) |

### 2.2 First-time setup

```bash
# 1. Clone repository
git clone https://gitlab.global.renesas.com/ids_ai/rica-oss.git
cd rica-oss

# 2. Install dependencies (root level)
npm install

# 3. Install GUI dependencies
cd gui && npm install && cd ..

# 4. Install VS Code extension dependencies
cd extensions/vscode && npm install && cd ../..
```

---

## 3. Quick start (automated build)

The fastest path is the automated build script:

```bash
# Standard release
node rica/scripts/build-vsix.js

# Pre-release build
node rica/scripts/build-vsix.js --pre-release

# Platform-specific build (e.g. Windows x64)
node rica/scripts/build-vsix.js --target win32-x64

# Skip branding validation (development only)
node rica/scripts/build-vsix.js --skip-validation
```

What it does:

1. ✅ Generate `extensions/vscode/package.json` with RICA branding
2. ✅ Build GUI assets (`gui/dist/`)
3. ✅ Build extension (`extensions/vscode/out/`)
4. ✅ Run TypeScript type check
5. ✅ Validate branding (no Continue leaks)
6. ✅ Package the VSIX

**Output:** `extensions/vscode/build/rica-<version>.vsix`

### 3.1 TL;DR — 3 commands to build VSIX

```bash
# 1. Apply branding vào package.json
node rica/scripts/generate-manifest.js

# 2. Build GUI
cd gui && npm run build && cd ..

# 3. Package VSIX (tự động chạy: prepackage → esbuild → rebrand-urls → vsce)
cd extensions/vscode && npm run package && cd ../..
```

After packaging, restore upstream state:

```bash
git checkout -- extensions/vscode/package.json extensions/vscode/package-lock.json
```

> **Note:** Step 3 internally triggers: `prepackage` (copy GUI assets) → `vscode:prepublish` (`esbuild --minify` → `rebrand-urls.js`) → `vsce package`. No manual intervention needed between steps.

---

## 4. Manual build steps

Use these steps when debugging the pipeline or preparing CI snippets.

### 4.1 Generate manifest

Apply RICA branding to `extensions/vscode/package.json`:

```bash
node rica/scripts/generate-manifest.js
```

What it does:

- Reads `rica/branding/brand.json` (single source of truth).
- Applies branding (name, publisher, URLs, icons).
- Normalizes namespaces: `continue.*` → `rica.*`.
- Merges `rica/branding/vscode.manifest.overrides.json`.
- Writes the result to `extensions/vscode/package.json`.

Verify:

```bash
grep '"name"' extensions/vscode/package.json
# Expected: "name": "rica",
```

### 4.2 Build GUI

```bash
cd gui
npm run build
cd ..
```

- Compiles TypeScript + React.
- Bundles assets with Vite.
- Output: `gui/dist/` (must contain `index.html` and `assets/`).

### 4.3 Build extension

```bash
cd extensions/vscode
npm run esbuild
cd ../..
```

- Runs `scripts/esbuild.js`.
- Compiles + bundles to `extensions/vscode/out/extension.js` (~5–10 MB).

### 4.4 TypeScript check

```bash
cd extensions/vscode
npm run tsc:check
cd ../..
```

`tsc --noEmit`. No output means success.

### 4.5 Branding validation

```bash
node rica/scripts/check-branding.js
```

Expected output on success:

```
✅ Branding Validation PASSED

   All public surfaces use RICA branding correctly.
   No Continue branding leaks detected.
```

Exit codes:

- `0` — validation passed (build proceeds).
- `1` — validation failed (build must be blocked).

### 4.6 Package VSIX

```bash
cd extensions/vscode

# Standard release
npm run package

# Pre-release
npm run package:pre-release

# Platform-specific
npm run package -- --target win32-x64

cd ../..
```

Verify:

```bash
ls -lh extensions/vscode/build/*.vsix
# Expected: rica-<version>.vsix
```

---

## 5. Build options

### 5.1 Pre-release builds (`--pre-release`)

```bash
node rica/scripts/build-vsix.js --pre-release
```

- Marks the extension as “pre-release” in VS Code.
- Used for internal testing before a stable release.

### 5.2 Platform-specific builds (`--target`)

Eight supported platforms:

```bash
# Windows
--target win32-x64
--target win32-arm64

# macOS
--target darwin-x64
--target darwin-arm64

# Linux
--target linux-x64
--target linux-arm64

# Alpine (for Docker)
--target alpine-x64
--target alpine-arm64
```

When to use:

- Distributing to specific platforms only.
- Reducing VSIX size (excludes binaries for other platforms).
- Native modules require platform-specific builds.

### 5.3 Skip branding validation (`--skip-validation`)

```bash
node rica/scripts/build-vsix.js --skip-validation
```

> ⚠️ Development only. **Never** ship a VSIX without branding validation.

### 5.4 Build all platforms

```bash
cd extensions/vscode
npm run package-all
cd ../..
```

Produces multiple VSIX files, e.g.:

- `rica-1.3.39-win32-x64.vsix`
- `rica-1.3.39-darwin-arm64.vsix`
- …

---

## 6. Branding validation gates

Branding validation is the release gate. Build pipelines must fail if it does not pass.

### 6.1 Source of truth

- `rica/branding/brand.json` — single source of truth.
- `rica/branding/vscode.manifest.overrides.json` — manifest-only overrides.

Do **not** hand-edit `extensions/vscode/package.json`. Treat it as a generated file.

### 6.2 Allowed URLs (public surfaces)

| Status       | URL pattern                                        |
| ------------ | -------------------------------------------------- |
| ✅ Allowed   | `rica.global.renesas.com` (production)             |
| ✅ Allowed   | `rica-uat.global.renesas.com` (staging)            |
| ✅ Allowed   | `gitlab.global.renesas.com/ids_ai/rica-oss`        |
| ❌ Forbidden | `continue.dev`                                     |
| ❌ Forbidden | `github.com/continuedev/continue`                  |
| ❌ Forbidden | `api.continue.dev`                                 |
| ❌ Forbidden | Any other Continue-branded URL on a public surface |

### 6.3 Namespace rules

- Commands, settings, context keys must use `rica.*`.
- Internal compatibility identifiers (e.g. `continueGUIState`) may keep the upstream form when not user-visible. See [`REPO_STANDARD.md`](./REPO_STANDARD.md) §6.3.

### 6.4 What `check-branding.js` scans

- `extensions/vscode/package.json`
- Built artifacts (`gui/dist/`, `extensions/vscode/out/`)
- Public documentation
- CLI output where applicable

A non-zero exit is a hard build failure.

---

## 7. Build pipeline architecture

### 7.1 Pipeline diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    RICA Build Pipeline                      │
└─────────────────────────────────────────────────────────────┘

1. Manifest Generation
   ├─ Input:  rica/branding/brand.json
   ├─ Input:  rica/branding/vscode.manifest.overrides.json
   ├─ Input:  extensions/vscode/package.json (upstream base)
   └─ Output: extensions/vscode/package.json (generated, NOT committed)

2. GUI Build
   ├─ Input:  gui/src/**/*.tsx
   ├─ Tool:   Vite + React + TypeScript
   └─ Output: gui/dist/

3. Extension Build
   ├─ Input:  extensions/vscode/src/**/*.ts
   ├─ Input:  rica/adapters/**/*.ts
   ├─ Tool:   esbuild + TypeScript
   └─ Output: extensions/vscode/out/extension.js

4. URL Rebranding (post-build)
   ├─ Input:  extensions/vscode/gui/assets/index.js
   ├─ Input:  extensions/vscode/out/extension.js
   ├─ Tool:   rica/scripts/rebrand-urls.js
   └─ Output: same files with Continue URLs → RICA URLs

5. Branding Validation
   ├─ Input:  package.json, gui/dist/, out/
   ├─ Tool:   rica/scripts/check-branding.js
   └─ Output: pass/fail (exit code)

6. VSIX Packaging
   ├─ Input:  package.json, out/, media/, gui/dist/
   ├─ Tool:   @vscode/vsce package
   └─ Output: build/rica-<version>.vsix

7. Restore (local builds only)
   ├─ Tool:   git checkout -- extensions/vscode/package.json
   └─ Purpose: keep Git state clean (upstream package.json)
```

### 7.2 File structure of build artifacts

```
rica-oss/
├── rica/
│   ├── branding/
│   │   ├── brand.json                 # 🔑 Single source of truth
│   │   ├── vscode.manifest.overrides.json
│   │   └── assets/source/             # Icons, logos
│   ├── scripts/
│   │   ├── generate-manifest.js       # Step 1: manifest branding
│   │   ├── rebrand-urls.js            # Step 4: post-build URL replacement
│   │   ├── check-branding.js          # Step 5: validation gate
│   │   └── build-vsix.js              # Automated wrapper (all steps)
│   └── adapters/                      # RICA-specific code (consumed by step 3)
├── gui/
│   ├── src/
│   └── dist/                          # Step 2 output
├── extensions/vscode/
│   ├── src/                           # Extension source
│   ├── out/                           # Step 3 output
│   ├── media/                         # Icons (copied from rica/branding)
│   ├── build/                         # Step 6 output
│   ├── package.json                   # Generated, do not hand-edit
│   └── scripts/
│       ├── esbuild.js
│       └── package.js
└── rica/docs/
    └── BUILD_AND_RELEASE.md           # This file
```

### 7.3 Branding flow

```
rica/branding/brand.json (source of truth)
        ↓
rica/scripts/generate-manifest.js (apply branding to package.json)
        ↓
extensions/vscode/package.json (generated — DO NOT commit)
        ↓
esbuild --minify (bundle extension.js)
        ↓
rica/scripts/rebrand-urls.js (replace Continue URLs in JS bundles)
        ↓
@vscode/vsce package (embed in VSIX)
        ↓
rica/scripts/check-branding.js (validate)
        ↓
git checkout -- extensions/vscode/package.json (restore upstream state)
```

### 7.4 Key principles

1. **Single source of truth** — `rica/branding/brand.json`. Never edit `package.json` manually.
2. **Automated branding** — `generate-manifest.js` is the only correct way to apply branding.
3. **Validation gate** — `check-branding.js` blocks builds with violations.
4. **Reproducible builds** — `build-vsix.js` performs the same steps every time.
5. **Never commit branded `package.json`** — keep it as upstream original in Git (see §7.5).

### 7.5 package.json — DO NOT COMMIT after branding

> **Rule:** `extensions/vscode/package.json` MUST remain in its **upstream original state** in Git.
> Branding is applied at build time only — never committed.

**Why:**

- Committing a branded `package.json` causes **merge conflicts on every upstream sync** (upstream changes the same file frequently).
- The generate-manifest script is idempotent — running it on the upstream file always produces the correct RICA output.
- Keeping upstream state means `git diff` clearly shows only RICA-specific source changes, not generated output.

**Build-time flow:**

```
[Git state: upstream package.json]
        ↓
generate-manifest.js → [branded package.json in working tree]
        ↓
prepackage → esbuild → rebrand-urls.js → vsce package
        ↓
[rica-<version>.vsix produced]
        ↓
git checkout -- extensions/vscode/package.json   ← RESTORE
```

**After building, always restore:**

```bash
# Restore package.json to upstream state after build
git checkout -- extensions/vscode/package.json
```

Or if using the `package-lock.json` that also got modified:

```bash
git checkout -- extensions/vscode/package.json extensions/vscode/package-lock.json
```

**CI/CD note:** In CI pipelines, this is not an issue because the pipeline starts from a clean checkout. But for local builds, developers MUST restore after building.

**What to do if you accidentally committed it:**

```bash
# Find the upstream base commit for package.json
git log --oneline --follow -- extensions/vscode/package.json | head -5

# Restore from the upstream commit (e.g. d971363a7)
git checkout <upstream-commit> -- extensions/vscode/package.json
git add extensions/vscode/package.json
git commit -m "fix: restore package.json to upstream state (branding applied at build time)"
```

### 7.6 Post-build URL rebranding (`rebrand-urls.js`)

After esbuild bundles the extension, Continue URLs remain in the compiled JS output.
`rica/scripts/rebrand-urls.js` replaces all Continue URLs with RICA equivalents in the built bundles.

**How it works:**

- Scans `extensions/vscode/gui/assets/index.js` (GUI bundle) and `extensions/vscode/out/extension.js` (extension bundle).
- Replaces `docs.continue.dev/*`, `continue.dev/*`, `github.com/continuedev/*` with RICA equivalents.
- Reads target URLs from `rica/branding/brand.json`.
- Runs validation after replacement to confirm zero Continue URLs remain.

**Integration:** Already chained in `vscode:prepublish`:

```json
"vscode:prepublish": "npm run esbuild-base -- --minify && node ../../rica/scripts/rebrand-urls.js"
```

This means `npm run package` (and `vsce package`) automatically runs rebrand-urls after esbuild — no manual step needed.

**Manual usage:**

```bash
# Preview what would change (no files modified)
node rica/scripts/rebrand-urls.js --dry-run --verbose

# Apply changes
node rica/scripts/rebrand-urls.js

# Apply with detailed output
node rica/scripts/rebrand-urls.js --verbose
```

---

## 8. Build scripts reference

### 8.1 `rica/scripts/build-vsix.js`

End-to-end build orchestrator.

| Flag                | Effect                                       |
| ------------------- | -------------------------------------------- |
| `--pre-release`     | Mark extension as pre-release.               |
| `--target <plat>`   | Build for one of the 8 platforms (see §5.2). |
| `--skip-validation` | Skip branding validation (development only). |

Steps performed (1–6 above) and exit conditions: any failed step aborts the build with a non-zero exit code.

### 8.2 `rica/scripts/generate-manifest.js`

Applies branding to `extensions/vscode/package.json`.

Run when:

- `rica/branding/brand.json` changes.
- `rica/branding/vscode.manifest.overrides.json` changes.
- Before building VSIX (automated by `build-vsix.js`).

Verify:

```bash
grep '"name"' extensions/vscode/package.json
# Expected: "name": "rica",
```

### 8.3 `rica/scripts/rebrand-urls.js`

Post-build URL replacement. Replaces all Continue URLs in compiled JS bundles with RICA equivalents.

| Flag        | Effect                                   |
| ----------- | ---------------------------------------- |
| `--dry-run` | Preview changes without modifying files. |
| `--verbose` | Show each URL replacement with count.    |

Run when:

- Automatically: triggered by `vscode:prepublish` after esbuild.
- Manually: when debugging URL branding issues in built output.

Target files:

- `extensions/vscode/gui/assets/index.js` — GUI bundle (user-facing buttons/links)
- `extensions/vscode/out/extension.js` — Extension bundle (error messages, tooltips)

URL mapping (configured in script, reads targets from `brand.json`):

| Source pattern                                | Replacement                        |
| --------------------------------------------- | ---------------------------------- |
| `docs.continue.dev/*`                         | `rica.global.renesas.com/docs/*`   |
| `github.com/continuedev/continue/issues`      | GitLab RICA issues URL             |
| `github.com/continuedev/continue/discussions` | RICA support URL                   |
| `continue.dev/settings`                       | `rica.global.renesas.com/settings` |
| `continue.dev/*` (catch-all)                  | `rica.global.renesas.com/*`        |

### 8.4 `rica/scripts/check-branding.js`

Branding validator. Scans manifest + built artifacts + public docs.

Exit codes:

- `0` — pass.
- `1` — fail (build blocked).

Sample failure:

```
❌ Branding Validation FAILED

Found 1 error(s):

1. extensions/vscode/package.json:23
   Forbidden: "Continue"
   Line: "displayName": "Continue",

💡 Fix these issues before release!
```

---

## 9. Troubleshooting

### 9.1 `npm run esbuild` fails — `Cannot find module 'rica/adapters/core/RicaCore'`

Cause: RICA files not committed/tracked by Git.

Fix:

```bash
git status rica/
git add rica/
cd extensions/vscode && npm run tsc && cd ../..
```

### 9.2 Branding validation fails

Cause: `package.json` still contains Continue branding.

Fix:

```bash
node rica/scripts/generate-manifest.js
grep '"displayName"' extensions/vscode/package.json
# Expected: "displayName": "RICA",
```

### 9.3 GUI assets not found (`Could not find gui/dist/index.html`)

Fix:

```bash
cd gui
npm install   # only if first time
npm run build
cd ..
ls gui/dist/
```

### 9.4 `vsce package` authentication / publisher error

Cause: missing publisher in manifest.

Fix:

```bash
grep '"publisher"' extensions/vscode/package.json
# Expected: "publisher": "Rica",
node rica/scripts/generate-manifest.js
```

### 9.5 Large VSIX (>100 MB)

Cause: `node_modules/` or `.git/` accidentally bundled.

Fix — verify `.vscodeignore`:

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

Inspect VSIX content:

```bash
cd extensions/vscode/build
unzip -l rica-1.3.39.vsix | head -50
```

### 9.6 TypeScript errors (TS2307)

```
error TS2307: Cannot find module '../../../../rica/adapters/core/RicaCore'
```

Fix:

```bash
ls -la rica/adapters/core/      # confirm files exist
git status rica/                # commit untracked rica/ files
cd extensions/vscode
rm -rf out/
npm run esbuild
cd ../..
```

### 9.7 Extension does not load in VS Code after install

Steps:

1. Open the Extension Host log: `View → Output → "Extension Host"`.
2. Look for messages like `Cannot find module 'rica/adapters/auth/EntraIDAuthProvider'`.
3. Verify the build artifact size:
   ```bash
   ls -lh extensions/vscode/out/extension.js
   # Expected: ~5–10 MB
   ```
4. Reinstall with `--force`:
   ```bash
   code --install-extension extensions/vscode/build/rica-1.3.39.vsix --force
   ```

---

## 10. CI integration

### 10.1 GitLab CI

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

### 10.2 GitHub Actions

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

### 10.3 Required environment / secrets

The build itself does not need secrets. Publishing (out of scope for this document) requires a VS Code Marketplace PAT or an internal artifact-server token, depending on distribution channel.

---

## 11. Release checklist

Before tagging or uploading a VSIX:

- [ ] `node rica/scripts/check-branding.js` passes.
- [ ] No Continue URLs appear on any public surface.
- [ ] `extensions/vscode/package.json` shows `"name": "rica"` and `"publisher": "Rica"`.
- [ ] Extension ID resolves to `Rica.rica`.
- [ ] Activity bar icon is the RICA-branded icon.
- [ ] VSIX installs cleanly: `code --install-extension extensions/vscode/build/rica-*.vsix`.
- [ ] Smoke test: EntraID login works, `rica.*` commands appear in Command Palette, no console errors.
- [ ] Version bumped according to release strategy (stable vs pre-release).
- [ ] Release notes updated.

---

## 12. Quick reference

### 12.1 Common commands

```bash
# Automated build (recommended)
node rica/scripts/build-vsix.js

# Manual build (each step)
node rica/scripts/generate-manifest.js                   # 1. manifest
cd gui && npm run build && cd ..                         # 2. GUI
cd extensions/vscode && npm run esbuild && cd ../..      # 3. extension
cd extensions/vscode && npm run tsc:check && cd ../..    # 4. typecheck
node rica/scripts/check-branding.js                      # 5. validate
cd extensions/vscode && npm run package && cd ../..      # 6. package

# Install built extension
code --install-extension extensions/vscode/build/rica-*.vsix

# Develop / debug
code .   # then F5 for Extension Development Host
```

### 12.2 Build artifacts

| File                                            | Size   | Description                  |
| ----------------------------------------------- | ------ | ---------------------------- |
| `gui/dist/index.html`                           | ~5 KB  | GUI entry point              |
| `gui/dist/assets/`                              | ~2 MB  | GUI bundle                   |
| `extensions/vscode/out/extension.js`            | ~8 MB  | Extension entry point        |
| `extensions/vscode/out/extension.js.map`        | ~10 MB | Source map (dev builds only) |
| `extensions/vscode/build/rica-<version>.vsix`   | ~15 MB | **Final output (release)**   |
| `extensions/vscode/build/rica-<version>-*.vsix` | ~15 MB | Platform-specific build      |

### 12.3 Build performance

| Step                | Time     | Cacheable |
| ------------------- | -------- | --------- |
| Manifest generation | ~1 s     | No        |
| GUI build           | 30–60 s  | Yes       |
| Extension build     | 30–60 s  | Yes       |
| TypeScript check    | 10–20 s  | No        |
| Branding validation | 2–5 s    | No        |
| VSIX packaging      | 5–10 s   | No        |
| **Total (cold)**    | ~3–5 min | —         |
| **Total (warm/CI)** | ~1–2 min | —         |

---

## 13. Related canonical documents

- [`README.md`](./README.md) — documentation index
- [`REPO_STANDARD.md`](./REPO_STANDARD.md) — architecture, ownership, branding policy
- [`REFORK_PLAN.md`](./REFORK_PLAN.md) — refork strategy and phases
- [`UPSTREAM_SYNC_CONFLICTS.md`](./UPSTREAM_SYNC_CONFLICTS.md) — sync conflict playbooks (esp. `package.json` regeneration scenario)
