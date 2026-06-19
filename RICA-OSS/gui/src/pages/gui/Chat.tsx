import {
  ArrowLeftIcon,
  ChatBubbleOvalLeftIcon,
} from "@heroicons/react/24/outline";
import { Editor, JSONContent } from "@tiptap/react";
import { ChatHistoryItem, InputModifiers } from "core";
import { renderChatMessage } from "core/util/messageContent";
import {
  useCallback,
  useContext,
  useEffect,
  useMemo,
  useRef,
  useState,
} from "react";
import { ErrorBoundary } from "react-error-boundary";
import styled from "styled-components";
import { Button, lightGray, vscBackground } from "../../components";
import { useFindWidget } from "../../components/find/FindWidget";
import TimelineItem from "../../components/gui/TimelineItem";
import { NewSessionButton } from "../../components/mainInput/belowMainInput/NewSessionButton";
import ThinkingBlockPeek from "../../components/mainInput/belowMainInput/ThinkingBlockPeek";
import ContinueInputBox from "../../components/mainInput/ContinueInputBox";
import { useOnboardingCard } from "../../components/OnboardingCard";
import StepContainer from "../../components/StepContainer";
import { TabBar } from "../../components/TabBar/TabBar";
import { IdeMessengerContext } from "../../context/IdeMessenger";
import { useWebviewListener } from "../../hooks/useWebviewListener";
import { useAppDispatch, useAppSelector } from "../../redux/hooks";
import {
  selectDoneApplyStates,
  selectPendingToolCalls,
} from "../../redux/selectors/selectToolCalls";
import { selectSessionTokens } from "../../redux/selectors/selectSessionTokens";
import { selectCurrentOrg } from "../../redux/slices/profilesSlice";
import {
  cancelToolCall,
  ChatHistoryItemWithMessageId,
  clearStreamError,
  newSession,
  updateSubAgentProgress,
  updateToolCallOutput,
} from "../../redux/slices/sessionSlice";
import { streamEditThunk } from "../../redux/thunks/edit";
import { loadLastSession, saveCurrentSession } from "../../redux/thunks/session";
import { streamResponseThunk } from "../../redux/thunks/streamResponse";
import { isJetBrains, isMetaEquivalentKeyPressed } from "../../util";
import MultiAgentNudge from "../../components/gui/MultiAgentNudge";
import UsageWarningToast from "../../components/gui/UsageWarningToast";
import { MultiAgentLockWarning } from "./MultiAgentLockWarning";
import { MultiAgentPanel } from "./MultiAgentPanel";
import { SessionUsageBar } from "./SessionUsageBar";
import { ToolCallDiv } from "./ToolCallDiv";

import { useStore } from "react-redux";
import { CliInstallBanner } from "../../components/CliInstallBanner";
import FeedbackDialog from "../../components/dialogs/FeedbackDialog";

import { FatalErrorIndicator } from "../../components/config/FatalErrorNotice";
import InlineErrorMessage from "../../components/mainInput/InlineErrorMessage";
import { resolveEditorContent } from "../../components/mainInput/TipTapEditor/utils/resolveEditorContent";
import { setDialogMessage, setShowDialog } from "../../redux/slices/uiSlice";
import { RootState } from "../../redux/store";
import { cancelStream } from "../../redux/thunks/cancelStream";
import { getLocalStorage, setLocalStorage } from "../../util/localStorage";
import { EmptyChatBody } from "./EmptyChatBody";
import { ExploreDialogWatcher } from "./ExploreDialogWatcher";
import { useAutoScroll } from "./useAutoScroll";

// Helper function to find the index of the latest conversation summary
function findLatestSummaryIndex(history: ChatHistoryItem[]): number {
  for (let i = history.length - 1; i >= 0; i--) {
    if (history[i].conversationSummary) {
      return i;
    }
  }
  return -1; // No summary found
}

const StepsDiv = styled.div`
  position: relative;
  background-color: transparent;

  & > * {
    position: relative;
  }

  .thread-message {
    margin: 0 0 0 1px;
  }
`;

export const MAIN_EDITOR_INPUT_ID = "main-editor-input";

