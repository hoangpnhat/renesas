import {
  ConsumptionLimit,
  ModelDescription,
  SubAgentProgress,
  Tool,
  WorkflowPattern,
} from "core";
import {
  DEFAULT_AGENT_SYSTEM_MESSAGE,
  DEFAULT_CHAT_SYSTEM_MESSAGE,
  DEFAULT_PLAN_SYSTEM_MESSAGE,
  RICA_IDENTITY,
} from "core/llm/defaultSystemMessages";
import {
  COLLABORATIVE_SYSTEM_MESSAGE,
  DEFAULT_MULTI_AGENT_SYSTEM_MESSAGE,
  ITERATIVE_SYSTEM_MESSAGE,
  PARALLEL_SYSTEM_MESSAGE,
  SEQUENTIAL_SYSTEM_MESSAGE,
} from "core/llm/multiAgentSystemMessage";

export const NO_TOOL_WARNING =
  "\n\nTHE USER HAS NOT PROVIDED ANY TOOLS, DO NOT ATTEMPT TO USE ANY TOOLS. STOP AND LET THE USER KNOW THAT THERE ARE NO TOOLS AVAILABLE. The user can provide tools by enabling them in the Tool Policies section of the notch (wrench icon)";

const WORKFLOW_PATTERN_MESSAGES: Record<WorkflowPattern, string> = {
  auto: DEFAULT_MULTI_AGENT_SYSTEM_MESSAGE,
  sequential: SEQUENTIAL_SYSTEM_MESSAGE,
  parallel: PARALLEL_SYSTEM_MESSAGE,
  iterative: ITERATIVE_SYSTEM_MESSAGE,
  collaborative: COLLABORATIVE_SYSTEM_MESSAGE,
};

/**
 * Build a summary of completed/running agents for the orchestrator's
 * awareness — keeps it from re-spawning agents whose work it can read.
 */
function buildAgentRegistrySummary(
  subAgentProgress: Record<string, SubAgentProgress>,
): string {
  const agents = Object.values(subAgentProgress);
  if (agents.length === 0) return "";

  const lines = agents.map((a) => {
    const name = a.displayName || a.role || a.description;
    const status = a.status;
    const summary = a.result
      ? a.result.slice(0, 150).replace(/\n/g, " ")
      : a.error
        ? `Error: ${a.error.slice(0, 80)}`
        : "in progress";
    return `  - [${status}] ${name}: ${summary}`;
  });

  return (
    `\n  PREVIOUSLY SPAWNED AGENTS IN THIS SESSION:\n` +
    lines.join("\n") +
    `\n  Do NOT re-spawn agents for tasks already completed above. Use their results directly.`
  );
}

/**
 * Per-turn budget snapshot the orchestrator can read to decide spawn count.
 *
 * Backend's `consumed` is **weighted** — `tokens_in + tokens_out × 5` — so the
 * units the orchestrator reasons about match what its own decisions burn
 * server-side.
 *
 * Backend reports once per ~60s poll AND is subject to a per-user response
 * cache, so it can lag by minutes. We also accept `sessionLocalWeighted`,
 * the exact same-formula sum of every API call this Redux session has made,
 * and surface both numbers — the orchestrator picks `max(backend, local)`
 * as the effective consumed.
 *
 * No hard client-side gate. The orchestrator is trusted to read this and
 * self-throttle. Backend's threshold + cancel-on-block is the real brake.
 */
