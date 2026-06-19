# RICA Documentation Restructure Summary

> **Date:** 2026-05-21  
> **Purpose:** Overview of documentation consolidation and cleanup plan

---

## 1. New Documentation Structure

```
rica/docs/                          # CANONICAL LOCATION
├── README.md                       # Index and navigation
├── REPO_STANDARD.md                # Architecture, ownership, branding rules
├── REFORK_PLAN.md                  # Refork strategy, commit inventory, execution
├── UPSTREAM_SYNC_CONFLICTS.md      # Conflict playbooks, sync workflow
├── BUILD_AND_RELEASE.md            # Build pipeline, VSIX packaging
└── DOCUMENTATION_RESTRUCTURE_SUMMARY.md  # This file
```

**Total: 6 documents** (reduced from 16+ scattered files)

---

## 2. Document Purposes

| Document                       | Audience              | Content                                                                           |
| ------------------------------ | --------------------- | --------------------------------------------------------------------------------- |
| **README.md**                  | Everyone              | Navigation, quick links, consolidation map                                        |
| **REPO_STANDARD.md**           | Developers, Reviewers | Architecture layers, ownership model, branding governance, development rules      |
| **REFORK_PLAN.md**             | Maintainers           | Commit classification, phase mapping, execution procedure, verification checklist |
| **UPSTREAM_SYNC_CONFLICTS.md** | Maintainers           | File risk matrix, per-file playbooks, monthly workflow, decision matrix           |
| **BUILD_AND_RELEASE.md**       | Developers, CI        | Build steps, branding validation, troubleshooting                                 |

---

## 3. Files to Delete (Cleanup Plan)

### Already Deleted (in git staging)

- `docs/RICA_COMMIT_REORGANIZATION_PLAN.md`
- `docs/RICA_FEATURE_INVENTORY.md`
- `docs/RICA_REFORK_ANALYSIS.md`

### To Delete After Verification

```bash
# Root docs/ - superseded by rica/docs/
rm docs/RICA_BUILD_GUIDE.md
rm docs/RICA_REFORK_PLAN.md
rm docs/RICA_REFORK_OUTPUT.md
rm docs/RICA_REFORK_EXECUTIVE_SUMMARY.md
rm docs/RICA_REPO_STANDARD.md

# Patches - absorbed into UPSTREAM_SYNC_CONFLICTS.md
rm rica/vscode/patches/UPSTREAM_SYNC_GUIDE.md
rm rica/vscode/patches/UPSTREAM_SYNC_IMPACT_ANALYSIS.md
rm rica/vscode/patches/CONTROL_PLANE_ARCHITECTURE.md
rm rica/vscode/patches/IMPLEMENTATION_HISTORY.md
rm rica/vscode/patches/INTEGRATION_PATCHES.md
rm rica/vscode/patches/FULL_BRANCH_ANALYSIS.md

# Temp files - analysis artifacts
rm .tmp_analysis.md
rm .tmp_commit_reorg.md
rm .tmp_refork_analysis.md
```

### To Keep (Reference Only)

- `rica/branding/BRANDING_POLICY.md` - Detailed branding rules (referenced by REPO_STANDARD)
- `rica/scripts/README.md` - Script-specific documentation
- `rica/adapters/*/README.md` - Component-specific documentation

---

## 4. Content Consolidation Summary

### REPO_STANDARD.md absorbs:

| Source                             | Sections Absorbed                 |
| ---------------------------------- | --------------------------------- |
| `docs/RICA_REPO_STANDARD.md`       | All (primary source)              |
| `rica/branding/BRANDING_POLICY.md` | Branding governance rules         |
| `rica/adapters/*/README.md`        | Architecture boundary definitions |

### REFORK_PLAN.md absorbs:

| Source                                  | Sections Absorbed                    |
| --------------------------------------- | ------------------------------------ |
| `docs/RICA_REFORK_PLAN.md`              | Commit classification, phase mapping |
| `docs/RICA_REFORK_OUTPUT.md`            | Execution procedure, agent prompts   |
| `docs/RICA_REFORK_EXECUTIVE_SUMMARY.md` | Compliance scores, priorities        |
| `FULL_BRANCH_ANALYSIS.md`               | Branch state reference, SHAs         |

### UPSTREAM_SYNC_CONFLICTS.md absorbs:

| Source                             | Sections Absorbed                      |
| ---------------------------------- | -------------------------------------- |
| `UPSTREAM_SYNC_GUIDE.md`           | Compliance assessment, workflow phases |
| `UPSTREAM_SYNC_IMPACT_ANALYSIS.md` | File classification, risk matrix       |
| `CONTROL_PLANE_ARCHITECTURE.md`    | RicaCore injection pattern             |
| `IMPLEMENTATION_HISTORY.md`        | Lessons learned, failed approaches     |
| `INTEGRATION_PATCHES.md`           | Patch descriptions, diff examples      |

