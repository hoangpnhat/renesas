/**
 * Per-session shared context store for multi-agent orchestration.
 * The orchestrator controls writes; agents can read shared context.
 * Supports token budget awareness and automatic summarization.
 */

export interface ContextEntry {
  key: string;
  content: string;
  source: "orchestrator" | "agent-result";
  sourceAgentId?: string;
  sourceAgentName?: string;
  timestamp: number;
  tokenCount: number;
  summary?: string;
  priority: number; // Higher = more important to keep when budget is tight
}

// Token threshold above which we auto-generate a summary
const SUMMARY_TOKEN_THRESHOLD = 2000;

// Default context budget as fraction of model's contextLength
const DEFAULT_BUDGET_FRACTION = 0.3;

/**
 * Simple token estimation without requiring the full tokenizer.
 * Roughly 4 chars per token for English text.
 */
function estimateTokens(text: string): number {
  return Math.ceil(text.length / 4);
}

export class ContextBridge {
  private static instances = new Map<string, ContextBridge>();

  private entries = new Map<string, ContextEntry>();

  /**
   * Get or create a ContextBridge for a session.
   */
  static forSession(sessionId: string): ContextBridge {
    let bridge = ContextBridge.instances.get(sessionId);
    if (!bridge) {
      bridge = new ContextBridge();
      ContextBridge.instances.set(sessionId, bridge);
    }
    return bridge;
  }

  /**
   * Destroy the ContextBridge for a session (cleanup on session kill).
   */
  static destroySession(sessionId: string): void {
    ContextBridge.instances.delete(sessionId);
  }

  /**
   * Push context from the orchestrator or an external source.
   */
  pushContext(key: string, content: string, priority: number = 5): void {
    this.entries.set(key, {
      key,
      content,
      source: "orchestrator",
      timestamp: Date.now(),
      tokenCount: estimateTokens(content),
      priority,
    });
  }

  /**
   * Push an agent's result into shared context.
   * Automatically generates a summary if the result exceeds the token threshold.
   */
  pushAgentResult(
    agentId: string,
    agentName: string,
    result: string,
    summary?: string,
  ): void {
    const tokenCount = estimateTokens(result);
    const key = `agent-result:${agentId}`;

    // Auto-summarize long results if no summary provided
    let autoSummary = summary;
    if (!autoSummary && tokenCount > SUMMARY_TOKEN_THRESHOLD) {
      // Create a simple extractive summary: first 500 chars + last 200 chars
      autoSummary = this.createExtractSummary(result);
    }

    this.entries.set(key, {
      key,
      content: result,
      source: "agent-result",
      sourceAgentId: agentId,
      sourceAgentName: agentName,
      timestamp: Date.now(),
      tokenCount,
      summary: autoSummary,
      priority: 3, // Agent results get medium priority by default
    });
  }

