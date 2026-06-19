import { v4 as uuidv4 } from "uuid";

import { ToolImpl } from ".";
import { AgentManager } from "../multiagent/AgentManager.js";
import { ContextBridge } from "../multiagent/ContextBridge.js";
import { MultiAgentLogger } from "../multiagent/MultiAgentLogger.js";
import { runAgentWithRetry } from "../multiagent/runAgentWithRetry.js";
import { BuiltInToolNames } from "../builtIn.js";

export const spawnAgentImpl: ToolImpl = async (args, extras) => {
  const { task, description, role, context } = args;

  if (!task || !description) {
    throw new Error(
      "SpawnAgent requires both 'task' and 'description' parameters.",
    );
  }

  if (!extras.config) {
    throw new Error("Config not available for sub-agent.");
  }

  const manager = AgentManager.getInstance();

  // ── Deduplication check ──
  const dupCheck = manager.checkDuplicate(task, role, extras.sessionId);
  if (dupCheck.isDuplicate && dupCheck.existingAgent) {
    const existing = dupCheck.existingAgent;

    if (existing.status === "completed" && existing.result) {
      console.log(
        `[SpawnAgent] Duplicate detected — reusing result from "${existing.displayName}" (${existing.subAgentId.slice(0, 8)})`,
      );
      return [
        {
          name: `Sub-Agent Result (cached): ${description}`,
          description: `Reused result from ${existing.displayName}`,
          content: `[Cached from previous agent "${existing.displayName}"]\n\n${existing.result}`,
        },
      ];
    }

    if (existing.status === "running") {
      console.log(
        `[SpawnAgent] Duplicate detected — agent "${existing.displayName}" is already running this task`,
      );
      return [
        {
          name: `Sub-Agent Duplicate: ${description}`,
          description: `Duplicate of running agent ${existing.displayName}`,
          content: `An equivalent agent "${existing.displayName}" is already running this task. Wait for it to complete rather than spawning a duplicate. Agent ID: ${existing.subAgentId}`,
        },
      ];
    }
  }

  // Gather available tools for the sub-agent (excluding orchestrator-only tools)
  const subAgentTools = extras.config.tools.filter(
    (t) =>
      t.function.name !== BuiltInToolNames.SpawnAgent &&
      t.function.name !== BuiltInToolNames.SpawnAgents &&
      t.function.name !== BuiltInToolNames.AnalyzeTask,
  );

  const subAgentId = uuidv4();

  // ── Register with AgentManager for lifecycle tracking ──
  const agentRecord = manager.registerAgent({
    subAgentId,
    task,
    description,
    role: role || undefined,
    sessionId: extras.sessionId,
    abortController: new AbortController(),
    parentAgentId: undefined,
  });

  console.log(
    `[SpawnAgent] Spawning "${agentRecord.displayName}" (id: ${subAgentId.slice(0, 8)})`,
    {
      task: task.slice(0, 150),
      availableTools: subAgentTools.map((t: any) => t.function.name),
      parentToolCallId: extras.toolCallId,
    },
  );

  // ── Log session metadata + decision for single-agent spawns ──
  if (extras.sessionId) {
    const logger = MultiAgentLogger.forSession(extras.sessionId);
    if (extras.llm) {
      logger.setModel(`${extras.llm.providerName}/${extras.llm.model}`);
    }
    logger.setUserPrompt(description);
    logger.setWorkflowPattern("single");
    logger.orchestratorDecision([{ role: role || "Agent", task, description }]);
  }

  // ── ContextBridge: push orchestrator-provided context ──
  if (extras.sessionId && context) {
    const bridge = ContextBridge.forSession(extras.sessionId);
    bridge.pushContext(
      `orchestrator-context:${subAgentId}`,
      context,
      7, // High priority for orchestrator-provided context
    );
  }

  // ── Run agent with fault handling ──
  return await runAgentWithRetry({
    subAgentId,
    agentRecord,
    task,
    description,
    role,
    subAgentTools,
    extras,
  });
};

