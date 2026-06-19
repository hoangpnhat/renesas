import {
  CheckCircleIcon,
  ChevronRightIcon,
  CpuChipIcon,
  ExclamationCircleIcon,
  StopCircleIcon,
} from "@heroicons/react/24/outline";
import { SubAgentProgress, ToolAction, ToolCallState } from "core";
import {
  useCallback,
  useEffect,
  useLayoutEffect,
  useMemo,
  useRef,
  useState,
} from "react";
import { useAppSelector } from "../../../redux/hooks";
import StyledMarkdownPreview from "../../../components/StyledMarkdownPreview";
import Spinner from "../../../components/gui/Spinner";

interface MultiAgentTreeViewProps {
  spawnAgentToolCallStates: ToolCallState[];
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

// ─── ToolTimeline: shows each tool used by a sub-agent ──────

function ToolTimeline({ actions }: { actions: ToolAction[] }) {
  if (!actions || actions.length === 0) return null;

  return (
    <div className="space-y-0.5">
      <div className="mb-1 text-[10px] font-medium uppercase tracking-wider text-zinc-500">
        Tool Timeline
      </div>
      {actions.map((action, i) => (
        <div
          key={i}
          className="flex items-center gap-1.5 py-0.5 text-[11px]"
        >
          {action.status === "completed" ? (
            <CheckCircleIcon className="h-3 w-3 flex-shrink-0 text-green-500" />
          ) : action.status === "errored" ? (
            <ExclamationCircleIcon className="h-3 w-3 flex-shrink-0 text-red-400" />
          ) : (
            <span className="flex-shrink-0">
              <Spinner />
            </span>
          )}
          <span className="min-w-0 flex-1 truncate text-zinc-300">
            {formatToolName(action.toolName)}
          </span>
          <span className="flex-shrink-0 text-zinc-500">
            {action.durationMs != null
              ? formatDuration(action.durationMs)
              : "..."}
          </span>
        </div>
      ))}
    </div>
  );
}

// ─── SubAgentNode: a single agent card in the tree ──────────

function SubAgentNode({
  toolCallState,
  nodeRef,
}: {
  toolCallState: ToolCallState;
  nodeRef: (el: HTMLDivElement | null) => void;
}) {
  const [expanded, setExpanded] = useState(false);
  const allProgress = useAppSelector(
    (state) => state.session.subAgentProgress,
  );

  const progress = useMemo(
    () =>
      Object.values(allProgress).find(
        (p) => p.parentToolCallId === toolCallState.toolCallId,
      ),
    [allProgress, toolCallState.toolCallId],
  );

  const description =
    progress?.description ||
    toolCallState.parsedArgs?.description ||
    "Sub-agent";
  const task = progress?.task || toolCallState.parsedArgs?.task || "";
  const status: SubAgentProgress["status"] =
    progress?.status ||
    (toolCallState.status === "done" ? "completed" : "running");
  const currentAction = progress?.currentAction;
  const stepsCompleted = progress?.stepsCompleted || 0;
  const result = progress?.result;
  const error = progress?.error;
  const toolActions = progress?.toolActions ?? [];
  const startedAt = progress?.startedAt;
  const completedAt = progress?.completedAt;

  const elapsed =
    startedAt && completedAt
      ? formatDuration(completedAt - startedAt)
      : undefined;

  // Border color by status
  const borderColor =
    status === "running"
      ? "border-blue-500/40"
      : status === "completed"
        ? "border-green-500/30"
        : status === "errored"
          ? "border-red-500/30"
          : "border-zinc-700/50";

  return (
    <div
      ref={nodeRef}
      className={`w-44 overflow-hidden rounded-lg border border-solid ${borderColor} bg-zinc-900/50 transition-colors duration-500`}
    >
      {/* Card header */}
      <div
        className="flex cursor-pointer items-center gap-1.5 px-2.5 py-2 transition-colors hover:bg-zinc-800/40"
        onClick={() => setExpanded((prev) => !prev)}
      >
        <StatusDot status={status} />
        <span className="min-w-0 flex-1 truncate text-[11px] font-medium text-zinc-200">
          {description}
        </span>
        <ChevronRightIcon
          className={`h-3 w-3 flex-shrink-0 text-zinc-500 transition-transform duration-200 ${
            expanded ? "rotate-90" : ""
          }`}
        />
      </div>

      {/* Compact info line (always visible) */}
      <div className="flex items-center gap-1.5 border-t border-zinc-700/30 px-2.5 py-1.5">
        {status === "running" && currentAction ? (
          <span className="truncate text-[10px] text-zinc-500">
            {currentAction}
          </span>
        ) : (
          <span className="text-[10px] text-zinc-500">
            {stepsCompleted} step{stepsCompleted !== 1 ? "s" : ""}
            {elapsed ? ` · ${elapsed}` : ""}
          </span>
        )}
      </div>

      {/* Expanded content */}
      {expanded && (
        <div className="border-t border-zinc-700/30 px-2.5 py-2 space-y-2">
          {/* Status badge */}
          <div className="flex items-center justify-between">
            <StatusBadge status={status} />
            {elapsed && (
              <span className="text-[10px] text-zinc-500">{elapsed}</span>
            )}
          </div>

          {/* Task */}
          <div>
            <div className="mb-0.5 text-[10px] font-medium uppercase tracking-wider text-zinc-500">
              Task
            </div>
            <div className="text-[11px] leading-relaxed text-zinc-300">
              {task}
            </div>
          </div>

          {/* Tool timeline */}
          <ToolTimeline actions={toolActions} />

          {/* Result */}
          {result && (
            <div>
              <div className="mb-0.5 text-[10px] font-medium uppercase tracking-wider text-zinc-500">
                Result
              </div>
              <div className="max-h-48 overflow-y-auto rounded bg-zinc-900/50 p-1.5 text-xs">
                <StyledMarkdownPreview source={result} />
              </div>
            </div>
          )}

          {/* Error */}
          {error && (
            <div>
              <div className="mb-0.5 text-[10px] font-medium uppercase tracking-wider text-red-400">
                Error
              </div>
              <div className="rounded bg-red-900/20 p-1.5 text-[11px] text-red-300">
                {error}
              </div>
            </div>
          )}
        </div>
      )}
    </div>
  );
}

// ─── Main component: MultiAgentTreeView ─────────────────────

export function MultiAgentTreeView({
  spawnAgentToolCallStates,
}: MultiAgentTreeViewProps) {
  const [expanded, setExpanded] = useState(true);
  const [lines, setLines] = useState<
    { d: string; isRunning: boolean }[]
  >([]);

  const allProgress = useAppSelector(
    (state) => state.session.subAgentProgress,
  );

  const orchestratorRef = useRef<HTMLDivElement>(null);
  const svgRef = useRef<SVGSVGElement>(null);
  const agentRefs = useRef<Map<string, HTMLDivElement>>(new Map());

  // Map each tool call to its progress
  const agentProgresses = useMemo(() => {
    return spawnAgentToolCallStates.map((tcs) => {
      const progress = Object.values(allProgress).find(
        (p) => p.parentToolCallId === tcs.toolCallId,
      );
      return {
        toolCallState: tcs,
        progress,
        status: (progress?.status ||
          (tcs.status === "done"
            ? "completed"
            : "running")) as SubAgentProgress["status"],
      };
    });
  }, [spawnAgentToolCallStates, allProgress]);

  // Aggregate status for the header
  const summary = useMemo(() => {
    const total = agentProgresses.length;
    const completed = agentProgresses.filter(
      (a) => a.status === "completed",
    ).length;
    const running = agentProgresses.filter(
      (a) => a.status === "running",
    ).length;
    const errored = agentProgresses.filter(
      (a) => a.status === "errored",
    ).length;
    const canceled = agentProgresses.filter(
      (a) => a.status === "canceled",
    ).length;

    if (completed === total) return { text: `All ${total} agents completed`, color: "text-green-400" };
    if (errored > 0 && running === 0)
      return { text: `${errored} errored, ${completed} completed`, color: "text-red-400" };
    if (running > 0)
      return { text: `${running} of ${total} agents running...`, color: "text-blue-400" };
    return { text: `${completed}/${total} completed`, color: "text-zinc-400" };
  }, [agentProgresses]);

  // Ref callback for agent nodes
  const setAgentRef = useCallback(
    (toolCallId: string) => (el: HTMLDivElement | null) => {
      if (el) {
        agentRefs.current.set(toolCallId, el);
      } else {
        agentRefs.current.delete(toolCallId);
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

    agentProgresses.forEach(({ toolCallState, status }) => {
      const el = agentRefs.current.get(toolCallState.toolCallId);
      if (!el) return;

      const rect = el.getBoundingClientRect();
      const agentCenterX = rect.left + rect.width / 2 - svgRect.left;
      const agentTopY = rect.top - svgRect.top;
      const midY = orchBottomY + (agentTopY - orchBottomY) * 0.5;

      newLines.push({
        d: `M ${orchCenterX} ${orchBottomY} L ${orchCenterX} ${midY} L ${agentCenterX} ${midY} L ${agentCenterX} ${agentTopY}`,
        isRunning: status === "running",
      });
    });

    setLines(newLines);
  }, [expanded, agentProgresses]);

  useLayoutEffect(() => {
    // Defer one frame to ensure layout is stable
    const rafId = requestAnimationFrame(calculateLines);
    return () => cancelAnimationFrame(rafId);
  }, [calculateLines]);

  // Recalculate on resize
  useEffect(() => {
    if (!svgRef.current?.parentElement) return;
    const observer = new ResizeObserver(() => {
      requestAnimationFrame(calculateLines);
    });
    observer.observe(svgRef.current.parentElement);
    return () => observer.disconnect();
  }, [calculateLines]);

  return (
    <div className="mx-2 my-3 overflow-hidden rounded-lg border border-solid border-zinc-700/50 bg-zinc-900/20">
      {/* ── Orchestrator Header ── */}
      <div
        className="flex cursor-pointer items-center gap-2 px-3 py-2.5 transition-colors hover:bg-zinc-800/30"
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
      </div>

      {/* ── Collapsed: Agent summary strip ── */}
      {!expanded && (
        <div className="flex flex-wrap gap-x-3 gap-y-1 border-t border-zinc-700/30 px-3 py-2">
          {agentProgresses.map(({ toolCallState, progress, status }) => {
            const desc =
              progress?.description ||
              toolCallState.parsedArgs?.description ||
              "Agent";
            const action = progress?.currentAction;
            return (
              <div
                key={toolCallState.toolCallId}
                className="flex items-center gap-1.5 text-[10px]"
              >
                <StatusDot status={status} />
                <span className="font-medium text-zinc-300">{desc}:</span>
                <span className="max-w-[120px] truncate text-zinc-500">
                  {status === "completed"
                    ? `Done (${progress?.stepsCompleted || 0} steps)`
                    : status === "errored"
                      ? "Error"
                      : action || "Working..."}
                </span>
              </div>
            );
          })}
        </div>
      )}

      {/* ── Expanded: Tree body ── */}
      {expanded && (
        <div className="relative border-t border-zinc-700/30 px-3 pb-4 pt-3">
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
                  line.isRunning
                    ? "text-blue-500/60"
                    : "text-zinc-600"
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

          {/* Orchestrator node (centered pill) */}
          <div className="relative flex justify-center pb-6" style={{ zIndex: 1 }}>
            <div
              ref={orchestratorRef}
              className="flex items-center gap-1.5 rounded-full border border-zinc-600/50 bg-zinc-800/80 px-3 py-1.5"
            >
              <CpuChipIcon className="h-3.5 w-3.5 text-zinc-400" />
              <span className="text-[11px] font-medium text-zinc-300">
                Orchestrator
              </span>
            </div>
          </div>

          {/* Sub-agent cards */}
          <div
            className="relative flex flex-wrap justify-center gap-3"
            style={{ zIndex: 1 }}
          >
            {spawnAgentToolCallStates.map((tcs) => (
              <SubAgentNode
                key={tcs.toolCallId}
                toolCallState={tcs}
                nodeRef={setAgentRef(tcs.toolCallId)}
              />
            ))}
          </div>
        </div>
      )}
    </div>
  );
}
