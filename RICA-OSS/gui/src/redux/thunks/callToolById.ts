import { createAsyncThunk, unwrapResult } from "@reduxjs/toolkit";
import { ContextItem, McpUiState } from "core";
import { CLIENT_TOOLS_IMPLS } from "core/tools/builtIn";
import { ContinueError, ContinueErrorReason } from "core/util/errors";
import posthog from "posthog-js";
import { callClientTool } from "../../util/clientTools/callClientTool";
import { selectSelectedChatModel } from "../slices/configSlice";
import {
  acceptToolCall,
  errorToolCall,
  setInactive,
  setToolCallCalling,
  updateToolCallOutput,
} from "../slices/sessionSlice";
import { ThunkApiType } from "../store";
import { findToolCallById, logToolUsage } from "../util";
import { streamResponseAfterToolCall } from "./streamResponseAfterToolCall";

// ── Circuit breaker for repeated tool failures ──
// Same tool + same target failing back-to-back == retry loop. After
// CIRCUIT_BREAKER_THRESHOLD strikes we synthesize a stop result instead of
// re-running the tool, breaking the loop deterministically. Cleared on
// success, on session change, or when the user submits a new message.
// Key: `${sessionId}:${toolName}:${targetKey}` — sessionId scopes per chat,
// targetKey is filepath when present, else a short JSON hash of args.
const CIRCUIT_BREAKER_THRESHOLD = 2;
const toolFailureCounts = new Map<string, number>();
let lastSessionIdSeen: string | null = null;

function getCircuitKey(
  sessionId: string,
  toolName: string,
  argsJson: string,
): string {
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
  return `${sessionId}:${toolName}:${target}`;
}

export const callToolById = createAsyncThunk<
  void,
  { toolCallId: string; isAutoApproved?: boolean; depth?: number },
  ThunkApiType
