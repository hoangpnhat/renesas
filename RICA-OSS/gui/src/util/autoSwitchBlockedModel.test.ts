import { describe, it, expect } from "vitest";
import { findUnblockedAlternative } from "./autoSwitchBlockedModel";

describe("findUnblockedAlternative - auto-switch logic", () => {
  it("should return unblocked model when active model is blocked for chat role", () => {
    const blocked = {
      title: "blocked-model",
      consumptionLimit: { enabled: true, blocked: true, blockedUntil: null },
    } as any;
    const unblocked = {
      title: "unblocked-model",
      consumptionLimit: { enabled: true, blocked: false },
    } as any;

    const result = findUnblockedAlternative(blocked, [blocked, unblocked], "chat");
    expect(result?.title).toBe("unblocked-model");
  });

  it("should return null when active model is not blocked", () => {
    const unblocked = {
      title: "unblocked-model",
      consumptionLimit: { enabled: true, blocked: false },
    } as any;

    const result = findUnblockedAlternative(unblocked, [unblocked], "chat");
    expect(result).toBeNull();
  });

  it("should return null when all alternatives are also blocked", () => {
    const blocked1 = {
      title: "blocked-1",
      consumptionLimit: { enabled: true, blocked: true },
    } as any;
    const blocked2 = {
      title: "blocked-2",
      consumptionLimit: { enabled: true, blocked: true },
    } as any;

    const result = findUnblockedAlternative(
      blocked1,
      [blocked1, blocked2],
      "chat",
    );
    expect(result).toBeNull();
  });

  it("should return null for non-applicable roles like autocomplete", () => {
    const blocked = {
      title: "blocked-model",
      consumptionLimit: { enabled: true, blocked: true },
    } as any;
    const unblocked = {
      title: "unblocked-model",
      consumptionLimit: { enabled: true, blocked: false },
    } as any;

    const result = findUnblockedAlternative(
      blocked,
      [blocked, unblocked],
      "autocomplete",
    );
    expect(result).toBeNull();
  });

  it("should return null when active model is null", () => {
    const unblocked = {
      title: "unblocked-model",
      consumptionLimit: { enabled: true, blocked: false },
    } as any;

    const result = findUnblockedAlternative(null, [unblocked], "chat");
    expect(result).toBeNull();
  });

  it("should return first unblocked model for edit role", () => {
    const blocked = {
      title: "blocked-model",
      consumptionLimit: { enabled: true, blocked: true },
    } as any;
    const unblocked1 = {
      title: "unblocked-1",
      consumptionLimit: { enabled: true, blocked: false },
    } as any;
    const unblocked2 = {
      title: "unblocked-2",
      consumptionLimit: { enabled: true, blocked: false },
    } as any;

    const result = findUnblockedAlternative(
      blocked,
      [blocked, unblocked1, unblocked2],
      "edit",
    );
    expect(result?.title).toBe("unblocked-1");
  });

  it("should handle model without consumptionLimit as unblocked", () => {
    const blocked = {
      title: "blocked-model",
      consumptionLimit: { enabled: true, blocked: true },
    } as any;
    const noConsumption = {
      title: "no-consumption",
      consumptionLimit: undefined,
    } as any;

    const result = findUnblockedAlternative(
      blocked,
      [blocked, noConsumption],
      "chat",
    );
    expect(result?.title).toBe("no-consumption");
  });

  it("should handle model with consumption disabled as unblocked", () => {
    const blocked = {
      title: "blocked-model",
      consumptionLimit: { enabled: true, blocked: true },
    } as any;
    const disabled = {
      title: "disabled-consumption",
      consumptionLimit: { enabled: false },
    } as any;

    const result = findUnblockedAlternative(
      blocked,
      [blocked, disabled],
      "chat",
    );
    expect(result?.title).toBe("disabled-consumption");
  });

  it("should return null when isSessionLocked is true, even if blocked", () => {
    // Multi-agent Tier S lock: never auto-switch silently.
    const blocked = {
      title: "blocked-model",
      consumptionLimit: { enabled: true, blocked: true, blockedUntil: null },
    } as any;
    const unblocked = {
      title: "unblocked-model",
      consumptionLimit: { enabled: true, blocked: false },
    } as any;

    const result = findUnblockedAlternative(
      blocked,
      [blocked, unblocked],
      "chat",
      true,
    );
    expect(result).toBeNull();
  });
});
