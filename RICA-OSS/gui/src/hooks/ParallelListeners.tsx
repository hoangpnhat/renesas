import { useCallback, useContext, useEffect, useRef, useState } from "react";
import { IdeMessengerContext } from "../context/IdeMessenger";

import { FromCoreProtocol } from "core/protocol";
import { useAppDispatch, useAppSelector } from "../redux/hooks";
import { selectSessionTokens } from "../redux/selectors/selectSessionTokens";
import { setConfigLoading, setConfigResult } from "../redux/slices/configSlice";
import { setLastNonEditSessionEmpty } from "../redux/slices/editState";
import { updateIndexingStatus } from "../redux/slices/indexingSlice";
import {
  initializeProfilePreferences,
  selectSelectedProfile,
  setOrganizations,
  setSelectedOrgId,
  setSelectedProfile,
} from "../redux/slices/profilesSlice";
import {
  addContextItemsAtIndex,
  newSession,
  setHasReasoningEnabled,
  setIsSessionMetadataLoading,
  setMode,
} from "../redux/slices/sessionSlice";
import { setTTSActive, showUsageWarning } from "../redux/slices/uiSlice";

import { modelSupportsReasoning } from "core/llm/autodetect";
import { useStore } from "react-redux";
import type { RootState } from "../redux/store";
import { cancelStream } from "../redux/thunks/cancelStream";
import { handleApplyStateUpdate } from "../redux/thunks/handleApplyStateUpdate";
import { loadSession, refreshSessionMetadata } from "../redux/thunks/session";
import { updateFileSymbolsFromHistory } from "../redux/thunks/updateFileSymbols";
import { updateSelectedModelByRole } from "../redux/thunks/updateSelectedModelByRole";
import { findUnblockedAlternative } from "../util/autoSwitchBlockedModel";
import {
  setDocumentStylesFromLocalStorage,
  setDocumentStylesFromTheme,
} from "../styles/theme";
import { isJetBrains } from "../util";
import { setLocalStorage } from "../util/localStorage";
import { migrateLocalStorage } from "../util/migrateLocalStorage";
import { useWebviewListener } from "./useWebviewListener";

