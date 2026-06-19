import {
  ArrowsPointingInIcon,
  ArrowsPointingOutIcon,
  BeakerIcon,
  ChartBarIcon,
  CheckCircleIcon,
  ChevronRightIcon,
  ClipboardDocumentListIcon,
  ClockIcon,
  CodeBracketIcon,
  CpuChipIcon,
  ExclamationCircleIcon,
  EyeIcon,
  MagnifyingGlassIcon,
  StopCircleIcon,
  XMarkIcon,
} from "@heroicons/react/24/outline";
import { SubAgentProgress, ThinkingEntry, ToolAction } from "core";
import {
  useCallback,
  useContext,
  useEffect,
  useLayoutEffect,
  useMemo,
  useRef,
  useState,
} from "react";
import { IdeMessengerContext } from "../../context/IdeMessenger";
import { useAppSelector } from "../../redux/hooks";
import Spinner from "../../components/gui/Spinner";
import StyledMarkdownPreview from "../../components/StyledMarkdownPreview";

// ─── Role icon mapping ──────────────────────────────────────

function RoleIcon({
  role,
  className,
}: {
  role?: string;
  className?: string;
}) {
  const cls = className || "h-3.5 w-3.5";
  switch (role) {
    case "Researcher":
      return <MagnifyingGlassIcon className={cls} />;
    case "Implementer":
      return <CodeBracketIcon className={cls} />;
    case "Tester":
      return <BeakerIcon className={cls} />;
    case "Reviewer":
      return <EyeIcon className={cls} />;
    case "Analyst":
      return <ChartBarIcon className={cls} />;
    case "Planner":
      return <ClipboardDocumentListIcon className={cls} />;
    default:
      return <CpuChipIcon className={cls} />;
  }
}

// ─── Token formatting ───────────────────────────────────────

function formatTokenCount(n: number): string {
  if (n >= 1_000_000) return `${(n / 1_000_000).toFixed(1)}M`;
  if (n >= 1_000) return `${(n / 1_000).toFixed(1)}k`;
  return `${n}`;
}

// ─── Status helpers ──────────────────────────────────────────

function StatusDot({ status }: { status: SubAgentProgress["status"] }) {
  const base = "inline-block h-2 w-2 rounded-full flex-shrink-0";
  switch (status) {
    case "running":
      return <span className={`${base} animate-pulse bg-blue-400`} />;
    case "completed":
      return <span className={`${base} bg-green-400`} />;
    case "errored":
      return <span className={`${base} bg-red-400`} />;
    case "canceled":
      return <span className={`${base} bg-yellow-400`} />;
    default:
      return <span className={`${base} bg-zinc-500`} />;
  }
}

function StatusBadge({ status }: { status: SubAgentProgress["status"] }) {
  switch (status) {
    case "running":
      return (
        <span className="inline-flex items-center gap-1 rounded-full bg-blue-500/20 px-2 py-0.5 text-[10px] font-medium text-blue-400">
          <span className="h-1.5 w-1.5 animate-pulse rounded-full bg-blue-400" />
          Running
        </span>
      );
    case "completed":
      return (
        <span className="inline-flex items-center gap-1 rounded-full bg-green-500/20 px-2 py-0.5 text-[10px] font-medium text-green-400">
          <CheckCircleIcon className="h-3 w-3" />
          Completed
        </span>
      );
    case "errored":
      return (
        <span className="inline-flex items-center gap-1 rounded-full bg-red-500/20 px-2 py-0.5 text-[10px] font-medium text-red-400">
          <ExclamationCircleIcon className="h-3 w-3" />
          Error
        </span>
      );
    case "canceled":
      return (
        <span className="inline-flex items-center gap-1 rounded-full bg-yellow-500/20 px-2 py-0.5 text-[10px] font-medium text-yellow-400">
          <StopCircleIcon className="h-3 w-3" />
          Canceled
        </span>
      );
    default:
      return null;
  }
}

function formatDuration(ms: number): string {
  if (ms < 1000) return `${ms}ms`;
  return `${(ms / 1000).toFixed(1)}s`;
}

function formatToolName(name: string): string {
  return name.replace(/^builtin_/, "").replace(/_/g, " ");
}

function getAgentDisplayName(
  progress: SubAgentProgress,
  index: number,
): string {
  // Prefer the auto-generated displayName from AgentManager
  if (progress.displayName) return progress.displayName;
  if (progress.role) return progress.role;
  if (progress.description && progress.description.length <= 20)
    return progress.description;
  return `Agent-${index + 1}`;
}

function buildAgentMarkdown(progress: SubAgentProgress, index: number): string {
  const lines: string[] = [];
  const name = getAgentDisplayName(progress, index);
  const durationMs = progress.completedAt && progress.startedAt
    ? progress.completedAt - progress.startedAt
    : undefined;

  lines.push(`# Agent: ${name}`);
  lines.push("");
  lines.push(`**Status:** ${progress.status}`);
  if (durationMs != null) lines.push(`**Duration:** ${formatDuration(durationMs)}`);
  lines.push(`**Steps:** ${progress.stepsCompleted}`);
  if (progress.tokenUsage && progress.tokenUsage.totalTokens > 0) {
    const t = progress.tokenUsage;
    lines.push(`**Tokens:** In: ${formatTokenCount(t.promptTokens)} | Out: ${formatTokenCount(t.completionTokens)} | Total: ${formatTokenCount(t.totalTokens)}`);
    if ((t as any).budget > 0) {
      const pct = Math.round((t.totalTokens / (t as any).budget) * 100);
      lines.push(`**Budget:** ${formatTokenCount(t.totalTokens)} / ${formatTokenCount((t as any).budget)} (${pct}%)`);
    }
  }
  if (progress.role) lines.push(`**Role:** ${progress.role}`);

  lines.push("");
  lines.push("---");
  lines.push("");
  lines.push("## Task");
  lines.push("");
  lines.push(progress.task);

  if (progress.toolActions && progress.toolActions.length > 0) {
    lines.push("");
    lines.push("---");
    lines.push("");
    lines.push("## Steps");
    lines.push("");
    lines.push("| # | Action | Args | Duration | Status |");
    lines.push("|---|--------|------|----------|--------|");
    progress.toolActions.forEach((action, i) => {
      const dur = action.durationMs != null ? formatDuration(action.durationMs) : "—";
      const status = action.status === "completed" ? "✓" : action.status === "errored" ? "✗" : "⟳";
      const args = action.argSummary ? action.argSummary.replace(/\|/g, "\\|") : "—";
      lines.push(`| ${i + 1} | ${action.toolName} | ${args} | ${dur} | ${status} |`);
    });
  }

  if (progress.result) {
    lines.push("");
    lines.push("---");
    lines.push("");
    lines.push("## Result");
    lines.push("");
    lines.push(progress.result);
  }

  if (progress.error) {
    lines.push("");
    lines.push("---");
    lines.push("");
    lines.push("## Error");
    lines.push("");
    lines.push(`\`\`\`\n${progress.error}\n\`\`\``);
  }

  return lines.join("\n");
}

// ─── Tool Timeline (used in detail view) ─────────────────────

