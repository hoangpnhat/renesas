// Phase 5.1.11 (Group 3): TelemetryProviders is now a passthrough — no
// PostHog/Sentry init, no provider wrappers. The original test suite
// verified the upstream init flow which no longer exists. Replaced with a
// single rendering smoke test that confirms children pass through.

import { render, screen } from "@testing-library/react";
import { describe, expect, it } from "vitest";
import TelemetryProviders from "./TelemetryProviders";

describe("TelemetryProviders (Phase 5.1.11 passthrough)", () => {
  it("renders children unchanged with no telemetry provider wrapping", () => {
    render(
      <TelemetryProviders>
        <div data-testid="child">child content</div>
      </TelemetryProviders>,
    );
    expect(screen.getByTestId("child")).toBeInTheDocument();
    expect(screen.getByText("child content")).toBeInTheDocument();
  });
});
