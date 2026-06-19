import {
  ChevronDownIcon,
  LockClosedIcon,
} from "@heroicons/react/24/outline";
import { isMultiAgentEligible } from "core/config/multiAgentTier";
import { useEffect } from "react";
import { useAuth } from "../../context/Auth";
import { useAppDispatch, useAppSelector } from "../../redux/hooks";
import { selectSelectedChatModel } from "../../redux/slices/configSlice";
import { updateSelectedModelByRole } from "../../redux/thunks/updateSelectedModelByRole";
import {
  formatBlockedCountdown,
  getUsageColor,
  getUsagePercent,
} from "../../util/consumptionLimit";
import {
  Listbox,
  ListboxButton,
  ListboxOption,
  ListboxOptions,
} from "../ui/Listbox";

/**
 * Picker shown in the input toolbar while in multi-agent mode.
 * - Before lock: dropdown filtered to Tier S models. Selecting a model
 *   sets it as the active chat model AND applies the session lock.
 * - After lock: read-only chip. To change model, the user starts a new chat.
 *
 * This component is rendered INSTEAD of the regular ModelSelect when in
 * multi-agent mode, keeping the chat-mode picker code untouched.
 */
export function MultiAgentModelSelect() {
  const dispatch = useAppDispatch();
  const sessionModelLock = useAppSelector((s) => s.session.sessionModelLock);
  const selectedModel = useAppSelector(selectSelectedChatModel);
  const chatModels = useAppSelector(
    (s) => s.config.config.modelsByRole.chat,
  );
  const { selectedProfile } = useAuth();

  // When a session lock exists, force the chat-role model to match the
  // lock. The chat-role default can drift if Sang's auto-switch moved you
  // off the locked model in chat mode earlier; reentering the multi-agent
  // session would otherwise stream against the wrong model. Idempotent:
  // only dispatches if the active model differs from the lock and the
  // locked model is present in the user's config.
  useEffect(() => {
    if (!sessionModelLock || !selectedProfile) return;
    if (selectedModel?.title === sessionModelLock.modelTitle) return;
    const lockedExists = (chatModels ?? []).some(
      (m) => m.title === sessionModelLock.modelTitle,
    );
    if (!lockedExists) return;
    void dispatch(
      updateSelectedModelByRole({
        selectedProfile,
        role: "chat",
        modelTitle: sessionModelLock.modelTitle,
      }),
    );
  }, [
    sessionModelLock,
    selectedModel?.title,
    chatModels,
    selectedProfile,
    dispatch,
  ]);

  // No lock yet: if the current chat-role default isn't a usable Tier S
  // model (either blocked, or not Tier S at all), auto-pick the first
  // unblocked Tier S model instead. Spares the user from submitting on a
  // blocked model only to be refused by the guard. If every Tier S model
  // is blocked we leave selection alone — user sees a disabled dropdown.
  useEffect(() => {
    if (sessionModelLock || !selectedProfile) return;
    const currentEligible = selectedModel
      ? isMultiAgentEligible(selectedModel)
      : false;
    const currentBlocked =
      selectedModel?.consumptionLimit?.blocked === true;
    if (currentEligible && !currentBlocked) return;
    const fallback = (chatModels ?? []).find(
      (m) =>
        isMultiAgentEligible(m) && m.consumptionLimit?.blocked !== true,
    );
    if (!fallback) return;
    if (fallback.title === selectedModel?.title) return;
    void dispatch(
      updateSelectedModelByRole({
        selectedProfile,
        role: "chat",
        modelTitle: fallback.title,
      }),
    );
  }, [
    sessionModelLock,
    selectedModel,
    chatModels,
    selectedProfile,
    dispatch,
  ]);

  // Locked: read-only chip, no dropdown.
  if (sessionModelLock) {
    // Look up the locked model's consumption limit by title from the chat
    // models list — not from selectedChatModel, which can drift if Sang's
    // auto-switch changed the chat-role default elsewhere.
    const lockedModel = (chatModels ?? []).find(
      (m) => m.title === sessionModelLock.modelTitle,
    );
    const limit = lockedModel?.consumptionLimit;
    const showUsage = limit?.enabled === true;
    const percent = showUsage ? getUsagePercent(limit) : 0;
    const isBlocked = showUsage && limit.blocked === true;
    // Always render the locked model's name from the lock itself, not the
    // active chat-role model. Otherwise if the chat-role default changed
    // elsewhere (e.g. Sang's auto-switch moved you off Sonnet in chat
    // mode), the chip would lie about what's actually locked.
    return (
      <div
        className="text-description bg-lightgray/20 flex flex-row items-center gap-1 rounded-full px-1.5 py-0.5 text-xs"
        title="Locked for this session — start a new chat to switch model"
      >
        <LockClosedIcon className="h-3 w-3 flex-shrink-0" />
        <span className="line-clamp-1">{sessionModelLock.modelTitle}</span>
        {showUsage && (
          <span className={getUsageColor(percent, isBlocked)}>
            · {percent}%
          </span>
        )}
        {isBlocked && (
          <span className="text-red-500 italic">
            ({formatBlockedCountdown(limit.blockedUntil)})
          </span>
        )}
      </div>
    );
  }

  // Not yet locked: dropdown filtered strictly to Tier S models.
  // If none match, the dropdown shows an empty state — the user must add
  // a Tier S model to the config (or the backend must mark one as
  // multiAgentEligible) before they can use multi-agent mode.
  const eligible = (chatModels ?? []).filter(isMultiAgentEligible);

  // Picking just updates the active chat model — the lock is applied later,
  // when the user hits send (see streamNormalInput.ts). This preserves the
  // pre-commit freedom to swap between Tier S models before sending.
  const onPick = (modelTitle: string) => {
    if (!modelTitle) return;
    if (selectedProfile) {
      void dispatch(
        updateSelectedModelByRole({
          selectedProfile,
          role: "chat",
          modelTitle,
        }),
      );
    }
  };

  const buttonLabel = selectedModel?.title ?? "Pick a model";

  return (
    <Listbox value={selectedModel?.title ?? ""} onChange={onPick}>
      <div className="relative">
        <ListboxButton
          data-testid="multi-agent-model-select-button"
          className="text-description bg-lightgray/20 flex flex-row items-center gap-1 rounded-full border-none px-1.5 py-0.5 text-xs transition-colors duration-200 hover:brightness-110"
        >
          <span className="line-clamp-1">{buttonLabel}</span>
          <ChevronDownIcon
            className="h-2 w-2 flex-shrink-0"
            aria-hidden="true"
          />
        </ListboxButton>
        <ListboxOptions className="min-w-32 max-w-64">
          {eligible.length === 0 && (
            <div className="text-description-muted px-2 py-1 text-xs italic">
              No multi-agent eligible models. Tier S only.
            </div>
          )}
          {eligible.map((model) => {
            const mLimit = model.consumptionLimit;
            const mShowUsage = mLimit?.enabled === true;
            const mPercent = mShowUsage ? getUsagePercent(mLimit) : 0;
            const mBlocked = mShowUsage && mLimit.blocked === true;
            return (
              <ListboxOption
                key={model.title}
                value={model.title}
                disabled={mBlocked}
                className={mBlocked ? "opacity-50 cursor-not-allowed" : ""}
              >
                <div className="flex flex-row items-center gap-1.5">
                  <span className="line-clamp-1 flex-1">{model.title}</span>
                  {mShowUsage && (
                    <span className={getUsageColor(mPercent, mBlocked)}>
                      {mPercent}%
                    </span>
                  )}
                  {mBlocked && (
                    <span className="text-red-500 text-[10px] italic">
                      {formatBlockedCountdown(mLimit.blockedUntil)}
                    </span>
                  )}
                </div>
              </ListboxOption>
            );
          })}
          <div className="text-description-muted px-2 py-1 text-[10px] italic">
            Locks after the first response
          </div>
        </ListboxOptions>
      </div>
    </Listbox>
  );
}
