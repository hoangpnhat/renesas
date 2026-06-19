import { createAsyncThunk } from "@reduxjs/toolkit";
import posthog from "posthog-js";
import StreamErrorDialog from "../../pages/gui/StreamError";
import { analyzeError } from "../../util/errorAnalysis";
import { selectSelectedChatModel } from "../slices/configSlice";
import { setDialogMessage, setShowDialog } from "../slices/uiSlice";
import { ThunkApiType } from "../store";
import { cancelStream } from "./cancelStream";
import { saveCurrentSession } from "./session";

export const streamThunkWrapper = createAsyncThunk<
  void,
  () => Promise<void>,
  ThunkApiType
>("chat/streamWrapper", async (runStream, { dispatch, getState, extra }) => {
  try {
    await runStream();
    const state = getState();
    if (!state.session.isInEdit) {
      await dispatch(
        saveCurrentSession({
          openNewSession: false,
          generateTitle: true,
        }),
      );
    }
  } catch (e) {
    const state = getState();
    const selectedModel = selectSelectedChatModel(state);
    const { parsedError, statusCode, modelTitle, providerName } = analyzeError(
      e,
      selectedModel,
    );

    // Snapshot intentional-stop flag BEFORE cancelStream() flips isStreaming.
    // If the user already pressed stop / started a new session, don't show
    // the error dialog and don't log this as an LLM error.
    const wasIntentionallyStopped = !state.session.isStreaming;
    await dispatch(cancelStream());

    // Multi-agent: capture the error in the orchestrator session log so the
    // export bundle and tester reports can pinpoint the failure. Stringify
    // carefully — bare `String(obj)` yields "[object Object]" which loses
    // all debugging context.
    if (state.session.mode === "multi-agent" && state.session.id) {
      let errorMsg: string;
      if (e instanceof Error) {
        errorMsg = e.message + (e.stack ? `\n${e.stack.slice(0, 1500)}` : "");
      } else if (e && typeof e === "object") {
        const anyE = e as any;
        if (typeof anyE.message === "string") {
          errorMsg = anyE.message;
          if (anyE.status) errorMsg = `[HTTP ${anyE.status}] ${errorMsg}`;
          if (anyE.code) errorMsg = `[${anyE.code}] ${errorMsg}`;
        } else {
          try {
            errorMsg = JSON.stringify(e, null, 2);
          } catch {
            errorMsg = String(e); // fallback for circular refs
          }
        }
      } else {
        errorMsg = String(e);
      }
      const errorType = wasIntentionallyStopped ? "cancel" : "llm_error";
      extra.ideMessenger.post("multiagent/logError", {
        sessionId: state.session.id,
        errorType,
        message: errorMsg.slice(0, 2000),
      });
    }

    // Save the session even when the stream errored. Preserves in-flight
    // subAgentProgress and sessionModelLock to disk so a refresh / restart
    // doesn't lose them. Common error paths that hit here:
    //   - cancel-on-block hook aborting agents mid-flight
    //   - MODEL RATE-LIMITED / INSUFFICIENT BUDGET throws from spawnAgents
    //   - any backend HTTP error during the orchestrator's stream
    // Skip when no history yet (nothing to save) or no session id.
    const postCancelState = getState();
    console.log(
      `[LOCK-DEBUG] save-on-error: mode=${postCancelState.session.mode} ` +
        `lock=${postCancelState.session.sessionModelLock?.modelTitle ?? "none"} ` +
        `subagentCount=${Object.keys(postCancelState.session.subAgentProgress ?? {}).length} ` +
        `historyLen=${postCancelState.session.history.length} ` +
        `id=${postCancelState.session.id?.slice(0, 8)}`,
    );
    if (
      postCancelState.session.history.length > 0 &&
      postCancelState.session.id &&
      (postCancelState.session.mode === "chat" ||
        postCancelState.session.mode === "agent" ||
        postCancelState.session.mode === "multi-agent")
    ) {
      try {
        await dispatch(
          saveCurrentSession({
            openNewSession: false,
            generateTitle: false,
          }),
        );
      } catch (saveErr) {
        console.error(
          "[streamThunkWrapper] save-on-error failed:",
          saveErr,
        );
      }
    }

    if (!wasIntentionallyStopped) {
      dispatch(setDialogMessage(<StreamErrorDialog error={e} />));
      dispatch(setShowDialog(true));
    }

    const errorData = {
      error_type: statusCode ? `HTTP ${statusCode}` : "Unknown",
      error_message: parsedError,
      model_provider: providerName,
      model_title: modelTitle,
    };

    posthog.capture("gui_stream_error", errorData);
  }
});
