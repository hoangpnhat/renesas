import {
  ChatMessage,
  ContextItem,
  ILLM,
  IDE,
  SubAgentProgress,
  ThinkingEntry,
  Tool,
  ToolAction,
  ToolCall,
  ToolCallDelta,
} from "../../index.js";
import { callTool } from "../callTool.js";
import { BuiltInToolNames } from "../builtIn.js";
import { editFileForSubAgentImpl } from "../implementations/editFileForSubAgent.js";
import { ContinueConfig } from "../../index.js";
import { FetchFunction } from "../../index.js";
import { SubAgentRegistry } from "./SubAgentRegistry.js";
import { ContextBridge } from "./ContextBridge.js";
import { FileTracker } from "./FileTracker.js";
import { MultiAgentLogger } from "./MultiAgentLogger.js";
import { resolveRelativePathInDir } from "../../util/ideUtils.js";

const MAX_TOOL_ITERATIONS = 15;
const DEFAULT_TOKEN_BUDGET = 100_000;
const MESSAGE_PRUNE_THRESHOLD = 20;

const SUB_AGENT_SYSTEM_MESSAGE = `\
You are a focused sub-agent working on a specific task. Complete the task thoroughly and report your results concisely.

You have full access to tools: read files, edit files, create files, search code, run terminal commands, etc.
Use them as needed to accomplish your task.

Important:
- Focus only on your assigned task — do not explore unrelated areas.
- Be thorough but EFFICIENT — complete your task in the minimum number of steps.
- NEVER re-read a file you have already read in this conversation unless it has been modified.
- NEVER repeat a tool call with the same arguments — if you already have the result, use it.
- When you're done, provide a clear summary of what you did and the results. Then STOP.
- If you encounter errors, try to resolve them with at most 2 retry attempts. If still failing, explain what went wrong and STOP.
- You have a limited token budget. Do not waste it on unnecessary exploration.
- Report back when done OR when blocked — do not keep looping.`;

const ROLE_SYSTEM_MESSAGES: Record<string, string> = {
  Researcher: `\
You are a Research Agent. Your specialty is exploring codebases, reading files, searching for patterns, and gathering information.

You have full access to tools: read files, search code (grep, glob), run terminal commands, etc.
Use them to thoroughly investigate and report findings.

Important:
- Focus on exploration and information gathering — do NOT modify files.
- Search broadly, then drill into specifics.
- Report your findings with file paths, line numbers, and relevant code snippets.
- If you encounter errors, try alternative search strategies.
- NEVER re-read a file you already read — use the result from your earlier read.
- When using grep/search, use SPECIFIC patterns targeting specific directories. Never use broad regex like "thread|threadId|thread_id" on the whole workspace — narrow to the relevant source directory.
- Complete your research in the minimum number of steps. Stop when you have enough evidence.
- You have a limited token budget — be efficient.
`,

  Implementer: `\
You are an Implementation Agent. Your specialty is writing new code, editing existing files, and making code changes.

You have full access to tools: read files, edit files, create files, run terminal commands, etc.
Use them to implement the requested changes.

Important:
- Read relevant files before making changes to understand context.
- Make precise, minimal edits — do not refactor unrelated code.
- Follow existing code patterns and conventions.
- Verify your changes compile/parse correctly if possible.
- Report what you changed and why.
- NEVER re-read a file you already read — use the result from your earlier read.
- Complete in minimum steps. If blocked after 2 retries, stop and report.
- You have a limited token budget — be efficient.
`,

  Tester: `\
You are a Testing Agent. Your specialty is running tests, validating changes, and checking for regressions.

You have full access to tools: read files, run terminal commands, search code, etc.
Use them to execute tests and validate correctness.

Important:
- Identify and run the relevant test suites.
- Report test results clearly: passed, failed, errors.
- If tests fail, analyze the failure output and report the root cause.
- Check for regressions in related functionality.
- Complete in minimum steps. If blocked after 2 retries, stop and report.
- You have a limited token budget — be efficient.
`,

  Reviewer: `\
You are a Code Review Agent. Your specialty is analyzing code quality, identifying issues, and providing feedback.

You have full access to tools: read files, search code, run terminal commands, etc.
Use them to thoroughly review code.

Important:
- Check for correctness, security issues, and potential bugs.
- Verify error handling and edge cases.
- Assess code style and consistency with the existing codebase.
- Provide specific, actionable feedback with file paths and line numbers.
- NEVER re-read a file you already read — use the result from your earlier read.
- When using grep/search, target specific directories — never search the entire workspace with broad patterns.
- Complete in minimum steps. Stop when you have enough evidence for your review.
- You have a limited token budget — be efficient.
`,

  Analyst: `\
You are an Analysis Agent. Your specialty is investigating bugs, analyzing problems, and debugging issues.

You have full access to tools: read files, search code, run terminal commands, etc.
Use them to diagnose and understand problems.

Important:
- Start by reproducing or understanding the problem.
- Trace the issue through the codebase systematically.
- Identify root causes, not just symptoms.
- Report your analysis with evidence and suggested fixes.
- NEVER re-read a file you already read — use the result from your earlier read.
- When using grep/search, use SPECIFIC patterns targeting specific directories. Never search the whole workspace with broad regex.
- Complete in minimum steps. Stop when you've identified the root cause.
- You have a limited token budget — be efficient.
`,

  Planner: `\
You are a Planning Agent. Your specialty is creating step-by-step implementation plans and making architecture decisions.

You have full access to tools: read files, search code, run terminal commands, etc.
Use them to understand the codebase before planning.

Important:
- Explore the relevant parts of the codebase first.
- Create specific, actionable plans with file paths and function names.
- Consider edge cases and potential issues.
- Provide clear ordering and dependencies between steps.
- NEVER re-read a file you already read — use the result from your earlier read.
- Complete in minimum steps. Stop when you have enough context to produce the plan.
- You have a limited token budget — be efficient.
`,
};

function getSystemMessage(role?: string): string {
  if (role && ROLE_SYSTEM_MESSAGES[role]) {
    return ROLE_SYSTEM_MESSAGES[role];
  }
  return SUB_AGENT_SYSTEM_MESSAGE;
}

