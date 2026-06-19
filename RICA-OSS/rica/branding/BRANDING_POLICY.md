# RICA Branding Policy

## Purpose

This policy ensures consistent RICA brand identity across all public-facing surfaces and prevents Continue branding leaks.

## Single Source of Truth

**All branding values MUST come from `rica/branding/brand.json`**

Do not hardcode branding values in:

- Extension manifests
- README files
- Documentation
- UI strings
- Error messages
- Log output

## Brand Identity

### Product Name

- **Public name**: RICA
- **Full name**: Renesas Intelligent Coding Assistant
- **Never use**: Continue, Continue.dev

### Publisher/Author

- **Publisher**: Rica
- **Author**: IDS/AI team Renesas
- **Organization**: Renesas Electronics Corporation

### URLs - Public Surfaces

All public-facing URLs MUST point to RICA domains:

✅ **Allowed:**

- `rica.global.renesas.com` (production)
- `rica-uat.global.renesas.com` (staging)
- `gitlab.global.renesas.com/ids_ai/rica-oss` (repository)

❌ **Forbidden:**

- `continue.dev`
- `github.com/continuedev/continue`
- `api.continue.dev`
- Any Continue-branded URLs

### Command/Config Namespaces

- Commands: `rica.*` (e.g., `rica.acceptDiff`, `rica.focusContinueInput`)
- Config: `rica.*`
- Context keys: `rica.*`

**Exception**: Internal references like `continueGUIState` are acceptable if not user-visible.

## Validation

### Automated Checks

Run before every release:

```bash
node rica/scripts/check-branding.js
```

This scans:

- Extension manifest (`package.json`)
- Built artifacts
- Public documentation
- CLI output (if applicable)

### Manual Review

Before releases, manually verify:

- Extension marketplace listing
- README on GitLab
- Help/About dialogs
- Error messages shown to users

## Exceptions

### Allowed Continue References

These are acceptable as they're not user-visible or are technical:

- `core/continueServer` (internal module name)
- `Continue proxy` (technical term for proxy service)
- `continue-proxy` (provider identifier)
- Code comments: `// Continue with...`
- Markdown lists: `* Continue to...`

### Upstream Attribution

In LICENSE or NOTICE files, proper attribution to Continue project is required:

```
This software is derived from Continue (https://github.com/continuedev/continue)
Licensed under Apache License 2.0
```

## Asset Guidelines

### Icons

- Use RICA branded icons from `rica/branding/assets/source/`
- Do not use Continue logo or colors

### Colors

- Primary: Renesas brand colors (TBD)
- Avoid Continue's brand colors (purple/blue gradient)

### Typography

- Product name: **RICA** (all caps)
- Tagline: "Renesas Intelligent Coding Assistant"

## Enforcement

### Pre-commit Hook (Future)

```bash
npm run check:branding
```

### CI/CD Gate

Branding validation MUST pass in CI before merge to main.

### Release Checklist

- [ ] Branding validation passes
- [ ] No Continue URLs in public surfaces
- [ ] Extension ID is `Rica.rica`
- [ ] Publisher is `Rica`
- [ ] Icon is RICA branded

## Updates

When updating branding:

1. Update `rica/branding/brand.json` ONLY
2. Regenerate manifests/configs from source
3. Run validation
4. Commit with clear description

## Questions

Contact RICA team for branding questions:

- Email: rica-support@renesas.com
- GitLab: rica-oss project issues