// SessionTokenBadge — Phase 5.x port from v1.2.1.
// Lives above StepsDiv next to MultiAgentLockWarning + SessionUsageBar.
// Renders nothing when sessionTotal === 0 so chat-mode-no-traffic is clean.
// Click to expand: per-model breakdown + per-source (orchestrator vs sub-agent)
// breakdown + click-to-copy session id + ⬇ save log button hitting the
// multiagent/getSessionLog IPC channel (already wired in Phase 4 fix-0).
// Ported verbatim from v1.2.1 Chat.tsx — selectSessionTokens shape verified
// equivalent on refork (Phase 3b net-new selector).
function formatTokenCount(n: number): string {
  if (n >= 1_000_000) return `${(n / 1_000_000).toFixed(1)}M`;
  if (n >= 1_000) return `${(n / 1_000).toFixed(1)}k`;
  return `${n}`;
}

function formatExact(n: number): string {
  return n.toLocaleString();
}

function SourceBadge({ source }: { source: "api" | "estimated" | "mixed" }) {
  const color =
    source === "api" ? "bg-emerald-500/15 text-emerald-400 border-emerald-500/30" :
    source === "mixed" ? "bg-yellow-500/15 text-yellow-400 border-yellow-500/30" :
    "bg-zinc-700/50 text-zinc-400 border-zinc-600/30";
  const label =
    source === "api" ? "Real API Usage" :
    source === "mixed" ? "Mixed (API + Estimated)" :
    "Estimated (tiktoken)";
  return (
    <span className={`inline-flex items-center rounded px-1.5 py-0.5 text-[9px] font-medium border ${color}`}>
      {label}
    </span>
  );
}

function TokenRow({ label, value, color, indent }: { label: string; value: number; color?: string; indent?: boolean }) {
  if (value === 0) return null;
  return (
    <div className={`flex items-center justify-between py-[1px] ${indent ? "pl-3" : ""}`}>
      <span className={`text-[11px] ${color || "text-zinc-400"}`}>{label}</span>
      <span className={`text-[11px] font-mono ${color || "text-zinc-300"}`}>{formatExact(value)}</span>
    </div>
  );
}

