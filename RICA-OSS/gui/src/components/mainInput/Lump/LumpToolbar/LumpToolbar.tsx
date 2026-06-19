import { BuiltInToolNames } from "core/tools/builtIn";
import { useContext, useEffect } from "react";
import { IdeMessengerContext } from "../../../../context/IdeMessenger";
import { useAppDispatch, useAppSelector } from "../../../../redux/hooks";
import {
  selectFirstPendingToolCall,
  selectPendingToolCalls,
  selectToolCallsByStatus,
} from "../../../../redux/selectors/selectToolCalls";
import { callToolById } from "../../../../redux/thunks/callToolById";
import { cancelStream } from "../../../../redux/thunks/cancelStream";
// RICA Multi-Agent: route Cancel through cancelToolCallThunk so the
// orchestrator gets a "tool cancelled" message and continues. Dispatching
// the bare `cancelToolCall` slice action leaves it silent — the
// regression v1.2.1 (commit accae9e92) closed for the toolbar Reject
// button. The keyboard shortcut path on this branch had reverted to the
// bare slice action; Phase 4 verify caught it.
import { cancelToolCallThunk } from "../../../../redux/thunks/cancelToolCall";
import { logToolUsage } from "../../../../redux/util";
import { isJetBrains } from "../../../../util";
import { useMainEditor } from "../../TipTapEditor";
import { BlockSettingsTopToolbar } from "./BlockSettingsTopToolbar";
import { EditOutcomeToolbar } from "./EditOutcomeToolbar";
import { EditToolbar } from "./EditToolbar";
import { IsApplyingToolbar } from "./IsApplyingToolbar";
import { PendingApplyStatesToolbar } from "./PendingApplyStatesToolbar";
import { PendingToolCallToolbar } from "./PendingToolCallToolbar";
import { StreamingToolbar } from "./StreamingToolbar";
import { StreamingToolbarWithAgents } from "./StreamingToolbarWithAgents";
import { TtsActiveToolbar } from "./TtsActiveToolbar";

// Keyboard shortcut detection utilities
const isExecuteToolCallShortcut = (event: KeyboardEvent) => {
  const metaKey = event.metaKey || event.ctrlKey;
  return metaKey && event.key === "Enter";
};

const isCancelToolCallShortcut = (
  event: KeyboardEvent,
  isJetBrains: boolean,
) => {
  const metaKey = event.metaKey || event.ctrlKey;
  const altKey = event.altKey;
  const modifierKey = isJetBrains ? altKey : metaKey;
  return modifierKey && event.key === "Backspace";
};

// Check if a tool call is a terminal command
const isTerminalCommand = (toolCallState: any) => {
  return (
    toolCallState?.toolCall?.function?.name ===
    BuiltInToolNames.RunTerminalCommand
  );
};

