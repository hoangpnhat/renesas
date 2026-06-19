import { ChartBarIcon, XMarkIcon } from "@heroicons/react/24/outline";
import { isMultiAgentEligible } from "core/config/multiAgentTier";
import { useMemo, useState } from "react";
import { useAppSelector } from "../../redux/hooks";
import { selectSelectedChatModel } from "../../redux/slices/configSlice";
import {
  formatBlockedCountdown,
  getUsageColor,
  getUsagePercent,
} from "../../util/consumptionLimit";

type ChatModelLike = {
  title?: string;
  model?: string;
  model_id?: string;
  multiAgentEligible?: boolean;
  consumptionLimit?: {
    threshold: number;
    consumed: number;
    remaining?: number;
    blocked?: boolean;
    blockedUntil?: number | null;
    rangeHours?: number;
  } | null;
};

function formatTokenCount(n: number): string {
  if (n >= 1_000_000) return `${(n / 1_000_000).toFixed(1)}M`;
  if (n >= 1_000) return `${(n / 1_000).toFixed(1)}k`;
  return `${n}`;
}

function pickBarColor(pct: number, blocked: boolean): string {
  if (blocked || pct >= 100) return "bg-red-500/80";
  if (pct >= 80) return "bg-amber-500/80";
  if (pct >= 50) return "bg-yellow-500/80";
  return "bg-emerald-500/70";
}

