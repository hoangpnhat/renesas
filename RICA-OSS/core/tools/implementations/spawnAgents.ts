import { v4 as uuidv4 } from "uuid";

import { ContextItem, ChatMessage } from "../../index.js";
import { ToolImpl } from ".";
import { AgentManager } from "../multiagent/AgentManager.js";
import { ContextBridge } from "../multiagent/ContextBridge.js";
import { FileTracker } from "../multiagent/FileTracker.js";
import { runAgentWithRetry } from "../multiagent/runAgentWithRetry.js";
import { MultiAgentLogger } from "../multiagent/MultiAgentLogger.js";
import { BuiltInToolNames } from "../builtIn.js";
import { CheckpointResult, CheckpointDecision } from "../multiagent/SubAgentRunner.js";

interface AgentSpec {
  task: string;
  description: string;
  role?: string;
  context?: string;
}

const MAX_BATCH_SIZE = 10;
const CHECKPOINT_INTERVAL = 10;
const MAX_PHASES = 3;
const DEFAULT_AGENT_BUDGET = 100_000;
// SESSION_TOKEN_BUDGET enforcement intentionally disabled in v2.0.0.
// The 400k client-side session cap was redundant with backend's threshold
// (Sang's gate fires at the same point) + the cancel-on-block hook in
// ConfigHandler. Trust backend + orchestrator's per-turn budget context
// instead. Kept as a constant for re-enable if backend gating ever lapses.
// const SESSION_TOKEN_BUDGET = 400_000;

