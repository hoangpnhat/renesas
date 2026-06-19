import { SubAgentStatus } from "../../index.js";
import { createHash } from "crypto";

export interface AgentRecord {
  subAgentId: string;
  displayName: string;
  role: string | undefined;
  taskHash: string;
  task: string;
  scope?: string;
  status: SubAgentStatus;
  retryCount: number;
  version: number;
  parentAgentId?: string;
  sessionId?: string;
  /** Title of the model the agent is running on. Used by ConfigHandler's
   * cancel-on-block hook to match a session against a now-blocked model. */
  modelTitle?: string;
  abortController: AbortController;
  startedAt: number;
  completedAt?: number;
  result?: string;
  error?: string;
}

export interface DuplicateCheckResult {
  isDuplicate: boolean;
  existingAgent?: AgentRecord;
}

/**
 * Enhanced agent registry with auto-naming, deduplication, and versioning.
 * Maintains backward compatibility with SubAgentRegistry API.
 */
export class AgentManager {
  private static instance: AgentManager;

  private agents = new Map<string, AgentRecord>();

  // Per-session counters for auto-naming: "sessionId:role" -> count
  private roleCounters = new Map<string, number>();

  // Global counter for agents without roles
  private globalCounter = 0;

  static getInstance(): AgentManager {
    if (!AgentManager.instance) {
      AgentManager.instance = new AgentManager();
    }
    return AgentManager.instance;
  }

  // ── Backward-compatible API (matches old SubAgentRegistry) ──

  register(
    subAgentId: string,
    abortController: AbortController,
    sessionId?: string,
  ): void {
    // If already registered with full record (via registerAgent), skip
    if (this.agents.has(subAgentId)) {
      const existing = this.agents.get(subAgentId)!;
      existing.abortController = abortController;
      existing.sessionId = sessionId;
      return;
    }

    // Fallback: create a minimal record for backward compatibility
    this.agents.set(subAgentId, {
      subAgentId,
      displayName: `Agent-${++this.globalCounter}`,
      role: undefined,
      taskHash: "",
      task: "",
      status: "running",
      retryCount: 0,
      version: 1,
      sessionId,
      abortController,
      startedAt: Date.now(),
    });
  }

  /**
   * Called by SubAgentRunner.finally to clean up abort listeners.
   * Keeps the agent record for deduplication lookups — only clears
   * the abort controller reference so it can be GC'd.
   */
  unregister(subAgentId: string): void {
    const record = this.agents.get(subAgentId);
    if (!record) return;

    // If the record has a final status with cached result, keep it for dedup
    if (
      record.status === "completed" ||
      record.status === "errored" ||
      record.status === "canceled"
    ) {
      // Already finalized by updateStatus — keep the record, just
      // replace the abort controller with a dummy so the real one can be GC'd
      record.abortController = new AbortController();
      return;
    }

    // Still "running" at unregister time means updateStatus hasn't been
    // called yet (SubAgentRunner.finally fires before spawnAgentImpl
    // calls updateStatus). Mark as completed-pending so it stays in map.
    record.abortController = new AbortController();
  }

  cancelAgent(subAgentId: string): boolean {
    const entry = this.agents.get(subAgentId);
    if (entry) {
      entry.abortController.abort();
      entry.status = "canceled";
      entry.completedAt = Date.now();
      // Keep the record in the map so it remains visible in Agent History
      // (getAgentsForSession). It's already excluded from checkDuplicate
      // and getActiveSessions because those filter by status.
      return true;
    }
    return false;
  }

  cancelAllForSession(sessionId: string): number {
    let count = 0;
    for (const [, entry] of this.agents) {
      if (entry.sessionId === sessionId) {
        entry.abortController.abort();
        entry.status = "canceled";
        entry.completedAt = Date.now();
        // Keep canceled records visible in Agent History — same rationale
        // as cancelAgent above.
        count++;
      }
    }
    return count;
  }

  cancelAll(): number {
    const count = this.agents.size;
    for (const [, entry] of this.agents) {
      entry.abortController.abort();
      entry.status = "canceled";
      entry.completedAt = Date.now();
    }
    // Don't clear the map — canceled agents stay in history.
    return count;
  }

  // ── Enhanced API ──

  /**
   * Register a full agent record with auto-naming and metadata.
   * Call this instead of register() when spawning through spawnAgentImpl.
   */
  registerAgent(params: {
    subAgentId: string;
    task: string;
    description: string;
    role?: string;
    sessionId?: string;
    abortController: AbortController;
    parentAgentId?: string;
    retryCount?: number;
    version?: number;
    modelTitle?: string;
  }): AgentRecord {
    const {
      subAgentId,
      task,
      description,
      role,
      sessionId,
      abortController,
      parentAgentId,
      retryCount = 0,
      version = 1,
      modelTitle,
    } = params;

    const taskHash = this.hashTask(task, role);
    const scope = this.extractScope(task);
    const displayName = this.generateDisplayName(role, scope, sessionId, version);

    const record: AgentRecord = {
      subAgentId,
      displayName,
      role,
      taskHash,
      task,
      scope,
      status: "running",
      retryCount,
      version,
      parentAgentId,
      sessionId,
      modelTitle,
      abortController,
      startedAt: Date.now(),
    };

    this.agents.set(subAgentId, record);
    return record;
  }

