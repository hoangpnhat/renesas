# RICA Maintenance & Upstream Sync Handover

> **Status:** Canonical
> **Last updated:** 2026-06-09
> **Location:** `rica/docs/MAINTENANCE_AND_SYNC_HANDOVER.md`
>
> This document is the operational handover guide for maintaining RICA-OSS and performing upstream syncs with Continue.

---

## 1. Overview

RICA-OSS is a **product overlay** on top of [Continue](https://github.com/continuedev/continue) (an open-source AI code assistant). The maintenance model requires:

- Keeping RICA-specific features working (`rica/` layer)
- Periodically syncing upstream changes from Continue
- Resolving conflicts in documented hotspot files
- Validating branding and build integrity after each change

**Key principle:** RICA is not a deep fork. It is a thin overlay with adapters, wrappers, and minimal upstream patches.

---

## 2. Repository Architecture (Quick Reference)

```
rica-oss/
├── core/               ← upstream-managed (accept upstream, avoid changes)
├── packages/           ← upstream-managed
├── gui/                ← modified-upstream (has RICA GUI components in gui/src/rica/)
├── extensions/vscode/  ← modified-upstream (high conflict zone)
├── rica/               ← rica-only (NEVER accept upstream changes here)
│   ├── adapters/       ← auth, config, control-plane, core, documents, llm
│   ├── branding/       ← brand.json = single source of truth
│   ├── cli/            ← RICA CLI adapter layer
│   ├── docs/           ← all documentation (this file)
│   ├── scripts/        ← build, branding validation, manifest generation
│   ├── utils/          ← shared utilities
│   └── vscode/         ← VS Code integration bridge
└── docs/               ← upstream docs only (not RICA product docs)
```

### Ownership Rules

| Category              | Paths                            | Rule                                    |
| --------------------- | -------------------------------- | --------------------------------------- |
| **upstream-managed**  | `core/**`, `packages/**`         | Accept upstream, avoid RICA changes     |
| **modified-upstream** | `extensions/vscode/**`, `gui/**` | Manual review on sync, re-apply patches |
| **rica-only**         | `rica/**`                        | Never accept upstream, protected        |

---

## 3. Upstream Sync Workflow

### 3.1 Sync Frequency

- **Recommended:** Monthly
- **Triggers:** Major upstream releases, security patches
- **Avoid:** Syncing during active RICA feature development

### 3.2 Prerequisites

```bash
# Ensure upstream remote is configured
git remote -v
# If missing:
git remote add upstream https://github.com/continuedev/continue.git
```

### 3.3 Full Sync Procedure

#### Phase 1: Preparation (30 min)

```bash
# Tag current state for rollback
git tag pre-sync-$(date +%Y-%m)

# Create sync branch
git checkout -b rica/upstream-staging-$(date +%Y-%m)

# Fetch latest upstream
git fetch upstream

# Generate change report
git diff --stat HEAD..upstream/main > .sync-report.txt
```

#### Phase 2: Safe Changes (2 hours)

```bash
# Merge upstream (do NOT auto-commit)
git merge upstream/main --no-commit

# Accept upstream for safe areas (EXCEPT files listed in Section 4)
git checkout --theirs core/**/*.ts packages/**
```

#### Phase 3: High-Risk Areas (2 hours)

For each file in **Section 4** (Conflict Hotspots), follow its specific playbook.

#### Phase 4: Validate (1 hour)

```bash
# TypeScript check
cd extensions/vscode && npm run tsc:check && cd ../..

# Build extension
cd extensions/vscode && npm run esbuild && cd ../..

# Branding validation
node rica/scripts/check-branding.js

# Manual test: F5 in VS Code → Extension Development Host
```

#### Phase 5: Finalize (30 min)

```bash
# Commit sync
git add .
git commit -m "chore: upstream sync $(date +%Y-%m)"

# Merge to dev branch
git checkout dev
git merge --no-ff rica/upstream-staging-$(date +%Y-%m)

# Tag completed sync
git tag sync-$(date +%Y-%m)
```

### 3.4 Emergency Rollback

```bash
git checkout dev
git reset --hard pre-sync-YYYY-MM
```

---

## 4. Conflict Hotspots

### 4.1 Quick Decision Matrix

| Condition                      | Action                              |
| ------------------------------ | ----------------------------------- |
| File in `core/**` (not below)  | Accept upstream                     |
| File in `packages/**`          | Accept upstream                     |
| File in `rica/**`              | Never accept upstream               |
| File in `extensions/vscode/**` | Manual review (see playbooks below) |
| Branding-related               | Keep RICA                           |
| Auth-related                   | Keep RICA                           |
| Security patch                 | Accept upstream                     |

### 4.2 File Risk Assessment

| File                                                 | Risk      | Resolution Time | Feature    |
| ---------------------------------------------------- | --------- | --------------- | ---------- |
| `extensions/vscode/package.json`                     | VERY HIGH | 5-10 min        | Branding   |
| `extensions/vscode/src/extension/VsCodeExtension.ts` | HIGH      | 15-30 min       | Core Init  |
| `core/control-plane/AuthTypes.ts`                    | HIGH      | 10 min          | Auth       |
| `core/llm/llms/index.ts`                             | MEDIUM    | 5 min           | DynConfig  |
| `core/llm/toolSupport.ts`                            | LOW       | 5 min           | DynConfig  |
| `core/control-plane/client.ts`                       | MEDIUM    | 5 min           | TypeFix    |
| `extensions/vscode/src/stubs/WorkOsAuthProvider.ts`  | MEDIUM    | 5 min           | TypeFix    |
| `extensions/vscode/tsconfig.json`                    | LOW       | 2 min           | Build      |
| `core/protocol/core.ts`                              | LOW       | 5 min           | OnlineDocs |
| `core/protocol/passThrough.ts`                       | LOW       | 2 min           | OnlineDocs |
| `gui/src/redux/util/constructMessages.ts`            | MEDIUM    | 10 min          | OnlineDocs |
| `gui/src/components/.../AtMentionDropdown/index.tsx` | MEDIUM    | 10 min          | OnlineDocs |

### 4.3 Playbook: `extensions/vscode/package.json` (AUTOMATED)

This is the most frequent conflict. Resolution is **fully automated**:

```bash
# 1. Accept upstream version
git checkout --theirs extensions/vscode/package.json

# 2. Re-apply RICA branding
node rica/scripts/generate-manifest.js

# 3. Validate
node rica/scripts/check-branding.js

# 4. Verify
grep '"name"' extensions/vscode/package.json
# Must show: "name": "rica",

# 5. Stage
git add extensions/vscode/package.json
```

### 4.4 Playbook: `VsCodeExtension.ts` (MANUAL)

```bash
# 1. Accept upstream changes
git checkout --theirs extensions/vscode/src/extension/VsCodeExtension.ts

# 2. Re-apply RICA changes (4 locations):
#    - Import RicaCore instead of Core
#    - Type declaration: private core: RicaCore
#    - Instantiation: new RicaCore(...)
#    - Import RICA auth: RicaAuthProvider, getRicaControlPlaneSessionInfo

# 3. Test compilation
cd extensions/vscode && npm run tsc:check && cd ../..
```

**Decision guide:**

| Upstream Change                    | Action                                          |
| ---------------------------------- | ----------------------------------------------- |
| No changes to Core init            | Keep RICA version as-is                         |
| Minor changes (new methods)        | Merge upstream + keep RicaCore import           |
| Major refactor of Core constructor | Update RicaCore constructor + trap installation |

### 4.5 Playbook: `core/control-plane/AuthTypes.ts` (MANUAL)

```bash
# 1. Accept upstream
git checkout --theirs core/control-plane/AuthTypes.ts

# 2. Re-add RICA enum values:
#    RicaProd = "Rica"
#    RicaStaging = "rica-staging"

# 3. Re-add to AUTH_TYPE union types:
#    | AuthType.RicaProd | AuthType.RicaStaging
```

### 4.6 Playbook: `core/llm/llms/index.ts` (MANUAL)

```bash
# 1. Accept upstream
git checkout --theirs core/llm/llms/index.ts

# 2. Re-add normalizeProviderName() function (databricks → openai)
# 3. Re-add normalization calls in llmFromDescription() and llmFromProviderAndOptions()
```

### 4.7 Playbook: Online Documents Files (MANUAL)

For `core/protocol/core.ts`, `core/protocol/passThrough.ts`, and other Online Documents files:

```bash
# Accept upstream, then re-add RICA entries at END of structures
git checkout --theirs <file>
# Re-add RICA protocol messages / passthrough routes / context provider registration
# All RICA entries are clearly marked with "// RICA" comments
```

---

## 5. Development Rules for New Features

### 5.1 Decision Questions (Before Implementing)

1. **Can this live entirely in `rica/**`?\*\*

   - If yes -> implement there (preferred)
   - If no -> continue to question 2

2. **What is the smallest modified-upstream surface?**

   - Minimize changes to `extensions/vscode/**` and `core/**`

3. **Can the change be regenerated instead of hand-maintained?**

   - Prefer scripts over manual edits

4. **What file will likely conflict on next sync?**

   - Document in `UPSTREAM_SYNC_CONFLICTS.md`

5. **What playbook should be written now?**
   - Don't let future maintainers rediscover solutions

### 5.2 Code Placement Guide

| Type of Change            | Where to Put It              |
| ------------------------- | ---------------------------- |
| New authentication method | `rica/adapters/auth/`        |
| New configuration option  | `rica/adapters/config/`      |
| New VS Code command       | `rica/vscode/integration/`   |
| Branding change           | `rica/branding/brand.json`   |
| Build script change       | `rica/scripts/`              |
| Documentation             | `rica/docs/`                 |
| New GUI component         | `gui/src/rica/`              |
| Core platform change      | Avoid - justify if necessary |

### 5.3 Priority Rules

| Priority | Rule                                                       |
| -------- | ---------------------------------------------------------- |
| 1        | Prefer `rica/**` over `core/**`                            |
| 2        | Prefer wrappers/adapters over direct upstream modification |
| 3        | Document patches if modifying `extensions/vscode/**`       |
| 4        | Update sync conflict guidance for new hotspots             |
| 5        | Add branding validation if change affects public surfaces  |

---

## 6. Build & Release (Quick Reference)

### 6.1 Automated Build (Recommended)

```bash
node rica/scripts/build-vsix.js
```

### 6.2 Manual Build Steps

```bash
# 1. Apply branding
node rica/scripts/generate-manifest.js

# 2. Build GUI
cd gui && npm run build && cd ..

# 3. Package VSIX (runs: esbuild -> rebrand-urls -> vsce)
cd extensions/vscode && npm run package && cd ../..

# 4. Restore upstream state
git checkout -- extensions/vscode/package.json extensions/vscode/package-lock.json
```

### 6.3 Branding Validation

```bash
node rica/scripts/check-branding.js
```

Exit code `0` = pass, `1` = fail (build must be blocked).

### 6.4 Key Rule: Never Commit Branded package.json

`extensions/vscode/package.json` must remain in upstream state in Git. Branding is applied at build time only. Always restore after building.

---

## 7. Troubleshooting Guide

### 7.1 Build Issues

| Issue                     | Diagnosis                | Solution                                       |
| ------------------------- | ------------------------ | ---------------------------------------------- |
| `npm install` fails       | Network or registry      | Check proxy, `npm cache clean --force`         |
| GUI build fails           | Missing dependencies     | `cd gui && rm -rf node_modules && npm install` |
| Extension build fails     | TypeScript errors        | Check error messages, fix types                |
| Branding validation fails | Continue branding leaked | Run `generate-manifest.js`                     |
| VSIX packaging fails      | Missing files            | Check `.vscodeignore`, rebuild GUI             |
| TS2307 module not found   | RICA files not tracked   | `git status rica/` then `git add rica/`        |

### 7.2 Runtime Issues

| Issue                      | Diagnosis          | Solution                              |
| -------------------------- | ------------------ | ------------------------------------- |
| Extension won't activate   | Check Output panel | View -> Output -> "Extension Host"    |
| Auth not working           | Token issues       | Clear stored credentials, re-login    |
| Backend connection fails   | Wrong environment  | Check `rica/adapters/config/env.ts`   |
| Commands not found         | Namespace mismatch | Verify `rica.*` namespace in manifest |
| controlPlaneClient 404/403 | Trap not installed | Check RicaCore constructor log output |

### 7.3 Debug Commands

```bash
# Check extension logs (in VS Code)
# View -> Output -> Select "RICA" from dropdown

# Check if extension is loaded
# Ctrl+Shift+P -> "Developer: Show Running Extensions"

# Check branding in installed extension
code --list-extensions --show-versions | findstr rica

# Verify branding source
node -e "console.log(JSON.parse(require('fs').readFileSync('rica/branding/brand.json','utf8')).name)"

# TypeScript check (full project)
cd extensions/vscode && npx tsc --noEmit && cd ../..
```

---

## 8. Architecture Deep Dive (For Maintainers)

### 8.1 Control Plane Pattern

RICA uses a **property descriptor trap** to intercept all `controlPlaneClient` assignments:

```
VsCodeExtension.ts  →  new RicaCore(...)
                           ↓
                    installControlPlaneClientTrap()
                           ↓
                    Object.defineProperty on configHandler.controlPlaneClient
                           ↓
                    Every write → wraps in RicaControlPlaneClient
```

**Why:** Upstream `updateControlPlaneSessionInfo()` replaces the client on login/logout. The trap ensures RICA's wrapper is always re-applied.

**Dependency:** Requires `controlPlaneClient` to remain a public, configurable property on `configHandler`.

### 8.2 Dynamic Configuration Flow

```
EntraIDAuthProvider (PKCE flow)
  ├── entraAccessToken → RICA backend (/api/user-model/configs)
  │   └── Returns: model list, tool support config
  └── MDP token (exchanged) → Databricks model serving
      └── Used as apiKey on model descriptions
```

### 8.3 Online Documents Architecture

```
rica/adapters/documents/
  ├── DocumentService.ts          ← orchestrator
  ├── ElasticsearchService.ts     ← BM25 search
  ├── EmbeddingService.ts         ← vector search
  ├── ImageService.ts             ← image extraction
  ├── KnowledgeBaseService.ts     ← KB management
  └── RicaDocsContextProvider.ts  ← registered as @rica-docs

GUI components (bridge pattern):
  gui/src/rica/                    ← all RICA GUI code
  gui/src/components/.../          ← 1-line bridge re-exports only
```

---

## 9. RicaProxy LLM Provider Migration (Upcoming)

### 9.1 Current Architecture

RicaProxy (`rica/adapters/llm/RicaProxy.ts`) currently sends requests to **Databricks Model Serving endpoints** using the `/invocations` pattern:

```
Current endpoint: {apiBase}/{modelName}/invocations
Example:          https://adb-*.azuredatabricks.net/serving-endpoints/databricks-gpt-5-4/invocations
```

**Current request structure:**

```json
{
  "messages": [...],
  "temperature": 0.7,
  "max_tokens": 1024,
  "stream": true,
  "tools": [...],
  "client_request_id": "user@email.com"
}
```

**Current authentication:** MDP token (obtained via EntraID exchange) passed as `Bearer` in `Authorization` header.

### 9.2 New Architecture (AI Gateway)

The new structure uses **Databricks AI Gateway** with MLflow-compatible OpenAI chat completions format:

```
New endpoint: https://adb-379144824042062.2.azuredatabricks.net/ai-gateway/mlflow/v1/chat/completions
```

**New request structure:**

```bash
curl https://adb-379144824042062.2.azuredatabricks.net/ai-gateway/mlflow/v1/chat/completions \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $DATABRICKS_TOKEN" \
  -d '{
    "model": "databricks-gpt-5-4",
    "max_tokens": 1024,
    "messages": [
      {"role": "user", "content": "Hello!"},
      {"role": "assistant", "content": "Hello! How can I assist you today?"},
      {"role": "user", "content": "What is Databricks?"}
    ]
  }'
```

**Key differences:**

| Aspect           | Current (Serving Endpoints)                        | New (AI Gateway)                                  |
| ---------------- | -------------------------------------------------- | ------------------------------------------------- |
| Endpoint pattern | `{apiBase}/{modelName}/invocations`                | `{apiBase}/ai-gateway/mlflow/v1/chat/completions` |
| Model routing    | Model name is part of URL path                     | Model name is in request body (`"model"` field)   |
| Auth token       | MDP token (Bearer)                                 | MDP token (Bearer) - same `$DATABRICKS_TOKEN`     |
| Request format   | Custom (no `model` field, model inferred from URL) | OpenAI-compatible (requires `model` field)        |
| Streaming        | SSE via `"stream": true`                           | SSE via `"stream": true` (same)                   |

### 9.3 Files That Need Changes

| File                                    | Change Required                                       |
| --------------------------------------- | ----------------------------------------------------- |
| `rica/adapters/llm/RicaProxy.ts`        | New endpoint construction + add `model` field to body |
| `rica/adapters/config/modelMapping.ts`  | May need new apiBase format or model name mapping     |
| `rica/adapters/config/dynamicConfig.ts` | Backend may return new apiBase URL pattern            |

### 9.4 Migration Plan

**In `RicaProxy.ts` `_streamChat()`:**

```diff
- const endpoint = `${this.apiBase}${modelName}/invocations`;
- const body = {
-   messages: formattedMessages,
-   temperature: options.temperature ?? 0.7,
-   max_tokens: options.maxTokens,
-   ...
- };
+ const endpoint = `${this.apiBase}/ai-gateway/mlflow/v1/chat/completions`;
+ const body = {
+   model: modelName,
+   messages: formattedMessages,
+   temperature: options.temperature ?? 0.7,
+   max_tokens: options.maxTokens,
+   ...
+ };
```

**In `RicaProxy.ts` `_embed()`:**

```diff
- const resp = await this.fetch(`${this.apiBase}${modelName}/invocations`, {
+ const resp = await this.fetch(`${this.apiBase}/ai-gateway/mlflow/v1/embeddings`, {
    method: "POST",
    headers: embedHeaders,
-   body: JSON.stringify({ input: chunk }),
+   body: JSON.stringify({ model: modelName, input: chunk }),
  });
```

### 9.5 Token Flow (Unchanged)

```
EntraIDAuthProvider (PKCE)
  └── Exchange with RICA backend (/auth/exchange)
      └── MDP token = $DATABRICKS_TOKEN
          └── Bearer header for AI Gateway requests
```

The MDP token (`$DATABRICKS_TOKEN`) remains the same -- it is the Databricks PAT/token used for AI Gateway authentication. No changes needed in the auth flow.

### 9.6 Validation After Migration

```bash
# 1. Verify endpoint connectivity
curl -s -o /dev/null -w "%{http_code}" \
  https://adb-379144824042062.2.azuredatabricks.net/ai-gateway/mlflow/v1/chat/completions \
  -H "Authorization: Bearer $MDP_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"model":"databricks-gpt-5-4","messages":[{"role":"user","content":"test"}],"max_tokens":5}'
# Expected: 200

# 2. In VS Code Extension Development Host (F5):
#    - Open chat, send a message
#    - Verify response streams correctly
#    - Check Output panel for "RicaProxy API error" (should be none)

# 3. Test non-streaming models (if any):
#    - Verify complete response returned
```

### 9.7 Rollback Strategy

If migration fails, revert `rica/adapters/llm/RicaProxy.ts` to the `/invocations` endpoint pattern. The auth token is unchanged, so rollback is isolated to endpoint construction logic only.

---

## 10. High-Risk Scenarios & Responses (General)

| Scenario                                     | Response                                                |
| -------------------------------------------- | ------------------------------------------------------- |
| Upstream refactors Core class                | Update `RicaCore` to match new interface                |
| Upstream refactors ControlPlaneClient        | Update `RicaControlPlaneClient`, override new methods   |
| Upstream makes controlPlaneClient private    | Move injection to constructor override or find new seam |
| Upstream adds native Databricks provider     | Remove RICA's `normalizeProviderName()` alias           |
| Upstream adds native multimodal context      | Adopt upstream's approach, remove RICA's `imageUrls`    |
| Upstream converts protocol type to interface | Use module augmentation (move RICA entries to `rica/`)  |
| Upstream removes `useHub()`                  | Find new mechanism, override in `RicaCore`              |

---

## 11. Documentation Locations

| Document                               | Purpose                        | When to Update                     |
| -------------------------------------- | ------------------------------ | ---------------------------------- |
| `rica/docs/README.md`                  | Documentation index            | When adding new docs               |
| `rica/docs/REPO_STANDARD.md`           | Architecture & rules           | When architecture changes          |
| `rica/docs/BUILD_AND_RELEASE.md`       | Build guide                    | When build process changes         |
| `rica/docs/UPSTREAM_SYNC_CONFLICTS.md` | Full sync playbooks (per-file) | After each sync with new conflicts |
| `rica/docs/REFORK_PLAN.md`             | Refork strategy                | When considering major refork      |
| This document                          | Handover guide                 | For future handovers               |

### Documentation Principles

1. **Canonical location:** All RICA docs belong in `rica/docs/`
2. **Deepest detail wins:** Keep the most actionable version
3. **No parallel summaries:** One source of truth per topic
4. **Prepare for automation:** Structure for future tooling

---

## 12. Release Flow

### 12.1 Release Pipeline

```
dev branch
  ↓
UAT (collaborate with FS team) — ~1 week testing
  ↓
Build .vsix (node rica/scripts/build-vsix.js)
  ↓
Send to Renesas Software Center Team (get release deadline)
  ↓
Create CR (Change Request)
  ↓
Wait for CR approval
  ↓
Update user-guide-doc / create tutorial videos
  ↓
Ask DS about current user list
  ↓
Add to Viva Engage community
  ↓
Create announcement on Viva Engage (GIP if needed)
```

### 12.2 Release Steps Detail

| Step | Action                                         | Owner            | Duration |
| ---- | ---------------------------------------------- | ---------------- | -------- |
| 1    | Merge feature to `dev`                         | Developer        | -        |
| 2    | Deploy to UAT environment                      | FS team (Hai La) | ~1 week  |
| 3    | UAT testing & validation                       | RICA team        | ~1 week  |
| 4    | Build VSIX (`node rica/scripts/build-vsix.js`) | Developer        | 5 min    |
| 5    | Submit VSIX to Renesas Software Center         | Developer        | -        |
| 6    | Create CR (Change Request)                     | Developer        | -        |
| 7    | CR approval                                    | Approvers        | Varies   |
| 8    | Update user guide / tutorial videos            | Developer        | -        |
| 9    | Get current user list from DS                  | Developer        | -        |
| 10   | Post announcement on Viva Engage               | Developer        | -        |

### 12.3 Key Contacts for Release

| Role               | Team/Person                  | Responsibility                      |
| ------------------ | ---------------------------- | ----------------------------------- |
| UAT coordination   | FS team (Hai La)             | Backend deployment, UAT environment |
| Software Center    | Renesas Software Center Team | Distribution, release scheduling    |
| User communication | DS team                      | Current user list                   |
| Announcements      | Developer                    | Viva Engage posts, GIP              |

---

## 13. Ticket Handling

### 13.1 ServiceNow

Monitor ServiceNow for tickets related to RICA errors.

**Resolution workflow:**

1. Check the [FAQ](https://renesasgroup.sharepoint.com/:fl:/g/contentstorage/CSP_34ce1f7b-c5d5-4693-8bf1-158ff4685626/IQAzaJhGLuCARJzlpZ-3XNE5Aa7Hc46xQ_N5DcYH0FFmpto) first
2. Check User Guide documentation
3. Search for known issues in [Continue GitHub Issues](https://github.com/continuedev/continue/issues)
4. Search old posts in Viva Engage for similar problems
5. If new issue: investigate, fix, and update FAQ

### 13.2 Viva Engage

- Check frequently for new posts or comments from users
- Respond to questions and issues promptly
- Use Viva Engage for announcements (new releases, known issues, tips)

### 13.3 Common Ticket Categories

| Category              | First Action                          | Escalation |
| --------------------- | ------------------------------------- | ---------- |
| Login/Auth failure    | Check FAQ, verify EntraID config      | FS team    |
| Extension not loading | Check VS Code version, reinstall VSIX | Developer  |
| Model not responding  | Check backend status, MDP token       | FS team    |
| Feature request       | Log in backlog, assess priority       | Developer  |
| Performance issue     | Check model config, context length    | Developer  |

---

## 14. External References (SharePoint)

| Resource            | Link                                                                                                                                                                                                                                  | Purpose                                          |
| ------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------ |
| FAQ                 | [SharePoint FAQ](https://renesasgroup.sharepoint.com/:fl:/g/contentstorage/CSP_34ce1f7b-c5d5-4693-8bf1-158ff4685626/IQAzaJhGLuCARJzlpZ-3XNE5Aa7Hc46xQ_N5DcYH0FFmpto)                                                                  | Common questions & answers for users and support |
| RICA Release folder | [SharePoint Release](<https://renesasgroup.sharepoint.com/:f:/r/sites/AIDataAnalyticsDivision-AI/Shared%20Documents/AI/01_AI%20Project/01_Coding%20Assistant%20(RICA)/03_RICA%20release/2_RICA%20IDE%20release?csf=1&web=1&e=D5NkiJ>) | VSIX builds, release notes, user guides          |

---

## 15. Contacts & Escalation

| Role                | Contact       | Scope                  |
| ------------------- | ------------- | ---------------------- |
| Previous maintainer | Sang Dao      | Architecture questions |
| Continue upstream   | GitHub Issues | Platform bugs          |
| Fullstack team      | Hai La        | Backend/deployment     |

---

<!-- ## 16. Handover Checklist

### Knowledge Transfer Verification

- [ ] Understand repository architecture (Section 2)
- [ ] Can perform upstream sync independently (Section 3)
- [ ] Know all conflict hotspots and playbooks (Section 4)
- [ ] Understand development rules for new features (Section 5)
- [ ] Can build VSIX independently (Section 6)
- [ ] Can troubleshoot common issues (Section 7)
- [ ] Understand control plane architecture (Section 8)
- [ ] Understand RicaProxy migration plan (Section 9)
- [ ] Know documentation locations (Section 11)
- [ ] Understand release flow (Section 12)
- [ ] Know ticket handling process (Section 13)
- [ ] Have access to SharePoint resources (Section 14) -->

### Access & Credentials Needed

| Item                              | Status | Notes                    |
| --------------------------------- | ------ | ------------------------ |
| GitLab repository access          |        |                          |
| RICA backend/control plane access |        | If applicable            |
| EntraID app registration details  |        | Client ID, Tenant ID     |
| CI/CD pipeline credentials        |        | If applicable            |
| VS Code Marketplace publisher     |        | If publishing externally |

### Post-Handover Support

| Phase              | Duration | Support Level                  |
| ------------------ | -------- | ------------------------------ |
| Shadow period      | 2 weeks  | Available for questions        |
| Transition period  | 2 weeks  | Available for questions        |
| Maintenance period | Ongoing  | Escalation for critical issues |

---

<!-- ## 17. Related Canonical Documents

- [`README.md`](./README.md) -- Documentation index
- [`REPO_STANDARD.md`](./REPO_STANDARD.md) -- Architecture standard
- [`UPSTREAM_SYNC_CONFLICTS.md`](./UPSTREAM_SYNC_CONFLICTS.md) -- Detailed per-file conflict playbooks
- [`REFORK_PLAN.md`](./REFORK_PLAN.md) -- Refork strategy and commit classification
- [`BUILD_AND_RELEASE.md`](./BUILD_AND_RELEASE.md) -- Full build guide -->