function extractBestPartialResult(thinkingLog: ThinkingEntry[], messages: any[]): string {
  // First try: use thinkingLog (collects all assistant text, most reliable)
  if (thinkingLog.length > 0) {
    const last3 = thinkingLog.slice(-3).map(t => t.content);
    const combined = last3.join("\n\n");
    if (combined.trim()) return combined;
  }
  // Fallback: scan messages for any assistant text
  for (let i = messages.length - 1; i >= 0; i--) {
    const m = messages[i];
    if (m.role !== "assistant") continue;
    const text = typeof m.content === "string"
      ? m.content
      : Array.isArray(m.content)
        ? m.content.filter((p: any) => p.type === "text").map((p: any) => p.text).join("\n")
        : "";
    if (text.trim()) return text;
  }
  return "Agent did not produce text output before hitting its limit.";
}

function basename(filepath: string): string {
  const parts = filepath.replace(/\\/g, "/").split("/");
  return parts[parts.length - 1] || filepath;
}

function summarizeToolArgs(toolName: string, argsJson: string): string | undefined {
  try {
    const args = JSON.parse(argsJson);
    switch (toolName) {
      case BuiltInToolNames.ReadFile:
        return args.filepath ? basename(args.filepath) : undefined;
      case BuiltInToolNames.GrepSearch:
        return args.query ? `"${args.query}"` : undefined;
      case BuiltInToolNames.FileGlobSearch:
        return args.pattern ? args.pattern : undefined;
      case BuiltInToolNames.RunTerminalCommand:
        return args.command ? args.command.slice(0, 80) : undefined;
      case BuiltInToolNames.EditExistingFile:
        return args.filepath ? basename(args.filepath) : undefined;
      case BuiltInToolNames.CreateNewFile:
        return args.filepath ? basename(args.filepath) : undefined;
      case BuiltInToolNames.LSTool:
        return args.dirPath || undefined;
      default: {
        const firstVal = Object.values(args).find((v): v is string => typeof v === "string");
        return firstVal ? firstVal.slice(0, 60) : undefined;
      }
    }
  } catch {
    return undefined;
  }
}

export interface CheckpointResult {
  interimSummary: string;
  stepsCompleted: number;
  tokensUsed: number;
  isDone: boolean;
  finalResult?: string;
}

export interface CheckpointDecision {
  action: "continue" | "redirect" | "stop";
  newContext?: string;
}

export type CheckpointCallback = (checkpoint: CheckpointResult) => Promise<CheckpointDecision>;

interface SubAgentRunParams {
  subAgentId: string;
  parentToolCallId: string;
  task: string;
  description: string;
  role?: string;
  tools: Tool[];
  model: ILLM;
  displayName?: string;
  version?: number;
  retryCount?: number;
  parentAgentId?: string;
  contextBridgeSessionId?: string;
  tokenBudget?: number;
  checkpointInterval?: number;
  onCheckpoint?: CheckpointCallback;
}

export class SubAgentRunner {
  constructor(
    private ide: IDE,
    private config: ContinueConfig,
    private fetchFn: FetchFunction,
    private parentAbortSignal: AbortSignal,
    private sendProgress: (progress: SubAgentProgress) => void,
    private sessionId?: string,
  ) {}

