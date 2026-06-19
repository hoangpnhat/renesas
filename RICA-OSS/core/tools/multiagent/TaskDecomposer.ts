import { IDE } from "../../index.js";

export type ComplexityLevel = "simple" | "moderate" | "complex";

export interface SubtaskPlan {
  id: string;
  description: string;
  suggestedRole: string;
  scope: string[];
  estimatedTokens?: number;
  dependsOn: string[];
}

export interface FileOverlap {
  file: string;
  subtaskIds: string[];
}

export interface TaskAnalysis {
  estimatedComplexity: ComplexityLevel;
  suggestedPattern: string;
  subtasks: SubtaskPlan[];
  dependencies: [string, string][]; // [subtaskId, dependsOnSubtaskId]
  warnings: string[];
  fileAnalysis: FileInfo[];
  fileOverlaps: FileOverlap[];
}

interface FileInfo {
  path: string;
  sizeBytes: number;
  estimatedTokens: number;
  lineCount: number;
}

// Role suggestions based on task keywords
const ROLE_KEYWORDS: Record<string, string[]> = {
  Researcher: [
    "find", "search", "look", "explore", "investigate", "read", "list",
    "check", "examine", "analyze", "what", "where", "how", "understand",
  ],
  Implementer: [
    "implement", "create", "write", "add", "build", "make", "change",
    "update", "modify", "edit", "fix", "refactor", "develop", "code",
  ],
  Tester: [
    "test", "verify", "validate", "check", "assert", "run tests",
    "regression", "unit test", "integration test",
  ],
  Reviewer: [
    "review", "audit", "inspect", "quality", "security", "assess",
    "evaluate", "feedback",
  ],
  Analyst: [
    "debug", "diagnose", "trace", "root cause", "issue", "bug",
    "problem", "error", "failure", "crash",
  ],
  Planner: [
    "plan", "design", "architect", "strategy", "approach", "outline",
    "roadmap", "proposal",
  ],
};

// Sequential dependency keywords
const SEQUENTIAL_KEYWORDS = [
  "then", "after", "once", "based on", "using results",
  "when done", "followed by", "next", "subsequently",
];

// Parallel independence keywords
const PARALLEL_KEYWORDS = [
  "and", "also", "simultaneously", "at the same time",
  "in parallel", "independently", "separately",
];

