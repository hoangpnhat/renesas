# RICA Refork Plan

> **Status:** Canonical document  
> **Last updated:** 2026-06-01  
> **Location:** `rica/docs/REFORK_PLAN.md`
>
> This document consolidates:
>
> - `docs/RICA_REFORK_PLAN.md` (commit inventory)
> - `docs/RICA_REFORK_OUTPUT.md` (action-ready summary)
> - `docs/RICA_REFORK_EXECUTIVE_SUMMARY.md` (compliance scores)
> - Branch state context from `rica/vscode/patches/FULL_BRANCH_ANALYSIS.md`

---

## 1. Executive Summary

### Current Status: 🟡 Functional — Online Documents Complete, Dynamic Config In Progress

| Criteria               | Score      | Status                                        |
| ---------------------- | ---------- | --------------------------------------------- |
| Architecture Boundary  | 8/10       | ⚠️ 8 core files modified (minimal, justified) |
| Branding Enforcement   | 9/10       | ✅ Mostly compliant                           |
| Commit Organization    | 7/10       | ⚠️ Phased commits on refork-clean             |
| Sync-Friendliness      | 8/10       | ✅ Property trap pattern, documented hotspots |
| **Overall Compliance** | **8.0/10** | Target: ≥8.5/10                               |

### What's Working ✅

- RICA extension runs successfully
- EntraID authentication functional
- Commands registered as `rica.*`
- Branding shows RICA everywhere
- Control Plane wrapper prevents 404/403 errors
- Dynamic config loading from RICA backend (`/api/user-model/configs`)
- Provider alias mapping: `rica-proxy` → `continue-proxy`, `databricks` → `openai`
- Property descriptor trap ensures RICA client survives login/logout
- Auto-refresh (5 min) with cache dedup (30s TTL)
- Corporate proxy detection (redirect + content-type)
- Auth token passthrough (Bearer from EntraID session)
- Online Documents: multimodal RAG with images sent to LLM
- Knowledge Base listing and refresh
- Hybrid search (Elasticsearch BM25 + vector embedding)
- Context items clickable: images open directly in VSCode
- GUI components isolated in `gui/src/rica/` (bridge pattern)
- Protocol types owned by `rica/adapters/documents/types.ts`

### What Needs Fixing ❌

- ~10 core/ files modified (minimal, additive — documented below in A4 upstream table)
- AuthTypes in `core/` (RICA enum values added to upstream AuthType)
- End-to-end validation with real backend not yet complete
- Phase 4 (Dynamic Config) squash commit cần verify missing commits

### Time Estimate

- Refactoring: 2 days
- Automation: 1 day
- Documentation: 0.5 day
- Testing: 0.5 day
- **Total: 4 days** to reach compliance target

---

## 2. Commit Classification

### Overview

| Group | Description                     | Count | Action                                         |
| ----- | ------------------------------- | ----- | ---------------------------------------------- |
| **A** | RICA-Specific Features          | ~130  | **RE-APPLY** (squash into Phase commits)       |
| **B** | Generic Bugfixes/Features       | ~35   | **EVALUATE** per commit (DROP if upstream has) |
| **C** | Upstream Sync/Format/Cleanup    | ~7    | **DROP** (refork brings these)                 |
| **D** | Discardable/Temp/WIP/Duplicates | ~19   | **DROP**                                       |

---

### Group A — RICA-Specific Features (RE-APPLY)

#### A1. Branding & UI Customization → Phase 2

| SHA       | Date       | Author          | Subject                                                 | Status     |
| --------- | ---------- | --------------- | ------------------------------------------------------- | ---------- |
| b906fe2e9 | 2025-06-16 | kiettranrenesas | Disable some llm providers, remove Add new model button | ✅ In plan |
| a0565011f | 2025-06-19 | kiettranrenesas | Change application name, change the show message        | ✅ In plan |
| ce7e3b42b | 2025-06-20 | haila257        | Change icon view for rica-continue                      | ✅ In plan |
| 074fe6ef2 | 2025-06-24 | kiettranrenesas | Remove start window                                     | ✅ In plan |
| a01242d30 | 2025-06-22 | kiettranrenesas | Update feedback forms                                   | ⚠️ THIẾU   |
| 4bbb518a5 | 2025-07-14 | haila257        | chore: remove icon and update README                    | ⚠️ THIẾU   |
| 0d08e546b | 2025-07-14 | haila257        | fix: details readme                                     | ⚠️ THIẾU   |
| b81431176 | 2025-07-15 | kiettranrenesas | Make rica icon bigger                                   | ✅ In plan |
| 276ecdb0b | 2025-07-15 | kiettranrenesas | Update: model configurations, warning users to sign in  | ⚠️ THIẾU   |
| 1722068c7 | 2025-07-15 | kiettranrenesas | Update model configurations                             | ⚠️ THIẾU   |
| 64f2db182 | 2025-07-15 | kiettranrenesas | Fix issue: don't have sign in button                    | ⚠️ THIẾU   |
| 084e0d7ca | 2025-07-16 | kiettranrenesas | Change feedback links                                   | ⚠️ THIẾU   |
| 9a7d4b05f | 2025-08-20 | kiet.tran       | Update feedback forms                                   | ⚠️ THIẾU   |
| 03d42a016 | 2025-08-20 | kiet.tran       | Update feedback link                                    | ⚠️ THIẾU   |
| 73daa33e9 | 2026-02-02 | Bruce Pham      | Improve RICA GUI components                             | ✅ In plan |

