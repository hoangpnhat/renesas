import { ExclamationTriangleIcon } from "@heroicons/react/24/solid";
import { useEffect } from "react";
import { useAppDispatch, useAppSelector } from "../../redux/hooks";
import { clearUsageWarning } from "../../redux/slices/uiSlice";

const AUTO_DISMISS_MS = 3000;

/**
 * Self-dismissing toast that appears at the top of the webview when the
 * active model's effective consumption crosses 90% of its threshold.
 *
 * Re-fires on every cycle (drop below 90% → cross back up). Auto-dismiss
 * timer resets when the toast `id` changes, so a new firing replaces an
 * existing toast cleanly.
 */
export default function UsageWarningToast() {
  const dispatch = useAppDispatch();
  const warning = useAppSelector((state) => state.ui.usageWarning);

  useEffect(() => {
    if (!warning) return;
    const t = setTimeout(() => {
      dispatch(clearUsageWarning());
    }, AUTO_DISMISS_MS);
    return () => clearTimeout(t);
  }, [warning?.id, dispatch]);

  if (!warning) return null;

  return (
    <div
      className="pointer-events-none fixed left-1/2 top-3 z-[200] -translate-x-1/2"
      role="status"
      aria-live="polite"
    >
      <div className="pointer-events-auto flex max-w-md items-start gap-2 rounded-lg border border-amber-500/50 bg-amber-500/95 px-4 py-3 text-sm text-zinc-900 shadow-2xl">
        <ExclamationTriangleIcon className="mt-0.5 h-5 w-5 flex-shrink-0" />
        <div className="font-medium leading-tight">{warning.message}</div>
      </div>
    </div>
  );
}