function SessionBudgetDetailOverlay({
  activeModel,
  otherModels,
  onClose,
}: {
  activeModel: ChatModelLike | undefined;
  otherModels: ChatModelLike[];
  onClose: () => void;
}) {
  const limit = activeModel?.consumptionLimit;
  const pct = limit
    ? getUsagePercent({ consumed: limit.consumed, threshold: limit.threshold })
    : 0;
  const blocked = !!limit?.blocked;
  const barBg = !limit ? "bg-zinc-600" : pickBarColor(pct, blocked);

  return (
    <div
      className="fixed inset-0 z-[100] flex items-start justify-center overflow-y-auto bg-black/60 backdrop-blur-sm"
      onClick={(e) => {
        if (e.target === e.currentTarget) onClose();
      }}
    >
      <div className="mx-2 my-8 w-full max-w-md overflow-hidden rounded-xl border border-solid border-zinc-700/60 bg-zinc-900 shadow-2xl">
        {/* Header */}
        <div className="flex items-center gap-2 border-b border-zinc-700/50 px-4 py-3">
          <ChartBarIcon className="h-5 w-5 text-zinc-300" />
          <div className="min-w-0 flex-1">
            <div className="text-sm font-semibold text-zinc-100">
              Session Usage
            </div>
          </div>
          <button
            onClick={onClose}
            className="ml-1 rounded-md bg-zinc-800/60 p-1.5 text-zinc-500 transition-all hover:bg-zinc-700/70 hover:text-zinc-300"
          >
            <XMarkIcon className="h-4 w-4" />
          </button>
        </div>

        {/* Body */}
        <div className="space-y-3 px-4 py-3">
          {/* Current model — large */}
          <div className="rounded-lg bg-zinc-800/50 px-3 py-2.5">
            <div className="mb-1 flex items-center justify-between">
              <span className="text-[10px] font-medium uppercase tracking-wider text-zinc-500">
                Current model
              </span>
              {limit?.rangeHours ? (
                <span className="text-[9px] text-zinc-600">
                  per {limit.rangeHours}h window
                </span>
              ) : null}
            </div>
            <div className="mb-2 text-sm font-semibold text-zinc-100">
              {activeModel?.title ?? "(no model selected)"}
            </div>
            {limit ? (
              <>
                <div className="mb-1.5 flex items-baseline justify-between">
                  <span className="text-[11px] text-zinc-400">
                    <span className="font-mono tabular-nums text-zinc-200">
                      {formatTokenCount(limit.consumed)}
                    </span>
                    {" / "}
                    <span className="font-mono tabular-nums">
                      {formatTokenCount(limit.threshold)}
                    </span>
                  </span>
                  <span
                    className={`text-xs font-semibold ${getUsageColor(pct, blocked)}`}
                  >
                    {pct}%
                  </span>
                </div>
                <div className="h-1.5 w-full overflow-hidden rounded-full bg-zinc-700/50">
                  <div
                    className={`h-full rounded-full transition-all duration-500 ${barBg}`}
                    style={{ width: `${Math.min(pct, 100)}%` }}
                  />
                </div>
                {blocked ? (
                  <div className="mt-1.5 text-[10px] font-medium text-red-400">
                    Blocked — {formatBlockedCountdown(limit.blockedUntil ?? null)}
                  </div>
                ) : pct >= 80 ? (
                  <div className="mt-1.5 text-[10px] text-amber-400">
                    {pct >= 95
                      ? "Almost out — consider switching models"
                      : "Approaching limit"}
                  </div>
                ) : null}
              </>
            ) : (
              <div className="text-[11px] text-zinc-500">
                No usage limit configured
              </div>
            )}
          </div>

          {/* Other available models */}
          {otherModels.length > 0 && (
            <div>
              <div className="mb-1.5 text-[10px] font-medium uppercase tracking-wider text-zinc-500">
                Other available
              </div>
              <div className="space-y-1">
                {otherModels.map((m) => {
                  const ml = m.consumptionLimit;
                  const mPct = ml
                    ? getUsagePercent({
                        consumed: ml.consumed,
                        threshold: ml.threshold,
                      })
                    : 0;
                  const mBlocked = !!ml?.blocked;
                  const mBg = !ml
                    ? "bg-zinc-600"
                    : pickBarColor(mPct, mBlocked);
                  return (
                    <div
                      key={m.title ?? m.model_id ?? m.model}
                      className="flex items-center gap-2 rounded-lg bg-zinc-800/30 px-3 py-2"
                    >
                      <span className="min-w-0 flex-1 truncate text-[11px] font-medium text-zinc-200">
                        {m.title ?? m.model_id ?? "Unnamed"}
                      </span>
                      {ml ? (
                        <>
                          <div className="h-1 w-20 flex-shrink-0 overflow-hidden rounded-full bg-zinc-700/50">
                            <div
                              className={`h-full rounded-full ${mBg}`}
                              style={{ width: `${Math.min(mPct, 100)}%` }}
                            />
                          </div>
                          <span
                            className={`w-9 flex-shrink-0 text-right text-[10px] font-mono tabular-nums ${getUsageColor(mPct, mBlocked)}`}
                          >
                            {mPct}%
                          </span>
                          {mBlocked ? (
                            <span className="flex-shrink-0 rounded bg-red-500/15 px-1.5 py-0.5 text-[9px] font-medium text-red-400">
                              blocked
                            </span>
                          ) : null}
                        </>
                      ) : (
                        <span className="flex-shrink-0 rounded bg-zinc-700/30 px-1.5 py-0.5 text-[9px] font-medium text-zinc-500">
                          no limit
                        </span>
                      )}
                    </div>
                  );
                })}
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}

/**
 * Small session-usage bar for the active chat model. Shows in chat, agent
 * and multi-agent modes. Hidden if the active model has no
 * consumptionLimit. Click opens a detail overlay with current model + list
 * of other available models.
 */
export function SessionUsageBar() {
  const [showDetail, setShowDetail] = useState(false);
  const sessionModelLock = useAppSelector((s) => s.session.sessionModelLock);
  const mode = useAppSelector((s) => s.session.mode);
  const chatModels = useAppSelector(
    (s) => s.config.config.modelsByRole?.chat,
  );
  const defaultChatModel = useAppSelector(selectSelectedChatModel);

  const activeModel = useMemo(() => {
    if (sessionModelLock && chatModels) {
      const found = chatModels.find(
        (m) => m.title === sessionModelLock.modelTitle,
      );
      if (found) return found as ChatModelLike;
    }
    return (defaultChatModel ?? undefined) as ChatModelLike | undefined;
  }, [sessionModelLock, chatModels, defaultChatModel]);

  const otherModels = useMemo(() => {
    if (!chatModels) return [] as ChatModelLike[];
    const filterFn =
      mode === "multi-agent"
        ? (m: any) => isMultiAgentEligible(m)
        : (_m: any) => true;
    return chatModels.filter(
      (m) => m.title !== activeModel?.title && filterFn(m),
    ) as ChatModelLike[];
  }, [chatModels, activeModel, mode]);

  // Always render the bar so users can see the system is wired up. Three
  // states:
  //   1. no active model (config still loading / no chat models)
  //   2. active model but no consumption limit configured backend-side
  //   3. active model with limit — real consumed/threshold/blocked
  const limit = activeModel?.consumptionLimit;
  const hasLimit =
    !!limit && typeof limit.threshold === "number" && limit.threshold > 0;

  const consumed = hasLimit ? limit!.consumed ?? 0 : 0;
  const threshold = hasLimit ? limit!.threshold : 0;
  const blocked = hasLimit ? !!limit!.blocked : false;
  const pct = hasLimit ? getUsagePercent({ consumed, threshold }) : 0;
  const barBg = hasLimit ? pickBarColor(pct, blocked) : "bg-white/15";
  const textCls = hasLimit
    ? getUsageColor(pct, blocked) || "text-white/70"
    : "text-white/40";

  const rightLabel = !activeModel
    ? "no model"
    : !hasLimit
      ? "no limit"
      : blocked
        ? "blocked"
        : `${pct}%`;

  const tooltip = !activeModel
    ? "No active model — pick a model from the picker"
    : hasLimit
      ? `${activeModel.title ?? "Model"}: ${formatTokenCount(consumed)} / ${formatTokenCount(threshold)} (${pct}%) — click for details`
      : `${activeModel.title ?? "Model"}: no usage limit configured — click for details`;

  return (
    <>
      <div
        onClick={() => setShowDetail(true)}
        className="flex items-center justify-end gap-1.5 px-3 py-0.5 text-[10px] cursor-pointer select-none transition-opacity hover:opacity-80"
        title={tooltip}
      >
        <div className="h-1 w-14 overflow-hidden rounded-full bg-zinc-700/40">
          <div
            className={`h-full rounded-full transition-all duration-500 ${barBg}`}
            style={{ width: `${hasLimit ? Math.min(pct, 100) : 100}%` }}
          />
        </div>
        <span className={`font-mono tabular-nums ${textCls}`}>
          {rightLabel}
        </span>
      </div>

      {showDetail && (
        <SessionBudgetDetailOverlay
          activeModel={activeModel}
          otherModels={otherModels}
          onClose={() => setShowDetail(false)}
        />
      )}
    </>
  );
}