function ToolTimeline({ actions }: { actions: ToolAction[] }) {
  if (!actions || actions.length === 0) return null;

  return (
    <div className="space-y-0.5">
      <div className="mb-1 text-[10px] font-medium uppercase tracking-wider text-zinc-500">
        Tool Steps
      </div>
      {actions.map((action, i) => (
        <div
          key={i}
          className="flex items-start gap-1.5 py-0.5 text-[11px]"
        >
          <span className="mt-0.5 flex-shrink-0">
            {action.status === "completed" ? (
              <CheckCircleIcon className="h-3 w-3 text-green-500" />
            ) : action.status === "errored" ? (
              <ExclamationCircleIcon className="h-3 w-3 text-red-400" />
            ) : (
              <Spinner />
            )}
          </span>
          <div className="min-w-0 flex-1 text-zinc-300">
            <span className="font-medium">{formatToolName(action.toolName)}</span>
            <span className="ml-1.5 flex-shrink-0 text-zinc-500">
              {action.durationMs != null
                ? formatDuration(action.durationMs)
                : "..."}
            </span>
            {action.argSummary && (
              <div className="mt-0.5 break-all text-zinc-500">{action.argSummary}</div>
            )}
          </div>
        </div>
      ))}
    </div>
  );
}

// ─── Thinking Log Component ──────────────────────────────────

function ThinkingLog({ entries }: { entries: ThinkingEntry[] }) {
  const [expandedIdx, setExpandedIdx] = useState<number | null>(null);

  if (!entries || entries.length === 0) return null;

  return (
    <div>
      <div className="mb-1 text-[10px] font-medium uppercase tracking-wider text-zinc-500">
        Thinking ({entries.length} step{entries.length !== 1 ? "s" : ""})
      </div>
      <div className="space-y-1">
        {entries.map((entry, i) => (
          <div
            key={i}
            className="overflow-hidden rounded-lg border border-solid border-zinc-700/30 bg-zinc-800/40"
          >
            <div
              className="flex cursor-pointer items-center gap-1.5 px-2.5 py-1.5 transition-colors hover:bg-zinc-700/30"
              onClick={() => setExpandedIdx(expandedIdx === i ? null : i)}
            >
              <ChevronRightIcon
                className={`h-2.5 w-2.5 flex-shrink-0 text-zinc-500 transition-transform duration-150 ${
                  expandedIdx === i ? "rotate-90" : ""
                }`}
              />
              <span className="text-[10px] font-medium text-zinc-400">
                Step {entry.iteration}
              </span>
              <span className="min-w-0 flex-1 truncate text-[10px] text-zinc-500">
                {entry.content.slice(0, 80)}
                {entry.content.length > 80 ? "..." : ""}
              </span>
            </div>
            {expandedIdx === i && (
              <div className="max-h-48 overflow-y-auto border-t border-zinc-700/30 p-2.5 text-xs thin-scrollbar">
                <StyledMarkdownPreview source={entry.content} />
              </div>
            )}
          </div>
        ))}
      </div>
    </div>
  );
}

// ─── Agent Detail Overlay ────────────────────────────────────

