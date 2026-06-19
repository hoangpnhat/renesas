// Phase 5.x — v1.2.0 parity: "Cancel All / Cancel agents / per-agent" dropdown
// shown above the input bar while sub-agents are running. v1.2.1's
// StreamingToolbar bundled this into one component; refork rewrote
// StreamingToolbar to a tiny prop-driven stop button (LumpToolbar passes
// onStop). Splitting the dropdown variant out keeps refork's API intact —
// LumpToolbar routes here when subAgentProgress has running agents,
// otherwise it falls through to the simple <StreamingToolbar onStop=... />.
//
// Logic mirrors v1.2.1 verbatim (handleCancelAll cancels every agent
// AND cancels the orchestrator stream; handleCancelAllAgents only
// cancels agents; per-agent buttons cancel one at a time).
import {
  ChevronDownIcon,
  CpuChipIcon,
  StopCircleIcon,
  XMarkIcon,
} from "@heroicons/react/24/outline";
import { useContext, useEffect, useMemo, useRef, useState } from "react";
import { IdeMessengerContext } from "../../../../context/IdeMessenger";
import { useAppDispatch, useAppSelector } from "../../../../redux/hooks";
import { cancelStream } from "../../../../redux/thunks/cancelStream";
import { getAltKeyLabel, getMetaKeyLabel, isJetBrains } from "../../../../util";
import { GeneratingIndicator } from "./GeneratingIndicator";

export function StreamingToolbarWithAgents() {
  const dispatch = useAppDispatch();
  const ideMessenger = useContext(IdeMessengerContext);
  const jetbrains = isJetBrains();
  const [showMenu, setShowMenu] = useState(false);
  const menuRef = useRef<HTMLDivElement>(null);

  const subAgentProgress = useAppSelector(
    (state) => state.session.subAgentProgress,
  );

  const runningAgents = useMemo(
    () => Object.values(subAgentProgress).filter((a) => a.status === "running"),
    [subAgentProgress],
  );

  // Close dropdown on outside click
  useEffect(() => {
    if (!showMenu) return;
    const handleClickOutside = (event: MouseEvent) => {
      if (menuRef.current && !menuRef.current.contains(event.target as Node)) {
        setShowMenu(false);
      }
    };
    document.addEventListener("mousedown", handleClickOutside);
    return () => document.removeEventListener("mousedown", handleClickOutside);
  }, [showMenu]);

  const handleCancelAll = () => {
    for (const agent of runningAgents) {
      ideMessenger.post("cancelSubAgent", {
        subAgentId: agent.subAgentId,
      });
    }
    void dispatch(cancelStream());
    setShowMenu(false);
  };

  const handleCancelAllAgents = () => {
    for (const agent of runningAgents) {
      ideMessenger.post("cancelSubAgent", {
        subAgentId: agent.subAgentId,
      });
    }
    setShowMenu(false);
  };

  const handleCancelAgent = (subAgentId: string) => {
    ideMessenger.post("cancelSubAgent", { subAgentId });
    setShowMenu(false);
  };

  const shortcutLabel = jetbrains ? getAltKeyLabel() : getMetaKeyLabel();

  return (
    <div className="flex w-full items-center justify-between">
      <GeneratingIndicator />
      <div className="relative" ref={menuRef}>
        <div className="flex items-center gap-0.5">
          <div
            onClick={handleCancelAll}
            className="text-2xs cursor-pointer px-1.5 py-0.5 hover:brightness-125"
          >
            <span className="text-description">Cancel All</span>
            <span className="text-description-muted ml-1 opacity-75">
              {shortcutLabel}⌫
            </span>
          </div>
          <button
            className="rounded-md bg-zinc-800/50 p-1 text-zinc-400 transition-all hover:bg-zinc-700/70 hover:text-zinc-200"
            onClick={() => setShowMenu(!showMenu)}
            title="Cancel options"
          >
            <ChevronDownIcon className="h-3 w-3" />
          </button>
        </div>

        {showMenu && (
          <div className="absolute bottom-full right-0 z-50 mb-1 w-56 overflow-hidden rounded-xl border border-solid border-zinc-700/60 bg-zinc-900/95 shadow-2xl backdrop-blur-sm">
            <div className="px-1.5 py-1.5">
              <button
                className="flex w-full items-center gap-2 rounded-lg px-2.5 py-2 text-left text-xs text-zinc-200 transition-colors hover:bg-zinc-800/80"
                onClick={handleCancelAll}
              >
                <XMarkIcon className="h-3.5 w-3.5 text-red-400" />
                Cancel everything
              </button>
              <button
                className="flex w-full items-center gap-2 rounded-lg px-2.5 py-2 text-left text-xs text-zinc-200 transition-colors hover:bg-zinc-800/80"
                onClick={handleCancelAllAgents}
              >
                <StopCircleIcon className="h-3.5 w-3.5 text-yellow-400" />
                Cancel all agents ({runningAgents.length})
              </button>
            </div>
            <div className="mx-2 border-t border-zinc-700/40" />
            <div className="px-1.5 py-1.5">
              {runningAgents.map((agent) => (
                <button
                  key={agent.subAgentId}
                  className="flex w-full items-center gap-2 rounded-lg px-2.5 py-2 text-left text-xs text-zinc-400 transition-colors hover:bg-zinc-800/80 hover:text-zinc-200"
                  onClick={() => handleCancelAgent(agent.subAgentId)}
                >
                  <CpuChipIcon className="h-3.5 w-3.5 flex-shrink-0 text-zinc-500" />
                  <span className="truncate">
                    Cancel: {agent.role || agent.description || agent.subAgentId.slice(0, 8)}
                  </span>
                </button>
              ))}
            </div>
          </div>
        )}
      </div>
    </div>
  );
}
