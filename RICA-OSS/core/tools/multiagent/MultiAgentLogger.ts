import * as fs from "fs";
import * as path from "path";
import * as os from "os";
import * as zlib from "zlib";

export interface LogEvent {
  timestamp: number;
  elapsed: number;
  type: string;
  level: "info" | "warn" | "error" | "debug";
  phase: string;
  agent?: string;
  message: string;
  data?: Record<string, any>;
}

export interface OrchestratorMessage {
  step: number;
  timestamp: number;
  type: "system_prompt" | "user_message" | "llm_response" | "tool_result" | "decision" | "checkpoint_eval" | "synthesis" | "error" | "abort";
  content?: string;
  // Metadata fields (present on relevant types)
  toolName?: string;
  toolCallId?: string;
  agentSpecs?: Array<{ role?: string; task: string; description: string }>;
  durationMs?: number;
  tokensUsed?: number;
  error?: string;
  modelTitle?: string;
}

export interface SessionLog {
  sessionId: string;
  startedAt: number;
  completedAt?: number;
  userPrompt?: string;
  model?: string;
  workflowPattern?: string;
  agents: AgentLog[];
  orchestratorConversation: OrchestratorMessage[];
  summary?: SessionSummary;
}

export interface AgentLog {
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

export interface ToolCallLog {
  iteration: number;
  step: number;
  toolName: string;
  args: string;
  argSummary?: string;
  startedAt: number;
  durationMs: number;
  status: "completed" | "errored";
  resultFull: string;
  error?: string;
  truncated?: boolean;
  originalLength?: number;
}

export interface LLMCallLog {
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

export interface MessageSnapshot {
  iteration: number;
  timestamp: number;
  messageCount: number;
  roles: string[];
  estimatedTokens: number;
  messages: Array<{
    role: string;
    contentPreview: string;
    contentLength: number;
    hasToolCalls: boolean;
    toolCallId?: string;
  }>;
}

export interface APIErrorLog {
  agentId: string;
  iteration: number;
  timestamp: number;
  errorType: "rate_limit" | "overloaded" | "timeout" | "network" | "auth" | "server_error" | "unknown";
  statusCode?: number;
  errorMessage: string;
  durationMs: number;
}

export interface PromptAssemblyLog {
  systemMessageLength: number;
  userMessageLength: number;
  contextBridgeItems: number;
  contextBridgeChars: number;
  totalEstimatedTokens: number;
  systemMessagePreview: string;
  userMessagePreview: string;
}


export interface SessionSummary {
  totalAgents: number;
  completedAgents: number;
  erroredAgents: number;
  canceledAgents: number;
  totalTokens: number;
  orchestratorTokens: number;
  totalDurationMs: number;
  scopeOverlapsDetected: number;
  overlappingFiles?: Array<{ file: string; agents: string[] }>;
  budgetExhaustions: number;
  apiErrors: number;
  toolTruncations: number;
}

const LOG_DIR_NAME = ".rica-multiagent-logs";

function getLogDir(): string {
  const homeDir = os.homedir();
  const logDir = path.join(homeDir, LOG_DIR_NAME);
  if (!fs.existsSync(logDir)) {
    fs.mkdirSync(logDir, { recursive: true });
  }
  return logDir;
}

export class MultiAgentLogger {
  private static instances = new Map<string, MultiAgentLogger>();

  private sessionLog: SessionLog;
  private agentLogs = new Map<string, AgentLog>();
  private startTime: number;
  private orchestratorStep = 0;
  private systemPromptLogged = false;
  private scopeOverlapCount = 0;
  private scopeOverlapFiles: Array<{ file: string; agents: string[] }> = [];
  private lastUserMessageContent = "";

  private constructor(sessionId: string) {
    this.startTime = Date.now();
    this.sessionLog = {
      sessionId,
      startedAt: this.startTime,
      agents: [],
      orchestratorConversation: [],
    };
  }

  static forSession(sessionId: string): MultiAgentLogger {
    if (!this.instances.has(sessionId)) {
      this.instances.set(sessionId, new MultiAgentLogger(sessionId));
    }
    return this.instances.get(sessionId)!;
  }

  static cleanup(sessionId: string): void {
    this.instances.delete(sessionId);
  }

