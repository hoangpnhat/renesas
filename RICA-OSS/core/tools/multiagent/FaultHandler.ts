import { v4 as uuidv4 } from "uuid";
import { ContextItem } from "../../index.js";
import { AgentManager, AgentRecord } from "./AgentManager.js";

export interface ErrorClassification {
  category: "retryable" | "non-retryable" | "user-canceled";
  reason: string;
  suggestedAction: "retry-same" | "retry-modified" | "escalate" | "abort";
}

export interface RetryDecision {
  shouldRetry: boolean;
  modifiedTask?: string;
  delay: number;
  retryNumber: number;
}

// Patterns that indicate transient / retryable errors
const RETRYABLE_PATTERNS = [
  /rate.?limit/i,
  /429/,
  /too many requests/i,
  /timeout/i,
  /ETIMEDOUT/,
  /ECONNRESET/,
  /ECONNREFUSED/,
  /ENOTFOUND/,
  /network error/i,
  /model.?overload/i,
  /server.?error/i,
  /503/,
  /502/,
  /500/,
  /internal server error/i,
  /temporarily unavailable/i,
  /EPIPE/,
  /socket hang up/i,
  /premature close/i,
  /fetch failed/i,
];

// Patterns that indicate non-retryable errors
const NON_RETRYABLE_PATTERNS = [
  /cancel/i,
  /abort/i,
  /unauthorized/i,
  /401/,
  /403/,
  /forbidden/i,
  /invalid.?api.?key/i,
  /authentication/i,
  /permission.?denied/i,
  /max.*iteration/i,
];

export class FaultHandler {
  static readonly MAX_RETRIES = 3;
  static readonly RETRY_DELAYS = [0, 2000, 5000]; // Progressive backoff in ms

  /**
   * Classify an error into retryable, non-retryable, or user-canceled.
   */
  classifyError(error: string, agentRecord?: AgentRecord): ErrorClassification {
    // Check for user cancellation (abort signal)
    if (agentRecord?.status === "canceled") {
      return {
        category: "user-canceled",
        reason: "Agent was canceled by the user",
        suggestedAction: "abort",
      };
    }

    // Check non-retryable patterns first (more specific)
    for (const pattern of NON_RETRYABLE_PATTERNS) {
      if (pattern.test(error)) {
        return {
          category: "non-retryable",
          reason: `Non-retryable error: ${error.slice(0, 100)}`,
          suggestedAction: "escalate",
        };
      }
    }

    // Check retryable patterns
    for (const pattern of RETRYABLE_PATTERNS) {
      if (pattern.test(error)) {
        return {
          category: "retryable",
          reason: `Transient error: ${error.slice(0, 100)}`,
          suggestedAction: "retry-modified",
        };
      }
    }

    // Default: treat unknown errors as retryable on first attempt, non-retryable after
    if (agentRecord && agentRecord.retryCount >= 1) {
      return {
        category: "non-retryable",
        reason: `Unclassified error after retry: ${error.slice(0, 100)}`,
        suggestedAction: "escalate",
      };
    }

    return {
      category: "retryable",
      reason: `Unclassified error (will retry): ${error.slice(0, 100)}`,
      suggestedAction: "retry-modified",
    };
  }

  /**
   * Decide whether to retry based on the error classification and agent history.
   */
  shouldRetry(
    classification: ErrorClassification,
    agentRecord?: AgentRecord,
  ): RetryDecision {
    if (classification.category !== "retryable") {
      return { shouldRetry: false, delay: 0, retryNumber: 0 };
    }

    const currentRetries = agentRecord?.retryCount ?? 0;
    if (currentRetries >= FaultHandler.MAX_RETRIES) {
      return { shouldRetry: false, delay: 0, retryNumber: currentRetries };
    }

    const delay =
      FaultHandler.RETRY_DELAYS[currentRetries] ??
      FaultHandler.RETRY_DELAYS[FaultHandler.RETRY_DELAYS.length - 1];

    const modifiedTask = agentRecord
      ? this.buildRetryTask(agentRecord.task, classification.reason, currentRetries + 1)
      : undefined;

    return {
      shouldRetry: true,
      modifiedTask,
      delay,
      retryNumber: currentRetries + 1,
    };
  }

  /**
   * Build escalation context items when retries are exhausted.
   */
  buildEscalation(
    agentRecord: AgentRecord,
    lastError: string,
  ): ContextItem[] {
    const manager = AgentManager.getInstance();
    const history = manager.getRetryHistory(agentRecord.subAgentId);

    const attempts = history
      .map(
        (h, i) =>
          `  Attempt ${i + 1} (${h.displayName}): ${h.error || h.result?.slice(0, 100) || "no result"}`,
      )
      .join("\n");

    return [
      {
        name: `Sub-Agent Failed (after ${agentRecord.retryCount + 1} attempts)`,
        description: `Agent "${agentRecord.displayName}" failed after all retries`,
        content:
          `Agent "${agentRecord.displayName}" failed after ${agentRecord.retryCount + 1} attempt(s).\n\n` +
          `Last error: ${lastError}\n\n` +
          `Attempt history:\n${attempts}\n\n` +
          `Suggestions:\n` +
          `1. Break the task into smaller, more focused subtasks\n` +
          `2. Try a different approach or role for this task\n` +
          `3. Check if the target files/resources are accessible\n` +
          `4. Ask the user for guidance on how to proceed`,
      },
    ];
  }

  /**
   * Create a new retry agent record in AgentManager.
   */
  prepareRetry(
    originalRecord: AgentRecord,
    modifiedTask: string,
  ): {
    subAgentId: string;
    record: AgentRecord;
  } {
    const manager = AgentManager.getInstance();
    const newId = uuidv4();

    const record = manager.registerAgent({
      subAgentId: newId,
      task: modifiedTask,
      description: originalRecord.task, // Keep original description
      role: originalRecord.role,
      sessionId: originalRecord.sessionId,
      abortController: new AbortController(),
      parentAgentId: originalRecord.subAgentId,
      retryCount: originalRecord.retryCount + 1,
      version: originalRecord.version + 1,
    });

    return { subAgentId: newId, record };
  }

  // ── Private helpers ──

  private buildRetryTask(
    originalTask: string,
    errorReason: string,
    attemptNumber: number,
  ): string {
    return (
      `[RETRY ATTEMPT ${attemptNumber}/${FaultHandler.MAX_RETRIES}]\n` +
      `PREVIOUS ATTEMPT FAILED: ${errorReason}\n` +
      `Adjust your approach to avoid the same failure. Try a different strategy if needed.\n\n` +
      `Original task:\n${originalTask}`
    );
  }
}