function SessionTokenBadge() {
  const [expanded, setExpanded] = useState(false);
  const [copiedId, setCopiedId] = useState(false);
  const [downloadState, setDownloadState] = useState<"idle" | "loading" | "done" | "not_found">("idle");
  const cardRef = useRef<HTMLDivElement>(null);
  const ideMessenger = useContext(IdeMessengerContext);
  const sessionId = useAppSelector((state) => state.session.id);
  const tokens = useAppSelector(selectSessionTokens);
  const {
    sessionTotal, orchestratorTokens, agentTokens,
    totalReasoning, totalCacheRead, totalCacheCreation,
    usageSource, llmCalls, llmCallCount, agentCount,
  } = tokens;

  // Close card when clicking outside
  useEffect(() => {
    if (!expanded) return;
    const handler = (e: MouseEvent) => {
      if (cardRef.current && !cardRef.current.contains(e.target as Node)) {
        setExpanded(false);
      }
    };
    document.addEventListener("mousedown", handler);
    return () => document.removeEventListener("mousedown", handler);
  }, [expanded]);

  if (sessionTotal === 0) return null;

  const sourceColor =
    usageSource === "api" ? "text-emerald-500" :
    usageSource === "mixed" ? "text-yellow-500" :
    "text-zinc-600";
  const sourceLabel =
    usageSource === "api" ? "API" :
    usageSource === "mixed" ? "~API" :
    "~est";

  return (
    <div className="relative" ref={cardRef}>
      {/* Clickable text */}
      <div
        onClick={() => setExpanded(!expanded)}
        className="flex items-center justify-end gap-1.5 px-3 py-0.5 text-[10px] cursor-pointer select-none"
      >
        <span className="text-white/80">{formatTokenCount(sessionTotal)} tokens</span>
        <span className={`${sourceColor}`}>{sourceLabel}</span>
      </div>

      {/* Expanded detail card */}
      {expanded && (
        <div className="absolute right-2 top-full z-50 mt-1 w-80 rounded-lg border border-zinc-700/60 bg-zinc-900/95 shadow-xl backdrop-blur-sm overflow-hidden">
          {/* Header */}
          <div className="border-b border-zinc-700/40 px-3 py-2">
            <div className="flex items-center justify-between">
              <span className="text-[12px] font-medium text-zinc-200">Session Token Usage</span>
              <SourceBadge source={usageSource} />
            </div>
            {sessionId && (
              <div className="flex items-center gap-1.5 mt-1">
                <span
                  className="text-[9px] text-zinc-500 font-mono cursor-pointer hover:text-zinc-400 transition-colors"
                  onClick={(e) => {
                    e.stopPropagation();
                    navigator.clipboard.writeText(sessionId);
                    setCopiedId(true);
                    setTimeout(() => setCopiedId(false), 1500);
                  }}
                  title="Click to copy full session ID"
                >
                  ID: {sessionId.slice(0, 8)} {copiedId && <span className="text-emerald-500">✓</span>}
                </span>
                <span className="text-zinc-700">|</span>
                <span
                  className={`text-[9px] cursor-pointer transition-colors ${
                    downloadState === "done" ? "text-emerald-500" :
                    downloadState === "not_found" ? "text-red-400" :
                    downloadState === "loading" ? "text-zinc-500" :
                    "text-zinc-500 hover:text-zinc-300"
                  }`}
                  onClick={async (e) => {
                    e.stopPropagation();
                    if (downloadState === "loading") return;
                    setDownloadState("loading");
                    try {
                      const result: any = await ideMessenger.request("multiagent/getSessionLog" as any, { sessionId, workspacePath: (window as any).workspacePaths?.[0] });
                      if (result?.status === "error" || !result?.content?.found) {
                        setDownloadState("not_found");
                        setTimeout(() => setDownloadState("idle"), 2000);
                        return;
                      }
                      setDownloadState("done");
                      setTimeout(() => setDownloadState("idle"), 3000);
                    } catch {
                      setDownloadState("not_found");
                      setTimeout(() => setDownloadState("idle"), 2000);
                    }
                  }}
                  title="Download session log for sharing"
                >
                  {downloadState === "done" ? "✓ saved to session_logs/" :
                   downloadState === "not_found" ? "no log found" :
                   downloadState === "loading" ? "saving..." :
                   "⬇ save log"}
                </span>
              </div>
            )}
          </div>

          {/* Summary section */}
          <div className="border-b border-zinc-700/40 px-3 py-2">
            <div className="flex items-center justify-between mb-1">
              <span className="text-[10px] font-medium uppercase tracking-wider text-zinc-500">Summary</span>
              <span className="text-[10px] text-zinc-500">{llmCallCount} call{llmCallCount !== 1 ? "s" : ""}{agentCount > 0 ? ` + ${agentCount} agent${agentCount !== 1 ? "s" : ""}` : ""}</span>
            </div>
            <div className="flex items-center justify-between py-0.5">
              <span className="text-[12px] font-medium text-zinc-200">Total</span>
              <span className="text-[12px] font-mono font-medium text-zinc-100">{formatExact(sessionTotal)}</span>
            </div>
            <TokenRow label="Prompt (input)" value={orchestratorTokens.promptTokens + agentTokens.promptTokens} color="text-blue-400" />
            <TokenRow label="Completion (output)" value={orchestratorTokens.completionTokens + agentTokens.completionTokens} color="text-green-400" />
            <TokenRow label="Reasoning" value={totalReasoning} color="text-purple-400" />
            <TokenRow label="Cache read (reused)" value={totalCacheRead} color="text-sky-400" />
            <TokenRow label="Cache write (new)" value={totalCacheCreation} color="text-sky-300" />
          </div>

          {/* Orchestrator vs Agents breakdown */}
          {agentTokens.total > 0 && (
            <div className="border-b border-zinc-700/40 px-3 py-2">
              <div className="text-[10px] font-medium uppercase tracking-wider text-zinc-500 mb-1">Breakdown</div>
              <div className="flex items-center justify-between py-0.5">
                <span className="text-[11px] text-zinc-300">Orchestrator / Chat</span>
                <span className="text-[11px] font-mono text-zinc-300">{formatExact(orchestratorTokens.total)}</span>
              </div>
              <TokenRow label="Prompt" value={orchestratorTokens.promptTokens} indent color="text-blue-400/70" />
              <TokenRow label="Completion" value={orchestratorTokens.completionTokens} indent color="text-green-400/70" />
              <TokenRow label="Cache read" value={orchestratorTokens.cacheReadTokens} indent color="text-sky-400/70" />

              <div className="flex items-center justify-between py-0.5 mt-1">
                <span className="text-[11px] text-zinc-300">Sub-agents</span>
                <span className="text-[11px] font-mono text-zinc-300">{formatExact(agentTokens.total)}</span>
              </div>
              <TokenRow label="Prompt" value={agentTokens.promptTokens} indent color="text-blue-400/70" />
              <TokenRow label="Completion" value={agentTokens.completionTokens} indent color="text-green-400/70" />
              <TokenRow label="Cache read" value={agentTokens.cacheReadTokens} indent color="text-sky-400/70" />
            </div>
          )}

          {/* Per-model aggregated usage */}
          {llmCalls.length > 0 && (() => {
            // Aggregate calls by model name
            const byModel = new Map<string, { model: string; promptTokens: number; completionTokens: number; totalTokens: number; calls: number; hasActual: boolean }>();
            for (const call of llmCalls) {
              const existing = byModel.get(call.model);
              if (existing) {
                existing.promptTokens += call.promptTokens;
                existing.completionTokens += call.completionTokens;
                existing.totalTokens += call.totalTokens;
                existing.calls += 1;
                if (call.isActualUsage) existing.hasActual = true;
              } else {
                byModel.set(call.model, {
                  model: call.model,
                  promptTokens: call.promptTokens,
                  completionTokens: call.completionTokens,
                  totalTokens: call.totalTokens,
                  calls: 1,
                  hasActual: call.isActualUsage,
                });
              }
            }
            const aggregated = [...byModel.values()].sort((a, b) => b.totalTokens - a.totalTokens);

            return (
              <div className="px-3 py-2 max-h-40 overflow-y-auto">
                <div className="flex items-center justify-between mb-1">
                  <span className="text-[10px] font-medium uppercase tracking-wider text-zinc-500">Model Usage</span>
                  <span className="text-[9px] text-zinc-600 italic">{llmCalls.length} calls</span>
                </div>
                <div className="space-y-1">
                  {aggregated.map((entry, i) => (
                    <div key={entry.model} className="flex items-center gap-2 text-[10px]">
                      <span className="text-zinc-600 w-4 text-right shrink-0">#{i + 1}</span>
                      <span className="text-zinc-400 truncate flex-1" title={`${entry.model} (${entry.calls} calls)`}>{entry.model}</span>
                      <span className="text-zinc-600 shrink-0 text-[9px]">×{entry.calls}</span>
                      <span className="text-blue-400/70 shrink-0">{formatTokenCount(entry.promptTokens)}</span>
                      <span className="text-zinc-600 shrink-0">/</span>
                      <span className="text-green-400/70 shrink-0">{formatTokenCount(entry.completionTokens)}</span>
                      <span className={`shrink-0 text-[8px] font-medium ${entry.hasActual ? "text-emerald-500" : "text-zinc-600"}`}>
                        {entry.hasActual ? "API" : "est"}
                      </span>
                    </div>
                  ))}
                </div>
              </div>
            );
          })()}
        </div>
      )}
    </div>
  );
}