>("chat/callTool", async (inputs, { dispatch, extra, getState }) => {
  const { toolCallId, isAutoApproved, depth = 0 } = inputs;

  const state = getState();
  const sessionIdAtStart = state.session.id;
  const toolCallState = findToolCallById(state.session.history, toolCallId);
  if (!toolCallState) {
    console.warn(`Tool call with ID ${toolCallId} not found`);
    return;
  }

  if (toolCallState.status !== "generated") {
    return;
  }

  // Wipe failure counters on session change so old loops don't poison new chats
  if (lastSessionIdSeen !== sessionIdAtStart) {
    toolFailureCounts.clear();
    lastSessionIdSeen = sessionIdAtStart;
  }

  const circuitKey = getCircuitKey(
    sessionIdAtStart,
    toolCallState.toolCall.function.name,
    toolCallState.toolCall.function.arguments || "",
  );
  const priorFailures = toolFailureCounts.get(circuitKey) || 0;

  // Track tool call acceptance and start timing
  const startTime = Date.now();

  const selectedChatModel = selectSelectedChatModel(state);

  posthog.capture("tool_call_decision", {
    model: selectedChatModel,
    decision: isAutoApproved ? "auto_accept" : "accept",
    toolName: toolCallState.toolCall.function.name,
    toolCallId: toolCallId,
  });

  if (!selectedChatModel) {
    throw new Error("No model selected");
  }

  dispatch(
    setToolCallCalling({
      toolCallId,
    }),
  );

  let output: ContextItem[] | undefined = undefined;
  let mcpUiState: McpUiState | undefined = undefined;
  let error: ContinueError | undefined = undefined;
  let streamResponse: boolean;

  // ── Circuit breaker trip ──
  if (priorFailures >= CIRCUIT_BREAKER_THRESHOLD) {
    console.log(
      `[callToolById] Circuit breaker tripped for ${circuitKey} (${priorFailures} prior failures) — skipping execution`,
    );
    error = new ContinueError(
      ContinueErrorReason.Unspecified,
      `This tool (${toolCallState.toolCall.function.name}) has already failed ${priorFailures} times on the same target in this session. ` +
        `STOP retrying this exact call. Try a different approach: a smaller change, a different file, or report the failure to the user. ` +
        `The most likely cause is that the file is too large, the model output is being truncated, or the IDE took too long to apply the change.`,
    );
    streamResponse = true;
  } else if (
    // IMPORTANT:
    // Errors that occur while calling tool call implementations
    // Are caught and passed in output as context items
    // Errors that occur outside specifically calling the tool
    // Should not be caught here - should be handled as normal stream errors
    CLIENT_TOOLS_IMPLS.find(
      (toolName) => toolName === toolCallState.toolCall.function.name,
    )
  ) {
    // Tool is called on client side — with timeout to prevent session freeze.
    // Must be >= the inner applyToFile IPC ceiling (5 min, set in editImpl.ts
    // via Fix 1) — otherwise this outer race fires before the inner edit can
    // complete on large files. cd037ad6 stress test confirmed a 120s outer
    // cap killed a 200-line prepend on a ~9KB file at exactly 2:00.
    const CLIENT_TOOL_TIMEOUT_MS = 300_000;
    const clientToolPromise = callClientTool(toolCallState, {
      dispatch,
      ideMessenger: extra.ideMessenger,
      getState,
    });
    const timeoutPromise = new Promise<never>((_, reject) =>
      setTimeout(
        () => reject(new Error("TOOL_TIMEOUT")),
        CLIENT_TOOL_TIMEOUT_MS,
      ),
    );
    try {
      const {
        output: clientToolOutput,
        respondImmediately,
        error: clientToolError,
      } = await Promise.race([clientToolPromise, timeoutPromise]);
      output = clientToolOutput;
      error = clientToolError;
      streamResponse = respondImmediately;
    } catch (e: any) {
      if (e?.message === "TOOL_TIMEOUT") {
        error = new ContinueError(
          ContinueErrorReason.Unspecified,
          `Client tool timed out after ${CLIENT_TOOL_TIMEOUT_MS / 1000}s — the edit operation did not complete. The session will continue.`,
        );
        streamResponse = true;
      } else {
        throw e;
      }
    }
  } else {
    // Tool is called on core side
    const toolName = toolCallState.toolCall.function.name;
    const ipcSentAt = Date.now();
    // Spawn-agent tools legitimately run for many minutes (sub-agents iterate
    // through their own tool loops with up to 100k token budgets). The
    // default 5-min IPC cap was killing long Implementer/Researcher runs at
    // exactly 5:00 with a "tools/call timed out" error even though the
    // sub-agent was still making progress. Give spawn tools 20 min; keep
    // 5 min for everything else.
    const isSpawnTool =
      toolName === "spawn_agent" || toolName === "spawn_agents";
    const ipcTimeoutMs = isSpawnTool ? 1_200_000 : 300_000;
    console.log(
      `[IPC-GUI] Sending tools/call: ${toolName} at ${ipcSentAt} (timeout=${ipcTimeoutMs}ms)`,
    );
    const ipcRequestPromise = extra.ideMessenger.request("tools/call", {
      toolCall: toolCallState.toolCall,
    });
    const ipcTimeoutPromise = new Promise<never>((_, reject) =>
      setTimeout(() => reject(new Error("IPC_TIMEOUT")), ipcTimeoutMs),
    );
    let result: Awaited<typeof ipcRequestPromise>;
    try {
      result = await Promise.race([ipcRequestPromise, ipcTimeoutPromise]);
    } catch (e: any) {
      if (e?.message === "IPC_TIMEOUT") {
        const ms = Date.now() - ipcSentAt;
        console.log(
          `[IPC-GUI] tools/call TIMEOUT: ${toolName} after ${ms}ms (cap=${ipcTimeoutMs}ms)`,
        );
        throw new Error(
          `tools/call timed out after ${ipcTimeoutMs / 1000}s for ${toolName}`,
        );
      }
      throw e;
    }
    const ipcReceivedAt = Date.now();
    const ipcDuration = ipcReceivedAt - ipcSentAt;
    const resultItemCount =
      result.status === "success"
        ? result.content.contextItems?.length || 0
        : 0;
    const resultSize =
      result.status === "success"
        ? result.content.contextItems?.reduce(
            (s: number, ci: any) => s + (ci.content?.length || 0),
            0,
          ) || 0
        : 0;
    console.log(
      `[IPC-GUI] Received tools/call response: ${toolName}, status=${result.status}, items=${resultItemCount}, size=${resultSize}, duration=${ipcDuration}ms`,
    );
    if (result.status === "error") {
      console.log(`[IPC-GUI] tools/call ERROR: ${toolName}: ${result.error}`);
      throw new Error(result.error);
    } else {
      output = result.content.contextItems;
      mcpUiState = result.content.mcpUiState;
      error = result.content.errorMessage
        ? new ContinueError(
            result.content.errorReason || ContinueErrorReason.Unspecified,
            result.content.errorMessage,
          )
        : undefined;
      if (error) {
        console.log(
          `[IPC-GUI] tools/call tool error: ${toolName}: ${error.message}`,
        );
      }
      if (!output || output.length === 0) {
        console.log(
          `[IPC-GUI] tools/call EMPTY result: ${toolName} returned ${output === undefined ? "undefined" : "empty array"}`,
        );
      }
    }
    streamResponse = true;
  }

  // ── Update circuit breaker counters based on outcome ──
  if (error) {
    toolFailureCounts.set(circuitKey, priorFailures + 1);
  } else {
    toolFailureCounts.delete(circuitKey);
  }

  if (error) {
    dispatch(
      updateToolCallOutput({
        toolCallId,
        contextItems: [
          {
            icon: "problems",
            name: "Tool Call Error",
            description: "Tool Call Failed",
            content: `${toolCallState.toolCall.function.name} failed with the message: ${error.message}\n\nPlease try something else or request further instructions.`,
            hidden: false,
          },
        ],
      }),
    );
  } else if (output !== undefined) {
    // NOTE: was `output?.length` — changed to allow empty-array results
    // through. Spawn-agent tools can legitimately return [] when a sub-agent
    // is canceled or budget-exhausted, and the orchestrator needs to see a
    // tool message in either case to continue the loop.
    dispatch(
      updateToolCallOutput({
        toolCallId,
        contextItems: output,
        mcpUiState,
      }),
    );
  }

  // Capture telemetry for tool call execution outcome with duration
  const duration_ms = Date.now() - startTime;
  posthog.capture("tool_call_outcome", {
    model: selectedChatModel,
    succeeded: !error,
    toolName: toolCallState.toolCall.function.name,
    errorReason: error?.reason,
    duration_ms: duration_ms,
  });

  if (streamResponse) {
    if (error) {
      logToolUsage(toolCallState, false, false, extra.ideMessenger, output);
      dispatch(
        errorToolCall({
          toolCallId,
        }),
      );
    } else {
      logToolUsage(toolCallState, true, true, extra.ideMessenger, output);
      dispatch(
        acceptToolCall({
          toolCallId,
        }),
      );
    }

    // Send to the LLM to continue the conversation
    const wrapped = await dispatch(
      streamResponseAfterToolCall({
        toolCallId,
        depth: depth + 1,
      }),
    );
    unwrapResult(wrapped);
  } else {
    dispatch(setInactive());
  }
});