  /**
   * Enumerate all running agents grouped by session, with their model titles.
   * Used by ConfigHandler's cancel-on-block hook to match active sessions
   * against models whose backend `consumptionLimit.blocked` just flipped true.
   *
   * Returns one entry per (sessionId, modelTitle) pair — if a session has
   * agents on multiple models (rare), each combination is reported.
   */
  getActiveSessions(): Array<{ sessionId: string; modelTitle: string }> {
    const seen = new Set<string>();
    const out: Array<{ sessionId: string; modelTitle: string }> = [];
    for (const agent of this.agents.values()) {
      if (agent.status !== "running") continue;
      if (!agent.sessionId || !agent.modelTitle) continue;
      const key = `${agent.sessionId}::${agent.modelTitle}`;
      if (seen.has(key)) continue;
      seen.add(key);
      out.push({ sessionId: agent.sessionId, modelTitle: agent.modelTitle });
    }
    return out;
  }

  /**
   * Check if an equivalent task is already running or completed in this session.
   */
  checkDuplicate(task: string, role?: string, sessionId?: string): DuplicateCheckResult {
    const taskHash = this.hashTask(task, role);

    for (const agent of this.agents.values()) {
      if (agent.taskHash !== taskHash) continue;
      if (sessionId && agent.sessionId !== sessionId) continue;

      if (agent.status === "completed" && agent.result) {
        return { isDuplicate: true, existingAgent: agent };
      }

      if (agent.status === "running") {
        return { isDuplicate: true, existingAgent: agent };
      }
    }

    return { isDuplicate: false };
  }

  /**
   * Update agent status after completion or failure.
   */
  updateStatus(
    subAgentId: string,
    status: SubAgentStatus,
    result?: string,
    error?: string,
  ): void {
    const agent = this.agents.get(subAgentId);
    if (!agent) return;

    agent.status = status;
    if (result !== undefined) agent.result = result;
    if (error !== undefined) agent.error = error;
    if (status === "completed" || status === "errored" || status === "canceled") {
      agent.completedAt = Date.now();
    }
  }

  /**
   * Get agent record by ID.
   */
  getAgent(subAgentId: string): AgentRecord | undefined {
    return this.agents.get(subAgentId);
  }

  /**
   * Get all agent records for a session.
   */
  getAgentsForSession(sessionId: string): AgentRecord[] {
    return Array.from(this.agents.values()).filter(
      (a) => a.sessionId === sessionId,
    );
  }

  /**
   * Get retry history for an agent (all versions sharing the same task hash in the session).
   */
  getRetryHistory(subAgentId: string): AgentRecord[] {
    const agent = this.agents.get(subAgentId);
    if (!agent) return [];

    return Array.from(this.agents.values())
      .filter(
        (a) =>
          a.taskHash === agent.taskHash &&
          a.sessionId === agent.sessionId,
      )
      .sort((a, b) => a.version - b.version);
  }

  // ── Private helpers ──

  private hashTask(task: string, role?: string): string {
    const normalized = `${(role || "").toLowerCase()}:${task.trim().replace(/\s+/g, " ").toLowerCase()}`;
    return createHash("sha256").update(normalized).digest("hex").slice(0, 16);
  }

  /**
   * Extract file/directory scope from a task description.
   * Looks for common path patterns like src/components/Login.tsx or ./data.json
   */
  private extractScope(task: string): string | undefined {
    // Match file paths (Unix or Windows style)
    const pathMatch = task.match(
      /(?:^|\s|["'`(])([./\\]?(?:[\w.-]+[/\\])+[\w.-]+\.\w+)/,
    );
    if (pathMatch) {
      // Return just the filename or last directory + filename
      const fullPath = pathMatch[1];
      const parts = fullPath.replace(/\\/g, "/").split("/");
      if (parts.length >= 2) {
        return parts.slice(-2).join("/");
      }
      return parts[parts.length - 1];
    }

    // Match directory-like references (src/components, gui/src)
    const dirMatch = task.match(
      /(?:^|\s|["'`(])([./\\]?(?:[\w.-]+[/\\])+[\w.-]+)/,
    );
    if (dirMatch) {
      const fullPath = dirMatch[1];
      const parts = fullPath.replace(/\\/g, "/").split("/");
      if (parts.length >= 2) {
        return parts.slice(-2).join("/");
      }
      return parts[parts.length - 1];
    }

    return undefined;
  }

  /**
   * Generate a human-readable display name for an agent.
   * Pattern: "Role-N (scope)" or "Agent-N" if no role.
   */
  private generateDisplayName(
    role: string | undefined,
    scope: string | undefined,
    sessionId?: string,
    version: number = 1,
  ): string {
    const base = role || "Agent";
    const counterKey = `${sessionId || "global"}:${base}`;

    const count = (this.roleCounters.get(counterKey) || 0) + 1;
    this.roleCounters.set(counterKey, count);

    let name = `${base}-${count}`;
    if (scope) {
      name += ` (${scope})`;
    }
    if (version > 1) {
      name += ` v${version}`;
    }

    return name;
  }
}
