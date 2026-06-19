import {
  CheckIcon,
  ChevronDownIcon,
  ExclamationTriangleIcon,
  InformationCircleIcon,
} from "@heroicons/react/24/outline";
import { MessageModes } from "core";
import { isMultiAgentEligible } from "core/config/multiAgentTier";
import { isRecommendedAgentModel } from "core/llm/toolSupport";
import { useCallback, useEffect, useMemo } from "react";
import { useAuth } from "../../context/Auth";
import { useAppDispatch, useAppSelector } from "../../redux/hooks";
import { selectSelectedChatModel } from "../../redux/slices/configSlice";
import { setMode } from "../../redux/slices/sessionSlice";
import { updateSelectedModelByRole } from "../../redux/thunks/updateSelectedModelByRole";
import { findUnblockedTierSModel } from "../../util/autoSwitchBlockedModel";
import { getFontSize, getMetaKeyLabel } from "../../util";
import { ToolTip } from "../gui/Tooltip";
import { useMainEditor } from "../mainInput/TipTapEditor";
import { Listbox, ListboxButton, ListboxOption, ListboxOptions } from "../ui";
import { ModeIcon } from "./ModeIcon";

export function ModeSelect() {
  const dispatch = useAppDispatch();
  const mode = useAppSelector((store) => store.session.mode);
  const sessionModelLock = useAppSelector(
    (store) => store.session.sessionModelLock,
  );
  const selectedModel = useAppSelector(selectSelectedChatModel);
  const { selectedProfile } = useAuth();
  const chatModels = useAppSelector(
    (store) => store.config.config?.modelsByRole?.chat ?? [],
  );

  const isGoodAtAgentMode = useMemo(() => {
    if (!selectedModel) {
      return undefined;
    }
    return isRecommendedAgentModel(selectedModel.model);
  }, [selectedModel]);

  // Resolve the auto-switch target for entering multi-agent mode:
  //   - If the current model is already an unblocked Tier S, no switch needed.
  //   - Otherwise look for any unblocked Tier S model in the available list.
  //   - If neither is available, multi-agent option is disabled (Layer 1).
  const tierSAutoSwitchTarget = useMemo(() => {
    if (!selectedModel) return null;
    const currentIsTierSAndUnblocked =
      isMultiAgentEligible(selectedModel as any) &&
      selectedModel.consumptionLimit?.blocked !== true;
    if (currentIsTierSAndUnblocked) return null; // no switch needed
    return findUnblockedTierSModel(chatModels as any);
  }, [selectedModel, chatModels]);

  const multiAgentAvailable = useMemo(() => {
    if (!selectedModel) return false;
    const currentIsUsable =
      isMultiAgentEligible(selectedModel as any) &&
      selectedModel.consumptionLimit?.blocked !== true;
    if (currentIsUsable) return true;
    // Need at least one unblocked Tier S to auto-switch into
    return findUnblockedTierSModel(chatModels as any) !== null;
  }, [selectedModel, chatModels]);

  // Once a multi-agent session is committed (sessionModelLock set after the
  // first response), lock the mode selector for the rest of the session.
  // Same trigger as the model lock — both commit together. To use a
  // different mode, the user starts a new chat.
  const modeLocked = mode === "multi-agent" && !!sessionModelLock;

  const { mainEditor } = useMainEditor();
  const metaKeyLabel = useMemo(() => {
    return getMetaKeyLabel();
  }, []);

  // Enter multi-agent mode WITHOUT auto-locking. The lock is applied later
  // at submit time. Before switching mode: if the current chat model isn't
  // an unblocked Tier S, auto-switch to one. This prevents the bug where a
  // user picks a non-Tier-S model in chat mode then enters multi-agent and
  // accidentally locks the wrong model for the session.
  const enterMultiAgent = useCallback(() => {
    if (tierSAutoSwitchTarget && selectedProfile) {
      console.log(
        `[ModeSelect] Auto-switching to Tier S model "${tierSAutoSwitchTarget.title}" before entering multi-agent`,
      );
      dispatch(
        updateSelectedModelByRole({
          role: "chat",
          modelTitle: tierSAutoSwitchTarget.title,
          selectedProfile,
        }),
      );
    }
    dispatch(setMode("multi-agent"));
  }, [dispatch, tierSAutoSwitchTarget, selectedProfile]);

  const cycleMode = useCallback(() => {
    if (modeLocked) return;
    // chat → plan → agent → multi-agent → chat
    // Skip multi-agent when no unblocked Tier S available.
    let nextMode: MessageModes;
    if (mode === "chat") {
      nextMode = "plan";
    } else if (mode === "plan") {
      nextMode = "agent";
    } else if (mode === "agent") {
      nextMode = multiAgentAvailable ? "multi-agent" : "chat";
    } else {
      nextMode = "chat";
    }
    if (nextMode === "multi-agent") {
      enterMultiAgent();
    } else {
      dispatch(setMode(nextMode));
    }
    // Only focus main editor if another one doesn't already have focus
    if (!document.activeElement?.classList?.contains("ProseMirror")) {
      mainEditor?.commands.focus();
    }
  }, [
    mode,
    mainEditor,
    modeLocked,
    multiAgentAvailable,
    enterMultiAgent,
    dispatch,
  ]);

  const selectMode = useCallback(
    (newMode: MessageModes) => {
      if (modeLocked) return;
      if (newMode === mode) {
        return;
      }
      // Reject multi-agent selection when no unblocked Tier S model is available
      if (newMode === "multi-agent" && !multiAgentAvailable) {
        return;
      }

      if (newMode === "multi-agent") {
        enterMultiAgent();
      } else {
        dispatch(setMode(newMode));
      }

      mainEditor?.commands.focus();
    },
    [mode, mainEditor, modeLocked, multiAgentAvailable, enterMultiAgent, dispatch],
  );

  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      if (e.key === "." && (e.metaKey || e.ctrlKey)) {
        e.preventDefault();
        void cycleMode();
      }
    };

    document.addEventListener("keydown", handleKeyDown);
    return () => document.removeEventListener("keydown", handleKeyDown);
  }, [cycleMode]);

  const notGreatAtAgent = (mode: string) => (
    <>
      <ToolTip
        style={{
          zIndex: 200001, // in front of listbox
        }}
        className="flex items-center gap-1"
        content={`${mode} might not work well with this model.`}
      >
        <ExclamationTriangleIcon className="text-warning h-2.5 w-2.5" />
      </ToolTip>
    </>
  );

  return (
    <Listbox value={mode} onChange={selectMode} disabled={modeLocked}>
      <div className="relative">
        <ListboxButton
          data-testid="mode-select-button"
          // @ts-expect-error — Listbox primitive doesn't type 'title' but
          // VS Code's webview renders it as the native HTML attr for tooltip
          title={
            modeLocked
              ? "Mode locked for this session — start a new chat to switch"
              : undefined
          }
          className={`xs:px-2 text-description bg-lightgray/20 gap-1 rounded-full border-none px-1.5 py-0.5 transition-colors duration-200 ${
            modeLocked ? "cursor-not-allowed opacity-80" : "hover:brightness-110"
          }`}
        >
          <ModeIcon mode={mode} />
          <span className="hidden sm:block">
            {mode === "chat"
              ? "Chat"
              : mode === "agent"
                ? "Agent"
                : mode === "multi-agent"
                  ? "Multi-Agent"
                  : "Plan"}
          </span>
          {!modeLocked && (
            <ChevronDownIcon
              className="h-2 w-2 flex-shrink-0"
              aria-hidden="true"
            />
          )}
        </ListboxButton>
        <ListboxOptions className="min-w-32 max-w-48">
          <ListboxOption value="chat" disabled={modeLocked}>
            <div className="flex flex-row items-center gap-1.5">
              <ModeIcon mode="chat" />
              <span className="">Chat</span>
              <ToolTip
                style={{
                  zIndex: 200001,
                }}
                content="All tools disabled"
              >
                <InformationCircleIcon
                  data-tooltip-id="chat-tip"
                  className="h-2.5 w-2.5 flex-shrink-0"
                />
              </ToolTip>
              <span
                className={`text-description-muted text-[${getFontSize() - 3}px] mr-auto`}
              >
                {getMetaKeyLabel()}L
              </span>
            </div>
            {mode === "chat" && <CheckIcon className="ml-auto h-3 w-3" />}
          </ListboxOption>
          <ListboxOption value="plan" className={"gap-1"} disabled={modeLocked}>
            <div className="flex flex-row items-center gap-1.5">
              <ModeIcon mode="plan" />
              <span className="">Plan</span>
              <ToolTip
                style={{
                  zIndex: 200001,
                }}
                content="Read-only/MCP tools available"
              >
                <InformationCircleIcon className="h-2.5 w-2.5 flex-shrink-0" />
              </ToolTip>
            </div>
            {!isGoodAtAgentMode && notGreatAtAgent("Plan")}
            <CheckIcon
              className={`ml-auto h-3 w-3 ${mode === "plan" ? "" : "opacity-0"}`}
            />
          </ListboxOption>

          <ListboxOption
            value="agent"
            className={"gap-1"}
            disabled={modeLocked}
          >
            <div className="flex flex-row items-center gap-1.5">
              <ModeIcon mode="agent" />
              <span className="">Agent</span>
              <ToolTip
                style={{
                  zIndex: 200001,
                }}
                content="All tools available"
              >
                <InformationCircleIcon className="h-2.5 w-2.5 flex-shrink-0" />
              </ToolTip>
            </div>
            {!isGoodAtAgentMode && notGreatAtAgent("Agent")}
            <CheckIcon
              className={`ml-auto h-3 w-3 ${mode === "agent" ? "" : "opacity-0"}`}
            />
          </ListboxOption>

          <ListboxOption
            value="multi-agent"
            disabled={!multiAgentAvailable || modeLocked}
            className={"gap-1"}
            // @ts-expect-error — Listbox primitive doesn't type 'title' but
            // VS Code's wrapper renders it as the native HTML attr for tooltip
            title={
              !multiAgentAvailable
                ? "All multi-agent eligible models are rate-limited. Switch model or wait."
                : undefined
            }
          >
            <div className="flex flex-row items-center gap-1.5">
              <ModeIcon mode="multi-agent" />
              <span className="">Multi-Agent</span>
              <ToolTip
                style={{
                  zIndex: 200001,
                }}
                content="Spawns parallel sub-agents to decompose work"
              >
                <InformationCircleIcon className="h-2.5 w-2.5 flex-shrink-0" />
              </ToolTip>
            </div>
            {!multiAgentAvailable ? (
              <span>(All blocked)</span>
            ) : (
              <CheckIcon
                className={`ml-auto h-3 w-3 ${mode === "multi-agent" ? "" : "opacity-0"}`}
              />
            )}
          </ListboxOption>

          <div className="text-description-muted px-2 py-1">
            {modeLocked
              ? "Locked for this session"
              : `${metaKeyLabel} . for next mode`}
          </div>
        </ListboxOptions>
      </div>
    </Listbox>
  );
}