### BUILD_AND_RELEASE.md absorbs:

| Source                         | Sections Absorbed            |
| ------------------------------ | ---------------------------- |
| `docs/RICA_BUILD_GUIDE.md`     | Build steps, troubleshooting |
| `rica/scripts/README_BUILD.md` | Pipeline architecture        |
| `rica/scripts/README.md`       | Script documentation         |

---

## 5. Key Improvements

### Before

- 16+ markdown files scattered across 4 directories
- Duplicate content at different depths
- Multiple "executive summaries" repeating same info
- No clear canonical location
- Difficult to find authoritative version

### After

- 5 canonical documents in `rica/docs/`
- Single source of truth for each topic
- Clear navigation and cross-references
- Structured for future SKILL.md automation
- 69% reduction in file count

---

## 6. Changed Files Attention List

### High Priority (Conflict Hotspots)

These files need attention during every upstream sync:

| File                                                 | Risk   | Action                              |
| ---------------------------------------------------- | ------ | ----------------------------------- |
| `extensions/vscode/package.json`                     | 🔴 80% | Run `generate-manifest.js`          |
| `extensions/vscode/src/extension/VsCodeExtension.ts` | 🔴 40% | Re-apply RicaCore patches           |
| `core/control-plane/AuthTypes.ts`                    | 🟠 30% | Use rica/adapters/auth/AuthTypes.ts |
| `core/control-plane/env.ts`                          | 🟡 10% | Re-add useHub() guard               |

### Protected (Never Accept Upstream)

```
rica/**                     # All RICA-specific code
rica/docs/**                # All RICA documentation
rica/branding/**            # Branding assets and config
rica/adapters/**            # Auth, config, control-plane adapters
rica/vscode/integration/**  # VSCode integration layer
rica/scripts/**             # Build and validation scripts
```

---

## 7. Conflict Resolution Scenarios

### Scenario A: Monthly Upstream Sync

1. Create staging branch
2. Fetch upstream
3. Merge with `--no-commit`
4. For each conflict, follow playbook in `UPSTREAM_SYNC_CONFLICTS.md` §3
5. Validate branding
6. Test build
7. Merge to dev

### Scenario B: Urgent Security Patch

1. Cherry-pick specific commit
2. If conflicts, use decision matrix in `UPSTREAM_SYNC_CONFLICTS.md` §7
3. Fast-track validation
4. Merge directly

### Scenario C: Major Upstream Refactor

1. Analyze upstream changes
2. Update RicaCore/RicaControlPlaneClient if needed
3. Follow full sync workflow
4. Extended testing period

---

## 8. Future SKILL.md Preparation

Documents are structured to enable agent automation:

### Planned Skills

| Skill File                       | Source               | Trigger                     |
| -------------------------------- | -------------------- | --------------------------- |
| `SKILL_package_json_sync.md`     | UPSTREAM_SYNC §3.1   | package.json conflict       |
| `SKILL_vscode_extension_sync.md` | UPSTREAM_SYNC §3.2   | VsCodeExtension.ts conflict |
| `SKILL_monthly_sync_workflow.md` | UPSTREAM_SYNC §6     | Monthly sync task           |
| `SKILL_refork_execution.md`      | REFORK_PLAN §4       | Refork needed               |
| `SKILL_branding_validation.md`   | BUILD_AND_RELEASE §6 | Build validation            |

### Skill Template

```markdown
# SKILL: [Name]

## Trigger

When this skill applies.

## Context Required

- File paths
- State indicators

## Procedure

1. Step one
2. Step two
   ...

## Verification

How to confirm success.

## Rollback

How to undo if failed.
```

---

## 9. Verification Checklist

After restructure, verify:

- [ ] All 5 canonical documents exist in `rica/docs/`
- [ ] README.md links work correctly
- [ ] Cross-references between documents work
- [ ] No broken links to deleted files
- [ ] Build still works: `node rica/scripts/build-vsix.js`
- [ ] Branding validation passes: `node rica/scripts/check-branding.js`

---

## 10. Next Steps

### Immediate

1. ✅ Create consolidated documents (DONE)
2. ⬜ Delete superseded files (PENDING - needs verification)
3. ⬜ Update any external references

### Short-term

1. ⬜ Create first SKILL.md document
2. ⬜ Add documentation to CI validation
3. ⬜ Test sync workflow with documentation

### Long-term

1. ⬜ Automate documentation updates
2. ⬜ Create agent-executable skills
3. ⬜ Integrate with Continue for self-maintenance
