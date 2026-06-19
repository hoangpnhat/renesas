// Phase 5.1.23 (Group 5 — WorkOS auth severed): the original test suite
// (~700 LOC) drove the WorkOS PKCE login flow, JWT refresh retry/backoff,
// session lifecycle, and getControlPlaneSessionInfo. Mirrored what
// WorkOsAuthProvider did against api.workos.com and ${CONTROL_PLANE_URL}.
// All of that runtime is now severed — the class constructor throws
// "Rica: not configured" and no caller in the tree imports the file
// (verified with `grep -rn "from .*WorkOsAuthProvider"` returning zero
// hits Jun 10). The runtime path is RicaAuthProvider →
// EntraIDAuthProvider in rica/vscode/integration/auth.ts.
//
// Replaced with a smoke test: instantiation throws + the standalone
// getControlPlaneSessionInfo throws. Mirrors the Group 3 telemetry pattern
// (TelemetryProviders.test.tsx → render-only smoke). When Phase 5.5/6
// decides whether to delete the stub entirely, this file goes with it.

import { describe, expect, it, vi } from "vitest";

vi.mock("vscode", () => ({
  authentication: {
    registerAuthenticationProvider: vi.fn(),
  },
  window: {
    registerUriHandler: vi.fn(),
  },
  EventEmitter: vi.fn(() => ({
    event: { dispose: vi.fn() },
    fire: vi.fn(),
    dispose: vi.fn(),
  })),
  Disposable: {
    from: vi.fn(() => ({ dispose: vi.fn() })),
  },
  env: { uriScheme: "vscode" },
}));

describe("WorkOsAuthProvider (severed for Rica build)", () => {
  it("constructor throws Rica configuration error", async () => {
    const { WorkOsAuthProvider } = await import("./WorkOsAuthProvider");
    expect(
      () => new WorkOsAuthProvider({} as any, {} as any),
    ).toThrow(/Rica: WorkOsAuthProvider is not configured/);
  });

  it("getControlPlaneSessionInfo throws Rica configuration error", async () => {
    const { getControlPlaneSessionInfo } = await import("./WorkOsAuthProvider");
    await expect(getControlPlaneSessionInfo(true, false)).rejects.toThrow(
      /Rica: WorkOsAuthProvider is not configured/,
    );
  });
});