> **⚠️ CẦN KIỂM TRA:** 9 commits thiếu ở trên có thể đã được bao hàm trong squash commit Phase 2 (`27a190493`). Verify trước khi thêm.

**Squash message:** `feat(rica): branding system — app name, icons, hide Continue providers, GUI customization`

#### A2. Authentication & SSO (EntraID) → Phase 3

| SHA       | Date       | Author          | Subject                                                | Status     |
| --------- | ---------- | --------------- | ------------------------------------------------------ | ---------- |
| 62b9a53de | 2025-06-19 | tung-pham-fz    | SSO Login Entra ID                                     | ✅ In plan |
| 0901d30aa | 2025-06-23 | tung-pham-fz    | SSO Temp (early SSO implementation)                    | ⚠️ THIẾU   |
| ef77c80e4 | 2025-07-02 | tung-pham-fz    | Refactor WorkOS to EntraID and update model configs    | ✅ In plan |
| 5544c2d99 | 2025-07-14 | kiettranrenesas | Merge sso feature + disable doc button when API issues | ⚠️ THIẾU   |
| 932701ded | 2025-07-31 | Hoang Pham      | fix bug authentication session                         | ⚠️ THIẾU   |
| 444afe24b | 2025-08-18 | kiet.tran       | Fix problem: agent doesn't work (post-auth)            | ⚠️ THIẾU   |
| 65be80132 | 2025-08-20 | haila257        | Change the flow entra ID to be exchanged               | ✅ In plan |
| 76bdb6b90 | 2025-08-20 | kiet.tran       | increase time for singing out                          | ⚠️ THIẾU   |
| 4aefa3d0c | 2025-08-20 | haila257        | improvement: add client_request_id                     | ⚠️ THIẾU   |
| d625b7def | 2025-08-20 | haila257        | improvement: define client_request_id                  | ⚠️ THIẾU   |
| 59e5ffb0f | 2025-08-21 | kiet.tran       | Fix Agent mode after merge (post-auth)                 | ⚠️ THIẾU   |
| aad926b4a | 2025-08-27 | haila257        | improvement: add offline_access to get refresh token   | ⚠️ THIẾU   |
| 2f34b1edc | 2025-08-27 | haila257        | fix: error get access token                            | ⚠️ THIẾU   |
| f6935cc5b | 2025-08-27 | Hoang Pham      | Fix RicaProxy agent tool calling and SecretStorage     | ✅ In plan |
| d41e63d9c | 2025-08-28 | kiet.tran       | Refresh token after a period of time                   | ✅ In plan |
| dac153609 | 2025-12-04 | haila257        | feat: add version to body request (renew token)        | ⚠️ THIẾU   |
| 435dae125 | 2026-02-02 | Bruce Pham      | Improve token refresh mechanism                        | ✅ In plan |

> **⚠️ CẦN KIỂM TRA:** 11 commits thiếu ở trên có thể đã được bao hàm trong squash commit Phase 3 (`1b4b4a8a3`). Verify trước khi thêm.

**Squash message:** `feat(rica): EntraID SSO authentication, token refresh, SecretStorage`

#### A3. Dynamic Configuration → Phase 4

| SHA       | Date       | Author     | Subject                                                      | Status     |
| --------- | ---------- | ---------- | ------------------------------------------------------------ | ---------- |
| fa81dcf82 | 2025-10-01 | HoangPham  | Add dynamic configuration with EntraID integration           | ✅ In plan |
| f0358664d | 2025-12-04 | Bruce Pham | Fix dynamic config crash when loading RICA profile           | ✅ In plan |
| 1af478119 | 2026-02-05 | Bruce Pham | Add automatic refresh mechanism for RICA Dynamic config      | ✅ In plan |
| 8cc0564d7 | 2026-02-24 | Bruce Pham | fix: extract contextLength from RICA dynamic config response | ⚠️ THIẾU   |
| f181532ee | 2026-03-05 | Bruce Pham | hotfix: fix contextLength parsing and validation for dynamic | ⚠️ THIẾU   |
| ac3f33f1e | 2026-04-15 | Sang Dao   | fix: Inject model_id from dynamic config into BaseLLM (YAML) | ⚠️ THIẾU   |
| 1ad4b4e4c | 2026-04-16 | Sang Dao   | fix: wrong DYNAMIC_CONFIG_REFRESH_INTERVAL_MS value          | ⚠️ THIẾU   |
| e95b2c560 | 2026-04-16 | Sang Dao   | Restore correct refresh interval and fix model auto-switch   | ✅ In plan |

> **⚠️ CẦN KIỂM TRA:** 4 commits thiếu — đặc biệt `ac3f33f1e` (model_id inject) và `1ad4b4e4c` (refresh interval) rất quan trọng cho dynamic config hoạt động đúng. Verify xem đã có trong Phase 4 squash chưa.

**Squash message:** `feat(rica): dynamic configuration with EntraID + auto-refresh`

#### A4. RICA Online Documents / Multimodal RAG → Phase 5

