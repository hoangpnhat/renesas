import { ContextItem } from "../../index.js";
import { SubAgentRunner, CheckpointCallback } from "./SubAgentRunner.js";
import { AgentManager } from "./AgentManager.js";
import { ContextBridge } from "./ContextBridge.js";
import { FaultHandler } from "./FaultHandler.js";

const faultHandler = new FaultHandler();

/**
 * Run an agent with automatic retry on transient failures.
 * Shared by both spawnAgent (singular) and spawnAgents (batch).
 */
export async function runAgentWithRetry(params: {
  subAgentId: string;
  agentRecord: ReturnType<InstanceType<typeof AgentManager>["registerAgent"]>;
  task: string;
  description: string;
  role?: string;
  subAgentTools: any[];
  extras: any;
  /** Per-agent token budget. Derived from the active model's
   * `consumptionLimit.threshold - consumed` at spawn time by the orchestrator.
   * Falls back to 100k when caller doesn't provide one (e.g. retry recursion
   * or models with no backend limit). */
  tokenBudget?: number;
  checkpointInterval?: number;
  onCheckpoint?: CheckpointCallback;
}): Promise<ContextItem[]> {
  const { subAgentId, agentRecord, task, description, role, subAgentTools, extras } = params;
  const manager = AgentManager.getInstance();
  const abortSignal = extras.abortSignal || new AbortController().signal;
  const sendProgress = extras.sendSubAgentProgress || (() => {});

  const runner = new SubAgentRunner(
    extras.ide,
    extras.config,
    extras.fetch,
    abortSignal,
    sendProgress,
    extras.sessionId,
  );

  const result = await runner.run({
    subAgentId,
    parentToolCallId: extras.toolCallId || "unknown",
    task,
    description,
    role: role || undefined,
    tools: subAgentTools,
    model: extras.llm,
    displayName: agentRecord.displayName,
    version: agentRecord.version,
    retryCount: agentRecord.retryCount,
    contextBridgeSessionId: extras.sessionId,
    tokenBudget: params.tokenBudget ?? 100_000,
    checkpointInterval: params.checkpointInterval,
    onCheckpoint: params.onCheckpoint,
  });

  // ── Determine outcome ──
  const resultText = result.map((r) => r.content).join("\n");
  const isCanceled = result.some((r) => r.name === "Sub-Agent Canceled");
  const isError = result.some((r) => r.name === "Sub-Agent Error");

  if (isCanceled) {
    manager.updateStatus(subAgentId, "canceled");
    return result;
  }

  if (isError) {
    manager.updateStatus(subAgentId, "errored", undefined, resultText);

    // ── FaultHandler: classify error and decide on retry ──
    const currentRecord = manager.getAgent(subAgentId) || agentRecord;
    const classification = faultHandler.classifyError(resultText, currentRecord);
    const retryDecision = faultHandler.shouldRetry(classification, currentRecord);

    if (retryDecision.shouldRetry && retryDecision.modifiedTask) {
      console.log(
        `[RunAgent] FaultHandler: retrying "${agentRecord.displayName}" (attempt ${retryDecision.retryNumber}/${FaultHandler.MAX_RETRIES})`,
      );

      // Wait for backoff delay
      if (retryDecision.delay > 0) {
        await new Promise((resolve) => setTimeout(resolve, retryDecision.delay));
      }

      // Prepare retry agent
      const { subAgentId: retryId, record: retryRecord } =
        faultHandler.prepareRetry(currentRecord, retryDecision.modifiedTask);

      // Recursively run with retry — forward the same tokenBudget so the
      // retry doesn't silently re-inflate to the 100k default.
      return await runAgentWithRetry({
        subAgentId: retryId,
        agentRecord: retryRecord,
        task: retryDecision.modifiedTask,
        description,
        role,
        subAgentTools,
        extras,
        tokenBudget: params.tokenBudget,
        checkpointInterval: params.checkpointInterval,
        onCheckpoint: params.onCheckpoint,
      });
    }

    // Retries exhausted or non-retryable: escalate
    if (currentRecord.retryCount > 0) {
      return faultHandler.buildEscalation(currentRecord, resultText);
    }

    return result;
  }

  // ── Success ──
  manager.updateStatus(subAgentId, "completed", resultText);

  // ── ContextBridge: push agent result for other agents to use ──
  if (extras.sessionId) {
    const bridge = ContextBridge.forSession(extras.sessionId);
    bridge.pushAgentResult(
      subAgentId,
      agentRecord.displayName,
      resultText,
    );
  }

  return result;
}