  async run(params: SubAgentRunParams): Promise<ContextItem[]> {
    const {
      subAgentId, parentToolCallId, task, description, role, tools, model,
      displayName, version, retryCount, parentAgentId, contextBridgeSessionId,
    } = params;

    // Create child abort controller linked to parent
    const abortController = new AbortController();
    const onParentAbort = () => abortController.abort();
    this.parentAbortSignal.addEventListener("abort", onParentAbort);

    // Register with global registry for per-agent and session-level cancellation
    SubAgentRegistry.getInstance().register(subAgentId, abortController, this.sessionId);

    // Filter out SpawnAgent tool to prevent nesting
    // Also enforce least-privilege: read-only roles cannot create or edit files
    const READ_ONLY_ROLES = new Set(["Researcher", "Reviewer", "Analyst", "Planner"]);
    const WRITE_TOOLS = new Set([
      BuiltInToolNames.EditExistingFile,
      BuiltInToolNames.CreateNewFile,
    ]);

    const isReadOnly = role ? READ_ONLY_ROLES.has(role) : false;

    const subAgentTools = tools.filter((t) => {
      if (t.function.name === BuiltInToolNames.SpawnAgent) return false;
      if (t.function.name === BuiltInToolNames.AnalyzeTask) return false;
      if (isReadOnly && WRITE_TOOLS.has(t.function.name as BuiltInToolNames)) return false;
      return true;
    });

    // Inject shared context from ContextBridge if available
    let enrichedTask = task;
    if (contextBridgeSessionId) {
      const bridge = ContextBridge.forSession(contextBridgeSessionId);
      const sharedContext = bridge.getContextForAgent(subAgentId);
      if (sharedContext) {
        enrichedTask = `${sharedContext}\n---\n\n${task}`;
        console.log(`[SubAgent:${displayName || subAgentId.slice(0, 8)}] Injected shared context (~${Math.ceil(sharedContext.length / 4)} tokens)`);
      }
    }

    // Build isolated conversation with role-specific system message
    const systemMsg = getSystemMessage(role);
    const messages: ChatMessage[] = [
      { role: "system", content: systemMsg },
      { role: "user", content: enrichedTask },
    ];

    let stepsCompleted = 0;
    let lastCheckpointStep = 0;
    const toolActions: ToolAction[] = [];
    const thinkingLog: ThinkingEntry[] = [];
    const startedAt = Date.now();
    let lastCheckpointTimestamp = startedAt;
    let totalPromptTokens = 0;
    let totalCompletionTokens = 0;
    let totalReasoningTokens = 0;
    let totalCacheReadTokens = 0;
    let totalCacheCreationTokens = 0;
    let hasActualUsage = false;

    // ── Sub-agent circuit breaker (mirrors callToolById.ts orchestrator-side) ──
    // When the LLM stream truncates a tool-call mid-args, we get the SAME
    // malformed JSON every retry (e.g. `{"filepath": "X.md"` — ends at pos 34
    // before the closing comma). callTool() returns errorMessage, the LLM
    // sees the error, retries with the same truncated args, infinite loop.
    // After 2 strikes on the same (toolName, target) we synthesize a STOP
    // result instead of running, breaking the loop deterministically.
    const SUB_AGENT_CIRCUIT_THRESHOLD = 2;
    const subAgentToolFailures = new Map<string, number>();
    const getSubAgentCircuitKey = (
      toolName: string,
      argsJson: string,
    ): string => {
      let target = "";
      try {
        const args = JSON.parse(argsJson || "{}");
        target =
          typeof args.filepath === "string"
            ? args.filepath
            : argsJson.slice(0, 80);
      } catch {
        target = argsJson.slice(0, 80);
      }
      return `${toolName}:${target}`;
    };

    const emitProgress = (
      status: SubAgentProgress["status"],
      currentAction?: string,
      result?: string,
      error?: string,
    ) => {
      this.sendProgress({
        subAgentId,
        parentToolCallId,
        task,
        description,
        role,
        status,
        currentAction,
        stepsCompleted,
        result,
        error,
        toolActions: [...toolActions],
        thinkingLog: [...thinkingLog],
        startedAt,
        completedAt:
          status === "completed" || status === "errored" || status === "canceled"
            ? Date.now()
            : undefined,
        displayName,
        version,
        retryCount,
        parentAgentId,
        tokenUsage: {
          promptTokens: totalPromptTokens,
          completionTokens: totalCompletionTokens,
          reasoningTokens: totalReasoningTokens,
          totalTokens: totalPromptTokens + totalCompletionTokens,
          cacheReadTokens: totalCacheReadTokens,
          cacheCreationTokens: totalCacheCreationTokens,
          isActualUsage: hasActualUsage,
          budget: params.tokenBudget ?? DEFAULT_TOKEN_BUDGET,
        },
      });
    };

    const log = (msg: string, data?: any) => {
      const prefix = `[SubAgent:${displayName || subAgentId.slice(0, 8)}][${description}]`;
      if (data !== undefined) {
        console.log(`${prefix} ${msg}`, data);
      } else {
        console.log(`${prefix} ${msg}`);
      }
    };

    const sessionLogger = this.sessionId ? MultiAgentLogger.forSession(this.sessionId) : null;
    const budgetLimit = params.tokenBudget ?? DEFAULT_TOKEN_BUDGET;
    if (sessionLogger) {
      sessionLogger.agentStarted(subAgentId, displayName || subAgentId.slice(0, 8), role, task, budgetLimit);
      sessionLogger.agentSystemPrompt(subAgentId, getSystemMessage(role));
      sessionLogger.agentEnrichedTask(subAgentId, task, enrichedTask, enrichedTask !== task);
      sessionLogger.agentToolsAvailable(subAgentId, subAgentTools.map((t) => t.function.name));

      const bridgeContext = contextBridgeSessionId
        ? ContextBridge.forSession(contextBridgeSessionId).getContextForAgent(subAgentId)
        : null;
      const bridgeChars = bridgeContext ? bridgeContext.length : 0;
      const totalChars = systemMsg.length + enrichedTask.length;
      sessionLogger.agentPromptAssembled(
        subAgentId, systemMsg.length, enrichedTask.length,
        bridgeChars > 0 ? 1 : 0, bridgeChars,
        Math.ceil(totalChars / 4),
        systemMsg, enrichedTask,
      );
    }

    try {
      log("Started", { task: task.slice(0, 100), tools: subAgentTools.map((t) => t.function.name) });
      emitProgress("running", "Starting...");

      const MAX_STEPS = MAX_TOOL_ITERATIONS * 2; // hard cap on total tool steps
      for (let iteration = 0; iteration < MAX_TOOL_ITERATIONS; iteration++) {
        // Hard stop on total steps (iterations can have multiple tool calls each)
        if (stepsCompleted >= MAX_STEPS) {
          log(`Hit max steps (${MAX_STEPS})`, { stepsCompleted, iteration });
          let partialResult = extractBestPartialResult(thinkingLog, messages);
          if (partialResult.length < 500 && stepsCompleted >= 3) {
            emitProgress("running", "Summarizing findings...");
            const summary = await this.forceSummary(model, messages, abortController.signal, log);
            if (summary) {
              partialResult = summary;
              if (sessionLogger) sessionLogger.log("info", "forced_summary", `Forced summary produced ${summary.length} chars (trigger: max_steps)`, subAgentId, { chars: summary.length, trigger: "max_steps" });
            }
          }
          if (sessionLogger) {
            sessionLogger.agentCompleted(subAgentId, `(Max steps ${MAX_STEPS}) ${partialResult}`);
          }
          emitProgress("completed", undefined, `(Reached max ${MAX_STEPS} steps) ${partialResult}`);
          return [{
            name: `Sub-Agent Result: ${description}`,
            description: `Task completed (step limit): ${description}`,
            content: `(Reached max ${MAX_STEPS} steps after ${iteration} iterations)\n\n${partialResult}`,
          }];
        }

        // Check abort before each LLM call
        if (abortController.signal.aborted) {
          log("Canceled (abort signal)");
          emitProgress("canceled");
          return [
            {
              name: "Sub-Agent Canceled",
              description,
              content: `Sub-agent "${description}" was canceled by the user. Do NOT spawn a new agent for this same task. Do NOT retry or redo this task. The user intentionally stopped it. Use the results you already have from other completed agents and move on.`,
            },
          ];
        }

        // Weighted to match Sang's backend formula:
        //   token_count = tokens_in + tokens_out × 5  (per app/const/token_consumption.py)
        // Pool sizing in spawnAgents.ts is `(threshold - consumed) × 0.4`,
        // i.e. weighted units. Gating raw here would let agents overshoot
        // the pool by ~3-5× for output-heavy tasks.
        const tokensSoFar = totalPromptTokens + totalCompletionTokens * 5;

        // Pre-call budget check: stop BEFORE making another expensive LLM call
        // Use 85% hard threshold + estimated next call cost (avg of prior calls)
        const avgTokensPerCall = iteration > 0 ? tokensSoFar / iteration : 0;
        const estimatedNextTotal = tokensSoFar + avgTokensPerCall;
        if (tokensSoFar >= budgetLimit * 0.85 || (iteration > 0 && estimatedNextTotal >= budgetLimit)) {
          log(`Budget pre-check stop: ${tokensSoFar}/${budgetLimit} (est next: ~${Math.round(estimatedNextTotal)})`);
          let partialResult = extractBestPartialResult(thinkingLog, messages);
          if (partialResult.length < 500 && stepsCompleted >= 3) {
            emitProgress("running", "Summarizing findings...");
            const summary = await this.forceSummary(model, messages, abortController.signal, log);
            if (summary) {
              partialResult = summary;
              if (sessionLogger) sessionLogger.log("info", "forced_summary", `Forced summary produced ${summary.length} chars (trigger: budget_precheck)`, subAgentId, { chars: summary.length, trigger: "budget_precheck" });
            }
          }
          if (sessionLogger) {
            sessionLogger.agentBudgetExhausted(subAgentId, tokensSoFar, budgetLimit, iteration + 1);
            sessionLogger.agentCompleted(subAgentId, `(Token budget reached) ${partialResult}`);
          }
          emitProgress("completed", undefined, `(Token budget reached) ${partialResult}`);
          return [{
            name: `Sub-Agent Result: ${description}`,
            description: `Task completed (token budget reached): ${description}`,
            content: `(Reached token budget of ~${Math.round(budgetLimit / 1000)}K after ${iteration + 1} iterations, ${tokensSoFar} tokens used)\n\n${partialResult}`,
          }];
        }

        log(`Iteration ${iteration + 1}/${MAX_TOOL_ITERATIONS} — calling LLM (${tokensSoFar} tokens used, budget: ${budgetLimit})`);
        emitProgress("running", "Thinking...");

        // Snapshot messages before LLM call
        if (sessionLogger) {
          sessionLogger.agentMessageSnapshot(subAgentId, iteration + 1, messages as any);
        }

        // Call LLM with tools
        const llmCallStart = Date.now();
        let llmResult;
        try {
          llmResult = await this.callLLM(
            model,
            messages,
            subAgentTools,
            abortController.signal,
          );
        } catch (llmError) {
          const llmCallDuration = Date.now() - llmCallStart;
          const errMsg = llmError instanceof Error ? llmError.message : String(llmError);
          if (sessionLogger) {
            let errorType: "rate_limit" | "overloaded" | "timeout" | "network" | "auth" | "server_error" | "unknown" = "unknown";
            let statusCode: number | undefined;
            if (/429|rate.?limit/i.test(errMsg)) { errorType = "rate_limit"; statusCode = 429; }
            else if (/overloaded/i.test(errMsg)) { errorType = "overloaded"; statusCode = 503; }
            else if (/timeout|timed.?out|abort/i.test(errMsg)) { errorType = "timeout"; }
            else if (/network|fetch|ECONNREFUSED|ENOTFOUND/i.test(errMsg)) { errorType = "network"; }
            else if (/401|403|auth|unauthorized|forbidden|api.?key/i.test(errMsg)) { errorType = "auth"; statusCode = /401/.test(errMsg) ? 401 : 403; }
            else if (/5\d{2}|server.?error/i.test(errMsg)) { errorType = "server_error"; const m = errMsg.match(/\b(5\d{2})\b/); if (m) statusCode = parseInt(m[1]); }
            sessionLogger.agentAPIError(subAgentId, iteration + 1, errorType, errMsg, llmCallDuration, statusCode);
          }
          throw llmError;
        }
        const llmCallDuration = Date.now() - llmCallStart;
        const assistantMessage = llmResult.message;
        totalPromptTokens += llmResult.promptTokens;
        totalCompletionTokens += llmResult.completionTokens;
        totalReasoningTokens += llmResult.reasoningTokens;
        totalCacheReadTokens += llmResult.cacheReadTokens;
        totalCacheCreationTokens += llmResult.cacheCreationTokens;
        if (llmResult.isActualUsage) hasActualUsage = true;

        // Log LLM call details
        if (sessionLogger) {
          const responseText = typeof assistantMessage.content === "string"
            ? assistantMessage.content
            : Array.isArray(assistantMessage.content)
              ? assistantMessage.content.filter((p: any) => p.type === "text").map((p: any) => p.text).join("\n")
              : "";
          const toolCallsInResponse = ((assistantMessage as any).toolCalls || []).map((tc: any) => ({
            name: tc.function?.name || "unknown",
            argsPreview: (tc.function?.arguments || "").slice(0, 200),
          }));
          sessionLogger.agentLLMCall(
            subAgentId, iteration + 1, llmCallDuration, messages.length,
            (messages[0]?.content as string || "").length,
            llmResult.promptTokens, llmResult.completionTokens,
            llmResult.cacheReadTokens, llmResult.cacheCreationTokens,
            llmResult.isActualUsage, responseText, toolCallsInResponse,
          );
          sessionLogger.agentIteration(subAgentId, iteration + 1, totalPromptTokens, totalCompletionTokens, totalCacheReadTokens);
        }

        // NOTE: post-call budget check moved AFTER tool execution (below)
        // so any side-effect tools the LLM emitted in THIS response (file
        // writes, terminal commands, etc.) actually run before we cut off.
        // Previous ordering caused agents to generate the work then have it
        // discarded — the "agent didn't deliver" UX. Tool execution itself
        // is local and doesn't burn LLM tokens, so executing them does not
        // worsen the overshoot.

        // Add assistant message to conversation
        messages.push(assistantMessage);

        // Capture the assistant's text as a thinking entry
        const thinkingText =
          typeof assistantMessage.content === "string"
            ? assistantMessage.content
            : Array.isArray(assistantMessage.content)
              ? assistantMessage.content
                  .filter((p: any) => p.type === "text")
                  .map((p: any) => p.text)
                  .join("\n")
              : "";
        if (thinkingText.trim()) {
          thinkingLog.push({
            iteration: iteration + 1,
            content: thinkingText.trim(),
            timestamp: Date.now(),
          });
        }

        // Check if there are tool calls
        const toolCalls = this.extractToolCalls(assistantMessage);

        if (toolCalls.length === 0) {
          // No tool calls — LLM is done, return final response
          const resultText =
            typeof assistantMessage.content === "string"
              ? assistantMessage.content
              : Array.isArray(assistantMessage.content)
                ? assistantMessage.content
                    .filter((p: any) => p.type === "text")
                    .map((p: any) => p.text)
                    .join("\n")
                : "";

          log("Completed", {
            stepsCompleted,
            toolActions: toolActions.map((a) => `${a.toolName}(${a.durationMs ?? "?"}ms)`),
            resultPreview: resultText.slice(0, 200),
            totalDurationMs: Date.now() - startedAt,
          });
          if (sessionLogger) {
            sessionLogger.agentCompleted(subAgentId, resultText);
          }
          emitProgress("completed", undefined, resultText);

          return [
            {
              name: `Sub-Agent Result: ${description}`,
              description: `Completed task: ${description}`,
              content: resultText,
            },
          ];
        }

        log(`LLM returned ${toolCalls.length} tool call(s): ${toolCalls.map((tc) => tc.function.name).join(", ")}`);

        // Sync assistant message toolCalls to only include validated ones
        // (streaming may produce incomplete tool call deltas)
        (assistantMessage as any).toolCalls = toolCalls;

        // Execute each tool call
        for (const toolCall of toolCalls) {
          if (abortController.signal.aborted) {
            break;
          }

          stepsCompleted++;
          const toolName = toolCall.function.name;

          // Track this tool action
          const actionIndex = toolActions.length;
          const toolStartTime = Date.now();
          const argSummary = summarizeToolArgs(toolName, toolCall.function.arguments);
          toolActions.push({
            toolName,
            timestamp: toolStartTime,
            status: "running",
            argSummary,
          });
          log(`Tool [${stepsCompleted}] ${toolName} — executing`, {
            args: toolCall.function.arguments.slice(0, 200),
          });
          emitProgress("running", argSummary ? `${toolName}: ${argSummary}` : `Using tool: ${toolName}`);

          let toolResult: string;

          // ── Circuit breaker pre-check ──
          const circuitKey = getSubAgentCircuitKey(
            toolName,
            toolCall.function.arguments || "",
          );
          const priorFailures = subAgentToolFailures.get(circuitKey) || 0;
          const circuitTripped =
            priorFailures >= SUB_AGENT_CIRCUIT_THRESHOLD;

          if (circuitTripped) {
            toolResult =
              `Error: This tool (${toolName}) has already failed ${priorFailures} times on the same target in this sub-agent. ` +
              `STOP retrying this exact call — your tool-call JSON args are likely being truncated mid-stream by the model. ` +
              `Try a different approach: a shorter file, a different tool, or stop and report the failure to the orchestrator in your final message.`;
            log(
              `Circuit breaker tripped: ${circuitKey} (${priorFailures} prior failures) — skipping execution`,
            );
          } else {
          try {
            // Handle EditExistingFile specially — use core-side implementation
            if (toolName === BuiltInToolNames.EditExistingFile) {
              const args = JSON.parse(toolCall.function.arguments || "{}");
              const contextItems = await editFileForSubAgentImpl(args, {
                ide: this.ide,
                llm: model,
                fetch: this.fetchFn,
                tool:
                  subAgentTools.find(
                    (t) => t.function.name === toolName,
                  ) || subAgentTools[0],
                config: this.config,
                agentId: subAgentId,
                agentName: displayName || subAgentId.slice(0, 8),
                sessionId: this.sessionId,
              });
              toolResult = contextItems
                .map((item) => item.content)
                .join("\n");
            } else {
              // Use standard callTool for all other tools
              const tool = subAgentTools.find(
                (t) => t.function.name === toolName,
              );
              if (!tool) {
                toolResult = `Error: Tool "${toolName}" not found.`;
              } else {
                // callTool takes the full ToolCall (it calls
                // safeParseToolCallArgs internally), not raw args. Passing raw
                // args here makes every sub-agent tool call receive empty args
                // (read_file, file_glob_search, etc. fail with "no filepath" /
                // "no pattern" errors, visible as ❌ in the robot strip). Pass
                // the full toolCall shape that callTool expects.
                const { contextItems, errorMessage } = await callTool(
                  tool,
                  toolCall as any,
                  {
                    ide: this.ide,
                    llm: model,
                    fetch: this.fetchFn,
                    tool,
                    toolCallId: toolCall.id,
                    config: this.config,
                  },
                );
                if (errorMessage) {
                  toolResult = `Error: ${errorMessage}`;
                } else {
                  toolResult = contextItems
                    .map((item) => item.content)
                    .join("\n");

                  // Track CreateNewFile writes for conflict detection
                  if (
                    toolName === BuiltInToolNames.CreateNewFile &&
                    this.sessionId
                  ) {
                    try {
                      const toolArgs = JSON.parse(toolCall.function.arguments || "{}");
                      if (toolArgs.filepath) {
                        const fileUri = await resolveRelativePathInDir(toolArgs.filepath, this.ide);
                        if (fileUri) {
                          const normalizedPath = fileUri.replace(/\\/g, "/").toLowerCase();
                          console.log(
                            `[FileTracker] Recording create: ${normalizedPath} by ${displayName || subAgentId.slice(0, 8)} (session: ${this.sessionId.slice(0, 8)})`,
                          );
                          FileTracker.forSession(this.sessionId).recordEdit({
                            filepath: normalizedPath,
                            agentId: subAgentId,
                            agentName: displayName || subAgentId.slice(0, 8),
                            timestamp: Date.now(),
                            contentHashBefore: "0", // new file
                            contentHashAfter: "0",
                          });
                        }
                      }
                    } catch { /* best-effort tracking */ }
                  }
                }
              }
            }
          } catch (e) {
            toolResult = `Error executing tool ${toolName}: ${e instanceof Error ? e.message : String(e)}`;
          }
          } // ← end of circuit-breaker `else` branch

          // ── Update circuit breaker counters based on outcome ──
          if (toolResult.startsWith("Error")) {
            subAgentToolFailures.set(circuitKey, priorFailures + 1);
          } else {
            subAgentToolFailures.delete(circuitKey);
          }

          // B6 fix: truncate oversized tool results to prevent API payload explosion
          const MAX_TOOL_RESULT_CHARS = 50_000;
          let wasTruncated = false;
          let originalResultLength = toolResult.length;
          if (toolResult.length > MAX_TOOL_RESULT_CHARS) {
            wasTruncated = true;
            originalResultLength = toolResult.length;
            const headSize = Math.round(MAX_TOOL_RESULT_CHARS * 0.8);
            const tailSize = Math.round(MAX_TOOL_RESULT_CHARS * 0.1);
            const head = toolResult.slice(0, headSize);
            const tail = toolResult.slice(-tailSize);
            toolResult = `${head}\n\n[... TRUNCATED: ${originalResultLength.toLocaleString()} chars total, showing first ~${headSize.toLocaleString()} + last ~${tailSize.toLocaleString()} chars ...]\n\n${tail}`;
            log(`Tool result truncated: ${originalResultLength.toLocaleString()} → ${toolResult.length.toLocaleString()} chars`);
            if (sessionLogger) {
              sessionLogger.log("warn", "tool_truncation", `Tool result truncated: ${toolName} ${originalResultLength.toLocaleString()} → ${toolResult.length.toLocaleString()} chars`, subAgentId, {
                toolName,
                originalChars: originalResultLength,
                truncatedTo: toolResult.length,
              });
            }
          }

          // Update tool action with completion status
          const toolDuration = Date.now() - toolStartTime;
          const toolStatus = toolResult.startsWith("Error") ? "errored" : "completed";
          toolActions[actionIndex] = {
            ...toolActions[actionIndex],
            status: toolStatus,
            durationMs: toolDuration,
          };
          log(`Tool [${stepsCompleted}] ${toolName} — ${toolStatus} (${toolDuration}ms)`, {
            resultPreview: toolResult.slice(0, 150),
          });

          // Log tool call to structured logger
          if (sessionLogger) {
            sessionLogger.agentToolCall(
              subAgentId, iteration + 1, stepsCompleted, toolName,
              toolCall.function.arguments || "{}", argSummary,
              toolDuration, toolStatus as "completed" | "errored",
              toolResult,
              toolStatus === "errored" ? toolResult : undefined,
              wasTruncated, wasTruncated ? originalResultLength : undefined,
            );
          }

          // Add tool result to conversation
          const toolResultMessage: ChatMessage = {
            role: "tool",
            content: toolResult,
            toolCallId: toolCall.id,
          };
          messages.push(toolResultMessage);

          // ── Checkpoint pause point (checked after EACH tool, not after entire batch) ──
          const checkpointDue = params.checkpointInterval && params.onCheckpoint &&
              stepsCompleted > 0 &&
              Math.floor(stepsCompleted / params.checkpointInterval) > Math.floor(lastCheckpointStep / params.checkpointInterval);
          if (checkpointDue) {
            // Add placeholder results for remaining tool calls in this batch
            for (const remaining of toolCalls) {
              const hasResult = messages.some(m => m.role === "tool" && (m as any).toolCallId === remaining.id);
              if (!hasResult) {
                messages.push({
                  role: "tool",
                  content: "Paused at checkpoint — tool not yet executed.",
                  toolCallId: remaining.id,
                });
              }
            }
            lastCheckpointStep = stepsCompleted;

          // Build summary from: last assistant text + recent tool results
          const recentAssistantText = thinkingLog.slice(-2).map(t => t.content).join("\n");
          const recentToolSummary = toolActions.slice(-params.checkpointInterval!).map(ta =>
            `${ta.toolName}: ${ta.status} — ${ta.argSummary || ""}`,
          ).join("\n");
          const interimSummary = recentAssistantText.trim()
            ? recentAssistantText
            : `Tools executed:\n${recentToolSummary}`;
          const tokensUsed = totalPromptTokens + totalCompletionTokens;

          if (contextBridgeSessionId) {
            ContextBridge.forSession(contextBridgeSessionId).pushCheckpoint(
              subAgentId,
              displayName || subAgentId.slice(0, 8),
              stepsCompleted,
              `[${displayName} — step ${stepsCompleted}]\n${interimSummary}`,
            );
          }

          emitProgress("running", `Checkpoint (${stepsCompleted} steps)`);
          if (sessionLogger) {
            sessionLogger.log("info", "checkpoint", `Checkpoint at step ${stepsCompleted}`, subAgentId, {
              step: stepsCompleted,
              summaryPreview: interimSummary.slice(0, 500),
              tokensUsed,
            });
          }

          const decision = await params.onCheckpoint!({
            interimSummary, stepsCompleted, tokensUsed, isDone: false,
          });

          if (decision.action === "stop") {
            log(`Stopped by master at checkpoint (step ${stepsCompleted})`);
            let finalResult = interimSummary;
            if (stepsCompleted >= 3 && interimSummary.length < 500) {
              emitProgress("running", "Summarizing findings...");
              const summary = await this.forceSummary(model, messages, abortController.signal, log);
              if (summary) {
                finalResult = summary;
                if (sessionLogger) sessionLogger.log("info", "forced_summary",
                  `Forced summary produced ${summary.length} chars (trigger: checkpoint_stop)`,
                  subAgentId, { chars: summary.length, trigger: "checkpoint_stop" });
              }
            }
            if (sessionLogger) sessionLogger.agentCompleted(subAgentId, `(Stopped at checkpoint) ${finalResult}`);
            emitProgress("completed", undefined, `(Stopped at checkpoint ${stepsCompleted}) ${finalResult}`);
            return [{
              name: `Sub-Agent Result: ${description}`,
              description: `Stopped at checkpoint: ${description}`,
              content: `(Stopped at step ${stepsCompleted} by orchestrator)\n\n${finalResult}`,
            }];
          }

          if (decision.action === "redirect" && decision.newContext) {
            log(`Redirected by master at checkpoint (step ${stepsCompleted})`);
            messages.push({
              role: "user",
              content: `[REDIRECT from orchestrator]\n${decision.newContext}\n\nAdjust your approach based on this feedback and continue.`,
            });
          }

          // Inject peer updates for awareness
          if (contextBridgeSessionId) {
            const bridge = ContextBridge.forSession(contextBridgeSessionId);
            const peerUpdates = bridge.getRecentPeerUpdates(subAgentId, lastCheckpointTimestamp);
            if (peerUpdates) {
              messages.push({
                role: "user",
                content: `[PEER UPDATES — other agents' recent progress]\n${peerUpdates}\n\nUse this awareness to avoid duplicating work already done by peers.`,
              });
            }
            lastCheckpointTimestamp = Date.now();
          }
            break; // exit tool loop — remaining tools got placeholders, next iteration starts fresh
          }
        }

        // Ensure ALL tool calls have matching tool_result messages.
        // Tool calls may be missing results if abort fired mid-loop.
        const resultIds = new Set(
          messages
            .filter((m): m is ChatMessage & { toolCallId: string } =>
              m.role === "tool" && "toolCallId" in m && !!m.toolCallId,
            )
            .map((m) => m.toolCallId),
        );
        for (const tc of toolCalls) {
          if (!resultIds.has(tc.id)) {
            messages.push({
              role: "tool",
              content: "Canceled — agent was stopped before this tool could run.",
              toolCallId: tc.id,
            });
          }
        }

        // Post-call budget check — runs AFTER tools have executed so any
        // side effects (file writes, etc.) from this iteration land first.
        // Weighted formula matches Sang's backend: prompt + completion × 5.
        const totalTokensUsed = totalPromptTokens + totalCompletionTokens * 5;
        const budget = params.tokenBudget ?? DEFAULT_TOKEN_BUDGET;
        if (totalTokensUsed >= budget) {
          let partialResult = extractBestPartialResult(thinkingLog, messages);
          if (partialResult.length < 500 && stepsCompleted >= 3) {
            emitProgress("running", "Summarizing findings...");
            const summary = await this.forceSummary(model, messages, abortController.signal, log);
            if (summary) {
              partialResult = summary;
              if (sessionLogger) sessionLogger.log("info", "forced_summary", `Forced summary produced ${summary.length} chars (trigger: budget_postcall)`, subAgentId, { chars: summary.length, trigger: "budget_postcall" });
            }
          }

          log(`Token budget exhausted: ${totalTokensUsed}/${budget} tokens after ${iteration + 1} iterations (tool calls already executed)`);
          if (sessionLogger) {
            sessionLogger.agentBudgetExhausted(subAgentId, totalTokensUsed, budget, iteration + 1);
            sessionLogger.agentCompleted(subAgentId, `(Token budget reached) ${partialResult}`);
          }
          emitProgress("completed", undefined, `(Token budget reached) ${partialResult}`);

          return [
            {
              name: `Sub-Agent Result: ${description}`,
              description: `Task completed (token budget reached): ${description}`,
              content: `(Reached token budget of ~${Math.round(budget / 1000)}K after ${iteration + 1} iterations, ${totalTokensUsed} tokens used)\n\n${partialResult}`,
            },
          ];
        }

        // Prune message history if too long to keep token usage manageable
        if (messages.length > MESSAGE_PRUNE_THRESHOLD) {
          const beforeCount = messages.length;
          const pruned = this.pruneMessages(messages);
          messages.length = 0;
          messages.push(...pruned);
          log(`Pruned messages: ${beforeCount} -> ${messages.length}`);
          if (sessionLogger) {
            sessionLogger.agentMessagesPruned(subAgentId, beforeCount, messages.length);
          }
        }
      }

      // Hit max iterations
      log(`Hit max iterations (${MAX_TOOL_ITERATIONS})`, { stepsCompleted, totalDurationMs: Date.now() - startedAt });
      let partialResult = extractBestPartialResult(thinkingLog, messages);
      if (partialResult.length < 500 && stepsCompleted >= 3) {
        emitProgress("running", "Summarizing findings...");
        const summary = await this.forceSummary(model, messages, abortController.signal, log);
        if (summary) {
          partialResult = summary;
          if (sessionLogger) sessionLogger.log("info", "forced_summary", `Forced summary produced ${summary.length} chars (trigger: max_iterations)`, subAgentId, { chars: summary.length, trigger: "max_iterations" });
        }
      }

      if (sessionLogger) {
        sessionLogger.agentCompleted(subAgentId, `(Max iterations) ${partialResult}`);
      }

      emitProgress(
        "completed",
        undefined,
        `(Reached max ${MAX_TOOL_ITERATIONS} iterations) ${partialResult}`,
      );

      return [
        {
          name: `Sub-Agent Result: ${description}`,
          description: `Task completed (max iterations reached): ${description}`,
          content: `(Reached max ${MAX_TOOL_ITERATIONS} iterations) ${partialResult}`,
        },
      ];
    } catch (e) {
      const errorMsg = e instanceof Error ? e.message : String(e);
      log(`Error: ${errorMsg}`, { aborted: abortController.signal.aborted, totalDurationMs: Date.now() - startedAt });

      if (abortController.signal.aborted) {
        if (sessionLogger) {
          sessionLogger.agentCanceled(subAgentId);
        }
        emitProgress("canceled");
        return [
          {
            name: "Sub-Agent Canceled",
            description,
            content: `Sub-agent "${description}" was canceled by the user. Do NOT spawn a new agent for this same task. Do NOT retry or redo this task. The user intentionally stopped it. Use the results you already have from other completed agents and move on.`,
          },
        ];
      }

      if (sessionLogger) {
        sessionLogger.agentErrored(subAgentId, errorMsg);
      }
      emitProgress("errored", undefined, undefined, errorMsg);
      return [
        {
          name: "Sub-Agent Error",
          description,
          content: `Sub-agent "${description}" failed with error: ${errorMsg}`,
        },
      ];
    } finally {
      this.parentAbortSignal.removeEventListener("abort", onParentAbort);
      SubAgentRegistry.getInstance().unregister(subAgentId);
      // Write session log to disk when last agent finishes
      if (sessionLogger) {
        sessionLogger.writeToDisk();
      }
    }
  }