| SHA       | Date       | Author     | Subject                                                             | Status  |
| --------- | ---------- | ---------- | ------------------------------------------------------------------- | ------- |
| c0edd2a68 | 2025-11-19 | Bruce Pham | Add RICA Online Documents feature                                   | ✅ Done |
| c0298af21 | 2025-11-27 | Bruce Pham | Implement complete RICA Online Documents multimodal RAG             | ✅ Done |
| f6143c196 | 2025-12-11 | Bruce Pham | Implement hybrid search for RICA Online Documents                   | ✅ Done |
| 14832d298 | 2025-12-23 | Bruce Pham | refactor: improve RICA Online Documents multimodal RAG system       | ✅ Done |
| c3c5c59a8 | 2025-12-29 | Bruce Pham | refactor: replace hardcoded tokens with dynamic auth in services    | ✅ Done |
| fedb530f3 | 2026-01-12 | Bruce Pham | Add Knowledge Base support to RICA Online Documents                 | ✅ Done |
| ade805177 | 2026-01-15 | Bruce Pham | feat: enhance RICA Online Documents with Knowledge Base integration | ✅ Done |
| 976f8799e | 2026-01-16 | Bruce Pham | chore: comment out verbose logging to reduce console noise          | ✅ Done |
| be52229b4 | 2026-02-02 | Bruce Pham | fix: update production Elasticsearch index name to user_documents   | ✅ Done |
| ce9cb7fc1 | 2026-02-26 | Bruce Pham | improve: Add page parameter support for RICA docs context provider  | ✅ Done |

> **✅ Tất cả commits đã được implement trên `rica/refork-clean`.** Bao gồm:
>
> - Multimodal RAG pipeline hoàn chỉnh: images gửi tới LLM thành công
> - Knowledge Base listing + refresh
> - Hybrid search (Elasticsearch + embedding)
> - Dynamic auth (EntraID + MDP token)
> - Page parameter support cho context provider (UI page input in popover + @mention)
> - GUI components đã move vào `gui/src/rica/` (bridge pattern, giảm conflict khi sync)
> - Protocol types đã move vào `rica/adapters/documents/types.ts` (core re-export bridge)
> - Context items clickable: mở image files trực tiếp trong VSCode
> - Search/filter trong cả popover, settings section, và docs page
> - Config tab "RICA Docs" với search, pagination, tabs (Documents/Knowledge Bases)
> - UI redesigned: Tailwind + design tokens (Card, EmptyState, ConfigHeader), không dùng hardcoded colors

**Squash message:** `feat(rica): Online Documents — multimodal RAG, hybrid search, Knowledge Base, page parameter, image viewer`

**File layout:**

```text
rica/adapters/documents/
├── types.ts                  # Source of truth cho protocol types (DocumentListResponse, etc.)
├── DocumentService.ts        # Document CRUD via RICA API
├── ElasticsearchService.ts   # Hybrid search (BM25 + vector)
├── EmbeddingService.ts       # Text embedding via RICA API
├── ImageService.ts           # Image download + save to workspace (returns file URI)
├── KnowledgeBaseService.ts   # KB listing/refresh via RICA API
├── RicaDocsContextProvider.ts # @rica-docs context provider
├── RicaDocumentsService.ts   # Orchestrator (coordinates all services)
├── ServiceEndpoints.ts       # Environment-based endpoint resolution
├── pageNumberParser.ts       # Parse "docId:p1-p5" syntax
└── index.ts

gui/src/rica/
├── index.ts                  # Barrel export
├── components/
│   ├── RicaDocsPopover.tsx   # Input toolbar popover (doc selection + page input)
│   └── RicaDocsPageInput.tsx # Page input for @mention dropdown (minimal upstream surface)
└── pages/
    ├── RicaDocsSection.tsx   # Config tab section (search, pagination, tabs)
    └── docs/
        ├── DocsPage.tsx      # Full-page docs browser (search, tabs)
        └── KnowledgeBaseView.tsx  # KB listing UI

core/protocol/documents.ts    # Re-export bridge → rica/adapters/documents/types.ts
```

**Upstream changes (minimal, tất cả ở cuối file/additive):**

| File                                                 | Change                                      | Risk     |
| ---------------------------------------------------- | ------------------------------------------- | -------- |
| `core/protocol/core.ts`                              | 5 RICA message entries (end of type)        | Low      |
| `core/protocol/passThrough.ts`                       | 5 RICA routes (end of array)                | Low      |
| `core/protocol/documents.ts`                         | New file, re-export from rica/              | None     |
| `core/index.d.ts`                                    | `imageUrls?: string[]` on ContextItem       | Very low |
| `core/config/types.ts`                               | `imageUrls?: string[]` on ContextItem       | Very low |
| `core/config/loadContextProviders.ts`                | Auto-instantiate rica-docs (8 lines)        | Low      |
| `core/llm/autodetect.ts`                             | `"rica-proxy"` in PROVIDER_SUPPORTS_IMAGES  | Very low |
| `gui/src/redux/util/constructMessages.ts`            | imageUrls → ImageMessagePart (~10 lines)    | Low      |
| `gui/src/components/.../AtMentionDropdown/index.tsx` | rica-docs page input intercept (~20 lines)  | Low      |
| `gui/src/components/.../BlockSettingsTopToolbar.tsx` | Import + render RicaDocsPopover (~16 lines) | Low      |
| `gui/src/pages/config/configTabs.tsx`                | rica-docs tab entry (~14 lines)             | Very low |
| `gui/src/util/navigation.ts`                         | RICA_DOCS route (2 lines)                   | Very low |
| `extensions/vscode/src/VsCodeIde.ts`                 | Binary file open logic (~7 lines)           | Low      |

**GUI bridge files (1-line re-exports, zero conflict risk):**

