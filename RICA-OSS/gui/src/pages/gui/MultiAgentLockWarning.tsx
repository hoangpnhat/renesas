import { useAppSelector } from "../../redux/hooks";
import { selectSelectedChatModel } from "../../redux/slices/configSlice";
import { formatBlockedCountdown } from "../../util/consumptionLimit";

export function MultiAgentLockWarning() {
  const mode = useAppSelector((s) => s.session.mode);
  const sessionModelLock = useAppSelector((s) => s.session.sessionModelLock);
  const isStreaming = useAppSelector((s) => s.session.isStreaming);
  const selectedChatModel = useAppSelector(selectSelectedChatModel);

  if (mode !== "multi-agent" || !sessionModelLock || !isStreaming) {
    return null;
  }
  if (selectedChatModel?.consumptionLimit?.blocked !== true) {
    return null;
  }

  const until = selectedChatModel.consumptionLimit?.blockedUntil ?? null;

  return (
    <div className="mx-2 my-1 rounded border border-yellow-500/40 bg-yellow-500/10 px-2 py-1 text-[11px] text-yellow-300">
      Model rate-limited ({formatBlockedCountdown(until)}). This run will
      complete, then chat will lock until the limit resets.
    </div>
  );
}
