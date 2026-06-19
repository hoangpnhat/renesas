# RICA Branding

This directory contains the source of truth for all RICA branding and product identity.

## Files (to be created in Phase 3)

- `brand.json` - Single source of truth for all branding values
- `BRANDING_POLICY.md` - Guidelines for maintaining brand consistency
- `assets/source/` - Original brand assets (logos, icons, etc.)

## Branding Elements

- Product name: `RICA`
- Publisher: `Rica`
- Extension ID: `Rica.rica`
- Command namespace: `rica.*`
- Config namespace: `rica.*`
- Support URLs: RICA-specific, not Continue

## Validation

All public-facing branding will be validated via:

- `rica/scripts/check-branding.js`

This ensures:

- No Continue branding leaks to public surfaces
- Consistent use of RICA identity
- Proper attribution in built artifacts