| File                                                | Re-exports from                           |
| --------------------------------------------------- | ----------------------------------------- |
| `gui/src/components/.../RicaDocsPopover.tsx`        | `gui/src/rica/components/RicaDocsPopover` |
| `gui/src/pages/config/sections/RicaDocsSection.tsx` | `gui/src/rica/pages/RicaDocsSection`      |

#### A5. Token Consumption Tracking → Phase 6

| SHA       | Date       | Author     | Subject                                             | Status     |
| --------- | ---------- | ---------- | --------------------------------------------------- | ---------- |
| 50074a4f6 | 2026-04-07 | Bruce Pham | Add token consumption tracking and auto-sync        | ✅ In plan |
| ce9d044b9 | 2026-04-08 | Bruce Pham | Change token consumption sync to individual records | ✅ In plan |
| adb3cce92 | 2026-04-14 | Sang Dao   | Filter LLM model based on consumption and threshold | ✅ In plan |
| 9a35cd015 | 2026-04-15 | Bruce Pham | fix: Correct field mappings and remove time filter  | ⚠️ THIẾU   |
| 4cfc15c62 | 2026-04-15 | Sang Dao   | Filter out consumption record that has no output    | ⚠️ THIẾU   |

> **⚠️ 2 commits thiếu** — đây là bugfixes cho token consumption, cần đi kèm.

**Squash message:** `feat(rica): token consumption tracking, auto-sync, threshold-based filtering`

#### A6. Databricks Provider → Phase 6 (part 2)

| SHA       | Date       | Author          | Subject                                                     | Status     |
| --------- | ---------- | --------------- | ----------------------------------------------------------- | ---------- |
| 2031bfa62 | 2025-06-24 | kiettranrenesas | Update databricks provider                                  | ✅ In plan |
| 5ef0b34f8 | 2025-07-01 | Hoang Pham      | Update toolSupport (agent) for Databricks provider          | ✅ In plan |
| 08045fb0c | 2026-04-02 | Hoang Pham      | Fix Indexing UNIQUE constraint and Databricks embedding API | ✅ In plan |

**Squash message:** `feat(rica): Databricks provider customization, embedding fixes`

#### A8. Environment & Endpoints Switching → Phase 7

| SHA       | Date       | Author     | Subject                                                 | Status     |
| --------- | ---------- | ---------- | ------------------------------------------------------- | ---------- |
| 91bed4bcc | 2026-02-02 | Bruce Pham | Add environment-based service endpoints switching       | ✅ In plan |
| a5382ce14 | 2026-03-31 | Bruce Pham | Fix environment inconsistency between auth and services | ✅ In plan |

**Squash message:** `feat(rica): environment-based service endpoints (Prod/Non-Prod/Local)`

#### A7. RICA-specific Bugfixes & Misc → Phase (TBD)

| SHA       | Date       | Author          | Subject                                             | Status   |
| --------- | ---------- | --------------- | --------------------------------------------------- | -------- |
| a73a2da79 | 2025-06-19 | kiettranrenesas | Fix bugs: can't build the codebase                  | ⚠️ THIẾU |
| 11da4871d | 2025-06-22 | kiettranrenesas | fix: isChatOnlyModel handles model name with 'gpt'  | ⚠️ THIẾU |
| 44fb4d8dc | 2025-06-27 | kiettranrenesas | fix issue: can't install package (rica-binary)      | ⚠️ THIẾU |
| 162c1b4b4 | 2025-07-23 | Hoang Pham      | Fix bug read prompt file path                       | ⚠️ THIẾU |
| d417693c2 | 2025-08-07 | Hoang Pham      | fix extract right content from docstring generation | ⚠️ THIẾU |
| a6235cb68 | 2025-08-07 | Hoang Pham      | fix shortcut accept/reject commands                 | ⚠️ THIẾU |
| 7fe7a34f0 | 2025-08-28 | kiet.tran       | Fix: chat and auto completion with new request body | ⚠️ THIẾU |
| 76e664b32 | 2025-09-06 | kiet.tran       | Fix issue: don't log the username                   | ⚠️ THIẾU |
| 1847bbb15 | 2025-11-20 | Bruce Pham      | perf: optimize extension startup performance        | ⚠️ THIẾU |

> **⚠️ 9 commits.** Đây là bugfixes liên quan trực tiếp đến RICA fork. Cần đánh giá từng commit: một số có thể đã outdated nếu upstream đã thay đổi code base.

**Squash message:** `fix(rica): RICA-specific bugfixes — build, auth session, model detection, startup perf`

#### A9. Build & Release Pipeline → Phase 8

