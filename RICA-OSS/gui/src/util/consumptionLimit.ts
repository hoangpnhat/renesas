/**
 * Consumption limit utility functions for displaying token usage state in the GUI
 */

/**
 * Calculate usage percentage from consumption limit
 * @param limit ConsumptionLimit object with consumed and threshold
 * @returns Percentage between 0-999 (capped to prevent overflow display)
 */
export function getUsagePercent(limit: {
  consumed: number;
  threshold: number;
}): number {
  // Avoid division by zero
  if (limit.threshold === 0) {
    return 0;
  }

  // Calculate percentage and cap at 999 to prevent overflow
  return Math.min(
    Math.round((limit.consumed / limit.threshold) * 100),
    999,
  );
}

/**
 * Get Tailwind color class based on usage percentage and blocked status
 * @param percent Usage percentage (0-999)
 * @param blocked Whether the model is currently blocked
 * @returns Tailwind CSS text-color class string (empty string for default color)
 */
export function getUsageColor(percent: number, blocked: boolean): string {
  // Blocked or at/over threshold: red
  if (blocked || percent >= 100) {
    return "text-red-500";
  }

  // 80% or more: yellow
  if (percent >= 80) {
    return "text-yellow-500";
  }

  // Below 80%: default color (no class needed)
  return "";
}

/**
 * Format blocked countdown message
 * @param blockedUntil Unix timestamp in seconds (or null if no expiry)
 * @returns Formatted countdown string (e.g., "blocked for 2h 10m" or "blocked")
 */
export function formatBlockedCountdown(blockedUntil: number | null): string {
  // No expiry time known
  if (blockedUntil === null) {
    return "blocked";
  }

  // Convert Unix seconds to milliseconds for Date
  const blockedUntilMs = blockedUntil * 1000;
  const now = Date.now();

  // Already past the expiry time
  if (blockedUntilMs <= now) {
    return "blocked";
  }

  // Calculate remaining time
  const diffMs = blockedUntilMs - now;
  const hours = Math.floor(diffMs / 3_600_000);
  const minutes = Math.floor((diffMs % 3_600_000) / 60_000);

  // Format output
  if (hours > 0) {
    return `blocked for ${hours}h ${minutes}m`;
  }

  return `blocked for ${minutes}m`;
}
