# RICA Upstream Sync Conflicts Guide

> **Status:** Canonical document
> **Last updated:** 2026-06-11
> **Location:** `rica/docs/UPSTREAM_SYNC_CONFLICTS.md`
>
> This document consolidates:
>
> - `rica/vscode/patches/UPSTREAM_SYNC_GUIDE.md`
> - `rica/vscode/patches/UPSTREAM_SYNC_IMPACT_ANALYSIS.md`
> - `rica/vscode/patches/CONTROL_PLANE_ARCHITECTURE.md`
> - `rica/vscode/patches/IMPLEMENTATION_HISTORY.md`
> - `rica/vscode/patches/INTEGRATION_PATCHES.md`

---

## 1. Overview & Compliance Assessment

### Current Compliance Score: 8.5/10

| Criteria                        | Score               | Status                                                                                                                          |
| ------------------------------- | ------------------- | ------------------------------------------------------------------------------------------------------------------------------- |
| Section 5.1 (upstream-managed)  | ⚠️ Documented       | 34 files in `core/` modified + 20 RICA-only files in `core/` — all documented in §3 (incl. the Multi-Agent surface §3.20–§3.36) |
| Section 5.2 (modified-upstream) | ⚠️ Mostly Compliant | 4 files in `extensions/vscode/`, 23 in `gui/src/` (bridge + multi-agent)                                                        |
| Section 5.3 (rica-only)         | ⚠️ Mostly Compliant | All adapters in `rica/adapters/`, GUI in `gui/src/rica/` (1 file misplaced)                                                     |
| Section 8 (Auth Boundary)       | ✅ Fully Compliant  | Control plane in `rica/adapters/control-plane/`                                                                                 |
| Section 8 (Extension Boundary)  | ✅ Fully Compliant  | Core wrapper in `rica/adapters/core/`                                                                                           |
| Dependency Direction            | ✅ Fully Compliant  | rica/ → core/ only (no reverse imports, core/protocol/documents re-exports)                                                     |

---

## 2. File Classification Matrix

### 2.1. Ownership Model

| Category              | Paths                            | Conflict Risk | Strategy                         |
| --------------------- | -------------------------------- | ------------- | -------------------------------- |
| **upstream-managed**  | `core/**`, `packages/**`         | Low           | Accept upstream                  |
| **modified-upstream** | `extensions/vscode/**`, `gui/**` | High          | Manual review + re-apply patches |
| **rica-only**         | `rica/**`                        | None          | Never accept upstream            |

### 2.2. Risk Assessment by File

| File                                                                                                               | Risk         | Prob | Time      | Feature                |
| ------------------------------------------------------------------------------------------------------------------ | ------------ | ---- | --------- | ---------------------- |
| `extensions/vscode/package.json`                                                                                   | 🔴 VERY HIGH | 80%  | 5-10 min  | Branding               |
| `extensions/vscode/src/extension/VsCodeExtension.ts`                                                               | 🔴 HIGH      | 40%  | 15-30 min | Core Init              |
| `core/control-plane/AuthTypes.ts`                                                                                  | 🟠 HIGH      | 30%  | 10 min    | Auth                   |
| `core/llm/llms/index.ts`                                                                                           | 🟡 MEDIUM    | 20%  | 5 min     | DynConfig              |
| `core/llm/toolSupport.ts`                                                                                          | 🟢 LOW       | 10%  | 5 min     | DynConfig              |
| `core/control-plane/client.ts`                                                                                     | 🟡 MEDIUM    | 30%  | 5 min     | TypeFix                |
| `extensions/vscode/src/stubs/WorkOsAuthProvider.ts`                                                                | 🟡 MEDIUM    | 20%  | 5 min     | TypeFix                |
| `extensions/vscode/tsconfig.json`                                                                                  | 🟢 LOW       | 10%  | 2 min     | Build                  |
| `core/protocol/core.ts`                                                                                            | 🟢 LOW       | 15%  | 5 min     | OnlineDocs             |
| `core/protocol/passThrough.ts`                                                                                     | 🟢 LOW       | 15%  | 2 min     | OnlineDocs             |
| `core/protocol/documents.ts`                                                                                       | 🟢 NONE      | 0%   | N/A       | OnlineDocs             |
| `core/index.d.ts`                                                                                                  | 🟢 LOW       | 10%  | 2 min     | OnlineDocs             |
| `core/config/types.ts`                                                                                             | 🟢 LOW       | 10%  | 2 min     | OnlineDocs             |
| `core/config/loadContextProviders.ts`                                                                              | 🟢 LOW       | 15%  | 5 min     | OnlineDocs             |
| `core/llm/autodetect.ts`                                                                                           | 🟢 LOW       | 10%  | 2 min     | OnlineDocs             |
| `gui/src/redux/util/constructMessages.ts`                                                                          | 🟡 MEDIUM    | 25%  | 10 min    | OnlineDocs             |
| `gui/src/components/.../AtMentionDropdown/index.tsx`                                                               | 🟡 MEDIUM    | 25%  | 10 min    | OnlineDocs             |
| `gui/src/components/.../BlockSettingsTopToolbar.tsx`                                                               | 🟢 LOW       | 15%  | 5 min     | OnlineDocs             |
| `gui/src/pages/config/configTabs.tsx`                                                                              | 🟢 LOW       | 10%  | 2 min     | OnlineDocs             |
| `gui/src/util/navigation.ts`                                                                                       | 🟢 LOW       | 10%  | 2 min     | OnlineDocs             |
| `gui/src/redux/util/getBaseSystemMessage.ts`                                                                       | 🟡 MEDIUM    | 30%  | 10 min    | MultiAgent             |
| `gui/src/redux/thunks/streamNormalInput.ts`                                                                        | 🟡 MEDIUM    | 30%  | 10 min    | MultiAgent             |
| `gui/src/components/gui/MultiAgentNudge.tsx`                                                                       | 🟡 MEDIUM    | 20%  | 15 min    | MultiAgent             |
| `gui/src/components/.../LumpToolbar/LumpToolbar.tsx`                                                               | 🟡 MEDIUM    | 30%  | 10 min    | MultiAgent             |
| `gui/src/pages/config/sections/ModelsSection.tsx`                                                                  | 🟢 LOW       | 15%  | 2 min     | Branding               |
| `gui/src/components/modelSelection/ModelSelect.tsx`                                                                | 🟢 LOW       | 15%  | 5 min     | MultiAgent             |
| `gui/src/components/mainInput/InlineErrorMessage.tsx`                                                              | 🟢 LOW       | 10%  | 2 min     | Branding               |
| `gui/src/pages/config/sections/HelpSection.tsx`                                                                    | 🟢 LOW       | 10%  | 2 min     | Branding               |
| `gui/src/pages/config/sections/ToolsSection.tsx`                                                                   | 🟢 LOW       | 10%  | 2 min     | Branding               |
| `gui/src/pages/config/sections/UserSettingsSection.tsx`                                                            | 🟢 LOW       | 10%  | 2 min     | Branding               |
| `gui/src/components/OnboardingCard/.../useOnboardingCard.ts`                                                       | 🟢 LOW       | 10%  | 2 min     | Branding               |
| `gui/src/components/CliInstallBanner.tsx`                                                                          | 🟢 LOW       | 10%  | 2 min     | Branding               |
| `gui/src/components/dialogs/FeedbackDialog.tsx`                                                                    | 🟢 LOW       | 10%  | 2 min     | Branding               |
| `gui/src/forms/AddModelForm.tsx`                                                                                   | 🟢 LOW       | 10%  | 2 min     | Branding               |
| `gui/src/pages/gui/StreamError.tsx`                                                                                | 🟢 LOW       | 10%  | 2 min     | Branding               |
| `gui/src/pages/gui/ToolCallDiv/ToolCallStatusMessage.tsx`                                                          | 🟢 LOW       | 10%  | 2 min     | MultiAgent             |
| `gui/src/pages/gui/ToolCallDiv/FunctionSpecificToolCallDiv.tsx`                                                    | 🟢 LOW       | 10%  | 5 min     | MultiAgent             |
| `extensions/vscode/src/VsCodeIde.ts`                                                                               | 🟢 LOW       | 15%  | 5 min     | OnlineDocs             |
| `extensions/vscode/src/ContinueGUIWebviewViewProvider.ts`                                                          | 🟡 MEDIUM    | 25%  | 5 min     | ViewID                 |
| `extensions/vscode/src/ContinueConsoleWebviewViewProvider.ts`                                                      | 🟡 MEDIUM    | 25%  | 5 min     | ViewID                 |
| `extensions/vscode/src/commands.ts`                                                                                | 🟡 MEDIUM    | 30%  | 10 min    | ViewID                 |
| `core/tools/builtIn.ts` + `index.ts` + `definitions/index.ts` + `callTool.ts`                                      | 🟡 MEDIUM    | 30%  | 10 min    | MultiAgent             |
| `core/llm/streamChat.ts`                                                                                           | 🟡 MEDIUM    | 30%  | 10 min    | MultiAgent             |
| `core/llm/index.ts` + `core/config/load.ts`                                                                        | 🟡 MEDIUM    | 25%  | 10 min    | MultiAgent             |
| `core/util/history.ts`                                                                                             | 🟢 LOW       | 15%  | 5 min     | MultiAgent             |
| `core/data/devdataSqlite.ts`                                                                                       | 🟢 LOW       | 15%  | 5 min     | MultiAgent             |
| `core/util/paths.ts`                                                                                               | 🟠 HIGH      | 30%  | 5 min     | Paths                  |
| `core/control-plane/env.ts` + `auth/index.ts`                                                                      | 🟠 HIGH      | 30%  | 10 min    | Auth/Env               |
| `core/config/profile/doLoadConfig.ts`                                                                              | 🟠 HIGH      | 30%  | 10 min    | DynConfig              |
| `core/config/ConfigHandler.ts`                                                                                     | 🟢 LOW       | 15%  | 5 min     | DynConfig              |
| `core/context/providers/index.ts`                                                                                  | 🟢 LOW       | 15%  | 5 min     | Severs                 |
| `core/util/posthog.ts` + `TeamAnalytics.ts` + `TokensBatchingService.ts` + `sentry/constants.ts`                   | 🟢 LOW       | 10%  | 10 min    | Telemetry              |
| `core/llm/openaiTypeConverters.ts`                                                                                 | 🟢 LOW       | 15%  | 5 min     | Proxy                  |
| `core/llm/countTokens.ts`                                                                                          | 🟢 LOW       | 20%  | 5 min     | Diagnostic             |
| `core/llm/defaultSystemMessages.ts`                                                                                | 🟢 LOW       | 10%  | 2 min     | Identity               |
| `core/package.json`                                                                                                | 🟢 LOW       | 15%  | 2 min     | Telemetry              |
| `core/config/multiAgentTier.ts` + `multiAgentTier.test.ts` + `core/tools/multiagent/**` + spawn/analyze defs+impls | 🟢 NONE      | 0%   | N/A       | MultiAgent (RICA-only) |
| `rica/**`                                                                                                          | 🟢 NONE      | 0%   | N/A       | Protected              |

### 2.3. Files by Feature Module