| SHA       | Date       | Author     | Subject                                              | Status     |
| --------- | ---------- | ---------- | ---------------------------------------------------- | ---------- |
| 992a4deaf | 2025-11-20 | Bruce Pham | handle win32 platform with .exe file of package      | ⚠️ THIẾU   |
| 1c711018e | 2025-12-05 | Bruce Pham | Build script multi platforms                         | ✅ In plan |
| ad885a8a2 | 2026-02-10 | Bruce Pham | chore: bump version to 1.1.1 and update ripgrep      | ⚠️ THIẾU   |
| 5e1b3e7ba | 2026-02-10 | Bruce Pham | chore: update package-lock.json after ripgrep update | ⚠️ THIẾU   |
| 2175ba8bd | 2026-03-13 | sang.dao   | Define build pipeline                                | ✅ In plan |
| 97950c1ae | 2026-03-13 | Sang Dao   | Update .gitlab-ci.yml                                | ⚠️ THIẾU   |
| e387ec45f | 2026-03-13 | Sang Dao   | feat: tag na-linux                                   | ⚠️ THIẾU   |
| a20baf352 | 2026-03-16 | Sang Dao   | suppress type error when build gui package           | ⚠️ THIẾU   |
| 4c91a8e21 | 2026-03-17 | Sang Dao   | Build linux                                          | ⚠️ THIẾU   |
| 18061d1ab | 2026-03-18 | Sang Dao   | build for windows os                                 | ⚠️ THIẾU   |
| 65761e272 | 2026-03-19 | Sang Dao   | Finalize Gitlab build pipeline                       | ✅ In plan |
| b08141fac | 2026-03-19 | Sang Dao   | Remove unsupported release job from CI pipeline      | ⚠️ THIẾU   |
| 16202033d | 2026-03-20 | Sang Dao   | fix error "jobs:release_job config contains unknown" | ⚠️ THIẾU   |
| 7168fbe72 | 2026-03-20 | Sang Dao   | configure artifact for build job                     | ⚠️ THIẾU   |
| 950262511 | 2026-03-24 | Sang Dao   | remove redundant command of package script           | ⚠️ THIẾU   |
| 4b5ba2d96 | 2026-03-24 | Sang Dao   | support build on windows environment                 | ⚠️ THIẾU   |
| 53c995350 | 2026-03-24 | Sang Dao   | remove @lancedb/vectordb-linux-x64-gnu               | ⚠️ THIẾU   |

> **⚠️ 14 commits thiếu** — đây là toàn bộ quá trình build pipeline evolution. Cần squash cùng 3 commits gốc.

**Squash message:** `chore(rica): GitLab CI/CD pipeline, multi-platform build (linux/windows)`

#### A10. RICA Documentation → Phase 1

| SHA       | Date       | Author     | Subject                                                  | Status   |
| --------- | ---------- | ---------- | -------------------------------------------------------- | -------- |
| 5f63f70fd | 2025-12-05 | Bruce Pham | docs: update release notes for version 1.0.2             | ⚠️ THIẾU |
| 8f7f0fe2a | 2026-02-03 | Bruce Pham | docs: add upstream migration plan and user manual        | ⚠️ THIẾU |
| 577336303 | 2026-02-24 | Hoang Pham | docs: Add Linux build documentation and release branch   | ⚠️ THIẾU |
| 9a1728f2d | 2026-03-04 | Bruce Pham | docs: add comprehensive build and troubleshooting guides | ⚠️ THIẾU |
| e56c0dff3 | 2026-03-04 | Bruce Pham | docs: add tree-sitter-verilog support guide and cleanup  | ⚠️ THIẾU |

> **⚠️ 5 commits docs.** Có thể đã có trong Phase 1 squash (`8f7085329`). Verify.

**Squash message:** `docs(rica): upstream migration plan, user manual, build & release docs`

#### A11. Multi-Agent Orchestration (Rakibul Haque) → Phase (NEW)

| SHA       | Date       | Author        | Subject                                                         | Status   |
| --------- | ---------- | ------------- | --------------------------------------------------------------- | -------- |
| 48a4cd4ea | 2026-04-17 | Rakibul Haque | feat: add multi-agent orchestration system (v1.0 -> v4.0)       | ⚠️ THIẾU |
| 11801c24d | 2026-04-17 | Rakibul Haque | fix: resolve TypeScript compilation errors in CI/CD build       | ⚠️ THIẾU |
| 284120847 | 2026-04-17 | Rakibul Haque | fix: pass build job artifacts to release job in CI pipeline     | ⚠️ THIẾU |
| 832e8e6ea | 2026-04-22 | Rakibul Haque | feat: add MultiAgentLogger for sub-agent session tracing        | ⚠️ THIẾU |
| 575c2cab8 | 2026-04-22 | Rakibul Haque | feat: wire live logging into SubAgentRunner and spawnAgents     | ⚠️ THIẾU |
| c0e06a221 | 2026-04-22 | Rakibul Haque | feat: compress old session logs, keep last 20 uncompressed      | ⚠️ THIẾU |
| cd1d8beda | 2026-04-22 | Rakibul Haque | feat: add orchestrator, IPC, API error, prompt assembly logging | ⚠️ THIẾU |
| 083e03d17 | 2026-04-23 | Rakibul Haque | feat: add session log export UI, stale agent fix                | ⚠️ THIẾU |
| bc004e65e | 2026-04-23 | Rakibul Haque | docs: add v5.0 section — structured logging, log export         | ⚠️ THIẾU |
| e3fc89aac | 2026-04-24 | Rakibul Haque | feat: v5.1 — checkpoint eval, budget UI, orchestrator overhaul  | ⚠️ THIẾU |
| 130f6efc9 | 2026-04-24 | Rakibul Haque | fix: resolve TypeScript strict null errors in SubAgentRunner    | ⚠️ THIẾU |
| 1759a80b5 | 2026-05-09 | Rakibul Haque | feat: clickable Task/Result in agent graph tree opens .md       | ⚠️ THIẾU |
| fa1127bd7 | 2026-05-11 | Rakibul Haque | feat: multi-agent Tier S model lock + GPT-5 backend compat      | ⚠️ THIẾU |
| ddf6a5fd8 | 2026-05-19 | Rakibul Haque | feat: v1.1.7 — multi-agent live budget injection, guided tour   | ⚠️ THIẾU |
| 7ca5e96cf | 2026-05-20 | Rakibul Haque | feat: v1.1.8 — release-eve triage closeout                      | ⚠️ THIẾU |

