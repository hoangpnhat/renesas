import {
  CheckCircleIcon,
  ChevronRightIcon,
  ExclamationCircleIcon,
  StopCircleIcon,
} from "@heroicons/react/24/outline";
import { ToolCallState } from "core";
import { useEffect, useMemo, useRef, useState } from "react";
import { useAppSelector } from "../../../redux/hooks";

interface AgentSpec {
  task: string;
  description: string;
  role?: string;
  context?: string;
}

interface BatchAgentsDivProps {
  toolCallState: ToolCallState;
}

const ROLE_COLORS: Record<string, string> = {
  Researcher: "text-cyan-400 bg-cyan-500/15",
  Implementer: "text-violet-400 bg-violet-500/15",
  Tester: "text-amber-400 bg-amber-500/15",
  Reviewer: "text-emerald-400 bg-emerald-500/15",
  Analyst: "text-rose-400 bg-rose-500/15",
  Planner: "text-blue-400 bg-blue-500/15",
};

function RobotIcon({ className }: { className?: string }) {
  return (
    <svg
      className={className}
      width="20"
      height="20"
      viewBox="0 0 24 24"
      fill="none"
      xmlns="http://www.w3.org/2000/svg"
    >
      {/* Antenna */}
      <line x1="12" y1="2" x2="12" y2="5" stroke="currentColor" strokeWidth="1.5" strokeLinecap="round" />
      <circle cx="12" cy="1.5" r="1" fill="currentColor" />
      {/* Head */}
      <rect x="5" y="5" width="14" height="10" rx="2.5" stroke="currentColor" strokeWidth="1.5" />
      {/* Eyes */}
      <circle cx="9" cy="10" r="1.5" fill="currentColor" />
      <circle cx="15" cy="10" r="1.5" fill="currentColor" />
      {/* Body */}
      <rect x="7" y="16" width="10" height="5" rx="1.5" stroke="currentColor" strokeWidth="1.5" />
      {/* Arms */}
      <line x1="5" y1="17" x2="3" y2="19" stroke="currentColor" strokeWidth="1.5" strokeLinecap="round" />
      <line x1="19" y1="17" x2="21" y2="19" stroke="currentColor" strokeWidth="1.5" strokeLinecap="round" />
    </svg>
  );
}

function AgentStatusIcon({ status }: { status: string }) {
  switch (status) {
    case "completed":
      return <CheckCircleIcon className="h-3.5 w-3.5 text-green-400" />;
    case "errored":
      return <ExclamationCircleIcon className="h-3.5 w-3.5 text-red-400" />;
    case "canceled":
      return <StopCircleIcon className="h-3.5 w-3.5 text-yellow-400" />;
    default:
      return (
        <span className="flex h-3.5 w-3.5 items-center justify-center">
          <span className="h-1.5 w-1.5 animate-pulse rounded-full bg-blue-400" />
        </span>
      );
  }
}