function fallbackRender({ error, resetErrorBoundary }: any) {
  // Call resetErrorBoundary() to reset the error boundary and retry the render.

  return (
    <div
      role="alert"
      className="px-2"
      style={{ backgroundColor: vscBackground }}
    >
      <p>Something went wrong:</p>
      <pre style={{ color: "red" }}>{error.message}</pre>
      <pre style={{ color: lightGray }}>{error.stack}</pre>

      <div className="text-center">
        <Button onClick={resetErrorBoundary}>Restart</Button>
      </div>
    </div>
  );
}

export function Chat() {
  const dispatch = useAppDispatch();
  const ideMessenger = useContext(IdeMessengerContext);
  const reduxStore = useStore<RootState>();
  const onboardingCard = useOnboardingCard();
  const showSessionTabs = useAppSelector(
    (store) => store.config.config.ui?.showSessionTabs,
  );
  const isStreaming = useAppSelector((state) => state.session.isStreaming);
  const streamError = useAppSelector((state) => state.session.streamError);
  const currentSessionId = useAppSelector((state) => state.session.id);
  const [stepsOpen] = useState<(boolean | undefined)[]>([]);
  const mainTextInputRef = useRef<HTMLInputElement>(null);
  const stepsDivRef = useRef<HTMLDivElement>(null);
  const tabsRef = useRef<HTMLDivElement>(null);
  const history = useAppSelector((state) => state.session.history);
  const showChatScrollbar = useAppSelector(
    (state) => state.config.config.ui?.showChatScrollbar,
  );
  const codeToEdit = useAppSelector((state) => state.editModeState.codeToEdit);
  const isInEdit = useAppSelector((store) => store.session.isInEdit);

  const lastSessionId = useAppSelector((state) => state.session.lastSessionId);
  const allSessionMetadata = useAppSelector(
    (state) => state.session.allSessionMetadata,
  );
  const hasDismissedExploreDialog = useAppSelector(
    (state) => state.ui.hasDismissedExploreDialog,
  );
  const mode = useAppSelector((state) => state.session.mode);
  const currentOrg = useAppSelector(selectCurrentOrg);
  const jetbrains = useMemo(() => {
    return isJetBrains();
  }, []);

  useAutoScroll(stepsDivRef, history);

  useEffect(() => {
    // Cmd + Backspace to delete current step
    const listener = (e: KeyboardEvent) => {
      if (
        e.key === "Backspace" &&
        (jetbrains ? e.altKey : isMetaEquivalentKeyPressed(e)) &&
        !e.shiftKey
      ) {
        void dispatch(cancelStream());
      }
    };
    window.addEventListener("keydown", listener);

    return () => {
      window.removeEventListener("keydown", listener);
    };
  }, [isStreaming, jetbrains, isInEdit]);

  const { widget, highlights } = useFindWidget(
    stepsDivRef,
    tabsRef,
    isStreaming,
  );

  const sendInput = useCallback(
    (
      editorState: JSONContent,
      modifiers: InputModifiers,
      index?: number,
      editorToClearOnSend?: Editor,
    ) => {
      const stateSnapshot = reduxStore.getState();
      // Clear any prior stream error on resubmit — the user is taking
      // another action, so the stale red banner is no longer relevant.
      if (stateSnapshot.session.streamError) {
        dispatch(clearStreamError());
      }
      const latestPendingToolCalls = selectPendingToolCalls(stateSnapshot);
      const latestPendingApplyStates = selectDoneApplyStates(stateSnapshot);
      const isCurrentlyInEdit = stateSnapshot.session.isInEdit;
      const codeToEditSnapshot = stateSnapshot.editModeState.codeToEdit;
      const selectedModelByRole =
        stateSnapshot.config.config.selectedModelByRole;

      // Cancel all pending tool calls
      latestPendingToolCalls.forEach((toolCallState) => {
        dispatch(
          cancelToolCall({
            toolCallId: toolCallState.toolCallId,
          }),
        );
      });

      // Reject all pending apply states
      latestPendingApplyStates.forEach((applyState) => {
        if (applyState.status !== "closed") {
          ideMessenger.post("rejectDiff", applyState);
        }
      });
      const model = isCurrentlyInEdit
        ? (selectedModelByRole.edit ?? selectedModelByRole.chat)
        : selectedModelByRole.chat;

      if (!model) {
        return;
      }

      if (isCurrentlyInEdit && codeToEditSnapshot.length === 0) {
        return;
      }

      if (isCurrentlyInEdit) {
        void dispatch(
          streamEditThunk({
            editorState,
            codeToEdit: codeToEditSnapshot,
          }),
        );
      } else {
        void dispatch(streamResponseThunk({ editorState, modifiers, index }));

        if (editorToClearOnSend) {
          editorToClearOnSend.commands.clearContent();
        }
      }

      // Increment localstorage counter for popup
      const currentCount = getLocalStorage("mainTextEntryCounter");
      if (currentCount) {
        setLocalStorage("mainTextEntryCounter", currentCount + 1);
        if (currentCount === 300) {
          dispatch(setDialogMessage(<FeedbackDialog />));
          dispatch(setShowDialog(true));
        }
      } else {
        setLocalStorage("mainTextEntryCounter", 1);
      }
    },
    [dispatch, ideMessenger, reduxStore],
  );

  useWebviewListener(
    "newSession",
    async () => {
      // unwrapResult(response) // errors if session creation failed
      mainTextInputRef.current?.focus?.();
    },
    [mainTextInputRef],
  );

  // Handle partial tool call output for streaming updates
  useWebviewListener(
    "toolCallPartialOutput",
    async (data) => {
      // Update tool call output in Redux store
      dispatch(
        updateToolCallOutput({
          toolCallId: data.toolCallId,
          contextItems: data.contextItems,
        }),
      );
    },
    [dispatch],
  );

  // Handle sub-agent progress updates (filtered by session). The orchestrator
  // posts these as sub-agents move through running -> completed/canceled/
  // errored. Persist to disk on each terminal event so the agent tree
  // survives extension restarts even if the post-stream save in
  // streamThunkWrapper races (or is skipped because the user closed the
  // window before the orchestrator wrapped). Skip running/pending — those
  // are high-frequency and the next terminal event will capture progress.
  useWebviewListener(
    "subAgentProgress",
    async (data) => {
      if (data.sessionId && data.sessionId !== currentSessionId) {
        return;
      }
      dispatch(updateSubAgentProgress(data));
      if (
        data.status === "completed" ||
        data.status === "canceled" ||
        data.status === "errored"
      ) {
        void dispatch(
          saveCurrentSession({ openNewSession: false, generateTitle: false }),
        );
      }
    },
    [dispatch, currentSessionId],
  );

  const isLastUserInput = useCallback(
    (index: number): boolean => {
      return !history
        .slice(index + 1)
        .some((entry) => entry.message.role === "user");
    },
    [history],
  );

  const renderChatHistoryItem = useCallback(
    (item: ChatHistoryItemWithMessageId, index: number) => {
      const {
        message,
        editorState,
        contextItems,
        appliedRules,
        toolCallStates,
      } = item;

      // Calculate once for the entire function
      const latestSummaryIndex = findLatestSummaryIndex(history);
      const isBeforeLatestSummary =
        latestSummaryIndex !== -1 && index < latestSummaryIndex;

      if (message.role === "user") {
        return (
          <ContinueInputBox
            onEnter={(editorState, modifiers) =>
              sendInput(editorState, modifiers, index)
            }
            isLastUserInput={isLastUserInput(index)}
            isMainInput={false}
            editorState={editorState ?? item.message.content}
            contextItems={contextItems}
            appliedRules={appliedRules}
            inputId={message.id}
          />
        );
      }

      if (message.role === "tool") {
        return null;
      }

      if (message.role === "assistant") {
        return (
          <>
            {/* Always render assistant content through normal path */}
            <div className="thread-message">
              <TimelineItem
                item={item}
                iconElement={
                  <ChatBubbleOvalLeftIcon width="16px" height="16px" />
                }
                open={
                  typeof stepsOpen[index] === "undefined"
                    ? true
                    : stepsOpen[index]!
                }
                onToggle={() => {}}
              >
                <StepContainer
                  index={index}
                  isLast={index === history.length - 1}
                  item={item}
                  latestSummaryIndex={latestSummaryIndex}
                />
              </TimelineItem>
            </div>

            {toolCallStates && (
              <ToolCallDiv
                toolCallStates={toolCallStates}
                historyIndex={index}
              />
            )}
          </>
        );
      }

      if (message.role === "thinking") {
        const thinkingContent = renderChatMessage(message);
        if (!thinkingContent?.trim()) {
          return null;
        }
        return (
          <div className={isBeforeLatestSummary ? "opacity-50" : ""}>
            <ThinkingBlockPeek
              content={thinkingContent}
              redactedThinking={message.redactedThinking}
              index={index}
              prevItem={index > 0 ? history[index - 1] : null}
              inProgress={index === history.length - 1 && isStreaming}
              signature={message.signature}
            />
          </div>
        );
      }

      // Default case - regular assistant message
      return (
        <div className="thread-message">
          <TimelineItem
            item={item}
            iconElement={<ChatBubbleOvalLeftIcon width="16px" height="16px" />}
            open={
              typeof stepsOpen[index] === "undefined" ? true : stepsOpen[index]!
            }
            onToggle={() => {}}
          >
            <StepContainer
              index={index}
              isLast={index === history.length - 1}
              item={item}
              latestSummaryIndex={latestSummaryIndex}
            />
          </TimelineItem>
        </div>
      );
    },
    [sendInput, isLastUserInput, history, stepsOpen, isStreaming],
  );

  const showScrollbar = showChatScrollbar ?? window.innerHeight > 5000;

  return (
    <>
      {/* Auto-dismissing toast when active model crosses 90% consumption */}
      <UsageWarningToast />

      {/* One-time onboarding nudge pointing at the mode selector */}
      <MultiAgentNudge />

      {!!showSessionTabs && !isInEdit && <TabBar ref={tabsRef} />}
      {widget}

      {/* Static multi-agent progress panel — shows whenever agents exist */}
      <MultiAgentPanel />

      {/* Mid-run warning when the session-locked model has been blocked */}
      <MultiAgentLockWarning />

      {/* Session model usage bar — visible in all modes when active model has a limit */}
      <SessionUsageBar />

      {/* Session token count — visible in all modes (renders null when sessionTotal===0) */}
      <SessionTokenBadge />

      <StepsDiv
        ref={stepsDivRef}
        className={`overflow-y-scroll pt-[8px] ${showScrollbar ? "thin-scrollbar" : "no-scrollbar"} ${history.length > 0 ? "flex-1" : ""}`}
      >
        {highlights}
        {history
          .filter((item) => item.message.role !== "system")
          .map((item, index: number) => (
            <div
              key={item.message.id}
              style={{
                minHeight: index === history.length - 1 ? "200px" : 0,
              }}
            >
              <ErrorBoundary
                FallbackComponent={fallbackRender}
                onReset={() => {
                  dispatch(newSession());
                }}
              >
                {renderChatHistoryItem(item, index)}
              </ErrorBoundary>
              {index === history.length - 1 && <InlineErrorMessage />}
            </div>
          ))}
      </StepsDiv>
      <div className={"relative"}>
        <ContinueInputBox
          isMainInput
          isLastUserInput={false}
          onEnter={(editorState, modifiers, editor) =>
            sendInput(editorState, modifiers, undefined, editor)
          }
          inputId={MAIN_EDITOR_INPUT_ID}
        />

        {streamError && (
          <div className="mx-2 mb-2 flex items-center gap-2 rounded-md border border-red-500/30 bg-red-500/10 px-3 py-2">
            <span className="text-xs text-red-400">
              Stream interrupted:{" "}
              {streamError.length > 80
                ? streamError.slice(0, 77) + "..."
                : streamError}
            </span>
            <button
              className="ml-auto rounded bg-red-500/20 px-2 py-0.5 text-[10px] text-red-300 hover:bg-red-500/30"
              onClick={() => dispatch(clearStreamError())}
            >
              Dismiss
            </button>
          </div>
        )}

        <CliInstallBanner
          sessionCount={allSessionMetadata.length}
          sessionThreshold={3}
          permanentDismissal={true}
        />

        <div
          style={{
            pointerEvents: isStreaming ? "none" : "auto",
          }}
        >
          <div className="flex flex-row items-center justify-between pb-1 pl-0.5 pr-2">
            <div className="xs:inline hidden">
              {history.length === 0 && lastSessionId && !isInEdit && (
                <NewSessionButton
                  onClick={async () => {
                    await dispatch(loadLastSession());
                  }}
                  className="flex items-center gap-2"
                >
                  <ArrowLeftIcon className="h-3 w-3" />
                  <span className="text-xs">Last Session</span>
                </NewSessionButton>
              )}
            </div>
          </div>
          <FatalErrorIndicator />
          {!hasDismissedExploreDialog && <ExploreDialogWatcher />}
          {history.length === 0 && (
            <EmptyChatBody showOnboardingCard={onboardingCard.show} />
          )}
        </div>
      </div>
    </>
  );
}
