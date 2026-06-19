# RICA Documentation

> **Status:** Canonical location for all RICA-specific documentation
> **Last updated:** 2026-05-21

This directory is the **single source of truth** for RICA product documentation.

---

## Documentation Structure

### Core Documents

| Document                                                                 | Purpose                                                     | Status      |
| ------------------------------------------------------------------------ | ----------------------------------------------------------- | ----------- |
| [`README.md`](./README.md)                                               | Index and navigation                                        | ✅ Current  |
| [`REPO_STANDARD.md`](./REPO_STANDARD.md)                                 | Architecture, ownership model, development rules            | ✅ Complete |
| [`REFORK_PLAN.md`](./REFORK_PLAN.md)                                     | Refork strategy, commit classification, execution procedure | ✅ Complete |
| [`UPSTREAM_SYNC_CONFLICTS.md`](./UPSTREAM_SYNC_CONFLICTS.md)             | Conflict resolution playbooks, sync workflow                | ✅ Complete |
| [`BUILD_AND_RELEASE.md`](./BUILD_AND_RELEASE.md)                         | Build pipeline, VSIX packaging, troubleshooting             | ✅ Complete |
| [`MAINTENANCE_AND_SYNC_HANDOVER.md`](./MAINTENANCE_AND_SYNC_HANDOVER.md) | Maintenance handover, upstream sync guide, troubleshooting  | ✅ Complete |

---

## Quick Navigation

### For Developers

- **New feature?** → [`REPO_STANDARD.md`](./REPO_STANDARD.md) §9 (Development Rules)
- **Build VSIX?** → [`BUILD_AND_RELEASE.md`](./BUILD_AND_RELEASE.md) §3 (Quick Start)
- **Branding rules?** → [`REPO_STANDARD.md`](./REPO_STANDARD.md) §6 (Branding Governance)

### For Maintainers

- **New to the project?** → [`MAINTENANCE_AND_SYNC_HANDOVER.md`](./MAINTENANCE_AND_SYNC_HANDOVER.md) (start here)
- **Monthly sync?** → [`UPSTREAM_SYNC_CONFLICTS.md`](./UPSTREAM_SYNC_CONFLICTS.md) §6 (Workflow)
- **Conflict resolution?** → [`UPSTREAM_SYNC_CONFLICTS.md`](./UPSTREAM_SYNC_CONFLICTS.md) §3 (Playbooks)
- **Refork needed?** → [`REFORK_PLAN.md`](./REFORK_PLAN.md) §4 (Execution Procedure)

### For Reviewers

- **Architecture check?** → [`REPO_STANDARD.md`](./REPO_STANDARD.md) §3-5 (Layers & Ownership)
- **Compliance score?** → [`REFORK_PLAN.md`](./REFORK_PLAN.md) §1 (Executive Summary)

---

## Source Consolidation Map

These documents **replace and absorb** content from:

### → `REPO_STANDARD.md`

- `docs/RICA_REPO_STANDARD.md` (DELETED)
- `rica/branding/BRANDING_POLICY.md` (branding rules section)
- `rica/adapters/*/README.md` (architecture sections)

### → `REFORK_PLAN.md`

- `docs/RICA_REFORK_PLAN.md` (NEW - untracked)
- `docs/RICA_REFORK_OUTPUT.md` (NEW - untracked)
- `docs/RICA_REFORK_EXECUTIVE_SUMMARY.md`
- `rica/vscode/patches/FULL_BRANCH_ANALYSIS.md` (branch state context)

### → `UPSTREAM_SYNC_CONFLICTS.md`

- `rica/vscode/patches/UPSTREAM_SYNC_GUIDE.md`
- `rica/vscode/patches/UPSTREAM_SYNC_IMPACT_ANALYSIS.md`
- `rica/vscode/patches/CONTROL_PLANE_ARCHITECTURE.md`
- `rica/vscode/patches/IMPLEMENTATION_HISTORY.md`
- `rica/vscode/patches/INTEGRATION_PATCHES.md`

### → `BUILD_AND_RELEASE.md`

- `docs/RICA_BUILD_GUIDE.md`
- `rica/scripts/README_BUILD.md`
- `rica/scripts/README.md`

---

## Documentation Principles

1. **Canonical location**: All RICA docs belong in `rica/docs/`
2. **Deepest detail wins**: When duplicates exist, keep the most actionable version
3. **No parallel summaries**: Executive summaries live at the top of canonical docs
4. **Optimize for maintenance**: Separate stable standards from time-bound plans
5. **Prepare for automation**: Structure content for future SKILL.md documents

---

## Future SKILL.md Documents

The conflict guide is structured for future agent automation:

| Planned Skill                    | Source Section               |
| -------------------------------- | ---------------------------- |
| `SKILL_package_json_sync.md`     | UPSTREAM_SYNC_CONFLICTS §3.1 |
| `SKILL_vscode_extension_sync.md` | UPSTREAM_SYNC_CONFLICTS §3.2 |
| `SKILL_monthly_sync_workflow.md` | UPSTREAM_SYNC_CONFLICTS §6   |
| `SKILL_refork_execution.md`      | REFORK_PLAN §4               |
| `SKILL_branding_validation.md`   | BUILD_AND_RELEASE §6         |

---

## Legacy Document Handling

Documents outside `rica/docs/` should either:

- **Point to** the canonical replacement here, OR
- **Be deleted** after content is absorbed

### Files to Delete (after verification)

- `docs/RICA_BUILD_GUIDE.md` → superseded by `BUILD_AND_RELEASE.md`
- `docs/RICA_REFORK_*.md` → absorbed into `REFORK_PLAN.md`
- `rica/vscode/patches/*.md` → absorbed into `UPSTREAM_SYNC_CONFLICTS.md`
- `.tmp_*.md` → temporary analysis files

---

## Maintenance

### When to Update

- After each upstream sync
- After architecture changes
- After new RICA features
- After conflict resolution discoveries

### Update Checklist

- [ ] Update "Last updated" date
- [ ] Verify all cross-references work
- [ ] Run through Quick Navigation links
- [ ] Check for outdated SHAs or commands