function AgentDetailOverlay({
  progress,
  index,
  onClose,
  onKill,
}: {
  progress: SubAgentProgress;
  index: number;
  onClose: () => void;
  onKill?: () => void;
}) {
  const ideMessenger = useContext(IdeMessengerContext);
  const name = getAgentDisplayName(progress, index);
  const elapsed =
    progress.startedAt && progress.completedAt
      ? formatDuration(progress.completedAt - progress.startedAt)
      : progress.startedAt
        ? formatDuration(Date.now() - progress.startedAt)
        : undefined;

  return (
    <div
      className="absolute inset-0 z-50 flex items-start justify-center overflow-y-auto bg-black/60 backdrop-blur-sm"
      onClick={(e) => {
        if (e.target === e.currentTarget) onClose();
      }}
    >
      <div className="mx-2 my-4 w-full max-w-md overflow-hidden rounded-xl border border-solid border-zinc-600/50 bg-zinc-900 shadow-2xl">
        {/* Detail header */}
        <div className="flex items-center gap-2 border-b border-zinc-700/50 px-4 py-3">
          <RoleIcon role={progress.role} className="h-5 w-5 text-zinc-300" />
          <div className="min-w-0 flex-1">
            <div className="text-sm font-semibold text-zinc-100">{name}</div>
            {progress.description && progress.description !== name && (
              <div className="truncate text-[11px] text-zinc-500">
                {progress.description}
              </div>
            )}
          </div>
          <StatusBadge status={progress.status} />
          {progress.status === "running" && onKill && (
            <button
              onClick={onKill}
              className="ml-1 flex items-center gap-1 rounded-md bg-red-500/10 px-2 py-1 text-[10px] font-medium text-red-400 transition-all hover:bg-red-500/25"
              title="Cancel this agent"
            >
              <StopCircleIcon className="h-3.5 w-3.5" />
              Cancel
            </button>
          )}
          <button
            onClick={onClose}
            className="ml-1 rounded-md bg-zinc-800/60 p-1.5 text-zinc-500 transition-all hover:bg-zinc-700 hover:text-zinc-300"
          >
            <XMarkIcon className="h-4 w-4" />
          </button>
        </div>

        {/* Detail body */}
        <div className="space-y-3 px-4 py-3">
          {/* Status + metrics bar */}
          <div className="flex items-center gap-3 text-[11px] text-zinc-400">
            <span>
              {progress.stepsCompleted} step
              {progress.stepsCompleted !== 1 ? "s" : ""} completed
            </span>
            {elapsed && <span>{elapsed}</span>}
            {progress.status === "running" && progress.currentAction && (
              <span className="flex items-center gap-1 text-blue-400">
                <Spinner />
                {progress.currentAction}
              </span>
            )}
          </div>

          {/* Token usage */}
          {progress.tokenUsage && progress.tokenUsage.totalTokens > 0 && (
            <div className="flex flex-wrap items-center gap-3 text-[11px] text-zinc-400">
              <span>In: {formatTokenCount(progress.tokenUsage.promptTokens)}</span>
              <span>Out: {formatTokenCount(progress.tokenUsage.completionTokens)}</span>
              {(progress.tokenUsage as any).reasoningTokens > 0 && (
                <span className="text-purple-400" title="Reasoning/thinking tokens">
                  R: {formatTokenCount((progress.tokenUsage as any).reasoningTokens)}
                </span>
              )}
              {((progress.tokenUsage as any).cacheReadTokens > 0 || (progress.tokenUsage as any).cacheCreationTokens > 0) && (
                <span
                  className="text-sky-400"
                  title={`Cache read: ${formatTokenCount((progress.tokenUsage as any).cacheReadTokens)} (reused)\nCache write: ${formatTokenCount((progress.tokenUsage as any).cacheCreationTokens)} (new)`}
                >
                  Cache: {formatTokenCount((progress.tokenUsage as any).cacheReadTokens)}/{formatTokenCount((progress.tokenUsage as any).cacheCreationTokens)}
                </span>
              )}
              <span className="text-zinc-300">
                Total: {formatTokenCount(progress.tokenUsage.totalTokens)} tokens
              </span>
              {(progress.tokenUsage as any).budget > 0 && (
                <span
                  className={`text-[9px] font-medium ${
                    progress.tokenUsage.totalTokens / (progress.tokenUsage as any).budget > 0.8
                      ? "text-red-400"
                      : progress.tokenUsage.totalTokens / (progress.tokenUsage as any).budget > 0.5
                        ? "text-amber-400"
                        : "text-emerald-400"
                  }`}
                  title={`Budget: ${formatTokenCount(progress.tokenUsage.totalTokens)} / ${formatTokenCount((progress.tokenUsage as any).budget)} tokens used`}
                >
                  Budget: {Math.round((progress.tokenUsage.totalTokens / (progress.tokenUsage as any).budget) * 100)}%
                </span>
              )}
              <span
                className={`text-[9px] font-medium ${(progress.tokenUsage as any).isActualUsage ? "text-emerald-500" : "text-zinc-600"}`}
                title={(progress.tokenUsage as any).isActualUsage ? "Actual usage from API" : "Estimated via tiktoken"}
              >
                {(progress.tokenUsage as any).isActualUsage ? "API" : "~est"}
              </span>
            </div>
          )}

          {/* Task */}
          <div
            className="cursor-pointer rounded-lg p-1 transition-colors [&:hover_.task-label]:text-orange-400 [&:hover_.task-label]:drop-shadow-[0_0_4px_rgba(251,146,60,0.5)]"
            onClick={() => {
              const id = progress.subAgentId?.slice(0, 8) || "unknown";
              const title = `Agent: ${name} (${id}).md`;
              void ideMessenger.post("showVirtualFile", { name: title, content: buildAgentMarkdown(progress, index) });
            }}
          >
            <div className="task-label mb-1 text-[10px] font-medium uppercase tracking-wider text-zinc-500 transition-all">
              Task
            </div>
            <div className="max-h-64 overflow-y-auto rounded-lg bg-zinc-800/60 p-3 text-xs thin-scrollbar">
              <StyledMarkdownPreview source={progress.task} />
            </div>
          </div>

          {/* Thinking log */}
          {progress.thinkingLog && progress.thinkingLog.length > 0 && (
            <ThinkingLog entries={progress.thinkingLog} />
          )}

          {/* Tool timeline */}
          {progress.toolActions && progress.toolActions.length > 0 && (
            <div>
              <ToolTimeline actions={progress.toolActions} />
            </div>
          )}

          {/* Result */}
          {progress.result && (
            <div
              className="cursor-pointer rounded-lg p-1 transition-colors [&:hover_.result-label]:text-orange-400 [&:hover_.result-label]:drop-shadow-[0_0_4px_rgba(251,146,60,0.5)]"
              onClick={() => {
                const id = progress.subAgentId?.slice(0, 8) || "unknown";
                const title = `Agent: ${name} (${id}).md`;
                void ideMessenger.post("showVirtualFile", { name: title, content: buildAgentMarkdown(progress, index) });
              }}
            >
              <div className="result-label mb-1 text-[10px] font-medium uppercase tracking-wider text-zinc-500 transition-all">
                Result
              </div>
              <div className="max-h-64 overflow-y-auto rounded-lg bg-zinc-800/60 p-3 text-xs thin-scrollbar">
                <StyledMarkdownPreview source={progress.result} />
              </div>
            </div>
          )}

          {/* Error */}
          {progress.error && (
            <div>
              <div className="mb-1 text-[10px] font-medium uppercase tracking-wider text-red-400">
                Error
              </div>
              <div className="rounded-lg bg-red-900/20 p-3 text-xs text-red-300">
                {progress.error}
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}

// ─── Orchestrator Detail Overlay ─────────────────────────────

function OrchestratorDetailOverlay({
  agents,
  onClose,
}: {
  agents: SubAgentProgress[];
  onClose: () => void;
}) {
  const totalSteps = agents.reduce((sum, a) => sum + a.stepsCompleted, 0);
  const completed = agents.filter((a) => a.status === "completed").length;
  const running = agents.filter((a) => a.status === "running").length;
  const errored = agents.filter((a) => a.status === "errored").length;

  const earliestStart = Math.min(...agents.map((a) => a.startedAt || Infinity));
  const latestEnd = Math.max(...agents.map((a) => a.completedAt || 0));
  const allDone = agents.every(
    (a) => a.status === "completed" || a.status === "errored" || a.status === "canceled",
  );
  const elapsed =
    earliestStart < Infinity
      ? formatDuration((allDone && latestEnd ? latestEnd : Date.now()) - earliestStart)
      : undefined;

  return (
    <div
      className="absolute inset-0 z-50 flex items-start justify-center overflow-y-auto bg-black/60 backdrop-blur-sm"
      onClick={(e) => {
        if (e.target === e.currentTarget) onClose();
      }}
    >
      <div className="mx-2 my-4 w-full max-w-md overflow-hidden rounded-xl border border-solid border-zinc-600/50 bg-zinc-900 shadow-2xl">
        {/* Header */}
        <div className="flex items-center gap-2 border-b border-zinc-700/50 px-4 py-3">
          <CpuChipIcon className="h-5 w-5 text-zinc-300" />
          <div className="min-w-0 flex-1">
            <div className="text-sm font-semibold text-zinc-100">
              Orchestrator
            </div>
            <div className="text-[11px] text-zinc-500">
              Multi-Agent Coordinator
            </div>
          </div>
          {allDone ? (
            <StatusBadge status="completed" />
          ) : running > 0 ? (
            <StatusBadge status="running" />
          ) : errored > 0 ? (
            <StatusBadge status="errored" />
          ) : null}
          <button
            onClick={onClose}
            className="ml-1 rounded-md bg-zinc-800/60 p-1.5 text-zinc-500 transition-all hover:bg-zinc-700 hover:text-zinc-300"
          >
            <XMarkIcon className="h-4 w-4" />
          </button>
        </div>

        {/* Body */}
        <div className="space-y-3 px-4 py-3">
          {/* Summary metrics */}
          <div className="grid grid-cols-2 gap-2">
            <div className="rounded-lg bg-zinc-800/60 px-3 py-2 text-center">
              <div className="text-lg font-bold text-zinc-100">
                {agents.length}
              </div>
              <div className="text-[10px] text-zinc-500">Agents Spawned</div>
            </div>
            <div className="rounded-lg bg-zinc-800/60 px-3 py-2 text-center">
              <div className="text-lg font-bold text-zinc-100">
                {totalSteps}
              </div>
              <div className="text-[10px] text-zinc-500">Total Steps</div>
            </div>
            <div className="rounded-lg bg-zinc-800/60 px-3 py-2 text-center">
              <div className="text-lg font-bold text-green-400">
                {completed}/{agents.length}
              </div>
              <div className="text-[10px] text-zinc-500">Completed</div>
            </div>
            <div className="rounded-lg bg-zinc-800/60 px-3 py-2 text-center">
              <div className="text-lg font-bold text-zinc-100">
                {elapsed || "—"}
              </div>
              <div className="text-[10px] text-zinc-500">Total Time</div>
            </div>
          </div>

          {/* Session Token Budget */}
          {(() => {
            const SESSION_BUDGET = 400_000;
            let totalTok = 0;
            for (const a of agents) {
              const tu = a.tokenUsage;
              if (tu) totalTok += (tu.promptTokens || 0) + (tu.completionTokens || 0);
            }
            const pct = (totalTok / SESSION_BUDGET) * 100;
            const barColor = pct >= 85 ? "bg-red-500" : pct >= 70 ? "bg-amber-500" : pct >= 50 ? "bg-yellow-500" : "bg-emerald-500";
            const textColor = pct >= 85 ? "text-red-400" : pct >= 70 ? "text-amber-400" : "text-zinc-300";
            return (
              <div className="rounded-lg bg-zinc-800/60 px-3 py-2">
                <div className="mb-1.5 flex items-center justify-between">
                  <span className="text-[10px] font-medium uppercase tracking-wider text-zinc-500">Session Token Budget</span>
                  <span className={`text-[11px] font-mono tabular-nums font-medium ${textColor}`}>
                    {formatTokenCount(totalTok)} / {formatTokenCount(SESSION_BUDGET)}
                  </span>
                </div>
                <div className="h-2 w-full overflow-hidden rounded-full bg-zinc-700/60">
                  <div
                    className={`h-full rounded-full transition-all duration-500 ${barColor}`}
                    style={{ width: `${Math.min(pct, 100)}%` }}
                  />
                </div>
                <div className="mt-1 text-right text-[9px] text-zinc-500">
                  {pct >= 100
                    ? <span className="font-semibold text-red-400">Budget exceeded — no new agents will be spawned</span>
                    : <>
                        {Math.round(pct)}% used
                        {pct >= 85 && " — budget nearly exhausted"}
                        {pct >= 70 && pct < 85 && " — approaching limit"}
                      </>
                  }
                </div>
              </div>
            );
          })()}

          {/* Agent breakdown */}
          <div>
            <div className="mb-1 text-[10px] font-medium uppercase tracking-wider text-zinc-500">
              Agent Breakdown
            </div>
            <div className="space-y-1">
              {agents.map((agent, i) => {
                const name = getAgentDisplayName(agent, i);
                const agentElapsed =
                  agent.startedAt && agent.completedAt
                    ? formatDuration(agent.completedAt - agent.startedAt)
                    : agent.startedAt
                      ? formatDuration(Date.now() - agent.startedAt)
                      : undefined;
                return (
                  <div
                    key={agent.subAgentId}
                    className="flex items-center gap-2 rounded-lg bg-zinc-800/40 px-3 py-2"
                  >
                    <RoleIcon
                      role={agent.role}
                      className="h-3.5 w-3.5 flex-shrink-0 text-zinc-400"
                    />
                    <span className="min-w-0 flex-1 text-xs font-medium text-zinc-200">
                      {name}
                    </span>
                    <span className="text-[10px] text-zinc-500">
                      {agent.stepsCompleted} steps
                    </span>
                    {agentElapsed && (
                      <span className="text-[10px] text-zinc-500">
                        {agentElapsed}
                      </span>
                    )}
                    <StatusDot status={agent.status} />
                  </div>
                );
              })}
            </div>
          </div>

          {/* Errors summary */}
          {errored > 0 && (
            <div>
              <div className="mb-1 text-[10px] font-medium uppercase tracking-wider text-red-400">
                Errors
              </div>
              {agents
                .filter((a) => a.status === "errored" && a.error)
                .map((a, i) => (
                  <div
                    key={i}
                    className="mb-1 rounded-lg bg-red-900/20 p-2 text-[11px] text-red-300"
                  >
                    <span className="font-medium">
                      {getAgentDisplayName(a, i)}:
                    </span>{" "}
                    {a.error}
                  </div>
                ))}
            </div>
          )}
        </div>
      </div>
    </div>
  );
}

// ─── Agent Card (metrics only, clickable) ────────────────────

function AgentCard({
  progress,
  index,
  nodeRef,
  onClick,
  onKill,
  isExpanded,
  onToggleExpand,
}: {
  progress: SubAgentProgress;
  index: number;
  nodeRef: (el: HTMLDivElement | null) => void;
  onClick: () => void;
  onKill?: () => void;
  isExpanded?: boolean;
  onToggleExpand?: () => void;
}) {
  const ideMessenger = useContext(IdeMessengerContext);
  const name = getAgentDisplayName(progress, index);
  const elapsed =
    progress.startedAt && progress.completedAt
      ? formatDuration(progress.completedAt - progress.startedAt)
      : progress.startedAt
        ? formatDuration(Date.now() - progress.startedAt)
        : undefined;

  const borderColor =
    progress.status === "running"
      ? "border-blue-500/40"
      : progress.status === "completed"
        ? "border-green-500/30"
        : progress.status === "errored"
          ? "border-red-500/30"
          : "border-zinc-700/50";

  return (
    <div
      ref={nodeRef}
      className={`${isExpanded ? "w-full max-w-sm" : "w-36"} cursor-pointer overflow-hidden rounded-lg border border-solid ${borderColor} bg-zinc-900/50 transition-all duration-200 hover:bg-zinc-800/60 hover:shadow-lg`}
    >
      {/* Card header: expand chevron + role icon + name + status + kill */}
      <div className="flex items-center gap-1.5 px-2.5 py-2" onClick={onClick}>
        {onToggleExpand && (
          <button
            onClick={(e) => {
              e.stopPropagation();
              onToggleExpand();
            }}
            className="flex-shrink-0 rounded-md bg-zinc-800/60 p-1 text-zinc-500 transition-all hover:bg-zinc-700/80 hover:text-zinc-300"
          >
            <ChevronRightIcon
              className={`h-2.5 w-2.5 transition-transform duration-150 ${
                isExpanded ? "rotate-90" : ""
              }`}
            />
          </button>
        )}
        <RoleIcon
          role={progress.role}
          className="h-3.5 w-3.5 flex-shrink-0 text-zinc-400"
        />
        <span className="min-w-0 flex-1 truncate text-[11px] font-medium text-zinc-200">
          {name}
        </span>
        <StatusDot status={progress.status} />
        {progress.status === "running" && onKill && (
          <button
            onClick={(e) => {
              e.stopPropagation();
              onKill();
            }}
            className="ml-0.5 flex items-center gap-0.5 rounded-md bg-zinc-800/60 px-1.5 py-0.5 text-[9px] font-medium text-zinc-500 transition-all hover:bg-red-500/20 hover:text-red-400"
            title="Cancel this agent"
          >
            <StopCircleIcon className="h-2.5 w-2.5" />
          </button>
        )}
      </div>

      {/* Metrics line */}
      <div className="border-t border-zinc-700/30 px-2 py-1" onClick={onClick}>
        <div className="flex items-center justify-between">
          <span className="text-[10px] text-zinc-500">
            {progress.stepsCompleted} step
            {progress.stepsCompleted !== 1 ? "s" : ""}
            {progress.tokenUsage && progress.tokenUsage.totalTokens > 0 && (
              <> &middot; {formatTokenCount(progress.tokenUsage.totalTokens)}
                {(progress.tokenUsage as any).budget > 0
                  ? <>/{formatTokenCount((progress.tokenUsage as any).budget)} tok</>
                  : <> tok</>
                }
                <span
                  className={`ml-0.5 ${(progress.tokenUsage as any).isActualUsage ? "text-emerald-500" : "text-zinc-600"}`}
                  title={(progress.tokenUsage as any).isActualUsage ? "Actual API usage" : "Estimated (tiktoken)"}
                >
                  {(progress.tokenUsage as any).isActualUsage ? "API" : "~est"}
                </span>
              </>
            )}
          </span>
          {elapsed && (
            <span className="text-[10px] text-zinc-500">{elapsed}</span>
          )}
        </div>
      </div>

      {/* Current action */}
      <div className="border-t border-zinc-700/30 px-2 py-1" onClick={onClick}>
        {progress.status === "running" && progress.currentAction ? (
          <div className="flex items-center gap-1">
            <span className="flex-shrink-0">
              <Spinner />
            </span>
            <span className="truncate text-[10px] text-zinc-400">
              {progress.currentAction}
            </span>
          </div>
        ) : progress.status === "completed" ? (
          <div className="flex items-center gap-1">
            <CheckCircleIcon className="h-3 w-3 flex-shrink-0 text-green-500" />
            <span className="text-[10px] text-green-400">Done</span>
          </div>
        ) : progress.status === "errored" ? (
          <div className="flex items-center gap-1">
            <ExclamationCircleIcon className="h-3 w-3 flex-shrink-0 text-red-400" />
            <span className="truncate text-[10px] text-red-400">
              {progress.error || "Error"}
            </span>
          </div>
        ) : progress.status === "canceled" ? (
          <div className="flex items-center gap-1">
            <StopCircleIcon className="h-3 w-3 flex-shrink-0 text-yellow-400" />
            <span className="text-[10px] text-yellow-400">Canceled</span>
          </div>
        ) : (
          <span className="text-[10px] text-zinc-500">Waiting...</span>
        )}
      </div>

      {/* ── Expanded detail sections ── */}
      {isExpanded && (
        <div className="space-y-2 border-t border-zinc-700/30 px-2 py-2">
          {/* Task */}
          <div
            className="cursor-pointer rounded-lg p-1 transition-colors [&:hover_.task-label]:text-orange-400 [&:hover_.task-label]:drop-shadow-[0_0_4px_rgba(251,146,60,0.5)]"
            onClick={(e) => {
              e.stopPropagation();
              const id = progress.subAgentId?.slice(0, 8) || "unknown";
              const title = `Agent: ${name} (${id}).md`;
              void ideMessenger.post("showVirtualFile", { name: title, content: buildAgentMarkdown(progress, index) });
            }}
          >
            <div className="task-label mb-1 text-[10px] font-medium uppercase tracking-wider text-zinc-500 transition-all">
              Task
            </div>
            <div className="max-h-48 overflow-y-auto rounded-lg bg-zinc-800/60 p-2 text-xs thin-scrollbar">
              <StyledMarkdownPreview source={progress.task} />
            </div>
          </div>

          {/* Token usage */}
          {progress.tokenUsage && progress.tokenUsage.totalTokens > 0 && (
            <div className="flex items-center gap-2 text-[10px] text-zinc-500">
              <span>In: {formatTokenCount(progress.tokenUsage.promptTokens)}</span>
              <span>Out: {formatTokenCount(progress.tokenUsage.completionTokens)}</span>
              <span className="text-zinc-400">Total: {formatTokenCount(progress.tokenUsage.totalTokens)}</span>
              {(progress.tokenUsage as any).budget > 0 && (
                <span
                  className={
                    progress.tokenUsage.totalTokens / (progress.tokenUsage as any).budget > 0.8
                      ? "text-red-400"
                      : progress.tokenUsage.totalTokens / (progress.tokenUsage as any).budget > 0.5
                        ? "text-amber-400"
                        : "text-emerald-400"
                  }
                >
                  Budget: {Math.round((progress.tokenUsage.totalTokens / (progress.tokenUsage as any).budget) * 100)}%
                </span>
              )}
            </div>
          )}

          {/* Tool timeline */}
          {progress.toolActions && progress.toolActions.length > 0 && (
            <ToolTimeline actions={progress.toolActions} />
          )}

          {/* Thinking log */}
          {progress.thinkingLog && progress.thinkingLog.length > 0 && (
            <ThinkingLog entries={progress.thinkingLog} />
          )}

          {/* Result */}
          {progress.result && (
            <div
              className="cursor-pointer rounded-lg p-1 transition-colors [&:hover_.result-label]:text-orange-400 [&:hover_.result-label]:drop-shadow-[0_0_4px_rgba(251,146,60,0.5)]"
              onClick={(e) => {
                e.stopPropagation();
                const id = progress.subAgentId?.slice(0, 8) || "unknown";
                const title = `Agent: ${name} (${id}).md`;
                void ideMessenger.post("showVirtualFile", { name: title, content: buildAgentMarkdown(progress, index) });
              }}
            >
              <div className="result-label mb-1 text-[10px] font-medium uppercase tracking-wider text-zinc-500 transition-all">
                Result
              </div>
              <div className="max-h-48 overflow-y-auto rounded-lg bg-zinc-800/60 p-2 text-xs thin-scrollbar">
                <StyledMarkdownPreview source={progress.result} />
              </div>
            </div>
          )}

          {/* Error */}
          {progress.error && (
            <div>
              <div className="mb-1 text-[10px] font-medium uppercase tracking-wider text-red-400">
                Error
              </div>
              <div className="rounded-lg bg-red-900/20 p-2 text-xs text-red-300">
                {progress.error}
              </div>
            </div>
          )}
        </div>
      )}
    </div>
  );
}

// ─── Main Panel Component ────────────────────────────────────

export function MultiAgentPanel() {
  const ideMessenger = useContext(IdeMessengerContext);
  const [expanded, setExpanded] = useState(true);
  const [fullScreen, setFullScreen] = useState(false);
  const [expandedCards, setExpandedCards] = useState<Set<string>>(new Set());
  const [orchestratorExpanded, setOrchestratorExpanded] = useState(false);
  const [lines, setLines] = useState<{ d: string; isRunning: boolean }[]>([]);
  const [selectedAgentId, setSelectedAgentId] = useState<string | null>(null);
  const [showOrchestratorDetail, setShowOrchestratorDetail] = useState(false);

  const allProgress = useAppSelector(
    (state) => state.session.subAgentProgress,
  );

  const orchestratorRef = useRef<HTMLDivElement>(null);
  const svgRef = useRef<SVGSVGElement>(null);
  const agentRefs = useRef<Map<string, HTMLDivElement>>(new Map());

  const [showHistory, setShowHistory] = useState(false);

  // Convert progress map to sorted array
  const agents = useMemo(() => {
    const entries = Object.values(allProgress);
    return entries.sort((a, b) => (a.startedAt || 0) - (b.startedAt || 0));
  }, [allProgress]);

  // Split into live (running) and history (everything else)
  const liveAgents = useMemo(
    () => agents.filter((a) => a.status === "running"),
    [agents],
  );
  const historyAgents = useMemo(
    () =>
      agents.filter(
        (a) =>
          a.status === "completed" ||
          a.status === "errored" ||
          a.status === "canceled",
      ),
    [agents],
  );

  // Aggregate status for the header
  const summary = useMemo(() => {
    const total = agents.length;
    const completed = agents.filter((a) => a.status === "completed").length;
    const running = agents.filter((a) => a.status === "running").length;
    const errored = agents.filter((a) => a.status === "errored").length;

    if (completed === total)
      return {
        text: `All ${total} agents completed`,
        color: "text-green-400",
      };
    if (errored > 0 && running === 0)
      return {
        text: `${errored} errored, ${completed} completed`,
        color: "text-red-400",
      };
    if (running > 0)
      return {
        text: `${running} running, ${completed} done`,
        color: "text-blue-400",
      };
    return {
      text: `${completed}/${total} completed`,
      color: "text-zinc-400",
    };
  }, [agents]);


  // Toggle card expand/collapse
  const toggleCardExpand = useCallback((subAgentId: string) => {
    setExpandedCards((prev) => {
      const next = new Set(prev);
      if (next.has(subAgentId)) {
        next.delete(subAgentId);
      } else {
        next.add(subAgentId);
      }
      return next;
    });
  }, []);

  // Kill a specific agent
  const handleKillAgent = useCallback(
    (subAgentId: string) => {
      ideMessenger.post("cancelSubAgent", { subAgentId });
    },
    [ideMessenger],
  );

  // Selected agent for detail overlay
  const selectedAgent = useMemo(() => {
    if (!selectedAgentId) return null;
    const idx = agents.findIndex((a) => a.subAgentId === selectedAgentId);
    if (idx < 0) return null;
    return { agent: agents[idx], index: idx };
  }, [selectedAgentId, agents]);

  // Ref callback for agent nodes
  const setAgentRef = useCallback(
    (id: string) => (el: HTMLDivElement | null) => {
      if (el) {
        agentRefs.current.set(id, el);
      } else {
        agentRefs.current.delete(id);
      }
    },
    [],
  );

  // Calculate SVG connection lines
  const calculateLines = useCallback(() => {
    const svg = svgRef.current;
    const orch = orchestratorRef.current;
    if (!svg || !orch || !expanded) {
      setLines([]);
      return;
    }

    const svgRect = svg.getBoundingClientRect();
    const orchRect = orch.getBoundingClientRect();
    const orchCenterX = orchRect.left + orchRect.width / 2 - svgRect.left;
    const orchBottomY = orchRect.bottom - svgRect.top;

    const newLines: { d: string; isRunning: boolean }[] = [];

    // Only draw lines to live (running) agents
    liveAgents.forEach((agent) => {
      const el = agentRefs.current.get(agent.subAgentId);
      if (!el) return;

      const rect = el.getBoundingClientRect();
      const agentCenterX = rect.left + rect.width / 2 - svgRect.left;
      const agentTopY = rect.top - svgRect.top;
      const midY = orchBottomY + (agentTopY - orchBottomY) * 0.5;

      newLines.push({
        d: `M ${orchCenterX} ${orchBottomY} L ${orchCenterX} ${midY} L ${agentCenterX} ${midY} L ${agentCenterX} ${agentTopY}`,
        isRunning: agent.status === "running",
      });
    });

    setLines(newLines);
  }, [expanded, fullScreen, liveAgents, expandedCards.size, orchestratorExpanded]);

  useLayoutEffect(() => {
    const rafId = requestAnimationFrame(calculateLines);
    return () => cancelAnimationFrame(rafId);
  }, [calculateLines]);

  useEffect(() => {
    if (!svgRef.current?.parentElement) return;
    const observer = new ResizeObserver(() => {
      requestAnimationFrame(calculateLines);
    });
    observer.observe(svgRef.current.parentElement);
    return () => observer.disconnect();
  }, [calculateLines]);

  // Don't render if no agents
  if (agents.length === 0) return null;

  // ── Full-screen overlay wrapper ──
  if (fullScreen) {
    return (
      <div className="fixed inset-0 z-[100] flex flex-col overflow-hidden bg-zinc-950/95 backdrop-blur-sm">
        {/* Full-screen header */}
        <div className="flex items-center gap-2 border-b border-zinc-700/50 px-4 py-3">
          <CpuChipIcon className="h-5 w-5 flex-shrink-0 text-zinc-400" />
          <span className="text-sm font-medium text-zinc-200">
            Multi-Agent Orchestrator
          </span>
          <span className={`ml-2 text-xs font-medium ${summary.color}`}>
            {summary.text}
          </span>
          <div className="ml-auto flex items-center gap-1">
            <button
              onClick={() => setFullScreen(false)}
              className="flex items-center gap-1.5 rounded-lg border border-solid border-zinc-700/50 bg-zinc-800/60 px-3 py-1.5 text-[11px] font-medium text-zinc-400 transition-all hover:border-zinc-600 hover:bg-zinc-700/80 hover:text-zinc-200"
              title="Exit full screen"
            >
              <ArrowsPointingInIcon className="h-3.5 w-3.5" />
              Exit
            </button>
          </div>
        </div>

        {/* Full-screen body */}
        <div className="flex-1 overflow-y-auto px-4 py-4 thin-scrollbar">
          <div className="relative">
            {/* SVG connection lines */}
            <svg
              ref={svgRef}
              className="pointer-events-none absolute inset-0 h-full w-full overflow-visible"
              style={{ zIndex: 0 }}
            >
              {lines.map((line, i) => (
                <path
                  key={i}
                  d={line.d}
                  fill="none"
                  strokeWidth="1.5"
                  strokeLinecap="round"
                  strokeLinejoin="round"
                  className={
                    line.isRunning ? "text-blue-500/60" : "text-zinc-600"
                  }
                  stroke="currentColor"
                  strokeDasharray={line.isRunning ? "4 4" : "none"}
                  style={
                    line.isRunning
                      ? { animation: "dash-flow 1s linear infinite" }
                      : undefined
                  }
                />
              ))}
            </svg>

            {/* Orchestrator node */}
            <div
              className="relative flex justify-center pb-6"
              style={{ zIndex: 1 }}
            >
              <div
                ref={orchestratorRef}
                className={`${orchestratorExpanded ? "w-full max-w-lg" : ""} cursor-pointer overflow-hidden rounded-xl border border-zinc-600/50 bg-zinc-800/80 transition-all duration-200`}
              >
                <div
                  className="flex items-center gap-1.5 px-4 py-2 transition-colors hover:bg-zinc-700/80"
                  onClick={() => setOrchestratorExpanded((prev) => !prev)}
                >
                  <ChevronRightIcon
                    className={`h-3 w-3 flex-shrink-0 text-zinc-500 transition-transform duration-150 ${
                      orchestratorExpanded ? "rotate-90" : ""
                    }`}
                  />
                  <CpuChipIcon className="h-4 w-4 text-zinc-400" />
                  <span className="text-xs font-medium text-zinc-300">
                    Orchestrator
                  </span>
                  <button
                    onClick={(e) => {
                      e.stopPropagation();
                      setShowOrchestratorDetail(true);
                    }}
                    className="ml-auto rounded p-0.5 text-zinc-500 transition-colors hover:text-zinc-300"
                    title="View full details"
                  >
                    <EyeIcon className="h-3.5 w-3.5" />
                  </button>
                </div>
                {orchestratorExpanded && (
                  <div className="border-t border-zinc-700/30 px-3 py-2">
                    <div className="grid grid-cols-4 gap-2">
                      <div className="rounded-lg bg-zinc-800/60 px-2 py-1.5 text-center">
                        <div className="text-lg font-bold text-zinc-100">{agents.length}</div>
                        <div className="text-[10px] text-zinc-500">Agents</div>
                      </div>
                      <div className="rounded-lg bg-zinc-800/60 px-2 py-1.5 text-center">
                        <div className="text-lg font-bold text-zinc-100">
                          {agents.reduce((s, a) => s + a.stepsCompleted, 0)}
                        </div>
                        <div className="text-[10px] text-zinc-500">Steps</div>
                      </div>
                      <div className="rounded-lg bg-zinc-800/60 px-2 py-1.5 text-center">
                        <div className="text-lg font-bold text-green-400">
                          {agents.filter((a) => a.status === "completed").length}/{agents.length}
                        </div>
                        <div className="text-[10px] text-zinc-500">Done</div>
                      </div>
                      <div className="rounded-lg bg-zinc-800/60 px-2 py-1.5 text-center">
                        <div className="text-lg font-bold text-zinc-100">
                          {(() => {
                            const earliest = Math.min(...agents.map((a) => a.startedAt || Infinity));
                            const latest = Math.max(...agents.map((a) => a.completedAt || 0));
                            const allDone = agents.every((a) => a.status !== "running");
                            return earliest < Infinity
                              ? formatDuration((allDone && latest ? latest : Date.now()) - earliest)
                              : "—";
                          })()}
                        </div>
                        <div className="text-[10px] text-zinc-500">Time</div>
                      </div>
                    </div>
                    {/* Agent token totals */}
                    {(() => {
                      const totalAgentTokens = agents.reduce(
                        (sum, a) => sum + (a.tokenUsage?.totalTokens ?? 0), 0
                      );
                      if (totalAgentTokens === 0) return null;
                      return (
                        <div className="mt-2 flex items-center justify-center gap-2 text-[10px] text-zinc-500">
                          <span>Agent tokens: {formatTokenCount(totalAgentTokens)}</span>
                        </div>
                      );
                    })()}
                  </div>
                )}
              </div>
            </div>

            {/* Live agent cards */}
            {liveAgents.length > 0 ? (
              <div
                className="relative flex flex-wrap justify-center gap-3"
                style={{ zIndex: 1 }}
              >
                {liveAgents.map((agent, i) => (
                  <AgentCard
                    key={agent.subAgentId}
                    progress={agent}
                    index={agents.indexOf(agent)}
                    nodeRef={setAgentRef(agent.subAgentId)}
                    onClick={() => setSelectedAgentId(agent.subAgentId)}
                    onKill={() => handleKillAgent(agent.subAgentId)}
                    isExpanded={expandedCards.has(agent.subAgentId)}
                    onToggleExpand={() => toggleCardExpand(agent.subAgentId)}
                  />
                ))}
              </div>
            ) : (
              <div className="py-4 text-center text-xs text-zinc-500">
                No agents currently running
              </div>
            )}
          </div>

          {/* History section in full-screen */}
          {historyAgents.length > 0 && (
            <div className="mt-6 rounded-lg border border-solid border-zinc-700/40 bg-zinc-900/40">
              <div
                className="flex cursor-pointer items-center gap-2 px-4 py-2.5 transition-colors hover:bg-zinc-800/30"
                onClick={() => setShowHistory((prev) => !prev)}
              >
                <ChevronRightIcon
                  className={`h-3 w-3 flex-shrink-0 text-zinc-500 transition-transform duration-150 ${
                    showHistory ? "rotate-90" : ""
                  }`}
                />
                <ClockIcon className="h-3.5 w-3.5 text-zinc-500" />
                <span className="text-xs font-medium text-zinc-400">
                  Agent History
                </span>
                <span className="text-[11px] text-zinc-600">
                  ({historyAgents.length} agent{historyAgents.length !== 1 ? "s" : ""})
                </span>
              </div>
              {showHistory && (
                <div className="border-t border-zinc-700/30 px-4 py-3">
                  <div className="grid gap-2 sm:grid-cols-2">
                    {historyAgents.map((agent) => {
                      const idx = agents.indexOf(agent);
                      const name = getAgentDisplayName(agent, idx);
                      const elapsed =
                        agent.startedAt && agent.completedAt
                          ? formatDuration(agent.completedAt - agent.startedAt)
                          : undefined;
                      return (
                        <div
                          key={agent.subAgentId}
                          className="flex cursor-pointer items-center gap-2 rounded-lg bg-zinc-800/40 px-3 py-2 transition-colors hover:bg-zinc-800/70"
                          onClick={() => setSelectedAgentId(agent.subAgentId)}
                        >
                          <RoleIcon
                            role={agent.role}
                            className="h-3.5 w-3.5 flex-shrink-0 text-zinc-400"
                          />
                          <span className="min-w-0 flex-1 truncate text-xs font-medium text-zinc-200">
                            {name}
                          </span>
                          <span className="text-[10px] text-zinc-500">
                            {agent.stepsCompleted} steps
                          </span>
                          {elapsed && (
                            <span className="text-[10px] text-zinc-500">
                              {elapsed}
                            </span>
                          )}
                          <StatusBadge status={agent.status} />
                        </div>
                      );
                    })}
                  </div>
                </div>
              )}
            </div>
          )}
        </div>

        {/* Agent Detail Overlay (within full-screen) */}
        {selectedAgent && (
          <AgentDetailOverlay
            progress={selectedAgent.agent}
            index={selectedAgent.index}
            onClose={() => setSelectedAgentId(null)}
            onKill={() => handleKillAgent(selectedAgent.agent.subAgentId)}
          />
        )}
        {showOrchestratorDetail && (
          <OrchestratorDetailOverlay
            agents={agents}
            onClose={() => setShowOrchestratorDetail(false)}
          />
        )}
      </div>
    );
  }

  return (
    <div className="relative mx-2 mb-1 overflow-hidden rounded-lg border border-solid border-zinc-700/50 bg-zinc-900/20">
      {/* ── Header ── */}
      <div
        className="flex cursor-pointer items-center gap-2 px-3 py-2 transition-colors hover:bg-zinc-800/30"
        onClick={() => setExpanded((prev) => !prev)}
      >
        <ChevronRightIcon
          className={`h-3.5 w-3.5 flex-shrink-0 text-zinc-400 transition-transform duration-200 ${
            expanded ? "rotate-90" : ""
          }`}
        />
        <CpuChipIcon className="h-4 w-4 flex-shrink-0 text-zinc-400" />
        <span className="text-xs font-medium text-zinc-200">
          Multi-Agent Orchestrator
        </span>
        <span className={`ml-auto text-[10px] font-medium ${summary.color}`}>
          {summary.text}
        </span>
        <button
          onClick={(e) => {
            e.stopPropagation();
            setFullScreen(true);
          }}
          className="ml-1 rounded-md bg-zinc-800/50 p-1.5 text-zinc-500 transition-all hover:bg-zinc-700/70 hover:text-zinc-300"
          title="Open in full screen"
        >
          <ArrowsPointingOutIcon className="h-3 w-3" />
        </button>
      </div>

      {/* ── Collapsed: only live agents ── */}
      {!expanded && (
        <div className="flex flex-wrap gap-x-3 gap-y-1 border-t border-zinc-700/30 px-3 py-1.5">
          {liveAgents.length > 0 ? (
            liveAgents.map((agent, i) => {
              const name = getAgentDisplayName(agent, i);
              return (
                <div
                  key={agent.subAgentId}
                  className="flex cursor-pointer items-center gap-1.5 text-[10px] hover:text-zinc-200"
                  onClick={(e) => {
                    e.stopPropagation();
                    setSelectedAgentId(agent.subAgentId);
                  }}
                >
                  <StatusDot status={agent.status} />
                  <RoleIcon
                    role={agent.role}
                    className="h-2.5 w-2.5 text-zinc-500"
                  />
                  <span className="font-medium text-zinc-300">{name}:</span>
                  <span className="max-w-[100px] truncate text-zinc-500">
                    {agent.currentAction || "Working..."}
                  </span>
                </div>
              );
            })
          ) : (
            <span className="text-[10px] text-zinc-500">
              No agents running — expand to see history
            </span>
          )}
        </div>
      )}

      {/* ── Expanded: Tree body (scrollable) ── */}
      {expanded && (
        <div className="border-t border-zinc-700/30">
          {/* Live agents tree — scrollable */}
          <div className="max-h-[350px] overflow-y-auto px-3 pb-3 pt-2 thin-scrollbar">
            <div className="relative">
              {/* SVG connection lines */}
              <svg
                ref={svgRef}
                className="pointer-events-none absolute inset-0 h-full w-full overflow-visible"
                style={{ zIndex: 0 }}
              >
                {lines.map((line, i) => (
                  <path
                    key={i}
                    d={line.d}
                    fill="none"
                    strokeWidth="1.5"
                    strokeLinecap="round"
                    strokeLinejoin="round"
                    className={
                      line.isRunning ? "text-blue-500/60" : "text-zinc-600"
                    }
                    stroke="currentColor"
                    strokeDasharray={line.isRunning ? "4 4" : "none"}
                    style={
                      line.isRunning
                        ? { animation: "dash-flow 1s linear infinite" }
                        : undefined
                    }
                  />
                ))}
              </svg>

              {/* Orchestrator node */}
              <div
                className="relative flex justify-center pb-5"
                style={{ zIndex: 1 }}
              >
                <div
                  ref={orchestratorRef}
                  className={`${orchestratorExpanded ? "w-full max-w-sm" : ""} cursor-pointer overflow-hidden rounded-xl border border-zinc-600/50 bg-zinc-800/80 transition-all duration-200`}
                >
                  <div
                    className="flex items-center gap-1.5 px-3 py-1 transition-colors hover:bg-zinc-700/80"
                    onClick={() => setOrchestratorExpanded((prev) => !prev)}
                  >
                    <ChevronRightIcon
                      className={`h-2.5 w-2.5 flex-shrink-0 text-zinc-500 transition-transform duration-150 ${
                        orchestratorExpanded ? "rotate-90" : ""
                      }`}
                    />
                    <CpuChipIcon className="h-3 w-3 text-zinc-400" />
                    <span className="text-[10px] font-medium text-zinc-300">
                      Orchestrator
                    </span>
                    <button
                      onClick={(e) => {
                        e.stopPropagation();
                        setShowOrchestratorDetail(true);
                      }}
                      className="ml-auto rounded p-0.5 text-zinc-500 transition-colors hover:text-zinc-300"
                      title="View full details"
                    >
                      <EyeIcon className="h-3 w-3" />
                    </button>
                  </div>
                  {orchestratorExpanded && (
                    <div className="border-t border-zinc-700/30 px-3 py-2">
                      <div className="grid grid-cols-4 gap-1.5">
                        <div className="rounded-lg bg-zinc-800/60 px-2 py-1.5 text-center">
                          <div className="text-sm font-bold text-zinc-100">
                            {agents.length}
                          </div>
                          <div className="text-[9px] text-zinc-500">Total</div>
                        </div>
                        <div className="rounded-lg bg-zinc-800/60 px-2 py-1.5 text-center">
                          <div className="text-sm font-bold text-blue-400">
                            {liveAgents.length}
                          </div>
                          <div className="text-[9px] text-zinc-500">Live</div>
                        </div>
                        <div className="rounded-lg bg-zinc-800/60 px-2 py-1.5 text-center">
                          <div className="text-sm font-bold text-green-400">
                            {agents.filter((a) => a.status === "completed").length}
                          </div>
                          <div className="text-[9px] text-zinc-500">Done</div>
                        </div>
                        <div className="rounded-lg bg-zinc-800/60 px-2 py-1.5 text-center">
                          <div className="text-sm font-bold text-zinc-100">
                            {agents.reduce((s, a) => s + a.stepsCompleted, 0)}
                          </div>
                          <div className="text-[9px] text-zinc-500">Steps</div>
                        </div>
                      </div>
                    </div>
                  )}
                </div>
              </div>

              {/* Live agent cards only */}
              {liveAgents.length > 0 ? (
                <div
                  className="relative flex flex-wrap justify-center gap-2"
                  style={{ zIndex: 1 }}
                >
                  {liveAgents.map((agent, i) => (
                    <AgentCard
                      key={agent.subAgentId}
                      progress={agent}
                      index={agents.indexOf(agent)}
                      nodeRef={setAgentRef(agent.subAgentId)}
                      onClick={() => setSelectedAgentId(agent.subAgentId)}
                      onKill={() => handleKillAgent(agent.subAgentId)}
                      isExpanded={expandedCards.has(agent.subAgentId)}
                      onToggleExpand={() => toggleCardExpand(agent.subAgentId)}
                    />
                  ))}
                </div>
              ) : (
                <div className="py-2 text-center text-[11px] text-zinc-500">
                  No agents currently running
                </div>
              )}
            </div>
          </div>

          {/* ── Agent History section ── */}
          {historyAgents.length > 0 && (
            <div className="border-t border-zinc-700/30">
              <div
                className="flex cursor-pointer items-center gap-1.5 px-3 py-1.5 transition-colors hover:bg-zinc-800/30"
                onClick={() => setShowHistory((prev) => !prev)}
              >
                <ChevronRightIcon
                  className={`h-2.5 w-2.5 flex-shrink-0 text-zinc-500 transition-transform duration-150 ${
                    showHistory ? "rotate-90" : ""
                  }`}
                />
                <ClockIcon className="h-3 w-3 text-zinc-500" />
                <span className="text-[10px] font-medium text-zinc-400">
                  Agent History
                </span>
                <span className="text-[10px] text-zinc-600">
                  ({historyAgents.length})
                </span>
              </div>
              {showHistory && (
                <div className="max-h-[200px] overflow-y-auto px-3 pb-2 thin-scrollbar">
                  <div className="space-y-1">
                    {historyAgents.map((agent) => {
                      const name = getAgentDisplayName(
                        agent,
                        agents.indexOf(agent),
                      );
                      const elapsed =
                        agent.startedAt && agent.completedAt
                          ? formatDuration(
                              agent.completedAt - agent.startedAt,
                            )
                          : undefined;
                      return (
                        <div
                          key={agent.subAgentId}
                          className="flex cursor-pointer items-center gap-2 rounded-lg bg-zinc-800/30 px-2.5 py-1.5 transition-colors hover:bg-zinc-800/60"
                          onClick={() =>
                            setSelectedAgentId(agent.subAgentId)
                          }
                        >
                          <RoleIcon
                            role={agent.role}
                            className="h-3 w-3 flex-shrink-0 text-zinc-500"
                          />
                          <span className="min-w-0 flex-1 truncate text-[11px] font-medium text-zinc-300">
                            {name}
                          </span>
                          <span className="text-[10px] text-zinc-500">
                            {agent.stepsCompleted} steps
                          </span>
                          {elapsed && (
                            <span className="text-[10px] text-zinc-500">
                              {elapsed}
                            </span>
                          )}
                          <StatusBadge status={agent.status} />
                        </div>
                      );
                    })}
                  </div>
                </div>
              )}
            </div>
          )}
        </div>
      )}

      {/* ── Agent Detail Overlay ── */}
      {selectedAgent && (
        <AgentDetailOverlay
          progress={selectedAgent.agent}
          index={selectedAgent.index}
          onClose={() => setSelectedAgentId(null)}
          onKill={() => handleKillAgent(selectedAgent.agent.subAgentId)}
        />
      )}

      {/* ── Orchestrator Detail Overlay ── */}
      {showOrchestratorDetail && (
        <OrchestratorDetailOverlay
          agents={agents}
          onClose={() => setShowOrchestratorDetail(false)}
        />
      )}
    </div>
  );
}