export const spawnAgentsImpl: ToolImpl = async (args, extras) => {
  const { agents } = args as { agents: AgentSpec[] };

  // ── Validate input ──
  if (!agents || !Array.isArray(agents) || agents.length === 0) {
    throw new Error(
      "SpawnAgents requires a non-empty 'agents' array.",
    );
  }

  if (agents.length > MAX_BATCH_SIZE) {
    throw new Error(
      `SpawnAgents supports a maximum of ${MAX_BATCH_SIZE} agents per batch. Got ${agents.length}.`,
    );
  }

  for (let i = 0; i < agents.length; i++) {
    if (!agents[i].task || !agents[i].description) {
      throw new Error(
        `Agent at index ${i} is missing required 'task' or 'description' field.`,
      );
    }
  }

  if (!extras.config) {
    throw new Error("Config not available for sub-agents.");
  }

  // ── Resolve the active model's consumptionLimit ──
  // Only used by the pre-spawn lock check below. RicaProfileLoader attaches
  // `consumptionLimit` directly to LLM instances at config load time
  // (see RicaProfileLoader.ts:222). Fallback to scanning `modelsByRole.chat`
  // if the field isn't on the LLM (defensive — shouldn't happen in practice).
  const activeModelTitle: string | undefined = extras.llm?.title;
  let activeLimit: any = (extras.llm as any)?.consumptionLimit;
  if (!activeLimit && activeModelTitle) {
    const chatModels = extras.config?.modelsByRole?.chat ?? [];
    const match = chatModels.find((m: any) => m?.title === activeModelTitle);
    activeLimit = (match as any)?.consumptionLimit;
  }

  // ── Pre-spawn lock check ──
  // If the backend says the active model is currently blocked, refuse to
  // spawn anything. The orchestrator should synthesize what it has and
  // report back, not fan out into a wall of HTTP 429s.
  if (activeLimit?.blocked === true) {
    throw new Error(
      `MODEL RATE-LIMITED — Agents NOT spawned.\n` +
      `Model "${activeModelTitle}" is currently rate-limited ` +
      `(consumed: ${activeLimit.consumed?.toLocaleString?.() ?? "?"}/` +
      `${activeLimit.threshold?.toLocaleString?.() ?? "?"}).\n\n` +
      `Action: synthesize the results you already have and present them ` +
      `to the user. Do NOT attempt to spawn additional agents until the ` +
      `block lifts.`,
    );
  }

  // ── Per-agent budget ──
  // Hardcoded 100k weighted (≈ 20k completion tokens). Backend's threshold
  // + cancel-on-block hook handle session-level safety. The orchestrator
  // gets per-turn budget context (see getBaseSystemMessage) and is trusted
  // to make smart spawn decisions.
  const perAgentBudget = DEFAULT_AGENT_BUDGET;

  // Session-wide budget enforcement intentionally disabled — see comment
  // on SESSION_TOKEN_BUDGET above. Backend threshold + cancel-on-block is
  // the session-level brake now.

  // ── Pre-spawn scope validation: detect file overlap between write-capable agents ──
  const FILE_PATH_RE = /(?:^|\s|["'`(,])([./\\]?(?:[\w@.-]+[/\\])*[\w@.-]+\.\w{1,10})/g;
  const READ_ONLY_ROLES = new Set(["Researcher", "Reviewer", "Analyst", "Planner"]);

  // Known file extensions — anything else is likely a JS identifier (module.exports, console.log)
  const VALID_FILE_EXTENSIONS = new Set([
    "js", "ts", "jsx", "tsx", "mjs", "cjs", "mts", "cts",
    "py", "rb", "rs", "go", "java", "kt", "cs", "cpp", "c", "h", "hpp",
    "json", "yaml", "yml", "toml", "xml", "csv", "sql",
    "md", "mdx", "txt", "rst", "html", "htm", "css", "scss", "sass", "less",
    "vue", "svelte", "astro", "php", "sh", "bash", "zsh", "ps1", "bat", "cmd",
    "dockerfile", "env", "example", "config", "lock", "map",
    "png", "jpg", "jpeg", "gif", "svg", "ico", "webp",
    "wasm", "proto", "graphql", "gql",
  ]);

  // Common JS/TS identifiers that look like file paths but aren't
  const NOT_FILE_PATTERNS = new Set([
    "module.exports", "console.log", "console.error", "console.warn", "console.info",
    "require.main", "require.resolve", "import.meta", "import.meta.url",
    "process.env", "process.exit", "process.cwd", "process.argv",
    "object.keys", "object.values", "object.entries", "object.assign",
    "array.from", "array.isarray", "promise.all", "promise.resolve",
    "json.parse", "json.stringify", "math.round", "math.floor", "math.ceil",
    "path.join", "path.resolve", "path.basename", "path.dirname",
    "fs.existssync", "fs.readfilesync", "fs.writefilesync",
    "string.prototype", "number.prototype", "error.message",
  ]);

  // Root config files that multiple agents commonly touch (additive edits, not conflicting)
  const SHARED_FILES = new Set([
    "package.json", "tsconfig.json", ".env", ".env.example", ".gitignore",
    "readme.md", "changelog.md", ".eslintrc.json", ".prettierrc",
    "jest.config.ts", "jest.config.js", "vite.config.ts", "webpack.config.js",
  ]);

  function extractWriteTargets(taskText: string): string[] {
    const writeMarker = /(?:create|write|edit|modify)\b/i;
    const markerMatch = taskText.match(writeMarker);

    let searchText = taskText;
    if (markerMatch) {
      const fromMarker = taskText.slice(markerMatch.index);
      const sentenceBreak = fromMarker.match(/\.\s+[A-Z]/);
      searchText = sentenceBreak
        ? fromMarker.slice(0, sentenceBreak.index! + 1)
        : fromMarker;
    }

    const paths: string[] = [];
    const regex = new RegExp(FILE_PATH_RE.source, "g");
    let match;
    while ((match = regex.exec(searchText)) !== null) {
      const p = match[1].trim();
      if (p.length > 2 && !p.startsWith("http") && !p.startsWith("//")) {
        const normalized = p.replace(/\\/g, "/").toLowerCase();
        if (NOT_FILE_PATTERNS.has(normalized)) continue;
        const ext = normalized.split(".").pop() || "";
        if (!VALID_FILE_EXTENSIONS.has(ext)) continue;
        paths.push(normalized);
      }
    }
    return paths;
  }

  const scopeMap = new Map<string, number[]>();
  for (let i = 0; i < agents.length; i++) {
    const taskText = `${agents[i].task} ${agents[i].description}`;
    const writePaths = extractWriteTargets(taskText);
    for (const norm of writePaths) {
      const existing = scopeMap.get(norm) || [];
      if (!existing.includes(i)) {
        existing.push(i);
      }
      scopeMap.set(norm, existing);
    }
  }

  const overlaps: Array<{ file: string; agentIndices: number[] }> = [];
  for (const [file, indices] of scopeMap) {
    if (indices.length < 2) continue;
    // Skip root config files that are commonly shared between agents
    const basename = file.split("/").pop() || file;
    if (SHARED_FILES.has(basename)) continue;
    const writeCapable = indices.filter(
      (i) => !agents[i].role || !READ_ONLY_ROLES.has(agents[i].role!),
    );
    if (writeCapable.length >= 2) {
      overlaps.push({ file, agentIndices: indices });
    }
  }

  if (overlaps.length > 0) {
    const overlapDetails = overlaps
      .map(
        (o) =>
          `  - ${o.file}: agents [${o.agentIndices.map((i) => agents[i].description).join(", ")}]`,
      )
      .join("\n");

    console.warn(`[SpawnAgents] SCOPE OVERLAP DETECTED:\n${overlapDetails}`);

    // Log scope overlap to structured logger
    if (extras.sessionId) {
      const logger = MultiAgentLogger.forSession(extras.sessionId);
      logger.scopeOverlapDetected(
        overlaps.map((o) => ({ file: o.file, agents: o.agentIndices.map((i) => agents[i].description) })),
      );
    }

    throw new Error(
      `SCOPE OVERLAP DETECTED — Agents NOT spawned.\n` +
      `The following files are targeted by multiple write-capable agents:\n${overlapDetails}\n\n` +
      `Action required: Restructure your agent assignments so each file is handled by exactly one agent.\n` +
      `1. Merge overlapping agents into a single agent\n` +
      `2. Split file responsibilities so each agent has distinct files\n` +
      `3. Switch to sequential pattern if agents must modify the same files\n\n` +
      `Re-call spawn_agents with non-overlapping scopes.`,
    );
  }

  const manager = AgentManager.getInstance();

  // Filter sub-agent tools (exclude orchestrator-only tools)
  const subAgentTools = extras.config.tools.filter(
    (t: any) =>
      t.function.name !== BuiltInToolNames.SpawnAgent &&
      t.function.name !== BuiltInToolNames.SpawnAgents &&
      t.function.name !== BuiltInToolNames.AnalyzeTask,
  );

  console.log(
    `[SpawnAgents] Batch spawning ${agents.length} agents in parallel`,
    agents.map((a) => ({ desc: a.description.slice(0, 60), role: a.role })),
  );

  // ── Prepare all agents ──
  const agentTasks: Array<{
    spec: AgentSpec;
    subAgentId: string;
    agentRecord: ReturnType<InstanceType<typeof AgentManager>["registerAgent"]>;
    isDuplicate: boolean;
    cachedResult?: ContextItem[];
  }> = [];

  let duplicateCount = 0;

  for (const spec of agents) {
    // Check for duplicates
    const dupCheck = manager.checkDuplicate(spec.task, spec.role, extras.sessionId);
    if (dupCheck.isDuplicate && dupCheck.existingAgent) {
      const existing = dupCheck.existingAgent;

      if (existing.status === "completed" && existing.result) {
        console.log(
          `[SpawnAgents] Duplicate detected — reusing cached result from "${existing.displayName}"`,
        );
        duplicateCount++;
        agentTasks.push({
          spec,
          subAgentId: existing.subAgentId,
          agentRecord: existing as any,
          isDuplicate: true,
          cachedResult: [
            {
              name: `Sub-Agent Result (cached): ${spec.description}`,
              description: `Reused result from ${existing.displayName}`,
              content: `[Cached from previous agent "${existing.displayName}"]\n\n${existing.result}`,
            },
          ],
        });
        continue;
      }

      if (existing.status === "running") {
        console.log(
          `[SpawnAgents] Duplicate detected — "${existing.displayName}" is already running`,
        );
        duplicateCount++;
        agentTasks.push({
          spec,
          subAgentId: existing.subAgentId,
          agentRecord: existing as any,
          isDuplicate: true,
          cachedResult: [
            {
              name: `Sub-Agent Duplicate: ${spec.description}`,
              description: `Duplicate of running agent ${existing.displayName}`,
              content: `An equivalent agent "${existing.displayName}" is already running this task. Agent ID: ${existing.subAgentId}`,
            },
          ],
        });
        continue;
      }
    }

    // Register new agent
    const subAgentId = uuidv4();
    const agentRecord = manager.registerAgent({
      subAgentId,
      task: spec.task,
      description: spec.description,
      role: spec.role || undefined,
      sessionId: extras.sessionId,
      abortController: new AbortController(),
      parentAgentId: undefined,
      modelTitle: activeModelTitle,
    });

    // Push orchestrator-provided context via ContextBridge
    if (extras.sessionId && spec.context) {
      const bridge = ContextBridge.forSession(extras.sessionId);
      bridge.pushContext(
        `orchestrator-context:${subAgentId}`,
        spec.context,
        7,
      );
    }

    agentTasks.push({
      spec,
      subAgentId,
      agentRecord,
      isDuplicate: false,
    });
  }

  // ── Log batch spawn start ──
  const batchStartTime = Date.now();
  if (extras.sessionId) {
    const logger = MultiAgentLogger.forSession(extras.sessionId);
    if (extras.llm) {
      logger.setModel(`${extras.llm.providerName}/${extras.llm.model}`);
    }
    const userTask = agents.map(a => a.description).join("; ");
    logger.setUserPrompt(userTask);
    const roles = agents.map(a => a.role).filter(Boolean);
    if (roles.length > 1) logger.setWorkflowPattern("parallel");
    else logger.setWorkflowPattern("auto");
    logger.batchSpawnStarted(agents.length, agents.map((a) => ({ description: a.description, role: a.role })));
    logger.orchestratorDecision(agents.map(a => ({ role: a.role, task: a.task, description: a.description })));
  }

  // ── Decide execution strategy: phased (2+ active agents) or direct ──
  const activeCount = agentTasks.filter(t => !t.isDuplicate).length;
  const usePhased = activeCount >= 2;

  let results: Array<{ spec: AgentSpec; result: ContextItem[]; success: boolean }>;

  if (usePhased) {
    console.log(`[SpawnAgents] Using phased parallel execution (${activeCount} active agents, checkpoint every ${CHECKPOINT_INTERVAL} steps)`);
    results = await runPhasedParallel(agentTasks, subAgentTools, extras, perAgentBudget);
  } else {
    // Single agent or all duplicates — direct execution, no checkpoint overhead
    const promises = agentTasks.map(async (agentTask) => {
      if (agentTask.isDuplicate && agentTask.cachedResult) {
        return { spec: agentTask.spec, result: agentTask.cachedResult, success: true };
      }
      try {
        const result = await runAgentWithRetry({
          subAgentId: agentTask.subAgentId,
          agentRecord: agentTask.agentRecord,
          task: agentTask.spec.task,
          description: agentTask.spec.description,
          role: agentTask.spec.role,
          subAgentTools,
          extras,
          tokenBudget: perAgentBudget,
        });
        const hasError = result.some((r) => r.name?.startsWith("Sub-Agent Error"));
        return { spec: agentTask.spec, result, success: !hasError };
      } catch (error) {
        console.error(`[SpawnAgents] Agent "${agentTask.spec.description}" failed:`, error);
        return {
          spec: agentTask.spec,
          result: [{
            name: `Sub-Agent Error: ${agentTask.spec.description}`,
            description: "Agent failed with an unhandled error",
            content: `Agent "${agentTask.spec.description}" failed: ${error instanceof Error ? error.message : String(error)}`,
          }] as ContextItem[],
          success: false,
        };
      }
    });
    results = await Promise.all(promises);
  }

  // Log batch completion
  if (extras.sessionId) {
    const logger = MultiAgentLogger.forSession(extras.sessionId);
    logger.batchSpawnCompleted(agents.length, Date.now() - batchStartTime);
    logger.writeToDisk();
  }

  // ── Build combined result ──
  const succeeded = results.filter((r) => r.success).length;
  const failed = results.filter((r) => !r.success).length;

  const summaryHeader: ContextItem = {
    name: "Batch Spawn Summary",
    description: `${agents.length} agents requested`,
    content:
      `## Batch Spawn Results\n` +
      `- **Total requested**: ${agents.length}\n` +
      `- **Duplicates (cached)**: ${duplicateCount}\n` +
      `- **Succeeded**: ${succeeded}\n` +
      `- **Failed**: ${failed}\n`,
  };

  const allContextItems: ContextItem[] = [summaryHeader];
  for (const r of results) {
    allContextItems.push(...r.result);
  }

  // ── Post-flight conflict detection ──
  if (extras.sessionId) {
    const conflicts = FileTracker.forSession(extras.sessionId).detectConflicts();
    if (conflicts.length > 0) {
      const conflictWarning: ContextItem = {
        name: "File Conflict Warning",
        description: `${conflicts.length} file conflict(s) detected`,
        content:
          `## FILE CONFLICTS DETECTED\n\n` +
          `${conflicts.length} file(s) were edited by multiple agents. ` +
          `The last write may have overwritten earlier changes.\n\n` +
          conflicts
            .map(
              (c) =>
                `- **${c.filepath}** — edited by ${c.edits.map((e) => e.agentName).join(", ")}\n` +
                `  Edit order: ${c.edits.map((e) => `${e.agentName} (${new Date(e.timestamp).toISOString().slice(11, 19)})`).join(" -> ")}`,
            )
            .join("\n") +
          `\n\n**Action**: Review these files. The last agent's changes may need to ` +
          `incorporate earlier agents' work. Consider re-running with sequential pattern.`,
      };
      // Insert at position 1 (after summary, before agent results) so orchestrator sees it early
      allContextItems.splice(1, 0, conflictWarning);
      console.log(
        `[SpawnAgents] FILE CONFLICTS: ${conflicts.length} file(s) edited by multiple agents`,
      );
    }
  }

  console.log(
    `[SpawnAgents] Batch complete: ${succeeded} succeeded, ${failed} failed, ${duplicateCount} duplicates`,
  );

  return allContextItems;
};


// ── Phased Parallel Runner ──

type AgentTask = {
  spec: AgentSpec;
  subAgentId: string;
  agentRecord: ReturnType<InstanceType<typeof AgentManager>["registerAgent"]>;
  isDuplicate: boolean;
  cachedResult?: ContextItem[];
};

interface AgentPhaseState {
  task: AgentTask;
  done: boolean;
  result: ContextItem[] | null;
  checkpointResolve: ((decision: CheckpointDecision) => void) | null;
  lastCheckpoint: CheckpointResult | null;
}

async function runPhasedParallel(
  agentTasks: AgentTask[],
  subAgentTools: any[],
  extras: any,
  perAgentBudget: number,
): Promise<Array<{ spec: AgentSpec; result: ContextItem[]; success: boolean }>> {

  // Hard timeout covering the ENTIRE phased run (eval loop + final wait)
  // Must finish before 5 min IPC timeout
  const BATCH_TIMEOUT_MS = 270_000;
  const batchStart = Date.now();

  const agentStates: AgentPhaseState[] = agentTasks.map(task => ({
    task,
    done: task.isDuplicate,
    result: task.cachedResult || null,
    checkpointResolve: null,
    lastCheckpoint: null,
  }));

  // Helper: check if we've exceeded the batch timeout
  const isTimedOut = () => Date.now() - batchStart >= BATCH_TIMEOUT_MS;

  const collectPartialResults = () => {
    const sendProgress = extras.sendSubAgentProgress || (() => {});

    // Finalize any agents still at "running" in the logger + update GUI cards
    if (extras.sessionId) {
      const logger = MultiAgentLogger.forSession(extras.sessionId);
      for (const state of agentStates) {
        if (!state.done) {
          const elapsed = Math.round((Date.now() - batchStart) / 1000);
          const summary = state.lastCheckpoint
            ? `Last checkpoint at step ${state.lastCheckpoint.stepsCompleted}: ${state.lastCheckpoint.interimSummary.slice(0, 300)}`
            : "No checkpoint data available.";
          logger.agentTimedOut(state.task.subAgentId, `(Batch timeout after ${elapsed}s) ${summary}`);

          // Update GUI graph card so it stops showing "running"
          sendProgress({
            subAgentId: state.task.subAgentId,
            parentToolCallId: "",
            task: state.task.spec.task,
            description: state.task.spec.description,
            role: state.task.spec.role,
            status: "completed" as const,
            currentAction: undefined,
            stepsCompleted: state.lastCheckpoint?.stepsCompleted ?? 0,
            result: `(Batch timeout after ${elapsed}s) ${summary}`,
            displayName: state.task.agentRecord?.displayName,
            startedAt: batchStart,
            completedAt: Date.now(),
            tokenUsage: {
              promptTokens: 0,
              completionTokens: 0,
              totalTokens: state.lastCheckpoint?.tokensUsed ?? 0,
              budget: 100_000,
            },
          });
        }
      }
    }

    // Release any agents still paused at checkpoint so their promises can settle
    for (const state of agentStates) {
      if (state.checkpointResolve) {
        state.checkpointResolve({ action: "stop" });
        state.checkpointResolve = null;
      }
    }

    return agentStates.map((state) => ({
      spec: state.task.spec,
      result: state.done && state.result
        ? state.result
        : [{
            name: `Sub-Agent Timeout: ${state.task.spec.description}`,
            description: "Agent did not complete in time",
            content: `Agent "${state.task.spec.description}" timed out after ${Math.round((Date.now() - batchStart) / 1000)}s. ${state.lastCheckpoint ? `Last checkpoint at step ${state.lastCheckpoint.stepsCompleted}: ${state.lastCheckpoint.interimSummary.slice(0, 300)}` : "No checkpoint data available."}`,
          }] as ContextItem[],
      success: state.done || false,
    }));
  };

  // Start all agents — each has a checkpoint callback that pauses via Promise
  const agentPromises = agentStates.map((state) => {
    if (state.done && state.result) {
      return Promise.resolve({
        spec: state.task.spec,
        result: state.result,
        success: true,
      });
    }

    return (async () => {
      try {
        const result = await runAgentWithRetry({
          subAgentId: state.task.subAgentId,
          agentRecord: state.task.agentRecord,
          task: state.task.spec.task,
          description: state.task.spec.description,
          role: state.task.spec.role,
          subAgentTools,
          extras,
          tokenBudget: perAgentBudget,
          checkpointInterval: CHECKPOINT_INTERVAL,
          onCheckpoint: (checkpoint: CheckpointResult) => {
            state.lastCheckpoint = checkpoint;
            if (checkpoint.isDone) {
              state.done = true;
              return Promise.resolve({ action: "continue" as const });
            }
            // Pause: return a Promise that resolves when master decides
            return new Promise<CheckpointDecision>((resolve) => {
              state.checkpointResolve = resolve;
            });
          },
        });
        state.done = true;
        state.result = result;
        const hasError = result.some((r) => r.name?.startsWith("Sub-Agent Error"));
        return { spec: state.task.spec, result, success: !hasError };
      } catch (error) {
        state.done = true;
        console.error(`[SpawnAgents:Phased] Agent "${state.task.spec.description}" failed:`, error);
        return {
          spec: state.task.spec,
          result: [{
            name: `Sub-Agent Error: ${state.task.spec.description}`,
            description: "Agent failed with an unhandled error",
            content: `Agent "${state.task.spec.description}" failed: ${error instanceof Error ? error.message : String(error)}`,
          }] as ContextItem[],
          success: false,
        };
      }
    })();
  });

  // Continuous evaluation loop — evaluate as soon as ANY agent hits a checkpoint
  const MAX_EVAL_ROUNDS = MAX_PHASES * agentStates.length;
  for (let round = 0; round < MAX_EVAL_ROUNDS; round++) {
    // Check batch timeout before waiting for next checkpoint
    if (isTimedOut()) {
      console.warn(`[SpawnAgents:Phased] Batch timeout hit during eval loop at round ${round + 1}`);
      return collectPartialResults();
    }

    // Wait until at least one agent hits checkpoint (or all done)
    // Cap wait time to remaining batch budget
    const remainingMs = BATCH_TIMEOUT_MS - (Date.now() - batchStart);
    await waitForAnyCheckpoint(agentStates, Math.max(remainingMs, 1000));

    if (isTimedOut()) {
      console.warn(`[SpawnAgents:Phased] Batch timeout hit after waitForAnyCheckpoint at round ${round + 1}`);
      return collectPartialResults();
    }

    // Mid-batch session-level budget check intentionally disabled.
    // Backend's threshold + cancel-on-block hook handles this now.

    const pendingAgents = agentStates.filter(s => s.checkpointResolve && !s.done);
    if (pendingAgents.length === 0) {
      if (agentStates.every(s => s.done)) break;
      continue;
    }

    console.log(`[SpawnAgents:Phased] Eval round ${round + 1}: ${pendingAgents.length} agent(s) at checkpoint (${Math.round((Date.now() - batchStart) / 1000)}s elapsed)`);

    // Master evaluation: ONE lightweight LLM call
    const evalStart = Date.now();
    const decisions = await evaluateCheckpoints(pendingAgents, extras);
    const evalDuration = Date.now() - evalStart;

    if (extras.sessionId) {
      const logger = MultiAgentLogger.forSession(extras.sessionId);
      const decisionSummary = pendingAgents.map((s) => {
        const d = decisions.get(s) || { action: "continue" };
        return `${s.task.spec.description}: ${d.action}`;
      }).join("; ");
      logger.log("info", "master_checkpoint_eval", `Round ${round + 1}: ${decisionSummary}`, undefined, {
        phase: round + 1,
        agentCount: pendingAgents.length,
        decisions: decisionSummary,
        durationMs: evalDuration,
      });
    }

    // Resume each agent with the master's decision.
    // Don't set state.done here — let the agent's promise resolve naturally
    // (after forceSummary if applicable), so Promise.all waits for the real result.
    for (const state of pendingAgents) {
      const decision = decisions.get(state) || { action: "continue" };
      if (decision.action === "stop") {
        console.log(`[SpawnAgents:Phased] Stopping "${state.task.spec.description}" — master decision`);
      }
      state.checkpointResolve!(decision);
      state.checkpointResolve = null;
    }

    if (agentStates.every(s => s.done)) break;
  }

  // Safety: release any agents still waiting at checkpoint
  const remaining = agentStates.filter(s => s.checkpointResolve);
  if (remaining.length > 0) {
    console.log(`[SpawnAgents:Phased] ${remaining.length} agents still at checkpoint after eval loop — releasing`);
    for (const state of remaining) {
      state.checkpointResolve!({ action: "continue" });
      state.checkpointResolve = null;
    }
  }

  // Final wait: use remaining time from batch budget
  const elapsed = Date.now() - batchStart;
  const remainingTimeout = Math.max(BATCH_TIMEOUT_MS - elapsed, 5_000); // at least 5s grace
  console.log(`[SpawnAgents:Phased] Eval loop done (${Math.round(elapsed / 1000)}s). Waiting up to ${Math.round(remainingTimeout / 1000)}s for agents to finish.`);

  const timeoutPromise = new Promise<never>((_, reject) =>
    setTimeout(() => reject(new Error("Batch execution timed out")), remainingTimeout),
  );

  try {
    return await Promise.race([Promise.all(agentPromises), timeoutPromise]);
  } catch {
    console.warn(`[SpawnAgents:Phased] Batch timed out after ${Math.round((Date.now() - batchStart) / 1000)}s total — returning partial results`);
    return collectPartialResults();
  }
}

async function waitForAnyCheckpoint(states: AgentPhaseState[], maxWaitMs: number = 300_000): Promise<void> {
  const POLL_MS = 200;
  const startWait = Date.now();

  while (Date.now() - startWait < maxWaitMs) {
    const anyAtCheckpoint = states.some(s => s.checkpointResolve !== null && !s.done);
    if (anyAtCheckpoint) return;
    if (states.every(s => s.done)) return;
    await new Promise(r => setTimeout(r, POLL_MS));
  }
}

async function evaluateCheckpoints(
  pendingAgents: AgentPhaseState[],
  extras: any,
): Promise<Map<AgentPhaseState, CheckpointDecision>> {
  const decisions = new Map<AgentPhaseState, CheckpointDecision>();

  // If no LLM available, continue all
  if (!extras.llm) {
    for (const s of pendingAgents) decisions.set(s, { action: "continue" });
    return decisions;
  }

  // Fast-path: skip LLM eval if all agents are under 30% budget (early = almost always "continue")
  const allEarly = pendingAgents.every(s => {
    const cp = s.lastCheckpoint!;
    return (cp.tokensUsed / DEFAULT_AGENT_BUDGET) < 0.3;
  });
  if (allEarly) {
    const detail = pendingAgents.map(s => {
      const cp = s.lastCheckpoint!;
      const budgetPct = Math.round((cp.tokensUsed / DEFAULT_AGENT_BUDGET) * 100);
      return `${s.task.spec.description} (${cp.stepsCompleted} steps, ${budgetPct}% budget)`;
    }).join("; ");
    console.log(`[SpawnAgents:Phased] Fast-path: all ${pendingAgents.length} agents under 30% budget — auto-continue: ${detail}`);
    if (extras.sessionId) {
      const logger = MultiAgentLogger.forSession(extras.sessionId);
      logger.orchestratorCheckpointEval(
        "(fast-path: all agents under 30% budget)",
        `Auto-continue ${pendingAgents.length} agents: ${detail}`,
        0,
        0,
      );
    }
    for (const s of pendingAgents) decisions.set(s, { action: "continue" });
    return decisions;
  }

  const checkpointReport = pendingAgents.map((s, i) => {
    const cp = s.lastCheckpoint!;
    const budgetPct = Math.round((cp.tokensUsed / DEFAULT_AGENT_BUDGET) * 100);
    return `Agent ${i + 1} (${s.task.spec.description}, ${s.task.spec.role || "Agent"}):\n` +
      `  Steps: ${cp.stepsCompleted}, Tokens: ~${Math.round(cp.tokensUsed / 1000)}K / ${Math.round(DEFAULT_AGENT_BUDGET / 1000)}K budget (${budgetPct}%)\n` +
      `  Progress: ${cp.interimSummary.slice(0, 400)}`;
  }).join("\n\n");

  const evaluationMessages: ChatMessage[] = [
    {
      role: "system",
      content:
        `You are a multi-agent coordinator evaluating checkpoint reports from parallel agents.\n` +
        `For each agent, decide: "continue" (on track), "redirect" (provide new focus), or "stop" (task done or redundant).\n` +
        `Stop agents that: overlap with other agents' work, have completed their subtask, OR are using too many tokens (>50% budget) without proportional progress.\n` +
        `Redirect agents that are off-track or wasting steps on irrelevant files.\n` +
        `Each agent has a 100K token budget. An agent above 50% budget should be near completion or stopped.\n` +
        `Respond with JSON only: [{"agent": 1, "action": "continue|redirect|stop", "reason": "...", "newContext": "..."}]`,
    },
    {
      role: "user",
      content:
        `Agents are working on these tasks:\n${pendingAgents.map((s, i) => `${i + 1}. ${s.task.spec.task.slice(0, 200)}`).join("\n")}\n\n` +
        `Checkpoint reports:\n${checkpointReport}\n\n` +
        `Evaluate each agent. Stop agents that overlap or are done. Redirect if off-track.`,
    },
  ];

  const evalStart = Date.now();
  const evalPrompt = evaluationMessages.map(m => `[${m.role}] ${m.content}`).join("\n\n");

  try {
    const evalAbort = new AbortController();
    const EVAL_STALE_TIMEOUT_MS = 60_000;
    let evalStaleTimer: ReturnType<typeof setTimeout> | null = null;
    const resetEvalTimer = () => {
      if (evalStaleTimer) clearTimeout(evalStaleTimer);
      evalStaleTimer = setTimeout(() => {
        console.warn(`[SpawnAgents:Phased] Checkpoint eval LLM stale for ${EVAL_STALE_TIMEOUT_MS / 1000}s — aborting`);
        evalAbort.abort();
      }, EVAL_STALE_TIMEOUT_MS);
    };

    const gen = extras.llm.streamChat(evaluationMessages, evalAbort.signal, {});
    let fullContent = "";
    resetEvalTimer();
    let result = await gen.next();
    while (!result.done) {
      resetEvalTimer();
      if (typeof result.value.content === "string") {
        fullContent += result.value.content;
      }
      result = await gen.next();
    }
    if (evalStaleTimer) clearTimeout(evalStaleTimer);
    const promptLog = result.value as any;
    const evalTokensUsed = (promptLog?.promptTokens ?? 0) + (promptLog?.completionTokens ?? 0);

    const evalDurationMs = Date.now() - evalStart;

    // Log the full orchestrator checkpoint conversation
    if (extras.sessionId) {
      const logger = MultiAgentLogger.forSession(extras.sessionId);
      logger.orchestratorCheckpointEval(evalPrompt, fullContent, evalDurationMs, evalTokensUsed);
    }

    // Extract JSON from response (may have markdown wrapping)
    const jsonMatch = fullContent.match(/\[[\s\S]*\]/);
    if (jsonMatch) {
      const parsed = JSON.parse(jsonMatch[0]);
      for (const d of parsed) {
        const state = pendingAgents[d.agent - 1];
        if (state) {
          decisions.set(state, {
            action: d.action,
            newContext: d.newContext,
          });
        }
      }
    }
  } catch (e) {
    console.warn(`[SpawnAgents:Phased] Master evaluation failed, continuing all agents:`, e);
    if (extras.sessionId) {
      const logger = MultiAgentLogger.forSession(extras.sessionId);
      logger.orchestratorCheckpointEval(evalPrompt, `ERROR: ${e instanceof Error ? e.message : String(e)}`, Date.now() - evalStart);
    }
  }

  // Default: continue any agent without a decision
  for (const s of pendingAgents) {
    if (!decisions.has(s)) decisions.set(s, { action: "continue" });
  }

  return decisions;
}