| Feature                      | Files Modified                                                                                                    | Section     |
| ---------------------------- | ----------------------------------------------------------------------------------------------------------------- | ----------- |
| **Branding**                 | `package.json`, icons, manifest                                                                                   | §3.1        |
| **Authentication (EntraID)** | `AuthTypes.ts`, `VsCodeExtension.ts`                                                                              | §3.2, §3.3  |
| **Control Plane**            | `RicaCore.ts` (rica-only)                                                                                         | §4          |
| **Dynamic Configuration**    | `llms/index.ts`, `toolSupport.ts`                                                                                 | §3.4, §3.5  |
| **View ID Namespacing**      | `ContinueGUIWebviewViewProvider.ts`, `ContinueConsoleWebviewViewProvider.ts`, `commands.ts`, `VsCodeExtension.ts` | §3.19       |
| **Build**                    | `tsconfig.json`                                                                                                   | §3.6        |
| **Upstream Fixes**           | `client.ts`, `WorkOsAuthProvider.ts`                                                                              | §3.7, §3.8  |
| **Multi-Agent**              | `getBaseSystemMessage.ts`, `streamNormalInput.ts`, `MultiAgentNudge.tsx`,                                         | §3.20–§3.23 |
|                              | `LumpToolbar.tsx`, `ModelSelect.tsx`, `ToolCallStatusMessage.tsx`,                                                |             |
|                              | `FunctionSpecificToolCallDiv.tsx`                                                                                 |             |
| **GUI Branding**             | `ModelsSection.tsx`, `HelpSection.tsx`, `ToolsSection.tsx`, `UserSettingsSection.tsx`,                            | §5 (batch)  |
|                              | `useOnboardingCard.ts`, `CliInstallBanner.tsx`, `FeedbackDialog.tsx`,                                             |             |
|                              | `AddModelForm.tsx`, `InlineErrorMessage.tsx`, `StreamError.tsx`                                                   |             |
| **Online Documents**         | `protocol/core.ts`, `passThrough.ts`, `loadContextProviders.ts`,                                                  | §3.9–§3.18  |
|                              | `autodetect.ts`, `index.d.ts`, `types.ts`, `constructMessages.ts`,                                                |             |
|                              | `VsCodeIde.ts`, `AtMentionDropdown/index.tsx`,                                                                    |             |
|                              | `BlockSettingsTopToolbar.tsx`, `configTabs.tsx`, `navigation.ts`                                                  |             |
| **Telemetry Severs**         | `posthog.ts`, `TeamAnalytics.ts`, `TokensBatchingService.ts`, `sentry/constants.ts`, `core/package.json`          | §3.29       |
| **Diagnostic**               | `countTokens.ts`                                                                                                  | §3.36       |

---

## 3. Conflict Hotspots — Per-File Playbooks

> **Template for adding new files:**
>
> ````markdown
> ### 3.X. `path/to/file.ts` — 🔴/🟠/🟡 RISK LEVEL
>
> **Feature:** Which RICA feature requires this change
> **Why it conflicts:** Why upstream might change this file
>
> **RICA changes:**
>
> - Change 1
> - Change 2
>
> **Diff:**
>
> ```diff
> - upstream code
> + RICA code
> ```
> ````
>
> **Resolution strategy:**
>
> ```bash
> # Step-by-step commands
> ```
>
> **Decision tree:**
> | Upstream Change | Action |
> |-----------------|--------|
> | Scenario 1 | Action 1 |
>
> **Verification:**
>
> - [ ] Check 1
> - [ ] Check 2
>
> ```
>
> ```

---

### 3.1. `extensions/vscode/package.json` — 🔴 VERY HIGH RISK

**Feature:** Branding
**Why it conflicts:** Upstream frequently adds commands, settings, menus, activation events.
**RICA changes:**

- Name: `"rica"` (was "continue")
- Display name: `"RICA"`
- Publisher: `"Rica"`
- Icon: `"media/icon-rica-small.png"`
- Command titles: "RICA: ..." (was "Continue: ...")
- Repository URLs: GitLab

**Resolution strategy (AUTOMATED):**

```bash
# Step 1: Accept upstream changes
git checkout --theirs extensions/vscode/package.json

# Step 2: Run RICA branding normalization
node rica/scripts/generate-manifest.js

# Step 3: Validate branding
node rica/scripts/check-branding.js
```

**Verification:**

- [ ] `"name": "rica"`
- [ ] `"displayName": "RICA"`
- [ ] `"publisher": "Rica"`
- [ ] Command categories: `"RICA"`
- [ ] Repository URLs: GitLab

---

### 3.2. `extensions/vscode/src/extension/VsCodeExtension.ts` — 🔴 HIGH RISK

**Feature:** Core Initialization, Authentication, View ID Registration
**Why it conflicts:** Core initialization point, upstream may refactor activation flow.

**RICA changes (6 locations):**

```diff
// Line ~8: Import RicaCore
- import { Core } from "core/core";
+ import { RicaCore } from "../../../../rica/adapters/core/RicaCore";

// Line ~82: Type declaration
- private core: Core;
+ private core: RicaCore;

// Line ~330: Instantiation
- this.core = new Core(inProcessMessenger, this.ide);
+ this.core = new RicaCore(inProcessMessenger, this.ide);

// Lines ~38-39: RICA auth imports
+ import {
+   RicaAuthProvider,
+   getRicaControlPlaneSessionInfo,
+ } from "../../../../rica/vscode/integration";

// Line ~305: View registration uses static viewType
- vscode.window.registerWebviewViewProvider("continue.continueGUIView", sidebar, {...});
+ vscode.window.registerWebviewViewProvider(ContinueGUIWebviewViewProvider.viewType, sidebar, {...});

// Line ~458: Console view registration uses static viewType
- vscode.window.registerWebviewViewProvider("continue.continueConsoleView", consoleProvider);
+ vscode.window.registerWebviewViewProvider(ContinueConsoleWebviewViewProvider.viewType, consoleProvider);
```

**Resolution strategy (MANUAL):**

```bash
# Step 1: Accept upstream changes
git checkout --theirs extensions/vscode/src/extension/VsCodeExtension.ts

# Step 2: Re-apply RICA changes manually (see diff above)

# Step 3: Test compilation
npm run tsc:check
```

**Decision tree:**

| Upstream Change                    | Action                                          |
| ---------------------------------- | ----------------------------------------------- |
| No changes to Core init            | Keep RICA version as-is                         |
| Minor changes (new methods)        | Merge upstream + keep RicaCore import           |
| Major refactor of Core constructor | Update RicaCore constructor + trap installation |
| controlPlaneClient becomes private | Move trap to a different injection point        |
| ConfigHandler refactored           | Update property descriptor target               |

**Verification:**

- [ ] RicaCore imported correctly
- [ ] RicaAuthProvider initialized
- [ ] Property descriptor trap intercepts `controlPlaneClient` writes
- [ ] Dynamic config refresh interval starts
- [ ] Extension loads without errors

---

### 3.3. `core/control-plane/AuthTypes.ts` — 🟠 HIGH RISK

**Feature:** Authentication (EntraID)
**Why it conflicts:** RICA adds EntraID types to upstream enum and type unions.

**RICA changes (enum + type unions only, no custom fields):**

```diff
 export enum AuthType {
   WorkOsProd = "continue",
   WorkOsStaging = "continue-staging",
+  RicaProd = "Rica",
+  RicaStaging = "rica-staging",
   OnPrem = "on-prem",
 }

 export interface HubSessionInfo {
   AUTH_TYPE:
     | AuthType.WorkOsProd
     | AuthType.WorkOsStaging
+    | AuthType.RicaProd
+    | AuthType.RicaStaging;
   accessToken: string;
   // NOTE: No custom RICA fields here — token is used as-is
```

**Resolution strategy:**

```bash
# Accept upstream, re-add RICA enum values + union types
git checkout --theirs core/control-plane/AuthTypes.ts
# Add: RicaProd = "Rica", RicaStaging = "rica-staging" to AuthType enum
# Add: | AuthType.RicaProd | AuthType.RicaStaging to HubSessionInfo.AUTH_TYPE
# Add: | AuthType.RicaProd | AuthType.RicaStaging to HubEnv.AUTH_TYPE
```

**Decision tree:**

| Upstream Change              | Action                                     |
| ---------------------------- | ------------------------------------------ |
| Adds new enum values         | Accept upstream + keep RICA values         |
| Refactors to string literals | Adapt RICA auth types to match new pattern |
| Removes AUTH_TYPE union      | Find new discriminator for RICA auth types |

**Verification:**

- [ ] TypeScript compiles
- [ ] EntraID auth works (both prod and staging)

---

### 3.4. `core/llm/llms/index.ts` — 🟡 MEDIUM RISK

**Feature:** Dynamic Configuration (Provider Alias: `databricks` → `openai`)
**Why it conflicts:** Upstream adds/removes LLM providers or refactors the provider resolution logic.

**RICA changes:**

- Added `normalizeProviderName()` function that maps `databricks` → `openai`
- Called before provider class lookup in `llmFromDescription()` and `llmFromProviderAndOptions()`
- **Static import of `RicaProxy` + entry in the `LLMClasses` array.** This registers the `rica-proxy` provider class so `getModelClass` can resolve it.

> **⚠️ Do not "deduplicate" this static import.** `RicaCore.registerRicaProviders()`
> ALSO pushes `RicaProxy` into `LLMClasses` at runtime, so it looks redundant for the
> VS Code extension (which runs core in-process via `RicaCore`). **It is NOT redundant for
> the core binary** (`binary/src/index.ts` constructs a plain `new Core()`, which never
> calls `registerRicaProviders()`), used by the JetBrains/IntelliJ integration. Removing
> the static import would leave `rica-proxy` unregistered on the binary path and every
> RICA model would fail to instantiate there. Keep both registrations.

**Diff:**

```diff
+function normalizeProviderName(providerName: string): string {
+  if (providerName === "databricks") {
+    return "openai";
+  }
+  return providerName;
+}
+
 export async function llmFromDescription(...) {
+  const normalizedProvider = normalizeProviderName(desc.provider);
+  const normalizedDesc = normalizedProvider === desc.provider
+    ? desc
+    : { ...desc, provider: normalizedProvider };
-  const cls = LLMClasses.find((llm) => llm.providerName === desc.provider);
+  const cls = LLMClasses.find((llm) => llm.providerName === normalizedDesc.provider);
```

**Resolution strategy:**

```bash
git checkout --theirs core/llm/llms/index.ts
# Re-add normalizeProviderName() function (only databricks → openai)
# Re-add normalization calls in llmFromDescription() and llmFromProviderAndOptions()
```

**Decision tree:**

| Upstream Change                  | Action                                        |
| -------------------------------- | --------------------------------------------- |
| Adds new providers to LLMClasses | Accept upstream, keep normalizeProviderName   |
| Refactors llmFromDescription()   | Re-apply normalization at new provider lookup |
| Adds native databricks provider  | Remove RICA alias entirely from this file     |

**Verification:**

- [ ] Models with `provider: "rica-proxy"` resolve to RicaProxy class (runtime-registered)
- [ ] Models with `provider: "databricks"` resolve to OpenAI class
- [ ] No imports from `rica/` in this file

---

### 3.5. `core/llm/toolSupport.ts` — 🟢 LOW RISK

**Feature:** Dynamic Configuration (Tool Support Heuristics)
**Why it conflicts:** Upstream adds new provider tool support entries.

**RICA changes:**

- Added `"rica-proxy"` entry that delegates to `continue-proxy` heuristic
- Added `"databricks"` entry with heuristic for common Databricks-hosted models

**Diff:**

```diff
 export const PROVIDER_TOOL_SUPPORT: Record<string, (model: string) => boolean> = {
   "continue-proxy": (model) => { ... },
+  "rica-proxy": (model) => PROVIDER_TOOL_SUPPORT["continue-proxy"](model),
+  databricks: (model) => {
+    const lower = model.toLowerCase();
+    return lower.includes("llama") || lower.includes("gpt") ||
+           lower.includes("claude") || lower.includes("mixtral") ||
+           lower.includes("qwen");
+  },
   anthropic: (model) => { ... },
```

**Resolution strategy:**

```bash
# Accept upstream, then re-add the two entries after "continue-proxy"
git checkout --theirs core/llm/toolSupport.ts
# Add "rica-proxy" and "databricks" entries to PROVIDER_TOOL_SUPPORT
```

**Verification:**

- [ ] Agent mode works with Databricks models
- [ ] Tool calling works with rica-proxy models

---

### 3.6. `extensions/vscode/tsconfig.json` — 🟢 LOW RISK

**Feature:** RICA compilation coverage
**Why it conflicts:** Upstream may add new include paths or change compiler options.

