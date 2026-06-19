import { createAsyncThunk, unwrapResult } from "@reduxjs/toolkit";
import { LLMFullCompletionOptions, ModelDescription } from "core";
import { getRuleId } from "core/llm/rules/getSystemMessageWithRules";
import { ToCoreProtocol } from "core/protocol";
import { BUILT_IN_GROUP_NAME } from "core/tools/builtIn";
import { selectActiveTools } from "../selectors/selectActiveTools";
import { selectSelectedChatModel } from "../slices/configSlice";
import {
  abortStream,
  addPromptCompletionPair,
  errorToolCall,
  setActive,
  setAppliedRulesAtIndex,
  setContextPercentage,
  setInactive,
  setInlineErrorMessage,
  setIsPruned,
  setSessionModelLock,
  setStreamError,
  setToolGenerated,
  streamUpdate,
} from "../slices/sessionSlice";
import { ThunkApiType } from "../store";
import { formatBlockedCountdown } from "../../util/consumptionLimit";
import { constructMessages } from "../util/constructMessages";
import { saveCurrentSession } from "./session";

import { modelSupportsNativeTools } from "core/llm/toolSupport";
import { applyToolOverrides } from "core/tools/applyToolOverrides";
import { addSystemMessageToolsToSystemMessage } from "core/tools/systemMessageTools/buildToolsSystemMessage";
import { interceptSystemToolCalls } from "core/tools/systemMessageTools/interceptSystemToolCalls";
import { SystemMessageToolCodeblocksFramework } from "core/tools/systemMessageTools/toolCodeblocks";
import posthog from "posthog-js";
import {
  selectCurrentToolCalls,
  selectPendingToolCalls,
} from "../selectors/selectToolCalls";
import { selectSessionTokens } from "../selectors/selectSessionTokens";
import { getBaseSystemMessage } from "../util/getBaseSystemMessage";
import { callToolById } from "./callToolById";
import { evaluateToolPolicies } from "./evaluateToolPolicies";
import { preprocessToolCalls } from "./preprocessToolCallArgs";
import { streamResponseAfterToolCall } from "./streamResponseAfterToolCall";

/**
 * Builds completion options with reasoning configuration based on session state and model capabilities.
 *
 * @param baseOptions - Base completion options to extend
 * @param hasReasoningEnabled - Whether reasoning is enabled in the session
 * @param model - The selected model with provider and completion options
 * @returns Completion options with reasoning configuration
 */
function buildReasoningCompletionOptions(
  baseOptions: LLMFullCompletionOptions,
  hasReasoningEnabled: boolean | undefined,
  model: ModelDescription,
): LLMFullCompletionOptions {
  if (hasReasoningEnabled === undefined) {
    return baseOptions;
  }

  const reasoningOptions: LLMFullCompletionOptions = {
    ...baseOptions,
    reasoning: !!hasReasoningEnabled,
  };

  // Add reasoning budget tokens if reasoning is enabled and provider supports it
  if (hasReasoningEnabled && model.underlyingProviderName !== "ollama") {
    // Ollama doesn't support limiting reasoning tokens at this point
    reasoningOptions.reasoningBudgetTokens =
      model.completionOptions?.reasoningBudgetTokens ?? 2048;
  }

  return reasoningOptions;
}

export const streamNormalInput = createAsyncThunk<
  void,
  {
    legacySlashCommandData?: ToCoreProtocol["llm/streamChat"][0]["legacySlashCommandData"];
    depth?: number;
  },
  ThunkApiType