  private nextStep(): number {
    return ++this.orchestratorStep;
  }

  setUserPrompt(prompt: string): void {
    this.sessionLog.userPrompt = prompt.slice(0, 500);
  }

  setModel(model: string): void {
    this.sessionLog.model = model;
  }

  setWorkflowPattern(pattern: string): void {
    this.sessionLog.workflowPattern = pattern;
  }

  getSessionTokensUsed(): number {
    let total = 0;
    for (const agent of this.agentLogs.values()) {
      total += agent.tokenUsage.total;
    }
    return total;
  }

  log(level: LogEvent["level"], phase: string, message: string, agentId?: string, data?: Record<string, any>): void {
    const prefix = agentId ? `[MultiAgent:${agentId.slice(0, 8)}]` : "[MultiAgent]";
    const logFn = level === "error" ? console.error : level === "warn" ? console.warn : console.log;
    logFn(`${prefix} [${phase}] ${message}`, data ? JSON.stringify(data).slice(0, 200) : "");
    this.flushLive();
  }

  // ── Agent lifecycle ──

  agentSystemPrompt(agentId: string, systemMessage: string): void {
    const agent = this.agentLogs.get(agentId);
    if (agent) {
      (agent as any).systemPrompt = systemMessage;
    }
    this.log("debug", "agent-prompt", `System prompt set (${systemMessage.length} chars)`, agentId);
  }

  agentEnrichedTask(agentId: string, originalTask: string, enrichedTask: string, contextBridgeInjected: boolean): void {
    const agent = this.agentLogs.get(agentId);
    if (agent) {
      (agent as any).originalTask = originalTask;
      (agent as any).enrichedTask = enrichedTask;
      (agent as any).contextBridgeInjected = contextBridgeInjected;
    }
    this.log("debug", "agent-task", `Task enriched: original ${originalTask.length} chars -> enriched ${enrichedTask.length} chars (bridge: ${contextBridgeInjected})`, agentId);
  }

  agentToolsAvailable(agentId: string, tools: string[]): void {
    this.log("debug", "agent-tools", `Tools available: ${tools.join(", ")}`, agentId);
  }

  agentStarted(agentId: string, displayName: string, role: string | undefined, task: string, budget: number): void {
    const agentLog: AgentLog = {
      agentId,
      displayName,
      role,
      task,
      status: "running",
      startedAt: Date.now(),
      iterations: 0,
      stepsCompleted: 0,
      tokenUsage: { prompt: 0, completion: 0, total: 0, cacheRead: 0, budget, budgetExhausted: false },
      toolCalls: [],
      llmCalls: [],
      messageSnapshots: [],
      apiErrors: [],
      pruningEvents: 0,
    };
    this.agentLogs.set(agentId, agentLog);
    this.log("info", "agent-start", `Agent "${displayName}" started — role: ${role || "default"}, budget: ${budget}`, agentId, {
      task: task.slice(0, 200),
    });
  }

  agentIteration(agentId: string, iteration: number, promptTokens: number, completionTokens: number, cacheReadTokens: number): void {
    const agent = this.agentLogs.get(agentId);
    if (agent) {
      agent.iterations = iteration;
      agent.tokenUsage.prompt = promptTokens;
      agent.tokenUsage.completion = completionTokens;
      agent.tokenUsage.total = promptTokens + completionTokens;
      agent.tokenUsage.cacheRead = cacheReadTokens;
    }
    this.log("debug", "agent-iteration", `Iteration ${iteration} — ${promptTokens + completionTokens} tokens used`, agentId);
  }

  agentToolCall(agentId: string, iteration: number, step: number, toolName: string, args: string, argSummary: string | undefined, durationMs: number, status: "completed" | "errored", resultFull: string, error?: string, truncated?: boolean, originalLength?: number): void {
    const agent = this.agentLogs.get(agentId);
    if (agent) {
      agent.stepsCompleted = step;
      agent.toolCalls.push({
        iteration,
        step,
        toolName,
        args,
        argSummary,
        startedAt: Date.now() - durationMs,
        durationMs,
        status,
        resultFull,
        error,
        ...(truncated && { truncated, originalLength }),
      });
    }
    const msg = status === "errored"
      ? `Tool ${toolName} ERRORED after ${durationMs}ms: ${error?.slice(0, 100)}`
      : `Tool ${toolName} completed in ${durationMs}ms — result: ${resultFull.length} chars`;
    this.log(status === "errored" ? "warn" : "debug", "agent-tool", msg, agentId, { argSummary, args: args.slice(0, 300) });
  }