> **⚠️ 15 commits — HOÀN TOÀN MỚI, chưa từng có trong plan.** Đây là hệ thống multi-agent orchestration do Rakibul Haque phát triển (Apr-May 2026). Cần quyết định có đưa vào refork hay không.

**Squash message:** `feat(rica): multi-agent orchestration system — logging, budget, checkpoint, graph UI`

#### A12. Vision & Model Enhancements → Phase (TBD)

| SHA       | Date       | Author     | Subject                                        | Status   |
| --------- | ---------- | ---------- | ---------------------------------------------- | -------- |
| 3a9066141 | 2025-09-09 | kiet.tran  | Enable processing image with gpt               | ⚠️ THIẾU |
| 702dfa972 | 2026-02-02 | Bruce Pham | feat: add Gemini to vision-capable models list | ⚠️ THIẾU |

> **⚠️ 2 commits** — vision/multimodal model support. Cần KEEP cho RICA.

**Squash message:** `feat(rica): vision model support — GPT image processing, Gemini vision`

---

### Group B — Generic Bugfixes (EVALUATE)

Before cherry-picking, verify with `git log upstream/main`:

```bash
git log upstream/main --oneline | findstr /I "parallel tool"
git log upstream/main --oneline | findstr /I "compaction conversation summary"
git log upstream/main --oneline | findstr /I "verilog"
git log upstream/main --oneline | findstr /I "rule toggle"
git log upstream/main --oneline | findstr /I "SSH path"
```

#### B.1 Parallel Tool Calls — ⚠️ CHECK UPSTREAM

- `165f57c0c` through `a4721d8cd` (9 commits)
- `ecd0938a5` — fix: map tool call deltas by ID instead of index ⚠️ **THIẾU — BỔ SUNG**
- **Likely already in upstream** (Patrick Erichsen is upstream contributor)

#### B.2 Conversation Summary / Compaction — ⚠️ CHECK UPSTREAM (THIẾU)

- `3697a24ae` feat: add conversation summary (compaction) feature ⚠️ **THIẾU**
- `efbeeac44` feat: improve conversation summary with full context ⚠️ **THIẾU**
- **Cần verify upstream có tương đương chưa**

#### B.3 Rule Toggle & Invokable Rules — ⚠️ CHECK UPSTREAM (THIẾU)

- `ad5c1ed2a` feat: add rule toggle, invokable rules, and markdown support ⚠️ **THIẾU**
- **Cần verify upstream đã hỗ trợ chưa**

#### B.4 Verilog / Tree-sitter — ✅ KEEP

- `44a6d05fe` Add complete Verilog language support
- `e56c0dff3` docs: add tree-sitter-verilog support guide ⚠️ **THIẾU — BỔ SUNG**
- **Renesas semiconductor needs this**

#### B.5 Autocomplete Tunings — ✅ KEEP

- `9a46ba96a` Set max-tokens = 256
- `2dda59cc6` Set debounce = 400ms

#### B.6 Windows SSH Path Resolution — ⚠️ CHECK UPSTREAM (THIẾU)

- `9f3aca0ec` Fix path resolution for remote SSH connections from Windows ⚠️ **THIẾU**
- **Cần verify upstream đã fix chưa**

---

### Group C — DROP (refork brings these)

`710c0468a`, `545dec2f4`, `5db591e73`, `2f7b2ac3c`, `221afa011`

---

### Group D — DROP (temp/WIP/duplicate/debug)

`f192f5ae2`, `95c4e8172`, `b0fccef57`, `926542f13` (duplicate of `a27b5defa`)

Thêm các commit debug/temp:

- `1fc3a78d5` — add log check autocomplete multiple line (debug log)
- `3e043c007` — add console log headers Databricks (debug log)
- `b0fccef57` — chore: remove debug console.log statements
- `91197f8f0` — Add hardware configuration files (không liên quan code)
- `0901d30aa` — SSO Temp (superseded by `62b9a53de`)

---

## 3. Phase Mapping

| Phase | Name                                  | Squash Message                                                            | Cluster | Status         |
| ----- | ------------------------------------- | ------------------------------------------------------------------------- | ------- | -------------- |
| 1     | RICA repository documentation         | `docs(rica): upstream migration plan, user manual, build & release docs`  | A10     | ✅ Done        |
| 2     | RICA branding system                  | `feat(rica): branding system — app name, icons, hide Continue providers`  | A1      | ✅ Done        |
| 3     | EntraID SSO authentication            | `feat(rica): EntraID SSO authentication, token refresh, SecretStorage`    | A2      | ✅ Done        |
| 4     | Dynamic Configuration & Control Plane | `feat(rica): dynamic config, provider alias, property trap, auto-refresh` | A3      | 🔧 In Progress |
| 5     | RICA Online Documents                 | `feat(rica): Online Documents — multimodal RAG, hybrid search, KB, page`  | A4      | ✅ Done        |
| 6     | Token consumption & Databricks        | `feat(rica): token consumption tracking + Databricks provider`            | A5 + A6 | ⬜ Pending     |
| 7     | Environment switching                 | `feat(rica): environment-based service endpoints`                         | A8      | ✅ Done        |
| 8     | Build & release pipeline              | `chore(rica): GitLab CI/CD pipeline, multi-platform build`                | A9      | ⬜ Pending     |
| 9     | RICA-specific bugfixes                | `fix(rica): RICA-specific bugfixes — build, auth, model detection`        | A7      | ⬜ Pending     |
| 10    | Vision & model enhancements           | `feat(rica): vision model support — GPT image, Gemini vision`             | A12     | ⬜ Pending     |
| 11    | Multi-Agent Orchestration             | `feat(rica): multi-agent orchestration system`                            | A11     | ⬜ Pending     |
| 12    | Generic features (selected)           | (varies)                                                                  | from B  | ⬜ Pending     |