>(
  "chat/streamNormalInput",
  async (
    { legacySlashCommandData, depth = 0 },
    { dispatch, extra, getState },
  ) => {
    if (process.env.NODE_ENV === "test" && depth > 50) {
      const message = `Max stream depth of ${50} reached in test`;
      console.error(message, JSON.stringify(getState(), null, 2));
      throw new Error(message);
    }
    const state = getState();

    // When a multi-agent session lock is set, the locked model is the
    // single source of truth for the stream target — Effect 1 in
    // MultiAgentModelSelect will eventually sync `selectedModelByRole.chat`
    // to match, but its timing is racy on session restore (a past session
    // can momentarily target a stale chat-role model while sessionModelLock
    // already points at the right one). Look up by title from the chat
    // models list, exactly like the picker chip does.
    const sessionModelLock = state.session.sessionModelLock;
    const chatModels = state.config.config.modelsByRole?.chat ?? [];
    const lockedChatModel = sessionModelLock
      ? chatModels.find((m) => m.title === sessionModelLock.modelTitle) ?? null
      : null;

    // Lock is set but the locked model isn't in chatModels — sign-out,
    // RICA Dynamic profile missing, or backend dropped the model. Refuse
    // to silently fall back to the local Assistant (which would route the
    // session through the wrong model and burn through Local without the
    // user knowing). Surface the error and let the user resolve it.
    if (sessionModelLock && !lockedChatModel) {
      const msg = `Model "${sessionModelLock.modelTitle}" is locked for this session but is not currently available. Sign in again or check your RICA configuration, then retry.`;
      dispatch(setStreamError(msg));
      dispatch(setInactive());
      return;
    }

    const selectedChatModel = lockedChatModel ?? selectSelectedChatModel(state);

    if (!selectedChatModel) {
      throw new Error("No chat model selected");
    }

    // Refuse new prompts when the chosen model is rate-limited — applies
    // to ALL modes. In multi-agent the lock prevents auto-switch, so this
    // is the only stop. In chat/agent, ParallelListeners.tsx normally
    // auto-switches to an unblocked alternative before the user submits,
    // but if config update + submit race or no alternative exists, this
    // catches it. In-flight streams are never aborted here — that path
    // lives in ParallelListeners' configUpdate hook.
    if (selectedChatModel.consumptionLimit?.blocked === true) {
      const until = selectedChatModel.consumptionLimit.blockedUntil;
      const msg = `Model "${selectedChatModel.title}" is rate-limited. ${formatBlockedCountdown(until)}.${state.session.mode === "multi-agent" ? " Start a new chat to use a different model." : ""}`;
      dispatch(setStreamError(msg));
      dispatch(setInactive());
      return;
    }

    // Commit the session model lock at submit time (not after first response).
    // The user has clicked send — they've committed to this model. Locking
    // here means a stream error doesn't lose the lock from disk on save,
    // and a save-on-error in streamThunkWrapper preserves it for restart.
    // Pre-send swap freedom is still preserved: until you hit send, the
    // picker stays open.
    console.log(
      `[LOCK-DEBUG] submit-time check: mode=${state.session.mode} existingLock=${state.session.sessionModelLock?.modelTitle ?? "none"} selectedTitle=${selectedChatModel?.title ?? "(none)"}`,
    );
    if (
      state.session.mode === "multi-agent" &&
      !state.session.sessionModelLock &&
      selectedChatModel.title
    ) {
      console.log(
        `[LOCK-DEBUG] dispatching setSessionModelLock("${selectedChatModel.title}")`,
      );
      dispatch(setSessionModelLock(selectedChatModel.title));
      // Persist the lock to disk immediately. Don't wait for the post-stream
      // save in streamThunkWrapper — if the stream throws or the user closes
      // the window mid-stream, the lock would otherwise never reach disk and
      // the session would reload as unlocked. Fire-and-forget; saveCurrentSession
      // reads its own state at execution time so it sees the just-dispatched lock.
      void dispatch(
        saveCurrentSession({ openNewSession: false, generateTitle: false }),
      );
    }

    // Get tools and apply model-level overrides (disabled, description, etc.)
    let activeTools = selectActiveTools(state);
    if (selectedChatModel.toolOverrides?.length) {
      const { tools: overriddenTools, errors } = applyToolOverrides(
        activeTools,
        selectedChatModel.toolOverrides,
      );
      activeTools = overriddenTools;
      for (const error of errors) {
        if (!error.fatal) {
          console.warn(`Tool override warning: ${error.message}`);
        }
      }
    }

    // Use the centralized selector to determine if system message tools should be used
    const useNativeTools = state.config.config.experimental
      ?.onlyUseSystemMessageTools
      ? false
      : modelSupportsNativeTools(selectedChatModel);
    const systemToolsFramework = !useNativeTools
      ? new SystemMessageToolCodeblocksFramework()
      : undefined;

    // Construct completion options
    let completionOptions: LLMFullCompletionOptions = {};
    if (useNativeTools && activeTools.length > 0) {
      completionOptions = {
        tools: activeTools,
      };
    }

    completionOptions = buildReasoningCompletionOptions(
      completionOptions,
      state.session.hasReasoningEnabled,
      selectedChatModel,
    );

    // Construct messages (excluding system message).
    // Multi-agent extras: workflow pattern, registered sub-agents, prior-mode
    // transition flag, and the per-turn budget snapshot (backend consumed +
    // session-local weighted total; see selectSessionTokens for the formula).
    const priorHistoryFromOtherMode =
      state.session.mode === "multi-agent" &&
      state.session.sessionModelLock === null &&
      state.session.history.length > 1;
    const sessionTokens = selectSessionTokens(state);
    const sessionLocalWeighted =
      (sessionTokens?.orchestratorTokens?.promptTokens ?? 0) +
      (sessionTokens?.agentTokens?.promptTokens ?? 0) +
      ((sessionTokens?.orchestratorTokens?.completionTokens ?? 0) +
        (sessionTokens?.agentTokens?.completionTokens ?? 0)) *
        5;
    const baseSystemMessage = getBaseSystemMessage(
      state.session.mode,
      selectedChatModel,
      activeTools,
      state.session.workflowPattern,
      state.session.subAgentProgress,
      priorHistoryFromOtherMode,
      selectedChatModel.consumptionLimit,
      sessionLocalWeighted,
    );

    const systemMessage = systemToolsFramework
      ? addSystemMessageToolsToSystemMessage(
          systemToolsFramework,
          baseSystemMessage,
          activeTools,
        )
      : baseSystemMessage;

    const withoutMessageIds = state.session.history.map((item) => {
      const { id, ...messageWithoutId } = item.message;
      return { ...item, message: messageWithoutId };
    });

    const { messages, appliedRules, appliedRuleIndex } = constructMessages(
      withoutMessageIds,
      systemMessage,
      state.config.config.rules,
      state.ui.ruleSettings,
      systemToolsFramework,
    );

    // TODO parallel tool calls will cause issues with this
    // because there will be multiple tool messages, so which one should have applied rules?
    dispatch(
      setAppliedRulesAtIndex({
        index: appliedRuleIndex,
        appliedRules: appliedRules,
      }),
    );

    dispatch(setActive());
    dispatch(setInlineErrorMessage(undefined));

    const precompiledRes = await extra.ideMessenger.request("llm/compileChat", {
      messages,
      options: completionOptions,
    });

    if (precompiledRes.status === "error") {
      if (precompiledRes.error.includes("Not enough context")) {
        dispatch(setInlineErrorMessage("out-of-context"));
        dispatch(setInactive());
        return;
      } else {
        throw new Error(precompiledRes.error);
      }
    }

    const { compiledChatMessages, didPrune, contextPercentage } =
      precompiledRes.content;

    dispatch(setIsPruned(didPrune));
    dispatch(setContextPercentage(contextPercentage));

    const start = Date.now();
    const streamAborter = state.session.streamAborter;
    let aborted = false;
    try {
      let gen = extra.ideMessenger.llmStreamChat(
        {
          completionOptions,
          title: selectedChatModel.title,
          messages: compiledChatMessages,
          legacySlashCommandData,
          messageOptions: { precompiled: true },
        },
        streamAborter.signal,
      );
      if (systemToolsFramework && activeTools.length > 0) {
        gen = interceptSystemToolCalls(
          gen,
          streamAborter,
          systemToolsFramework,
        );
      }

      let next = await gen.next();
      while (!next.done) {
        if (!getState().session.isStreaming) {
          aborted = true;
          dispatch(abortStream());
          break;
        }

        dispatch(streamUpdate(next.value));
        next = await gen.next();
      }
      const llmDurationMs = Date.now() - start;

      // Log abort in multi-agent mode so the orchestrator-side log captures
      // user-initiated cancels alongside backend errors.
      if (
        aborted &&
        state.session.mode === "multi-agent" &&
        state.session.id
      ) {
        extra.ideMessenger.post("multiagent/logError", {
          sessionId: state.session.id,
          errorType: "abort",
          message: `LLM stream aborted by user after ${llmDurationMs}ms`,
          durationMs: llmDurationMs,
        });
      }

      // Attach prompt log and end thinking for reasoning models
      if (next.done && next.value) {
        dispatch(addPromptCompletionPair([next.value]));

        try {
          extra.ideMessenger.post("devdata/log", {
            name: "chatInteraction",
            data: {
              prompt: next.value.prompt,
              completion: next.value.completion,
              modelProvider: selectedChatModel.underlyingProviderName,
              modelName: selectedChatModel.title,
              modelTitle: selectedChatModel.title,
              sessionId: state.session.id,
              ...(!!activeTools.length && {
                tools: activeTools.map((tool) => tool.function.name),
              }),
              ...(appliedRules.length > 0 && {
                rules: appliedRules.map((rule) => ({
                  id: getRuleId(rule),
                  slug: rule.slug,
                })),
              }),
            },
          });

          // Multi-agent: emit a per-turn record so the session log shows
          // the orchestrator's main-model calls alongside sub-agent activity.
          // Tool results are NOT included here — streamResponseAfterToolCall
          // already records them as separate tool_result steps; including
          // them again would duplicate.
          if (state.session.mode === "multi-agent" && state.session.id) {
            const lastMsgs = messages.slice(-5);
            const inputParts: string[] = [];
            for (const m of lastMsgs) {
              if (m.role === "user") {
                const text =
                  typeof m.content === "string"
                    ? m.content
                    : Array.isArray(m.content)
                      ? m.content
                          .filter((p: any) => p.type === "text")
                          .map((p: any) => p.text)
                          .join("\n")
                      : "";
                if (text.trim()) {
                  inputParts.push(text.slice(0, 3000));
                }
              }
            }
            extra.ideMessenger.post("multiagent/logMainModelTurn", {
              sessionId: state.session.id,
              prompt: next.value.prompt,
              completion: next.value.completion,
              modelTitle: selectedChatModel.title,
              promptTokens: next.value.promptTokens,
              completionTokens: next.value.completionTokens,
              inputSummary: inputParts.join("\n").slice(0, 5000),
              durationMs: llmDurationMs,
            });
          }
        } catch (e) {
          console.error("Failed to send dev data interaction log", e);
        }
      }
    } catch (e) {
      const toolCallsToCancel = selectCurrentToolCalls(getState());
      posthog.capture("stream_premature_close_error", {
        duration: (Date.now() - start) / 1000,
        model: selectedChatModel.model,
        provider: selectedChatModel.underlyingProviderName,
        context: legacySlashCommandData ? "slash_command" : "regular_chat",
        ...(legacySlashCommandData && {
          command: legacySlashCommandData.command.name,
        }),
      });
      if (
        toolCallsToCancel.length > 0 &&
        e instanceof Error &&
        e.message.toLowerCase().includes("premature close")
      ) {
        for (const tc of toolCallsToCancel) {
          dispatch(
            errorToolCall({
              toolCallId: tc.toolCallId,
              output: [
                {
                  name: "Tool Call Error",
                  description: "Premature Close",
                  content: `"Premature Close" error: this tool call was aborted mid-stream because the arguments took too long to stream or there were network issues. Please re-attempt by breaking the operation into smaller chunks or trying something else`,
                  icon: "problems",
                },
              ],
            }),
          );
        }
      } else {
        // Capture the error to the slice so StreamError.tsx renders the
        // user-facing dialog. streamThunkWrapper also analyzes + dispatches
        // its own dialog on rethrow; setStreamError is the persisted record
        // (survives a rehydrate via session.streamError).
        const errMsg =
          (e instanceof Error && e.message) ||
          (typeof e === "string" ? e : "Stream interrupted");
        console.error("[streamNormalInput] Stream error after retries:", errMsg);
        dispatch(setStreamError(errMsg));
        throw e;
      }
    }

    // Tool call sequence:
    // 1. Mark generating tool calls as generated
    const state1 = getState();
    if (streamAborter.signal.aborted || !state1.session.isStreaming) {
      return;
    }
    const originalToolCalls = selectCurrentToolCalls(state1);
    const generatingCalls = originalToolCalls.filter(
      (tc) => tc.status === "generating",
    );
    for (const { toolCallId } of generatingCalls) {
      dispatch(
        setToolGenerated({
          toolCallId,
          tools: state1.config.config.tools,
        }),
      );
    }

    // 2. Pre-process args to catch invalid args before checking policies
    const state2 = getState();
    if (streamAborter.signal.aborted || !state2.session.isStreaming) {
      return;
    }
    const generatedCalls2 = selectPendingToolCalls(state2);
    await preprocessToolCalls(dispatch, extra.ideMessenger, generatedCalls2);

    // 3. Security check: evaluate updated policies based on args
    const state3 = getState();
    if (streamAborter.signal.aborted || !state3.session.isStreaming) {
      return;
    }
    const generatedCalls3 = selectPendingToolCalls(state3);
    const toolPolicies = state3.ui.toolSettings;
    const policies = await evaluateToolPolicies(
      dispatch,
      extra.ideMessenger,
      activeTools,
      generatedCalls3,
      toolPolicies,
    );
    const autoApprovedPolicies = policies.filter(
      ({ policy }) => policy === "allowedWithoutPermission",
    );
    const needsApprovalPolicies = policies.filter(
      ({ policy }) => policy === "allowedWithPermission",
    );

    // 4. Execute remaining tool calls
    if (originalToolCalls.length === 0) {
      dispatch(setInactive());
    } else if (needsApprovalPolicies.length > 0) {
      const builtInReadonlyAutoApproved = autoApprovedPolicies.filter(
        ({ toolCallState }) =>
          toolCallState.tool?.group === BUILT_IN_GROUP_NAME &&
          toolCallState.tool?.readonly,
      );

      if (builtInReadonlyAutoApproved.length > 0) {
        const state4 = getState();
        if (streamAborter.signal.aborted || !state4.session.isStreaming) {
          return;
        }
        await Promise.all(
          builtInReadonlyAutoApproved.map(async ({ toolCallState }) => {
            unwrapResult(
              await dispatch(
                callToolById({
                  toolCallId: toolCallState.toolCallId,
                  isAutoApproved: true,
                  depth: depth + 1,
                }),
              ),
            );
          }),
        );
      }

      dispatch(setInactive());
    } else {
      // auto stream cases increase thunk depth by 1 for debugging
      const state4 = getState();
      const generatedCalls4 = selectPendingToolCalls(state4);
      if (streamAborter.signal.aborted || !state4.session.isStreaming) {
        return;
      }
      if (generatedCalls4.length > 0) {
        await Promise.all(
          generatedCalls4.map(async ({ toolCallId }) => {
            unwrapResult(
              await dispatch(
                callToolById({
                  toolCallId,
                  isAutoApproved: true,
                  depth: depth + 1,
                }),
              ),
            );
          }),
        );
      } else {
        for (const { toolCallId } of originalToolCalls) {
          unwrapResult(
            await dispatch(
              streamResponseAfterToolCall({
                toolCallId,
                depth: depth + 1,
              }),
            ),
          );
        }
      }
    }
  },
);