  agentLLMCall(agentId: string, iteration: number, durationMs: number, messagesCount: number, systemMessageLength: number, promptTokens: number, completionTokens: number, cacheReadTokens: number, cacheCreationTokens: number, isActualUsage: boolean, responseText: string, toolCallsReturned: Array<{ name: string; argsPreview: string }>): void {
    const agent = this.agentLogs.get(agentId);
    if (agent) {
      agent.llmCalls.push({
        iteration,
        timestamp: Date.now(),
        durationMs,
        messagesCount,
        systemMessageLength,
        promptTokens,
        completionTokens,
        cacheReadTokens,
        cacheCreationTokens,
        isActualUsage,
        responseText,
        toolCallsReturned,
      });
    }
    this.log("debug", "agent-llm", `LLM call #${iteration}: ${promptTokens + completionTokens} tokens, ${toolCallsReturned.length} tool calls, ${durationMs}ms`, agentId);
  }

  agentMessageSnapshot(agentId: string, iteration: number, messages: Array<{ role: string; content: any; toolCalls?: any; toolCallId?: string }>): void {
    const agent = this.agentLogs.get(agentId);
    if (agent) {
      const snapshot: MessageSnapshot = {
        iteration,
        timestamp: Date.now(),
        messageCount: messages.length,
        roles: messages.map((m) => m.role),
        estimatedTokens: messages.reduce((sum, m) => {
          const content = typeof m.content === "string" ? m.content : JSON.stringify(m.content);
          return sum + Math.ceil(content.length / 4);
        }, 0),
        messages: messages.map((m) => {
          const content = typeof m.content === "string" ? m.content : JSON.stringify(m.content);
          return {
            role: m.role,
            contentPreview: content.slice(0, 300),
            contentLength: content.length,
            hasToolCalls: !!m.toolCalls && m.toolCalls.length > 0,
            ...(m.toolCallId && { toolCallId: m.toolCallId }),
          };
        }),
      };
      agent.messageSnapshots.push(snapshot);
    }
    this.flushLive();
  }

  agentBudgetExhausted(agentId: string, tokensUsed: number, budget: number, iteration: number): void {
    const agent = this.agentLogs.get(agentId);
    if (agent) {
      agent.tokenUsage.budgetExhausted = true;
    }
    this.log("warn", "agent-budget", `Token budget exhausted: ${tokensUsed}/${budget} after ${iteration} iterations`, agentId);
  }

  agentMessagesPruned(agentId: string, before: number, after: number): void {
    const agent = this.agentLogs.get(agentId);
    if (agent) {
      agent.pruningEvents++;
    }
    this.log("info", "agent-prune", `Messages pruned: ${before} -> ${after}`, agentId);
  }

  agentCompleted(agentId: string, resultFull: string): void {
    const agent = this.agentLogs.get(agentId);
    if (agent) {
      agent.status = "completed";
      agent.completedAt = Date.now();
      agent.durationMs = agent.completedAt - agent.startedAt;
      agent.resultFull = resultFull;
    }
    this.log("info", "agent-complete", `Agent completed in ${agent?.durationMs}ms — ${agent?.tokenUsage.total} tokens, ${agent?.stepsCompleted} steps, result: ${resultFull.length} chars`, agentId);
  }

  agentErrored(agentId: string, error: string): void {
    const agent = this.agentLogs.get(agentId);
    if (agent) {
      agent.status = "errored";
      agent.completedAt = Date.now();
      agent.durationMs = agent.completedAt - agent.startedAt;
      agent.error = error.slice(0, 500);
    }
    this.log("error", "agent-error", `Agent errored: ${error.slice(0, 200)}`, agentId);
  }