export function BatchAgentsDiv({ toolCallState }: BatchAgentsDivProps) {
  const [isOpen, setIsOpen] = useState(false);
  const [expandedIdx, setExpandedIdx] = useState<number | null>(null);
  const cardRef = useRef<HTMLDivElement>(null);
  const allProgress = useAppSelector((state) => state.session.subAgentProgress);

  const agents: AgentSpec[] = useMemo(() => {
    return toolCallState.parsedArgs?.agents || [];
  }, [toolCallState.parsedArgs]);

  const isDone = toolCallState.status === "done" || toolCallState.status === "errored" || toolCallState.status === "canceled";

  // Close popup when clicking outside
  useEffect(() => {
    if (!isOpen) return;
    const handler = (e: MouseEvent) => {
      if (cardRef.current && !cardRef.current.contains(e.target as Node)) {
        setIsOpen(false);
      }
    };
    document.addEventListener("mousedown", handler);
    return () => document.removeEventListener("mousedown", handler);
  }, [isOpen]);

  // Match each agent spec to its specific progress entry.
  const agentStatuses = useMemo(() => {
    const batchEntries = Object.values(allProgress).filter(
      (p) => p.parentToolCallId === toolCallState.toolCallId,
    );

    return agents.map((agent) => {
      const match =
        batchEntries.find((p) => p.description === agent.description) ||
        batchEntries.find((p) => p.task === agent.task);
      if (match?.status) return match.status;
      if (isDone) return "completed";
      return "running";
    });
  }, [agents, allProgress, toolCallState.toolCallId, isDone]);

  const completedCount = agentStatuses.filter((s) => s === "completed").length;
  const canceledCount = agentStatuses.filter((s) => s === "canceled").length;
  const erroredCount = agentStatuses.filter((s) => s === "errored").length;
  // Derive effective state from agent statuses — the tool call itself may show "done"
  // even when agents were cancelled
  const hasCanceled = canceledCount > 0;
  const allCanceled = agents.length > 0 && canceledCount === agents.length;

  // Build color array for visible robots (up to 3) proportional to agent statuses.
  // 1/7 cancelled → 1 yellow. 3/7 cancelled → 2 yellow. 7/7 → 3 yellow.
  const robotColors = useMemo(() => {
    const shown = Math.min(agents.length, 3);
    if (shown === 0) return [];
    if (!isDone) {
      return Array(shown).fill("text-zinc-400");
    }
    // Allocate slots: errored first, then cancelled, rest green
    const errorSlots = erroredCount > 0 ? Math.ceil((erroredCount / agents.length) * shown) : 0;
    const cancelSlots = canceledCount > 0 ? Math.ceil((canceledCount / agents.length) * shown) : 0;
    // Cap so we don't exceed shown
    const totalBad = Math.min(errorSlots + cancelSlots, shown);
    const actualError = Math.min(errorSlots, shown);
    const actualCancel = Math.min(cancelSlots, shown - actualError);
    const actualGreen = shown - actualError - actualCancel;

    return [
      ...Array(actualError).fill("text-red-400/70"),
      ...Array(actualCancel).fill("text-yellow-400/70"),
      ...Array(actualGreen).fill("text-green-400/70"),
    ];
  }, [agents.length, isDone, completedCount, canceledCount, erroredCount]);

  return (
    <div className="relative mx-2 my-1" ref={cardRef}>
      <div className="flex items-center gap-2.5 px-2 py-1.5">
        {/* Clickable robots only */}
        <div
          className="flex cursor-pointer items-end gap-0.5 rounded-md p-1 transition-colors hover:bg-zinc-800/40"
          onClick={() => setIsOpen((prev) => !prev)}
        >
          {robotColors.map((color, i) => (
            <RobotIcon
              key={i}
              className={`${isDone ? color : `${color} animate-robot-${i + 1}`} transition-colors duration-500`}
            />
          ))}
          {agents.length > 3 && (
            <span className="mb-0.5 ml-0.5 text-[10px] text-zinc-500">
              +{agents.length - 3}
            </span>
          )}
        </div>

        <span className={`text-[11px] ${hasCanceled ? "text-yellow-400/80" : "text-zinc-400"}`}>
          {allCanceled
            ? "Batch spawn cancelled by user"
            : hasCanceled
              ? `Cancelled — ${completedCount}/${agents.length} completed before cancel`
              : isDone
                ? erroredCount > 0
                  ? `${erroredCount} agent${erroredCount !== 1 ? "s" : ""} failed, ${completedCount} completed`
                  : `${agents.length} agent${agents.length !== 1 ? "s" : ""} completed`
                : completedCount > 0
                  ? `${completedCount}/${agents.length} agents done`
                  : `${agents.length} agent${agents.length !== 1 ? "s" : ""} working...`}
        </span>
      </div>

      {/* Floating popup card */}
      {isOpen && (
        <div className="absolute left-2 top-full z-50 mt-1 w-80 overflow-hidden rounded-lg border border-zinc-700/60 bg-zinc-900/95 shadow-xl backdrop-blur-sm">
          {/* Card header */}
          <div className="flex items-center justify-between border-b border-zinc-700/40 px-3 py-2">
            <span className={`text-[11px] font-medium ${hasCanceled ? "text-yellow-300" : "text-zinc-200"}`}>
              {hasCanceled ? "Cancelled by user" : "Parallel Agents"}
            </span>
            <span className="text-[10px] text-zinc-500">
              {completedCount}/{agents.length} done{canceledCount > 0 ? `, ${canceledCount} cancelled` : ""}
            </span>
          </div>

          {/* Agent list */}
          <div className="max-h-64 overflow-y-auto">
            {agents.map((agent, idx) => {
              const roleColors = ROLE_COLORS[agent.role || ""] || "text-zinc-400 bg-zinc-500/15";
              const isExpanded = expandedIdx === idx;
              const status = agentStatuses[idx];

              return (
                <div
                  key={idx}
                  className="border-b border-zinc-800/40 last:border-b-0"
                >
                  <div
                    className="flex cursor-pointer items-center gap-2 px-3 py-1.5 transition-colors hover:bg-zinc-800/20"
                    onClick={() => setExpandedIdx(isExpanded ? null : idx)}
                  >
                    <ChevronRightIcon
                      className={`h-2.5 w-2.5 flex-shrink-0 text-zinc-500 transition-transform duration-200 ${
                        isExpanded ? "rotate-90" : ""
                      }`}
                    />
                    <AgentStatusIcon status={status} />
                    {agent.role && (
                      <span
                        className={`rounded-full px-1.5 py-0.5 text-[9px] font-semibold uppercase tracking-wide ${roleColors}`}
                      >
                        {agent.role}
                      </span>
                    )}
                    <span className="min-w-0 flex-1 truncate text-[11px] text-zinc-300">
                      {agent.description}
                    </span>
                  </div>

                  {isExpanded && (
                    <div className="bg-zinc-900/30 px-3 py-2 pl-8">
                      <div className="mb-1 text-[9px] font-medium uppercase tracking-wider text-zinc-500">
                        Task
                      </div>
                      <div className="whitespace-pre-wrap text-[11px] leading-relaxed text-zinc-400">
                        {agent.task}
                      </div>
                    </div>
                  )}
                </div>
              );
            })}
          </div>
        </div>
      )}
    </div>
  );
}