  private async forceSummary(
    model: ILLM,
    messages: ChatMessage[],
    abortSignal: AbortSignal,
    log: (...args: any[]) => void,
  ): Promise<string> {
    try {
      log("Forcing summary call (agent produced no text output)");
      const summaryMessages: ChatMessage[] = [
        ...messages,
        {
          role: "user" as const,
          content: "You have reached your step/token limit. Summarize ALL findings from your tool calls above in a concise report. Include file paths, key data points, and any issues found. Do NOT call any tools — just write your summary.",
        },
      ];
      const result = await this.callLLM(model, summaryMessages, [], abortSignal);
      const text = typeof result.message.content === "string"
        ? result.message.content
        : Array.isArray(result.message.content)
          ? result.message.content.filter((p: any) => p.type === "text").map((p: any) => p.text).join("\n")
          : "";
      if (text.trim()) {
        log(`Forced summary produced ${text.length} chars`);
        return text;
      }
    } catch (e) {
      log(`Forced summary failed: ${e instanceof Error ? e.message : String(e)}`);
    }
    return "";
  }

  private async callLLM(
    model: ILLM,
    messages: ChatMessage[],
    tools: Tool[],
    signal: AbortSignal,
  ): Promise<{ message: ChatMessage; promptTokens: number; completionTokens: number; reasoningTokens: number; cacheReadTokens: number; cacheCreationTokens: number; isActualUsage: boolean }> {
    const abortController = new AbortController();
    const combinedSignal = signal;
    signal.addEventListener("abort", () => abortController.abort(), { once: true });

    const STALE_STREAM_TIMEOUT_MS = 90_000;
    let staleTimer: ReturnType<typeof setTimeout> | null = null;
    const resetStaleTimer = () => {
      if (staleTimer) clearTimeout(staleTimer);
      staleTimer = setTimeout(() => {
        console.warn(`[SubAgent:callLLM] No data received for ${STALE_STREAM_TIMEOUT_MS / 1000}s — aborting stale LLM call`);
        abortController.abort();
      }, STALE_STREAM_TIMEOUT_MS);
    };

    const gen = model.streamChat(messages, abortController.signal, {
      tools: tools.length > 0 ? tools : undefined,
    });

    // Collect the full response (we don't stream sub-agent output to GUI)
    let fullContent = "";
    let allToolCalls: ToolCallDelta[] = [];

    resetStaleTimer();
    let result = await gen.next();
    while (!result.done) {
      resetStaleTimer();
      const chunk = result.value;
      if (typeof chunk.content === "string") {
        fullContent += chunk.content;
      }
      if ("toolCalls" in chunk && chunk.toolCalls) {
        allToolCalls = this.mergeToolCallDeltas(allToolCalls, chunk.toolCalls);
      }
      result = await gen.next();
    }
    if (staleTimer) clearTimeout(staleTimer);

    // result.value is the PromptLog returned by the generator
    const promptLog = result.value as any;

    const assembled: ChatMessage = {
      role: "assistant",
      content: fullContent,
    };

    if (allToolCalls.length > 0) {
      assembled.toolCalls = allToolCalls;
    }

    return {
      message: assembled,
      promptTokens: promptLog?.promptTokens ?? 0,
      completionTokens: promptLog?.completionTokens ?? 0,
      reasoningTokens: promptLog?.reasoningTokens ?? 0,
      cacheReadTokens: promptLog?.cacheReadTokens ?? 0,
      cacheCreationTokens: promptLog?.cacheCreationTokens ?? 0,
      isActualUsage: promptLog?.isActualUsage ?? false,
    };
  }