  agentCanceled(agentId: string): void {
    const agent = this.agentLogs.get(agentId);
    if (agent) {
      agent.status = "canceled";
      agent.completedAt = Date.now();
      agent.durationMs = agent.completedAt - agent.startedAt;
    }
    this.log("info", "agent-cancel", "Agent canceled by user", agentId);
  }

  agentTimedOut(agentId: string, resultSummary: string): void {
    const agent = this.agentLogs.get(agentId);
    if (agent && agent.status === "running") {
      agent.status = "completed";
      agent.completedAt = Date.now();
      agent.durationMs = agent.completedAt - agent.startedAt;
      agent.resultFull = resultSummary;
    }
    this.log("warn", "agent-timeout", `Agent timed out: ${resultSummary.slice(0, 200)}`, agentId);
  }

  // ── Orchestration events ──

  scopeOverlapDetected(overlaps: Array<{ file: string; agents: string[] }>): void {
    this.scopeOverlapCount++;
    this.scopeOverlapFiles.push(...overlaps);
    this.log("warn", "scope-validation", `Scope overlap detected in ${overlaps.length} file(s)`, undefined, { overlaps });
  }

  agentRetry(agentId: string, attempt: number, maxRetries: number, reason: string): void {
    this.log("warn", "agent-retry", `Retry ${attempt}/${maxRetries}: ${reason}`, agentId);
  }

  batchSpawnStarted(agentCount: number, agents: Array<{ description: string; role?: string }>): void {
    this.log("info", "batch-spawn", `Spawning ${agentCount} agents in parallel`, undefined, { agents });
  }

  batchSpawnCompleted(agentCount: number, durationMs: number): void {
    this.log("info", "batch-complete", `All ${agentCount} agents completed in ${durationMs}ms`);
  }

  // ── API error logging ──

  agentAPIError(agentId: string, iteration: number, errorType: APIErrorLog["errorType"], errorMessage: string, durationMs: number, statusCode?: number): void {
    const agent = this.agentLogs.get(agentId);
    if (agent) {
      agent.apiErrors.push({
        agentId, iteration, timestamp: Date.now(),
        errorType, statusCode, errorMessage, durationMs,
      });
    }
    this.log("error", "agent-api-error", `API ${errorType}${statusCode ? ` (${statusCode})` : ""}: ${errorMessage.slice(0, 200)}`, agentId, { durationMs });
  }

  // ── Prompt assembly logging ──

  agentPromptAssembled(agentId: string, systemMessageLength: number, userMessageLength: number, contextBridgeItems: number, contextBridgeChars: number, totalEstimatedTokens: number, systemMessagePreview: string, userMessagePreview: string): void {
    const agent = this.agentLogs.get(agentId);
    if (agent) {
      agent.promptAssembly = {
        systemMessageLength, userMessageLength,
        contextBridgeItems, contextBridgeChars,
        totalEstimatedTokens,
        systemMessagePreview: systemMessagePreview.slice(0, 500),
        userMessagePreview: userMessagePreview.slice(0, 500),
      };
    }
    this.log("debug", "agent-prompt", `Prompt assembled: system ${systemMessageLength} chars, user ${userMessageLength} chars, bridge ${contextBridgeItems} items (${contextBridgeChars} chars), ~${totalEstimatedTokens} tokens`, agentId);
  }

  // ── Orchestrator / IPC events ──

  orchestratorDecision(agentSpecs: Array<{ role?: string; task: string; description: string }>): void {
    const summary = agentSpecs.map((s, i) =>
      `Agent ${i + 1}: [${s.role || "Agent"}] ${s.description}\n  Task: ${s.task.slice(0, 300)}`,
    ).join("\n");
    this.sessionLog.orchestratorConversation.push({
      step: this.nextStep(),
      timestamp: Date.now(),
      type: "decision",
      content: summary,
      agentSpecs,
    });
    this.flushLive();
  }

  orchestratorCheckpointEval(prompt: string, response: string, durationMs: number, tokensUsed?: number): void {
    this.sessionLog.orchestratorConversation.push({
      step: this.nextStep(),
      timestamp: Date.now(),
      type: "checkpoint_eval",
      content: response.slice(0, 5_000),
      durationMs,
      tokensUsed,
    });
    this.flushLive();
  }

