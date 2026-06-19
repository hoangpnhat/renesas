# RICA Multi-Agent Feature — Technical Documentation

**Document Version:** 5.5  
**Date:** 2026-05-29  
**Status:** v1.1.9 candidate — generalizes the v1.1.8 silent-orchestrator carve-out to all permission-gated tools, plus Cancel-button orchestrator-silence fix  
**Base Branch:** `multiagent-test` (uncommitted; v1.1.7 shipped at `ddf6a5fd8`, v1.1.8 tag pending)

---

## Table of Contents

1. [Executive Summary](#1-executive-summary)
2. [Baseline: RICA Before Multi-Agent](#2-baseline-rica-before-multi-agent)
3. [Architecture Overview](#3-architecture-overview)
4. [Implementation Details](#4-implementation-details)
   - 4.1 [Core Backend — New Files](#41-core-backend--new-files)
   - 4.2 [Core Backend — Modified Files](#42-core-backend--modified-files)
   - 4.3 [GUI Frontend — New Files](#43-gui-frontend--new-files)
   - 4.4 [GUI Frontend — Modified Files](#44-gui-frontend--modified-files)
5. [Data Flow & Sequence Diagrams](#5-data-flow--sequence-diagrams)
6. [Type System & Interfaces](#6-type-system--interfaces)
7. [Orchestrator Intelligence](#7-orchestrator-intelligence)
8. [Agent Roles & Specialization](#8-agent-roles--specialization)
9. [Parallel Execution Engine](#9-parallel-execution-engine)
10. [Progress Tracking & Real-Time UI](#10-progress-tracking--real-time-ui)
11. [Tool Argument Summarization](#11-tool-argument-summarization)
12. [Abort & Cancellation Architecture](#12-abort--cancellation-architecture)
13. [File Change Summary](#13-file-change-summary)
14. [v2.0 — Session Lifecycle, Kill Controls & UI Hardening](#14-v20--session-lifecycle-kill-controls--ui-hardening)
15. [Bugs Encountered & Resolutions](#15-bugs-encountered--resolutions)
16. [Known Limitations & Future Work](#16-known-limitations--future-work)
17. [v3.0 — Token Usage Tracking, API Usage Capture & Session Token UI](#17-v30--token-usage-tracking-api-usage-capture--session-token-ui)
18. [v4.0 — Batch Spawn, Orchestrator Patterns, Conflict Detection & Token Management](#18-v40--batch-spawn-orchestrator-patterns-conflict-detection--token-management)
    - 18.1 [Overview](#181-overview)
    - 18.2 [Batch Agent Spawning (builtin_spawn_agents)](#182-batch-agent-spawning-builtin_spawn_agents)
    - 18.3 [Orchestrator Pattern System](#183-orchestrator-pattern-system)
    - 18.4 [Advanced Agent Infrastructure](#184-advanced-agent-infrastructure)
    - 18.5 [Task Analysis Tool (builtin_analyze_task)](#185-task-analysis-tool-builtin_analyze_task)
    - 18.6 [Pre-flight File Overlap Detection](#186-pre-flight-file-overlap-detection)
    - 18.7 [Post-flight Conflict Detection (FileTracker)](#187-post-flight-conflict-detection-filetracker)
    - 18.8 [BatchAgentsDiv — Animated Robot UI](#188-batchagentsdiv--animated-robot-ui)
    - 18.9 [Token Management Analysis & Recommendations](#189-token-management-analysis--recommendations)
    - 18.10 [New Files (v4.0)](#1810-new-files-v40)
    - 18.11 [Modified Files (v4.0)](#1811-modified-files-v40)
    - 18.12 [Updated Limitations & Future Work](#1812-updated-limitations--future-work)
19. [v5.0 — Structured Session Logging, Log Export UI & Stability Fixes](#19-v50--structured-session-logging-log-export-ui--stability-fixes)
    - 19.1 [Overview](#191-overview)
    - 19.2 [MultiAgentLogger — Structured Session Tracing](#192-multiagentlogger--structured-session-tracing)
    - 19.3 [Logger Integration — SubAgentRunner & spawnAgents](#193-logger-integration--subagentrunner--spawnagents)
    - 19.4 [Orchestrator & IPC Observability](#194-orchestrator--ipc-observability)
22. [v5.3 — Budget Architecture Revert, Live Injection, UX Polish & Guided Tour](#22-v53--budget-architecture-revert-live-injection-ux-polish--guided-tour)
    - 22.1 [Motivation](#221-motivation)
    - 22.2 [Sub-Agent Budget Architecture — Reverted to Static 100K Per-Agent](#222-sub-agent-budget-architecture--reverted-to-static-100k-per-agent)
    - 22.3 [Live `<session_budget>` Injection to Orchestrator](#223-live-session_budget-injection-to-orchestrator)
    - 22.4 [MultiAgentLogger Improvements](#224-multiagentlogger-improvements)
    - 22.5 [Blocked-Model Guards — Symmetric Coverage](#225-blocked-model-guards--symmetric-coverage)
    - 22.6 [Two Long-Standing Behavioral Bugs Resolved](#226-two-long-standing-behavioral-bugs-resolved)
    - 22.7 [Tier S Availability Gating in ModeSelect](#227-tier-s-availability-gating-in-modeselect)
    - 22.8 [90% Usage Warning Toast](#228-90-usage-warning-toast)
    - 22.9 [Multi-Agent Guided Tour](#229-multi-agent-guided-tour)
    - 22.10 [File Change Summary (v5.3)](#2210-file-change-summary-v53)
    - 22.11 [Resolved Limitations](#2211-resolved-limitations)
    - 22.12 [Remaining Limitations](#2212-remaining-limitations)
23. [v5.4 — v1.1.8 Triage Closeout: Spawn IPC, Tool Timeouts & Circuit Breakers](#23-v54--v118-triage-closeout-spawn-ipc-tool-timeouts--circuit-breakers)
    - 23.1 [Motivation](#231-motivation)
    - 23.2 [Fix 1 — applyToFile IPC Timeout 60s → 5min](#232-fix-1--applytofile-ipc-timeout-60s--5min)
    - 23.3 [Fix 2 — Orchestrator-Side Circuit Breaker](#233-fix-2--orchestrator-side-circuit-breaker)
    - 23.4 [Fix 3 — Silent-Orchestrator Re-Arm for Spawn Tools](#234-fix-3--silent-orchestrator-re-arm-for-spawn-tools)
    - 23.5 [Fix 4 — JSON-Truncation User-Facing Error Dialog](#235-fix-4--json-truncation-user-facing-error-dialog)
    - 23.6 [Fix 5 — Sub-Agent-Side Circuit Breaker](#236-fix-5--sub-agent-side-circuit-breaker)
    - 23.7 [Fix 6 — Spawn-Tool IPC Timeout 5min → 20min](#237-fix-6--spawn-tool-ipc-timeout-5min--20min)
    - 23.8 [Fix 7 — Client-Tool Outer-Wrapper Timeout 120s → 5min](#238-fix-7--client-tool-outer-wrapper-timeout-120s--5min)
    - 23.9 [Position-34 Truncation Root Cause (Deferred to v1.1.9)](#239-position-34-truncation-root-cause-deferred-to-v119)
    - 23.10 [Combo Stress Test cd037ad6 — Live Verification](#2310-combo-stress-test-cd037ad6--live-verification)
    - 23.11 [Bruce 2b2f5b7f Bug-Status Mapping](#2311-bruce-2b2f5b7f-bug-status-mapping)
    - 23.12 [File Change Summary (v5.4)](#2312-file-change-summary-v54)
    - 23.13 [Resolved Limitations](#2313-resolved-limitations)
    - 23.14 [Remaining Limitations](#2314-remaining-limitations)
24. [v5.5 — Permission-Gate Silent Orchestrator + Cancel-Button Continuation Fix](#24-v55--permission-gate-silent-orchestrator--cancel-button-continuation-fix)
    - 24.1 [Motivation](#241-motivation)
    - 24.2 [Tester Report (May 29, session 2d0e0ab0)](#242-tester-report-may-29-session-2d0e0ab0)
    - 24.3 [Fix 1 — Generalize Silent-Orchestrator Re-Arm to All Tools](#243-fix-1--generalize-silent-orchestrator-re-arm-to-all-tools)
    - 24.4 [Fix 2 — Cancel-Button Orchestrator Continuation](#244-fix-2--cancel-button-orchestrator-continuation)
    - 24.5 [Why It Was Intermittent (Race Condition)](#245-why-it-was-intermittent-race-condition)
    - 24.6 [File Change Summary (v5.5)](#246-file-change-summary-v55)
    - 24.7 [Resolved Limitations](#247-resolved-limitations)
    - 24.8 [Remaining Limitations](#248-remaining-limitations)
    - 24.9 [WSL2 cmd.exe Bug (Bug 2 — Tracked Separately)](#249-wsl2-cmdexe-bug-bug-2--tracked-separately)
    - 19.5 [Log Retention & Compression](#195-log-retention--compression)
    - 19.6 [Session Log Export UI](#196-session-log-export-ui)
    - 19.7 [Stale Agent Fix (Redux-Persist Rehydration)](#197-stale-agent-fix-redux-persist-rehydration)
    - 19.8 [Session Lifecycle Logging](#198-session-lifecycle-logging)
    - 19.9 [Feedback Tracker & Release Plan](#199-feedback-tracker--release-plan)
    - 19.10 [New Files (v5.0)](#1910-new-files-v50)
    - 19.11 [Modified Files (v5.0)](#1911-modified-files-v50)
    - 19.12 [Bugs Encountered & Resolutions (v5.0)](#1912-bugs-encountered--resolutions-v50)
    - 19.13 [Updated Limitations & Future Work](#1913-updated-limitations--future-work)
20. [v5.1 — Bug Fixes, Checkpoint Evaluation, Budget UI, GUI Logging & Orchestrator Prompt Overhaul](#20-v51--bug-fixes-checkpoint-evaluation-budget-ui-gui-logging--orchestrator-prompt-overhaul)
    - 20.1 [Overview](#201-overview)
    - 20.2 [Stashed Fixes Applied](#202-stashed-fixes-applied)
    - 20.3 [Phased Checkpoint Evaluation System](#203-phased-checkpoint-evaluation-system)
    - 20.4 [Stale LLM Stream Protection](#204-stale-llm-stream-protection)
    - 20.5 [Session Log Enrichment](#205-session-log-enrichment)
    - 20.6 [Checkpoint Eval Token Capture](#206-checkpoint-eval-token-capture)
    - 20.7 [Markdown Table Rendering Fix](#207-markdown-table-rendering-fix)
    - 20.8 [Single-Agent Spawn Logging Fix](#208-single-agent-spawn-logging-fix)
    - 20.9 [Per-Agent & Session Budget UI](#209-per-agent--session-budget-ui)
    - 20.10 [GUI-Driven Orchestrator Logging](#2010-gui-driven-orchestrator-logging)
    - 20.11 [Tool Call Accumulation in LLM Streaming](#2011-tool-call-accumulation-in-llm-streaming)
    - 20.12 [Real API Token Usage Restored](#2012-real-api-token-usage-restored)
    - 20.13 [IPC Timeout & Tool Result Display Truncation](#2013-ipc-timeout--tool-result-display-truncation)
    - 20.14 [Orchestrator System Prompt Overhaul](#2014-orchestrator-system-prompt-overhaul)
    - 20.15 [Chat.tsx Thread Message Class](#2015-chattsx-thread-message-class)
    - 20.16 [New Files (v5.1)](#2016-new-files-v51)
    - 20.17 [Modified Files (v5.1)](#2017-modified-files-v51)
    - 20.18 [Stress Test Results](#2018-stress-test-results)
    - 20.19 [Bugs Encountered & Resolutions (v5.1)](#2019-bugs-encountered--resolutions-v51)
    - 20.20 [Updated Limitations & Future Work](#2020-updated-limitations--future-work)

---

## 1. Executive Summary

RICA (formerly a fork of Continue, the open-source AI coding assistant) was extended with a **multi-agent orchestration system** that enables an LLM orchestrator to decompose complex tasks into subtasks and delegate them to specialized, autonomous sub-agents running in parallel or sequentially.

**Before:** RICA operated in two modes — `chat` (conversational) and `agent` (single-agent with tool access). All tool execution was sequential and single-threaded.

**After:** RICA now supports a third mode — `multi-agent` — where an orchestrator LLM intelligently decomposes tasks and spawns specialized sub-agents (Researcher, Implementer, Tester, Reviewer, Analyst, Planner). Each sub-agent runs autonomously with its own conversation context and full tool access. Multiple agents can execute in parallel via `Promise.all`. A persistent UI panel displays real-time agent status, tool usage with argument details, thinking logs, and results.

### Key Metrics

| Metric | v1.0 | v2.0 | v3.0 | v4.0 | v5.0 |
|--------|------|------|------|------|------|
| New files created | 10 | 11 (+SubAgentRegistry) | 12 (+selectSessionTokens) | 22 (+10 batch/patterns/conflict) | 23 (+MultiAgentLogger) |
| Existing files modified | 22 | 30 | 34 | 40 | 49 (+9 logging/UI/stability) |
| Total lines added | ~2,500+ (new) + ~600 (mods) | ~3,200+ (new) + ~1,100 (mods) | ~3,400+ (new) + ~1,500 (mods) | ~5,900+ (new) + ~1,700 (mods) | ~6,450+ (new) + ~2,300+ (mods) |
| New TypeScript interfaces | 5 | 5 (+sessionId fields) | 6 (+LLMCallDetail) | | 13 (+SessionLog, AgentLog, ToolCallLog, LLMCallLog, MessageSnapshot, APIErrorLog, PromptAssemblyLog, OrchestratorEvent, SessionSummary) |
| New protocol messages | 1 (`subAgentProgress`) | 3 (+`cancelSubAgent`, `killSession`) | 3 | 3 | 4 (+`multiagent/getSessionLog`) |
| New Redux selectors | 0 | 0 | 1 (`selectSessionTokens`) | 1 | 1 |
| New built-in tool | 1 (`builtin_spawn_agent`) | 1 | 1 | 3 (+`spawn_agents`, `analyze_task`) | 3 |
| Agent roles defined | 6 | 6 | 6 | 6 | 6 |
| Max tool iterations per agent | 25 | 25 | 25 | 25 | 25 |
| Token tracking | None | None | Full (API + tiktoken fallback) | Full | Full + structured per-call logging |
| Session logging | None | None | None | None | Full (MultiAgentLogger → JSON files) |

---

## 2. Baseline: RICA Before Multi-Agent

Prior to the multi-agent feature, RICA's architecture was:

### 2.1 Mode System

```
MessageModes = "chat" | "agent"
```

- **Chat mode:** Conversational responses only, no tool access
- **Agent mode:** Single LLM agent with access to built-in tools (read files, edit files, grep, glob, terminal commands, etc.)

### 2.2 Tool Execution

- All tools defined in `core/tools/builtIn.ts` — 12 built-in tools:
  - `builtin_read_file`, `builtin_edit_existing_file`, `builtin_create_new_file`
  - `builtin_run_terminal_command`, `builtin_grep_search`, `builtin_file_glob_search`
  - `builtin_ls`, `builtin_search_web`, `builtin_view_diff`
  - `builtin_read_currently_open_file`, `builtin_create_rule_block`, `builtin_request_rule`
- Tool execution was **strictly sequential** — one tool call at a time
- Tool calls were dispatched via `gui/src/redux/thunks/callToolById.ts` → Core `tools/call` handler → `callTool()` → tool implementation
- No mechanism for parallel tool execution

### 2.3 Conversation Flow

```
User Input → streamNormalInput → LLM response → (if tool calls) → callToolById → streamResponseAfterToolCall → LLM continues
```

Single-threaded, single-agent loop. The LLM could use tools iteratively but had one conversation context and one identity.

### 2.4 UI

- `gui/src/pages/gui/Chat.tsx` rendered chat history as a flat list
- Tool calls rendered inline via `ToolCallDiv` components
- No concept of sub-agent panels, progress tracking, or orchestration visualization

### 2.5 Protocol

- `core/protocol/webview.ts` (`ToWebviewFromIdeOrCoreProtocol`) — messages from Core to GUI
- `core/protocol/core.ts` (`ToCoreFromIdeOrWebviewProtocol`) — messages from GUI to Core
- No sub-agent-related protocol messages existed

### 2.6 Redux State

```typescript
type SessionState = {
  history: ChatHistoryItemWithMessageId[];
  isStreaming: boolean;
  mode: MessageModes;  // "chat" | "agent"
  toolCallStates: ToolCallState[];
  // ... other fields
};
```

No `subAgentProgress` field. No mechanism to track parallel agent state.

---

## 3. Architecture Overview

The multi-agent system follows a **hub-and-spoke** architecture:

```
┌─────────────────────────────────────────────────────────────────┐
│                         GUI (React/Redux)                       │
│                                                                 │
│  ┌─────────────┐  ┌──────────────────┐  ┌───────────────────┐  │
│  │ ModeSelect  │  │ MultiAgentPanel  │  │   Chat History    │  │
│  │ (3 modes)   │  │ (static panel)   │  │ (SpawnAgent tags) │  │
│  └──────┬──────┘  └────────▲─────────┘  └───────────────────┘  │
│         │                  │ subAgentProgress                   │
│         │ mode selection   │ (Redux store)                      │
├─────────┼──────────────────┼────────────────────────────────────┤
│         │                  │        IPC Protocol                │
├─────────┼──────────────────┼────────────────────────────────────┤
│         │                  │                                    │
│  ┌──────▼──────┐  ┌───────┴─────────┐  ┌──────────────────┐   │
│  │  Core.ts    │──│  tools/call      │──│  callTool()      │   │
│  │  (handler)  │  │  (handler)       │  │                  │   │
│  └─────────────┘  └─────────────────┘  └────────┬─────────┘   │
│                                                  │              │
│                              ┌────────────────────▼──────────┐  │
│                              │  spawnAgentImpl               │  │
│                              │  (tool implementation)        │  │
│                              └────────────────┬──────────────┘  │
│                                               │                 │
│                              ┌─────────────────▼─────────────┐  │
│                              │     SubAgentRunner            │  │
│                              │  ┌─────────────────────────┐  │  │
│                              │  │ Isolated LLM context    │  │  │
│                              │  │ Role-specific system msg │  │  │
│                              │  │ Tool loop (max 25 iter) │  │  │
│                              │  │ Progress emission       │  │  │
│                              │  └─────────────────────────┘  │  │
│                              └───────────────────────────────┘  │
│                           Core Backend (Node.js)                │
└─────────────────────────────────────────────────────────────────┘
```

### Key Design Decisions

1. **Sub-agents cannot spawn sub-agents** — `SpawnAgent` is filtered from sub-agent tool lists to prevent unbounded recursion
2. **Sub-agents use the same LLM** as the orchestrator (`config.selectedModelByRole.chat`)
3. **EditExistingFile has a special sub-agent implementation** — writes directly via IDE API instead of going through the GUI diff UX
4. **Progress is streamed via protocol messages** — not through the conversation history
5. **Parallel execution uses `Promise.all`** — auto-approved tool calls fire concurrently
6. **v2.0: SubAgentRegistry is the single source of truth** for active agent state, enabling per-agent and session-level cancellation without relying on frontend state
7. **v2.0: Session-tagged agents** — every agent is tagged with its originating session ID for isolation and batch cleanup
8. **v2.0: Agent progress persists to disk** — `subAgentProgress` is saved with the session and restored on revisit

---

## 4. Implementation Details

### 4.1 Core Backend — New Files

#### `core/tools/multiagent/SubAgentRunner.ts` (578 lines)

The execution engine for sub-agents. Each sub-agent gets:
- Its own `ChatMessage[]` conversation (starting with role-specific system message + task)
- An `AbortController` linked to the parent's abort signal
- Access to all built-in tools except `SpawnAgent`
- A maximum of 25 tool-call iterations before forced completion

**Lifecycle:**

```
constructor(ide, config, fetchFn, parentAbortSignal, sendProgress)
    │
    ▼
run(subAgentId, task, description, role, tools, model)
    │
    ├─ Create child AbortController (linked to parent)
    ├─ Filter tools (remove SpawnAgent)
    ├─ Build conversation: [system message, user task]
    │
    ▼
    ┌─ LOOP (max 25 iterations) ──────────────────┐
    │                                              │
    │  1. Check abort signal                       │
    │  2. Call LLM (streamChat)                    │
    │  3. Capture thinking text                    │
    │  4. If no tool calls → DONE (return result)  │
    │  5. Execute tool calls sequentially:         │
    │     - Track ToolAction (name, args, timing)  │
    │     - Special-case EditExistingFile           │
    │     - Standard callTool for others           │
    │  6. Append tool results to conversation      │
    │  7. Emit progress update                     │
    │                                              │
    └──────────────────────────────────────────────┘
    │
    ▼
Return ContextItem[] with agent's final response
```

**Key methods:**
- `run()` — Main execution loop
- `callLLM()` — Streams LLM response, assembles full content + tool call deltas
- `mergeToolCallDeltas()` — Handles incremental tool call streaming
- `extractToolCalls()` — Validates and normalizes tool calls from LLM response
- `summarizeToolArgs()` — Extracts human-readable summaries from tool arguments (e.g., file paths, search queries)

#### `core/tools/definitions/spawnAgent.ts` (51 lines)

JSON Schema tool definition registered as `builtin_spawn_agent`:

```typescript
{
  name: "builtin_spawn_agent",
  parameters: {
    task: string,         // Detailed instructions for the sub-agent
    description: string,  // One-line UI description
    role?: enum           // "Researcher" | "Implementer" | "Tester" | "Reviewer" | "Analyst" | "Planner"
  }
}
```

UI metadata for rendering in chat:
- `wouldLikeTo`: "delegate a task to a sub-agent: {description}"
- `isCurrently`: "running sub-agent: {description}"
- `hasAlready`: "completed sub-agent task: {description}"

#### `core/tools/implementations/spawnAgent.ts` (57 lines)

Bridge between the tool call system and `SubAgentRunner`:

1. Validates required `task` and `description` parameters
2. Gathers available tools from config (filters out SpawnAgent)
3. Generates a UUID for the sub-agent
4. Creates `SubAgentRunner` with IDE, config, fetch, abort signal, and progress callback
5. Calls `runner.run()` and returns the result as `ContextItem[]`

#### `core/tools/implementations/editFileForSubAgent.ts` (89 lines)

Specialized file editor for sub-agents that bypasses the GUI diff workflow:

- Resolves relative paths against IDE workspace directories
- Detects "lazy placeholders" (e.g., `// ... existing code`, `# ... existing`) via regex
- If placeholders found: uses LLM to merge the agent's proposed changes with existing file content
- If no placeholders: writes content directly via `ide.writeFile()`
- Strips markdown code blocks that LLMs sometimes wrap around code

#### `core/llm/multiAgentSystemMessage.ts` (63 lines)

The system prompt that transforms the LLM into an orchestrator. Defines:

- **Decision framework** — when to reply directly vs. spawn agents
- **Execution patterns** — parallel (independent tasks), sequential (dependent phases), single agent
- **Anti-duplication rules** — no overlapping scopes, split by area not function
- **Role definitions** — 6 specialized roles with descriptions
- **Task description quality** — good/bad examples for specificity

#### `core/tools/multiagent/SubAgentRegistry.ts` (65 lines) — **v2.0 NEW**

Centralized singleton registry for tracking all active sub-agents:

```typescript
class SubAgentRegistry {
  private agents = Map<string, { abortController: AbortController; sessionId?: string }>;

  register(subAgentId, abortController, sessionId?)   // Called on agent start
  unregister(subAgentId)                                // Called on agent completion/error
  cancelAgent(subAgentId): boolean                      // Aborts one agent
  cancelAllForSession(sessionId): number                // Batch-cancel by session
  cancelAll(): number                                   // Nuclear option
}
```

The registry is the backend source of truth for agent lifecycle. The frontend may have stale state (e.g., after a session switch), but the registry always knows which agents are genuinely running.

#### `core/tools/multiagent/index.ts` (1 line)

Barrel export: `export { SubAgentRunner } from "./SubAgentRunner.js";`

### 4.2 Core Backend — Modified Files

#### `core/index.d.ts` — Type Definitions

Added 5 new types/interfaces:

```typescript
export type MessageModes = "chat" | "agent" | "multi-agent";  // was: "chat" | "agent"

export type SubAgentStatus = "running" | "completed" | "errored" | "canceled";

export interface ToolAction {
  toolName: string;
  timestamp: number;
  status: "running" | "completed" | "errored";
  durationMs?: number;
  argSummary?: string;  // Human-readable summary of tool arguments
}

export interface ThinkingEntry {
  iteration: number;
  content: string;
  timestamp: number;
}

export interface SubAgentProgress {
  subAgentId: string;
  parentToolCallId: string;
  task: string;
  description: string;
  role?: string;
  status: SubAgentStatus;
  currentAction?: string;
  stepsCompleted: number;
  result?: string;
  error?: string;
  toolActions: ToolAction[];
  thinkingLog: ThinkingEntry[];
  startedAt: number;
  completedAt?: number;
}
```

Extended `ToolExtras` with sub-agent support:

```typescript
export interface ToolExtras {
  // ... existing fields ...
  abortSignal?: AbortSignal;                                         // NEW
  sendSubAgentProgress?: (progress: SubAgentProgress) => void;       // NEW
}
```

Added `index?: number` to `ToolCallDelta` for parallel tool call matching.

#### `core/core.ts` — Protocol Handler

- **`tools/call` handler** — now accepts `messageId`, creates an `AbortController`, defines `sendSubAgentProgress` callback that forwards progress to GUI via `messenger.send("subAgentProgress", progress)`, and passes both `abortSignal` and `sendSubAgentProgress` through `callTool()`

#### `core/protocol/webview.ts` — Protocol Messages

Added new message type:
```typescript
subAgentProgress: [SubAgentProgress, void];
```

#### `core/protocol/passThrough.ts` — Protocol Pass-Through

Added `"subAgentProgress"` to the pass-through list so VS Code extension forwards it.

#### `core/tools/builtIn.ts` — Built-In Tool Registry

Added:
```typescript
SpawnAgent = "builtin_spawn_agent",
```

#### `core/tools/callTool.ts` — Tool Dispatcher

Passes `extras.abortSignal` and `extras.sendSubAgentProgress` through to tool implementations.

#### `core/tools/index.ts` — Tool Registration

Imports and registers `spawnAgentTool` and `spawnAgentImpl`.

#### `core/llm/countTokens.ts` — Token Counting

Added handling for multi-agent mode system message injection when counting tokens.

#### `core/llm/messages.ts` — Message Construction

Added multi-agent system message to the conversation when mode is `"multi-agent"`.

#### `core/llm/openaiTypeConverters.ts` — LLM Response Parsing

Enhanced `fromChatCompletionChunk` to handle parallel tool call deltas with `index` field for correct tool call matching during streaming.

### 4.3 GUI Frontend — New Files

#### `gui/src/pages/gui/MultiAgentPanel.tsx` (843 lines)

Persistent panel rendered above the chat area. Visible when `subAgentProgress` contains entries.

**Component hierarchy:**
```
MultiAgentPanel
├── PanelHeader (collapsible, shows aggregate status)
├── PanelBody (when expanded)
│   ├── SVG connection lines (animated for running agents)
│   ├── OrchestratorNode (centered pill, clickable)
│   └── AgentCards (flex-wrap row)
│       └── AgentCard (compact: role icon, name, status dot, steps, current action, elapsed time)
├── CollapsedSummary (when collapsed: inline strip)
├── AgentDetailOverlay (modal: full task, thinking log, tool timeline, result)
└── OrchestratorDetailOverlay (modal: aggregate stats across all agents)
```

**Sub-components:**
- `RoleIcon` — Maps roles to Heroicons (MagnifyingGlass, CodeBracket, Beaker, Eye, ChartBar, ClipboardDocumentList, CpuChip)
- `StatusDot` — Animated color dot (blue pulse = running, green = completed, red = errored, yellow = canceled)
- `StatusBadge` — Pill badge with icon and text
- `ToolTimeline` — Chronological list of tool executions with name, arg summary, duration, and status icon
- `ThinkingLog` — Expandable iteration-by-iteration reasoning log with markdown rendering
- `AgentCard` — Compact 144px card showing metrics only (no response text)
- `AgentDetailOverlay` — Full-screen modal with task description, thinking log, tool timeline, result/error

#### `gui/src/pages/gui/ToolCallDiv/MultiAgentTreeView.tsx` (500 lines)

Alternative inline tree view rendered within the chat conversation history (alongside tool calls). Provides expandable agent nodes with SVG connection lines.

#### `gui/src/pages/gui/ToolCallDiv/SubAgentDiv.tsx` (157 lines)

Compact inline component for a single sub-agent's tool call result in chat history. Collapsible header showing status, description, step count, and result.

### 4.4 GUI Frontend — Modified Files

#### `gui/src/pages/gui/Chat.tsx`

Major changes:
1. **MultiAgentPanel mount** — inserted between `TabBar` and `StepsDiv`
2. **subAgentProgress listener** — `useWebviewListener("subAgentProgress", ...)` dispatches to Redux
3. **SpawnAgent-aware rendering** — chat history separates SpawnAgent tool calls from regular tool calls. SpawnAgent calls render as compact inline tags (`"Spawned Researcher: ..."`) instead of full ToolCallDiv components. Regular tool calls render normally.

#### `gui/src/redux/slices/sessionSlice.ts`

1. **New state field:** `subAgentProgress: Record<string, SubAgentProgress>`
2. **New reducer:** `updateSubAgentProgress` — stores progress keyed by `subAgentId`
3. **Fixed `streamUpdate` reducer** — rewrote tool call delta merging logic to handle parallel tool calls correctly:
   - Each tool result gets its own history item (role-based new item condition)
   - Tool call deltas merged by ID matching (not index replacement)
   - Streaming chunks that carry both content and toolCalls processed independently

#### `gui/src/redux/thunks/streamNormalInput.ts`

1. **Multi-agent mode recognition** — added `"multi-agent"` to the mode check
2. **Parallel tool call execution** — when multiple tool calls are auto-approved:
   ```typescript
   if (autoApproved.length > 1) {
     const promises = autoApproved.map((tc) =>
       dispatch(callToolById({ toolCallId: tc.toolCallId })).then(unwrapResult)
     );
     await Promise.all(promises);
   }
   ```
   This enables all SpawnAgent calls to fire concurrently.
3. Sequential fallback for single or non-auto-approved tool calls.

#### `gui/src/redux/thunks/streamResponseAfterToolCall.ts`

Updated to collect tool results for ALL parallel tool calls (not just the first) and send them back to the LLM in a single continuation request. This enables the orchestrator to see all agent results before responding.

#### `gui/src/redux/thunks/callToolById.ts`

Extended to pass `messageId` through the `tools/call` protocol message, enabling abort controller association on the Core side.

#### `gui/src/components/ModeSelect/ModeSelect.tsx`

Added "Multi-Agent" as a third mode option in the dropdown:
```typescript
{ value: "multi-agent", label: "Multi-Agent", icon: <CpuChipIcon /> }
```

#### `gui/src/components/ModeSelect/ModeIcon.tsx`

Added `CpuChipIcon` rendering for the `"multi-agent"` mode.

#### `gui/src/redux/selectors/selectActiveTools.ts`

Added `SpawnAgent` to the active tools list when mode is `"multi-agent"`.

#### `gui/src/redux/slices/uiSlice.ts`

Added default tool setting for `builtin_spawn_agent` as `"allowedWithoutPermission"` — sub-agents auto-execute without user approval.

#### `gui/src/index.css`

Added CSS animations for the multi-agent panel (pulse animation for running status dots).

---

## 5. Data Flow & Sequence Diagrams

### 5.1 Normal Flow (Parallel Agents)

```
User: "Check package.json AND search for TODO comments"
  │
  ▼
GUI: streamNormalInput dispatched
  │
  ▼
Core: LLM called with multi-agent system message
  │
  ▼
LLM Response: 2 SpawnAgent tool calls (parallel)
  ├─ SpawnAgent(role:"Researcher", task:"Read package.json...")
  └─ SpawnAgent(role:"Researcher", task:"Search for TODO comments in...")
  │
  ▼
GUI: streamUpdate reducer stores 2 tool call states
  │
  ▼
GUI: streamNormalInput detects 2 auto-approved tool calls
  │
  ▼
GUI: Promise.all([callToolById(tc1), callToolById(tc2)])
  │                                          │
  ▼                                          ▼
Core: tools/call handler (tc1)    Core: tools/call handler (tc2)
  │                                          │
  ▼                                          ▼
spawnAgentImpl                     spawnAgentImpl
  │                                          │
  ▼                                          ▼
SubAgentRunner.run()               SubAgentRunner.run()
  │  (own LLM context)                │  (own LLM context)
  │  emitProgress ──────┐             │  emitProgress ──────┐
  │                     │             │                     │
  │                     ▼             │                     ▼
  │              messenger.send       │              messenger.send
  │              ("subAgentProgress") │              ("subAgentProgress")
  │                     │             │                     │
  ▼                     ▼             ▼                     ▼
Returns ContextItem[]   GUI updates   Returns ContextItem[]  GUI updates
  │                    Redux store     │                   Redux store
  │                                    │
  └────────────────┬───────────────────┘
                   ▼
GUI: streamResponseAfterToolCall
     (sends both results to LLM)
                   │
                   ▼
LLM: Synthesizes final response
                   │
                   ▼
GUI: Renders orchestrator's summary
```

### 5.2 Protocol Message Flow

```
GUI → Core:                              Core → GUI:
─────────────                            ─────────────
tools/call { toolCall, messageId }       subAgentProgress { subAgentId, status, sessionId, ... }
cancelSubAgent { subAgentId }      →     { success: boolean }
killSession { sessionId }          →     { killedCount: number }
                                         subAgentProgress (repeated per progress update)
                                         toolCallPartialOutput { ... }
```

---

## 6. Type System & Interfaces

### Core Types (core/index.d.ts)

```typescript
// Mode system (extended)
export type MessageModes = "chat" | "agent" | "multi-agent";

// Sub-agent lifecycle status
export type SubAgentStatus = "running" | "completed" | "errored" | "canceled";

// Individual tool execution record within a sub-agent
export interface ToolAction {
  toolName: string;       // e.g., "builtin_grep_search"
  timestamp: number;      // Unix ms when tool execution started
  status: "running" | "completed" | "errored";
  durationMs?: number;    // Time taken for tool execution
  argSummary?: string;    // Human-readable: e.g., '"useState"' for grep, 'App.tsx' for read
}

// Sub-agent reasoning step
export interface ThinkingEntry {
  iteration: number;      // 1-indexed loop iteration
  content: string;        // LLM's text response for this iteration
  timestamp: number;      // Unix ms
}

// Full sub-agent state (streamed to GUI in real-time)
export interface SubAgentProgress {
  subAgentId: string;           // UUID
  parentToolCallId: string;     // Links to orchestrator's SpawnAgent tool call
  task: string;                 // Full task description given to agent
  description: string;          // Short one-line description
  role?: string;                // "Researcher" | "Implementer" | etc.
  status: SubAgentStatus;
  currentAction?: string;       // What the agent is doing right now
  stepsCompleted: number;       // Number of tool calls executed
  result?: string;              // Final response text (when completed)
  error?: string;               // Error message (when errored)
  toolActions: ToolAction[];    // Full timeline of tool executions
  thinkingLog: ThinkingEntry[]; // Iteration-by-iteration reasoning
  startedAt: number;            // Unix ms
  completedAt?: number;         // Unix ms (set on terminal states)
  sessionId?: string;           // v2.0: originating session for isolation
}

// v2.0: Session now persists agent progress
export interface Session {
  sessionId: string;
  title: string;
  workspaceDirectory: string;
  history: ChatHistoryItem[];
  subAgentProgress?: Record<string, SubAgentProgress>;  // v2.0: persisted agent tree
}

// Extended tool extras (core/index.d.ts)
export interface ToolExtras {
  ide: IDE;
  llm: ILLM;
  fetch: FetchFunction;
  tool: Tool;
  toolCallId?: string;
  onPartialOutput?: (...) => void;
  config: ContinueConfig;
  abortSignal?: AbortSignal;                                    // v1.0
  sendSubAgentProgress?: (progress: SubAgentProgress) => void;  // v1.0
  sessionId?: string;                                           // v2.0
}
```

### Redux State (gui/src/redux/slices/sessionSlice.ts)

```typescript
type SessionState = {
  // ... existing fields ...
  subAgentProgress: Record<string, SubAgentProgress>;  // NEW: keyed by subAgentId
};
```

---

## 7. Orchestrator Intelligence

The orchestrator's behavior is defined by the system message in `core/llm/multiAgentSystemMessage.ts`. It evolved through two major iterations:

### Version 1 (Initial — Aggressive Spawning)

```
MANDATORY: For every user request, you MUST spawn at least 2 agents with different roles.
NEVER do work yourself — you ALWAYS delegate.
```

**Problem:** Even "Hi" or "what can you do?" spawned 2 agents unnecessarily.

### Version 2 (Current — Smart Dispatch)

```
DECISION FRAMEWORK:
A) REPLY DIRECTLY — greetings, simple questions, clarifications, trivial requests
B) SPAWN AGENTS — when task requires file access, code changes, or investigation

PHASED EXECUTION:
1) PARALLEL — independent subtasks spawn simultaneously
2) SEQUENTIAL — dependent tasks spawn in phases (research → implement → test)
3) SINGLE AGENT — focused tasks need only one specialist

PREVENTING DUPLICATE WORK:
- Never spawn two agents with same role doing overlapping tasks
- Split by AREA not FUNCTION
- Give explicit file/directory boundaries
```

The phased execution is supported by the existing architecture — after all agents in Phase 1 complete, `streamResponseAfterToolCall` sends their results back to the orchestrator, which can then spawn Phase 2 agents with context from Phase 1.

---

## 8. Agent Roles & Specialization

Each role gets a distinct system message in `SubAgentRunner.ts`:

| Role | System Message Focus | Typical Tools Used |
|------|---------------------|-------------------|
| **Researcher** | Explore codebase, read files, search patterns. Do NOT modify files. Report with file paths and line numbers. | ReadFile, GrepSearch, FileGlobSearch, LS |
| **Implementer** | Write/edit code. Read before changing. Minimal edits, follow conventions. Verify changes compile. | ReadFile, EditExistingFile, CreateNewFile, RunTerminalCommand |
| **Tester** | Run test suites, validate correctness. Report pass/fail clearly. Check regressions. | RunTerminalCommand, ReadFile, GrepSearch |
| **Reviewer** | Check correctness, security, edge cases, code style. Provide specific actionable feedback. | ReadFile, GrepSearch, FileGlobSearch |
| **Analyst** | Investigate bugs, reproduce problems, trace root causes systematically. | ReadFile, GrepSearch, RunTerminalCommand, LS |
| **Planner** | Create step-by-step implementation plans with specific file paths and function names. | ReadFile, GrepSearch, FileGlobSearch, LS |

---

## 9. Parallel Execution Engine

### How It Works

In `gui/src/redux/thunks/streamNormalInput.ts`:

```typescript
// After LLM responds with tool calls:
const autoApproved = toolCallStates.filter(
  (tc) => toolSettings[tc.toolCall.function.name] === "allowedWithoutPermission"
);

if (autoApproved.length > 1) {
  // PARALLEL: fire all at once
  const promises = autoApproved.map((tc) =>
    dispatch(callToolById({ toolCallId: tc.toolCallId })).then(unwrapResult)
  );
  await Promise.all(promises);
} else {
  // SEQUENTIAL: one at a time
  for (const tc of autoApproved) {
    const response = await dispatch(callToolById({ toolCallId: tc.toolCallId }));
    unwrapResult(response);
  }
}
```

### Why SpawnAgent Auto-Approves

In `gui/src/redux/slices/uiSlice.ts`, `builtin_spawn_agent` is set to `"allowedWithoutPermission"` by default, meaning the user is not prompted for each agent spawn.

### Streaming Challenges Solved

Parallel tool calls introduced a streaming challenge: the LLM sends tool call deltas incrementally, and multiple tool calls can arrive interleaved in a single stream. The `sessionSlice.ts` `streamUpdate` reducer was rewritten to:

1. Match deltas by `id` (not array index) to handle out-of-order arrival
2. Merge argument fragments into existing tool call states (not replace)
3. Process content and toolCalls independently (a chunk can carry both)
4. Create separate history items for each tool result (each has a unique `toolCallId`)

---

## 10. Progress Tracking & Real-Time UI

### Data Flow

```
SubAgentRunner (Core)
    │ emitProgress(status, currentAction, result, error)
    ▼
sendSubAgentProgress callback (Core handler)
    │ messenger.send("subAgentProgress", progress)
    ▼
IPC → VS Code Extension → Webview
    ▼
Chat.tsx: useWebviewListener("subAgentProgress")
    │ dispatch(updateSubAgentProgress(data))
    ▼
Redux: state.session.subAgentProgress[subAgentId] = progress
    ▼
MultiAgentPanel: useAppSelector(state => state.session.subAgentProgress)
    │ Re-renders with new data
    ▼
UI: Cards update in real-time (status, steps, current action, elapsed time)
```

### What's Tracked Per Agent

| Field | Updated When | Shown Where |
|-------|-------------|-------------|
| `status` | Every state change | StatusDot, StatusBadge |
| `currentAction` | Every tool execution start | AgentCard, collapsed strip |
| `stepsCompleted` | After each tool call | AgentCard metrics |
| `toolActions[]` | Each tool start/complete | ToolTimeline in detail overlay |
| `thinkingLog[]` | Each LLM iteration | ThinkingLog in detail overlay |
| `result` | On completion | AgentDetailOverlay |
| `error` | On failure | AgentDetailOverlay, AgentCard |
| `startedAt` / `completedAt` | Start / terminal state | Elapsed time display |

---

## 11. Tool Argument Summarization

Added in the final iteration to enrich agent cards with tool context.

### `summarizeToolArgs()` in SubAgentRunner.ts

Extracts human-readable summaries from tool JSON arguments:

| Tool | Extraction Logic | Example Output |
|------|-----------------|----------------|
| `builtin_read_file` | `basename(args.filepath)` | `"App.tsx"` |
| `builtin_grep_search` | `'"' + args.query + '"'` | `'"useState"'` |
| `builtin_file_glob_search` | `args.pattern` | `"**/*.test.ts"` |
| `builtin_run_terminal_command` | `args.command.slice(0, 80)` | `"npm run test"` |
| `builtin_edit_existing_file` | `basename(args.filepath)` | `"Login.tsx"` |
| `builtin_create_new_file` | `basename(args.filepath)` | `"utils.ts"` |
| `builtin_ls` | `args.dirPath` | `"src/components/"` |
| Other/unknown | First string value, 60 chars | varies |

### Where It Appears

1. **`currentAction` field** — enriched to show `"builtin_grep_search: \"useState\""` instead of just `"Using tool: builtin_grep_search"`
2. **`ToolAction.argSummary`** — stored per tool execution, rendered in ToolTimeline:
   ```
   ✓ Grep Search — "useState"              1.2s
   ✓ Read File — App.tsx                    0.3s
   ✓ Edit Existing File — App.tsx           0.8s
   ```

---

## 12. Abort & Cancellation Architecture

### v2.0: Three Independent Abort Chains

A critical architectural insight discovered during hardening: the system has **three fully isolated abort chains** that do not interfere with each other:

```
Chain 1: STREAM CANCEL (user clicks "Cancel" or starts new session)
──────────────────────────────────────────────────────────────────
GUI: cancelStream() → state.streamAborter.abort()
    │
    ▼ passed as cancelToken to streamRequest()
IdeMessenger.streamRequest abort handler
    │ posts "abort" with the STREAM's messageId
    ▼
Core: abortById(streamMessageId) → aborts llm/streamChat controller
    ❌ Does NOT affect tools/call requests (different messageId)

Chain 2: TOOL CALL ABORT (per tools/call request)
──────────────────────────────────────────────────
Core: tools/call handler → addMessageAbortController(toolCallMessageId)
    │
    ▼ passed as extras.abortSignal
SubAgentRunner.parentAbortSignal
    │ linked via addEventListener("abort")
    ▼
SubAgentRunner.abortController (child)
    ❌ Only fires if someone calls abortById(toolCallMessageId)

Chain 3: PER-AGENT / SESSION CANCEL (v2.0 — via SubAgentRegistry)
─────────────────────────────────────────────────────────────────
GUI: post("cancelSubAgent", { subAgentId })
    │
    ▼
Core: SubAgentRegistry.cancelAgent(subAgentId)
    │ entry.abortController.abort()
    ▼
SubAgentRunner detects abort → emits "canceled" → returns cancel message

GUI: post("killSession", { sessionId })
    │
    ▼
Core: SubAgentRegistry.cancelAllForSession(sessionId)
    │ aborts ALL agents tagged with that session
    ▼
All matched runners detect abort → emit "canceled"
```

### Why This Isolation Matters

When a user clicks "Cancel All" in the StreamingToolbar:
1. Each running agent gets `cancelSubAgent` → Chain 3 fires for each
2. Main stream gets `cancelStream()` → Chain 1 fires
3. `callToolById` checks `isStreaming` after tool returns → sees `false` → skips `streamResponseAfterToolCall`
4. Cancel messages from agents are still stored in history but the orchestrator doesn't continue

When a user starts a new session:
1. `startNewSession` dispatches `killSession` → Chain 3 batch-cancels all agents
2. Stream is aborted → Chain 1
3. Session is saved with agents marked "canceled"
4. New session loads clean

### v2.0: Cancel Message Design (Spawn Loop Prevention)

The cancel message returned to the orchestrator went through three iterations:

| Version | Message | Problem |
|---------|---------|---------|
| v1 | "Agent was cancelled. This is not an error." | Orchestrator confused — thought it should retry |
| v2 | "You may spawn a new agent for this or a different task if needed." | **Spawn cascade**: cancelled agents → orchestrator respawns → cancel → respawn → 3→6→9 agents |
| v3 (current) | "Do NOT spawn a new agent for this same task. Do NOT retry or redo this task. The user intentionally stopped it. Use the results you already have from other completed agents and move on." | Works — orchestrator takes over directly or reports what it has |

### SubAgentRunner Registry Integration

```
constructor(ide, config, fetchFn, parentAbortSignal, sendProgress, sessionId?)
    │
    ▼
run(subAgentId, task, description, role, tools, model)
    │
    ├─ Create child AbortController (linked to parent)
    ├─ SubAgentRegistry.register(subAgentId, abortController, sessionId)  ← v2.0
    ├─ Filter tools (remove SpawnAgent)
    │
    ▼
    ┌─ LOOP (max 25 iterations) ──────────────────┐
    │  1. Check abort signal                       │
    │  2. Call LLM → execute tools → emit progress │
    └──────────────────────────────────────────────┘
    │
    ├─ finally: SubAgentRegistry.unregister(subAgentId)  ← v2.0
    ▼
Return ContextItem[]
```

### Behavior on Abort

1. Per-agent cancel or session kill fires → child abort controller triggers
2. SubAgentRunner checks `abortController.signal.aborted` before each iteration
3. If aborted mid-loop:
   - Missing tool results get placeholder messages
   - Agent emits `"canceled"` status with `completedAt` timestamp
   - Returns `ContextItem` with explicit "do not respawn" message
4. Cleanup: parent abort listener removed + registry unregistered in `finally` block

---

## 13. File Change Summary

### New Files (12)

| File | Lines | Purpose | Version |
|------|-------|---------|---------|
| `core/tools/multiagent/SubAgentRunner.ts` | 578 | Sub-agent execution engine | v1.0 + **v3.0** |
| `core/tools/multiagent/SubAgentRegistry.ts` | 65 | Centralized agent registry (singleton) | **v2.0** |
| `core/tools/multiagent/index.ts` | 1 | Barrel export | v1.0 |
| `core/tools/definitions/spawnAgent.ts` | 51 | SpawnAgent tool schema | v1.0 |
| `core/tools/implementations/spawnAgent.ts` | 57 | SpawnAgent tool implementation | v1.0 |
| `core/tools/implementations/editFileForSubAgent.ts` | 89 | Sub-agent file editor | v1.0 |
| `core/llm/multiAgentSystemMessage.ts` | 63 | Orchestrator system prompt | v1.0 |
| `gui/src/pages/gui/MultiAgentPanel.tsx` | 1,170 | Progress panel (expanded from 843) | v1.0 + **v2.0** + **v3.0** |
| `gui/src/pages/gui/ToolCallDiv/MultiAgentTreeView.tsx` | 500 | Inline tree view | v1.0 |
| `gui/src/pages/gui/ToolCallDiv/SubAgentDiv.tsx` | 157 | Inline agent result component | v1.0 |
| `gui/src/redux/selectors/selectSessionTokens.ts` | 137 | Session token totals memoized selector | **v3.0** |
| **Total New** | **~2,868** | |

### Modified Files (34)

| File | Change Summary | Version |
|------|---------------|---------|
| `core/index.d.ts` | +5 types, extended ToolExtras/MessageModes/ToolCallDelta; +`sessionId` on SubAgentProgress/ToolExtras; +`subAgentProgress` on Session; +token fields on PromptLog; +`tokenUsage` on SubAgentProgress | v1.0 + **v2.0** + **v3.0** |
| `core/core.ts` | tools/call handler (abort, progress); +`cancelSubAgent` handler; +`killSession` handler; sessionId capture via `getCurrentSessionId` | v1.0 + **v2.0** |
| `core/protocol/webview.ts` | +`subAgentProgress` message | v1.0 |
| `core/protocol/core.ts` | +`cancelSubAgent`, `killSession` protocol messages | **v2.0** |
| `core/protocol/passThrough.ts` | +`subAgentProgress`, `cancelSubAgent`, `killSession` pass-through | v1.0 + **v2.0** |
| `core/tools/builtIn.ts` | +`SpawnAgent` enum | v1.0 |
| `core/tools/callTool.ts` | Pass through abortSignal, sendSubAgentProgress, sessionId | v1.0 + **v2.0** |
| `core/tools/index.ts` | Register spawnAgentTool and spawnAgentImpl | v1.0 |
| `core/util/history.ts` | `HistoryManager.save()` includes `subAgentProgress` in persisted session | **v2.0** |
| `core/llm/countTokens.ts` | Multi-agent system message token counting | v1.0 |
| `core/llm/messages.ts` | Multi-agent system message injection | v1.0 |
| `core/llm/openaiTypeConverters.ts` | Parallel tool call delta handling with index field | v1.0 |
| `core/config/types.ts` | Mode type update; +PromptLog token fields | v1.0 + **v3.0** |
| `gui/src/pages/gui/Chat.tsx` | MultiAgentPanel mount, subAgentProgress listener, **sessionId filtering**; +SessionTokenBadge (compact + detail card) | v1.0 + **v2.0** + **v3.0** |
| `gui/src/redux/slices/sessionSlice.ts` | subAgentProgress state/reducer, streamUpdate rewrite; `newSession` restores subAgentProgress | v1.0 + **v2.0** |
| `gui/src/redux/slices/uiSlice.ts` | Default auto-approve for SpawnAgent | v1.0 |
| `gui/src/redux/thunks/session.ts` | +`startNewSession` thunk (kill agents + save session + mark cancelled) | **v2.0** |
| `gui/src/redux/thunks/streamNormalInput.ts` | Multi-agent mode, parallel Promise.all execution | v1.0 |
| `gui/src/redux/thunks/streamResponseAfterToolCall.ts` | Collect all parallel tool results; +empty history guard | v1.0 + **v2.0** |
| `gui/src/redux/thunks/callToolById.ts` | messageId for abort; +session guard; +isStreaming guard; +sibling tool call detection | v1.0 + **v2.0** |
| `gui/src/redux/thunks/streamThunkWrapper.tsx` | Multi-agent mode; +intentional stop detection (suppress error dialog) | v1.0 + **v2.0** |
| `gui/src/redux/selectors/selectActiveTools.ts` | SpawnAgent in active tools for multi-agent mode | v1.0 |
| `gui/src/components/ModeSelect/ModeSelect.tsx` | "Multi-Agent" mode option | v1.0 |
| `gui/src/components/ModeSelect/ModeIcon.tsx` | CpuChipIcon for multi-agent | v1.0 |
| `gui/src/components/mainInput/Lump/LumpToolbar/StreamingToolbar.tsx` | Kill dropdown (cancel all, cancel all agents, per-agent cancel) | **v2.0** |
| `gui/src/index.css` | Pulse animation for status dots | v1.0 |
| `gui/src/util/index.ts` | Utility updates | v1.0 |
| `core/llm/index.ts` | +`_lastApiUsage` property; OpenAI adapter usage capture (streaming + non-streaming); `stream_options: { include_usage: true }`; PromptLog return sites with API-prefer-tiktoken-fallback | **v3.0** |
| `core/llm/llms/stubs/RicaProxy.ts` | Rica usage capture (streaming + non-streaming); debug logging | **v3.0** |
| `gui/src/redux/selectors/selectSessionTokens.ts` | **NEW** — memoized selector for session token totals, per-call breakdown, usage source detection | **v3.0** |
| `gui/src/pages/gui/MultiAgentPanel.tsx` | Per-agent token display (card metrics + detail overlay with reasoning/cache/source indicator) | **v3.0** |

---

## 14. v2.0 — Session Lifecycle, Kill Controls & UI Hardening

This section documents all features added after the v1.0 document (2026-04-14 → 2026-04-15).

### 14.1 SubAgentRegistry — Centralized Agent Tracking

**File:** `core/tools/multiagent/SubAgentRegistry.ts` (NEW)

A singleton registry that tracks every active sub-agent with its `AbortController` and `sessionId`. This is the backend source of truth — the frontend may have stale state after a session switch, but the registry always knows what's genuinely running.

**API:**
- `register(subAgentId, abortController, sessionId)` — called in `SubAgentRunner.run()` after creating the child abort controller
- `unregister(subAgentId)` — called in the `finally` block of `SubAgentRunner.run()`
- `cancelAgent(subAgentId): boolean` — aborts one agent, removes from registry
- `cancelAllForSession(sessionId): number` — batch-cancels all agents tagged with a session
- `cancelAll(): number` — global cancel

### 14.2 Protocol Messages for Cancellation

**Files:** `core/protocol/core.ts`, `core/protocol/passThrough.ts`, `core/core.ts`

Two new protocol messages:

```typescript
"cancelSubAgent": [{ subAgentId: string }, { success: boolean }];
"killSession":    [{ sessionId: string },  { killedCount: number }];
```

Core handlers delegate directly to `SubAgentRegistry`. Both are added to `WEBVIEW_TO_CORE_PASS_THROUGH` for the VS Code extension to forward.

### 14.3 Session Isolation — Progress Event Filtering

**File:** `gui/src/pages/gui/Chat.tsx`

The `subAgentProgress` webview listener now compares `data.sessionId` against the current session ID. Progress events from a different session are silently dropped. This prevents agents from a previous session bleeding into the current session's graph tree.

### 14.4 Session-Level Kill on Switch

**File:** `gui/src/redux/thunks/session.ts`

The `startNewSession` thunk now performs a full cleanup sequence:

```
1. dispatch(setInactive())            — mark stream as stopped
2. dispatch(abortStream())            — abort the LLM stream
3. dispatch(clearLastEmptyResponse()) — clean up partial messages
4. post("killSession", { sessionId }) — kill ALL backend agents for old session
5. Save old session to disk:
   - Mark "running" agents as "canceled" with completedAt timestamp
   - Include subAgentProgress in the Session object
   - Fire-and-forget request("history/save", sessionToSave)
6. dispatch(newSession(session))       — switch to new/loaded session
```

This fires on every session switch: new session, load session, load last session.

### 14.5 Session Persistence — Agent Progress Survives Revisits

**Files:** `core/index.d.ts`, `core/util/history.ts`, `gui/src/redux/slices/sessionSlice.ts`, `gui/src/redux/thunks/session.ts`

Changes to make agent tree persist:

1. **`Session` interface** — added `subAgentProgress?: Record<string, SubAgentProgress>`
2. **`HistoryManager.save()`** — the explicit key-ordered session reconstruction now includes `subAgentProgress` (was previously stripped because it wasn't in the ordered object)
3. **`newSession` reducer** — restores `payload.subAgentProgress ?? {}` when loading a session
4. **`saveCurrentSession`** — marks "running" agents as "canceled" before persisting (agents won't be running when revisited)

### 14.6 Per-Agent Kill Buttons

**File:** `gui/src/pages/gui/MultiAgentPanel.tsx`

Running agent cards now display a kill button (StopCircleIcon). Clicking calls `ideMessenger.post("cancelSubAgent", { subAgentId })`. The button uses `e.stopPropagation()` to prevent card click from firing. Kill buttons also appear in the `AgentDetailOverlay` header with a "Cancel" label. Only visible when `agent.status === "running"`.

### 14.7 Kill Options Dropdown (Claude Code-Style)

**File:** `gui/src/components/mainInput/Lump/LumpToolbar/StreamingToolbar.tsx`

When agents are running, the streaming toolbar shows a split button:

- **Main button:** "Cancel All" (keyboard shortcut) — cancels all agents + aborts stream
- **Dropdown chevron** revealing:
  - "Cancel everything" (red XMarkIcon) — abort stream + kill all agents
  - "Cancel all agents (N)" (yellow StopCircleIcon) — kill agents, keep orchestrator stream
  - Divider
  - Per-agent entries (CpuChipIcon) — "Cancel: [Agent Role/Name]" for each running agent
- Click-outside handler closes the dropdown

When no agents are running, shows the simple "Cancel" button (unchanged from v1.0).

### 14.8 Expandable Agent Cards

**File:** `gui/src/pages/gui/MultiAgentPanel.tsx`

Agent cards can be expanded inline via a chevron toggle. Expanded cards show:
- Task description (bordered box)
- ToolTimeline (reused from detail overlay)
- ThinkingLog (last entries, expandable)
- Result/Error section

The SVG connection lines recalculate on expand/collapse via `expandedCards.size` in the dependency array.

### 14.9 Full-Screen Overlay Mode

**File:** `gui/src/pages/gui/MultiAgentPanel.tsx`

A full-screen button (ArrowsPointingOutIcon) opens the entire graph tree in a fixed overlay covering the viewport. Exit via ArrowsPointingInIcon button. Useful when many agents are running and the inline panel is too small.

### 14.10 Live/History Agent Split

**File:** `gui/src/pages/gui/MultiAgentPanel.tsx`

The expanded tree body is split into two sections:

1. **Live agents** (top, scrollable `max-h-[350px]`) — only `status === "running"` agents, with SVG connection lines from the orchestrator node
2. **Agent History** (bottom, collapsible, scrollable `max-h-[200px]`) — completed, errored, and canceled agents with StatusBadge tags

The **collapsed** summary strip shows only live agents. When no agents are running: "No agents running — expand to see history".

### 14.11 Session Guards in Tool Call Thunks

**File:** `gui/src/redux/thunks/callToolById.ts`

Three safety guards prevent orphaned tool results from corrupting state:

1. **Session guard** — captures `sessionIdAtStart`; after tool returns, checks if session changed → skips continuation
2. **Streaming guard** — checks `isStreaming` → if false, stream was intentionally stopped → skips continuation
3. **Sibling guard** — checks if other parallel tool calls are still running → only the last one to finish triggers `streamResponseAfterToolCall`

### 14.12 Intentional Stop Detection

**File:** `gui/src/redux/thunks/streamThunkWrapper.tsx`

The catch block previously showed an error dialog for ALL exceptions, including intentional aborts. Now checks `!getState().session.isStreaming` BEFORE dispatching `cancelStream()`. If already stopped, it was intentional (user started new session, cancelled) — suppress the error dialog.

---

## 15. Bugs Encountered & Resolutions

### 15.1 Spawn Cascade Loop (3 → 6 → 9 agents)

**Symptom:** User cancels 3 agents. Orchestrator respawns 3 new agents for the cancelled tasks. User cancels again. Orchestrator spawns 3 more. Agent count grows: 3 → 6 → 9 → ...

**Root cause:** The cancel message in `SubAgentRunner.ts` said "You may spawn a new agent for this or a different task if needed." The orchestrator interpreted this as instruction to retry.

**Fix:** Changed cancel message to explicitly prohibit respawning: "Do NOT spawn a new agent for this same task. Do NOT retry or redo this task."

**Lesson:** LLM-facing messages must be unambiguous. A permissive cancel message creates an infinite spawn loop because the orchestrator treats "may spawn" as "should spawn."

### 15.2 Error Dialog on New Session ("no user/tool message found")

**Symptom:** Starting a new session while agents are running produced an error dialog: "Error parsing chat history: no user/tool message found" from `compileChatMessages` in `core/llm/countTokens.ts`.

**Root cause chain:**
1. `startNewSession` dispatched `newSession()` which cleared history to `[]`
2. In-flight `streamResponseAfterToolCall` (from a tool call that completed during the switch) tried to compile the now-empty history
3. `compileChatMessages` expects at least one user/tool message → threw

**Fix (multi-layered):**
1. `startNewSession` now dispatches `setInactive()` + `abortStream()` BEFORE `newSession()` — in-flight thunks see `isStreaming=false` and bail
2. `callToolById` checks session ID and streaming state before continuing
3. `streamResponseAfterToolCall` has empty-history guard: `if (initialHistory.length === 0) return`
4. `streamThunkWrapper` detects intentional stops and suppresses error dialog

### 15.3 Session Persistence Not Working

**Symptom:** Agent tree disappeared when revisiting old sessions. Clicking through history showed no agents.

**Root cause:** `core/util/history.ts` `HistoryManager.save()` explicitly reconstructed the session object with only 4 fields (`sessionId`, `title`, `workspaceDirectory`, `history`), stripping `subAgentProgress`.

**Fix:** Added `subAgentProgress` to the ordered session reconstruction in `HistoryManager.save()`.

**Lesson:** When backend serialization explicitly lists fields (instead of spreading), new fields are silently dropped. Always check the serialization layer.

### 15.4 GUI Build Not Deploying

**Symptom:** All GUI changes appeared non-functional. "None of the changes worked."

**Root cause:** Build copy command `cp -r dist ../extensions/vscode/gui` created a nested `gui/dist/` directory instead of placing files at `gui/assets/`, `gui/index.html`, etc.

**Fix:** Changed to `cp -r gui/dist/* extensions/vscode/gui/` (copy contents, not the directory).

**Lesson:** Always verify the deployment path after build. A single wrong `cp` flag means zero UI changes are visible despite correct source code.

### 15.5 Agents Still Running After Session Switch

**Symptom:** Switching sessions or clicking "New Session" left backend agents running, consuming tokens. Returning to old sessions showed agents as "running" when they should have been killed.

**Root cause:** `startNewSession` only checked frontend state (`state.session.subAgentProgress`) to decide whether to kill agents. But the frontend might have already cleared state, and it never saved the session before switching.

**Fix:** 
1. `startNewSession` now unconditionally sends `killSession` (backend registry is source of truth)
2. Before switching, saves the old session with agents marked as "canceled"
3. `saveCurrentSession` also marks running agents as "canceled" before persisting

### 15.6 Cancelled Agent Detection (1 of 3)

**Symptom:** User cancels all 3 agents, but orchestrator only detects 1 as cancelled and respawns for the other 2.

**Root cause:** Two agents completed before the cancel signal arrived (fast-completing tasks like file reads). The orchestrator saw 2 completed results + 1 cancel message.

**Status:** Partially mitigated by the stronger cancel message. The timing issue (fast agents completing before cancel arrives) is inherent to async execution. The orchestrator now uses completed results and doesn't retry cancelled tasks.

---

## 16. Known Limitations & Future Work

### Current Limitations (v2.0)

1. ~~**No per-agent cancellation from GUI**~~ — **RESOLVED in v2.0**: kill buttons, dropdown menu, protocol messages all implemented.

2. ~~**No token usage tracking**~~ — **RESOLVED in v3.0**: full token tracking with real API usage capture, per-agent counts, session totals, and interactive detail card UI.

3. **No progress percentage** — `stepsCompleted` is a count, not a percentage. There's no way to estimate total steps in advance.

4. **No agent-to-agent direct communication** — agents are fully isolated. Coordination happens only through the orchestrator (sequential phases). **Design decision confirmed in v4.0**: isolation model is correct. ContextBridge provides orchestrator-mediated context sharing between sequential phases. See §18.4.

5. ~~**No retry on failure**~~ — **RESOLVED in v4.0**: FaultHandler provides automatic retry with progressive backoff for transient errors (rate limits, timeouts). Non-retryable errors are escalated. See §18.4.

6. **Sub-agents share the same LLM model** — no ability to assign different models to different agent roles.

7. **Debug logging present** — `[PARALLEL-DEBUG]`, `[PARALLEL-DEBUG-CORE]`, `[SubAgent:...]` console.log statements remain. Should be removed or gated behind a debug flag before release.

8. **Cancel timing race** — fast-completing agents may finish before a cancel signal arrives. The frontend shows them as "completed" even if the user intended to cancel everything.

### Future Work

- ~~**Token usage tracking**~~ — **DONE in v3.0**
- ~~**Collaborative workflows**~~ — **DONE in v4.0**: 5 orchestrator patterns including Iterative (test-fix-retest) and Collaborative (multi-perspective review)
- ~~**Persistent agent memory / scratchpad**~~ — **DONE in v4.0**: ContextBridge provides per-session shared context between sequential phases
- ~~**Agent cost estimation**~~ — **PARTIALLY DONE in v4.0**: builtin_analyze_task estimates complexity and token counts pre-spawn
- **Agent attribution in chat** — messages attributed to originating agent with role label
- **Configurable agent roles** — user-defined roles beyond the 6 built-in ones
- **Model selection per role** — assign cheaper/faster models to Researchers, more capable models to Implementers
- **Cancel-all semantics** — option to discard completed results when user cancels "everything", not just running agents
- **Token budget enforcement** — ContextBridge.DEFAULT_BUDGET_FRACTION (0.3) exists but is not wired up. See §18.9
- **Conversation compaction** — sub-agent conversations grow unbounded up to 25 iterations. Mid-conversation summarization would reduce token waste. See §18.9
- **Remove debug logging** — `[TOKEN-SOURCE]`, `[RICA-PROXY][TOKEN-SOURCE]`, `[FileTracker]`, `[SpawnAgents]` console.log statements should be removed or gated behind a debug flag before release

---

## 17. v3.0 — Token Usage Tracking, API Usage Capture & Session Token UI

This section documents all features added after the v2.0 document (2026-04-15). The core goal: give users full visibility into token consumption across all modes (chat, agent, multi-agent) with real API usage data from Rica/Databricks rather than tiktoken estimation.

### 17.1 Problem Statement

Prior to v3.0, RICA had **zero token visibility** in the frontend. Token counts were calculated in `BaseLLM._logEnd()` using tiktoken (local estimation) and logged to SQLite + telemetry, but **never flowed back to the GUI**. The `PromptLog` interface lacked token fields entirely. Sub-agents consumed tokens with no tracking mechanism. Users had no way to know how many tokens a session consumed, whether counts were real or estimated, or how usage distributed across orchestrator vs. sub-agents.

Meanwhile, the Rica/Databricks API **already returns actual token usage** in every response — it was just being ignored.

### 17.2 Architecture: Two Data Paths

Token data flows through two independent pipelines that converge in a single frontend selector:

```
Path 1: Orchestrator / Chat / Agent tokens
─────────────────────────────────────────
Rica API response → _lastApiUsage (BaseLLM) → PromptLog return value
    → ChatHistoryItem.promptLogs[] → Session.history → disk
    → selectSessionTokens selector → SessionTokenBadge UI

Path 2: Sub-agent tokens
────────────────────────
Rica API response → _lastApiUsage (BaseLLM) → PromptLog return value
    → SubAgentRunner.callLLM() captures PromptLog
    → Accumulated per-agent totals → emitProgress() with tokenUsage
    → SubAgentProgress.tokenUsage → Session.subAgentProgress → disk
    → selectSessionTokens selector → SessionTokenBadge UI + MultiAgentPanel cards
```

Both paths persist automatically — `PromptLog` tokens are saved in `ChatHistoryItem.promptLogs[]` within `Session.history`, and sub-agent tokens are saved in `Session.subAgentProgress`. The selector recomputes on-the-fly from persisted data when revisiting old sessions.

### 17.3 Real API Usage Capture (`_lastApiUsage`)

**File:** `core/llm/index.ts` (lines 186-194)

A new protected property on `BaseLLM` captures real usage from the most recent API response:

```typescript
protected _lastApiUsage?: {
  promptTokens: number;
  completionTokens: number;
  reasoningTokens: number;
  totalTokens: number;
  cacheReadTokens: number;
  cacheCreationTokens: number;
};
```

**Lifecycle:**
1. Cleared at the start of every `streamChat()` and `streamComplete()` call: `this._lastApiUsage = undefined;`
2. Populated at **4 capture points** (see 17.4)
3. Read at PromptLog return sites — if populated, real values used; if `undefined`, falls back to tiktoken estimation

### 17.4 Four Usage Capture Points

#### Capture Point 1: OpenAI Adapter — Non-Streaming (`core/llm/index.ts` ~line 968)

```typescript
if ((response as any).usage) {
  const u = (response as any).usage;
  this._lastApiUsage = {
    promptTokens: u.prompt_tokens ?? 0,
    completionTokens: u.completion_tokens ?? 0,
    reasoningTokens: u.reasoning_tokens ?? 0,
    totalTokens: u.total_tokens ?? (u.prompt_tokens ?? 0) + (u.completion_tokens ?? 0),
    cacheReadTokens: u.cache_read_input_tokens ?? 0,
    cacheCreationTokens: u.cache_creation_input_tokens ?? 0,
  };
}
```

#### Capture Point 2: OpenAI Adapter — Streaming (`core/llm/index.ts` ~line 994)

Streaming requires `stream_options: { include_usage: true }` in the request body (added at line 989). The usage object appears on the **last chunk only**. Each chunk overwrites `_lastApiUsage`, so the final value has real completion counts.

#### Capture Point 3: RicaProxy — Non-Streaming (`core/llm/llms/stubs/RicaProxy.ts` ~line 367)

Rica's non-streaming response includes `data.usage` with the same field names as OpenAI. Captured with debug logging:
```
[RICA-PROXY][TOKEN-SOURCE] Non-streaming API usage FOUND: {...}
```

An else branch logs when usage is missing:
```
[RICA-PROXY][TOKEN-SOURCE] Non-streaming API usage NOT found. Response keys: [...]
```

#### Capture Point 4: RicaProxy — Streaming (`core/llm/llms/stubs/RicaProxy.ts` ~line 396)

**Discovery:** Rica streaming SSE chunks include `usage` on **every** chunk, but `completion_tokens` and `total_tokens` are `null` on intermediate chunks — only populated on the **last chunk**. Our code overwrites `_lastApiUsage` on every chunk (last one wins with real values). Debug logging:
```
[RICA-PROXY][TOKEN-SOURCE] Streaming chunk usage FOUND: {...}
```

### 17.5 Rica API Usage Format

Rica/Databricks returns usage in OpenAI-compatible format with Anthropic-style cache fields:

```json
{
  "usage": {
    "prompt_tokens": 786,
    "completion_tokens": 93,
    "total_tokens": 879,
    "reasoning_tokens": 116,
    "cache_read_input_tokens": 0,
    "cache_creation_input_tokens": 0
  }
}
```

**Key findings during implementation:**
- `reasoning_tokens` — present in response, represents thinking/reasoning tokens consumed by the model
- `cache_read_input_tokens` / `cache_creation_input_tokens` — Anthropic-style cache fields. Currently always `0` for the user's model/endpoint (caching not active), but the UI correctly hides cache display when values are 0
- Streaming: intermediate chunks have `completion_tokens: null`, `total_tokens: null`; only the last chunk has real values
- `prompt_tokens` is populated on all chunks (same value repeated)

### 17.6 PromptLog Type Extension

**Files:** `core/index.d.ts` (line 425), `core/config/types.ts` (line 389)

```typescript
export interface PromptLog {
  modelTitle: string;
  completionOptions: CompletionOptions;
  prompt: string;
  completion: string;
  promptTokens?: number;       // NEW v3.0
  completionTokens?: number;   // NEW v3.0
  reasoningTokens?: number;    // NEW v3.0
  totalTokens?: number;        // NEW v3.0
  cacheReadTokens?: number;    // NEW v3.0
  cacheCreationTokens?: number; // NEW v3.0
  /** true = actual API usage from provider; false/undefined = tiktoken estimation */
  isActualUsage?: boolean;     // NEW v3.0
}
```

### 17.7 PromptLog Population (Prefer API, Fallback to Tiktoken)

**File:** `core/llm/index.ts` (lines 1076-1098)

Both `streamChat()` and `streamComplete()` return sites use the same pattern:

```typescript
const hasApiUsage = !!this._lastApiUsage;
const promptTokens = this._lastApiUsage?.promptTokens ?? this.countTokens(prompt);
const completionTokens = this._lastApiUsage?.completionTokens ?? this.countTokens(completion);
const reasoningTokens = this._lastApiUsage?.reasoningTokens ?? 0;
const totalTokens = this._lastApiUsage?.totalTokens ?? (promptTokens + completionTokens);
const cacheReadTokens = this._lastApiUsage?.cacheReadTokens ?? 0;
const cacheCreationTokens = this._lastApiUsage?.cacheCreationTokens ?? 0;

return {
  modelTitle: this.title ?? completionOptions.model,
  prompt, completion, completionOptions,
  promptTokens, completionTokens, reasoningTokens, totalTokens,
  cacheReadTokens, cacheCreationTokens,
  isActualUsage: hasApiUsage,
};
```

The `isActualUsage` boolean allows the frontend to distinguish real API counts from tiktoken estimates.

### 17.8 SubAgentProgress Token Tracking

**File:** `core/index.d.ts` (line 481), `core/tools/multiagent/SubAgentRunner.ts`

Added `tokenUsage` to `SubAgentProgress`:

```typescript
tokenUsage?: {
  promptTokens: number;
  completionTokens: number;
  reasoningTokens: number;
  totalTokens: number;
  cacheReadTokens: number;
  cacheCreationTokens: number;
  isActualUsage: boolean;
};
```

**SubAgentRunner changes:**

1. **Accumulators** (line 207-212): Six counters initialized at `run()` start:
   ```typescript
   let totalPromptTokens = 0;
   let totalCompletionTokens = 0;
   let totalReasoningTokens = 0;
   let totalCacheReadTokens = 0;
   let totalCacheCreationTokens = 0;
   let hasActualUsage = false;
   ```

2. **`callLLM()` return type** (line 529): Extended to return all token fields:
   ```typescript
   Promise<{ message: ChatMessage; promptTokens: number; completionTokens: number;
     reasoningTokens: number; cacheReadTokens: number; cacheCreationTokens: number;
     isActualUsage: boolean }>
   ```

3. **PromptLog capture** (line 551): The generator's return value (`result.value` when `result.done === true`) is the `PromptLog`. Previously ignored at line 526 — now captured and destructured.

4. **Accumulation** (lines 288-293): After each LLM call in the main loop:
   ```typescript
   totalPromptTokens += llmResult.promptTokens;
   totalCompletionTokens += llmResult.completionTokens;
   totalReasoningTokens += llmResult.reasoningTokens;
   totalCacheReadTokens += llmResult.cacheReadTokens;
   totalCacheCreationTokens += llmResult.cacheCreationTokens;
   if (llmResult.isActualUsage) hasActualUsage = true;
   ```

5. **Progress emission** (lines 238-246): Every `emitProgress()` call includes the running `tokenUsage` totals, so the frontend sees live-updating counts as the agent works.

### 17.9 Session Token Selector

**NEW file:** `gui/src/redux/selectors/selectSessionTokens.ts` (137 lines)

A memoized Redux selector (`createSelector`) computing session-wide token totals from persisted data. Takes two inputs: `state.session.history` and `state.session.subAgentProgress`.

**Exports:**

```typescript
export interface LLMCallDetail {
  index: number;
  model: string;
  promptTokens: number;
  completionTokens: number;
  reasoningTokens: number;
  cacheReadTokens: number;
  cacheCreationTokens: number;
  totalTokens: number;
  isActualUsage: boolean;
}
```

**Return shape:**
```typescript
{
  orchestratorTokens: {
    promptTokens, completionTokens, reasoningTokens,
    cacheReadTokens, cacheCreationTokens, total
  },
  agentTokens: { /* same shape */ },
  sessionTotal: number,       // orchestratorTotal + agentTotal
  totalReasoning: number,
  totalCacheRead: number,
  totalCacheCreation: number,
  usageSource: "api" | "estimated" | "mixed",
  llmCalls: LLMCallDetail[],  // Per-call breakdown
  llmCallCount: number,
  agentCount: number,
}
```

**How `usageSource` is determined:**
- `"api"` — all calls have `isActualUsage: true`
- `"estimated"` — all calls used tiktoken (no API usage available)
- `"mixed"` — some calls have API usage, some don't (e.g., different providers or fallback scenarios)

### 17.10 Session Token Badge UI

**File:** `gui/src/pages/gui/Chat.tsx` (lines 80-234)

A clickable token counter visible in **all modes** (chat, agent, multi-agent), positioned at the top-right of the chat area.

**Sub-components (local to file):**

| Component | Purpose |
|-----------|---------|
| `formatTokenCount(n)` | Compact display: `879` / `1.2k` / `45.3M` |
| `formatExact(n)` | Locale-formatted for detail card: `1,234` |
| `SourceBadge` | Colored pill: green "Real API Usage" / yellow "Mixed (API + Estimated)" / gray "Estimated (tiktoken)" |
| `TokenRow` | Reusable row for label/value pairs with optional color and indent. Returns `null` when value is 0 (auto-hides empty rows). |
| `SessionTokenBadge` | Main component with compact badge + expandable detail card |

**Compact badge:** Plain clickable white text (not a button/panel), showing `"{total} tokens {source}"`. Source indicator: green "API" / yellow "~API" / gray "~est".

```
                                                879 tokens API
```

**Detail card (click to expand):** Absolute-positioned dropdown with click-outside-to-close behavior:

```
┌────────────────────────────────────────┐
│ Session Token Usage    [Real API Usage]│
├────────────────────────────────────────┤
│ SUMMARY                       2 calls  │
│ Total                            879   │
│ Prompt (input)                   786   │ ← blue
│ Completion (output)               93   │ ← green
│ Reasoning                        116   │ ← purple
│ Cache read (reused)               --   │ ← sky (hidden when 0)
│ Cache write (new)                 --   │ ← sky (hidden when 0)
├────────────────────────────────────────┤
│ BREAKDOWN (if agents exist)            │
│ Orchestrator / Chat              500   │
│   Prompt                         400   │
│   Completion                     100   │
│ Sub-agents                       379   │
│   Prompt                         386   │
│   Completion                      93   │
├────────────────────────────────────────┤
│ LLM CALLS              ranked by usage │
│ #1  Claude-4.5 Sonnet   411 / 84  API │
│ #2  Gemini 3.1 Flash    375 / 9   API │
└────────────────────────────────────────┘
```

**LLM Calls section:**
- Sorted by `totalTokens` descending (highest usage first)
- Numbered sequentially based on sort order (#1, #2, ...), not original call order
- Each row: `#{rank}  {model}  {prompt} / {completion}  {API|est}`
- "ranked by usage" italic hint next to the section header
- Scrollable with `max-h-40` for long sessions

### 17.11 Per-Agent Token Display on MultiAgentPanel

**File:** `gui/src/pages/gui/MultiAgentPanel.tsx`

#### Agent Card Metrics Line (~line 628)

Compact token count appended after step count:

```
3 steps · 1.2k tok API
```

The `API` / `~est` indicator uses green/gray coloring to match the session badge.

#### Agent Detail Overlay (~line 306)

Full token breakdown when expanded:

```
In: 786    Out: 93    R: 116    Cache: 0/0    Total: 879 tokens    API
```

- `In:` = promptTokens (default zinc text)
- `Out:` = completionTokens (default zinc text)
- `R:` = reasoningTokens (purple, only shown when > 0)
- `Cache:` = cacheRead/cacheCreation (sky blue, only shown when either > 0, tooltip explains "read" vs "write")
- `Total:` = totalTokens (lighter zinc)
- `API` / `~est` indicator (emerald / gray)

### 17.12 Modified Files Summary (v3.0)

| File | Change | Version |
|------|--------|---------|
| `core/index.d.ts` | +7 fields on `PromptLog` (promptTokens, completionTokens, reasoningTokens, totalTokens, cacheReadTokens, cacheCreationTokens, isActualUsage); +`tokenUsage` object on `SubAgentProgress` | **v3.0** |
| `core/config/types.ts` | Mirror `PromptLog` token fields | **v3.0** |
| `core/llm/index.ts` | +`_lastApiUsage` property; cleared in streamChat/streamComplete; 2 OpenAI adapter capture points (non-streaming + streaming with `stream_options`); 2 PromptLog return sites with API-prefer-tiktoken-fallback pattern | **v3.0** |
| `core/llm/llms/stubs/RicaProxy.ts` | 2 capture points (non-streaming + streaming); debug logging | **v3.0** |
| `core/tools/multiagent/SubAgentRunner.ts` | Token accumulators in `run()`; extended `callLLM()` return type + PromptLog capture; `emitProgress()` includes `tokenUsage` | **v3.0** |
| `gui/src/redux/selectors/selectSessionTokens.ts` | **NEW** — memoized selector for session token totals, per-call breakdown, usage source detection | **v3.0** |
| `gui/src/pages/gui/Chat.tsx` | +`SessionTokenBadge` component (compact badge + expandable detail card); +`formatTokenCount`, `formatExact`, `SourceBadge`, `TokenRow` helpers; imports `selectSessionTokens` | **v3.0** |
| `gui/src/pages/gui/MultiAgentPanel.tsx` | Per-agent token display on cards (metrics line) + detail overlay (full breakdown with reasoning, cache, source indicator) | **v3.0** |

### 17.13 Persistence

Token counts persist automatically through existing mechanisms:

- **PromptLog tokens** — saved in `ChatHistoryItem.promptLogs[]` → `Session.history` → disk (via `HistoryManager.save()`)
- **SubAgentProgress tokens** — saved in `Session.subAgentProgress` → disk (added in v2.0)
- **Session totals** — computed on-the-fly by `selectSessionTokens` from persisted data. No separate storage needed.
- **Old sessions** — loading restores `history` + `subAgentProgress`, selector recomputes totals immediately. Sessions created before v3.0 will show 0 tokens (no `promptTokens` field in their PromptLogs).

### 17.14 Bugs Encountered & Resolutions (v3.0)

#### 17.14.1 Rica Streaming Usage Format Discovery

**Symptom:** During initial implementation, it was assumed the streaming usage object would appear only on the last chunk (OpenAI behavior). Testing showed the `_lastApiUsage` was sometimes populated with `completion_tokens: 0`.

**Root cause:** Rica sends `usage` on **every** SSE chunk, but `completion_tokens` and `total_tokens` are `null` on intermediate chunks — only the last chunk has real values. The code was correctly overwriting `_lastApiUsage` on each chunk, but early during debugging it was unclear why intermediate values showed `null`.

**Resolution:** No code change needed — the overwrite-on-every-chunk pattern naturally handles this (last chunk wins). Added detailed debug logging (`[RICA-PROXY][TOKEN-SOURCE]`) to confirm behavior. Documented the Rica streaming format for future reference.

**Lesson:** Don't assume all OpenAI-compatible APIs follow the exact same streaming behavior. Rica sends usage on every chunk; OpenAI sends it only on the last chunk (and only with `stream_options: { include_usage: true }`).

#### 17.14.2 Token Badge Styled as Panel/Button

**Symptom:** User reported the session token badge looked like a button/panel, requested plain clickable text.

**Root cause:** Initial implementation used `<button>` with background color, hover effects, border-radius, and a chevron icon — making it look like a UI control.

**Fix:** Changed from `<button>` to `<div>` with `cursor-pointer`, `text-white/80`, removed background/hover/chevron. Now appears as plain text that happens to be clickable.

**Lesson:** Token/status indicators should be unobtrusive — plain text with cursor-pointer is more appropriate than button styling for always-visible metrics.

#### 17.14.3 LLM Call Numbering After Sort

**Symptom:** After sorting LLM calls by usage (descending), the numbers showed original call order (#2, #1) instead of sorted rank (#1, #2).

**Root cause:** Used `call.index` (the original call sequence number) instead of the map iteration index after sorting.

**Fix:** Changed from `call.index` to `i + 1` where `i` is the `.map()` callback index:
```typescript
{[...llmCalls].sort((a, b) => b.totalTokens - a.totalTokens).map((call, i) => (
  <span>#{i + 1}</span>
))}
```

#### 17.14.4 Cache Tokens Always Zero

**Symptom:** Cache read and cache creation tokens consistently showed 0 in the UI despite being captured.

**Root cause:** Not a bug — the user's model/endpoint (Claude-4.5 Sonnet via Rica) does not have prompt caching active. The `cache_read_input_tokens` and `cache_creation_input_tokens` fields are present in Rica responses but always `0`.

**Resolution:** No code fix needed. The `TokenRow` component returns `null` when value is 0, so cache rows are automatically hidden. The UI will display them when caching is enabled for the endpoint.

### 17.15 Debug Logging Inventory (v3.0)

The following console.log statements were added for debugging and **remain in the codebase**. They should be removed or gated behind a debug flag before production release:

| File | Log Prefix | Purpose |
|------|-----------|---------|
| `core/llm/index.ts:1085` | `[TOKEN-SOURCE] streamChat PromptLog:` | Logs source (REAL API / TIKTOKEN EST) and all token counts at PromptLog return |
| `core/llm/llms/stubs/RicaProxy.ts:369` | `[RICA-PROXY][TOKEN-SOURCE] Non-streaming API usage FOUND:` | Confirms Rica non-streaming response has usage |
| `core/llm/llms/stubs/RicaProxy.ts:379` | `[RICA-PROXY][TOKEN-SOURCE] Non-streaming API usage NOT found.` | Warns when Rica non-streaming response lacks usage |
| `core/llm/llms/stubs/RicaProxy.ts:396` | `[RICA-PROXY][TOKEN-SOURCE] Streaming chunk usage FOUND:` | Logs every Rica streaming chunk that has usage (verbose — fires per chunk) |

---

## 18. v4.0 — Batch Spawn, Orchestrator Patterns, Conflict Detection & Token Management

This section documents all features added after v3.0 (2026-04-15 → 2026-04-16). The core goals: (1) enable true batch agent spawning with a dedicated tool and UI, (2) provide the orchestrator with multiple execution patterns, (3) add infrastructure for agent coordination — ContextBridge, deduplication, retry, task analysis, and (4) implement pre-flight and post-flight file conflict detection.

### 18.1 Overview

v4.0 transforms RICA's multi-agent system from a basic "spawn one agent at a time" model into a production-grade orchestration platform with:

- **Batch spawning** — `builtin_spawn_agents` (plural) spawns multiple agents in a single tool call via `Promise.all()`
- **5 orchestrator patterns** — Auto, Sequential, Parallel, Iterative, Collaborative — each with tailored system messages
- **ContextBridge** — per-session shared context store for passing data between sequential agent phases
- **AgentManager** — centralized lifecycle management with task deduplication, scope extraction, and retry tracking
- **FaultHandler** — automatic retry with progressive backoff for transient errors
- **TaskDecomposer** — pre-spawn task analysis: complexity scoring, pattern suggestion, subtask decomposition, file overlap detection
- **FileTracker** — post-flight conflict detection for concurrent file edits
- **BatchAgentsDiv** — animated robot UI for batch spawn tool calls with proportional status coloring

### 18.2 Batch Agent Spawning (`builtin_spawn_agents`)

**Problem:** v1.0-v3.0 only had `builtin_spawn_agent` (singular). Spawning N agents required N separate tool calls, each processed sequentially by the streaming thunk. There was no true batch mechanism.

**Solution:** New `builtin_spawn_agents` (plural) tool that accepts an `agents` array and launches all agents simultaneously via `Promise.all()`.

#### Tool Definition

**File:** `core/tools/definitions/spawnAgents.ts` (70 lines)

```
Name: builtin_spawn_agents
Parameters:
  agents: Array of {
    task: string       — detailed task instructions
    description: string — short summary for UI display
    role?: string      — Researcher | Implementer | Tester | Reviewer | Analyst | Planner
    context?: string   — shared context from prior agents
  }
```

#### Implementation

**File:** `core/tools/implementations/spawnAgents.ts` (245 lines)

Flow:
1. Validate input (non-empty array, ≤ `MAX_BATCH_SIZE=10`)
2. Filter sub-agent tools (exclude `SpawnAgent`, `SpawnAgents`, `AnalyzeTask` from sub-agents)
3. For each agent spec:
   - Check for duplicates via `AgentManager.checkDuplicate()` (task hash matching)
   - If duplicate and completed → return cached result
   - If duplicate and running → return "already running" message
   - Else → register agent, push orchestrator context to ContextBridge
4. `Promise.all(promises)` — all non-duplicate agents run in parallel
5. Post-flight conflict detection via `FileTracker.detectConflicts()`
6. Build combined result with summary header + all agent results + conflict warnings

#### Registration in Tool System

- `core/tools/builtIn.ts` — `SpawnAgents = "builtin_spawn_agents"` enum value
- `core/tools/callTool.ts` — dispatch case for `BuiltInToolNames.SpawnAgents`
- `core/tools/index.ts` — `spawnAgentsTool` registered in `baseToolDefinitions`

### 18.3 Orchestrator Pattern System

**Problem:** v1.0-v3.0 had a single system message for the orchestrator that gave generic instructions. The orchestrator had no structured guidance on when to use parallel vs sequential execution, or how to handle iterative workflows.

**Solution:** 5 distinct orchestrator system messages, one per pattern, with shared building blocks and pattern-specific execution rules.

**File:** `core/llm/multiAgentSystemMessage.ts` (319 lines)

#### Shared Building Blocks

| Block | Purpose |
|-------|---------|
| `ORCHESTRATOR_PREAMBLE` | Identity: "You are a multi-agent orchestrator" |
| `DECISION_FRAMEWORK` | When to reply directly vs spawn agents |
| `AGENT_ROLES` | Role definitions with parameter names |
| `TASK_DESCRIPTIONS` | Good vs bad task scoping examples |
| `ADVANCED_CAPABILITIES` | Task planning, context passing, naming, error handling |
| `BASE_RULES` | Core rules for all patterns |

#### Pattern Descriptions

| Pattern | Export | When Used | Key Rules |
|---------|--------|-----------|-----------|
| **Auto** | `DEFAULT_MULTI_AGENT_SYSTEM_MESSAGE` | Default — LLM picks best approach | 5 execution modes described; "MINIMIZE AGENT COUNT" guidance; prefer single agent for ≤5 files |
| **Sequential** | `SEQUENTIAL_SYSTEM_MESSAGE` | Tasks with dependencies | One agent at a time; must continue the chain; pass context between phases |
| **Parallel** | `PARALLEL_SYSTEM_MESSAGE` | Independent subtasks | ONE call to `builtin_spawn_agents` with ALL agents; no overlap; split by area |
| **Iterative** | `ITERATIVE_SYSTEM_MESSAGE` | Test-fix-retest cycles | Phases: Investigate → Implement → Validate → retry (max 3 cycles) |
| **Collaborative** | `COLLABORATIVE_SYSTEM_MESSAGE` | Multi-perspective review | Multiple reviewers with DIFFERENT focus areas on SAME target; classify findings as CRITICAL/NON-CRITICAL |

#### Agent Count Optimization

Added to Auto pattern to prevent unnecessary agent spawning:

```
MINIMIZE AGENT COUNT:
- Use the FEWEST agents that can accomplish the task well
- Do NOT split "gather info" and "analyze info" into separate agents
- Only split when subtasks are genuinely INDEPENDENT and LARGE enough
- For a folder with ≤5 files, ONE agent should handle the entire task
```

### 18.4 Advanced Agent Infrastructure

#### ContextBridge — Per-Session Shared Context

**File:** `core/tools/multiagent/ContextBridge.ts` (232 lines)

Per-session singleton that stores context entries for sharing between sequential agent phases. The orchestrator writes context; agents read it.

```
ContextBridge.forSession(sessionId)
  .pushContext(key, content, priority)     — orchestrator writes
  .getContextForAgent(agentId, budget?)    — agent reads (priority-sorted, budget-aware)
  .pushAgentResult(agentId, name, result)  — system writes after agent completes
  .destroySession(sessionId)              — cleanup
```

- Token-aware: entries have `tokenCount` (estimated at ~4 chars/token)
- Auto-summarization: results >2000 tokens get extractive summaries (first 5 lines + last 3)
- Priority system: orchestrator context (priority 7) > agent results (priority 3)

**Current limitation:** `getContextForAgent()` is called without a `tokenBudget` parameter, so all context is included unbounded. See §18.9.

#### AgentManager — Lifecycle & Deduplication

**File:** `core/tools/multiagent/AgentManager.ts` (340 lines)

Global singleton managing agent records:

- **Registration:** `registerAgent()` creates `AgentRecord` with UUID, task hash, scope, display name
- **Deduplication:** `checkDuplicate()` matches `SHA256(role + ":" + normalizedTask)` against existing agents in same session
- **Scope extraction:** regex-based file/directory path extraction from task text
- **Display names:** auto-generated as `Role-N (scope)` (e.g., "Researcher-1 (frontend)")
- **Status tracking:** running → completed | errored | canceled

#### FaultHandler — Automatic Retry

**File:** `core/tools/multiagent/FaultHandler.ts` (217 lines)

Classifies errors and decides on retry strategy:

- **Retryable:** rate limits, timeouts, network errors → retry up to 3 times with progressive backoff (2s, 4s, 8s)
- **Non-retryable:** auth errors, permission failures, logical errors → escalate immediately
- **Escalation:** builds structured failure report with all attempt history and suggestions

#### runAgentWithRetry — Execution Wrapper

**File:** `core/tools/multiagent/runAgentWithRetry.ts` (116 lines)

Shared by both `spawnAgent` (singular) and `spawnAgents` (batch):

1. Creates `SubAgentRunner` with session ID and progress callback
2. Runs agent with full params (task, role, tools, model, display name, context bridge session)
3. On success → pushes result to ContextBridge for downstream agents
4. On error → FaultHandler classifies and decides retry/escalate
5. On cancel → respects cancellation, no retry

### 18.5 Task Analysis Tool (`builtin_analyze_task`)

**File (definition):** `core/tools/definitions/analyzeTask.ts` (39 lines)
**File (implementation):** `core/tools/implementations/analyzeTask.ts` (28 lines)
**File (logic):** `core/tools/multiagent/TaskDecomposer.ts` (559 lines)

Pre-spawn analysis tool that the orchestrator calls before spawning agents for complex tasks.

#### TaskDecomposer Analysis Pipeline

```
analyzeTask(task, ide, additionalPaths?)
  ├─ extractPaths(task)              — regex-based file/dir path extraction
  ├─ analyzeFiles(paths[], ide)      — read file sizes, line counts, token estimates
  ├─ estimateComplexity(task, files)  — scoring: simple | moderate | complex
  ├─ suggestPattern(task)            — keyword-based: sequential | parallel | iterative | collaborative | auto
  ├─ decomposeTask(task, files)      — split into SubtaskPlans by numbered lists, bullets, conjunctions
  ├─ extractDependencies(subtasks)   — heuristic: "based on" / "fix" implies dependency on prior subtask
  ├─ detectFileOverlaps(subtasks)    — cross-check subtask scopes for shared files (v4.0)
  └─ generateWarnings(files, task, overlaps) — large files, destructive ops, file conflicts (v4.0)
```

#### Complexity Scoring

| Factor | Score |
|--------|-------|
| Task length > 100 words | +2 |
| Task length > 50 words | +1 |
| Files > 5 | +2 |
| Files > 2 | +1 |
| Total tokens > 50k | +2 |
| Total tokens > 10k | +1 |
| Individual file > 1000 lines | +1 |
| Multiple actions (≥3 conjunctions) | +2 |
| Result: simple (< 2), moderate (2-4), complex (≥ 5) | |

#### Output Format

The orchestrator receives formatted markdown with:
- Complexity level and suggested pattern
- FILE CONFLICTS DETECTED section (if overlaps found) — see §18.6
- ACTION REQUIRED (if multiple files detected)
- File size table
- Suggested subtasks with roles, scopes, dependencies, token estimates
- Dependency graph

### 18.6 Pre-flight File Overlap Detection

**Problem:** The orchestrator could decompose a task into subtasks where two subtasks target the same file. Spawning parallel agents for overlapping files causes race conditions — the last write silently overwrites the first.

**Solution:** `TaskDecomposer.detectFileOverlaps()` cross-checks subtask scopes after decomposition. If two subtasks share a file path, a prominent warning is generated.

**Implementation** (in `TaskDecomposer.ts`):

```typescript
private detectFileOverlaps(subtasks: SubtaskPlan[]): FileOverlap[] {
  // Build file → subtask[] map from each subtask's scope
  // Return entries where 2+ subtasks target the same normalized path
}
```

**Warning format:**
```
### FILE CONFLICTS DETECTED
- **manual-testing-sandbox/test.js** is targeted by subtasks T1, T2

**Action**: Merge conflicting subtasks into one agent, or switch to sequential pattern.
```

**Orchestrator behavior (observed):** Upon seeing this warning, the orchestrator merges conflicting subtasks into a single agent and runs non-conflicting subtasks in parallel. This eliminates ~90% of potential file conflicts before any agent runs.

### 18.7 Post-flight Conflict Detection (FileTracker)

**Problem:** Pre-flight detection catches planned overlaps, but LLM agents are unpredictable — an agent might edit files not mentioned in its original task scope.

**Solution:** `FileTracker` records every file edit with agent identity and content hashes. After `Promise.all()` completes, `detectConflicts()` finds files edited by multiple agents.

#### FileTracker

**File:** `core/tools/multiagent/FileTracker.ts` (81 lines)

Per-session singleton (same pattern as ContextBridge):

```typescript
interface FileEdit {
  filepath: string;        // Normalized (forward slashes, lowercase)
  agentId: string;
  agentName: string;
  timestamp: number;
  contentHashBefore: string;  // Hash of file when agent read it
  contentHashAfter: string;   // Hash of file after agent wrote it
}

interface FileConflict {
  filepath: string;
  edits: FileEdit[];  // All edits, sorted by timestamp
}

FileTracker.forSession(sessionId)
  .recordEdit(edit)         — called by editFileForSubAgent after each write
  .detectConflicts()        — returns files where 2+ different agents made edits
  .destroySession(sessionId) — cleanup on killSession
```

#### Edit Recording Points

1. **EditExistingFile** — `editFileForSubAgent.ts` hashes content before/after write, records to FileTracker with agent ID, name, session ID, timestamp
2. **CreateNewFile** — `SubAgentRunner.ts` records creates to FileTracker after successful `callTool()` for CreateNewFile

#### Conflict Detection Flow

```
spawnAgents Promise.all() completes
  └─ FileTracker.forSession(sessionId).detectConflicts()
     ├─ Groups edits by normalized filepath
     ├─ Filters to files with 2+ unique agent IDs
     └─ Returns FileConflict[] sorted by filepath, edits sorted by timestamp

  If conflicts found:
    └─ Inserts warning ContextItem into results:
       "FILE CONFLICTS DETECTED: test.js edited by Implementer-1, Implementer-2
        Edit order: Implementer-1 (12:45:03) -> Implementer-2 (12:45:05)
        Action: Review files. Consider re-running with sequential pattern."
```

#### Session Cleanup

`core/core.ts` killSession handler calls `FileTracker.destroySession(sessionId)` alongside `ContextBridge.destroySession(sessionId)` and `SubAgentRegistry.cancelAllForSession(sessionId)`.

### 18.8 BatchAgentsDiv — Animated Robot UI

**Problem:** The `builtin_spawn_agents` tool call displayed as a generic spinner with raw JSON args in the chat UI.

**Solution:** Custom `BatchAgentsDiv` component with animated robot icons, floating popup card, and proportional status coloring.

**File:** `gui/src/pages/gui/ToolCallDiv/BatchAgentsDiv.tsx` (248 lines)

#### Layout

**While running:**
```
[robot1 robot2 robot3]  3 agents working...
```
- Up to 3 animated robot SVGs with staggered bounce animation (0s, 0.2s, 0.4s delay)
- `+N` badge if more than 3 agents
- Robots area is the only click target (not the full row)

**While completed:**
```
[robot1 robot2 robot3]  3 agents completed
```
- Static robots with proportional color coding

#### Floating Popup Card

Clicking robots opens a floating popup with:
- Header: "Parallel Agents" or "Cancelled by user"
- Count: `N/M done, K cancelled`
- Per-agent rows: chevron expand + status icon + role badge + description
- Expandable task detail per agent

#### Proportional Robot Coloring

When agents finish with mixed statuses (some completed, some cancelled, some errored), the 3 visible robots proportionally represent the distribution:

```typescript
const errorSlots = erroredCount > 0 ? Math.ceil((erroredCount / agents.length) * shown) : 0;
const cancelSlots = canceledCount > 0 ? Math.ceil((canceledCount / agents.length) * shown) : 0;
// Priority: red (errored) → yellow (cancelled) → green (completed)
```

Examples:
- 7 agents, 1 cancelled → 1 yellow, 2 green robots
- 7 agents, 3 cancelled → 2 yellow, 1 green robots
- 7 agents, 7 cancelled → 3 yellow robots

#### Cancellation State

Derived from individual agent progress entries (not `toolCallState.status`, which shows "done" even when all agents were cancelled):

```typescript
const hasCanceled = canceledCount > 0;
const allCanceled = agents.length > 0 && canceledCount === agents.length;
// Guard: agents.length > 0 prevents flash of "cancelled" while args are still parsing
```

#### CSS Animations

**File:** `gui/src/index.css`

```css
@keyframes robot-work {
  0%, 100% { transform: translateY(0); }
  50% { transform: translateY(-3px); }
}
.animate-robot-1 { animation: robot-work 1.2s ease-in-out infinite; }
.animate-robot-2 { animation: robot-work 1.2s ease-in-out 0.2s infinite; }
.animate-robot-3 { animation: robot-work 1.2s ease-in-out 0.4s infinite; }
```

#### Registration

**File:** `gui/src/pages/gui/ToolCallDiv/FunctionSpecificToolCallDiv.tsx`

```typescript
case BuiltInToolNames.SpawnAgents:
  return <BatchAgentsDiv toolCallState={toolCallState} />;
```

### 18.9 Token Management Analysis & Recommendations

A comprehensive analysis of token usage across the multi-agent system revealed three areas of inefficiency and corresponding recommendations.

#### Current Token Tracking (Working Well)

- SubAgentRunner tracks 6 token metrics per agent (prompt, completion, reasoning, total, cache read, cache creation)
- Tokens accumulated per LLM iteration and reported in SubAgentProgress
- `isActualUsage` flag distinguishes real API usage from tiktoken estimation
- `selectSessionTokens` memoized selector aggregates across orchestrator + all agents
- UI displays per-agent and session-level token counts

#### Inefficiency 1: Unbounded Context Inclusion (High Impact)

**Problem:** `ContextBridge.getContextForAgent()` is called in `SubAgentRunner.ts` without a `tokenBudget` parameter. The method defaults to returning ALL stored context entries.

```typescript
// SubAgentRunner.ts:212 — no budget passed
const sharedContext = bridge.getContextForAgent(subAgentId);
```

`DEFAULT_BUDGET_FRACTION = 0.3` exists in ContextBridge.ts but is **never wired up**. In sequential patterns with many phases, each later agent receives the full cumulative output of all previous agents — growing unbounded.

**Recommendation:** Pass `model.contextLength * 0.3` as tokenBudget:
```typescript
const sharedContext = bridge.getContextForAgent(subAgentId, model.contextLength * 0.3);
```

**Estimated savings:** 30-60% reduction in initial context for sequential agents beyond phase 2.

#### Inefficiency 2: No Conversation Compaction (Medium Impact)

**Problem:** SubAgentRunner appends messages indefinitely for up to 25 iterations. Tool results (often 500-1000 tokens each) accumulate in the conversation history. By iteration 15, every LLM call includes the full history of all prior iterations.

`compileChatMessages()` in `core/llm/index.ts` does prune old messages when the context window is exceeded, but this is a blunt instrument — it removes entire messages rather than summarizing them.

**Recommendation:** After every N iterations (e.g., 8), summarize older tool results into a compact "work done so far" message:
```
Iterations 1-8 summary:
- Read src/utils.ts (245 lines)
- Found 3 issues: missing null check L42, unused import L3, wrong return type L78
- Fixed null check at L42
```

**Estimated savings:** 10K-25K tokens per long-running agent (15+ iterations).

#### Inefficiency 3: All Tools Sent Every Request (Low Impact)

**Problem:** Every LLM call in SubAgentRunner includes all tool definitions (~10+ tokens per tool, repeated up to 25x). Tools the agent has never used and is unlikely to use still consume context.

**Recommendation:** After iteration 5, prune tool definitions to only include tools the agent has actually used + a small "discovery" set. Low priority — the per-request overhead is small.

**Estimated savings:** 5-10% per request (~100-500 tokens per iteration in a 25-iteration agent).

#### Token Metrics Summary

| Parameter | Value | Location |
|-----------|-------|----------|
| MAX_TOOL_ITERATIONS | 25 | SubAgentRunner.ts:23 |
| MAX_BATCH_SIZE | 10 agents | spawnAgents.ts:18 |
| SUMMARY_TOKEN_THRESHOLD | 2000 | ContextBridge.ts:20 |
| DEFAULT_BUDGET_FRACTION | 0.3 (unused) | ContextBridge.ts:23 |
| DEFAULT_CONTEXT_LENGTH | 8192 | core/llm/constants.ts:2 |
| DEFAULT_MAX_TOKENS | 4096 | core/llm/constants.ts:1 |
| MIN_RESPONSE_TOKENS | 1000 | countTokens.ts:308 |
| TOKEN_SAFETY_PROPORTION | 0.02 | countTokens.ts |

### 18.10 New Files (v4.0)

| File | Lines | Purpose |
|------|-------|---------|
| `core/tools/multiagent/ContextBridge.ts` | 232 | Per-session shared context store |
| `core/tools/multiagent/AgentManager.ts` | 340 | Agent lifecycle, deduplication, scope extraction |
| `core/tools/multiagent/FaultHandler.ts` | 217 | Error classification and retry logic |
| `core/tools/multiagent/FileTracker.ts` | 81 | Post-flight file conflict detection |
| `core/tools/multiagent/TaskDecomposer.ts` | 559 | Pre-spawn task analysis and decomposition |
| `core/tools/multiagent/runAgentWithRetry.ts` | 116 | Retry wrapper shared by singular/batch spawn |
| `core/tools/definitions/spawnAgents.ts` | 70 | Tool definition for batch spawn |
| `core/tools/definitions/analyzeTask.ts` | 39 | Tool definition for task analysis |
| `core/tools/implementations/spawnAgents.ts` | 245 | Batch spawn implementation with conflict detection |
| `core/tools/implementations/analyzeTask.ts` | 28 | Task analysis implementation |
| `core/llm/multiAgentSystemMessage.ts` | 319 | 5 orchestrator pattern system messages |
| `gui/src/pages/gui/ToolCallDiv/BatchAgentsDiv.tsx` | 248 | Animated robot UI for batch spawn |
| **Total new** | **2,494** | |

### 18.11 Modified Files (v4.0)

| File | Changes |
|------|---------|
| `core/tools/builtIn.ts` | +`SpawnAgents`, `AnalyzeTask` enum values |
| `core/tools/callTool.ts` | +dispatch cases for SpawnAgents, AnalyzeTask |
| `core/tools/index.ts` | +register `spawnAgentsTool`, `analyzeTaskTool` |
| `core/tools/implementations/editFileForSubAgent.ts` | +FileTracker import, `simpleHash()`, extended extras type with `agentId/agentName/sessionId`, content hashing before/after write, edit recording |
| `core/tools/multiagent/SubAgentRunner.ts` | +FileTracker/resolveRelativePathInDir imports, pass agent identity to editFileForSubAgentImpl, track CreateNewFile writes to FileTracker |
| `core/core.ts` | +FileTracker import, `FileTracker.destroySession()` in killSession handler |
| `gui/src/pages/gui/ToolCallDiv/FunctionSpecificToolCallDiv.tsx` | +BatchAgentsDiv import, SpawnAgents case in switch |
| `gui/src/index.css` | +robot-work keyframe animation, .animate-robot-1/2/3 classes |

### 18.12 Updated Limitations & Future Work

#### Resolved in v4.0

| Limitation | Resolution |
|------------|-----------|
| No retry on failure | FaultHandler with 3 retries + progressive backoff |
| No agent coordination | ContextBridge for sequential context passing |
| No task analysis | builtin_analyze_task with TaskDecomposer |
| No conflict detection | Pre-flight (TaskDecomposer) + Post-flight (FileTracker) |
| Generic spawn UI | BatchAgentsDiv with animated robots and proportional status |
| Single execution pattern | 5 patterns: Auto, Sequential, Parallel, Iterative, Collaborative |
| Over-spawning agents | "MINIMIZE AGENT COUNT" guidance + single-agent preference for small tasks |

#### Remaining Limitations

1. **ContextBridge budget not enforced** — `DEFAULT_BUDGET_FRACTION` exists but `getContextForAgent()` called without budget. Unbounded context growth in sequential patterns.
2. **No conversation compaction** — sub-agent conversations grow up to 25 iterations without summarization.
3. **No model selection per role** — all sub-agents use the same LLM model as the orchestrator.
4. **Double path in resolveRelativePathInDir** — file URIs sometimes contain doubled directory segments (e.g., `manual-testing-sandbox/manual-testing-sandbox/test.js`). Not breaking (both agents get same doubled path, so conflict detection still works) but should be fixed.
5. **Debug logging remains** — `[FileTracker]`, `[SpawnAgents]`, `[TOKEN-SOURCE]`, `[RICA-PROXY]` console.log statements in production code.

---

## 19. v5.0 — Structured Session Logging, Log Export UI & Stability Fixes

This section documents all features added after v4.0 (2026-04-16 → 2026-04-23). The core goals: (1) add end-to-end structured session logging for post-mortem debugging, (2) give testers a way to export and share session logs, (3) fix stability issues found during Phase 1 internal testing.

**Commits:**
- `832e8e6ea` — feat: add MultiAgentLogger for granular sub-agent session tracing
- `575c2cab8` — feat: wire live logging into SubAgentRunner and spawnAgents
- `c0e06a221` — feat: compress old session logs, keep last 20 uncompressed
- `cd1d8beda` — feat: add orchestrator, IPC, API error, and prompt assembly logging
- `083e03d17` — feat: add session log export UI, stale agent fix, and feedback tracker

**Tracker Reference:** ENH-002 (Fixed), tracked in `docs/RICA_Multi_Agent_Feedback_Tracker.xlsx`

### 19.1 Overview

Prior to v5.0, the only debugging artifacts available were:
- **13 `console.log` statements** in SubAgentRunner — ephemeral, visible only in VS Code Extension Host output during debug, gone when session closes
- **Redux state dumps** — the chat history JSON from the GUI, containing only final messages — no tool call details, no token breakdowns, no timing, no LLM call internals

v5.0 introduces **MultiAgentLogger**, a structured session tracing system that captures every step of multi-agent workflows and persists them as JSON files. This is the same distributed tracing pattern used by production LLM observability tools (LangSmith, Braintrust, Arize Phoenix) — implemented in-house because RICA is internal and cannot ship data to external services.

**What's now captured per session:**

| Layer | Data Captured |
|-------|---------------|
| Session | sessionId, userPrompt, model, workflowPattern, startedAt, completedAt |
| Per Agent | agentId, displayName, role, task, status, iterations, stepsCompleted, durationMs |
| Token Usage | prompt, completion, total, cacheRead per agent, budget tracking, exhaustion events |
| Tool Calls | toolName, args (full), argSummary, durationMs, status, resultFull, error |
| LLM Calls | iteration, durationMs, messagesCount, systemMessageLength, promptTokens, completionTokens, cacheReadTokens, cacheCreationTokens, isActualUsage, responseText, toolCallsReturned |
| Message Snapshots | Per-iteration: message count, roles, estimated tokens, content previews |
| Prompt Assembly | systemMessageLength, userMessageLength, contextBridgeItems, contextBridgeChars, totalEstimatedTokens |
| API Errors | errorType (rate_limit/overloaded/timeout/network/auth/server_error/unknown), statusCode, errorMessage, durationMs |
| Orchestrator | tool_call_received/completed/error events, IPC sent/received events |
| Summary | totalAgents, completedAgents, erroredAgents, canceledAgents, totalTokens, totalDurationMs, scopeOverlapsDetected, budgetExhaustions, apiErrors |

### 19.2 MultiAgentLogger — Structured Session Tracing

**File:** `core/tools/multiagent/MultiAgentLogger.ts` (555 lines) — NEW

Singleton-per-session logger that accumulates structured data throughout the multi-agent workflow lifecycle.

#### Architecture

```
MultiAgentLogger.forSession(sessionId)  ← singleton factory
    │
    ├── SessionLog (top-level)
    │     ├── sessionId, startedAt, completedAt
    │     ├── userPrompt, model, workflowPattern
    │     ├── events: LogEvent[]           ← timeline of all events
    │     ├── orchestratorEvents: OrchestratorEvent[]
    │     ├── agents: AgentLog[]           ← finalized from agentLogs map
    │     └── summary: SessionSummary      ← computed at finalize()
    │
    ├── agentLogs: Map<agentId, AgentLog>  ← mutable during session
    │     ├── toolCalls: ToolCallLog[]
    │     ├── llmCalls: LLMCallLog[]
    │     ├── messageSnapshots: MessageSnapshot[]
    │     ├── apiErrors: APIErrorLog[]
    │     └── promptAssembly: PromptAssemblyLog
    │
    ├── flushLive()   ← writes JSON after every event (live_*.json)
    ├── writeToDisk() ← finalizes + writes stable filename, removes live file
    └── cleanup()     ← removes singleton instance
```

#### Key Interfaces

```typescript
interface SessionLog {
  sessionId: string;
  startedAt: number;
  completedAt?: number;
  userPrompt?: string;
  model?: string;
  workflowPattern?: string;
  agents: AgentLog[];
  events: LogEvent[];
  orchestratorEvents: OrchestratorEvent[];
  summary?: SessionSummary;
}

interface AgentLog {
  agentId: string;
  displayName: string;
  role?: string;
  task: string;
  status: "running" | "completed" | "errored" | "canceled";
  startedAt: number;
  completedAt?: number;
  durationMs?: number;
  iterations: number;
  stepsCompleted: number;
  tokenUsage: {
    prompt: number;
    completion: number;
    total: number;
    cacheRead: number;
    budget: number;
    budgetExhausted: boolean;
  };
  toolCalls: ToolCallLog[];
  llmCalls: LLMCallLog[];
  messageSnapshots: MessageSnapshot[];
  apiErrors: APIErrorLog[];
  promptAssembly?: PromptAssemblyLog;
  pruningEvents: number;
  resultFull?: string;
  error?: string;
}

interface ToolCallLog {
  iteration: number;
  step: number;
  toolName: string;
  args: string;          // full JSON args
  argSummary?: string;   // human-readable summary
  startedAt: number;
  durationMs: number;
  status: "completed" | "errored";
  resultFull: string;    // full tool result
  error?: string;
}

interface LLMCallLog {
  iteration: number;
  timestamp: number;
  durationMs: number;
  messagesCount: number;
  systemMessageLength: number;
  promptTokens: number;
  completionTokens: number;
  cacheReadTokens: number;
  cacheCreationTokens: number;
  isActualUsage: boolean;
  responseText: string;
  toolCallsReturned: Array<{ name: string; argsPreview: string }>;
}

interface APIErrorLog {
  agentId: string;
  iteration: number;
  timestamp: number;
  errorType: "rate_limit" | "overloaded" | "timeout" | "network" | "auth" | "server_error" | "unknown";
  statusCode?: number;
  errorMessage: string;
  durationMs: number;
}

interface PromptAssemblyLog {
  systemMessageLength: number;
  userMessageLength: number;
  contextBridgeItems: number;
  contextBridgeChars: number;
  totalEstimatedTokens: number;
  systemMessagePreview: string;   // first 500 chars
  userMessagePreview: string;     // first 500 chars
}

interface OrchestratorEvent {
  timestamp: number;
  type: "tool_call_received" | "tool_call_completed" | "tool_call_error" | "ipc_sent" | "ipc_received";
  toolName?: string;
  sessionId?: string;
  durationMs?: number;
  resultSize?: number;
  error?: string;
  data?: Record<string, any>;
}

interface SessionSummary {
  totalAgents: number;
  completedAgents: number;
  erroredAgents: number;
  canceledAgents: number;
  totalTokens: number;
  totalDurationMs: number;
  scopeOverlapsDetected: number;
  budgetExhaustions: number;
  apiErrors: number;
}
```

#### Logger Methods

| Method | When Called | What It Captures |
|--------|-----------|------------------|
| `setUserPrompt(prompt)` | streamChat.ts on user message | User's request (first 500 chars) |
| `setModel(model)` | streamChat.ts / spawnAgents.ts | Provider/model string |
| `setWorkflowPattern(pattern)` | spawnAgents.ts at batch spawn | "parallel", "sequential", "auto" |
| `agentStarted(...)` | SubAgentRunner.run() start | Agent ID, name, role, task, budget |
| `agentSystemPrompt(...)` | SubAgentRunner before first LLM call | Full system message |
| `agentEnrichedTask(...)` | SubAgentRunner after context bridge | Original vs enriched task, bridge injected |
| `agentToolsAvailable(...)` | SubAgentRunner before first LLM call | List of tool names |
| `agentIteration(...)` | SubAgentRunner after each LLM call | Running token totals |
| `agentToolCall(...)` | SubAgentRunner after each tool execution | Full args, result, duration, status |
| `agentLLMCall(...)` | SubAgentRunner after each LLM response | Token breakdown, response text, tool calls returned |
| `agentMessageSnapshot(...)` | SubAgentRunner before each LLM call | Full message array state |
| `agentPromptAssembled(...)` | SubAgentRunner before first iteration | System + user message sizes, context bridge stats |
| `agentBudgetExhausted(...)` | SubAgentRunner when budget hit | Tokens used vs budget |
| `agentMessagesPruned(...)` | SubAgentRunner after pruning | Before/after message counts |
| `agentCompleted(...)` | SubAgentRunner on success | Full result text |
| `agentErrored(...)` | SubAgentRunner on error | Error message (500 chars) |
| `agentCanceled(...)` | SubAgentRunner on abort | Timestamp |
| `agentAPIError(...)` | SubAgentRunner on LLM API error | Classified error type, status code, duration |
| `orchestratorToolCall(...)` | core.ts tools/call handler | Tool name, session ID, duration, result size |
| `ipcEvent(...)` | core.ts IPC events | Event type, data |
| `scopeOverlapDetected(...)` | spawnAgents.ts pre-spawn validation | Overlapping files and agents |
| `batchSpawnStarted(...)` | spawnAgents.ts before Promise.all | Agent count, descriptions |
| `batchSpawnCompleted(...)` | spawnAgents.ts after Promise.all | Duration |
| `flushLive()` | After every event | Live JSON snapshot to disk |
| `writeToDisk()` | On session kill / agent completion | Final JSON, removes live file |
| `finalize()` | Called by writeToDisk | Computes SessionSummary |

### 19.3 Logger Integration — SubAgentRunner & spawnAgents

**File:** `core/tools/multiagent/SubAgentRunner.ts` — 63 lines added in `575c2cab8`

Integration points in the SubAgentRunner execution loop:

```
run(subAgentId, task, ...)
  │
  ├─ sessionLogger.agentStarted(...)           ← before try block
  ├─ sessionLogger.agentSystemPrompt(...)
  ├─ sessionLogger.agentEnrichedTask(...)
  ├─ sessionLogger.agentToolsAvailable(...)
  │
  ├─ LOOP:
  │   ├─ sessionLogger.agentMessageSnapshot(...)  ← before LLM call
  │   ├─ [LLM call with timing]
  │   ├─ sessionLogger.agentLLMCall(...)          ← after LLM response
  │   ├─ sessionLogger.agentIteration(...)        ← running token totals
  │   │
  │   ├─ [tool execution with timing]
  │   ├─ sessionLogger.agentToolCall(...)         ← after each tool
  │   │
  │   ├─ [check completion → sessionLogger.agentCompleted(...)]
  │   └─ [check budget → sessionLogger.agentBudgetExhausted(...)]
  │
  ├─ catch: sessionLogger.agentErrored(...) / agentCanceled(...)
  └─ finally: sessionLogger.writeToDisk()
```

**File:** `core/tools/implementations/spawnAgents.ts` — 15 lines added

```
spawnAgentsImpl(args, extras)
  │
  ├─ logger.batchSpawnStarted(count, descriptions)
  ├─ Promise.all(agents)
  ├─ logger.batchSpawnCompleted(count, duration)
  │
  └─ [also sets: logger.setModel(), setUserPrompt(), setWorkflowPattern()]
```

### 19.4 Orchestrator & IPC Observability

**File:** `core/core.ts` — 54 lines added in `cd1d8beda`

Added logging around the `tools/call` handler — the entry point for all tool calls from the orchestrator:

```
on("tools/call", ...)
  │
  ├─ logger.orchestratorToolCall("tool_call_received", toolName, sessionId)
  ├─ [callTool() execution with timing]
  ├─ logger.orchestratorToolCall("tool_call_completed", toolName, ..., durationMs, resultSize)
  └─ on error: logger.orchestratorToolCall("tool_call_error", toolName, ..., error)
```

**File:** `core/llm/streamChat.ts` — 25 lines added in `cd1d8beda`

Logs the orchestrator's entry point:
- User prompt text (first 500 chars)
- Selected model (provider/model)
- Sets these on `MultiAgentLogger.forSession(sessionId)` so session-level fields are populated before sub-agents run

**File:** `gui/src/redux/thunks/callToolById.ts` — 15 lines added

GUI-side IPC timing:
- Logs when tool call request is sent (timestamp)
- Logs when response arrives (timestamp, status, content size)
- Logs empty array responses specifically (relevant to BUG-001 debugging)

### 19.5 Log Retention & Compression

**File:** `core/tools/multiagent/MultiAgentLogger.ts` — `writeToDisk()` method

Three-tier retention policy:

| Tier | Sessions | Format | Access |
|------|----------|--------|--------|
| Recent | Last 20 | `.json` (uncompressed) | Instant — read directly |
| Archived | 21–50 | `.json.gz` (gzip compressed) | Decompress when needed |
| Expired | 51+ | Deleted | N/A |

Implementation in `writeToDisk()`:
```typescript
// Sort all non-live .json files reverse chronologically
const jsonFiles = fs.readdirSync(logDir)
  .filter(f => f.endsWith(".json") && !f.startsWith("live_"))
  .sort().reverse();

// Compress sessions 21-50
for (const file of jsonFiles.slice(20, 50)) {
  const content = fs.readFileSync(fullPath);
  const compressed = zlib.gzipSync(content);
  fs.writeFileSync(fullPath + ".gz", compressed);
  fs.unlinkSync(fullPath);
}

// Delete anything beyond 50
const allLogFiles = fs.readdirSync(logDir)
  .filter(f => f.endsWith(".json") || f.endsWith(".json.gz"))
  .sort().reverse();
for (const old of allLogFiles.slice(50)) {
  fs.unlinkSync(path.join(logDir, old));
}
```

**Storage location:** `~/.rica-multiagent-logs/`

**Live logging:** `flushLive()` writes a `live_*.json` file after every event for real-time visibility during active sessions. The live file is deleted when `writeToDisk()` produces the final log.

**Stable filenames:** `writeToDisk()` uses a `finalLogPath` instance property based on `startTime` to avoid creating duplicate files when called multiple times per session.

### 19.6 Session Log Export UI

**File:** `gui/src/pages/gui/Chat.tsx` — `SessionTokenBadge` component extended

Two additions to the expanded token detail panel:

#### Session ID Display

Shows the short session ID (first 8 chars) in the panel header. Click to copy to clipboard.

```
Session Token Usage    [Real API Usage]    c2732ee1 📋
```

States: default → "copied!" (1.5s timeout) → default

#### Save Log Button

A "save log" button next to the session ID. Triggers server-side file copy.

```
⬇ save log    →    saving...    →    ✓ saved to session_logs/
                                     (or: no log found)
```

**IPC flow:**

```
GUI: ideMessenger.request("multiagent/getSessionLog", {
       sessionId, workspacePath: window.workspacePaths[0]
     })
  │
  ▼
passThrough.ts: "multiagent/getSessionLog" ← MUST be registered here
  │
  ▼
core.ts handler:
  1. Read ~/.rica-multiagent-logs/
  2. Find latest file matching sessionId (sort reverse, pick first)
  3. Resolve workspace path from GUI-provided workspacePath
  4. Create {workspace}/session_logs/ directory
  5. Copy log to {workspace}/session_logs/{shortId}.json
  6. Return { found: true, savedTo: path }
  │
  ▼
GUI: Shows "✓ saved to session_logs/" (3s timeout → reset)
```

**Protocol definition** (`core/protocol/core.ts`):
```typescript
"multiagent/getSessionLog": [
  { sessionId: string; workspacePath?: string },
  { found: boolean; savedTo?: string },
];
```

**Why server-side copy instead of browser download:** VS Code webview sandboxing blocks `document.createElement('a').click()` — the standard browser download API does not work in webview context. The file must be written by the core process.

**Why `workspacePath` is passed from GUI:** Calling `this.ide.getWorkspaceDirs()` inside an `on()` handler causes an IPC deadlock (core→IDE request inside webview→core request). The GUI sends `window.workspacePaths[0]` directly.

**Windows file URI handling:** `workspacePaths[0]` returns a file URI like `file:///c%3A/Users/...`. The handler decodes it with `decodeURIComponent(new URL(uri).pathname)` and strips the leading `/` on win32.

### 19.7 Stale Agent Fix (Redux-Persist Rehydration)

**Problem:** After F5 restart (or extension reload), redux-persist rehydrates the last session's state. Tool calls that were mid-flight come back as `status: "calling"` or `"generated"` and render as stuck "working..." spinners in the UI. Sub-agents show as perpetually "running".

**File:** `gui/src/redux/store.ts` — `sanitizeStaleToolCalls` transform

```typescript
const sanitizeStaleToolCalls = createTransform(
  (inbound: any) => inbound,                    // write: no-op
  (outbound: any, key) => {                      // read: sanitize
    if (key !== "session" || !outbound?.history) return outbound;
    const history = outbound.history.map((item: any) => {
      if (!item.toolCallStates) return item;
      const toolCallStates = item.toolCallStates.map((tc: any) => {
        if (tc.status === "calling" || tc.status === "generated") {
          return { ...tc, status: "canceled" };
        }
        return tc;
      });
      return { ...item, toolCallStates };
    });
    return { ...outbound, history };
  },
);
```

Added to `persistConfig.transforms` array alongside existing `saveSubsetFilters`.

**File:** `gui/src/redux/slices/sessionSlice.ts` — `newSession` reducer

When restoring a session via payload, marks any "running" sub-agents as "canceled":

```typescript
const restored = payload.subAgentProgress ?? {};
for (const id of Object.keys(restored)) {
  if (restored[id].status === "running") {
    restored[id] = { ...restored[id], status: "canceled", completedAt: Date.now() };
  }
}
state.subAgentProgress = restored;
```

### 19.8 Session Lifecycle Logging

**File:** `core/core.ts` — `killSession` handler updated

```
on("killSession", async (msg) => {
  const { sessionId } = msg.data;
  SubAgentRegistry.cancelAllForSession(sessionId);
  ContextBridge.destroySession(sessionId);
  FileTracker.destroySession(sessionId);
  // v5.0: Finalize and write logger before cleanup
  try {
    const logger = MultiAgentLogger.forSession(sessionId);
    logger.writeToDisk();
  } catch { /* best effort */ }
  MultiAgentLogger.cleanup(sessionId);      // v5.0: remove singleton
});
```

**File:** `core/llm/streamChat.ts` — Session-level field population

Always sets `userPrompt` and `model` on the logger when a user message is present and a session ID is available — no longer gated on `isAgentMode`:

```typescript
if (userPromptText) {
  try {
    const sessionId = await messenger.request("getCurrentSessionId", undefined);
    if (sessionId) {
      const logger = MultiAgentLogger.forSession(sessionId);
      logger.setUserPrompt(userPromptText);
      logger.setModel(`${model.providerName}/${model.model}`);
    }
  } catch { /* session ID not available */ }
}
```

### 19.9 Feedback Tracker & Release Plan

**File:** `docs/generate_tracker.js` (203 lines) — NEW  
**File:** `docs/RICA_Multi_Agent_Feedback_Tracker.xlsx` — NEW  
**File:** `docs/RICA_Multi_Agent_Release_Plan.md` (217 lines) — NEW

#### Feedback Tracker

Excel spreadsheet generated from `generate_tracker.js` with ExcelJS. Designed for Phase 1 testers to log bugs and track fixes.

**19 columns:** ID, Type, Title, Severity, Status, Reported By, Date Reported, Assigned To, Deadline, Reproduction, Session ID, Logs / Session File, SharePoint Log Link, Root Cause, Fix Description, Commit / MR, Retested By, Retest Date, Notes

**Features:**
- Frozen header row with blue background
- Data validation dropdowns for Type (Bug/Enhancement/UX), Severity (Blocker/Major/Minor), Status (New/Assigned/In Progress/Fixed/Retest/Closed/Deferred), Assigned To
- Conditional formatting: Blocker=red, Major=orange, Minor=blue
- Auto-filter on all columns
- Rows pre-populated with bugs from Phase 1 internal testing

**Current items:**

| ID | Type | Title | Severity | Status |
|----|------|-------|----------|--------|
| BUG-001 | Bug | Sub-agent not reporting back to master agent | Blocker | New |
| BUG-002 | Bug | Sub-agent excessive token usage (~750K tokens) | Blocker | New |
| BUG-003 | Bug | Sub-agent task overlap in parallel mode | Major | New |
| ENH-001 | Enhancement | Japanese language support in system prompts | Blocker | New |
| ENH-002 | Enhancement | Multi-level structured logging | Major | **Fixed** |
| ENH-003 | Enhancement | Token limiter to cap excessive usage | Major | New |
| ENH-004 | Enhancement | Vietnamese language support | Minor | New |

**Debug workflow for testers:**
1. Open token panel → click "save log" → log saved to `session_logs/`
2. Upload `.json` to shared SharePoint folder
3. Paste SharePoint link in "SharePoint Log Link" column
4. Include Session ID (from token panel) in tracker

#### Release Plan

5-phase rollout plan for v2.0.0 release (May 20, 2026):
- Phase 1: Internal Testing (Apr 21–25) — current phase
- Phase 2: Bug Fixes & Beta Build (Apr 27–28)
- Holiday Break (Apr 29 – May 6)
- Phase 3: Pilot Release (May 7–9)
- Phase 4: Stabilization (May 12–16)
- Phase 5: Production Release (May 19–20)

### 19.10 New Files (v5.0)

| File | Lines | Purpose |
|------|-------|---------|
| `core/tools/multiagent/MultiAgentLogger.ts` | 555 | Structured session tracing — singleton logger, JSON persistence |
| `docs/generate_tracker.js` | 203 | Excel feedback tracker generator |
| `docs/RICA_Multi_Agent_Feedback_Tracker.xlsx` | — | Generated Excel tracker for Phase 1 |
| `docs/RICA_Multi_Agent_Release_Plan.md` | 217 | 5-phase v2.0.0 release plan |
| **Total new** | **~975** | |

### 19.11 Modified Files (v5.0)

| File | Changes | Commit |
|------|---------|--------|
| `core/tools/multiagent/SubAgentRunner.ts` | +63 lines: logger integration at agent start, LLM calls, tool calls, snapshots, completion, error, cancel; writeToDisk in finally block | `575c2cab8` |
| `core/tools/implementations/spawnAgents.ts` | +25 lines: batch spawn start/complete logging, session-level field population (model, userPrompt, workflowPattern), scope overlap logging | `575c2cab8`, `083e03d17` |
| `core/core.ts` | +103 lines: orchestrator tool call logging, `multiagent/getSessionLog` IPC handler, logger finalize/cleanup in killSession | `cd1d8beda`, `083e03d17` |
| `core/llm/streamChat.ts` | +40 lines: user prompt + model logging to MultiAgentLogger, removed isAgentMode gate | `cd1d8beda`, `083e03d17` |
| `core/protocol/core.ts` | +6 lines: `multiagent/getSessionLog` protocol message type | `083e03d17` |
| `core/protocol/passThrough.ts` | +2 lines: `multiagent/getSessionLog` in WEBVIEW_TO_CORE_PASS_THROUGH | `083e03d17` |
| `gui/src/pages/gui/Chat.tsx` | +61 lines: session ID display, click-to-copy, save log button, IPC call | `083e03d17` |
| `gui/src/redux/store.ts` | +21 lines: `sanitizeStaleToolCalls` redux-persist transform | `083e03d17` |
| `gui/src/redux/slices/sessionSlice.ts` | +11 lines: mark running agents as canceled on session restore | `083e03d17` |
| `gui/src/redux/thunks/callToolById.ts` | +15 lines: GUI-side IPC timing logs | `cd1d8beda` |

### 19.12 Bugs Encountered & Resolutions (v5.0)

#### 19.12.1 passThrough.ts Silent Message Drop

**Symptom:** Save log button stuck on "saving..." indefinitely. No error in console.

**Root cause:** `multiagent/getSessionLog` was not registered in `WEBVIEW_TO_CORE_PASS_THROUGH` in `core/protocol/passThrough.ts`. The VS Code extension silently drops any webview→core message not in this whitelist.

**Fix:** Added `"multiagent/getSessionLog"` to the pass-through array.

**Lesson:** `passThrough.ts` is the IPC whitelist. Any new webview→core message MUST be added there or it will be silently dropped with no error feedback. This is a recurring trap — should be documented prominently.

#### 19.12.2 IPC Deadlock on getWorkspaceDirs

**Symptom:** Save log handler in `core.ts` hung when calling `this.ide.getWorkspaceDirs()` inside the `on()` handler.

**Root cause:** IPC deadlock — calling core→IDE (to get workspace dirs) inside a webview→core handler. The IDE messenger is busy servicing the webview request and cannot process the nested core→IDE request.

**Fix:** Pass `workspacePath` from the GUI side via `window.workspacePaths[0]` in the request payload, eliminating the need for the core→IDE call.

**Lesson:** Never call `this.ide.*` methods inside `on()` handlers. Pass required data from the GUI in the request payload.

#### 19.12.3 VS Code Webview Download Blocked

**Symptom:** `document.createElement('a').click()` with a blob URL did nothing in the webview.

**Root cause:** VS Code webview sandboxing blocks the browser download API. The webview has restricted access to the DOM and cannot trigger file downloads.

**Fix:** Changed from browser download to server-side file copy. The core process reads the log file, creates `session_logs/` in the workspace, and copies the file there.

#### 19.12.4 Duplicate Log Files Per Session

**Symptom:** Multiple JSON files with different timestamps but the same session ID in `~/.rica-multiagent-logs/`.

**Root cause:** `writeToDisk()` generated a new timestamped filename with `new Date().toISOString()` on every call. Since `writeToDisk()` is called after every agent completion, a session with 3 agents produced 3 separate files.

**Fix:** Added `finalLogPath` instance property. `writeToDisk()` generates the filename once (based on `startTime`, not current time) and overwrites the same file on subsequent calls.

#### 19.12.5 Save Button Picking Oldest Log

**Symptom:** Save button returned an old mid-session log instead of the latest completed log.

**Root cause:** The `getSessionLog` handler used `.find()` which returns the first (oldest) match in the directory listing.

**Fix:** Sort files reverse chronologically with `.sort().reverse()` before `.find()` so the latest file is picked first.

#### 19.12.6 Missing Session-Level Fields

**Symptom:** `userPrompt`, `model`, `workflowPattern` were all missing in session logs.

**Root cause:** In `streamChat.ts`, logging was gated on `isAgentMode` which checked for `baseAgentSystemMessage` on the model — a property that was never set in the multi-agent flow.

**Fix:** Removed the `isAgentMode` gate. Now always logs when there's a user prompt and a valid session ID. Also added field population in `spawnAgents.ts` at batch spawn time.

#### 19.12.7 Stale Running Agents on F5 Restart

**Symptom:** After F5 restart, the last session loaded with tool calls showing "working..." spinners and sub-agents showing "running" status — permanently stuck.

**Root cause:** Redux-persist rehydrates the last session's state including `toolCallStates` with `status: "calling"` and `subAgentProgress` with `status: "running"`. No sanitization was applied during rehydration.

**Fix:** Two-layer fix: (1) `sanitizeStaleToolCalls` redux-persist transform converts `"calling"`/`"generated"` to `"canceled"` during outbound rehydration. (2) `newSession` reducer marks any `"running"` sub-agents as `"canceled"` with a `completedAt` timestamp when restoring a session.

#### 19.12.8 Windows File URI Decoding

**Symptom:** `session_logs/` folder created in wrong location or not at all.

**Root cause:** `window.workspacePaths[0]` returns a file URI like `file:///c%3A/Users/a5163050/Desktop/...`. Using it directly as a filesystem path fails. After URL decoding, the result has a leading `/` which is invalid on Windows.

**Fix:** `decodeURIComponent(new URL(uri).pathname)` + strip leading `/` on `process.platform === "win32"`.

### 19.13 Updated Limitations & Future Work

#### Resolved in v5.0

| Limitation | Resolution |
|------------|-----------|
| No structured logging | MultiAgentLogger captures every step — LLM calls, tool calls, token breakdown, API errors, orchestrator events |
| No log export for testers | Save button in token panel exports to workspace `session_logs/` |
| No session ID visibility | Session ID displayed in token panel with click-to-copy |
| Stale agents on restart | Redux-persist transform + session restore sanitization |
| Debug logging unstructured | Structured JSON logs with typed interfaces replace ad-hoc console.log for multi-agent sessions |
| No feedback tracking | Excel tracker with 19 columns, data validation, conditional formatting |

#### Remaining Limitations

1. **ContextBridge budget not enforced** — `DEFAULT_BUDGET_FRACTION` exists but `getContextForAgent()` called without budget. Unbounded context growth in sequential patterns.
2. **No conversation compaction** — sub-agent conversations grow up to 25 iterations without summarization. (Fix implemented in stash but not yet applied — see ENH-003.)
3. **No token budget per agent** — `DEFAULT_TOKEN_BUDGET` exists in stash but not yet applied. (See BUG-002, ENH-003.)
4. **No model selection per role** — all sub-agents use the same LLM model as the orchestrator.
5. **Live log file not always cleaned up** — `flushLive()` writes a `live_*.json` that should be deleted by `writeToDisk()`, but if the session ends without `writeToDisk()` being called (e.g., extension crash), the live file persists.
6. **Debug logging remains** — `[FileTracker]`, `[SpawnAgents]`, `[TOKEN-SOURCE]`, `[RICA-PROXY]`, `[MultiAgent]` console.log statements in production code.

#### Stashed Fixes (Ready to Apply)

The following fixes are implemented and stashed (`git stash@{0}: BUG-001 + BUG-002 + BUG-003 + ENH-001 fixes`) but not yet applied to the branch:

| Fix | Tracker Item | Description |
|-----|-------------|-------------|
| Token budget (100K default) | BUG-002, ENH-003 | `DEFAULT_TOKEN_BUDGET = 100_000`; stops agent when budget exhausted |
| Message pruning | BUG-002 | `MESSAGE_PRUNE_THRESHOLD = 20`; summarizes old messages to control context growth |
| MAX_TOOL_ITERATIONS 25→15 | BUG-002 | Hard cap reduction |
| Tighter system prompts | BUG-002 | "stop when done", "max 2 retries", "never re-read", "limited token budget" |
| Pre-spawn scope validation | BUG-003 | Regex-based file path extraction; rejects batch if multiple write-agents target same file |
| Stricter parallel mode prompts | BUG-003 | Must name specific files per agent; "system will REJECT if overlap detected" |
| Language matching | ENH-001 | "Always respond in the same language the user used in their prompt" added to all role system messages |

---

## 20. v5.1 — Bug Fixes, Checkpoint Evaluation, Budget UI, GUI Logging & Orchestrator Prompt Overhaul

This section documents all features added after v5.0 (2026-04-23 → 2026-04-24). The core goals: (1) apply the stashed BUG-001/002/003 + ENH-001 fixes, (2) build phased checkpoint evaluation, (3) add stale LLM stream protection, (4) enrich session logs, (5) add per-agent and session-level budget UI, (6) move orchestrator logging from core to GUI thunks, (7) overhaul orchestrator system prompt, (8) restore real API token tracking, (9) add IPC timeout handling.

**Commits:** All uncommitted — built and stress-tested, pending commit and push to GitLab.

**Tracker References:** BUG-001, BUG-002, BUG-003, ENH-001, B6 (all resolved)

### 20.1 Overview

v5.0 left several critical bug fixes stashed (token budget, scope overlap, language matching) and had gaps in logging and UI feedback. v5.1 applies all stashed fixes, adds the phased checkpoint evaluation loop, introduces budget visualization at both per-agent and session level, migrates orchestrator logging to GUI-driven protocol messages, overhauls the orchestrator system prompt, restores real API token tracking, and adds IPC timeout protection.

**What changed (25 files, +2068 / -238 lines):**

| Category | Changes |
|----------|---------|
| Bug fixes applied | BUG-001 (forceSummary), BUG-002 (token budget 100K), BUG-003 (scope overlap), ENH-001 (language match), B6 (tool truncation 50K cap) |
| New system | Phased checkpoint evaluation with LLM-based agent coordination |
| Stability | Stale LLM stream watchdog timers (90s/60s) at 3 layers |
| Log enrichment | `truncated`/`originalLength` on tool calls, `orchestratorTokens`/`toolTruncations`/`overlappingFiles` in summary |
| Budget UI | Per-agent budget % indicator (color-coded), session token budget bar (400K), full-screen + collapsed views |
| GUI logging | 3 new protocol messages (`logMainModelTurn`, `logToolResult`, `logError`) — logging moved from core.ts to GUI thunks |
| LLM streaming | Tool call accumulation in PromptLog, real API usage restored with cache/reasoning tokens |
| Orchestrator prompt | Recon-first instruction, retry/follow-up batch rules, 2-batch max, checkpoint coordination |
| IPC resilience | IdeMessenger 60s timeout with reject, callToolById 300s timeout, tool result display truncation (10K) |
| UI fix | Markdown table rendering (backtick content in remarkTables) |
| Logging fix | Single-agent spawn (`builtin_spawn_agent`) now logs metadata + decision step |

### 20.2 Stashed Fixes Applied

The following fixes were stashed in v5.0 and are now applied and verified:

#### 20.2.1 BUG-001 — Sub-Agent Not Reporting Back

**Problem:** Agents hitting budget exhaustion or producing short results returned empty/minimal content to the orchestrator, making their work invisible in the final synthesis.

**Fix — forceSummary mechanism (`SubAgentRunner.ts`):**
- When an agent is stopped (budget exhaustion or external stop) and its accumulated text is < 500 chars, the system makes one final LLM call asking: *"Summarize everything you've found so far"*
- The LLM receives the full conversation context and produces a comprehensive summary
- Result is prefixed with `(Token budget reached)` for visibility

**Verification:** Stress-tested across 8 sessions. Budget-exhausted agents consistently produced 9K-31K chars of useful content via forceSummary. All agents reported back in every test.

#### 20.2.2 BUG-002 — Token Blowout

**Problem:** Agents consumed unbounded tokens — a single agent could burn 300K+ tokens on a simple task, with no budget enforcement.

**Fix — Multi-layer token budget (`SubAgentRunner.ts`, `spawnAgents.ts`):**

| Layer | Budget | Enforcement |
|-------|--------|-------------|
| Per-agent | 100,000 tokens (`DEFAULT_TOKEN_BUDGET`) | Agent stopped when `tokenUsage.total >= budget` |
| Per-session | 400,000 tokens (`SESSION_TOKEN_BUDGET`) | New batch spawns blocked, running agents canceled |
| Iteration cap | 15 max (`MAX_TOOL_ITERATIONS`) | Hard stop, reduced from 25 |
| Step cap | 30 max (`MAX_TOOL_STEPS`) | Hard stop on tool call count |
| Message pruning | Threshold 20 messages | Old messages summarized to control context growth |

**Verification:** Across 8 stress test sessions, maximum per-agent usage was 142K (budget enforcement triggered), maximum session total was 383K. No runaway token consumption observed.

#### 20.2.3 BUG-003 — Scope Overlap Between Agents

**Problem:** Multiple write-capable agents could be assigned the same files, causing conflicting edits.

**Fix — Pre-spawn scope validation (`spawnAgents.ts`):**
- Regex-based file path extraction from agent task descriptions
- Read-only roles (`Researcher`, `Reviewer`, `Analyst`, `Planner`) excluded from overlap checks
- Shared config files (package.json, tsconfig.json, etc.) excluded
- When write-overlap detected: logged via `MultiAgentLogger.scopeOverlapDetected()`, warning injected into orchestrator context

**Verification:** Tested with prompts designed to force overlap (3 tasks targeting test.js). Orchestrator correctly merged into 1 agent. When parallel agents had distinct scopes, `scopeOverlapsDetected: 0` confirmed no false positives.

#### 20.2.4 ENH-001 — Language Matching

**Problem:** Final response always in English regardless of user's language.

**Fix (`multiAgentSystemMessage.ts`):**
- Orchestrator system prompt includes: *"Your final response to the user MUST be in the same language the user writes in. But agent tasks MUST always be written in English — agents work internally in English for best reasoning quality and token efficiency."*
- Agents work in English for consistency; translation happens at orchestrator synthesis

#### 20.2.5 B6 — Tool Result Truncation

**Problem:** Large tool results (grep returning 30-40MB) consumed entire agent token budget in a single step.

**Fix (`SubAgentRunner.ts`):**
- `MAX_TOOL_RESULT_CHARS = 50_000` — hard cap on tool result size
- Truncation strategy: 80% head (40K chars) + 10% tail (5K chars) with truncation marker in between
- Truncation metadata tracked: `wasTruncated` flag, `originalResultLength` preserved
- Logged to session log via `agentToolCall()` with `truncated: true` and `originalLength`

**Verification:** 5 sessions showed truncation working — grep results of 37-40M chars truncated to ~45K. All 9 truncations in session cc0c9b0c properly logged with `originalLength`.

### 20.3 Phased Checkpoint Evaluation System

**Files:** `spawnAgents.ts` (607 new lines), `SubAgentRunner.ts` (checkpoint callback)

**Problem:** In v4.0, agents ran independently with no mid-execution coordination. If an agent wasted budget re-reading files or went off-track, there was no mechanism to course-correct.

**Solution — Phased parallel execution with LLM-based checkpoint evaluation:**

When 2+ agents are spawned, they enter phased parallel mode:

1. **Agents run concurrently** via `Promise.all()` with checkpoint callbacks
2. **Every `CHECKPOINT_INTERVAL` (10) tool steps**, each agent yields a `CheckpointResult`:
   ```typescript
   interface CheckpointResult {
     stepsCompleted: number;
     tokensUsed: number;
     interimSummary: string;  // LLM-generated progress summary
   }
   ```
3. **Fast-path optimization**: If all pending agents are under 30% budget, auto-continue without LLM eval
4. **LLM evaluation**: For agents past 30% budget, the orchestrator calls the LLM with all checkpoint reports. The LLM returns per-agent decisions:
   ```typescript
   interface CheckpointDecision {
     action: "continue" | "redirect" | "stop";
     newContext?: string;  // injected into agent's next iteration
   }
   ```
5. **Redirect**: Agent receives new instructions mid-run (e.g., *"Stop re-reading files, start writing your report"*)
6. **Stop**: Agent is terminated, forceSummary produces final output

**Constants:**
- `CHECKPOINT_INTERVAL = 10` steps between checkpoints
- `MAX_PHASES = 3` maximum checkpoint evaluation rounds
- `SESSION_TOKEN_BUDGET = 400_000` hard session cap
- `DEFAULT_AGENT_BUDGET = 100_000` per-agent budget reference

**Observed behavior in stress testing:**
- Fast-path checkpoint (session a35ad9a4): *"Auto-continue 1 agents: Grep-based pattern scan (10 steps, 11% budget)"*
- LLM redirect (session 8ea7c174): Eval told agent to *"Stop re-reading files, proceed directly to writing report"* — actively corrected wasted effort
- Most agents complete in 5-8 tool steps, before hitting the 10-step checkpoint threshold

### 20.4 Stale LLM Stream Protection

**Files:** `streamChat.ts`, `SubAgentRunner.ts`, `spawnAgents.ts`

**Problem:** Session 222f74fe froze permanently. The LLM API stopped sending chunks but never closed the connection. `await gen.next()` blocked indefinitely with no timeout.

**Fix — Stale stream watchdog timers at 3 layers:**

| Layer | Timeout | File | Purpose |
|-------|---------|------|---------|
| Orchestrator chat | 90s | `streamChat.ts` | Main user-facing LLM stream |
| Sub-agent LLM calls | 90s | `SubAgentRunner.ts` | Per-agent `callLLM()` |
| Checkpoint eval | 60s | `spawnAgents.ts` | Checkpoint evaluation LLM call |

**Implementation pattern (identical at all 3 layers):**
```typescript
const STALE_STREAM_TIMEOUT_MS = 90_000;
let staleTimer: ReturnType<typeof setTimeout> | null = null;
const resetStaleTimer = () => {
  if (staleTimer) clearTimeout(staleTimer);
  staleTimer = setTimeout(() => {
    console.warn(`[Layer] No data for ${STALE_STREAM_TIMEOUT_MS / 1000}s — aborting`);
    abortController.abort();
  }, STALE_STREAM_TIMEOUT_MS);
};
resetStaleTimer();
// Reset on every chunk received
while (!result.done) {
  resetStaleTimer();
  // ... process chunk ...
}
if (staleTimer) clearTimeout(staleTimer);
```

**Status:** Built and deployed. Not yet triggered in testing (API has been responsive since the fix). The protection is passive — it only activates if the API stalls for 90s/60s without sending any data.

### 20.5 Session Log Enrichment

**File:** `MultiAgentLogger.ts` (+233 lines), `SubAgentRunner.ts`

Three fields added to the session log based on a 20-question debuggability audit:

#### 20.5.1 Tool Call Truncation Markers

**Location:** `agents[].toolCalls[]`

When B6 truncation fires (result > 50K chars), the tool call entry now includes:
- `truncated: true` — boolean flag
- `originalLength: number` — original result size before truncation (e.g., `40490486` for a 40MB grep result)

**Implementation:** `SubAgentRunner.ts` tracks `wasTruncated` and `originalResultLength`, passes to `sessionLogger.agentToolCall()` via optional parameters.

#### 20.5.2 Scope Overlap Details in Summary

**Location:** `summary.overlappingFiles`

When scope overlap is detected (BUG-003), the summary now includes:
```typescript
overlappingFiles?: Array<{ file: string; agents: string[] }>
```

Only populated when `scopeOverlapsDetected > 0`. Tracks which specific files triggered the overlap and which agents were involved.

#### 20.5.3 Orchestrator Token Total & Truncation Count

**Location:** `summary.orchestratorTokens`, `summary.toolTruncations`

| Field | Description |
|-------|-------------|
| `orchestratorTokens` | Sum of `tokensUsed` from all orchestrator conversation steps (LLM calls + checkpoint evals) |
| `toolTruncations` | Count of tool calls where B6 truncation fired |

**Implementation:** Computed in `finalize()` by scanning `orchestratorConversation` and `agents[].toolCalls[]`.

### 20.6 Checkpoint Eval Token Capture

**File:** `spawnAgents.ts`

**Problem:** All checkpoint evaluation steps had `tokensUsed: undefined` in the session log. The LLM eval call returned a `PromptLog` with token data, but it was never captured.

**Fix:** After the checkpoint eval `streamChat` generator completes, capture the `PromptLog` return value:
```typescript
const promptLog = result.value as any;
const evalTokensUsed = (promptLog?.promptTokens ?? 0) + (promptLog?.completionTokens ?? 0);
```
Pass `evalTokensUsed` to `logger.orchestratorCheckpointEval()`.

**Fast-path checkpoint detail:** The fast-path (all agents under 30% budget) now logs per-agent detail instead of just *"Auto-continue N agents"*:
```
Auto-continue 1 agents: Grep-based pattern scan (10 steps, 11% budget)
```

### 20.7 Markdown Table Rendering Fix

**File:** `gui/src/components/StyledMarkdownPreview/utils/remarkTables.tsx`

**Problem:** Tables with backtick-wrapped content showed commas instead of filenames. When remark parses `` `main.py`, `cnn.py` ``, backtick content becomes `inlineCode` MDAST nodes. The buffer collection only visited `text` nodes, so the buffer became `", "` — just the commas between the inline code blocks.

**Fix:** Changed visitor from type-specific to generic:
```typescript
// Before — only collected text nodes:
visit(paragraphNode, "text", (textNode) => {
  buffer += textNode.value;
});

// After — collects both text and inlineCode:
visit(paragraphNode, (node: any) => {
  if (node.type === "text") buffer += node.value;
  else if (node.type === "inlineCode") buffer += node.value;
});
```

### 20.8 Single-Agent Spawn Logging Fix

**File:** `core/tools/implementations/spawnAgent.ts` (+12 lines)

**Problem:** When the orchestrator used `builtin_spawn_agent` (singular) instead of `builtin_spawn_agents` (plural), the session log had:
- `model: undefined`
- `workflow: undefined`
- `userPrompt: ""`
- No `decision` step

This happened because the metadata and decision logging was only wired into the batch `spawnAgents.ts` path.

**Fix:** Added `MultiAgentLogger` calls to the singular spawn path:
```typescript
if (extras.sessionId) {
  const logger = MultiAgentLogger.forSession(extras.sessionId);
  if (extras.llm) {
    logger.setModel(`${extras.llm.providerName}/${extras.llm.model}`);
  }
  logger.setUserPrompt(description);
  logger.setWorkflowPattern("single");
  logger.orchestratorDecision([{ role: role || "Agent", task, description }]);
}
```

**Verification:** Session d1e449ee confirmed all fields populated: `workflow: "single"`, `model` set, `decision` step present.

### 20.9 Per-Agent & Session Budget UI

**File:** `gui/src/pages/gui/MultiAgentPanel.tsx` (+135 lines), `core/index.d.ts` (+1 line)

**Problem:** Users had no visibility into how much budget each agent was consuming or how close the session was to the 400K token limit. Agents could silently exhaust budget with no UI indicator.

#### 20.9.1 Per-Agent Budget Percentage Indicator

Added to both the **AgentCard** (collapsed view) and **AgentDetailOverlay** (expanded view):

- Displays `Budget: X%` with color-coded thresholds:
  - **Emerald** (< 50%) — healthy
  - **Amber** (50-80%) — approaching limit
  - **Red** (> 80%) — near exhaustion
- Only shown when `tokenUsage.budget > 0` (budget field populated by SubAgentRunner)
- Tooltip shows absolute values: `"Budget: 45,200 / 100,000 tokens used"`

**AgentCard token display** changed from `X tok` to `X/Y tok` format when budget is available, showing the denominator.

**Type change:** Added `budget?: number` to `SubAgentProgress.tokenUsage` in `core/index.d.ts`.

#### 20.9.2 Session Token Budget Progress Bar

Tracks total token usage across all agents against the `SESSION_TOKEN_BUDGET = 400,000` limit. Displayed in three locations:

1. **Full-screen header** — 24px-wide progress bar with `"Session Budget"` label, absolute + percentage values
2. **Collapsed panel header** — 16px-wide compact bar with `"LIMIT"` text when exceeded
3. **OrchestratorDetailOverlay** — full-width bar with detailed messaging

**Color transitions:**
| Percentage | Color | Extra |
|-----------|-------|-------|
| < 50% | `bg-emerald-500` | — |
| 50-70% | `bg-yellow-500` | — |
| 70-85% | `bg-amber-500` | "approaching limit" label |
| 85-100% | `bg-red-500` | "budget nearly exhausted" label |
| > 100% | `bg-red-600` | `animate-pulse`, "EXCEEDED" badge, bold red text |

**Implementation:** `sessionTokens` useMemo recalculates on every `agents` change — sums `promptTokens + completionTokens` across all agents.

### 20.10 GUI-Driven Orchestrator Logging

**Files:** `core/core.ts` (+27/-26 lines), `core/protocol/core.ts` (+32 lines), `core/protocol/passThrough.ts` (+3 lines), `gui/src/redux/thunks/streamNormalInput.ts` (+39 lines), `gui/src/redux/thunks/streamResponseAfterToolCall.ts` (+15 lines), `gui/src/redux/thunks/streamThunkWrapper.tsx` (+14 lines)

**Problem:** v5.0 logged orchestrator tool calls from `core.ts` using a verbose try/catch wrapper around `callTool`. This approach:
- Only captured agent-related tool calls (`spawn_agent`/`analyze_task`), missing read/write/grep calls
- Required wrapping every `callTool` invocation
- Had no access to the LLM response content (only tool calls were logged)
- Could not log abort/cancel events (those happen in GUI state)

**Solution — Protocol-based logging from GUI thunks:**

#### 20.10.1 Three New Protocol Messages

Defined in `core/protocol/core.ts` and registered in `passThrough.ts`:

| Message | Payload | Source Thunk |
|---------|---------|-------------|
| `multiagent/logMainModelTurn` | `sessionId`, `prompt`, `completion`, `modelTitle`, `promptTokens`, `completionTokens`, `inputSummary`, `durationMs` | `streamNormalInput.ts` |
| `multiagent/logToolResult` | `sessionId`, `toolName`, `toolCallId`, `resultPreview` (5K limit), `durationMs` | `streamResponseAfterToolCall.ts` |
| `multiagent/logError` | `sessionId`, `errorType` (`"abort"` / `"cancel"` / `"llm_error"`), `message` (2K limit), `durationMs` | `streamNormalInput.ts`, `streamThunkWrapper.tsx` |

#### 20.10.2 core.ts Simplified Handler

Removed the verbose try/catch wrapper around `callTool` that logged `tool_call_received`/`tool_call_completed`/`tool_call_error`. Replaced with direct `callTool` return. Added 3 `on()` handlers that receive the new protocol messages and delegate to `MultiAgentLogger`.

#### 20.10.3 streamNormalInput.ts — LLM Turn Logging

After each orchestrator LLM response in multi-agent mode:
- Sends `multiagent/logMainModelTurn` with PromptLog data (prompt, completion, tokens, duration)
- Extracts user messages as `inputSummary` (last 5 messages, 3K char limit per message, 5K total)
- On abort: sends `multiagent/logError` with `errorType: "abort"` and duration

#### 20.10.4 streamResponseAfterToolCall.ts — Tool Result Logging

After all tool calls complete in multi-agent mode:
- Iterates `allToolCalls` and sends `multiagent/logToolResult` for each
- Includes `resultPreview` from `renderContextItems()`, capped at 5K chars

#### 20.10.5 streamThunkWrapper.tsx — Error Logging

On exception in any stream thunk in multi-agent mode:
- Sends `multiagent/logError` with `errorType: "cancel"` (intentional stop) or `"llm_error"` (unexpected)
- Message capped at 2K chars

### 20.11 Tool Call Accumulation in LLM Streaming

**File:** `core/llm/index.ts` (+56 lines)

**Problem:** When the orchestrator's LLM response was purely tool calls (no text content), `PromptLog.completion` was an empty string. This meant:
- Session logs showed empty `completion` for tool-call-only turns
- GUI thunk logging via `multiagent/logMainModelTurn` sent empty completion
- Impossible to see which tools the orchestrator invoked from the log alone

**Fix — Accumulate tool calls during streaming:**

```typescript
const toolCallAccum: Map<number, { id?: string; name: string; args: string }> = new Map();
```

Accumulated across all three streaming paths (non-streaming OpenAI adapter, streaming adapter, and generic `_streamChat`). After streaming completes:

```typescript
if (toolCallAccum.size > 0) {
  const tcSummary = Array.from(toolCallAccum.values()).map(tc => {
    const argPreview = tc.args.length > 500 ? tc.args.slice(0, 500) + "..." : tc.args;
    return `[tool_call: ${tc.name}(${argPreview})]`;
  }).join("\n");
  completionForLog = completion ? `${completion}\n\n${tcSummary}` : tcSummary;
}
```

The `PromptLog.completion` now contains text content (if any) plus tool call summaries.

### 20.12 Real API Token Usage Restored

**File:** `core/llm/index.ts` (within the +56 lines)

**Problem:** v5.0 downgraded `streamChat()` token tracking from real API usage to tiktoken estimation only. The PromptLog always returned `isActualUsage: false`, `reasoningTokens: 0`, and no cache token data — even when the API provided accurate usage.

**Context:** v5.0 made this change to fix a different bug. v5.1 restores the full token tracking:

```typescript
const hasApiUsage = !!this._lastApiUsage;
const promptTokens = this._lastApiUsage?.promptTokens ?? this.countTokens(prompt);
const completionTokens = this._lastApiUsage?.completionTokens ?? this.countTokens(completion);
const reasoningTokens = this._lastApiUsage?.reasoningTokens ?? 0;
const totalTokens = this._lastApiUsage?.totalTokens ?? (promptTokens + completionTokens);
const cacheReadTokens = this._lastApiUsage?.cacheReadTokens ?? 0;
const cacheCreationTokens = this._lastApiUsage?.cacheCreationTokens ?? 0;
```

Returns `isActualUsage: true` when API data is available, with full cache and reasoning token breakdowns.

### 20.13 IPC Timeout & Tool Result Display Truncation

**Files:** `gui/src/context/IdeMessenger.tsx` (+22 lines), `gui/src/redux/thunks/callToolById.ts` (+4 lines), `gui/src/redux/slices/sessionSlice.ts` (+8 lines), `core/llm/constructMessages.ts` (+12 lines)

#### 20.13.1 IdeMessenger Timeout

**Problem:** `IdeMessenger.request()` returned a Promise that could hang forever if the core never responded (e.g., process crash, IPC disconnect). No timeout, no error — just a silent hang.

**Fix:**
- Added optional `timeoutMs` parameter (default 60s) to `request()`
- Uses `setTimeout` + `settled` flag to prevent double-resolve/reject race
- On timeout: removes event listener, rejects with descriptive Error:
  `IdeMessenger.request("tools/call") timed out after 60000ms`

#### 20.13.2 callToolById 300s Timeout

Agent spawns can legitimately take 3-5 minutes (multiple agents each doing 10+ tool calls). The default 60s timeout is too aggressive.

- Changed `tools/call` IPC request to use `300_000` ms (5 minutes)
- Also fixed `output?.length` check to `output !== undefined` — empty string `""` is a valid tool output, but `"".length` is falsy

#### 20.13.3 Tool Result Display Truncation

**Problem:** Large tool results (e.g., 40K char grep output) stored in Redux `history[].message.content` caused UI lag and made the chat panel hard to scroll.

**Fix (sessionSlice.ts):**
- `MAX_TOOL_DISPLAY = 10_000` chars
- When `role === "tool"` and rendered content exceeds the limit, truncates with marker:
  `[... truncated for display: 42,156 total chars ...]`

#### 20.13.4 Tool Result Reconstruction for LLM

**Problem:** After display truncation, the truncated content would be sent back to the LLM in the next conversation turn — the LLM would see `[... truncated for display ...]` instead of the real tool result.

**Fix (constructMessages.ts):**
- Before sending messages to LLM, checks if tool message content contains `"truncated for display"`
- If so, reconstructs the full content from `historyItem.contextItems` (which store the original untruncated result)

### 20.14 Orchestrator System Prompt Overhaul

**File:** `core/llm/multiAgentSystemMessage.ts` (+92 lines)

The orchestrator system prompt was rewritten to address observed agent behavior issues during stress testing.

#### 20.14.1 Recon-First Instruction

**Problem:** Agents spawned blind — no codebase knowledge — wasting 10+ tool steps just reading directory structure.

**Fix:** Added to decision framework:
```
RECON FIRST: Before spawning ANY agents, use ReadFile/ListDir yourself to understand
the codebase structure. At minimum: list the root directory, read package.json or
equivalent config, identify key directories. This takes 1-2 tool calls but saves
each agent 10+ wasted exploration steps.
```

Agents now receive specific file paths instead of vague "explore" instructions.

#### 20.14.2 Retry & Follow-Up Batch Rules

**Problem:** Orchestrator was spawning 3-4 retry batches when agents failed, burning through the session budget.

**Fix:** Added explicit rules:
- Before spawning retry agents, CHECK what previous batch actually produced
- If work was partially completed by other agents, acknowledge it — don't respawn
- Only retry what is ACTUALLY missing
- 2-batch maximum (implement + review) unless critical work is missing
- Session token budget awareness: if most of 400K is used, synthesize instead of spawning

#### 20.14.3 Task Description Standards

Rewritten with emphasis on specificity:
- **Every task MUST include** at least one specific file path, directory path, or function/class name
- Under-10-tool-call target per agent (changed from "under 30 seconds")
- When in doubt, prefer parallel decomposition — a single overloaded agent that hits budget returns nothing

#### 20.14.4 Single-Agent vs Multiple Clarification

Rewritten to be less ambiguous:
- ONE agent only when genuinely one focused job ("fix the typo in Login.tsx")
- If user lists multiple subtasks, use PARALLEL regardless of seeming relatedness
- Example: "Check API endpoints, auth flow, database config, and test coverage" → 4 agents, NOT 1

#### 20.14.5 Checkpoint Coordination Instructions

New section for parallel mode:
- Agents checkpointed every 5 tool steps (note: 5 in prompt, 10 in code — prompt rounds down for agent planning)
- Redundant/off-track agents stopped or redirected
- Agents receive peer updates between checkpoints
- Write SPECIFIC tasks with clear completion criteria to maximize checkpoint effectiveness

#### 20.14.6 Language Matching (ENH-001)

Added to base rules:
- Final response in user's language
- Agent tasks always in English for best reasoning quality and token efficiency

### 20.15 Chat.tsx Thread Message Class

**File:** `gui/src/pages/gui/Chat.tsx` (+2 lines)

Added `thread-message` CSS class to the tool call div wrapper. This enables CSS targeting of tool call groups in the chat panel for styling and layout purposes.

### 20.16 New Files (v5.1)

No new files were added in v5.1. All changes are modifications to existing files introduced in v1.0–v5.0.

### 20.17 Modified Files (v5.1)

| File | Changes |
|------|---------|
| `core/tools/implementations/spawnAgents.ts` | +607 lines: phased checkpoint evaluation, session token budget, checkpoint eval token capture, fast-path detail logging, `DEFAULT_AGENT_BUDGET` constant |
| `core/tools/multiagent/SubAgentRunner.ts` | +417 lines: token budget enforcement, forceSummary, B6 truncation with tracking, stale stream watchdog, checkpoint callback, message pruning |
| `core/tools/multiagent/MultiAgentLogger.ts` | +233 lines: `truncated`/`originalLength` on ToolCallLog, `overlappingFiles`/`orchestratorTokens`/`toolTruncations` in SessionSummary, `orchestratorMainTurn`/`orchestratorToolResult`/`orchestratorError` methods |
| `core/llm/multiAgentSystemMessage.ts` | +92 lines: recon-first instruction, retry/follow-up batch rules (2-batch max), task description standards, single-agent clarification, checkpoint coordination, language matching (ENH-001) |
| `core/llm/streamChat.ts` | +14 lines: stale stream watchdog (90s) on orchestrator LLM calls |
| `core/llm/index.ts` | +56 lines: tool call accumulation across 3 streaming paths, `completionForLog` with `[tool_call: name(args)]` summaries, real API usage restored (`_lastApiUsage` with cache/reasoning tokens) |
| `core/llm/constructMessages.ts` | +12 lines: tool result reconstruction — rebuilds truncated tool messages from `contextItems` before sending to LLM |
| `core/tools/implementations/spawnAgent.ts` | +12 lines: metadata + decision logging for single-agent spawns (`setModel`, `setUserPrompt`, `setWorkflowPattern("single")`, `orchestratorDecision`) |
| `core/tools/multiagent/ContextBridge.ts` | +44 lines: context bridge priority-based push for orchestrator-provided context |
| `core/tools/multiagent/FaultHandler.ts` | +2 lines: fault handling additions |
| `core/tools/multiagent/runAgentWithRetry.ts` | +9 lines: retry logic updates |
| `core/protocol/core.ts` | +32 lines: 3 new protocol message types: `multiagent/logMainModelTurn`, `multiagent/logToolResult`, `multiagent/logError` with full typed payloads |
| `core/protocol/passThrough.ts` | +3 lines: route 3 new multiagent messages from GUI webview to core |
| `core/core.ts` | +27/-26 lines: removed verbose try/catch tool call wrapper, added 3 `on()` handlers for GUI-driven logging protocol messages |
| `core/index.d.ts` | +1 line: added `budget?: number` to `SubAgentProgress.tokenUsage` type |
| `gui/src/components/StyledMarkdownPreview/utils/remarkTables.tsx` | +78/-78 lines: markdown table fix — buffer collects both `text` and `inlineCode` MDAST nodes |
| `gui/src/pages/gui/MultiAgentPanel.tsx` | +135 lines: per-agent budget % indicator (emerald/amber/red), session token budget bar (400K, 4 color tiers + pulsing red), `X/Y tok` format, full-screen + collapsed views |
| `gui/src/pages/gui/Chat.tsx` | +2 lines: added `thread-message` CSS class to tool call div wrapper |
| `gui/src/context/IdeMessenger.tsx` | +22 lines: configurable `timeoutMs` (default 60s), reject on timeout, `settled` flag prevents double-resolve |
| `gui/src/redux/slices/sessionSlice.ts` | +8 lines: tool result display truncation: `MAX_TOOL_DISPLAY = 10_000` chars with `[... truncated for display ...]` marker |
| `gui/src/redux/thunks/streamNormalInput.ts` | +39 lines: logs orchestrator LLM turns via `multiagent/logMainModelTurn`, logs abort events, extracts user messages as `inputSummary` |
| `gui/src/redux/thunks/streamResponseAfterToolCall.ts` | +15 lines: logs tool results via `multiagent/logToolResult` with 5K char preview per tool call |
| `gui/src/redux/thunks/callToolById.ts` | +4 lines: 300s IPC timeout for `tools/call`, fixed `output?.length` → `output !== undefined` |
| `gui/src/redux/thunks/streamThunkWrapper.tsx` | +14 lines: logs errors/cancellations via `multiagent/logError`, distinguishes `"cancel"` vs `"llm_error"` type |

### 20.18 Stress Test Results

8 sessions tested across auto, parallel, sequential, and single-agent modes:

| Session | Mode | Agents | Prompt | Bugs Tested | Result |
|---------|------|--------|--------|-------------|--------|
| f0dbd6fa | Auto (parallel) | 2 | Dependency map + cross-reference | BUG-001, BUG-002 | PASS — both reported, 50K tokens |
| 7dd9cb5a | Auto (parallel) | 4 | Kitchen sink audit (security+perf+quality+grep) | All | PASS — 4/4 completed, 1 budget exhaustion, 5 truncations |
| ebcfcb1a | Auto (single) | 1 | Validation+errors+JSDoc on test.js | BUG-003 | PASS — orchestrator merged 3 tasks into 1 agent |
| 8ea7c174 | Parallel | 4 | Forced 4 agents: dependency, security, perf, imports | BUG-001, BUG-002, B6 | PASS — checkpoint redirect corrected agent mid-run |
| d1e449ee | Auto (single) | 1 | Multi-turn: validation then intentional breakage | Logging fix | PASS — single-agent metadata populated |
| 5d83fdcc | Parallel | 4 | 4 files: cnn, app, mario, movie_rec | BUG-001, BUG-002, B6 | PASS — 2 budget exhaustions, 4 truncations, all reported |
| 0290f04a | Sequential | 3 | 3-phase: inventory → dependencies → security vulns | BUG-001, BUG-002, context passing | PASS — true sequential with context chaining |
| cc0c9b0c | Parallel | 3 | Security audit (pre-fix baseline) | All log fields | PASS — all new log fields verified present |

### 20.19 Bugs Encountered & Resolutions (v5.1)

#### 20.19.1 Checkpoint Eval Tokens Undefined

**Symptom:** All checkpoint evaluation steps in session logs showed `tokensUsed: undefined`.

**Root cause:** `orchestratorCheckpointEval()` was called with only 3 arguments (prompt, response, duration). The optional 4th parameter `tokensUsed` was never passed. The `PromptLog` return value from `streamChat` generator was ignored.

**Fix:** Capture `result.value` after generator completes, compute `evalTokensUsed = promptTokens + completionTokens`, pass as 4th argument.

#### 20.19.2 Fast-Path Checkpoint No Agent Detail

**Symptom:** Early checkpoint evaluations logged *"Auto-continue 1 agents"* with no per-agent information — useless for debugging.

**Root cause:** Fast-path only logged the count of agents being continued.

**Fix:** Build detail string with per-agent name, steps completed, and budget percentage: `"Grep-based pattern scan (10 steps, 11% budget)"`.

#### 20.19.3 Magic Number 100K for Budget Threshold

**Symptom:** Fast-path budget check used `(cp.tokensUsed / 100_000) < 0.3` and checkpoint report used `100_000` in 3 places — hardcoded with no named constant.

**Fix:** Added `const DEFAULT_AGENT_BUDGET = 100_000` at file top, replaced all 3 occurrences.

#### 20.19.4 Markdown Tables Showing Commas

**Symptom:** Tables in the chat UI showed commas where filenames should be. For example, a table with `` `main.py`, `cnn.py` `` rendered as just `, `.

**Root cause:** `remarkTables.tsx` buffer collection used `visit(paragraphNode, "text", ...)` which only visits `text` MDAST nodes. Backtick-wrapped content becomes `inlineCode` nodes that were skipped. Buffer became `", "` — just the separator commas.

**Fix:** Changed to generic visitor that collects both `text` and `inlineCode` node values.

#### 20.19.5 Stale LLM Stream Hang (Session 222f74fe)

**Symptom:** Orchestrator froze permanently during chat. No error, no timeout, no recovery. Required manual kill.

**Root cause:** `streamChat.ts` line 133 `await gen.next()` blocks indefinitely if the LLM API stops sending chunks but doesn't close the connection. No timeout existed at any layer of the LLM streaming stack.

**Fix:** Added `setTimeout`-based stale watchdog at 3 layers (orchestrator 90s, sub-agent 90s, checkpoint eval 60s). Timer resets on every chunk. If no data arrives within the timeout, `abortController.abort()` fires and the call terminates cleanly.

#### 20.19.6 Single-Agent Spawn Missing Log Metadata

**Symptom:** Session ebcfcb1a (single-agent spawn via `builtin_spawn_agent`) had `model: undefined`, `workflow: undefined`, `userPrompt: ""`, and no `decision` step in the log.

**Root cause:** Metadata and decision logging were only wired into `spawnAgents.ts` (plural/batch path). The singular `spawnAgent.ts` had no `MultiAgentLogger` calls.

**Fix:** Added `setModel()`, `setUserPrompt()`, `setWorkflowPattern("single")`, and `orchestratorDecision()` calls to `spawnAgent.ts`.

### 20.20 Updated Limitations & Future Work

#### Resolved in v5.1

| Limitation | Resolution |
|------------------------|-----------|
| No token budget per agent | `DEFAULT_TOKEN_BUDGET = 100_000` enforced in SubAgentRunner |
| No conversation compaction | Message pruning at threshold 20 |
| Stashed BUG-001/002/003/ENH-001 fixes | All applied, verified across 8 stress test sessions |

#### Remaining Limitations

1. **No post-completion coverage validation** — agents returning partial results (budget exhaustion) are not validated for completeness. Orchestrator synthesizes whatever it gets without checking task coverage. (Future consideration — add if users report incomplete results.)
2. **Message snapshots are previews not full content** — `contentPreview` + `contentLength` stored, not actual message content. Cannot replay exact agent conversation from log alone.
3. **No structured abort/error reasons in session log** — stale timeout and API error details go to console, not the JSON session log. Count is tracked but not the message/stack.
4. **eventLog array unpopulated** — flat chronological timeline across agents is empty. Data exists in per-agent `llmCalls`/`toolCalls` but no unified cross-agent view.
5. **No forceSummary flag on agent records** — must infer from `(Token budget reached)` prefix in result text.
6. **Checkpoint interval (10 steps) rarely triggered** — most agents complete in 5-8 tool steps. The evaluation system works but is infrequently exercised.
7. **No model selection per role** — all sub-agents use the same LLM model as the orchestrator.
8. **Live log file not always cleaned up** — persists if session ends without `writeToDisk()` being called.
9. **ContextBridge budget not enforced** — unbounded context growth in sequential patterns.

---

## 21. v5.2 — Multi-Agent Tier S Model Lock

### 21.1 Motivation

Multi-agent mode delegates to sub-agents that consume tokens fast and only work well when the orchestrator is a strong reasoning model. Before v5.2, users could pick any model — including small/fast ones — and freely change models mid-session. Combined with Sang Dao's token-consumption tracking (merged from main between v1.1.5 and v1.1.6), this exposed three problems:

1. Weak orchestrators producing poor multi-agent decompositions.
2. Mid-session model swaps invalidating prompt cache and mixing token accounting across vendors.
3. The default consumption-blocked auto-switch silently downgrading users from the model they had committed to — fine for single-agent chat, misleading in multi-agent.

### 21.2 Three Guarantees

1. **Tier S allowlist** — only "smart enough" models are pickable in multi-agent mode (Opus, Sonnet, GPT-5.x, Gemini Pro). The list is centrally maintained and overridable per-model from the backend dynamic config.
2. **Session model lock** — once a multi-agent session starts, the chosen model is fixed for that session. To change model, start a new chat.
3. **Lock-after-current-run** — when the session-locked model becomes `blocked: true` (rate-limited per Sang's tracker), the in-flight run completes naturally. The next user submit is refused with a countdown; no silent fallback to a weaker model.

### 21.3 Files Touched

| File | Change |
|---|---|
| `core/config/multiAgentTier.ts` | New — `DEFAULT_MULTI_AGENT_TIER_S` list + `isMultiAgentEligible()` helper |
| `packages/config-types/src/index.ts` | `multiAgentEligible: z.boolean().optional()` added to `modelDescriptionSchema` |
| `core/index.d.ts` | `multiAgentEligible?: boolean` added to `ModelDescription`, `JSONModelDescription`, and serialized types |
| `core/config/entraIDDynamicConfig.ts` | Backend mapper propagates `multiAgentEligible`; dev-only mock override layer added |
| `gui/src/redux/slices/sessionSlice.ts` | `sessionModelLock` field, `setSessionModelLock` / `clearSessionModelLock` reducers, save/load wiring |
| `gui/src/redux/thunks/session.ts` | `sessionModelLock` included in persisted session payload |
| `gui/src/util/autoSwitchBlockedModel.ts` | New `isSessionLocked` parameter; returns `null` when locked |
| `gui/src/hooks/ParallelListeners.tsx` | Reads `sessionModelLock`, passes it through to the auto-switch |
| `gui/src/redux/thunks/streamNormalInput.ts` | Submit-time guard: refuses when session-locked model is blocked |
| `gui/src/components/ModeSelect/ModeSelect.tsx` | Switching to multi-agent locks the model (current if eligible, else first Tier S) |
| `gui/src/components/modelSelection/ModelSelect.tsx` | Picker filters to Tier S models in multi-agent mode; read-only when locked |
| `gui/src/pages/gui/MultiAgentLockWarning.tsx` | New — mid-run banner when blocked status arrives during streaming |
| `gui/src/pages/gui/Chat.tsx` | Mounts `MultiAgentLockWarning` next to `MultiAgentPanel` |

### 21.4 Backend Override Surface

Admins can add or remove Tier S models without an extension release by setting `multiAgentEligible: true | false` per model in the EntraID dynamic config response. Setting `false` overrides the default list (a model otherwise hardcoded as Tier S can be retired from multi-agent without releasing). When the field is omitted, the client falls back to the hardcoded `DEFAULT_MULTI_AGENT_TIER_S` list — a safety net so multi-agent mode keeps working even if the backend response forgets the field.

### 21.5 Dev Test Surface

Two flags, both stripped from production builds via `process.env.NODE_ENV !== "production"`:

- **`RICA_DEV_MOCK_BLOCKED_MODELS`** (env var, core-side) — JSON object keyed by `model_id`, values are partial `ConsumptionLimit` overrides. Applied at the dynamic-config response normalizer. Lets you simulate `blocked: true` without burning real tokens. Example:
  ```bash
  RICA_DEV_MOCK_BLOCKED_MODELS='{"databricks-claude-opus-4-7":{"blocked":true,"blockedUntil":1747000000}}'
  ```
- **`rica.dev.bypassConsumptionLock`** (localStorage, GUI-side) — set in DevTools console:
  ```js
  localStorage.setItem("rica.dev.bypassConsumptionLock", "true")
  ```
  When set, the submit-time guard logs a warning and lets the request through. Lets you keep developing through a simulated lock without changing the mock.

### 21.6 Sub-Agent Inheritance (No Change Needed)

Sub-agents already inherit the orchestrator's model via `extras.llm` in `core/tools/multiagent/runAgentWithRetry.ts`. The session lock fixes the orchestrator's model, so all sub-agents in the session automatically use the locked model. No additional wiring required.

### 21.7 Test Coverage

- `core/config/multiAgentTier.test.ts` — eligibility helper unit tests (default list, backend override, missing fields)
- `gui/src/util/autoSwitchBlockedModel.test.ts` — extended with `isSessionLocked: true` case (must return null even when blocked)

### 21.8 Resolved Limitations

| Limitation (v5.1) | Resolution in v5.2 |
|---|---|
| No model selection per role | Multi-agent now enforces a single, deliberately chosen model for the whole session — orchestrator and sub-agents share it by design |
| Silent downgrade on quota exhaustion | Locked sessions surface the rate-limit explicitly instead of switching models behind the user's back |

---

## 22. v5.3 — Budget Architecture Revert, Live Injection, UX Polish & Guided Tour

**Document Version Bump:** 5.2 → 5.3
**Date:** 2026-05-19
**Branch:** `multiagent-test` (uncommitted; 25+ modified, 3 new GUI files since last commit `fa1127bd7`)
**Last committed state:** `fa1127bd7 feat: multi-agent Tier S model lock + GPT-5 backend compatibility` (v5.2)

### 22.1 Motivation

v5.2 shipped the Tier S model lock and the SessionUsageBar; v5.3 is the v2.0.0 release polish pass. Three problem areas drove the work:

1. **Dynamic 40%-of-remaining sub-agent budget was too tight in practice.** With weighted gating (`tokens_in + tokens_out × 5`), 2 agents on a 200k-threshold model yielded ~40k weighted each — barely enough room for one tool-call carrying a real file payload. Repeatedly tripped the post-call budget gate *before* the side-effect tool had executed, causing the "agent didn't deliver" UX.
2. **Orchestrator was budget-blind.** It saw the SessionUsageBar in the UI but had no in-prompt awareness of how much of the rolling window remained — so spawn decisions were uniform regardless of available headroom.
3. **No first-time-user onboarding.** v2.0.0 introduces multi-agent mode + workflow patterns + Tier S model gating — non-trivial to discover without guidance.

v5.3 also rolls up several smaller fixes (canceled agents missing from history, post-call gate ordering, error stringification, etc.) accumulated since v5.2 shipped.

### 22.2 Sub-Agent Budget Architecture — Reverted to Static 100K Per-Agent

| | Before v5.3 (dynamic) | After v5.3 (static) |
|---|---|---|
| Per-agent budget | min(POOL_PER_AGENT_CAP=100k, floor((threshold − consumed) × 0.4 / N)) | Hardcoded `DEFAULT_AGENT_BUDGET = 100,000` weighted |
| Session-wide cap | 400k weighted, throws on overshoot | Commented out (kept code, easy to re-enable) |
| Fallback when no `consumptionLimit` | `FALLBACK_AGENT_BUDGET = 100k` (separate code path) | Same 100k path — no fallback distinction |
| Batch trim on insufficient pool | Yes — reduce N agents to fit `MIN_USEFUL_BUDGET = 20k` floor | None — let backend gate + per-agent cap do the work |
| Refusal on insufficient budget | Throws `INSUFFICIENT BUDGET — Agents NOT spawned` | Removed |

**Why the revert:** Comparing against Claude Code, Cursor, and Codex showed none of them do client-side pool math — they trust the model's natural step limits + the API rate limiter. RICA's added value over those systems is Sang's per-user threshold tracking, but that's enforced server-side. The 40%-pool layer was a fourth safety net on top of three that already existed (backend threshold, cancel-on-block hook, step limit) — and was actively hurting the UX by being the tightest of the four.

**Files modified:**

| File | Change |
|---|---|
| `core/tools/implementations/spawnAgents.ts` | Deleted dynamic pool math (`SUBAGENT_POOL_RATIO`, `POOL_PER_AGENT_CAP`, `MIN_USEFUL_BUDGET`, `FALLBACK_AGENT_BUDGET`, the `extras.llm.consumptionLimit` lookup, the trim-N logic). Commented out (preserved as comments) the 400k `SESSION_TOKEN_BUDGET` enforcement block + the mid-batch session check inside `runPhasedParallel`. Replaced batch loop's `tokenBudget` parameter with the static `DEFAULT_AGENT_BUDGET = 100_000` constant. |
| `gui/src/pages/gui/SessionUsageBar.tsx` | Removed the SUB-AGENT BUDGET PREVIEW debug panel (rows 1×–5× with per-N estimates). The bar + percent + other-models list remain. |

**What survives the revert (still active):**

- Pre-spawn lock check (refuses to spawn if backend says `blocked: true`)
- Cancel-on-block hook in `ConfigHandler` (cancels in-flight sub-agents on transition)
- Weighted gating in `SubAgentRunner` (output × 5 to match backend formula)
- Step limit (15 iterations per agent)
- `_logEnd` real-API-counts capture (v5.2 fix)
- Tool-call sanitize for malformed `arguments` (v5.2 fix)
- Lock-at-submit + save-on-error (v5.2 fixes)

### 22.3 Live `<session_budget>` Injection to Orchestrator

The orchestrator now receives a per-turn `<session_budget>` block in its system prompt, summarizing the active model's consumption against threshold:

```
<session_budget>
Model: <name>
Backend (last poll, may lag minutes): X / threshold (rolling window)
This session's local activity: Y weighted tokens (real-time, exact for this session)
Effective consumed (max of backend + local): Z / threshold (PCT%)
Remaining (effective): R
Note: weighted = prompt_tokens + completion_tokens × 5 (backend formula).
Each sub-agent has a 100k weighted cap (≈ 20k completion tokens).
Guidance: if remaining < 60k, prefer single agent or sequential.
If remaining < 30k, decline to spawn — synthesize what you have and tell the user.
</session_budget>
```

**Effective formula:** `max(backend.consumed, sessionLocalWeighted)` — not sum, to avoid double-counting tokens that have already been POSTed and are reflected in `backend.consumed`. Backend lags up to ~60s (poll cadence) plus per-user cache lag; local count is exact for the current Redux session. Whichever is higher is the conservative ceiling.

**Local computation:** uses the existing `selectSessionTokens` selector (drives `SessionTokenBadge`), summing across `history[].promptLogs` (orchestrator + chat + agent calls) and `subAgentProgress[].tokenUsage` (sub-agent calls). Weighted as `(promptTokens) + (completionTokens) × 5`.

**Wiring:**

| File | Change |
|---|---|
| `gui/src/util/index.ts` | New `buildSessionBudgetContext(consumptionLimit, modelTitle, sessionLocalWeighted)` helper. `getBaseSystemMessage` accepts new optional `consumptionLimit` + `sessionLocalWeighted` params and appends the budget block in multi-agent mode. |
| `gui/src/redux/thunks/streamResponse.ts` | Reads `selectSessionTokens` from store, computes weighted, passes through. |
| `gui/src/redux/thunks/streamResponseAfterToolCall.ts` | Same wiring on the post-tool-call path. |
| `core/llm/multiAgentSystemMessage.ts` | Replaced stale "There is a SESSION TOKEN BUDGET of 400K tokens" guidance with reference to the dynamic block. |

**Verified live behavior:** session 90eedd16 (Opus 4.7) progressed local: 0 → 9k → 18k → 28k weighted across 4 orchestrator turns. Session 01cd75a0 (GPT 5.4, 33 orch steps, threshold 300k) crossed 100% effective at step 43 and the orchestrator started refusing further spawns explicitly: *"I can't responsibly edit them in this session. Why: tool/session budget is exhausted..."*

This established that **with accurate live data + the existing decision framework, both Opus 4.7 and GPT 5.4 self-throttle correctly** — no prompt-engineering imperatives required.

**Known gap:** Sang's per-user response cache can lag local by several minutes. In session dc4acaef the backend froze at `consumed=185,562` for 16 orchestrator steps while local grew from 171k to 348k. Backend never reported `blocked: true`, so the mid-stream abort hook never fired. Effective number kept the orchestrator honest, but the session over-ran the threshold by 74% before the orchestrator self-stopped via "synthesize results" guidance. Accepted for v2.0.0.

### 22.4 MultiAgentLogger Improvements

| Change | File | Rationale |
|---|---|---|
| System prompt log truncation 8k → 20k chars | `core/tools/multiagent/MultiAgentLogger.ts:orchestratorMainTurn` | Static template alone is ~10k; was getting cut before any appended dynamic context (registry summary, mode-transition note, **budget block**) was visible in the log |
| Dedicated `[INJECTED BUDGET BLOCK]` log entry | Same method | Extracts the live-injected block via regex requiring leading newline + `Model:` line. Gives a clean per-turn record of the exact numbers the orchestrator saw, separate from the static prompt body. |
| Error stringification | `gui/src/redux/thunks/streamThunkWrapper.tsx` catch branch | Was logging `[object Object]` for non-Error throws. Now: Error → message + stack; object with `message` field → `[HTTP X] message`; plain object → `JSON.stringify`; fallback → `String(e)`. Captures the actual error type when LLM streams fail. |

### 22.5 Blocked-Model Guards — Symmetric Coverage

v5.2 introduced the multi-agent submit-time guard and the cancel-on-block hook for sub-agents. v5.3 closes the gaps:

| Layer | Covers | Implementation |
|---|---|---|
| Submit-time | Refuses NEW prompts on blocked model | `streamNormalInput.ts` — was multi-agent only, now applies to ALL modes. Multi-agent gets the "Start a new chat" hint; chat/agent gets the rate-limit message (auto-switch in `ParallelListeners` is the primary path; this is the safety net) |
| Mid-stream (sub-agents) | Cancels running sub-agents on transition | Existing `ConfigHandler._refreshDynamicConfig` → `AgentManager.cancelAllForSession` (v5.2) |
| **Mid-stream (orchestrator)** — NEW v5.3 | Cancels orchestrator's chat stream on transition | `ParallelListeners.handleConfigUpdate` — on every 60s poll, if `incomingSelectedByRole.chat.consumptionLimit.blocked === true` AND `isStreamingRef.current === true`, dispatches `cancelStream()`. Logs `[BLOCKED-MIDSTREAM]`. Symmetric with the sub-agent path. |

### 22.6 Two Long-Standing Behavioral Bugs Resolved

**22.6.1 Canceled Agents Vanishing From Agent History**

`AgentManager.cancelAgent`, `cancelAllForSession`, and `cancelAll` were calling `this.agents.delete(id)` after marking status to `"canceled"`. The live progress diagram (driven by Redux `subAgentProgress`) showed the canceled robots correctly, but Agent History (driven by `getAgentsForSession`) read directly from the manager's map and saw nothing.

**Fix:** removed the `.delete(id)` calls. Records persist with `status: "canceled"`. They're already filtered out of `checkDuplicate` (only matches `running` / `completed+result`) and `getActiveSessions` (only `running`), so leaving them in is safe.

**22.6.2 Post-Call Budget Gate Firing Before Tool Execution**

The "agent didn't deliver" UX. `SubAgentRunner.run()` ran the LLM, then immediately checked `totalTokensUsed >= budget`, and **returned with the partial result before the iteration's tool calls executed**. When the LLM had emitted a `builtin_create_new_file` with the full HTML payload as `arguments`, the file write never happened — the work was generated then discarded.

**Fix:** moved the post-call check to after the tool-execution loop and the cancel-fill block. New per-iteration order:

```
LLM call
push assistant message
extract tool calls
execute each tool call (file writes, terminal commands, edits)
fill missing tool results (for aborted-mid-tool-loop case)
POST-CALL BUDGET CHECK   ← moved here
prune messages
```

Tool execution is local — `fs.writeFile`, `child_process`, ripgrep, etc. — and burns zero LLM tokens. The one exception is `builtin_edit_existing_file`, which calls an LLM internally for diff apply (~2-5k tokens). So the worst-case overshoot from the new ordering is ~5k weighted on the iteration that trips, only when an Edit was in that iteration. Strictly better than losing the entire iteration's output.

### 22.7 Tier S Availability Gating in ModeSelect

v5.2 made multi-agent mode require a Tier S model as the orchestrator. v5.3 closes the bug where a user could sidestep that lock:

**The bug:** all Tier S models blocked → user goes to chat mode → switches to a non-Tier-S model (legal in chat) → switches back to multi-agent → the lock commits the non-Tier-S model.

**Fix — two layers:**

| Layer | Behavior |
|---|---|
| **Layer 1: Disable the multi-agent option** | New `multiAgentAvailable` memo in `ModeSelect.tsx` — true if either the current model is unblocked Tier S OR any unblocked Tier S exists in the chat models list. When false: dropdown option is disabled, label changes to `(All blocked)`, hover tooltip reads "All multi-agent eligible models are rate-limited. Switch model or wait.", and the keyboard mode-cycle skips the multi-agent slot. `selectMode` also rejects programmatic attempts. |
| **Layer 3: Auto-switch on entry** | When the user clicks Multi-Agent and the current model isn't unblocked Tier S, `enterMultiAgent` dispatches `updateSelectedModelByRole({role: "chat", modelTitle: <unblocked Tier S>})` BEFORE calling `setMode("multi-agent")`. Logs `[ModeSelect] Auto-switching to Tier S model "X"`. The submit-time lock then commits the auto-switched model. |

**New helper** in `gui/src/util/autoSwitchBlockedModel.ts`:

```typescript
export function findUnblockedTierSModel(
  chatModels: ModelDescription[],
): ModelDescription | null {
  return (
    chatModels.find(
      (m) =>
        isMultiAgentEligible(m as any) &&
        m.consumptionLimit?.blocked !== true,
    ) ?? null
  );
}
```

Reused by both layers.

### 22.8 90% Usage Warning Toast

A transient amber toast that pops at the top of the webview when the active model's effective consumption crosses 90% of its threshold. **Cycle-based:** fires only on a fresh below→above transition for that model — to re-fire, usage must drop below 90% (window slides forward) and climb back up. **Self-destruct:** 3-second `setTimeout` dispatches `clearUsageWarning`.

| File | Role |
|---|---|
| `gui/src/redux/slices/uiSlice.ts` | Adds `usageWarning` state + `showUsageWarning` / `clearUsageWarning` reducers. The `id` ticks up on every fire so a new toast cleanly replaces an in-flight one and the auto-dismiss timer resets. |
| `gui/src/components/gui/UsageWarningToast.tsx` | NEW — fixed-position amber toast with `ExclamationTriangleIcon`, mounted at the top of `Chat.tsx`. `useEffect([warning?.id])` schedules the 3s clear. |
| `gui/src/hooks/ParallelListeners.tsx` | Trigger logic in `handleConfigUpdate`. Computes effective consumed via `selectSessionTokens` + `consumptionLimit.consumed` and the same max-of-both formula. Per-model `usageWarningAboveRef` tracks "currently above" for cycle gating. Wording: "Approaching token limit (X%) — session will lock soon." |

### 22.9 Multi-Agent Guided Tour

A 3-step floating-UI tour pointing first-time users through multi-agent setup. Replaces an earlier single-step "nudge" that pre-dated this version.

**Steps:**

| # | Anchor | Title | Auto-advance trigger |
|---|---|---|---|
| 1 | `[data-testid="mode-select-button"]` | Try Multi-Agent mode | User picks "Multi-Agent" from the mode selector → `mode === "multi-agent"` |
| 2 | `[data-testid="workflow-pattern-select-button"]` | Pick a workflow pattern (with one-line per-pattern descriptions) | User clicks **Next** |
| 3 | `[data-testid^="continue-input-box-"]` | Now describe your task | User clicks **Done** → tour finishes |

**Anchor resolution** retries every 200ms up to 25 attempts (~5s total) — needed because Step 2's anchor (`workflow-pattern-select-button`) only mounts when `mode === "multi-agent"`, and there's a render gap between the user clicking Multi-Agent and the workflow-pattern selector appearing.

**Bilingual** — built-in EN / 日本語 toggle in the top-right of the tour card. Switches all step titles, body content, button labels, and step counter (`Step X of 3` ↔ `ステップ X / 3`). Translations are inline in the component as a `Record<Lang, StepLocalized>`, not extracted to a locale file (single-feature scope).

**Animations** (CSS keyframes injected via `<style>` block — Tailwind animation utilities not available in the webview):

| Animation | Target | Effect |
|---|---|---|
| `ricaTourEnter` (280ms cubic-bezier ease-out) | Tour card on mount | Fades from 0→1 opacity, slides from -6px translateY, scales 0.96→1.01→1 (subtle bounce) |
| `ricaTourArrowPulse` (1.8s loop) | The floating arrow | Scale 1→1.4→1 with opacity 1→0.65→1 — draws the eye toward the anchored element |
| `ricaTourBtnPulse` (2.4s loop) | Next / Done button | Box-shadow expands and contracts, reinforcing it as the primary action |
| `ricaTourContentFade` (220ms) | Inner title + body divs | Fades in step content on every step / language change |

**Position-lag bug fixed during build:** initially used `key={tour-step-${stepIndex}-${lang}}` on the tour card to force the entrance animation to re-fire on each step change. This caused floating-ui to flash the card at default `top: 0 / left: 0` for one frame before recalculating position. Fix: removed `key` from the card; moved it to the inner `.rica-tour-content` divs so only content remounts (not the positioned shell).

**Session-aware reset bug fixed during build:** `dismissedRef.current` persisted across session changes because `Chat.tsx` doesn't unmount the tour on navigation — it just resets internal state. New chat clicks looked broken (no tour). Fix: watch `sessionId` in the show-decision effect; reset all tour state on every session change.

**Persistence:** localStorage key `hasSeenMultiAgentTour_v1`. Once dismissed (X / Skip / Done), the tour never reappears for that user (window). To reset for QA, clear the key in DevTools or flip the in-source `TESTING_ALWAYS_SHOW = true` flag.

**Files:**

| File | Change |
|---|---|
| `gui/src/components/gui/MultiAgentNudge.tsx` | NEW — 3-step tour with EN/JP, animations, anchor retry, sessionId-based reset. (File kept its older "Nudge" name despite now being a tour — saves the import rename.) |
| `gui/src/pages/gui/Chat.tsx` | Mounts `<MultiAgentNudge />` next to `<UsageWarningToast />` |
| `gui/src/components/ModeSelect/WorkflowPatternSelect.tsx` | Added `data-testid="workflow-pattern-select-button"` to `<ListboxButton>` so the tour can anchor |
| `gui/src/util/localStorage.ts` | Added `hasSeenMultiAgentTour_v1: boolean` to `LocalStorageTypes` |

### 22.10 File Change Summary (v5.3)

**Core (modified):**

- `core/tools/implementations/spawnAgents.ts` — dynamic budget removed, session cap commented
- `core/tools/multiagent/AgentManager.ts` — canceled agents preserved in map
- `core/tools/multiagent/SubAgentRunner.ts` — post-call budget gate moved after tool execution
- `core/tools/multiagent/MultiAgentLogger.ts` — 20k truncation + `[INJECTED BUDGET BLOCK]` extraction
- `core/llm/multiAgentSystemMessage.ts` — stale 400k SESSION BUDGET text replaced with budget-block reference
- `core/tools/multiagent/runAgentWithRetry.ts` — comment updated for new tokenBudget semantics
- `core/llm/llms/Databricks.ts` — verified tools sent every iteration (relevant context for v5.4 prompt-cache work)
- `core/llm/llms/stubs/RicaProxy.ts`, `core/llm/index.ts`, `core/config/ConfigHandler.ts`, `core/config/entraIDDynamicConfig.ts`, `core/llm/toolSupport.ts` — minor updates carrying forward May 16-18 work

**GUI (new):**

- `gui/src/components/gui/UsageWarningToast.tsx` — 90% warning toast component
- `gui/src/components/gui/MultiAgentNudge.tsx` — guided tour component
- `gui/src/pages/gui/SessionUsageBar.tsx` — extracted from MultiAgentPanel (moved/promoted)

**GUI (modified):**

- `gui/src/util/index.ts` — `buildSessionBudgetContext` helper + extended `getBaseSystemMessage` signature
- `gui/src/util/autoSwitchBlockedModel.ts` — `findUnblockedTierSModel` helper
- `gui/src/util/localStorage.ts` — new tour key
- `gui/src/redux/thunks/streamResponse.ts` — wires `sessionLocalWeighted` through
- `gui/src/redux/thunks/streamResponseAfterToolCall.ts` — same on post-tool-call path
- `gui/src/redux/thunks/streamThunkWrapper.tsx` — proper error stringification
- `gui/src/redux/thunks/streamNormalInput.ts` — submit-time guard extended to all modes
- `gui/src/redux/slices/uiSlice.ts` — `usageWarning` state + reducers
- `gui/src/hooks/ParallelListeners.tsx` — mid-stream abort hook + 90% warning trigger
- `gui/src/components/ModeSelect/ModeSelect.tsx` — Tier S availability gating + auto-switch on entry
- `gui/src/components/ModeSelect/WorkflowPatternSelect.tsx` — testid for tour anchor
- `gui/src/pages/gui/Chat.tsx` — mounts `UsageWarningToast` and `MultiAgentNudge`
- `gui/src/pages/gui/MultiAgentPanel.tsx`, `gui/src/components/mainInput/ContinueInputBox.tsx`, `gui/src/components/mainInput/TipTapEditor/TipTapEditor.tsx`, `gui/src/components/modelSelection/MultiAgentModelSelect.tsx` — minor updates carrying forward May 16-18 work

### 22.11 Resolved Limitations

| Limitation (v5.2) | Resolution in v5.3 |
|---|---|
| Orchestrator unaware of session-level token budget | Live `<session_budget>` block injected per turn, max-of-(backend, local) |
| "Agent didn't deliver" — file writes lost when budget tripped during write | Post-call gate moved after tool execution; tools land before exit |
| Canceled sub-agents disappeared from Agent History | Records preserved with `status: "canceled"` |
| Submit-time block guard only covered multi-agent mode | All modes (chat / agent / multi-agent) refuse on `blocked: true` |
| Orchestrator's chat stream kept running when active model became blocked mid-flight | New `[BLOCKED-MIDSTREAM]` hook in `ParallelListeners` cancels within 60s |
| Tier S lock could be sidestepped by chat-mode model swap before mode change | Two layers: disable multi-agent option when no unblocked Tier S, auto-switch to one on entry |
| First-time multi-agent users had no onboarding guidance | 3-step guided tour with EN/JP and animations |
| Errors logged as `[object Object]` | Proper Error / object / string stringification |

### 22.12 Remaining Limitations

1. **5-minute IPC timeout on long Implementer runs.** The orchestrator's `builtin_spawn_agent` tool-call IPC channel has a 5-minute cap. Implementers running >5min cause the orchestrator to receive an `[llm_error]` while the agent keeps running independently and finishes. Orchestrator never gets the result to synthesize. Deferred to v5.4.
2. **Per-user backend cache lag.** Sang's `/api/user-model/configs` response cache can serve stale `consumed` for several minutes even on a fresh poll with the same token. The injection's `max(backend, local)` formula keeps the orchestrator honest but the SessionUsageBar (backend-only) can read stale.
3. **Local-vs-backend over-count ~16% at parity-points.** Probably backend-side dedup on `(user_id, model_id, client_timestamp)` combined with historical tiktoken-vs-API-counts mixing in the SQLite store. Safe-direction (orchestrator self-throttles earlier than necessary).
4. **Tools sent on every iteration with no prompt-cache.** `Databricks.ts:116` passes `tools: options.tools` fresh per call. ~1.5-3k tokens of static tool schemas burned on every iteration; for a 7-iteration agent that's ~14% of the 100k cap. Adding `cache_control: { type: "ephemeral" }` on the tools array would cut this ~10x via Anthropic prompt caching. Deferred to v5.4.
5. **Tour is not retriggerable from the UI.** Auto-on-first-load shipped; the "Show tour" button (in settings/help icon) deferred to v5.4. To re-trigger after dismissal, manually clear `localStorage.hasSeenMultiAgentTour_v1`.
6. **Diagnostic logs still in production code.** `[CONSUMPTION]`, `[LOCK-DEBUG]`, `[USAGE-WARN]`, `[BLOCKED-MIDSTREAM]`, `[ModeSelect]` auto-switch logs. Some are useful (USAGE-WARN, BLOCKED-MIDSTREAM); others (LOCK-DEBUG) were for the now-resolved lock-not-persisting bug. Decide gate strategy before commit cleanup.

---

## 23. v5.4 — v1.1.8 Triage Closeout: Spawn IPC, Tool Timeouts & Circuit Breakers

**Document Version Bump:** 5.3 → 5.4
**Date:** 2026-05-20
**Branch:** `multiagent-test` (uncommitted at time of writing; v1.1.7 shipped at `ddf6a5fd8`)
**Last committed state:** `ddf6a5fd8 feat: v1.1.7 — multi-agent live budget injection, guided tour, blocked-model guards, UX polish` (v5.3)

### 23.1 Motivation

v1.1.7 shipped on May 19. Within hours, tester Bruce Pham filed two reports against session `2b2f5b7f`:

1. **Silent orchestrator after a single sub-agent finishes.** Workaround: copy the sub-agent's output and paste it back into the conversation manually so the orchestrator could synthesize.
2. **`applyToFile timed out after 60000ms` on large file edits**, killing the tool call mid-stream.
3. (Discovered by reading the same log) **Malformed-JSON tool args** — the LLM emitting tool calls with truncated `arguments` that wouldn't parse, producing `Expected ',' or '}' after property value in JSON at position N` and pushing the orchestrator/sub-agent into a retry loop.

A subsequent in-house stress test surfaced two further failure modes:

4. **Sub-agent retry loops on the same JSON-truncation pattern**, escaping the orchestrator-side circuit breaker (the loop was happening *inside* `SubAgentRunner`'s tool-execution loop, not on the GUI dispatch path).
5. **`tools/call` IPC ceiling at 5 minutes** killing legitimately-long Implementer/Researcher runs at exactly 5:00 with `IdeMessenger.request("tools/call") timed out` — the agent kept working independently but the orchestrator never received the result.
6. **Outer client-tool wrapper at 120 seconds** racing against (and beating) the inner `applyToFile` 5-min cap, masking Fix 1 for any single edit >2:00 wall time.

v5.4 closes all six bugs with seven targeted patches plus a deferred config-level root-cause fix (DEFAULT_MAX_TOKENS) for v1.1.9.

### 23.2 Fix 1 — applyToFile IPC Timeout 60s → 5min

**Bug A.** `editImpl.ts` was calling `ideMessenger.request("applyToFile", ...)` without a timeout argument, defaulting to the IDE messenger's 60-second cap. Files large enough that the diff-stream + IDE WorkspaceEdit application took longer than 60s were killed mid-write, leaving the orchestrator with a `ToolCallError` on what looked like an otherwise correct LLM output.

**Fix.** Pass `300_000` (5 min) as the 4th argument to the request call. Ceiling, not fixed wait — small edits still return in ms.

**File:** [`gui/src/util/clientTools/editImpl.ts`](../gui/src/util/clientTools/editImpl.ts) (1-line change).

```ts
const response = await ideMessenger.request(
  "applyToFile",
  { streamId, toolCallId, text, filepath, isSearchAndReplace },
  300_000,  // was implicit 60_000 default
);
```

### 23.3 Fix 2 — Orchestrator-Side Circuit Breaker

**Bug B (orchestrator-side).** When the LLM emits a tool call with truncated `arguments` (e.g., the JSON cuts off mid-string at position N), `JSON.parse` throws and the tool dispatcher returns an error. The orchestrator sees the error, decides to retry, emits the same truncated args (same prompt → same thinking → same stop point), and loops. Repeats indefinitely until the user manually intervenes or the session runs out of budget.

**Fix.** Module-level circuit breaker keyed `${sessionId}:${toolName}:${target}` where `target` is `args.filepath` if present, else first 80 chars of the args JSON (treated as a fingerprint). After `CIRCUIT_BREAKER_THRESHOLD = 2` consecutive failures on the same key, the dispatcher synthesizes a stop-result message instead of running the tool again, breaking the loop deterministically. The synthesized message is directive: *"This tool has already failed N times on the same target in this session. STOP retrying this exact call. Try a different approach: a smaller change, a different file, or report the failure to the user."*

Counter is cleared on success, on session change, or when the user submits a new message.

**File:** [`gui/src/redux/thunks/callToolById.ts`](../gui/src/redux/thunks/callToolById.ts) (~40 lines added).

```ts
const CIRCUIT_BREAKER_THRESHOLD = 2;
const toolFailureCounts = new Map<string, number>();
let lastSessionIdSeen: string | null = null;

function getCircuitKey(sessionId, toolName, argsJson) {
  let target = "";
  try {
    const args = JSON.parse(argsJson || "{}");
    target = typeof args.filepath === "string" ? args.filepath : argsJson.slice(0, 80);
  } catch {
    target = argsJson.slice(0, 80);
  }
  return `${sessionId}:${toolName}:${target}`;
}

// Wipe failure counters on session change so old loops don't poison new chats
if (lastSessionIdSeen !== sessionIdAtStart) {
  toolFailureCounts.clear();
  lastSessionIdSeen = sessionIdAtStart;
}

const priorFailures = toolFailureCounts.get(circuitKey) || 0;
if (priorFailures >= CIRCUIT_BREAKER_THRESHOLD) {
  errorMessage = `This tool (${toolName}) has already failed ${priorFailures} times on the same target...`;
  streamResponse = true;  // synthesize stop, do not call the tool
}

// After execution:
if (errorMessage) toolFailureCounts.set(circuitKey, priorFailures + 1);
else toolFailureCounts.delete(circuitKey);
```

### 23.4 Fix 3 — Silent-Orchestrator Re-Arm for Spawn Tools

**Bug C.** v1.1.7's mid-stream blocked-model abort hook (`ParallelListeners.handleConfigUpdate`, dispatches `cancelStream()` when the active model becomes blocked mid-flight) had a side effect: during a single long-running `builtin_spawn_agent` call, the 60-second poll fires at least once. If the poll happens to flip `isStreaming=false` for any reason (transient blocked status, polling race with cancel hook re-evaluation), the existing guard at `callToolById.ts:171` would silently `return` instead of calling `streamResponseAfterToolCall` — leaving the orchestrator mute after the sub-agent finished.

The user-visible symptom matched Bruce's report exactly: *"my sub-agent finished but the orchestrator never said anything; I had to copy its output myself and paste it back."*

Multi-agent batch spawns dodged the trap because parallel agents typically finish faster than the 60s poll interval. Single-agent spawns running >60s reliably tripped it.

**Fix.** When the post-tool-call code reaches the `!isStreaming` branch, check whether the just-finished tool was `builtin_spawn_agent` or `builtin_spawn_agents`. If so, log a re-arm and continue — `streamResponseAfterToolCall` flips `isStreaming=true` internally. For non-spawn tools, retain the original early-return behavior (we do not want runaway orchestrator continuations on regular tool calls when the user has truly canceled).

**File:** [`gui/src/redux/thunks/callToolById.ts`](../gui/src/redux/thunks/callToolById.ts) (~7 lines added inside the existing `streamResponse` branch).

```ts
if (!currentState.session.isStreaming) {
  const toolName = toolCallState.toolCall.function.name;
  const isSpawnTool =
    toolName === "builtin_spawn_agent" ||
    toolName === "builtin_spawn_agents";
  if (!isSpawnTool) {
    console.log(`[callToolById] Stream no longer active, skipping streamResponseAfterToolCall`);
    return;
  }
  console.log(`[callToolById] Re-arming stream for spawn tool ${toolName} (orchestrator continuation)`);
  // streamResponseAfterToolCall sets isStreaming=true internally
}
```

### 23.5 Fix 4 — JSON-Truncation User-Facing Error Dialog

**Bug B (UX layer).** When a JSON parse failure escaped the circuit breaker (first strike) or originated upstream of the dispatcher, the user saw the raw `SyntaxError` from `JSON.parse` in the StreamError dialog: *"Expected ',' or '}' after property value in JSON at position 1512"* — meaningless to non-engineers and unhelpfully framed (the model didn't malfunction; the response was cut off).

**Fix.** New detection branch in `StreamError.tsx` matching the four common syntax-error patterns (`Expected ',' or '}'`, `JSON at position`, `after property value in JSON`, `Unexpected end of JSON`) and rendering an empathetic, actionable message: *"the model's response was cut off mid tool-call. This usually means the session is approaching the context limit. Start a new session, or compact this one to free up room."*

This is a user-facing message; it does not affect the LLM-facing circuit-breaker text from Fix 2.

**File:** [`gui/src/pages/gui/StreamError.tsx`](../gui/src/pages/gui/StreamError.tsx) (~20 lines, new branch above the existing "overloaded/malformed" branch).

### 23.6 Fix 5 — Sub-Agent-Side Circuit Breaker

**Bug B-sub.** Stress-test session `7547b0fc` Phase 2 (3000-line README → edit-for-errors task) reproduced the position-34 truncation pattern — but inside the Implementer sub-agent's own tool-execution loop, not on the GUI side. The orchestrator-side circuit breaker (Fix 2) never saw the failures because the sub-agent's `tools/call` IPC was returning `success` to the GUI dispatcher (the tool error was wrapped in the agent's own iteration result). The Implementer looped 8 times on `builtin_create_new_file` with identical 34-char-truncated args before exhausting its step limit.

**Fix.** Mirrored circuit-breaker pattern in `SubAgentRunner.ts`. Map<string, number> scoped *per sub-agent run* (not module-level — wiped when the agent class is constructed, not shared across agents). Key: `${toolName}:${target}` (filepath when present, else 80-char arg slice — same heuristic as Fix 2 minus the sessionId prefix, which is implicit per-run). Threshold 2.

On trip, synthesize a stop-result with directive language tuned to the truncation hypothesis: *"Error: This tool has already failed N times with identical args. STOP retrying this exact call — your tool-call JSON args are likely being truncated mid-stream. Try a smaller payload, a different filepath, or summarize what you have to the orchestrator."*

**File:** [`core/tools/multiagent/SubAgentRunner.ts`](../core/tools/multiagent/SubAgentRunner.ts) (~50 lines added).

**Live verification:** `cd037ad6` Implementer-1 hit the breaker after exactly 2 strikes on a synthetic JSON-truncation prompt — exactly as designed. See [§23.10](#2310-combo-stress-test-cd037ad6--live-verification).

### 23.7 Fix 6 — Spawn-Tool IPC Timeout 5min → 20min

**Bug D.** With Fix 5 in place, sub-agents now reliably iterate past truncation strikes (instead of being killed by them at strike 1) and routinely run 8–10 minutes for non-trivial tasks like 3000-line refactors. The `tools/call` IPC channel in `callToolById.ts` was hardcoded at 300_000ms (5 min) — sufficient for v1.1.7's truncation-killed agents but a hard ceiling for v1.1.8's resilient ones. Stress test `6e71c198` Phase 1 saw `IdeMessenger.request("tools/call") timed out after 300000ms` at exactly the 5:00 mark while Implementer-1 was still active and would eventually finish at 9:46 wall time.

**Fix.** Detect spawn tools by name and use a 20-minute IPC ceiling for them; keep 5-minute for everything else.

**File:** [`gui/src/redux/thunks/callToolById.ts`](../gui/src/redux/thunks/callToolById.ts) (~10 lines).

```ts
const isSpawnTool =
  toolName === "builtin_spawn_agent" ||
  toolName === "builtin_spawn_agents";
const ipcTimeoutMs = isSpawnTool ? 1_200_000 : 300_000;
const result = await extra.ideMessenger.request(
  "tools/call",
  { toolCall: toolCallState.toolCall },
  ipcTimeoutMs,
);
```

**Why 20min and not unbounded:** sub-agents have their own 100k-token cap (≈200 step ceiling), so even a runaway Researcher will self-stop within ~15 min on most models. 20-minute IPC is comfortably above that; making it unbounded would defeat the purpose of having an IPC kill-switch for genuinely-deadlocked calls.

### 23.8 Fix 7 — Client-Tool Outer-Wrapper Timeout 120s → 5min

**Bug E.** Discovered live during the cd037ad6 stress test. The client-tool dispatch path in `callToolById.ts` (the branch for tools listed in `CLIENT_TOOLS_IMPLS` — including the various edit/apply tools) wraps the inner `callClientTool` Promise in a `Promise.race` against a `setTimeout(reject, CLIENT_TOOL_TIMEOUT_MS)`. The constant was set to **120_000ms** at the start of v1.1.7 work as a "freeze prevention" cap. Fix 1 raised the *inner* `applyToFile` IPC ceiling to 5 min — but the outer 120s race was still primary, so any single edit taking longer than 2:00 wall time was killed by the outer wrapper before the inner edit had a chance to finish.

cd037ad6 confirmed this: a 200-line TOC prepend on a ~9KB file took ~2:00 to apply (reasonable for a streamed diff over IPC + IDE WorkspaceEdit), and the outer race fired at exactly 120s with `Client tool timed out after 120s — the edit operation did not complete`.

**Fix.** Bump `CLIENT_TOOL_TIMEOUT_MS` to **300_000** so it matches the inner ceiling and stops being the primary kill-switch for slow but legitimate edits. The outer wrapper still serves its original purpose (preventing infinite session freeze if the inner Promise never resolves) but no longer truncates ahead of the inner cap.

**File:** [`gui/src/redux/thunks/callToolById.ts`](../gui/src/redux/thunks/callToolById.ts) (1-line change + multi-line comment explaining the relationship to Fix 1).

```ts
// Tool is called on client side — with timeout to prevent session freeze.
// Must be >= the inner applyToFile IPC ceiling (5 min, set in editImpl.ts
// via Fix 1) — otherwise this outer race fires before the inner edit can
// complete on large files. cd037ad6 stress test confirmed 120s outer cap
// killed a 200-line prepend on a ~9KB file at exactly 2:00.
const CLIENT_TOOL_TIMEOUT_MS = 300_000;
```

**Reversibility note:** trivially revertable (single-line literal change). Risk: low — the outer wrapper is a safety net, raising its threshold makes it less aggressive but does not change semantics. Prior 120s value was an arbitrary "feels short enough" pick, not a tuned constant.

### 23.9 Position-34 Truncation Root Cause (Deferred to v1.1.9)

Multiple stress tests reproduced an unusual signature: tool-call args truncating at exactly **34 characters** (`{"filepath": "LONG_TEST_README.md"`) every time on the 3000-line README task. Same task → same thinking → same stop point → 8/8 identical truncations.

**Root cause traced:** [`core/llm/constants.ts:1`](../core/llm/constants.ts) sets `DEFAULT_MAX_TOKENS = 4096`. `SubAgentRunner.callLLM` at line 1083 invokes `model.streamChat(messages, signal, { tools })` with **no `maxTokens` override**. For the 3000-line README task, Claude emits ~4060 tokens of thinking content before starting the tool call, then runs out at exactly 34 chars of arguments.

**Why deferred:** bumping `maxTokens` (to e.g. 16384) is a config-level change touching every sub-agent LLM call — could affect cost, latency, and behavior on workflows that intentionally run with shorter ceilings. Position-34 is now contained by the sub-agent circuit breaker (Fix 5) on the runtime side and surfaced clearly to the user (Fix 4) on the UX side. The proper config fix lands in v1.1.9 with appropriate per-role tuning (Researcher/Implementer/Synthesizer may need different ceilings).

**Where the v1.1.9 fix should land:** `SubAgentRunner.ts:1083` — pass `{ tools, maxTokens: 16384 }` (or pull from model config when set). Investigate per-role overrides separately.

### 23.10 Combo Stress Test cd037ad6 — Live Verification

A combo prompt was constructed to exercise all 6 (then 7) fixes in a single multi-agent session targeting `manual-testing-sandbox/`. Step-by-step monitoring captured each fix firing or being exercised:

| Step | Action | Fix Exercised | Result |
|---|---|---|---|
| 1 | Spawn Researcher to summarize a directory | Fix 6 (20min IPC), Fix 3 (re-arm) | ✅ Researcher returned in ~3min, orchestrator continued |
| 2 | Edit a medium file (200 lines) | Fix 1 (applyToFile 5min) | ✅ Edit completed in ~30s |
| 3 | Edit a larger file (~500 lines) | Fix 1 | ✅ Completed in ~75s |
| 4 | Prepend 200-line TOC to ~9KB file | Fix 1 + Fix 7 (outer wrapper) | ❌ Killed at 2:00 by 120s outer wrapper → led to Fix 7 |
| 5 | Spawn Implementer w/ JSON-truncation seed prompt | Fix 5 (sub-agent breaker) | ✅ Self-stopped at strike 2 with directive message |
| 6 | Re-run step 4 after Fix 7 build | Fix 1 + Fix 7 | ⏳ Verified in built bundle (3e5 literal); user re-runs in IDE |

**Fixes verified live:** Fix 1 (partial, see step 4 → Fix 7), Fix 3, Fix 5, Fix 6.
**Fixes verified by build inspection:** Fix 2 ("Circuit breaker tripped" string), Fix 4 ("model's response was cut off mid tool-call" string), Fix 7 (`3e5` minified literal).
**Fixes not exercised by this stress test:** Fix 2 (would require a real upstream truncation; orchestrator-side cases are rarer than sub-agent-side per cd037ad6 evidence).

### 23.11 Bruce 2b2f5b7f Bug-Status Mapping

Bruce's actual log file (`live_2026-05-19T07-05-16-003Z_2b2f5b7f.json`, 16 turns) was inspected. Initial reading missed an Analyst-1 spawn that lived in the system-prompt history block of later turns (the spawn invocation predated the captured snapshot); see [feedback memory entry on log inspection](../.claude/memory/feedback_log_inspection.md) for the lesson learned.

**Corrected reading:**

- Session DID spawn 1 sub-agent: **Analyst-1** (target: `docs/RICA_COMMIT_REORGANIZATION_PLAN.md`), 7 steps, 122.7s wall, completed with "(Token budget reached)" prefix (hit the 100k cap).
- Analyst-1 returned successfully; subsequent 16 captured turns are pure orchestrator-direct edits responding to Bruce's follow-up Vietnamese prompts.
- Bugs visible during follow-ups: **Bug A** (`applyToFile timed out after 60000ms` at turn 11) and **Bug B** (`JSON at position 1512` at turn 15).
- **Bug C-relevant signal:** Analyst-1 finished in 122.7s — well under the old 5-min IPC cap. Bruce's "silent orchestrator" complaint is best-explained by the post-spawn `cancelStream` from the mid-stream abort hook flipping `isStreaming=false` (Bug C / Fix 3), not by the IPC-timeout class of bugs (Fix 6).

**v1.1.8 status against Bruce's actual session:**

| Bruce report | Maps to bug | Status |
|---|---|---|
| Silent orchestrator after sub-agent | Bug C | **Solved** by Fix 3 |
| applyToFile 60s timeout | Bug A | **Solved** by Fix 1 + Fix 7 (only Fix 1 was needed for his specific 2:00ish edit; Fix 7 raises the ceiling for any future user with longer edits) |
| JSON position-1512 truncation loop | Bug B | **Contained** by Fix 2 (orchestrator-side breaker stops the loop at strike 2) + Fix 4 (better dialog). **Root cause** (DEFAULT_MAX_TOKENS=4096) deferred to v1.1.9 |

### 23.12 File Change Summary (v5.4)

**No new files.** All changes are surgical edits to existing v1.1.7 code paths.

| File | Fixes | Lines | Type |
|---|---|---|---|
| [`gui/src/util/clientTools/editImpl.ts`](../gui/src/util/clientTools/editImpl.ts) | Fix 1 | 1 | Constant change (timeout arg) |
| [`gui/src/redux/thunks/callToolById.ts`](../gui/src/redux/thunks/callToolById.ts) | Fix 2, 3, 6, 7 | ~60 | Module-level state + branch logic + constants |
| [`gui/src/pages/gui/StreamError.tsx`](../gui/src/pages/gui/StreamError.tsx) | Fix 4 | ~20 | New error-detection branch |
| [`core/tools/multiagent/SubAgentRunner.ts`](../core/tools/multiagent/SubAgentRunner.ts) | Fix 5 | ~50 | Per-run circuit breaker |

### 23.13 Resolved Limitations

| Limitation (v5.3) | Resolution in v5.4 |
|---|---|
| 5-minute IPC timeout on long Implementer runs | Fix 6: spawn-tool IPC raised to 20 minutes; non-spawn tools unchanged at 5 min |
| applyToFile 60s default killing large file edits | Fix 1: explicit 5-min IPC override |
| 120s outer client-tool wrapper masking Fix 1 | Fix 7: outer wrapper raised to 5 min to match inner |
| Silent orchestrator after single-agent spawn (Bug C) | Fix 3: re-arm stream when finished tool is a spawn |
| Truncated tool-call JSON args producing infinite retry loop (orchestrator-side) | Fix 2: 2-strike circuit breaker per (sessionId, toolName, target) |
| Same loop, sub-agent-side | Fix 5: mirrored breaker scoped per sub-agent run |
| Cryptic SyntaxError dialog for truncated JSON | Fix 4: empathetic user-facing message with actionable advice |

### 23.14 Remaining Limitations

1. **Position-N truncation root cause not fixed.** `DEFAULT_MAX_TOKENS = 4096` in `core/llm/constants.ts` + sub-agent `streamChat` calls passing no `maxTokens` override. Containment shipped (Fix 5 stops the loop, Fix 4 surfaces actionable message). Proper fix is a config-level change deferred to v1.1.9.
2. **Per-user backend cache lag.** Unchanged from v5.3 — orchestrator self-throttles via the live-injected block.
3. **Tools sent on every iteration with no prompt-cache.** Unchanged from v5.3 — `Databricks.ts:116` still passes `tools: options.tools` fresh per call.
4. **Tour is not retriggerable from the UI.** Unchanged from v5.3.
5. **Diagnostic logs still in production code.** v5.4 adds `[callToolById] Circuit breaker tripped`, `[callToolById] Re-arming stream for spawn tool`, `[IPC-GUI] Sending tools/call: ...`, `[IPC-GUI] Received tools/call response: ...`. The `[IPC-GUI]` logs are useful for ongoing IPC-timeout diagnostics; the breaker logs are useful for confirming the safety net fired. Decide gate strategy alongside v5.3's diagnostic-log audit.
6. **20-minute spawn-tool IPC ceiling is empirical, not principled.** Picked because sub-agents currently self-stop within ~15 min via the 100k-token budget. If model speeds change or token budgets are raised, this ceiling will need re-tuning. A more principled fix would tie the IPC timeout to the agent's remaining budget × a worst-case tokens-per-second floor.

---

## 24. v5.5 — Permission-Gate Silent Orchestrator + Cancel-Button Continuation Fix

### 24.1 Motivation

The v1.1.8 silent-orchestrator carve-out (§23.4 / Fix 3) only re-armed the stream when the finished tool was `builtin_spawn_agent` or `builtin_spawn_agents`. **Any other permission-gated tool** (notably `runTerminalCommand`) hit the same `if (!isStreaming) return` early-exit, leaving the orchestrator silent after the user clicked **Continue** on the tool prompt. The Cancel button on the same toolbar had a parallel leak: it dispatched only the `cancelToolCall` slice action, never the continuation thunk, so rejecting a tool also killed the orchestrator.

### 24.2 Tester Report (May 29, session 2d0e0ab0)

A v1.2.0 tester reported "the multi-agent stops immediately when a generated terminal command errors" and noted the bug "solves when all tools need no permission." Live log [`live_2026-05-29T10-36-07-668Z_2d0e0ab0.json`](#) confirmed the diagnosis was misframed (not error-driven) but the symptom was real.

Decisive evidence in the log: step 17 issues a `builtin_run_terminal_command` tool call. The UI shows the command ran and produced output. **There is no matching `tool_result` step in the orchestrator log between steps 17 and 18.** Steps 7-13 (fast-click) worked; step 17 (after a 23.5-second wait before clicking Continue) silently dropped; step 19 (manual retry) worked again. Same code path, different timing → different outcome.

### 24.3 Fix 1 — Generalize Silent-Orchestrator Re-Arm to All Tools

**File:** [`gui/src/redux/thunks/callToolById.ts`](../gui/src/redux/thunks/callToolById.ts)

**Before (v5.4 / v1.1.8):**

```ts
if (!currentState.session.isStreaming) {
  // Spawn-agent tools are special: ...
  const toolName = toolCallState.toolCall.function.name;
  const isSpawnTool =
    toolName === "builtin_spawn_agent" ||
    toolName === "builtin_spawn_agents";
  if (!isSpawnTool) {
    console.log(`[callToolById] Stream no longer active, skipping streamResponseAfterToolCall`);
    return;
  }
  console.log(`[callToolById] Re-arming stream for spawn tool ${toolName} (orchestrator continuation)`);
}
```

**After (v5.5 / v1.1.9):**

```ts
if (!currentState.session.isStreaming) {
  // Re-arm streaming so the orchestrator can synthesize after ANY tool
  // returns. The mid-stream abort hook (v1.1.7) flips isStreaming=false
  // both during long single-agent spawns AND while the user is
  // approving a permission-gated tool — both paths must continue the
  // conversation, otherwise the orchestrator goes silent after the
  // tool runs. The session-id check above already handles the only
  // legitimate "don't continue" case (user started a new session).
  const toolName = toolCallState.toolCall.function.name;
  console.log(`[callToolById] Re-arming stream for ${toolName} (orchestrator continuation)`);
}
```

**Why this is safe:** the previous early-return existed to guard against continuing in a stale state. The only legitimate "stale" trigger is "user started a new session" — and the session-id comparison four lines above (`currentState.session.id !== sessionIdAtStart`) already returns out of the thunk for that case. The `isStreaming` flag is unreliable as a stale marker because the v1.1.7 mid-stream abort hook flips it false for legitimate-but-paused states (long spawns, awaiting permission). With the session-id check covering the real stale case, dropping the `isStreaming` branch is the correct generalization.

**Result:** any tool — auto-approved, manually approved, spawn or not — re-arms `streamResponseAfterToolCall`, which sets `isStreaming=true` internally and continues the orchestrator loop.

### 24.4 Fix 2 — Cancel-Button Orchestrator Continuation

**Files:**
- [`gui/src/components/mainInput/Lump/LumpToolbar/PendingToolCallToolbar.tsx`](../gui/src/components/mainInput/Lump/LumpToolbar/PendingToolCallToolbar.tsx)
- [`gui/src/components/mainInput/Lump/LumpToolbar/LumpToolbar.tsx`](../gui/src/components/mainInput/Lump/LumpToolbar/LumpToolbar.tsx) (Cmd/Alt+Backspace shortcut)

**Before:** both the visible **Cancel** button and the keyboard shortcut dispatched the bare `cancelToolCall` slice action — which only flips `toolCallState.status = "canceled"` and nothing else. The orchestrator never received a tool-result message and never produced a follow-up. The thunk `cancelCurrentToolCall` (which does the proper sequence) was exported from `gui/src/redux/thunks/cancelCurrentToolCall.ts` but never imported anywhere in the GUI codebase — orphaned.

**After:** both call sites dispatch `cancelCurrentToolCall()` instead. That thunk:

1. Dispatches `cancelToolCall` (status → `"canceled"`)
2. Dispatches `updateToolCallOutput` with a "tool was cancelled by the user" content item
3. Dispatches `streamResponseAfterToolCall` to continue the orchestrator with the cancellation as the tool result

The orchestrator now gets to acknowledge the cancellation ("Okay, I won't run that — what would you like instead?") rather than going mute.

### 24.5 Why It Was Intermittent (Race Condition)

The bug only triggered on slow clicks because of the order of state changes around the permission gate:

1. LLM stream finishes, emits the tool call. Tool state becomes `"generated"`.
2. Auto-approval filter runs ([`streamNormalInput.ts:267-278`](../gui/src/redux/thunks/streamNormalInput.ts#L267-L278)). Permission-gated tools fall out.
3. `streamNormalInput` returns immediately (no auto-approved work to dispatch).
4. `streamThunkWrapper` calls `setInactive()`. **`isStreaming` flips to `false`.**
5. `PendingToolCallToolbar` renders with Continue/Cancel buttons.
6. **User clicks Continue.** `dispatch(callToolById(...))` runs. The IPC call to the tool implementation fires.
7. Tool returns. `callToolById` reaches the `if (!isStreaming)` guard.

If the user is fast (steps 4 and 6 are very close together), `isStreaming` may still read `true` because the `setInactive` action hasn't propagated to the read in step 7's `getState()`. If the user is slow (multi-second pause), `isStreaming` is unambiguously `false` and the guard trips. The race window is small but reliable across realistic user behaviour. Tester's 23.5-second wait was deep into "always trips" territory.

Auto-approved tools never hit this bug because they dispatch in step 3 — `streamNormalInput` calls `callToolById` *inline* before `setInactive()` ever fires.

### 24.6 File Change Summary (v5.5)

| File | Fix | Lines | Notes |
|---|---|---|---|
| [`gui/src/redux/thunks/callToolById.ts`](../gui/src/redux/thunks/callToolById.ts) | Fix 1 | ~12 | Removed `isSpawnTool` carve-out; the `isSpawnTool` reference at line 162-165 (IPC-timeout branch) is unrelated and unchanged |
| [`gui/src/components/mainInput/Lump/LumpToolbar/PendingToolCallToolbar.tsx`](../gui/src/components/mainInput/Lump/LumpToolbar/PendingToolCallToolbar.tsx) | Fix 2 | ~4 | Swap import + handler dispatch from slice action to thunk |
| [`gui/src/components/mainInput/Lump/LumpToolbar/LumpToolbar.tsx`](../gui/src/components/mainInput/Lump/LumpToolbar/LumpToolbar.tsx) | Fix 2 | ~3 | Same swap for the Cmd/Alt+Backspace keyboard shortcut |

### 24.7 Resolved Limitations

| Limitation (v5.4) | Resolution in v5.5 |
|---|---|
| Silent orchestrator after permission-gated tool (any non-spawn tool) | Fix 1: re-arm for any tool — the carve-out is no longer spawn-only |
| Cancel button silently kills orchestrator | Fix 2: route through `cancelCurrentToolCall` thunk so a tool-result message reaches the model |

### 24.8 Remaining Limitations

1. **Position-N truncation root cause** — unchanged from §23.14. Still deferred.
2. **20-minute spawn-tool IPC ceiling** — unchanged from §23.14.
3. **Diagnostic logs still in production code** — v5.5 keeps `[callToolById] Re-arming stream for ...` (now the only log on the once-noisy `isStreaming` branch). Decide gate strategy alongside the v5.4 audit.
4. **Race-condition root cause not addressed at the architecture level.** Fix 1 makes the bug benign by routing every tool result through the same continuation path, but the underlying architectural issue — `isStreaming` being toggled by `setInactive` between the LLM stream ending and a tool resolving — remains. A future cleanup could decouple "stream finished" from "session is idle" so `isStreaming` is no longer the wrong signal to read at this point.

### 24.9 WSL2 cmd.exe Bug (Bug 2 — Tracked Separately)

The same May 29 tester report flagged a second issue: on WSL2, the orchestrator generates `cmd.exe`-shaped commands (`cd /d`, `2>nul`, `&&`, `%USERNAME%`) instead of bash. **This is not addressed in v5.5.** Root cause: `extensionKind: ["ui","workspace"]` in [`extensions/vscode/package.json`](../extensions/vscode/package.json#L10-L13) hosts the extension on the Windows side even in WSL Remote windows, so `os.platform() === "win32"` and `getPreferredShell()` returns `cmd.exe`. The PLATFORM_INFO string baked into the tool description tells the LLM it's targeting Windows, and `childProcess.spawn(..., { shell: true })` runs commands through `cmd.exe`. Predates the fork (present since v1.0.4).

Upstream commit `c565bdc46` (Jan 19 2026, "fix: use ide.runCommand when Windows host connects to WSL", fixes upstream #9661) provides a partial fix that we have not yet pulled in. Full fix would also require branching `PLATFORM_INFO` / `getPreferredShell()` on `remoteName === "wsl"` so the LLM is told the right environment. Tracked as a separate work item.