---

## 4. Execution Procedure

### Step 0 — Create refork branch

```bash
git fetch upstream
git checkout -b rica/refork-v2 upstream/main
```

### Step 1 — Replay Phase commits from rica/refork-clean

```bash
git cherry-pick 8f7085329   # Phase 1: RICA repo docs
git cherry-pick 27a190493   # Phase 2: RICA branding system
git cherry-pick 1b4b4a8a3   # Phase 3: RICA auth & config adapters
git cherry-pick c77934dc0   # Phase 4: RICA Control Plane adapters
git cherry-pick 4bd2936ce   # Phase 5: RICA VSCode extension integration
git cherry-pick d48bd7116   # Phase 6: RICA integration docs & package updates
```

### Step 2 — Phase 7: Token consumption tracking

```bash
git cherry-pick 50074a4f6 ce9d044b9 9a35cd015 4cfc15c62 adb3cce92
git rebase -i HEAD~5    # squash into 1 commit
```

### Step 3 — Phase 8: Environment switching

```bash
git cherry-pick 91bed4bcc a5382ce14
git rebase -i HEAD~2    # squash
```

### Step 4 — Phase 9: GitLab CI build pipeline

```bash
git cherry-pick 2175ba8bd 97950c1ae e387ec45f ... (see full list in A9)
git rebase -i HEAD~20   # squash
```

### Step 5 — Phase 10: Generic KEEPs

```bash
# Verify upstream doesn't have these first
git cherry-pick 9a46ba96a    # autocomplete max-tokens 256
git cherry-pick 2dda59cc6    # debounce 400ms
git cherry-pick 44a6d05fe    # Verilog tree-sitter
```

### Step 6 — Verify build & push

```bash
npm install
npm run build
git push origin rica/refork-v2
```

---

## 5. Verification Checklist

After refork, verify each item:

### Authentication

- [ ] EntraID SSO: login → token refresh → sign-out → re-login
- [ ] Token refresh tự động không cần user action
- [ ] SecretStorage crypto compatibility

### Dynamic Config

- [ ] Property descriptor trap intercept writes trên `controlPlaneClient`
- [ ] Refresh interval 5 min auto-reload config
- [ ] Cache 30s TTL — không duplicate fetch trong cùng cascade
- [ ] Bearer token truyền đúng vào `/api/user-model/configs`
- [ ] Corporate proxy detection: redirect (3xx) + non-JSON content-type
- [ ] Response format: `payload.models` / `payload.config.models` / `payload.data`
- [ ] Provider alias: `rica-proxy` → `continue-proxy`, `databricks` → `openai`
- [ ] `model_id` inject vào BaseLLM sau YAML round-trip
- [ ] `defaultCompletionOptions` pass-through từ backend
- [ ] Context length parsing đúng
- [ ] Login → dynamic models load; Logout → state reset; Re-login → RICA client retained

### RICA Online Documents

- [ ] Indexing vào ES `user_documents`
- [ ] Hybrid search hoạt động
- [ ] Knowledge Base integration

### Token Consumption

- [ ] Ghi từng record
- [ ] Filter theo threshold
- [ ] Auto-switch model khi vượt threshold

### Databricks Provider

- [ ] `databricks` → `openai` mapping qua `normalizeProviderName()` in `core/llm/llms/index.ts`
- [ ] Tool support cho agent mode (heuristic in `core/llm/toolSupport.ts`)
- [ ] Embedding API hoạt động qua OpenAI class

### Build & Branding

- [ ] App name = "RICA"
- [ ] Icon đúng
- [ ] Providers Continue bị ẩn
- [ ] GitLab CI artifact win + linux đầy đủ

### Environment

- [ ] Prod/Non-Prod/Local endpoints chuyển đúng
- [ ] Sign-in UI warning khi chưa đăng nhập

---

## 6. Branch State Reference

### Key SHAs

| Name                  | SHA         | Description                                |
| --------------------- | ----------- | ------------------------------------------ |
| Fork divergence point | `c3360451d` | Điểm tách giữa RICA fork và upstream       |
| Upstream baseline     | `cb273098d` | Commit upstream làm base cho refork-clean  |
| Phase 1               | `8f7085329` | RICA repository documentation              |
| Phase 2               | `27a190493` | RICA branding system                       |
| Phase 3               | `1b4b4a8a3` | RICA authentication & config adapters      |
| Phase 4               | `c77934dc0` | RICA Control Plane adapters & core patches |
| Phase 5               | `4bd2936ce` | RICA VSCode extension integration          |
| Phase 6               | `d48bd7116` | RICA integration docs & package updates    |

### Backup Branches

- `backup/before-parallel-tool-calls`
- `backup/rica-refork-base-20260519-104507`
- `backup/rica-work-before-refork-20260519-110634`
- `rica/refork-base`
- `rica/refork-clean`

---

## 7. Phase 4 Architecture Detail (Dynamic Configuration)

### File Layout