  orchestratorSynthesis(response: string): void {
    this.sessionLog.orchestratorConversation.push({
      step: this.nextStep(),
      timestamp: Date.now(),
      type: "synthesis",
      content: response.slice(0, 5_000),
    });
    this.flushLive();
  }

  orchestratorMainTurn(prompt: string, completion: string, modelTitle?: string, promptTokens?: number, completionTokens?: number, inputSummary?: string, durationMs?: number): void {
    // Once: log the system prompt so debuggers can see what instructions the orchestrator had
    if (!this.systemPromptLogged) {
      this.systemPromptLogged = true;
      const sysEnd = prompt.indexOf("\n\n<");
      const systemPrompt = sysEnd > 0 ? prompt.slice(0, sysEnd) : prompt.slice(0, 20_000);
      this.sessionLog.orchestratorConversation.push({
        step: this.nextStep(),
        timestamp: Date.now(),
        type: "system_prompt",
        content: systemPrompt.slice(0, 20_000),
        modelTitle,
      });
    }

    // Per-turn: extract and log the actual injected <session_budget> block.
    // The static system prompt mentions "<session_budget>" by name as a
    // teaching reference, so we match the INJECTED form specifically: it
    // starts on its own line with leading indentation, immediately followed
    // by "Model:" on the next line. That distinguishes it from the literal
    // tag string in the static prompt body.
    const budgetMatch = prompt.match(/\n\s+<session_budget>\n\s+Model:[\s\S]*?<\/session_budget>/);
    if (budgetMatch) {
      this.sessionLog.orchestratorConversation.push({
        step: this.nextStep(),
        timestamp: Date.now(),
        type: "system_prompt",
        content: `[INJECTED BUDGET BLOCK]${budgetMatch[0]}`,
        modelTitle,
      });
    }

    if (inputSummary?.trim() && inputSummary !== this.lastUserMessageContent) {
      this.lastUserMessageContent = inputSummary;
      this.sessionLog.orchestratorConversation.push({
        step: this.nextStep(),
        timestamp: Date.now(),
        type: "user_message",
        content: inputSummary.slice(0, 5_000),
      });
    }

    // Log what the LLM responded with (text + tool calls)
    this.sessionLog.orchestratorConversation.push({
      step: this.nextStep(),
      timestamp: Date.now(),
      type: "llm_response",
      content: completion.slice(0, 10_000),
      modelTitle,
      tokensUsed: (promptTokens || 0) + (completionTokens || 0),
      durationMs,
    });

    this.flushLive();
  }

  orchestratorToolResult(toolName: string, toolCallId: string, resultPreview: string, durationMs?: number): void {
    this.sessionLog.orchestratorConversation.push({
      step: this.nextStep(),
      timestamp: Date.now(),
      type: "tool_result",
      content: resultPreview.slice(0, 5_000),
      toolName,
      toolCallId,
      durationMs,
    });
    this.flushLive();
  }

  orchestratorError(errorType: string, message: string, durationMs?: number): void {
    this.sessionLog.orchestratorConversation.push({
      step: this.nextStep(),
      timestamp: Date.now(),
      type: "error",
      content: `[${errorType}] ${message}`,
      error: message,
      durationMs,
    });
    this.flushLive();
  }

  // ── Finalize and write ──

