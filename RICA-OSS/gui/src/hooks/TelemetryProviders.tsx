// Phase 5.1.11 (Group 3 — telemetry severed): RICA build never enables
// Continue's PostHog or Sentry SDKs in the GUI. The original implementation
// initialized both inside a useEffect and wrapped children in
// Sentry.ErrorBoundary + PostHogProvider — every prop change re-emitted to
// app.posthog.com / sentry.io. Replaced with a passthrough component so the
// existing import sites in App.tsx etc. continue to work, but no telemetry
// is initialized and no provider wraps the tree.
//
// Keeping the file (not deleting) because deleting would force every import
// site to be touched. The shell-only export is the minimum-blast-radius fix.
import { PropsWithChildren } from "react";

const TelemetryProviders = ({ children }: PropsWithChildren) => {
  return <>{children}</>;
};

export default TelemetryProviders;
