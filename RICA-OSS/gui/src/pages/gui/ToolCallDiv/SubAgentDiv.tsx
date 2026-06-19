import {
  CheckCircleIcon,
  ChevronRightIcon,
  ExclamationCircleIcon,
  StopCircleIcon,
  UserGroupIcon,
} from "@heroicons/react/24/outline";
import { SubAgentProgress, ToolCallState } from "core";
import { useMemo, useState } from "react";
import { useAppSelector } from "../../../redux/hooks";
import StyledMarkdownPreview from "../../../components/StyledMarkdownPreview";

interface SubAgentDivProps {
  toolCallState: ToolCallState;
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

export function SubAgentDiv({ toolCallState }: SubAgentDivProps) {
  const [expanded, setExpanded] = useState(false);

  // Get sub-agent progress from Redux store, keyed by the tool call's parsed args
  const allProgress = useAppSelector((state) => state.session.subAgentProgress);

  // Find the matching progress entry for this tool call
  const progress = useMemo(() => {
    // Match by parentToolCallId
    return Object.values(allProgress).find(
      (p) => p.parentToolCallId === toolCallState.toolCallId,
    );
  }, [allProgress, toolCallState.toolCallId]);

  const description =
    progress?.description || toolCallState.parsedArgs?.description || "Sub-agent";
  const task = progress?.task || toolCallState.parsedArgs?.task || "";
  const status = progress?.status || (toolCallState.status === "done" ? "completed" : "running");
  const currentAction = progress?.currentAction;
  const stepsCompleted = progress?.stepsCompleted || 0;
  const result = progress?.result;
  const error = progress?.error;

  return (
    <div className="mx-2 my-2 overflow-hidden rounded-lg border border-solid border-zinc-700/50">
      {/* Header - always visible */}
      <div
        className="flex cursor-pointer items-center gap-2 px-3 py-2 transition-colors hover:bg-zinc-800/30"
        onClick={() => setExpanded((prev) => !prev)}
      >
        <ChevronRightIcon
          className={`h-3.5 w-3.5 flex-shrink-0 text-zinc-400 transition-transform duration-200 ${
            expanded ? "rotate-90" : ""
          }`}
        />
        <UserGroupIcon className="h-4 w-4 flex-shrink-0 text-zinc-400" />
        <span className="min-w-0 flex-1 truncate text-xs font-medium text-zinc-200">
          {description}
        </span>
        <StatusBadge status={status} />
        {stepsCompleted > 0 && (
          <span className="text-[10px] text-zinc-500">
            {stepsCompleted} step{stepsCompleted !== 1 ? "s" : ""}
          </span>
        )}
      </div>

      {/* Current action line - visible when running */}
      {status === "running" && currentAction && !expanded && (
        <div className="border-t border-zinc-700/30 px-3 py-1.5">
          <span className="text-[11px] text-zinc-500">{currentAction}</span>
        </div>
      )}

      {/* Expanded content */}
      {expanded && (
        <div className="border-t border-zinc-700/30 px-3 py-2">
          {/* Task description */}
          <div className="mb-2">
            <div className="mb-1 text-[10px] font-medium uppercase tracking-wider text-zinc-500">
              Task
            </div>
            <div className="text-xs text-zinc-300">{task}</div>
          </div>

          {/* Current action */}
          {status === "running" && currentAction && (
            <div className="mb-2">
              <div className="mb-1 text-[10px] font-medium uppercase tracking-wider text-zinc-500">
                Current Action
              </div>
              <div className="flex items-center gap-1.5 text-xs text-zinc-400">
                <span className="h-1.5 w-1.5 animate-pulse rounded-full bg-blue-400" />
                {currentAction}
              </div>
            </div>
          )}

          {/* Result */}
          {result && (
            <div>
              <div className="mb-1 text-[10px] font-medium uppercase tracking-wider text-zinc-500">
                Result
              </div>
              <div className="max-h-60 overflow-y-auto rounded bg-zinc-900/50 p-2 text-xs">
                <StyledMarkdownPreview source={result} />
              </div>
            </div>
          )}

          {/* Error */}
          {error && (
            <div>
              <div className="mb-1 text-[10px] font-medium uppercase tracking-wider text-red-400">
                Error
              </div>
              <div className="rounded bg-red-900/20 p-2 text-xs text-red-300">
                {error}
              </div>
            </div>
          )}
        </div>
      )}
    </div>
  );
}