  finalize(): SessionLog {
    this.sessionLog.completedAt = Date.now();

    // Safety net: auto-complete any agents still marked "running"
    // Only set auto-finalize text if the agent has no real result yet
    for (const agent of this.agentLogs.values()) {
      if (agent.status === "running") {
        agent.completedAt = Date.now();
        agent.durationMs = agent.completedAt - agent.startedAt;
        if (!agent.resultFull) {
          agent.status = "completed";
          agent.resultFull = `(Auto-finalized — agent was still running at session end, ${agent.stepsCompleted} steps completed)`;
        }
        // If agent has a real result (from agentCompleted racing with finalize),
        // keep the result but still mark completed
        else {
          agent.status = "completed";
        }
      }
    }

    this.sessionLog.agents = Array.from(this.agentLogs.values());

    const agents = this.sessionLog.agents;
    const orchestratorTokens = this.sessionLog.orchestratorConversation
      .filter(s => s.tokensUsed != null)
      .reduce((sum, s) => sum + (s.tokensUsed ?? 0), 0);

    this.sessionLog.summary = {
      totalAgents: agents.length,
      completedAgents: agents.filter((a) => a.status === "completed").length,
      erroredAgents: agents.filter((a) => a.status === "errored").length,
      canceledAgents: agents.filter((a) => a.status === "canceled").length,
      totalTokens: agents.reduce((sum, a) => sum + a.tokenUsage.total, 0),
      orchestratorTokens,
      totalDurationMs: this.sessionLog.completedAt - this.sessionLog.startedAt,
      scopeOverlapsDetected: this.scopeOverlapCount,
      ...(this.scopeOverlapFiles.length > 0 && { overlappingFiles: this.scopeOverlapFiles }),
      budgetExhaustions: agents.filter((a) => a.tokenUsage.budgetExhausted).length,
      apiErrors: agents.reduce((sum, a) => sum + a.apiErrors.length, 0),
      toolTruncations: agents.reduce((sum, a) => sum + a.toolCalls.filter(tc => tc.truncated).length, 0),
    };

    return this.sessionLog;
  }

  private liveLogPath: string | null = null;
  private finalLogPath: string | null = null;

  flushLive(): void {
    try {
      this.sessionLog.agents = Array.from(this.agentLogs.values());
      const json = JSON.stringify(this.sessionLog, null, 2);

      // If finalized, update the final file directly (post-finalization IPC messages)
      if (this.finalLogPath) {
        fs.writeFileSync(this.finalLogPath, json, "utf8");
        return;
      }

      if (!this.liveLogPath) {
        const logDir = getLogDir();
        const timestamp = new Date().toISOString().replace(/[:.]/g, "-");
        const filename = `live_${timestamp}_${this.sessionLog.sessionId.slice(0, 8)}.json`;
        this.liveLogPath = path.join(logDir, filename);
      }
      fs.writeFileSync(this.liveLogPath, json, "utf8");
    } catch { /* best-effort live flush */ }
  }

  writeToDisk(): string | null {
    try {
      const log = this.finalize();
      const logDir = getLogDir();

      // Use a stable filename per session — overwrite on subsequent calls
      if (!this.finalLogPath) {
        const timestamp = new Date(this.startTime).toISOString().replace(/[:.]/g, "-");
        const filename = `${timestamp}_${log.sessionId.slice(0, 8)}.json`;
        this.finalLogPath = path.join(logDir, filename);
      }

      fs.writeFileSync(this.finalLogPath, JSON.stringify(log, null, 2), "utf8");

      // Remove live log if it exists (replaced by final)
      if (this.liveLogPath && fs.existsSync(this.liveLogPath)) {
        try { fs.unlinkSync(this.liveLogPath); } catch { /* best effort */ }
      }

      console.log(`[MultiAgentLogger] Session log written to: ${this.finalLogPath}`);

      // Keep last 20 sessions uncompressed, compress the next 30, delete the rest
      const jsonFiles = fs.readdirSync(logDir)
        .filter((f) => f.endsWith(".json") && !f.startsWith("live_"))
        .sort()
        .reverse();

      // Compress sessions 21-50 into .json.gz
      for (const file of jsonFiles.slice(20, 50)) {
        try {
          const fullPath = path.join(logDir, file);
          const content = fs.readFileSync(fullPath);
          const compressed = zlib.gzipSync(content);
          fs.writeFileSync(fullPath + ".gz", new Uint8Array(compressed));
          fs.unlinkSync(fullPath);
          console.log(`[MultiAgentLogger] Compressed: ${file}`);
        } catch { /* best effort */ }
      }

      // Delete anything beyond 50 (both .json and .gz)
      const allLogFiles = fs.readdirSync(logDir)
        .filter((f) => f.endsWith(".json") || f.endsWith(".json.gz"))
        .filter((f) => !f.startsWith("live_"))
        .sort()
        .reverse();
      for (const old of allLogFiles.slice(50)) {
        try { fs.unlinkSync(path.join(logDir, old)); } catch { /* best effort */ }
      }

      return this.finalLogPath;
    } catch (e) {
      console.error("[MultiAgentLogger] Failed to write log:", e);
      return null;
    }
  }
}