**RICA changes:**

- Added `"../../rica/**/*.ts"` to `include` array

**Resolution strategy:**

```bash
# Accept upstream, re-add rica include
git checkout --theirs extensions/vscode/tsconfig.json
# Add "../../rica/**/*.ts" to the "include" array
```

**Verification:**

- [ ] `npx tsc --noEmit --project extensions/vscode/tsconfig.json` shows no rica/ errors

---

### 3.7. `core/control-plane/client.ts` — 🟡 MEDIUM RISK

**Feature:** Upstream Compatibility Fix (node-fetch types)
**Why it conflicts:** Upstream may update node-fetch version or switch to native fetch.

**RICA changes:**

- Replaced `import { RequestInit, Response } from "node-fetch"` with local type aliases (node-fetch v3 does not re-export these)
- Fixed `stateResponse.statusText` → `stateResponse.status` (node-fetch Response may not populate statusText)

**Diff:**

```diff
-import fetch, { RequestInit, Response } from "node-fetch";
+import fetch from "node-fetch";
+
+type RequestInit = NonNullable<Parameters<typeof fetch>[1]>;
+type Response = Awaited<ReturnType<typeof fetch>>;

-  `Failed to fetch state from remote agent: ${stateResponse.statusText}`,
+  `Failed to fetch state from remote agent: ${stateResponse.status}`,
```

**Resolution strategy:**

```bash
# If upstream updates node-fetch or switches to native fetch:
# - If native fetch: remove the type aliases entirely
# - If node-fetch stays: check if types are re-exported in the new version
git checkout --theirs core/control-plane/client.ts
# Re-check if RequestInit/Response are importable; if not, re-add type aliases
```

**Decision tree:**

| Upstream Change                   | Action                   |
| --------------------------------- | ------------------------ |
| Upgrades node-fetch (types fixed) | Remove RICA type aliases |
| Switches to native fetch          | Remove RICA type aliases |
| No change to node-fetch           | Keep RICA type aliases   |

**Verification:**

- [ ] TypeScript compiles without errors in `core/control-plane/client.ts`
- [ ] Remote session fetch calls work (if feature enabled)

---

### 3.8. `extensions/vscode/src/stubs/WorkOsAuthProvider.ts` — 🟡 MEDIUM RISK

**Feature:** Upstream Compatibility Fix (node-fetch URL handling)
**Why it conflicts:** Upstream may refactor auth refresh logic or switch HTTP libraries.

**RICA changes:**

- Added `.toString()` to `new URL(...)` passed to `fetch()` (node-fetch does not accept URL objects in all environments)

**Diff:**

```diff
-    const response = await fetch(
-      new URL("/auth/refresh", controlPlaneEnv.CONTROL_PLANE_URL),
+    const response = await fetch(
+      new URL("/auth/refresh", controlPlaneEnv.CONTROL_PLANE_URL).toString(),
```

**Resolution strategy:**

```bash
# If upstream changes auth refresh logic:
git checkout --theirs extensions/vscode/src/stubs/WorkOsAuthProvider.ts
# Re-apply .toString() on any URL objects passed to fetch()
```

**Decision tree:**

| Upstream Change          | Action                                        |
| ------------------------ | --------------------------------------------- |
| Switches to native fetch | Remove .toString() (native fetch accepts URL) |
| Keeps node-fetch         | Keep .toString()                              |
| Refactors auth refresh   | Re-apply .toString() at new location          |

**Verification:**

- [ ] Token refresh works (observe in Developer Console: "Token refresh completed successfully")

---

### 3.9. `core/protocol/core.ts` — 🟢 LOW RISK

**Feature:** Online Documents (protocol message types)
**Why it conflicts:** Upstream adds new protocol message entries to `ToCoreFromIdeOrWebviewProtocol`.

**RICA changes (5 entries at END of type, clearly marked):**

```diff
+  // RICA Online Documents
+  "ricaDocs/list": [undefined, DocumentListResponse];
+  "ricaDocs/refresh": [undefined, DocumentListResponse];
+  "ricaDocs/query": [ElasticsearchQueryRequest, ElasticsearchQueryResponse];
+
+  // RICA Knowledge Base
+  "knowledgeBase/list": [
+    { page?: number; limit?: number } | undefined,
+    KnowledgeBaseListResponse,
+  ];
+  "knowledgeBase/refresh": [
+    { page?: number; limit?: number } | undefined,
+    KnowledgeBaseListResponse,
+  ];
```

Also adds import at top:

```diff
+import {
+  DocumentListResponse,
+  ElasticsearchQueryRequest,
+  ElasticsearchQueryResponse,
+  KnowledgeBaseListResponse,
+} from "./documents";
```

**Note:** `ToCoreFromIdeOrWebviewProtocol` is a `type` (not `interface`), so module augmentation is impossible. These entries must stay in this file for type safety with `messenger.on()`.

**Resolution strategy:**

```bash
git checkout --theirs core/protocol/core.ts
# Re-add import from "./documents" at top
# Re-add 5 RICA entries at END of ToCoreFromIdeOrWebviewProtocol (before closing };)
```

**Verification:**

- [ ] `npx tsc --noEmit --project core/tsconfig.json` passes
- [ ] `npx tsc --noEmit --project extensions/vscode/tsconfig.json` passes

---

### 3.10. `core/protocol/passThrough.ts` — 🟢 LOW RISK

**Feature:** Online Documents (webview→core message forwarding)
**Why it conflicts:** Upstream adds new passthrough routes.

**RICA changes (5 entries at END of array):**

```diff
     "models/fetch",
+    // RICA Online Documents
+    "ricaDocs/list",
+    "ricaDocs/refresh",
+    "ricaDocs/query",
+    "knowledgeBase/list",
+    "knowledgeBase/refresh",
   ];
```

**Resolution strategy:**

```bash
git checkout --theirs core/protocol/passThrough.ts
# Re-add 5 RICA entries at end of WEBVIEW_TO_CORE_PASS_THROUGH array (before ];)
```

**Verification:**

- [ ] RICA docs popover can list/refresh documents from GUI

---

### 3.11. `core/config/loadContextProviders.ts` — 🟢 LOW RISK

**Feature:** Online Documents (auto-instantiate rica-docs context provider)
**Why it conflicts:** Upstream may refactor context provider loading logic.

**RICA changes (8 lines, between existing provider blocks):**

```diff
   if (hasDocs && !providers?.some((cp) => cp.description.title === "docs")) {
     providers.push(new DocsContextProvider({}));
   }

+  // RICA: Always include rica-docs context provider if registered
+  if (!providers?.some((cp) => cp.description.title === "rica-docs")) {
+    const ricaDocsCls = contextProviderClassFromName(
+      "rica-docs" as any,
+    ) as any;
+    if (ricaDocsCls) {
+      providers.push(new ricaDocsCls({}));
+    }
+  }
+
   // @problems and @terminal are not supported in jetbrains
```

**Resolution strategy:**

```bash
git checkout --theirs core/config/loadContextProviders.ts
# Re-add 8-line block after DocsContextProvider instantiation
```

**Decision tree:**

| Upstream Change                        | Action                                        |
| -------------------------------------- | --------------------------------------------- |
| Keeps DocsContextProvider pattern      | Add RICA block after it (same pattern)        |
| Refactors to plugin-based loading      | Register rica-docs via plugin mechanism       |
| Removes contextProviderClassFromName() | Use direct import from rica/ (with type cast) |

**Verification:**

- [ ] `@rica-docs` appears in context provider list without explicit config

---

### 3.12. `core/llm/autodetect.ts` — 🟢 LOW RISK

**Feature:** Online Documents (image support for rica-proxy)
**Why it conflicts:** Upstream adds/removes providers from capability arrays.

**RICA changes (1 entry in PROVIDER_SUPPORTS_IMAGES array):**

```diff
 const PROVIDER_SUPPORTS_IMAGES = [
   "openai",
   "anthropic",
   ...
+  "rica-proxy",
 ];
```

**Resolution strategy:**

```bash
git checkout --theirs core/llm/autodetect.ts
# Re-add "rica-proxy" to PROVIDER_SUPPORTS_IMAGES array
```

**Verification:**

- [ ] Images from rica-docs context provider reach the LLM (not stripped by compileChatMessages)

---

### 3.13. `core/index.d.ts` + `core/config/types.ts` — 🟢 LOW RISK

**Feature:** Online Documents (image URLs on ContextItem)
**Why it conflicts:** Upstream adds new properties to ContextItem interface.

**RICA changes (1 optional property in each file):**

```diff
 export interface ContextItem {
   // ...existing properties...
   hidden?: boolean;
+  imageUrls?: string[];
 }
```

**Resolution strategy:**

```bash
git checkout --theirs core/index.d.ts core/config/types.ts
# Re-add imageUrls?: string[] to ContextItem interface in both files
```

**Decision tree:**

| Upstream Change                       | Action                                   |
| ------------------------------------- | ---------------------------------------- |
| Adds own image support to ContextItem | Use upstream's field name, remove RICA's |
| No image support added                | Keep `imageUrls?: string[]`              |
| ContextItem renamed/refactored        | Add `imageUrls` to the new interface     |

**Verification:**

- [ ] TypeScript compiles
- [ ] Context items with images pass imageUrls through pipeline

---

### 3.14. `gui/src/redux/util/constructMessages.ts` — 🟡 MEDIUM RISK

**Feature:** Online Documents (image parts in LLM messages)
**Why it conflicts:** Upstream may refactor message construction logic (actively maintained).

**RICA changes (~10 lines in context item → message part conversion):**

```diff
+const ctxItemParts: MessagePart[] = [];
+for (const ctxItem of item.contextItems) {
+  if (ctxItem.content.trim()) {
+    ctxItemParts.push({ type: "text", text: `${ctxItem.content}\n` } as TextMessagePart);
+  }
+  if (ctxItem.imageUrls && ctxItem.imageUrls.length > 0) {
+    for (const url of ctxItem.imageUrls) {
+      ctxItemParts.push({ type: "imageUrl", imageUrl: { url } } as ImageMessagePart);
+    }
+  }
+}
```

**Resolution strategy:**

```bash
git checkout --theirs gui/src/redux/util/constructMessages.ts
# Find context item → message part conversion section
# Re-add imageUrls handling alongside existing text content logic
```

**Decision tree:**

| Upstream Change                         | Action                                        |
| --------------------------------------- | --------------------------------------------- |
| Keeps same message construction pattern | Re-apply imageUrls logic at same location     |
| Adds native multimodal context support  | Use upstream's image mechanism, remove RICA's |
| Refactors to separate builder functions | Add imageUrls handling in new builder         |

**Verification:**

- [ ] Context items with imageUrls produce ImageMessagePart in LLM messages
- [ ] Text-only context items still work normally

---

### 3.15. `extensions/vscode/src/VsCodeIde.ts` — 🟢 LOW RISK

**Feature:** Online Documents (open binary/image files in VSCode)
**Why it conflicts:** Upstream may change `openFile` method signature.

**RICA changes (~7 lines in openFile method):**

```diff
 async openFile(fileUri: string): Promise<void> {
+  const uri = vscode.Uri.parse(fileUri);
+  const ext = uri.path.split(".").pop()?.toLowerCase();
+  const binaryExtensions = ["png", "jpg", "jpeg", "gif", "webp", "bmp", "ico", "svg", "pdf"];
+  if (ext && binaryExtensions.includes(ext)) {
+    await vscode.commands.executeCommand("vscode.open", uri);
+  } else {
     await this.ideUtils.openFile(uri);
+  }
 }
```

**Resolution strategy:**

```bash
git checkout --theirs extensions/vscode/src/VsCodeIde.ts
# Re-add binary extension detection in openFile method
```

**Verification:**

- [ ] Clicking image context items opens image preview in VSCode (not "binary file" error)
- [ ] Clicking text files still opens in text editor

---

