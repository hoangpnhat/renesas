/**
 * Per-session file edit tracker for multi-agent conflict detection.
 * Records which agents edit which files, detects when multiple agents
 * write to the same file (potential overwrite conflicts).
 *
 * Follows the same singleton-per-session pattern as ContextBridge.
 */

export interface FileEdit {
  filepath: string; // Normalized path (forward slashes, lowercase)
  agentId: string;
  agentName: string;
  timestamp: number;
  contentHashBefore: string;
  contentHashAfter: string;
}

export interface FileConflict {
  filepath: string;
  edits: FileEdit[]; // All edits to this file, sorted by timestamp
}

export class FileTracker {
  private static instances = new Map<string, FileTracker>();

  private edits: FileEdit[] = [];

  /**
   * Get or create a FileTracker for a session.
   */
  static forSession(sessionId: string): FileTracker {
    let tracker = FileTracker.instances.get(sessionId);
    if (!tracker) {
      tracker = new FileTracker();
      FileTracker.instances.set(sessionId, tracker);
    }
    return tracker;
  }

  /**
   * Destroy the FileTracker for a session (cleanup on session kill).
   */
  static destroySession(sessionId: string): void {
    FileTracker.instances.delete(sessionId);
  }

  /**
   * Record a file edit made by an agent.
   */
  recordEdit(edit: FileEdit): void {
    this.edits.push(edit);
  }

  /**
   * Detect files edited by multiple different agents.
   * Returns conflicts sorted by filepath, with edits sorted by timestamp.
   */
  detectConflicts(): FileConflict[] {
    // Group edits by filepath
    const byFile = new Map<string, FileEdit[]>();
    for (const edit of this.edits) {
      const existing = byFile.get(edit.filepath) || [];
      existing.push(edit);
      byFile.set(edit.filepath, existing);
    }

    const conflicts: FileConflict[] = [];
    for (const [filepath, edits] of byFile) {
      // Only a conflict if 2+ different agents edited the same file
      const uniqueAgents = new Set(edits.map((e) => e.agentId));
      if (uniqueAgents.size < 2) continue;

      conflicts.push({
        filepath,
        edits: edits.sort((a, b) => a.timestamp - b.timestamp),
      });
    }

    return conflicts.sort((a, b) => a.filepath.localeCompare(b.filepath));
  }
}