function ParallelListeners() {
  const dispatch = useAppDispatch();
  const store = useStore<RootState>();
  const ideMessenger = useContext(IdeMessengerContext);
  const history = useAppSelector((store) => store.session.history);
  const isInEdit = useAppSelector((store) => store.session.isInEdit);
  const sessionModelLock = useAppSelector(
    (store) => store.session.sessionModelLock,
  );
  const sessionMode = useAppSelector((store) => store.session.mode);
  const isStreaming = useAppSelector((store) => store.session.isStreaming);
  const selectedProfileId = useAppSelector(
    (store) => store.profiles.selectedProfileId,
  );
  const selectedProfile = useAppSelector(selectSelectedProfile);
  const reasoningSettings = useAppSelector(
    (store) => store.ui.reasoningSettings,
  );
  const hasDoneInitialConfigLoad = useRef(false);

  // Mirror isStreaming into a ref so handleConfigUpdate (a useCallback)
  // can read the current value without re-creating on every transition.
  const isStreamingRef = useRef(isStreaming);
  useEffect(() => {
    isStreamingRef.current = isStreaming;
  }, [isStreaming]);

  // Per-model record of whether usage is currently above 90% — fire the
  // toast only on a fresh below→above crossing, not on every poll while
  // already above. Resets when usage drops back below 90%.
  const usageWarningAboveRef = useRef<Record<string, boolean>>({});

  // Load symbols for chat on any session change
  const sessionId = useAppSelector((state) => state.session.id);
  const lastSessionId = useAppSelector((store) => store.session.lastSessionId);
  const [initialSessionId] = useState(sessionId || lastSessionId);

  const handleConfigUpdate = useCallback(
    async (isInitial: boolean, result: FromCoreProtocol["configUpdate"][0]) => {
      const {
        result: configResult,
        profileId,
        organizations,
        selectedOrgId,
      } = result;
      if (isInitial && hasDoneInitialConfigLoad.current) {
        return;
      }
      if (configResult.configLoadInterrupted || !configResult.config) {
        return;
      }
      hasDoneInitialConfigLoad.current = true;
      dispatch(setOrganizations(organizations));
      dispatch(setSelectedOrgId(selectedOrgId));
      dispatch(setSelectedProfile(profileId));
      dispatch(setConfigResult(configResult));

      // Auto-switch blocked models to unblocked alternatives.
      // Use the incoming config's selectedModelByRole rather than a stale
      // closure: core runs rectifySelectedModelsFromGlobalContext before
      // sending configUpdate, so the incoming model already carries the
      // latest consumptionLimit (including blocked: true if applicable).
      const newModelsByRole = configResult.config?.modelsByRole;
      const incomingSelectedByRole = configResult.config?.selectedModelByRole;

      if (incomingSelectedByRole && newModelsByRole) {
        // Multi-agent commits the user to one model for the session. A
        // silent auto-switch off that model would contradict the lock and
        // strand the user when they navigate back to chat/agent later (the
        // model would have been swapped under them across modes). Skip the
        // auto-switch entirely in multi-agent and rely on the submit-time
        // guard in streamNormalInput to refuse blocked models. Chat/agent
        // modes get the auto-switch as designed.
        if (sessionMode !== "multi-agent") {
          for (const role of ["chat", "edit"] as const) {
            const activeModel = incomingSelectedByRole[role];
            const roleModels = newModelsByRole[role] || [];

            // Even outside multi-agent, the lock chip is sticky to the
            // referenced model — never silently swap off the locked title.
            const isSessionLocked =
              sessionModelLock !== null &&
              activeModel?.title === sessionModelLock.modelTitle;
            const unblocked = findUnblockedAlternative(
              activeModel,
              roleModels,
              role,
              isSessionLocked,
            );
            if (unblocked && selectedProfile) {
              dispatch(
                updateSelectedModelByRole({
                  role,
                  modelTitle: unblocked.title,
                  selectedProfile,
                }),
              );
            }
          }
        }
      }

      // Mid-stream cancel-on-block. AgentManager handles the sub-agent
      // side of the cancel-on-block hook in core; the orchestrator's main
      // chat stream isn't tracked there, so the GUI does its own check on
      // every config update.
      if (
        isStreamingRef.current &&
        incomingSelectedByRole?.chat?.consumptionLimit?.blocked === true
      ) {
        const blockedTitle = incomingSelectedByRole.chat.title;
        console.warn(
          `[BLOCKED-MIDSTREAM] Active chat model "${blockedTitle}" transitioned to blocked — canceling in-flight stream`,
        );
        dispatch(cancelStream());
      }

      // 90% usage warning toast — cycle-based: fires only on a below→above
      // 90% transition. To re-fire, usage must drop below 90% (window
      // slides forward) and climb back up. Effective consumed uses the
      // same max(backend, local) formula as the injected budget block so
      // user-facing %ages match what the orchestrator sees.
      const usageChatModel = incomingSelectedByRole?.chat;
      const limit = usageChatModel?.consumptionLimit;
      if (
        usageChatModel &&
        limit &&
        typeof limit.threshold === "number" &&
        typeof limit.consumed === "number" &&
        limit.threshold > 0
      ) {
        const sessionTokens = selectSessionTokens(store.getState());
        const localWeighted =
          (sessionTokens?.orchestratorTokens?.promptTokens ?? 0) +
          (sessionTokens?.agentTokens?.promptTokens ?? 0) +
          ((sessionTokens?.orchestratorTokens?.completionTokens ?? 0) +
            (sessionTokens?.agentTokens?.completionTokens ?? 0)) *
            5;
        const effective = Math.max(limit.consumed, localWeighted);
        const pct = effective / limit.threshold;
        const modelKey = usageChatModel.title || "unknown";
        const wasAbove = !!usageWarningAboveRef.current[modelKey];
        // Only "approaching" — don't pop after already blocked (separate UI).
        const isAbove = pct >= 0.9 && pct < 1.0;
        if (isAbove && !wasAbove) {
          const pctDisplay = Math.round(pct * 100);
          console.log(
            `[USAGE-WARN] ${modelKey} crossed 90% (${pctDisplay}% effective) — showing 3s toast`,
          );
          dispatch(
            showUsageWarning({
              message: `Approaching token limit (${pctDisplay}%) — session will lock soon.`,
            }),
          );
        }
        usageWarningAboveRef.current[modelKey] = isAbove;
      }

      const isNewProfileId = profileId && profileId !== selectedProfileId;

      if (isNewProfileId) {
        dispatch(
          initializeProfilePreferences({
            defaultSlashCommands: [],
            profileId,
          }),
        );
      }

      // Perform any actions needed with the config
      if (configResult.config?.ui?.fontSize) {
        setLocalStorage("fontSize", configResult.config.ui.fontSize);
        document.body.style.fontSize = `${configResult.config.ui.fontSize}px`;
      }

      const chatModel = configResult.config?.selectedModelByRole.chat;
      const supportsReasoning = modelSupportsReasoning(chatModel);
      const isReasoningDisabled =
        chatModel?.completionOptions?.reasoning === false;
      const wasReasoningPreviouslyEnabled = chatModel?.title
        ? reasoningSettings[chatModel.title] !== false
        : true;
      dispatch(
        setHasReasoningEnabled(
          supportsReasoning &&
            !isReasoningDisabled &&
            wasReasoningPreviouslyEnabled,
        ),
      );
    },
    [
      dispatch,
      hasDoneInitialConfigLoad,
      selectedProfileId,
      reasoningSettings,
      sessionMode,
      sessionModelLock,
      selectedProfile,
      store,
    ],
  );

  // Load config from the IDE
  useEffect(() => {
    async function initialLoadConfig() {
      dispatch(setIsSessionMetadataLoading(true));
      dispatch(setConfigLoading(true));
      const result = await ideMessenger.request(
        "config/getSerializedProfileInfo",
        undefined,
      );
      if (result.status === "success") {
        await handleConfigUpdate(true, result.content);
      }
      dispatch(setConfigLoading(false));
      if (initialSessionId) {
        await dispatch(
          loadSession({
            sessionId: initialSessionId,
            saveCurrentSession: false,
          }),
        );
      }
    }
    void initialLoadConfig();
    const interval = setInterval(() => {
      if (hasDoneInitialConfigLoad.current) {
        // Init to run on initial config load
        ideMessenger.post("docs/initStatuses", undefined);
        void dispatch(updateFileSymbolsFromHistory());
        void dispatch(refreshSessionMetadata({}));

        // This triggers sending pending status to the GUI for relevant docs indexes
        clearInterval(interval);
      } else {
        void initialLoadConfig();
      }
    }, 2_000);

    return () => clearInterval(interval);
  }, [hasDoneInitialConfigLoad, ideMessenger, initialSessionId]);

  useWebviewListener(
    "configUpdate",
    async (update) => {
      if (!update) {
        return;
      }
      await handleConfigUpdate(false, update);
    },
    [handleConfigUpdate],
  );

  useEffect(() => {
    if (sessionId) {
      void dispatch(updateFileSymbolsFromHistory());
    }
  }, [sessionId]);

  // ON LOAD
  useEffect(() => {
    // Override persisted state
    void dispatch(cancelStream());

    const jetbrains = isJetBrains();
    setDocumentStylesFromLocalStorage(jetbrains);

    if (jetbrains) {
      // Save theme colors to local storage for immediate loading in JetBrains
      void ideMessenger
        .request("jetbrains/getColors", undefined)
        .then((result) => {
          if (result.status === "success") {
            setDocumentStylesFromTheme(result.content);
          }
        });

      // Tell JetBrains the webview is ready
      void ideMessenger
        .request("jetbrains/onLoad", undefined)
        .then((result) => {
          if (result.status === "error") {
            return;
          }

          const msg = result.content;
          (window as any).windowId = msg.windowId;
          (window as any).serverUrl = msg.serverUrl;
          (window as any).workspacePaths = msg.workspacePaths;
          (window as any).vscMachineId = msg.vscMachineId;
          (window as any).vscMediaUrl = msg.vscMediaUrl;
        });
    }
  }, []);

  useWebviewListener(
    "jetbrains/setColors",
    async (data) => {
      setDocumentStylesFromTheme(data);
    },
    [],
  );

  // IDE event listeners
  useWebviewListener(
    "getWebviewHistoryLength",
    async () => {
      return history.length;
    },
    [history],
  );

  useWebviewListener(
    "getCurrentSessionId",
    async () => {
      return sessionId;
    },
    [sessionId],
  );

  useWebviewListener("setInactive", async () => {
    void dispatch(cancelStream());
  });

  useWebviewListener("loadAgentSession", async (data) => {
    dispatch(newSession(data.session));
    dispatch(setMode("agent"));
  });

  useWebviewListener("setTTSActive", async (status) => {
    dispatch(setTTSActive(status));
  });

  useWebviewListener("addContextItem", async (data) => {
    dispatch(
      addContextItemsAtIndex({
        index: data.historyIndex,
        contextItems: [data.item],
      }),
    );
  });

  useWebviewListener("indexing/statusUpdate", async (data) => {
    dispatch(updateIndexingStatus(data));
  });

  useWebviewListener(
    "updateApplyState",
    async (state) => {
      void dispatch(handleApplyStateUpdate(state));
    },
    [],
  );

  useEffect(() => {
    if (!isInEdit) {
      dispatch(setLastNonEditSessionEmpty(history.length === 0));
    }
  }, [isInEdit, history]);

  useEffect(() => {
    migrateLocalStorage(dispatch);
  }, []);

  return <></>;
}

export default ParallelListeners;