function buildSessionBudgetContext(
  consumptionLimit?: Partial<ConsumptionLimit> | null,
  modelTitle?: string,
  sessionLocalWeighted?: number,
): string {
  if (
    !consumptionLimit ||
    typeof consumptionLimit.threshold !== "number" ||
    typeof consumptionLimit.consumed !== "number" ||
    consumptionLimit.threshold <= 0
  ) {
    return ""; // no backend limit configured — skip the block
  }

  const threshold = consumptionLimit.threshold;
  const backendConsumed = consumptionLimit.consumed;
  const window = consumptionLimit.rangeHours
    ? `${consumptionLimit.rangeHours}h rolling window`
    : "rolling window";

  const local =
    typeof sessionLocalWeighted === "number" && sessionLocalWeighted > 0
      ? sessionLocalWeighted
      : 0;
  const effective = Math.max(backendConsumed, local);
  const remainingEffective = Math.max(0, threshold - effective);
  const pctEffective = Math.round((effective / threshold) * 100);

  const localLine =
    local > 0
      ? `  This session's local activity: ${local.toLocaleString()} weighted tokens (real-time, exact for this session)\n`
      : "";

  return (
    `\n\n  <session_budget>\n` +
    `  Model: ${modelTitle || "active model"}\n` +
    `  Backend (last poll, may lag minutes): ${backendConsumed.toLocaleString()} / ${threshold.toLocaleString()} weighted tokens (${window})\n` +
    localLine +
    `  Effective consumed (max of backend + local): ${effective.toLocaleString()} / ${threshold.toLocaleString()} (${pctEffective}%)\n` +
    `  Remaining (effective): ${remainingEffective.toLocaleString()}\n` +
    `  Note: weighted = prompt_tokens + completion_tokens × 5 (backend formula).\n` +
    `  Each sub-agent has a 100k weighted cap (≈ 20k completion tokens).\n` +
    `  Guidance: if remaining < 60k, prefer single agent or sequential.\n` +
    `  If remaining < 30k, decline to spawn — synthesize what you have and tell the user.\n` +
    `  </session_budget>`
  );
}

/**
 * Get the base system message for the active mode from the model description.
 *
 * Multi-agent extras (only consulted when `messageMode === "multi-agent"`):
 * - `workflowPattern` selects between auto / sequential / parallel / iterative
 *   / collaborative orchestration patterns.
 * - `subAgentProgress` is rendered as a registry the orchestrator can read so
 *   it doesn't re-spawn agents that already ran in this session.
 * - `priorHistoryFromOtherMode` adds a transition note on the first
 *   multi-agent turn when the conversation already has chat/agent history,
 *   so earlier messages aren't mistaken for orchestrator output.
 * - `consumptionLimit` + `sessionLocalWeighted` produce a `<session_budget>`
 *   block the orchestrator can use to size spawn batches.
 */
export function getBaseSystemMessage(
  messageMode: string,
  model: ModelDescription | null,
  activeTools?: Tool[],
  workflowPattern: WorkflowPattern = "auto",
  subAgentProgress?: Record<string, SubAgentProgress>,
  priorHistoryFromOtherMode: boolean = false,
  consumptionLimit?: Partial<ConsumptionLimit> | null,
  sessionLocalWeighted?: number,
): string {
  let baseMessage: string;

  if (messageMode === "multi-agent") {
    baseMessage = WORKFLOW_PATTERN_MESSAGES[workflowPattern];

    if (subAgentProgress && Object.keys(subAgentProgress).length > 0) {
      baseMessage += buildAgentRegistrySummary(subAgentProgress);
    }

    const budgetBlock = buildSessionBudgetContext(
      consumptionLimit,
      model?.title,
      sessionLocalWeighted,
    );
    if (budgetBlock) {
      baseMessage += budgetBlock;
    }

    if (priorHistoryFromOtherMode) {
      baseMessage +=
        `\n\nMODE TRANSITION NOTICE: Earlier messages in this conversation were generated in chat or agent mode, not multi-agent mode. Treat them as user-provided background context only. From this turn forward you are operating as the multi-agent orchestrator — spawn specialized sub-agents for tasks rather than executing work directly yourself.`;
    }
    return `${RICA_IDENTITY}\n\n${baseMessage}`;
  }

  if (messageMode === "agent") {
    baseMessage = model?.baseAgentSystemMessage ?? DEFAULT_AGENT_SYSTEM_MESSAGE;
  } else if (messageMode === "plan") {
    baseMessage = model?.basePlanSystemMessage ?? DEFAULT_PLAN_SYSTEM_MESSAGE;
  } else {
    baseMessage = model?.baseChatSystemMessage ?? DEFAULT_CHAT_SYSTEM_MESSAGE;
  }

  // Add no-tools warning for agent/plan modes when no tools are available
  if (messageMode !== "chat" && (!activeTools || activeTools.length === 0)) {
    baseMessage += NO_TOOL_WARNING;
  }

  return `${RICA_IDENTITY}\n\n${baseMessage}`;
}