// File path pattern
const FILE_PATH_PATTERN =
  /(?:^|\s|["'`(,])([./\\]?(?:[\w@.-]+[/\\])*[\w@.-]+\.\w{1,10})/g;

// Directory path pattern
const DIR_PATH_PATTERN =
  /(?:^|\s|["'`(,])([./\\]?(?:[\w@.-]+[/\\])+[\w@.-]+)/g;

/**
 * Analyzes a task to determine complexity, suggest decomposition,
 * and check file sizes. Uses heuristics, not LLM calls, for speed.
 */
export class TaskDecomposer {
  /**
   * Analyze a task and return structured decomposition info.
   */
  async analyzeTask(
    task: string,
    ide: IDE,
    additionalPaths?: string[],
  ): Promise<TaskAnalysis> {
    // Extract file paths from task
    const extractedPaths = this.extractPaths(task);
    const allPaths = [...new Set([...extractedPaths, ...(additionalPaths || [])])];

    // Check file sizes
    const fileAnalysis = await this.analyzeFiles(allPaths, ide);

    // Detect complexity
    const complexity = this.estimateComplexity(task, fileAnalysis);

    // Suggest pattern
    const pattern = this.suggestPattern(task);

    // Build subtask plan
    const subtasks = this.decomposeTask(task, fileAnalysis);

    // Extract dependencies
    const dependencies = this.extractDependencies(subtasks);

    // Detect file overlaps between subtasks
    const fileOverlaps = this.detectFileOverlaps(subtasks);

    // Generate warnings (including file overlap warnings)
    const warnings = this.generateWarnings(fileAnalysis, task, fileOverlaps);

    return {
      estimatedComplexity: complexity,
      suggestedPattern: pattern,
      subtasks,
      dependencies,
      warnings,
      fileAnalysis,
      fileOverlaps,
    };
  }

  /**
   * Format the analysis as a readable markdown string for the orchestrator.
   */
  formatAnalysis(analysis: TaskAnalysis): string {
    const sections: string[] = [];

    // Complexity & Pattern
    sections.push(
      `## Task Analysis\n` +
      `**Complexity**: ${analysis.estimatedComplexity}\n` +
      `**Suggested Pattern**: ${analysis.suggestedPattern}`,
    );

    // Action directive: if multiple files, tell the orchestrator to spawn per-file agents
    if (analysis.fileAnalysis.length > 1) {
      const fileList = analysis.fileAnalysis.map((f) => f.path).join(", ");
      sections.push(
        `### ACTION REQUIRED\n` +
        `**${analysis.fileAnalysis.length} files detected.** Spawn one agent per file in your response. ` +
        `Each agent should handle exactly one file. Files: ${fileList}\n\n` +
        `Do NOT process them sequentially — spawn ALL agents NOW in this single response.`,
      );
    }

    // File conflict warnings (prominent, before general warnings)
    if (analysis.fileOverlaps.length > 0) {
      sections.push(
        `### FILE CONFLICTS DETECTED\n` +
        analysis.fileOverlaps
          .map(
            (o) =>
              `- **${o.file}** is targeted by subtasks ${o.subtaskIds.join(", ")}`,
          )
          .join("\n") +
        `\n\n**Action**: Merge conflicting subtasks into one agent, or switch to sequential pattern.`,
      );
    }

    // General warnings (excluding file conflict ones already shown above)
    const generalWarnings = analysis.warnings.filter(
      (w) => !w.startsWith("FILE CONFLICT:"),
    );
    if (generalWarnings.length > 0) {
      sections.push(
        `### Warnings\n` +
        generalWarnings.map((w) => `- ${w}`).join("\n"),
      );
    }

    // File Analysis
    if (analysis.fileAnalysis.length > 0) {
      const fileRows = analysis.fileAnalysis
        .map(
          (f) =>
            `| ${f.path} | ${f.lineCount} lines | ~${f.estimatedTokens} tokens |`,
        )
        .join("\n");
      sections.push(
        `### File Sizes\n` +
        `| File | Lines | Est. Tokens |\n` +
        `|------|-------|-------------|\n` +
        fileRows,
      );
    }

    // Subtask Plan
    if (analysis.subtasks.length > 0) {
      const taskList = analysis.subtasks
        .map((s) => {
          let line = `${s.id}. **[${s.suggestedRole}]** ${s.description}`;
          if (s.scope.length > 0) {
            line += `\n   Scope: ${s.scope.join(", ")}`;
          }
          if (s.dependsOn.length > 0) {
            line += `\n   Depends on: ${s.dependsOn.join(", ")}`;
          }
          if (s.estimatedTokens) {
            line += `\n   Est. tokens: ~${s.estimatedTokens}`;
          }
          return line;
        })
        .join("\n\n");
      sections.push(`### Suggested Subtasks\n\n${taskList}`);
    }

    // Dependencies
    if (analysis.dependencies.length > 0) {
      sections.push(
        `### Dependencies\n` +
        analysis.dependencies.map(([a, b]) => `- ${a} depends on ${b}`).join("\n"),
      );
    }

    return sections.join("\n\n");
  }

  // ── Private helpers ──

  private extractPaths(text: string): string[] {
    const paths = new Set<string>();

    // Extract file paths
    let match;
    const fileRegex = new RegExp(FILE_PATH_PATTERN.source, "g");
    while ((match = fileRegex.exec(text)) !== null) {
      const p = match[1].trim();
      // Filter out obvious non-paths
      if (p.length > 2 && !p.startsWith("http") && !p.startsWith("//")) {
        paths.add(p);
      }
    }

    // Extract directory paths
    const dirRegex = new RegExp(DIR_PATH_PATTERN.source, "g");
    while ((match = dirRegex.exec(text)) !== null) {
      const p = match[1].trim();
      if (p.length > 2 && !p.startsWith("http") && !p.startsWith("//")) {
        paths.add(p);
      }
    }

    return Array.from(paths);
  }

  private async analyzeFiles(paths: string[], ide: IDE): Promise<FileInfo[]> {
    const results: FileInfo[] = [];
    const seen = new Set<string>();

    for (const p of paths) {
      if (seen.has(p)) continue;
      seen.add(p);

      try {
        // First try reading as a file
        const content = await ide.readFile(p);
        if (content) {
          const lines = content.split("\n").length;
          const estimatedTokens = Math.ceil(content.length / 4);
          results.push({
            path: p,
            sizeBytes: content.length,
            estimatedTokens,
            lineCount: lines,
          });
          continue;
        }
      } catch {
        // Not a file — try as directory
      }

      try {
        // Try listing as a directory
        const entries = await ide.listDir(p);
        if (entries && entries.length > 0) {
          for (const [name, fileType] of entries) {
            // fileType 1 = File, 2 = Directory
            if (fileType === 1) {
              const filePath = p.endsWith("/") || p.endsWith("\\")
                ? `${p}${name}`
                : `${p}/${name}`;
              if (seen.has(filePath)) continue;
              seen.add(filePath);

              try {
                const content = await ide.readFile(filePath);
                if (content) {
                  const lines = content.split("\n").length;
                  const estimatedTokens = Math.ceil(content.length / 4);
                  results.push({
                    path: filePath,
                    sizeBytes: content.length,
                    estimatedTokens,
                    lineCount: lines,
                  });
                }
              } catch {
                // Skip unreadable files
              }
            }
          }
        }
      } catch {
        // Not a directory either; skip
      }
    }

    return results;
  }

  private estimateComplexity(
    task: string,
    files: FileInfo[],
  ): ComplexityLevel {
    let score = 0;

    // Task length as proxy for complexity
    const words = task.split(/\s+/).length;
    if (words > 100) score += 2;
    else if (words > 50) score += 1;

    // Number of files involved
    if (files.length > 5) score += 2;
    else if (files.length > 2) score += 1;

    // Total tokens across files
    const totalTokens = files.reduce((sum, f) => sum + f.estimatedTokens, 0);
    if (totalTokens > 50000) score += 2;
    else if (totalTokens > 10000) score += 1;

    // Large individual files
    const hasLargeFile = files.some((f) => f.lineCount > 1000);
    if (hasLargeFile) score += 1;

    // Multiple distinct actions
    const actionCount = (task.match(/\b(and|also|then|after|finally)\b/gi) || []).length;
    if (actionCount >= 3) score += 2;
    else if (actionCount >= 1) score += 1;

    if (score >= 5) return "complex";
    if (score >= 2) return "moderate";
    return "simple";
  }

  private suggestPattern(task: string): string {
    const lower = task.toLowerCase();

    // Check for sequential indicators
    const hasSequential = SEQUENTIAL_KEYWORDS.some((k) => lower.includes(k));

    // Check for parallel indicators
    const hasParallel = PARALLEL_KEYWORDS.some((k) => lower.includes(k));

    // Check for iterative indicators (test-fix cycles)
    if (
      lower.includes("fix") &&
      (lower.includes("test") || lower.includes("verify"))
    ) {
      return "iterative";
    }

    // Check for review/collaborative indicators
    if (
      lower.includes("review") ||
      lower.includes("audit") ||
      lower.includes("production ready")
    ) {
      return "collaborative";
    }

    if (hasSequential && !hasParallel) return "sequential";
    if (hasParallel && !hasSequential) return "parallel";

    return "auto";
  }

  private decomposeTask(task: string, files: FileInfo[]): SubtaskPlan[] {
    const subtasks: SubtaskPlan[] = [];
    let idCounter = 1;

    // Split task by common separators
    const segments = this.splitTaskSegments(task);

    for (const segment of segments) {
      if (segment.trim().length < 10) continue; // Skip very short fragments

      const role = this.suggestRole(segment);
      const scope = this.extractPaths(segment);

      // Check if any scoped files are large and might need splitting
      const scopedFiles = files.filter((f) =>
        scope.some((s) => f.path.includes(s) || s.includes(f.path)),
      );

      const estimatedTokens = scopedFiles.reduce(
        (sum, f) => sum + f.estimatedTokens,
        0,
      );

      subtasks.push({
        id: `T${idCounter++}`,
        description: segment.trim(),
        suggestedRole: role,
        scope,
        estimatedTokens: estimatedTokens > 0 ? estimatedTokens : undefined,
        dependsOn: [],
      });
    }

    // If no decomposition possible, return the whole task as one subtask
    if (subtasks.length === 0) {
      subtasks.push({
        id: "T1",
        description: task,
        suggestedRole: this.suggestRole(task),
        scope: this.extractPaths(task),
        dependsOn: [],
      });
    }

    return subtasks;
  }

  private splitTaskSegments(task: string): string[] {
    // Try numbered list first (1. xxx 2. yyy)
    const numbered = task.match(/\d+\.\s+[^\d]+/g);
    if (numbered && numbered.length > 1) {
      return numbered.map((s) => s.replace(/^\d+\.\s+/, ""));
    }

    // Try bullet points
    const bullets = task.match(/[-*]\s+[^-*]+/g);
    if (bullets && bullets.length > 1) {
      return bullets.map((s) => s.replace(/^[-*]\s+/, ""));
    }

    // Try "and"/"then" splitting for compound sentences
    const conjunctions = task.split(/\b(?:and then|then|after that|also)\b/i);
    if (conjunctions.length > 1) {
      return conjunctions;
    }

    // Return as single segment
    return [task];
  }

  private suggestRole(text: string): string {
    const lower = text.toLowerCase();
    let bestRole = "Researcher"; // Default
    let bestScore = 0;

    for (const [role, keywords] of Object.entries(ROLE_KEYWORDS)) {
      const score = keywords.filter((kw) => lower.includes(kw)).length;
      if (score > bestScore) {
        bestScore = score;
        bestRole = role;
      }
    }

    return bestRole;
  }

  private extractDependencies(subtasks: SubtaskPlan[]): [string, string][] {
    const deps: [string, string][] = [];

    // Simple heuristic: if subtasks mention "based on" or "using results from"
    // and reference another subtask's scope, add a dependency
    for (let i = 1; i < subtasks.length; i++) {
      const desc = subtasks[i].description.toLowerCase();
      if (
        desc.includes("based on") ||
        desc.includes("using results") ||
        desc.includes("from the") ||
        desc.includes("fix") // Fix tasks usually depend on analysis
      ) {
        // Depend on the previous subtask
        deps.push([subtasks[i].id, subtasks[i - 1].id]);
        subtasks[i].dependsOn.push(subtasks[i - 1].id);
      }
    }

    return deps;
  }

  private detectFileOverlaps(subtasks: SubtaskPlan[]): FileOverlap[] {
    const fileMap = new Map<string, string[]>();
    for (const st of subtasks) {
      for (const path of st.scope) {
        const norm = path.replace(/\\/g, "/").toLowerCase();
        const existing = fileMap.get(norm) || [];
        existing.push(st.id);
        fileMap.set(norm, existing);
      }
    }
    return Array.from(fileMap.entries())
      .filter(([_, ids]) => ids.length > 1)
      .map(([file, subtaskIds]) => ({ file, subtaskIds }));
  }

  private generateWarnings(
    files: FileInfo[],
    task: string,
    fileOverlaps: FileOverlap[],
  ): string[] {
    const warnings: string[] = [];

    // File overlap warnings
    for (const overlap of fileOverlaps) {
      warnings.push(
        `FILE CONFLICT: ${overlap.file} is targeted by subtasks ${overlap.subtaskIds.join(", ")}. ` +
        `Merge these into ONE agent or run them sequentially to avoid overwrites.`,
      );
    }

    for (const f of files) {
      if (f.lineCount > 2000) {
        warnings.push(
          `File "${f.path}" is ${f.lineCount} lines (~${f.estimatedTokens} tokens). ` +
          `Consider splitting work across multiple agents by line range or section.`,
        );
      } else if (f.lineCount > 1000) {
        warnings.push(
          `File "${f.path}" is ${f.lineCount} lines. ` +
          `Consider assigning a focused scope to the agent working on it.`,
        );
      }
    }

    // Check total scope
    const totalTokens = files.reduce((sum, f) => sum + f.estimatedTokens, 0);
    if (totalTokens > 100000) {
      warnings.push(
        `Total estimated tokens across all files: ~${totalTokens}. ` +
        `This exceeds typical context limits. Split work across multiple agents.`,
      );
    }

    // Check for potentially dangerous operations
    const lower = task.toLowerCase();
    if (lower.includes("delete") || lower.includes("remove") || lower.includes("drop")) {
      warnings.push(
        `Task mentions destructive operations (delete/remove/drop). Ensure agent has appropriate scope limits.`,
      );
    }

    return warnings;
  }
}
