# RICA Repository Standard

This document is the canonical repository standard for RICA after consolidation into `rica/docs/`.

It combines the stable, long-lived guidance previously spread across repository-level docs, overlay READMEs, branding policy notes, and adapter overviews.

---

## 1. Purpose

This standard defines:

- the official repository architecture
- ownership boundaries between upstream Continue and RICA
- where RICA-specific logic belongs
- branding governance
- sync-friendly development rules
- the documentation model for RICA

This document should be treated as the source of truth for:

- RICA maintainers
- reviewers
- upstream sync owners
- release/build owners
- developers adding new RICA-specific features

---

## 2. Official architecture statement

RICA is not maintained as a deep traditional fork.

The official model is:

> **Continue upstream = platform base**  
> **RICA = product overlay**

That means:

- upstream Continue remains the primary source for generic platform capabilities
- RICA-specific logic should be implemented as overlay code whenever possible
- direct upstream modifications must be minimized, documented, and justified
- monthly or periodic upstream sync must be operationally repeatable, not heroic

---

## 3. Repository layer model

```text
+--------------------------------------------------+
| RICA Product Overlay                             |
| rica/ branding / auth / config / adapters        |
| product-specific wrappers / docs / scripts       |
+--------------------------------------------------+
| Extension Integration Layer                      |
| extensions/vscode / gui / extension glue         |
+--------------------------------------------------+
| Continue Platform Layer                          |
| core/ packages/ binary/                          |
+--------------------------------------------------+
| Tooling & Delivery Layer                         |
| scripts/ .github/ docs / CI / release flow       |
+--------------------------------------------------+
```

### 3.1 Continue Platform Layer

Includes:

- `core/`
- `packages/`
- generic parts of `binary/`

Role:

- engine, protocol, model logic, indexing, utilities, shared platform behaviors

Rule:

- prefer upstream intake over local divergence

### 3.2 Extension Integration Layer

Includes:

- `extensions/vscode/`
- extension-facing GUI integration in `gui/`
- other extension-specific surfaces if added later

Role:

- wire upstream platform to user-facing product behavior

Rule:

- this area is conflict-prone and requires explicit patch strategy

### 3.3 RICA Product Overlay

Includes:

- `rica/adapters/`
- `rica/branding/`
- `rica/vscode/`
- `rica/scripts/`
- `rica/docs/`

Role:

- product identity
- enterprise auth and configuration
- wrappers, adapters, and custom integration logic
- RICA-specific operational documentation

Rule:

- new custom behavior should default here unless there is a compelling reason otherwise

### 3.4 Tooling and Delivery Layer

Includes:

- root `scripts/`
- CI configuration
- packaging and release automation
- generated artifact support

Role:

- build, validate, release, and maintain the product

---

## 4. Ownership model

Every file should be thought of as belonging to one of these groups.

### 4.1 `upstream-managed`

Typical paths:

- `core/**`
- `packages/**`
- generic tooling shared with upstream

Rules:

- prefer upstream versions
- avoid RICA-specific logic here
- if changed, mark the file as a conflict hotspot in sync docs

### 4.2 `modified-upstream`

Typical paths:

- `extensions/vscode/**`
- selected GUI integration files
- generated manifest outputs

Rules:

- upstream is still the base
- RICA patches may be re-applied after sync
- every recurring hotspot should have a documented conflict scenario

### 4.3 `rica-only`

Typical paths:

- `rica/**`
- RICA branding assets and scripts
- RICA docs under `rica/docs/`

Rules:

- upstream must not overwrite these files
- this is the preferred home for RICA-specific business logic and policies

---

## 5. Standard repository structure

Target structure:

```text
/
├─ core/
├─ packages/
├─ gui/
├─ binary/
├─ extensions/
│  └─ vscode/
├─ rica/
│  ├─ adapters/
│  │  ├─ auth/
│  │  ├─ config/
│  │  ├─ control-plane/
│  │  └─ core/
│  ├─ branding/
│  │  ├─ brand.json
│  │  ├─ vscode.manifest.overrides.json
│  │  └─ assets/
│  ├─ scripts/
│  ├─ vscode/
│  │  └─ integration/
│  └─ docs/
└─ docs/    (upstream or external-doc tooling only, not canonical RICA product docs)
```

### Structural rules

1. `rica/docs/` is the canonical location for RICA documentation.
2. `rica/branding/brand.json` is the branding source of truth.
3. `extensions/vscode/package.json` should be treated as generated or normalized output, not a long-term hand-edited truth source.
4. Repeated operational knowledge should live in docs and scripts, not only in individual heads.