  private mergeToolCallDeltas(
    existing: ToolCallDelta[],
    incoming: ToolCallDelta[],
  ): ToolCallDelta[] {
    for (const delta of incoming) {
      if (delta.id) {
        // New tool call or update existing by id
        const existingIdx = existing.findIndex((tc) => tc.id === delta.id);
        if (existingIdx >= 0) {
          // Append arguments
          const ex = existing[existingIdx];
          if (delta.function?.arguments) {
            ex.function = ex.function || {};
            ex.function.arguments =
              (ex.function.arguments || "") + delta.function.arguments;
          }
          if (delta.function?.name) {
            ex.function = ex.function || {};
            ex.function.name = delta.function.name;
          }
        } else {
          existing.push({ ...delta });
        }
      } else if (existing.length > 0) {
        // No id — append to the last tool call's arguments
        const last = existing[existing.length - 1];
        if (delta.function?.arguments) {
          last.function = last.function || {};
          last.function.arguments =
            (last.function.arguments || "") + delta.function.arguments;
        }
      }
    }
    return existing;
  }

  private extractToolCalls(message: ChatMessage): ToolCall[] {
    if (!("toolCalls" in message) || !message.toolCalls) {
      return [];
    }

    return message.toolCalls
      .filter(
        (tc): tc is ToolCall =>
          !!tc.id && !!tc.function?.name && tc.function?.arguments !== undefined,
      )
      .map((tc) => ({
        id: tc.id!,
        type: "function" as const,
        function: {
          name: tc.function!.name!,
          arguments: tc.function!.arguments || "{}",
        },
      }));
  }

  private pruneMessages(messages: ChatMessage[], keepRecent: number = 10): ChatMessage[] {
    if (messages.length <= MESSAGE_PRUNE_THRESHOLD) return messages;

    const systemMsg = messages[0];

    // Find a clean cut point that doesn't split tool_use/tool_result pairs
    let cutPoint = messages.length - keepRecent;
    while (cutPoint < messages.length && messages[cutPoint].role === "tool") {
      cutPoint--;
    }
    const recentMessages = messages.slice(cutPoint);
    const prunedMiddle = messages.slice(1, cutPoint);

    const toolNames = prunedMiddle
      .filter((m) => m.role === "assistant" && "toolCalls" in m && m.toolCalls)
      .flatMap((m) => ((m as any).toolCalls || []).map((tc: any) => tc.function?.name))
      .filter(Boolean);

    const summaryContent =
      `[Earlier conversation pruned for efficiency. ` +
      `${prunedMiddle.length} messages removed. ` +
      `Tools used: ${[...new Set(toolNames)].join(", ") || "none"}. ` +
      `Continue with your current task based on recent context.]`;

    return [
      systemMsg,
      { role: "user" as const, content: summaryContent },
      ...recentMessages,
    ];
  }
}
