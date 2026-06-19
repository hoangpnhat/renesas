import { describe, expect, it } from "@jest/globals";
import { isMultiAgentEligible, DEFAULT_MULTI_AGENT_TIER_S } from "./multiAgentTier";

describe("isMultiAgentEligible", () => {
  it("returns true when model has multiAgentEligible: true (backend override)", () => {
    expect(isMultiAgentEligible({ model_id: "anything", multiAgentEligible: true })).toBe(true);
  });

  it("returns false when model has multiAgentEligible: false (backend override)", () => {
    // Even if model_id is in the default list, an explicit false wins.
    expect(
      isMultiAgentEligible({
        model_id: DEFAULT_MULTI_AGENT_TIER_S[0],
        multiAgentEligible: false,
      }),
    ).toBe(false);
  });

  it("falls back to the default list when multiAgentEligible is undefined", () => {
    for (const id of DEFAULT_MULTI_AGENT_TIER_S) {
      expect(isMultiAgentEligible({ model_id: id })).toBe(true);
    }
  });

  it("returns false for an unknown model_id with no override", () => {
    expect(isMultiAgentEligible({ model_id: "databricks-claude-haiku-4-5" })).toBe(false);
  });

  it("matches the backend-served Sonnet 4.5 (200K ctx) by upstream model path", () => {
    expect(
      isMultiAgentEligible({
        model: "rica/rica/rica-proxy/databricks-claude-sonnet-4-5",
      }),
    ).toBe(true);
  });

  it("does NOT match the broken 4096-ctx 'sonnet-4-chat' variant", () => {
    expect(
      isMultiAgentEligible({
        model: "rica/rica/rica-proxy/ai-uat-coding-claude-sonnet-4-chat",
      }),
    ).toBe(false);
  });

  it("returns false when model_id is missing and no override is set", () => {
    expect(isMultiAgentEligible({})).toBe(false);
  });
});