### 3.16. `gui/src/components/mainInput/AtMentionDropdown/index.tsx` — 🟡 MEDIUM RISK

**Feature:** Online Documents (page input after @rica-docs selection)
**Why it conflicts:** Core @mention dropdown, upstream actively maintains for new context providers.

**RICA changes (~20 lines: 1 import, 3 state, 5 selectItem intercept, 8 render):**

```diff
+import { RicaDocsPageInput } from "../../../rica/components/RicaDocsPageInput";

+  const [ricaDocsPageInput, setRicaDocsPageInput] = useState<ComboBoxItem | undefined>(undefined);

   // In selectItem(), before generic command dispatch:
+    const itemWithProvider = item as any;
+    if (itemWithProvider.providerTitle === "rica-docs") {
+      setRicaDocsPageInput(item);
+      return;
+    }

   // In render, between querySubmenuItem and default list:
+      ) : ricaDocsPageInput ? (
+        <RicaDocsPageInput
+          item={ricaDocsPageInput}
+          onSubmit={({ item, query, label }) => {
+            props.command({ ...item, itemType: item.type, query, label });
+          }}
+          onCancel={() => setRicaDocsPageInput(undefined)}
+        />
```

**Resolution strategy:**

```bash
git checkout --theirs gui/src/components/mainInput/AtMentionDropdown/index.tsx
# Re-add import of RicaDocsPageInput from "../../../rica/components/RicaDocsPageInput"
# Re-add ricaDocsPageInput state + selectItem intercept + render branch
```

**Decision tree:**

| Upstream Change                | Action                                          |
| ------------------------------ | ----------------------------------------------- |
| Refactors selectItem logic     | Re-apply providerTitle check at new location    |
| Changes render structure       | Re-add RicaDocsPageInput branch in new JSX tree |
| Adds native submenu page input | Use upstream's mechanism, remove RICA's         |

**Verification:**

- [ ] Selecting a rica-docs item in @mention shows page input
- [ ] Enter submits, Escape cancels
- [ ] Other context providers still work normally

---

### 3.17. `gui/src/components/mainInput/Lump/LumpToolbar/BlockSettingsTopToolbar.tsx` — 🟢 LOW RISK

**Feature:** Online Documents (toolbar popover button)
**Why it conflicts:** Upstream may add/remove toolbar buttons.

**RICA changes (~16 lines: import + button + popover render):**

```diff
+import { RicaDocsPopover } from "./RicaDocsPopover";  // 1-line bridge re-export

+  const [showRicaDocs, setShowRicaDocs] = useState(false);

+  {/* RICA Documents popover button */}
+  <div className="relative">
+    <HeaderButtonWithToolTip text="RICA Documents" onClick={() => setShowRicaDocs(!showRicaDocs)}>
+      <DocumentTextIcon ... />
+    </HeaderButtonWithToolTip>
+    {showRicaDocs && <RicaDocsPopover onClose={() => setShowRicaDocs(false)} />}
+  </div>
```

**Resolution strategy:**

```bash
git checkout --theirs gui/src/components/mainInput/Lump/LumpToolbar/BlockSettingsTopToolbar.tsx
# Re-add RicaDocsPopover import (bridge file at same directory)
# Re-add state + button + popover render in toolbar
```

**Verification:**

- [ ] Document icon button appears in input toolbar
- [ ] Clicking shows RICA docs popover with search/tabs/page input

---

### 3.18. `gui/src/pages/config/configTabs.tsx` + `gui/src/util/navigation.ts` — 🟢 LOW RISK

**Feature:** Online Documents (settings tab registration)
**Why it conflicts:** Upstream may add/remove/reorder config tabs.

**RICA changes in configTabs.tsx (~14 lines):**

```diff
+import { RicaDocsSection } from "./sections/RicaDocsSection";  // bridge re-export

+      {
+        id: "rica-docs",
+        label: "RICA Docs",
+        component: (
+          <ConfigSection>
+            <RicaDocsSection />
+          </ConfigSection>
+        ),
+        icon: <DocumentTextIcon className="xs:h-4 xs:w-4 h-3 w-3 flex-shrink-0" />,
+      },
```

**RICA changes in navigation.ts (2 lines):**

```diff
+  | "rica-docs"
+  RICA_DOCS: buildConfigRoute("rica-docs"),
```

**Resolution strategy:**

```bash
git checkout --theirs gui/src/pages/config/configTabs.tsx gui/src/util/navigation.ts
# Re-add RicaDocsSection import + tab entry in configTabs.tsx
# Re-add "rica-docs" to ConfigTab type + RICA_DOCS route in navigation.ts
```

**Verification:**

- [ ] "RICA Docs" tab appears in settings sidebar
- [ ] Tab shows documents and knowledge bases with search

---

### 3.19. View ID Namespacing — 🟡 MEDIUM RISK

**Feature:** Branding (VS Code view provider registration must match manifest-declared IDs)
**Why it conflicts:** Upstream may rename views, add new views, or refactor view registration.

**Affected files (4):**

| File                                                          | Change                                                                                                                               |
| ------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------ |
| `extensions/vscode/src/ContinueGUIWebviewViewProvider.ts`     | `viewType = "rica.ricaGUIView"`                                                                                                      |
| `extensions/vscode/src/ContinueConsoleWebviewViewProvider.ts` | `` viewType = `${EXTENSION_NAME}.ricaConsoleView` ``                                                                                 |
| `extensions/vscode/src/commands.ts`                           | `endsWith("ricaGUIView")`, `cmd("ricaGUIView.focus")`, panel title "RICA"                                                            |
| `extensions/vscode/src/extension/VsCodeExtension.ts`          | Uses static `ContinueGUIWebviewViewProvider.viewType` and `ContinueConsoleWebviewViewProvider.viewType` instead of hardcoded strings |

**Why these changes are required:**

VS Code requires that the view provider ID registered via `vscode.window.registerWebviewViewProvider(id, provider)` matches EXACTLY with the view ID declared in `package.json` `contributes.views`. Since `generate-manifest.js` renames all view IDs from `continue.*` → `rica.*`, the source code must also use `rica.*` IDs at runtime.

**Diff (ContinueGUIWebviewViewProvider.ts):**

```diff
-  public static readonly viewType = "continue.continueGUIView";
+  public static readonly viewType = "rica.ricaGUIView";
```

**Diff (ContinueConsoleWebviewViewProvider.ts):**

```diff
-  public static readonly viewType = "continue.continueConsoleView";
+  public static readonly viewType = `${EXTENSION_NAME}.ricaConsoleView`;
```

**Diff (commands.ts):**

```diff
-  .filter((tab) => tab.label?.endsWith("continueGUIView"))
+  .filter((tab) => tab.label?.endsWith("ricaGUIView"))

-  await vscode.commands.executeCommand("rica.continueGUIView.focus");
+  await vscode.commands.executeCommand("rica.ricaGUIView.focus");

-  title: "Continue",
+  title: "RICA",
```

**Diff (VsCodeExtension.ts):**

```diff
-  vscode.window.registerWebviewViewProvider("rica.ricaGUIView", sidebar, {...});
+  vscode.window.registerWebviewViewProvider(ContinueGUIWebviewViewProvider.viewType, sidebar, {...});

-  vscode.window.registerWebviewViewProvider("rica.ricaConsoleView", consoleProvider);
+  vscode.window.registerWebviewViewProvider(ContinueConsoleWebviewViewProvider.viewType, consoleProvider);
```

**Resolution strategy:**

```bash
git checkout --theirs extensions/vscode/src/ContinueGUIWebviewViewProvider.ts
git checkout --theirs extensions/vscode/src/ContinueConsoleWebviewViewProvider.ts
git checkout --theirs extensions/vscode/src/commands.ts
git checkout --theirs extensions/vscode/src/extension/VsCodeExtension.ts

# Re-apply view ID changes:
# 1. ContinueGUIWebviewViewProvider.viewType = "rica.ricaGUIView"
# 2. ContinueConsoleWebviewViewProvider.viewType = `${EXTENSION_NAME}.ricaConsoleView`
# 3. commands.ts: ricaGUIView references + "RICA" panel title
# 4. VsCodeExtension.ts: use static viewType references
```

**Decision tree:**

| Upstream Change                                       | Action                                                                                                    |
| ----------------------------------------------------- | --------------------------------------------------------------------------------------------------------- |
| Renames view IDs                                      | Update RICA IDs to match new pattern (rica.newName) and update `generate-manifest.js`                     |
| Adds new views                                        | Add RICA namespace for new views in `generate-manifest.js` (no source change needed if using static refs) |
| Refactors registration to use class.viewType          | Already done on RICA side — accept upstream                                                               |
| Changes from `registerWebviewViewProvider` to new API | Update registration calls to match + keep RICA IDs                                                        |

**Verification:**

- [ ] Sidebar panel loads (not perpetually spinning)
- [ ] Console panel loads
- [ ] `rica.ricaGUIView.focus` command works
- [ ] Full-screen panel opens with title "RICA"

### 3.20. `gui/src/redux/util/getBaseSystemMessage.ts` — 🟡 MEDIUM RISK

**Feature:** Multi-Agent (RICA identity prefix + multi-agent system messages)
**Why it conflicts:** Upstream actively maintains system message construction for chat/agent modes.

**RICA changes (2 imports + 2 RICA_IDENTITY prefixes + multi-agent orchestrator messages):**

```diff
+import {
+  RICA_IDENTITY,
+} from "core/llm/defaultSystemMessages";
+import {
+  DEFAULT_MULTI_AGENT_SYSTEM_MESSAGE,
+  SEQUENTIAL_SYSTEM_MESSAGE,
+  PARALLEL_SYSTEM_MESSAGE,
+  ITERATIVE_SYSTEM_MESSAGE,
+  COLLABORATIVE_SYSTEM_MESSAGE,
+} from "core/llm/multiAgentSystemMessage";

 // In multi-agent mode return:
+    return `${RICA_IDENTITY}\n\n${baseMessage}`;

 // In chat/agent/plan mode return:
+  return `${RICA_IDENTITY}\n\n${baseMessage}`;
```

Also adds:

- `WORKFLOW_PATTERN_MESSAGES` record mapping `WorkflowPattern` → system message
- `buildAgentRegistrySummary()` function for orchestrator awareness of sub-agent state
- Multi-agent orchestrator message construction with budget, tool list, agent registry

**Resolution strategy:**

```bash
git checkout --theirs gui/src/redux/util/getBaseSystemMessage.ts
# Re-add RICA_IDENTITY import from "core/llm/defaultSystemMessages"
# Re-add multi-agent system message imports from "core/llm/multiAgentSystemMessage"
# Re-add `${RICA_IDENTITY}\n\n` prefix at both return points
# Re-add multi-agent mode branch with workflow patterns + agent registry
```

**Decision tree:**

| Upstream Change                        | Action                                                |
| -------------------------------------- | ----------------------------------------------------- |
| Keeps same getBaseSystemMessage shape  | Re-apply RICA_IDENTITY prefix + multi-agent branch    |
| Adds new modes (e.g. "review")         | Add RICA_IDENTITY prefix to new mode's return         |
| Refactors to per-mode builder classes  | Add RICA_IDENTITY in each builder + add multi-agent   |
| Adds native multi-agent system message | Evaluate if RICA's can be removed; keep RICA_IDENTITY |

**Verification:**

- [ ] System messages start with RICA identity block
- [ ] Multi-agent orchestrator gets workflow-specific instructions
- [ ] Chat/agent/plan modes still get correct base system messages

---

### 3.21. `gui/src/redux/thunks/streamNormalInput.ts` — 🟡 MEDIUM RISK

**Feature:** Multi-Agent (session model lock guard)
**Why it conflicts:** Core streaming thunk, upstream frequently adds pre-streaming checks.

**RICA changes (~10 lines: model lock guard before streaming starts):**