---

## 6. Branding governance

### 6.1 Source of truth

Branding source of truth lives in:

- `rica/branding/brand.json`

It should define at least:

- product name
- short name
- extension identifier
- publisher
- command/config namespaces
- homepage and issue URLs
- forbidden strings and branding checks

### 6.2 Public surfaces that must present RICA branding

- extension display name
- publisher
- user-facing command titles
- user-facing categories
- public settings descriptions where policy requires RICA wording
- activity bar and icon assets
- public docs and release artifacts

### 6.3 Branding enforcement rules

- avoid hardcoded branding across multiple files when it can be generated
- user-facing strings should not regress from `RICA` back to `Continue`
- internal compatibility IDs may still retain upstream forms where technically required, but public UX should remain branded as RICA

### 6.4 Manifest generation rule

For the VS Code extension:

- take upstream-compatible manifest base
- apply RICA branding overrides from `brand.json` and related override data
- validate output with branding checks before packaging

---

## 7. Adapter and integration standards

### 7.1 Auth

RICA-specific auth belongs in:

- `rica/adapters/auth/`

Expected scope:

- EntraID auth flow
- token exchange and refresh
- secure storage integration
- environment-specific auth configuration

Rule:

- auth-specific types should live in RICA space whenever possible
- if upstream protocol pressure forces temporary core patches, document them as debt and add sync playbooks

### 7.2 Config

RICA-specific config belongs in:

- `rica/adapters/config/`

Expected scope:

- environment detection
- backend URL selection
- config mapping and refresh policy
- extension identity and environment bridging

### 7.3 Control plane

RICA-specific control plane adaptation belongs in:

- `rica/adapters/control-plane/`
- `rica/adapters/core/`

Rule:

- prefer wrapper/injection patterns over direct modifications in `core/`
- document all known injection assumptions and sync implications

### 7.4 Extension integration

VS Code-specific RICA integration belongs in:

- `rica/vscode/integration/`

Rule:

- keep the patch surface in `extensions/vscode/` as small as possible
- every recurring modified-upstream file must be covered in upstream sync conflict guidance

---

## 8. Documentation standard

### 8.1 Canonical location

All maintained RICA docs should live in:

- `rica/docs/`

### 8.2 Document categories

RICA docs should be split by purpose:

- **Standard**: long-lived policies and architecture
- **Plan**: time-bound strategy and execution sequencing
- **Conflict guide**: scenario-based operational handling for sync and merge conflicts
- **Build/release**: packaging, validation, CI, troubleshooting

### 8.3 Documentation anti-patterns

Avoid:

- multiple separate executive summaries that repeat the same content
- README files that say only “to be created” after implementation already exists
- having both shallow and deep versions of the same operational guide without clear ownership

Rule:

- if two docs overlap, the deeper actionable document should absorb the shallower one

---

## 9. Development rules for new RICA work

1. Prefer `rica/**` over `core/**`
2. Prefer wrappers/adapters over direct upstream modification
3. If modifying `extensions/vscode/**`, document the patch and expected conflict scenarios
4. If modifying `core/**`, justify why the overlay layer was insufficient
5. Add or update build/branding validation if the change affects public surfaces
6. Add or update sync conflict guidance for any new hotspot path
7. Keep docs synchronized with the real implementation state

---

## 10. Sync-friendly design rules

When implementing a new feature, ask:

1. Can this live entirely in `rica/**`?
2. If not, what is the smallest modified-upstream surface?
3. Can the change be regenerated instead of hand-maintained?
4. What file will likely conflict on the next upstream sync?
5. What scenario-based playbook should be written now so future maintainers do not rediscover it?

---

## 11. Quality gates

A change touching RICA platform integration is not done until:

- architecture placement is acceptable
- branding remains valid
- docs are updated if new hotspot logic was introduced
- build and validation still pass
- sync implications are understood for modified-upstream files

---

## 12. Practical summary

### Keep in `rica/**`

- product branding
- auth
- config
- wrappers
- conflict knowledge
- product docs

### Keep minimal in modified upstream

- extension wiring
- generated manifest output
- unavoidable integration shims

### Avoid in `core/**`

- product identity
- business-specific behavior
- long-term RICA custom types if adapter-level types can work

---

## 13. Related canonical documents

- [`README.md`](./README.md)
- [`REFORK_PLAN.md`](./REFORK_PLAN.md)
- [`UPSTREAM_SYNC_CONFLICTS.md`](./UPSTREAM_SYNC_CONFLICTS.md)
- [`BUILD_AND_RELEASE.md`](./BUILD_AND_RELEASE.md)
