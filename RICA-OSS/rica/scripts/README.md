# RICA Scripts

Build, validation, and packaging scripts for the RICA VS Code extension.

## Scripts

| Script                 | Purpose                                                                                                                                                               |
| ---------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `generate-manifest.js` | Generate `extensions/vscode/package.json` from `brand.json` + `vscode.manifest.overrides.json`. Idempotent — handles both clean upstream and partially-branded input. |
| `check-branding.js`    | Validate no forbidden strings (Continue, continue.dev, etc.) leak into public surfaces. Reads forbidden list from `brand.json`.                                       |
| `rebrand-urls.js`      | Post-build replacement of Continue URLs in compiled bundles (`dist/`) with RICA equivalents.                                                                          |
| `build-vsix.js`        | End-to-end VSIX packaging orchestrator. Flags: `--pre-release`, `--target`, `--skip-validation`.                                                                      |

## Build Pipeline Order

```
1. generate-manifest.js   → Brand package.json
2. npm run build (gui/)   → Build GUI assets
3. tsc --noEmit           → TypeScript check
4. prepackage.js          → Install deps + copy binaries
5. esbuild --minify       → Bundle extension
6. rebrand-urls.js        → Replace URLs in dist
7. check-branding.js      → Validate no leaks
8. vsce package           → Package VSIX
```

## Usage

```bash
# Generate branded manifest (run after upstream sync or brand.json change)
node rica/scripts/generate-manifest.js

# Full VSIX build
node rica/scripts/build-vsix.js --target win32-x64

# Validate branding only
node rica/scripts/check-branding.js
```

## Key Files

- `rica/branding/brand.json` — Single source of truth for all branding values
- `rica/branding/vscode.manifest.overrides.json` — VS Code-specific manifest overrides
- `rica/docs/BUILD_AND_RELEASE.md` — Full build and release documentation