```text
rica/adapters/config/
├── env.ts                 # Environment detection, ControlPlaneEnv, APP_URL
├── modelMapping.ts        # normalizeProviderAlias(), normalizeRicaModel()
├── dynamicConfig.ts       # fetchRicaDynamicConfig(), proxy detection
└── RicaProfileLoader.ts   # loadDynamicAssistant() → AssistantUnrolled

rica/adapters/control-plane/
└── RicaControlPlaneClient.ts  # extends ControlPlaneClient, 30s TTL cache

rica/adapters/core/
└── RicaCore.ts            # Property descriptor trap + auto-refresh

core/ (modified-upstream, minimal):
├── llm/llms/index.ts      # normalizeProviderName() for rica-proxy/databricks
└── llm/toolSupport.ts     # PROVIDER_TOOL_SUPPORT entries
```

### Data Flow

```
VsCodeExtension
  └─ new RicaCore(messenger, ide)
       ├─ super(messenger, ide)  →  creates ConfigHandler + vanilla ControlPlaneClient
       ├─ installControlPlaneClientTrap()
       │    └─ Object.defineProperty(configHandler, "controlPlaneClient", { get/set })
       │         set(newClient) → new RicaControlPlaneClient(newClient.sessionInfoPromise, ide)
       └─ startDynamicConfigAutoRefresh()  →  setInterval(5min)
              └─ invalidateCache() + reloadConfig()

ConfigHandler.cascadeInit()
  └─ controlPlaneClient.listOrganizations()  →  [RicaControlPlaneClient via trap]
       └─ getDynamicAssistant()
            └─ RicaProfileLoader.loadDynamicAssistant()
                 └─ fetchRicaDynamicConfig(ideSettings, fetch, { accessToken })
                      ├─ GET /api/user-model/configs (Bearer token)
                      ├─ Detect redirect / non-JSON → throw with proxy hint
                      └─ normalizeRicaModels(extractModels(payload))

Runtime model instantiation:
  llmFromDescription(desc)
    └─ normalizeProviderName(desc.provider)
         ├─ "rica-proxy"  →  "continue-proxy"  →  ContinueProxy class
         └─ "databricks"  →  "openai"          →  OpenAI class
```

### Key Design Decisions

| Decision                                    | Rationale                                                     |
| ------------------------------------------- | ------------------------------------------------------------- |
| Property descriptor trap (not monkey-patch) | Survives any code path that writes `controlPlaneClient`       |
| 30s TTL cache (not per-request)             | Prevents double-fetch within single cascade                   |
| `redirect: "manual"`                        | Detects Netskope/corporate proxy before reading response body |
| Provider alias in `core/` (not adapter)     | Must happen at class lookup time in `llmFromDescription`      |
| `defaultCompletionOptions` passthrough      | Backend controls temperature/maxTokens per model              |

---

## 8. Open Questions

1. **Parallel tool calls** — upstream đã có chưa? (B.1)
2. **Conversation summary / compaction** — upstream có tương đương chưa? (B.2)
3. **Rule toggle, invokable rules** — upstream đã hỗ trợ chưa? (B.3)
4. **Verilog tree-sitter** — KEEP cho Renesas hay đợi upstream? (B.4) → **Recommend KEEP**
5. **Gemini vision** — upstream có chưa? (A12)
6. **Windows SSH path resolution** — upstream đã fix chưa? (B.6)
7. Verify duplicate `926542f13` vs `a27b5defa` — chỉ giữ 1 → **DROP `926542f13`**
8. **Multi-Agent Orchestration (A11)** — có đưa vào refork không? 15 commits từ Rakibul Haque (Apr-May 2026). Đây là feature mới hoàn toàn, cần quyết định scope.
9. **A1-A3 commits thiếu** — verify xem đã bao hàm trong squash commits trên `rica/refork-clean` chưa (Phase 2: `27a190493`, Phase 3: `1b4b4a8a3`, Phase 4: `c77934dc0`)

---

## 9. Agent Prompt Template

Use this prompt for Continue/agent to execute refork:

```
Bạn là agent thực hiện refork RICA từ upstream Continue. Đọc file
`rica/docs/REFORK_PLAN.md` để có context đầy đủ.

Nhiệm vụ:
1. Đảm bảo đang ở branch `main`, working tree sạch
2. Chạy `git fetch upstream` để lấy upstream mới nhất
3. Tạo branch mới `rica/refork-v2` từ `upstream/main`
4. Thực hiện tuần tự các Step 1–5 trong mục §4
5. Sau mỗi Phase, dừng lại để review trước khi tiếp tục
6. Với Group B, trước khi cherry-pick PHẢI verify với upstream
7. Sau khi hoàn tất: chạy `npm install && npm run build`
8. Push lên `origin/rica/refork-v2` (KHÔNG force-push lên main)

Lưu ý:
- KHÔNG tự ý quyết định DROP/KEEP với Group B mà không hỏi
- KHÔNG force-push lên `main`
- Nếu cherry-pick conflict, dừng lại và báo cáo
```

---

## 10. Related Documents

- [`README.md`](./README.md) — Documentation index
- [`REPO_STANDARD.md`](./REPO_STANDARD.md) — Architecture standard
- [`UPSTREAM_SYNC_CONFLICTS.md`](./UPSTREAM_SYNC_CONFLICTS.md) — Conflict resolution
- [`BUILD_AND_RELEASE.md`](./BUILD_AND_RELEASE.md) — Build guide