export function LumpToolbar() {
  const dispatch = useAppDispatch();
  const ideMessenger = useContext(IdeMessengerContext);
  const ttsActive = useAppSelector((state) => state.ui.ttsActive);
  const isStreaming = useAppSelector((state) => state.session.isStreaming);
  const isInEdit = useAppSelector((state) => state.session.isInEdit);
  const jetbrains = isJetBrains();
  const pendingToolCalls = useAppSelector(selectPendingToolCalls);
  const firstPendingToolCall = useAppSelector(selectFirstPendingToolCall);
  const editApplyState = useAppSelector(
    (state) => state.editModeState.applyState,
  );
  const applyStates = useAppSelector(
    (state) => state.session.codeBlockApplyStates.states,
  );
  const pendingApplyStates = applyStates.filter(
    (state) => state.status === "done",
  );
  const isApplying = applyStates.some((state) => state.status === "streaming");
  const editor = useMainEditor();

  // Get ALL running terminal commands
  const runningToolCalls = useAppSelector((state) =>
    selectToolCallsByStatus(state, "calling"),
  );
  const runningTerminalCalls = runningToolCalls.filter(isTerminalCommand);
  const hasRunningTerminalCommand = runningTerminalCalls.length > 0;

  // RICA Multi-Agent: when sub-agents are running, swap the simple stop
  // button for the v1.2.0-style "Cancel All / Cancel agents / per-agent"
  // dropdown. Mirrors v1.2.1 StreamingToolbar behavior on the new UI.
  const subAgentProgress = useAppSelector(
    (state) => state.session.subAgentProgress,
  );
  const hasRunningSubAgents = Object.values(subAgentProgress).some(
    (a) => a.status === "running",
  );

  // Simple handler: stop ALL running terminal commands
  const handleStopAllTerminalCommands = async () => {
    if (runningTerminalCalls.length === 0) {
      return;
    }

    // Stop all terminal commands concurrently
    const stopPromises = runningTerminalCalls.map(async (terminalCall) => {
      try {
        // Cancel the process on the backend
        await ideMessenger.request("process/killTerminalProcess", {
          toolCallId: terminalCall.toolCallId,
        });

        // Cancel the tool call in the UI. Route through the thunk so the
        // orchestrator gets a "tool cancelled" message and continues —
        // dispatching the bare slice action would leave it silent (same
        // regression v1.2.1 closed for the keyboard / Reject button).
        void dispatch(
          cancelToolCallThunk({
            toolCallId: terminalCall.toolCallId,
          }),
        );

        logToolUsage(terminalCall, false, true, ideMessenger);
      } catch (error) {
        console.error(
          `Failed to cancel terminal command ${terminalCall.toolCallId}:`,
          error,
        );
      }
    });

    // Wait for all cancellations to complete
    await Promise.all(stopPromises);
  };

  // Combined stop handler
  const handleStopAction = async () => {
    // Stop all terminal commands if any are running
    if (hasRunningTerminalCommand) {
      await handleStopAllTerminalCommands();
    }

    // Also stop regular streaming if it's happening
    if (isStreaming) {
      dispatch(cancelStream());
    }
  };

  useEffect(() => {
    if (!firstPendingToolCall && !hasRunningTerminalCommand) {
      return;
    }

    const handleToolCallKeyboardShortcuts = (event: KeyboardEvent) => {
      if (isExecuteToolCallShortcut(event) && firstPendingToolCall) {
        event.preventDefault();
        event.stopPropagation();

        void dispatch(
          callToolById({ toolCallId: firstPendingToolCall.toolCallId }),
        );
      } else if (isCancelToolCallShortcut(event, jetbrains)) {
        event.preventDefault();
        event.stopPropagation();

        if (hasRunningTerminalCommand) {
          // Stop running terminal commands
          void handleStopAction();
        } else if (firstPendingToolCall) {
          // Cancel pending tool call. If last call, focus editor
          if (pendingToolCalls.length === 1) {
            editor.mainEditor?.commands.focus();
          }
          void dispatch(
            cancelToolCallThunk({
              toolCallId: firstPendingToolCall.toolCallId,
            }),
          );
          logToolUsage(firstPendingToolCall, false, true, ideMessenger);
        }
      }
    };

    document.addEventListener("keydown", handleToolCallKeyboardShortcuts);
    return () => {
      document.removeEventListener("keydown", handleToolCallKeyboardShortcuts);
    };
  }, [
    firstPendingToolCall,
    pendingToolCalls,
    editor,
    hasRunningTerminalCommand,
    runningTerminalCalls,
  ]);

  if (isApplying) {
    return <IsApplyingToolbar />;
  }

  if (isInEdit) {
    if (editApplyState.status === "done") {
      return <EditOutcomeToolbar />;
    }

    return <EditToolbar />;
  }

  if (ttsActive) {
    return <TtsActiveToolbar />;
  }

  // RICA Multi-Agent: while sub-agents are running, the orchestrator is
  // paused inside the spawn_agents tool call so isStreaming===false and
  // pendingToolCalls===[the spawn call]. Without this branch we'd fall
  // through to PendingToolCallToolbar (which renders Accept/Reject for
  // editFile-style tools) and lose the per-agent cancel UI entirely.
  // Take priority over the streaming/terminal/pending checks so the user
  // can always reach the cancel dropdown while agents are running.
  if (hasRunningSubAgents) {
    return <StreamingToolbarWithAgents />;
  }

  // Only show terminal streaming for actual terminal commands
  if (hasRunningTerminalCommand) {
    const count = runningTerminalCalls.length;
    const stopText = `Stop Terminal${count > 1 ? ` (${count})` : ""}`;
    return (
      <StreamingToolbar onStop={handleStopAction} displayText={stopText} />
    );
  }

  // Regular streaming (non-terminal)
  if (isStreaming) {
    return <StreamingToolbar onStop={() => dispatch(cancelStream())} />;
  }

  if (pendingToolCalls.length > 0) {
    return <PendingToolCallToolbar />;
  }

  if (pendingApplyStates.length > 0) {
    return (
      <PendingApplyStatesToolbar pendingApplyStates={pendingApplyStates} />
    );
  }

  return <BlockSettingsTopToolbar />;
}
