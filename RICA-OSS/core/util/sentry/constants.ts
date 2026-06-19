/**
 * Sentry configuration constants
 *
 * RICA build ships an empty DSN (telemetry severed).
 * Sentry.init() with an empty string is a no-op (the SDK silently disables
 * itself), so even if a stray callsite calls Sentry.init({ dsn: SENTRY_DSN })
 * directly, no events ever leave the process. The TelemetryProviders.tsx
 * passthrough already prevents Sentry.init from running in the
 * GUI; this is defense-in-depth for any non-GUI caller.
 *
 * Original Continue Sentry DSN preserved here as a paper trail of what was
 * severed:
 *   "https://fe99934dcdc537d84209893a3f96a196@o4505462064283648.ingest.us.sentry.io/4508184596054016"
 */
export const SENTRY_DSN = "";