```diff
+    const lockedChatModel = sessionModelLock
+      ? chatModels.find((m) => m.title === sessionModelLock.modelTitle) ?? null
+      : null;
+
+    // Lock is set but the locked model isn't in chatModels — sign-out,
+    // RICA Dynamic profile missing, or backend dropped the model. Refuse
+    // to silently fall back to the local Assistant.
+    if (sessionModelLock && !lockedChatModel) {
+      const msg = `Model "${sessionModelLock.modelTitle}" is locked for this session but is not currently available. Sign in again or check your RICA configuration, then retry.`;
+      dispatch(setStreamError(msg));
+      dispatch(setInactive());
+      return;
+    }
+
+    const selectedChatModel = lockedChatModel ?? selectSelectedChatModel(state);
```

Also adds rate-limit guard that refuses streaming when selected model is rate-limited (prevents multi-agent from burning through the wrong model).

**Resolution strategy:**

```bash
git checkout --theirs gui/src/redux/thunks/streamNormalInput.ts
# Re-add session model lock imports and guard logic (before selectedChatModel selection)
# Re-add rate-limit guard (after model selection, before streaming starts)
```

**Decision tree:**

| Upstream Change                          | Action                                              |
| ---------------------------------------- | --------------------------------------------------- |
| Keeps same pre-streaming check pattern   | Re-apply lock guard at same location                |
| Refactors model selection to a helper    | Add lock guard inside the helper                    |
| Adds native model lock / session concept | Evaluate if RICA's can be removed; keep if distinct |
| Adds rate-limiting                       | Use upstream's mechanism, remove RICA guard         |

**Verification:**

- [ ] Locked model unavailable → error shown, stream not started
- [ ] Rate-limited model → stream refused with user-facing message
- [ ] Normal streaming works when model is available and not rate-limited

---

### 3.22. `gui/src/components/gui/MultiAgentNudge.tsx` — 🟡 MEDIUM RISK

**Feature:** Multi-Agent (guided onboarding tour)
**Why it conflicts:** Located outside `gui/src/rica/` (compliance issue). If upstream adds its own onboarding/tour system, could conflict.

**RICA changes (entire file is RICA-only, ~600 lines):**

- 3-step guided tour introducing multi-agent mode
- Bilingual content (English + Japanese)
- Floating UI positioning with anchor selectors
- localStorage persistence (`hasSeenMultiAgentTour_v1`)
- CSS-in-JSX styling with `.rica-tour-*` class names

**Why not in `gui/src/rica/`:** This component is rendered by the main GUI shell (`gui/src/components/gui/`) and requires direct access to the DOM anchors (mode selector, agent tab). Moving it to `gui/src/rica/` is a Tier 1 refactoring target — the component itself needs no change, only the import path in the parent.

**Resolution strategy:**

```bash
# This file has no upstream equivalent — it should never conflict.
# If upstream adds a file at the same path (unlikely), rename RICA's.
# Tier 1 target: move to gui/src/rica/components/MultiAgentNudge.tsx
git checkout --ours gui/src/components/gui/MultiAgentNudge.tsx
```

**Decision tree:**