  /**
   * Get formatted shared context for an agent, respecting a token budget.
   * Returns a string that can be prepended to the agent's task.
   */
  getContextForAgent(agentId: string, tokenBudget?: number): string {
    if (this.entries.size === 0) return "";

    const budget = tokenBudget ?? Infinity;

    // Sort entries by priority (highest first), then by timestamp (newest first)
    const sorted = Array.from(this.entries.values())
      .filter((e) => e.sourceAgentId !== agentId) // Don't include agent's own results
      .sort((a, b) => {
        if (b.priority !== a.priority) return b.priority - a.priority;
        return b.timestamp - a.timestamp;
      });

    if (sorted.length === 0) return "";

    const sections: string[] = [];
    let usedTokens = 0;
    const headerTokens = estimateTokens("## Context from Prior Work\n\n");
    usedTokens += headerTokens;

    for (const entry of sorted) {
      const label = entry.sourceAgentName
        ? `### From ${entry.sourceAgentName}`
        : `### ${entry.key}`;
      const labelTokens = estimateTokens(label + "\n");

      // Decide whether to use full content or summary based on budget
      let content: string;
      let contentTokens: number;

      if (usedTokens + labelTokens + entry.tokenCount <= budget) {
        // Full content fits within budget
        content = entry.content;
        contentTokens = entry.tokenCount;
      } else if (entry.summary) {
        // Use summary if available
        const summaryTokens = estimateTokens(entry.summary);
        if (usedTokens + labelTokens + summaryTokens <= budget) {
          content = entry.summary;
          contentTokens = summaryTokens;
        } else {
          // Even summary doesn't fit; skip this entry
          continue;
        }
      } else {
        // No summary and full content doesn't fit; truncate
        const remainingBudget = budget - usedTokens - labelTokens;
        if (remainingBudget <= 50) continue; // Not worth including a tiny snippet
        const charBudget = remainingBudget * 4; // Approximate chars from tokens
        content = entry.content.slice(0, charBudget) + "\n[...truncated]";
        contentTokens = remainingBudget;
      }

      sections.push(`${label}\n${content}`);
      usedTokens += labelTokens + contentTokens;

      if (usedTokens >= budget) break;
    }

    if (sections.length === 0) return "";

    return `## Context from Prior Work\n\n${sections.join("\n\n")}\n`;
  }

  /**
   * Get all context entries, optionally filtered by token budget.
   */
  getAllContext(tokenBudget?: number): ContextEntry[] {
    const sorted = Array.from(this.entries.values()).sort(
      (a, b) => b.priority - a.priority || b.timestamp - a.timestamp,
    );

    if (!tokenBudget) return sorted;

    const result: ContextEntry[] = [];
    let used = 0;
    for (const entry of sorted) {
      if (used + entry.tokenCount > tokenBudget) break;
      result.push(entry);
      used += entry.tokenCount;
    }
    return result;
  }

  /**
   * Get the total estimated token count of all stored context.
   */
  getTokenEstimate(): number {
    let total = 0;
    for (const entry of this.entries.values()) {
      total += entry.tokenCount;
    }
    return total;
  }

  /**
   * Get number of entries.
   */
  get size(): number {
    return this.entries.size;
  }

  /**
   * Clear all entries.
   */
  clear(): void {
    this.entries.clear();
  }

  /**
   * Push a checkpoint entry with agent source tracking.
   */
  pushCheckpoint(
    agentId: string,
    agentName: string,
    stepCount: number,
    content: string,
    priority: number = 2,
  ): void {
    const key = `checkpoint:${agentId}:${stepCount}`;
    this.entries.set(key, {
      key,
      content,
      source: "agent-result",
      sourceAgentId: agentId,
      sourceAgentName: agentName,
      timestamp: Date.now(),
      tokenCount: estimateTokens(content),
      priority,
    });
  }

  /**
   * Get recent peer checkpoint summaries for live mid-run context.
   * Returns only checkpoint entries posted AFTER sinceTimestamp,
   * excluding the requesting agent's own entries.
   */
  getRecentPeerUpdates(excludeAgentId: string, sinceTimestamp: number): string {
    const updates = Array.from(this.entries.values())
      .filter(e =>
        e.key.startsWith("checkpoint:") &&
        e.sourceAgentId !== excludeAgentId &&
        e.timestamp > sinceTimestamp,
      )
      .sort((a, b) => a.timestamp - b.timestamp);

    if (updates.length === 0) return "";

    return updates
      .map(e => `[${e.sourceAgentName || e.key}] ${e.summary || e.content.slice(0, 300)}`)
      .join("\n\n");
  }

  // ── Private helpers ──

  /**
   * Create a simple extractive summary: first few lines + key findings.
   */
  private createExtractSummary(text: string): string {
    const lines = text.split("\n").filter((l) => l.trim().length > 0);
    if (lines.length <= 10) return text;

    // Take first 5 lines and last 3 lines
    const head = lines.slice(0, 5).join("\n");
    const tail = lines.slice(-3).join("\n");

    return `${head}\n\n[...${lines.length - 8} lines omitted...]\n\n${tail}`;
  }
}
