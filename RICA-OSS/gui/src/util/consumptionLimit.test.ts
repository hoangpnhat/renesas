import {
  formatBlockedCountdown,
  getUsageColor,
  getUsagePercent,
} from "./consumptionLimit";

describe("getUsagePercent", () => {
  it("should calculate normal percentage correctly", () => {
    const limit = { consumed: 50, threshold: 100 };
    expect(getUsagePercent(limit)).toBe(50);
  });

  it("should handle zero-division guard when threshold is 0", () => {
    const limit = { consumed: 100, threshold: 0 };
    expect(getUsagePercent(limit)).toBe(0);
  });

  it("should round percentage correctly", () => {
    const limit = { consumed: 79, threshold: 100 };
    expect(getUsagePercent(limit)).toBe(79);
  });

  it("should handle 80% threshold", () => {
    const limit = { consumed: 80, threshold: 100 };
    expect(getUsagePercent(limit)).toBe(80);
  });

  it("should handle 100% usage", () => {
    const limit = { consumed: 100, threshold: 100 };
    expect(getUsagePercent(limit)).toBe(100);
  });

  it("should cap extreme values at 999%", () => {
    const limit = { consumed: 1500, threshold: 100 };
    expect(getUsagePercent(limit)).toBe(999);
  });

  it("should handle very large overflow correctly", () => {
    const limit = { consumed: 10000, threshold: 100 };
    expect(getUsagePercent(limit)).toBe(999);
  });

  it("should round 79.4% down to 79%", () => {
    const limit = { consumed: 794, threshold: 1000 };
    expect(getUsagePercent(limit)).toBe(79);
  });

  it("should round 79.5% up to 80%", () => {
    const limit = { consumed: 795, threshold: 1000 };
    expect(getUsagePercent(limit)).toBe(80);
  });

  it("should handle decimal consumed and threshold", () => {
    const limit = { consumed: 25.5, threshold: 100 };
    expect(getUsagePercent(limit)).toBe(26); // Rounds to nearest integer
  });
});

describe("getUsageColor", () => {
  it("should return red for blocked status regardless of percent", () => {
    expect(getUsageColor(0, true)).toBe("text-red-500");
    expect(getUsageColor(50, true)).toBe("text-red-500");
    expect(getUsageColor(100, true)).toBe("text-red-500");
  });

  it("should return red for 100% consumption", () => {
    expect(getUsageColor(100, false)).toBe("text-red-500");
  });

  it("should return red for percent >= 100", () => {
    expect(getUsageColor(150, false)).toBe("text-red-500");
    expect(getUsageColor(999, false)).toBe("text-red-500");
  });

  it("should return yellow for 80% consumption", () => {
    expect(getUsageColor(80, false)).toBe("text-yellow-500");
  });

  it("should return yellow for percent >= 80 and < 100", () => {
    expect(getUsageColor(85, false)).toBe("text-yellow-500");
    expect(getUsageColor(99, false)).toBe("text-yellow-500");
  });

  it("should return empty string for percent < 80", () => {
    expect(getUsageColor(0, false)).toBe("");
    expect(getUsageColor(50, false)).toBe("");
    expect(getUsageColor(79, false)).toBe("");
  });

  it("should return empty string for 79% exactly", () => {
    expect(getUsageColor(79, false)).toBe("");
  });
});

describe("formatBlockedCountdown", () => {
  it("should return 'blocked' when blockedUntil is null", () => {
    expect(formatBlockedCountdown(null)).toBe("blocked");
  });

  it("should return 'blocked' when blockedUntil is in the past", () => {
    const pastTimestamp = Math.floor(Date.now() / 1000) - 3600; // 1 hour ago
    expect(formatBlockedCountdown(pastTimestamp)).toBe("blocked");
  });

  it("should format countdown for future timestamp with hours", () => {
    const futureTimestamp = Math.floor(Date.now() / 1000) + 7200; // 2 hours from now
    const result = formatBlockedCountdown(futureTimestamp);
    expect(result).toMatch(/blocked for (1|2)h \d+m/);
  });

  it("should format countdown for future timestamp with minutes only", () => {
    const futureTimestamp = Math.floor(Date.now() / 1000) + 600; // 10 minutes from now
    const result = formatBlockedCountdown(futureTimestamp);
    expect(result).toMatch(/blocked for (9|10|11)m/);
  });

  it("should handle 1 hour exactly", () => {
    const futureTimestamp = Math.floor(Date.now() / 1000) + 3600; // Exactly 1 hour from now
    const result = formatBlockedCountdown(futureTimestamp);
    // Account for timing drift - could be 59m or 60m or 1h 0m depending on execution timing
    expect(result).toMatch(/blocked for (59|60)m|blocked for [0-9]h/);
  });

  it("should handle 1 minute exactly", () => {
    const futureTimestamp = Math.floor(Date.now() / 1000) + 60; // Exactly 1 minute from now
    const result = formatBlockedCountdown(futureTimestamp);
    expect(result).toMatch(/blocked for [0-9]+m/);
  });

  it("should handle very large countdown", () => {
    const futureTimestamp = Math.floor(Date.now() / 1000) + 86400; // 24 hours from now
    const result = formatBlockedCountdown(futureTimestamp);
    expect(result).toMatch(/blocked for (23|24)h/);
  });

  it("should round down minutes correctly", () => {
    // 2 hours and 45 minutes = 9900 seconds
    const futureTimestamp = Math.floor(Date.now() / 1000) + 9900;
    const result = formatBlockedCountdown(futureTimestamp);
    // Allow for slight time drift between calculation and execution
    expect(result).toMatch(/blocked for (2|1)h (44|45|46)m/);
  });

  it("should handle edge case: just under 1 hour", () => {
    const futureTimestamp = Math.floor(Date.now() / 1000) + 3599; // 59 min 59 sec
    const result = formatBlockedCountdown(futureTimestamp);
    expect(result).toMatch(/blocked for (59|58)m/);
  });

  it("should handle edge case: just over 1 hour", () => {
    const futureTimestamp = Math.floor(Date.now() / 1000) + 3660; // 1 hour 1 minute
    const result = formatBlockedCountdown(futureTimestamp);
    // Allow for timing variations
    expect(result).toMatch(/blocked for (0|1)h [0-9]+m/);
  });

  it("should handle zero time remaining (now)", () => {
    const nowTimestamp = Math.floor(Date.now() / 1000);
    expect(formatBlockedCountdown(nowTimestamp)).toBe("blocked");
  });
});