| Upstream Change                         | Action                                           |
| --------------------------------------- | ------------------------------------------------ |
| No change (file doesn't exist upstream) | Keep as-is (or move to gui/src/rica/)            |
| Upstream adds own tour/onboarding       | Evaluate if RICA can use upstream's; keep if not |

**Verification:**

- [ ] Tour appears on first chat when multi-agent is available
- [ ] Tour dismisses permanently after completion
- [ ] Language toggle (EN/JA) works

---

### 3.23. `gui/src/components/mainInput/Lump/LumpToolbar/LumpToolbar.tsx` — 🟡 MEDIUM RISK

**Feature:** Multi-Agent (sub-agent cancel UI routing)
**Why it conflicts:** Core toolbar component, upstream actively adds/removes toolbar states.

**RICA changes (3 locations, ~30 lines total):**

```diff
+// RICA Multi-Agent: route Cancel through cancelToolCallThunk
+import { cancelToolCallThunk } from "../../../../redux/thunks/cancelToolCall";

+  // RICA Multi-Agent: when sub-agents are running, swap toolbar
+  const subAgentProgress = useAppSelector(
+    (state) => state.session.subAgentProgress,
+  );
+  const hasRunningSubAgents = Object.values(subAgentProgress).some(
+    (a) => a.status === "running",
+  );

+  // RICA Multi-Agent: priority check before streaming/pending checks
+  if (hasRunningSubAgents) {
+    return <StreamingToolbarWithAgents />;
+  }
```

Also modifies terminal command stop handler to route through `cancelToolCallThunk` (instead of bare slice action) so orchestrator receives "tool cancelled" message.

**Resolution strategy:**

```bash
git checkout --theirs gui/src/components/mainInput/Lump/LumpToolbar/LumpToolbar.tsx
# Re-add cancelToolCallThunk import
# Re-add subAgentProgress selector + hasRunningSubAgents derived state
# Re-add hasRunningSubAgents priority check (before streaming/terminal checks)
# Re-add cancelToolCallThunk dispatch in terminal stop handler
```

**Decision tree:**

| Upstream Change                           | Action                                               |
| ----------------------------------------- | ---------------------------------------------------- |
| Keeps same toolbar state priority pattern | Re-apply hasRunningSubAgents check at same position  |
| Refactors toolbar to state machine        | Add "subAgentsRunning" state that renders agents UI  |
| Adds native multi-agent toolbar support   | Use upstream's mechanism, remove RICA's              |
| Changes cancelToolCall flow               | Ensure thunk dispatch preserved for orchestrator msg |

**Verification:**

- [ ] Sub-agents running → StreamingToolbarWithAgents shown (not PendingToolCallToolbar)
- [ ] Cancel button dispatches cancelToolCallThunk (orchestrator gets cancellation message)
- [ ] Terminal command stop still works
- [ ] Normal streaming toolbar unaffected

---

### 3.20. Multi-Agent — overview & file map

**Feature:** Multi-Agent orchestration (an orchestrator model spawns sub-agents that
work in parallel/sequence, with per-session token tracking, Tier-S model locking, and
rate-limit gating). This feature touches more `core/` files than any other RICA feature
and was not covered by the original hotspot list. The entries §3.20–§3.30 below document
every multi-agent core touch.

**Net-new RICA-only files (upstream has no equivalent — they never merge-conflict, but
upstream restructuring of the surrounding directories can orphan their imports):**

| Path                                                                                     | Role                                                                                                                                                            |
| ---------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `core/config/multiAgentTier.ts` (+ `.test.ts`)                                           | Tier-S eligibility + model-path gating                                                                                                                          |
| `core/llm/multiAgentSystemMessage.ts`                                                    | Orchestrator system prompt                                                                                                                                      |
| `core/tools/definitions/{spawnAgent,spawnAgents,analyzeTask}.ts`                         | Tool JSON-schema definitions                                                                                                                                    |
| `core/tools/implementations/{spawnAgent,spawnAgents,analyzeTask,editFileForSubAgent}.ts` | Tool implementations                                                                                                                                            |
| `core/tools/multiagent/**` (10 files)                                                    | Subsystem: AgentManager, ContextBridge, FaultHandler, FileTracker, MultiAgentLogger, SubAgentRegistry, SubAgentRunner, TaskDecomposer, runAgentWithRetry, index |

**Modified upstream files (the real conflict surface): §3.21–§3.30.**

**Sync rule of thumb:** all multi-agent additions are appended at the END of upstream
structures (enums, arrays, switch tails, type unions) and clearly marked with a `RICA`
comment. On a conflict, accept upstream then re-append the RICA block.

---

### 3.21. `core/tools/builtIn.ts` + `core/tools/index.ts` + `core/tools/definitions/index.ts` + `core/tools/callTool.ts` — 🟡 MEDIUM RISK

**Feature:** Multi-Agent (tool registration + dispatch)
**Why it conflicts:** Upstream actively adds built-in tools; these four files are the
tool registry and dispatch table.

**RICA changes (all additive, marked with `RICA multi-agent` comments):**

- `builtIn.ts` — 3 enum members appended: `SpawnAgent = "spawn_agent"`, `SpawnAgents = "spawn_agents"`, `AnalyzeTask = "builtin_analyze_task"`.
- `definitions/index.ts` — 3 barrel re-exports (`spawnAgentTool`, `spawnAgentsTool`, `analyzeTaskTool`).
- `index.ts` — the 3 tool definitions added to the base tool list in `getBaseToolDefinitions()` so they are always present in `config.tools`. Mode gating (`selectActiveTools` in the GUI) controls whether the orchestrator may call them.
- `callTool.ts` — 3 dispatch arms appended to `callBuiltInTool()` (`case SpawnAgent / SpawnAgents / AnalyzeTask`).

**Why all four must change together:** if the tool is defined but not registered in the
base list, `handleToolCall` + `preprocessArgs` throw "Tool not found" and the orchestrator
silently degrades to single-threaded. If registered but not dispatched, the call falls
through to the default throw.

**Resolution strategy:**

```bash
git checkout --theirs core/tools/builtIn.ts core/tools/index.ts core/tools/definitions/index.ts core/tools/callTool.ts
# Re-append: enum members → barrel exports → base-list entries → dispatch arms (in that order)
```

**Verification:**

- [ ] Multi-Agent mode spawns sub-agents (robot strip animates)
- [ ] `npx tsc --noEmit -p core/tsconfig.json` passes

---

### 3.22. `core/protocol/core.ts` + `core/protocol/passThrough.ts` + `core/protocol/webview.ts` — 🟢 LOW RISK

**Feature:** Multi-Agent (protocol channels)
**Why it conflicts:** Upstream adds protocol message entries; these are the message-type
maps. (Note: §3.9/§3.10 already cover the Online-Documents entries in core.ts/passThrough.ts —
the multi-agent entries are additional, in the same structures.)

**RICA changes (appended at END of each structure):**

- `core/protocol/core.ts` — `cancelSubAgent`, `killSession`, `multiagent/getSessionLog`, `multiagent/logMainModelTurn`, `multiagent/logToolResult`, `multiagent/logError` added to `ToCoreFromIdeOrWebviewProtocol`.
- `core/protocol/passThrough.ts` — the same `multiagent/*` routes added to `WEBVIEW_TO_CORE_PASS_THROUGH`.
- `core/protocol/webview.ts` — `subAgentProgress: [SubAgentProgress, void]` added to the core→webview map (+ `SubAgentProgress` import).

**Resolution strategy:**

```bash
git checkout --theirs core/protocol/core.ts core/protocol/passThrough.ts core/protocol/webview.ts
# Re-add multiagent/* entries at end of each map (before the closing brace/bracket)
```

**Verification:**

- [ ] Sub-agent progress streams to the GUI tree; cancel/kill buttons work
- [ ] Both core + extension tsconfig typechecks pass

---

### 3.23. `core/llm/multiAgentSystemMessage.ts` (net-new) + `core/llm/defaultSystemMessages.ts` — 🟢 LOW RISK

**Feature:** Multi-Agent system prompt + RICA product identity
**Why it conflicts:** `multiAgentSystemMessage.ts` is RICA-only (never conflicts).
`defaultSystemMessages.ts` is upstream and gains one exported constant.

**RICA changes:**

- `defaultSystemMessages.ts` — adds `export const RICA_IDENTITY = "You are RICA, …"`, prepended to every mode's base system message so the model identifies as RICA instead of confabulating a product name. **Candidate for relocation to `rica/` (see §3.30 note).**

**Resolution strategy:**

```bash
git checkout --theirs core/llm/defaultSystemMessages.ts
# Re-add the RICA_IDENTITY const + its prepend at the base-message return sites
```

**Verification:**

- [ ] Model identifies itself as RICA in chat

---

### 3.24. `core/llm/streamChat.ts` — 🟡 MEDIUM RISK

**Feature:** Multi-Agent orchestrator tracing + transient-stream retry
**Why it conflicts:** Core streaming entry point; upstream actively maintains it.

**RICA changes:**

- Imports `MultiAgentLogger`; logs the orchestrator's user prompt + model at stream entry (so the session log has the originating prompt before sub-agents run).
- Adds a transient-error retry wrapper (`STREAM_RETRYABLE_PATTERNS` / `STREAM_MAX_RETRIES` / `STREAM_RETRY_DELAYS`) — backend load balancers drop long sub-agent streams mid-flight; only network-class errors retry (auth/validation/abort stay fatal).

**Resolution strategy:**

```bash
git checkout --theirs core/llm/streamChat.ts
# Re-add: MultiAgentLogger import + entry-point logging block; the retry constants + wrapper around the stream loop
```

**Decision tree:**

| Upstream Change                 | Action                                                                         |
| ------------------------------- | ------------------------------------------------------------------------------ |
| Adds its own stream-retry logic | Adopt upstream's; drop RICA's retry constants; keep the logging block          |
| Refactors the stream loop       | Re-wrap the new loop with the retry helper; re-insert logging at the new entry |

**Verification:**

- [ ] Long multi-agent runs survive a dropped connection (retry, not hard fail)
- [ ] `[StreamChat]` entry log appears with model + prompt preview

---

### 3.25. `core/llm/index.ts` + `core/config/load.ts` + `core/index.d.ts` — 🟡 MEDIUM RISK

**Feature:** Per-model consumption window + multi-agent eligibility (rate-limit lock + Tier-S gate)
**Why it conflicts:** `BaseLLM` constructor, the model serializer, and the shared type
surface are all actively maintained upstream.

**RICA changes:**

- `core/index.d.ts` — `ConsumptionLimit` interface + `consumptionLimit?` / `multiAgentEligible?` optional fields on `LLMOptions` and `ModelDescription`; optional `promptTokens` / `completionTokens` on `PromptLog`; the multi-agent type surface (`SubAgentProgress`, `SubAgentStatus`, `ToolAction`, etc.) required by `core/tools/multiagent/*`.
- `core/llm/index.ts` — `BaseLLM` copies `consumptionLimit` + `multiAgentEligible` from options onto the instance (upstream only copied `model_id`); token-usage fields populated on the `PromptLog`.
- `core/config/load.ts` — `llmToSerializedModelDescription` forwards `consumptionLimit` + `multiAgentEligible` to the browser config. **Without this the GUI never sees `blocked: true` from the backend, so the entire rate-limit lock silently fails for every model** — high-value, easily-dropped on a serializer rewrite.

**Resolution strategy:**

```bash
git checkout --theirs core/llm/index.ts core/config/load.ts core/index.d.ts
# Re-add: the two optional fields everywhere they appear; the constructor copy; the serializer forward
```

**Verification:**

- [ ] A backend-blocked model shows the rate-limit badge + is disabled in all model dropdowns

---

### 3.26. `core/util/history.ts` — 🟢 LOW RISK

**Feature:** Multi-Agent tree persistence
**Why it conflicts:** Upstream owns the session disk serializer (`addToHistory` /
field-by-field `orderedSession` rebuild).

**RICA changes:** copies `session.sessionModelLock` + `session.subAgentProgress` (when
non-empty) onto `orderedSession`. The serializer rebuilds the session field-by-field, so
anything not explicitly copied is dropped at write time — without these two lines the
agent tree + model lock vanish on reload.

**Resolution strategy:**

```bash
git checkout --theirs core/util/history.ts
# Re-add the sessionModelLock + subAgentProgress copies in the orderedSession builder
```

**Verification:**

- [ ] Multi-agent tree + Tier-S lock survive reload / extension restart

---

### 3.27. `core/data/devdataSqlite.ts` — 🟢 LOW RISK

**Feature:** Token-consumption sync (feeds the backend rate-limit window)
**Why it conflicts:** Upstream owns the dev-data SQLite schema + migration logic.

**RICA changes:** adds `model_id` + `synced` columns to `tokens_generated` (idempotent
`ALTER TABLE` migrations), threads `modelId` through the insert, and adds
`getUnsyncedTokenRecords()` / `markRecordsAsSynced()` for the RICA dynamic-config sync.

**Resolution strategy:**

```bash
git checkout --theirs core/data/devdataSqlite.ts
# Re-add the two ALTER TABLE migration blocks, the insert column, and the two static sync methods
```

**Verification:**

- [ ] Token consumption persists with model_id; sync methods return unsynced rows

---

### 3.28. `core/util/paths.ts` — 🟠 HIGH RISK (foundational)

**Feature:** RICA global directory (`~/.rica`)
**Why it conflicts:** Foundational path constant used throughout core; upstream may
refactor `getGlobalDir()`.

**RICA changes:** global dir resolves to `RICA_GLOBAL_DIR` env → `CONTINUE_GLOBAL_DIR`
(back-compat for the test harness only) → default `~/.rica` (never `~/.continue`). Keeps
sessions/index/logs in the same root as the debug log.

**Resolution strategy:**

```bash
git checkout --theirs core/util/paths.ts
# Re-apply: prefer RICA_GLOBAL_DIR; default to ~/.rica instead of ~/.continue
```

**Verification:**

- [ ] Fresh profile writes sessions under `~/.rica` (not `~/.continue`)

---

### 3.29. Telemetry severs — `core/util/posthog.ts`, `core/control-plane/TeamAnalytics.ts`, `core/util/TokensBatchingService.ts`, `core/util/sentry/constants.ts`, `core/package.json` — 🟢 LOW RISK

**Feature:** No external telemetry in the RICA build
**Why it conflicts:** Upstream maintains its analytics/telemetry plumbing.

**RICA changes:** PostHog client stays perma-undefined; `TeamAnalytics` provider always
returns undefined; `TokensBatchingService.flushBatch` is a no-op (still empties the batch
map); `SENTRY_DSN = ""`. Each carries a comment explaining _why_ it is disabled.
Additionally, `core/package.json` has the `sentry:sourcemaps` script removed and `build:npm`
simplified to just `npm run build` (no Sentry upload step).

**Resolution strategy:** accept upstream, then re-apply each no-op/empty-value. For
`package.json`: if upstream adds new Sentry/telemetry scripts, remove them again.

**Verification:**

- [ ] No requests to posthog / sentry / analytics endpoints (DNS log or proxy capture)
- [ ] `npm run build:npm` in `core/` succeeds without Sentry CLI dependency

---

### 3.30. `core/config/multiAgentTier.ts` + `core/config/multiAgentTier.test.ts` — 🟢 NONE (relocated to overlay; thin re-export)

**Feature:** Tier-S eligibility + model-path gating for Multi-Agent
**Status:** The logic now lives in `rica/adapters/config/multiAgentTier.ts`. The core file
`core/config/multiAgentTier.ts` is a **1-line re-export** so existing import paths (1 core
test + 5 gui files: ModeSelect, MultiAgentModelSelect, SessionUsageBar, autoSwitchBlockedModel)
keep working. `core/config/multiAgentTier.test.ts` (48 lines) tests the re-exported logic —
RICA-only, will never conflict with upstream.

**Why a re-export instead of a clean import switch:** `gui/` resolves `core/...` via a
`node_modules/core` symlink but `rica/` is not a gui-resolvable module. The re-export from
`core/` (which can reach `rica/` via the relative path `../../rica/...`) lets the gui keep
importing `core/config/multiAgentTier` while the real code lives in the overlay. Verified:
gui tsc + vite bundle resolve it through the symlink; the moved logic is present in
`dist/assets/index.js`.

**On sync conflict:** the core file is now trivial (a re-export) — accept upstream's
version of the directory and keep the 1-line re-export. The real logic in `rica/` never
conflicts.

> **Same pattern applied to `RICA_IDENTITY` (§3.23):** the literal moved to
> `rica/adapters/llm/identity.ts`; `core/llm/defaultSystemMessages.ts` re-exports it. core +
> gui both reach it via the existing `core/llm/defaultSystemMessages` import path.

---

### 3.31. `core/llm/openaiTypeConverters.ts` — 🟢 LOW RISK

**Feature:** Multi-provider proxy — array-form `delta.content` handling
**Why it conflicts:** Core OpenAI-shape converter, actively maintained.

**RICA changes:** `fromChatCompletionChunk` now handles `delta.content` arriving as a
content-block array (some Databricks-hosted non-OpenAI models stream this shape), joining
`type === "text"` parts into a string instead of assuming a plain string.

**Resolution strategy:**

```bash
git checkout --theirs core/llm/openaiTypeConverters.ts
# Re-apply the string | array | fallback normalization in fromChatCompletionChunk
```

**Verification:**

- [ ] Streaming works for non-OpenAI models behind the proxy (no `[object Object]` content)

---

### 3.32. `core/config/ConfigHandler.ts` — 🟢 LOW RISK (irreducible)

**Feature:** Block "open profile" navigation for the RICA build
**Why it conflicts:** Upstream owns profile-navigation flow.

**RICA changes:** one `else if (EXTENSION_NAME === "rica")` branch that shows a toast
("RICA models are managed centrally …") instead of opening `${APP_URL}${profileId}`
(which would point at continue.dev). Defense-in-depth alongside the InlineErrorMessage toast.

**Resolution strategy:** re-apply the 4-line branch after merge.

---

### 3.33. `core/context/providers/index.ts` — 🟢 LOW RISK

**Feature:** Drop `ContinueProxyContextProvider` from the RICA build
**Why it conflicts:** Upstream owns the context-provider registry.

**RICA changes:** `ContinueProxyContextProvider` removed from the `Providers` array (it
POSTs to the Continue Hub team-add-on channel; no Renesas equivalent, and it would 404
noisily after CONTROL_PLANE_URL was repointed). Source file kept on disk.

**Resolution strategy:** accept upstream, then remove the `ContinueProxyContextProvider`
registration again.

---

### 3.34. `core/control-plane/auth/index.ts` + `core/control-plane/env.ts` — 🟠 HIGH RISK (irreducible)

**Feature:** EntraID auth + RICA control-plane environment
**Why it conflicts:** Auth + environment routing are core seams; every control-plane call
depends on `env.ts`.

**RICA changes:**

- `auth/index.ts` — `getAuthUrlForTokenPage` throws a RICA error (auth flows through `rica/adapters/auth/EntraIDAuthProvider`, not the WorkOS token page). Signature kept for the `auth/getAuthUrl` IPC contract.
- `env.ts` — `EXTENSION_NAME = "rica"`; `getControlPlaneEnvSync()` short-circuits to `RICA_PROD_ENV` / `RICA_STAGING_ENV` (Renesas domains) for the rica build; `enableHubContinueDev()` / `useHub()` return false. Upstream defaults kept in-file only as a paper trail.

**Resolution strategy:** these are permanent hotspots — accept upstream, then re-apply the
`EXTENSION_NAME === "rica"` branches + RICA\_\*\_ENV constants. See the decision tree in
§3.2 / §9 for Core-init interactions.

**Verification:**

- [ ] `env.APP_URL` resolves to a Renesas domain for every core caller
- [ ] EntraID sign-in works (prod + staging)

---

### 3.35. `core/config/profile/doLoadConfig.ts` — 🟠 HIGH RISK (irreducible)

**Feature:** Dynamic-config stash + apiKey rotation for rica-proxy models
**Why it conflicts:** Sits in the middle of the config-loading pipeline, which upstream
actively refactors. (Note: §5 "Removed" list says doLoadConfig no longer needs changes —
that is now **stale**; the file does carry RICA logic again.)

**RICA changes:**

- Stashes the backend dynamic-config block on `config.entraIDDynamicConfig` so multi-agent UI (Tier-S lock, usage bar, auto-switch) can read it.
- Broadens the apiKey-rotation predicate to include `rica-proxy` models — without it, `model.apiKey` freezes at the construction-time MDP token and 401s once the token expires (~60–75 min).
- Carries the `[RICA-DYNAMIC]` + `[CONTEXT-DEBUG]` diagnostics (permanent — do not strip).

**Resolution strategy:**

```bash
git checkout --theirs core/config/profile/doLoadConfig.ts
# Re-apply: the entraIDDynamicConfig stash block; the rica-proxy apiKey-rotation predicate; the diagnostics
```

**Verification:**

- [ ] Multi-agent UI reads dynamic config; chat model does not 401 after ~1 h idle

---

### 3.36. `core/llm/countTokens.ts` — 🟢 LOW RISK

**Feature:** `[CONTEXT-DEBUG]` diagnostic for token-budget calculation
**Why it conflicts:** `compileChatMessages` is a core utility upstream actively maintains
(token counting, message truncation). Structural refactors will touch surrounding code.

**RICA changes:** +30-line `try/catch` block inserted immediately before the
`if (knownContextLength !== undefined && inputTokensAvailable < 0)` throw. Logs a
`[CONTEXT-DEBUG]` JSON with all inputs to the token budget calculation (modelName,
contextLength, toolTokens, systemMsgTokens, etc.) so we can diagnose "Not enough context"
toasts without reproducing the exact model + message state. Permanent diagnostic — do not
strip.

**Resolution strategy:**

```bash
git checkout --theirs core/llm/countTokens.ts
# Locate the "if (knownContextLength !== undefined && inputTokensAvailable < 0)" throw
# Re-insert the [CONTEXT-DEBUG] try/catch block immediately above it
```

**Verification:**

- [ ] Send a long message → console shows `[CONTEXT-DEBUG]` JSON with correct field names
- [ ] If the throw triggers, the diagnostic appears _before_ the error toast

---

## 4. Control Plane Architecture

### 4.1. Problem Statement

Continue's ControlPlaneClient makes API calls to `api.continue.dev` which:

1. Returns 404 errors (RICA doesn't have these endpoints)
2. May trigger firewall blocks (external API calls)
3. Enables features RICA doesn't use (Hub, remote sessions)

### 4.2. Solution: RicaControlPlaneClient Wrapper

**Location:** `rica/adapters/control-plane/RicaControlPlaneClient.ts`

**Pattern:** Extend upstream ControlPlaneClient, override methods. Provides dynamic config loading via `RicaProfileLoader` with 30s TTL cache and auth token passthrough.

```typescript
export class RicaControlPlaneClient extends ControlPlaneClient {
  // Dynamic config from RICA backend, cached with 30s TTL
  async listOrganizations(): Promise<OrganizationDescription[]> {
    /* returns RICA org */
  }
  async listAssistants(): Promise<DynamicAssistantEntry[]> {
    /* returns dynamic models */
  }
  // Disabled upstream features
  async shouldEnableRemoteSessions(): Promise<boolean> {
    return false;
  }
  async listRemoteSessions(): Promise<any[]> {
    return [];
  }
  async getCreditStatus(): Promise<any> {
    return null;
  }
  async getPolicy(): Promise<any> {
    return null;
  }
  // Cache management
  invalidateCache(): void {
    /* forces re-fetch on next call */
  }
}
```

### 4.3. Injection Point: RicaCore (Property Descriptor Trap)

**Location:** `rica/adapters/core/RicaCore.ts`

**Pattern:** Uses `Object.defineProperty` to intercept all writes to `configHandler.controlPlaneClient`. When upstream code (e.g. `updateControlPlaneSessionInfo`) creates a new vanilla `ControlPlaneClient`, the setter automatically wraps it in `RicaControlPlaneClient`.

```typescript
export class RicaCore extends Core {
  constructor(messenger, ide) {
    super(messenger, ide);
    this.installControlPlaneClientTrap(); // property descriptor trap
    this.startDynamicConfigAutoRefresh(); // 5-min interval
  }

  private installControlPlaneClientTrap() {
    Object.defineProperty(this.configHandler, "controlPlaneClient", {
      get() {
        return ricaClient;
      },
      set(newClient: ControlPlaneClient) {
        // Intercept upstream assignment → wrap in RicaControlPlaneClient
        ricaClient = new RicaControlPlaneClient(
          newClient.sessionInfoPromise,
          ide,
        );
      },
    });
  }
}
```

**Why this pattern:**

- Upstream `updateControlPlaneSessionInfo()` replaces `controlPlaneClient` with a new instance
- The trap ensures RICA client is always re-created with the new session token
- No need to monkey-patch upstream methods or override `updateControlPlaneSessionInfo`
- Survives any upstream code path that writes to `controlPlaneClient`

**Dependency:** Requires `ControlPlaneClient.sessionInfoPromise` to remain a public/readonly property (currently at `core/control-plane/client.ts:95`).

### 4.4. Dynamic Config Fetch Pipeline

**Location:** `rica/adapters/config/dynamicConfig.ts`

**Features:**

- Dual-token architecture: EntraID token for backend, MDP token for model serving
- Corporate proxy detection: redirect interception (`redirect: "manual"`) + content-type validation
- Multiple response formats: `payload.models`, `payload.config.models`, `payload.data`
- User-Agent header: `RICA-Extension/1.0`
- `RicaProfileLoader` injects MDP token as `apiKey` on model descriptions (no `controlPlaneProxyInfo` injection needed in core/)

**Token flow (dual-token):**

```
EntraIDAuthProvider
  ├─ entraAccessToken (from PKCE flow)
  │   └─ getEntraIDAccessToken() → RicaCore → RicaControlPlaneClient.entraTokenGetter
  │       └─ RicaProfileLoader.getBackendToken → Bearer for /api/user-model/configs (RICA backend)
  │
  └─ session.accessToken (MDP token from exchangeTokenWithBackend)
      └─ HubSessionInfo.accessToken → ControlPlaneClient.getAccessToken()
          └─ RicaProfileLoader.getModelToken → apiKey on model descriptions → Databricks serving
```

**Why dual-token:**

- RICA backend (`rica.global.renesas.com`) authenticates with EntraID token
- Databricks model serving (`adb-*.azuredatabricks.net`) authenticates with MDP token
- MDP token is obtained by exchanging EntraID token with the RICA backend (`/auth/exchange`)

### 4.5. Sync Impact

| Component              | When to Update                                            |
| ---------------------- | --------------------------------------------------------- |
| RicaControlPlaneClient | Upstream adds new methods to ControlPlaneClient           |
| RicaCore trap          | Upstream changes `controlPlaneClient` property to private |
| RicaCore trap          | Upstream removes `sessionInfoPromise` from client         |
| RicaCore refresh       | Upstream changes `configHandler.reloadConfig()` signature |

---

## 5. Integration Patches Summary

| #   | File                                                          | Purpose                                         | Auto?        | Feature    |
| --- | ------------------------------------------------------------- | ----------------------------------------------- | ------------ | ---------- |
| 1   | VsCodeExtension.ts                                            | RicaCore import + Auth                          | ❌ Manual    | Core Init  |
| 2   | core/control-plane/AuthTypes.ts                               | RICA enum values                                | ❌ Manual    | Auth       |
| 3   | core/llm/llms/index.ts                                        | databricks→openai alias                         | ❌ Manual    | DynConfig  |
| 4   | core/llm/toolSupport.ts                                       | Tool support entries                            | ❌ Manual    | DynConfig  |
| 5   | extensions/vscode/tsconfig.json                               | RICA include path                               | ❌ Manual    | Build      |
| 6   | extensions/vscode/package.json                                | Branding                                        | ✅ Automated | Branding   |
| 7   | core/control-plane/client.ts                                  | node-fetch type fix                             | ❌ Manual    | TypeFix    |
| 8   | WorkOsAuthProvider.ts                                         | URL.toString() fix                              | ❌ Manual    | TypeFix    |
| 9   | core/protocol/core.ts                                         | RICA protocol messages (5)                      | ❌ Manual    | OnlineDocs |
| 10  | core/protocol/passThrough.ts                                  | RICA passthrough routes (5)                     | ❌ Manual    | OnlineDocs |
| 11  | core/protocol/documents.ts                                    | Re-export from rica/ (NEW)                      | ❌ Manual    | OnlineDocs |
| 12  | core/config/loadContextProviders                              | Auto-register rica-docs                         | ❌ Manual    | OnlineDocs |
| 13  | core/llm/autodetect.ts                                        | rica-proxy image support                        | ❌ Manual    | OnlineDocs |
| 14  | core/index.d.ts                                               | imageUrls on ContextItem                        | ❌ Manual    | OnlineDocs |
| 15  | core/config/types.ts                                          | imageUrls on ContextItem                        | ❌ Manual    | OnlineDocs |
| 16  | gui/src/.../constructMessages.ts                              | imageUrls → ImageMessagePart                    | ❌ Manual    | OnlineDocs |
| 17  | extensions/.../VsCodeIde.ts                                   | Binary file open logic                          | ❌ Manual    | OnlineDocs |
| 18  | gui/src/.../AtMentionDropdown                                 | rica-docs page input intercept                  | ❌ Manual    | OnlineDocs |
| 19  | gui/src/.../BlockSettingsTopToolbar                           | RicaDocsPopover button                          | ❌ Manual    | OnlineDocs |
| 20  | gui/src/pages/config/configTabs                               | rica-docs tab entry                             | ❌ Manual    | OnlineDocs |
| 21  | gui/src/util/navigation.ts                                    | RICA_DOCS route                                 | ❌ Manual    | OnlineDocs |
| 22  | ContinueGUIWebviewViewProvider.ts                             | viewType → rica.ricaGUIView                     | ❌ Manual    | ViewID     |
| 23  | ContinueConsoleWebviewViewProvider.ts                         | viewType → rica.ricaConsoleView                 | ❌ Manual    | ViewID     |
| 24  | extensions/.../commands.ts                                    | ricaGUIView refs + panel title                  | ❌ Manual    | ViewID     |
| 25  | core/tools/{builtIn,index,callTool}.ts + definitions/index.ts | Multi-agent tool reg + dispatch                 | ❌ Manual    | MultiAgent |
| 26  | core/protocol/{core,passThrough,webview}.ts                   | multiagent/\* channels + subAgentProgress       | ❌ Manual    | MultiAgent |
| 27  | core/llm/streamChat.ts                                        | Orchestrator trace + stream retry               | ❌ Manual    | MultiAgent |
| 28  | core/llm/index.ts + core/config/load.ts + core/index.d.ts     | consumptionLimit/Tier-S serialization           | ❌ Manual    | MultiAgent |
| 29  | core/util/history.ts                                          | Multi-agent tree persistence                    | ❌ Manual    | MultiAgent |
| 30  | core/data/devdataSqlite.ts                                    | Token-consumption sync schema                   | ❌ Manual    | MultiAgent |
| 31  | core/util/paths.ts                                            | RICA global dir (~/.rica)                       | ❌ Manual    | Paths      |
| 32  | core/control-plane/env.ts + auth/index.ts                     | RICA env routing + EntraID redirect             | ❌ Manual    | Auth/Env   |
| 33  | core/config/profile/doLoadConfig.ts                           | Dynamic-config stash + apiKey rotation          | ❌ Manual    | DynConfig  |
| 34  | core/config/ConfigHandler.ts                                  | Block profile-open navigation                   | ❌ Manual    | DynConfig  |
| 35  | core/context/providers/index.ts                               | Drop ContinueProxyContextProvider               | ❌ Manual    | Severs     |
| 36  | core/llm/{posthog,sentry,TeamAnalytics,TokensBatchingService} | Telemetry severs                                | ❌ Manual    | Telemetry  |
| 37  | core/llm/openaiTypeConverters.ts                              | Array-form delta.content handling               | ❌ Manual    | Proxy      |
| 38  | core/llm/defaultSystemMessages.ts                             | RICA_IDENTITY (RICA-only-in-core)               | ❌ Manual    | Identity   |
| 39  | gui/src/.../getBaseSystemMessage.ts                           | RICA_IDENTITY prefix + multi-agent orchestrator | ❌ Manual    | MultiAgent |
| 40  | gui/src/.../streamNormalInput.ts                              | Session model lock + rate-limit guard           | ❌ Manual    | MultiAgent |
| 41  | gui/src/.../MultiAgentNudge.tsx                               | RICA guided tour (new file)                     | ❌ Manual    | MultiAgent |
| 42  | gui/src/.../LumpToolbar.tsx                                   | Sub-agent cancel routing + priority check       | ❌ Manual    | MultiAgent |
| 43  | gui/src/.../ModelsSection.tsx                                 | Doc URLs → RICA_DOCS_URL                        | ❌ Manual    | Branding   |
| 44  | gui/src/.../ModelSelect.tsx                                   | Rate-limit lock + consumption window            | ❌ Manual    | MultiAgent |
| 45  | gui/src/.../HelpSection.tsx                                   | Keyboard descriptions + GitLab URLs             | ❌ Manual    | Branding   |
| 46  | gui/src/.../ToolsSection.tsx                                  | MCP disabled message                            | ❌ Manual    | Branding   |
| 47  | gui/src/.../UserSettingsSection.tsx                           | Hidden experimental settings                    | ❌ Manual    | Branding   |
| 48  | gui/src/.../useOnboardingCard.ts                              | `RICA_HIDE_ONBOARDING_CARD = true`              | ❌ Manual    | Branding   |
| 49  | gui/src/.../CliInstallBanner.tsx                              | `RICA_HIDE_CLI_BANNER = true`                   | ❌ Manual    | Branding   |
| 50  | gui/src/.../FeedbackDialog.tsx                                | "Help us improve RICA" text                     | ❌ Manual    | Branding   |
| 51  | gui/src/.../AddModelForm.tsx                                  | Doc URLs → rica.global.renesas.com              | ❌ Manual    | Branding   |
| 52  | gui/src/.../InlineErrorMessage.tsx                            | RICA error message text                         | ❌ Manual    | Branding   |
| 53  | gui/src/.../StreamError.tsx                                   | RICA error message text                         | ❌ Manual    | Branding   |
| 54  | gui/src/.../ToolCallStatusMessage.tsx                         | `RICA ${intro} ${message}`                      | ❌ Manual    | Branding   |
| 55  | gui/src/.../FunctionSpecificToolCallDiv.tsx                   | Multi-agent comments                            | ❌ Manual    | MultiAgent |
| 56  | gui/src/.../StyledMarkdownPreview                             | RICA multi-agent rendering note                 | ❌ Manual    | MultiAgent |

**Notes:**

- Items 9–21 are Phase 5 (Online Documents) additions
- Items 9–10: entries at END of structures, low merge conflict risk
- Item 11: new file with no conflict (re-exports from `rica/adapters/documents/types.ts`)
- Items 14–15: additive optional properties, extremely low conflict risk
- Items 18–21: GUI integration, all import from `gui/src/rica/` (bridge pattern)
- Items 19, 20: use 1-line bridge re-exports that point to `gui/src/rica/`
- Items 22–24: View ID namespacing — required because VS Code demands provider ID = manifest ID at runtime. Manifest is generated with `rica.*` namespace, so source must match.
- Items 25–28: Multi-Agent feature (Phase 4), full playbooks in §3.20–§3.23
- Items 29–42: GUI branding/feature-flag changes — trivial string or constant replacements, low conflict risk. Resolution: accept upstream, re-apply RICA strings/URLs/flags. No individual playbooks needed.

**Removed (no longer needed in core/):**

- ~~`core/control-plane/AuthTypes.ts` backendAccessToken~~ — uses `accessToken` directly
- ~~`core/core.ts` private→protected~~ — RicaCore stores own `ricaMessenger` field instead

> **Correction:** an earlier revision listed `core/config/profile/doLoadConfig.ts` here as
> "no longer needed in core/". That is **stale** — the file carries RICA logic again
> (dynamic-config stash + rica-proxy apiKey rotation). See §3.35. `core/core.ts` likewise
> carries the multi-agent session registries + handlers. Treat both as active hotspots.

**Note on RICA-only logic that sat in `core/`:** `multiAgentTier.ts` (§3.30) and
`RICA_IDENTITY` (§3.23) have been **relocated to `rica/`** with thin re-exports left in
`core/` so the gui's existing `core/...` import paths keep resolving (gui reaches `core/`
via a `node_modules/core` symlink; the re-export hops into `rica/` via a relative path).
This is the agreed pattern for shrinking the core footprint without a gui build-infra
change.

The remaining RICA-only files in `core/` (`core/tools/multiagent/**` and the spawn/analyze
tool defs+impls) are **kept in place by decision**: they form one tightly-coupled subsystem
wired into the tool-dispatch loop, they are net-new so they never conflict on sync, and
fragmenting the subsystem (moving one file out) would add risk for no sync benefit. They are
documented as a directory-level hotspot in §3.20.

---

## 6. Monthly Sync Workflow

### Phase 1: Preparation (30 min)

```bash
git tag pre-sync-$(date +%Y-%m)
git checkout -b rica/upstream-staging-$(date +%Y-%m)
git fetch upstream
git diff --stat HEAD..upstream/main > .sync-report.txt
```

### Phase 2: Safe Changes (2 hours)

```bash
git merge upstream/main --no-commit
git checkout --theirs core/**/*.ts packages/**
# (except files listed in Section 3)
```

### Phase 3: High-Risk Areas (2 hours)

For each file in Section 3, follow its playbook.

### Phase 4: Validate (1 hour)

```bash
npm run tsc:check
npm run esbuild
node rica/scripts/check-branding.js
# F5 test in VSCode
```

### Phase 5: Finalize (30 min)

```bash
git add . && git commit -m "chore: upstream sync $(date +%Y-%m)"
git checkout dev && git merge --no-ff rica/upstream-staging-$(date +%Y-%m)
git tag sync-$(date +%Y-%m)
```

---

## 7. Decision Matrix

| Condition                      | Action                 |
| ------------------------------ | ---------------------- |
| File in `core/**` (not in §3)  | Accept upstream        |
| File in `packages/**`          | Accept upstream        |
| File in `rica/**`              | Never accept upstream  |
| File in `extensions/vscode/**` | Manual review (see §3) |
| Branding-related               | Keep RICA              |
| Auth-related                   | Keep RICA              |
| Security patch                 | Accept upstream        |

---

## 8. Emergency Rollback

```bash
git checkout dev
git reset --hard pre-sync-YYYY-MM
```

---

## 9. High-Risk Scenarios

### Scenario 1: Upstream Refactors Core Class

**Action:** Update `RicaCore` to match new interface

### Scenario 2: Upstream Refactors ControlPlaneClient

**Action:** Update `RicaControlPlaneClient`, override new methods

### Scenario 3: Upstream Removes useHub()

**Action:** Find new mechanism, override in `RicaCore`

### Scenario 4: Upstream Makes controlPlaneClient Private or Readonly

**Action:** The property descriptor trap in `RicaCore.installControlPlaneClientTrap()` requires the property to be configurable. If upstream uses `private` or `#field`, move injection to constructor override or find new writable seam. Check `Object.getOwnPropertyDescriptor(configHandler, 'controlPlaneClient')` — if `configurable: false`, trap cannot be installed.

### Scenario 5: Upstream Adds Native Databricks Provider

**Action:** Remove `databricks` from `normalizeProviderName()` in `core/llm/llms/index.ts` and remove the `databricks` entry from `core/llm/toolSupport.ts`. Update `rica/adapters/config/modelMapping.ts` PROVIDER_ALIAS_MAP to remove `databricks` → `openai`.

### Scenario 6: Upstream Changes llmFromDescription Signature

**Action:** Re-apply `normalizeProviderName()` call at the point where provider name is resolved to a class.

### Scenario 7: Upstream Adds Native Multimodal Context Support

**Action:** If upstream adds its own `imageUrls` or similar field to ContextItem, adopt upstream's approach and remove RICA's `imageUrls` additions from `core/index.d.ts`, `core/config/types.ts`, and `gui/src/redux/util/constructMessages.ts`. Update `rica/adapters/documents/RicaDocsContextProvider.ts` to use the upstream field name.

### Scenario 8: Upstream Changes ToCoreFromIdeOrWebviewProtocol to Interface

**Action:** If upstream converts the `type` to an `interface`, RICA can use TypeScript module augmentation (declaration merging) to add protocol messages from `rica/` instead of core. Move RICA entries from `core/protocol/core.ts` to a `rica/adapters/protocol/rica-protocol.d.ts` file using `declare module` syntax. This would eliminate the core.ts modification entirely.

### Scenario 9: Upstream Refactors constructMessages.ts

**Action:** Find the new location where context items are converted to message parts. Re-apply `imageUrls` → `ImageMessagePart` conversion logic at that point. The pattern is: for each `ctxItem` with `imageUrls`, push `{ type: "imageUrl", imageUrl: { url } }` parts alongside text parts.

### Scenario 10: Upstream Adds Native Multi-Agent Mode

**Action:** Upstream Continue has expressed interest in multi-agent orchestration. If upstream adds:

- A `getBaseSystemMessage` multi-agent branch: evaluate whether RICA's orchestrator messages are a superset. If so, use upstream's base + prepend `RICA_IDENTITY`. If RICA needs additional instructions (workflow patterns, budget, agent registry), keep RICA's branch but rebase on upstream's structure.
- A native `streamNormalInput` model lock: adopt upstream's mechanism, remove RICA's `sessionModelLock` guard.
- A native toolbar for parallel agents: adopt upstream's `StreamingToolbarWithAgents` equivalent, remove RICA's `hasRunningSubAgents` check. The `cancelToolCallThunk` routing should be kept regardless.
- A native onboarding tour system: migrate RICA's tour content into upstream's framework (keep bilingual content), delete `MultiAgentNudge.tsx`.

### Scenario 11: Upstream Refactors LumpToolbar State Priority

**Action:** The RICA `hasRunningSubAgents` check MUST have priority over `isStreaming` and `pendingToolCalls` checks. If upstream reorders the toolbar state machine or converts to a priority enum, ensure the RICA sub-agent state is ranked above streaming/pending. Without this, active sub-agents would show Accept/Reject buttons (for the spawn_agents tool call) instead of the cancel dropdown.

---

## 10. Implementation History & Lessons Learned

### Key Lessons

1. **Minimize core/ changes** — Every change is a future conflict
2. **Use adapters** — Wrap upstream classes, don't modify them
3. **Automate branding** — Scripts prevent manual errors
4. **Document everything** — Future maintainers need context

---

## 11. Adding New Files to This Document

When you modify a new `core/` or `extensions/` file for RICA:

### Checklist

- [ ] Add to **Section 2.2** (Risk Assessment table)
- [ ] Add to **Section 2.3** (Files by Feature Module)
- [ ] Create **Section 3.X** playbook using template
- [ ] Add to **Section 5** (Integration Patches Summary) if applicable
- [ ] Update **Section 7** (Decision Matrix) if new condition
- [ ] Add to **Section 9** (High-Risk Scenarios) if applicable

### Template Location

See the template at the beginning of **Section 3**.

---

## 12. Future SKILL.md Preparation

| Skill                            | Scope         | Source |
| -------------------------------- | ------------- | ------ |
| `SKILL_package_json_sync.md`     | Branding      | §3.1   |
| `SKILL_vscode_extension_sync.md` | Core init     | §3.2   |
| `SKILL_auth_types_sync.md`       | Auth          | §3.3   |
| `SKILL_control_plane_sync.md`    | Control plane | §4     |
| `SKILL_monthly_sync_workflow.md` | Full workflow | §6     |

---

## 13. Related Documents

- [`README.md`](./README.md) — Documentation index
- [`REPO_STANDARD.md`](./REPO_STANDARD.md) — Architecture standard
- [`REFORK_PLAN.md`](./REFORK_PLAN.md) — Refork strategy
- [`BUILD_AND_RELEASE.md`](./BUILD_AND_RELEASE.md) — Build guide
