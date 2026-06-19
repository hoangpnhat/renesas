# RICA Overlay

This directory contains RICA-specific customizations overlaid on the Continue base.

## Structure

- `branding/` - Brand identity, assets, and source of truth for RICA product
- `adapters/` - RICA-specific adapters that wrap or extend Continue core
  - `auth/` - EntraID authentication and session management
  - `config/` - Dynamic configuration and environment handling
  - `context/` - Custom context providers (rica-proxy, documents)
  - `llm/` - Custom LLM adapters if needed
- `vscode/` - VS Code extension integration and customizations
  - `commands/` - RICA-specific commands
  - `providers/` - RICA-specific providers
  - `integration/` - Integration glue code
- `cli/` - CLI bootstrap and customizations
- `gui/` - GUI/webview theme and wrapper components
- `scripts/` - Build, validation, and sync scripts
- `tests/` - RICA-specific tests

## Principles

1. **Overlay over fork**: Keep RICA-specific code separate from upstream Continue
2. **Single source of truth**: Use `branding/brand.json` for all product identity
3. **Minimal upstream changes**: Prefer adapters and wrappers over direct modifications
4. **Easy sync**: Monthly upstream sync should be straightforward

## References

- See `docs/RICA_REPO_STANDARD.md` for architecture guidelines
- See `docs/RICA_REFORK_EXECUTION_PLAN.md` for migration plan
- See `docs/RICA_FEATURE_INVENTORY.md` for feature tracking
