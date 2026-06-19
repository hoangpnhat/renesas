// Default substrings — used as a fallback when the backend response does not
// carry the `multiAgentEligible` flag. Each entry is checked as a case-insensitive
// substring against `model.model_id` and `model.model` (the upstream path,
// e.g. "rica/rica/rica-proxy/databricks-claude-sonnet-4-6"). Backend admins
// can override per-model by setting `model.multiAgentEligible = true/false`
// in the EntraID dynamic config response, which takes precedence.
//
// Add or remove substrings here as model availability changes. Keep them
// specific enough that "haiku" or "mini" variants don't accidentally match
// (e.g. "claude-opus-4" is fine; "claude" alone would match Haiku too).
export const DEFAULT_MULTI_AGENT_TIER_S: string[] = [
  "claude-opus-4",      // matches any opus 4.x (no opus on Renesas backend yet)
  "claude-sonnet-4-6",  // Sonnet 4.6 specifically
  "claude-sonnet-4-5",  // Sonnet 4.5 — backend serves "databricks-claude-sonnet-4-5"
  //                       (200K ctx). Specific enough to NOT match the broken
  //                       "ai-uat-coding-claude-sonnet-4-chat" (4096 ctx).
  // gpt-5-5 INTENTIONALLY EXCLUDED: Renesas backend rejects
  // `function tools with reasoning_effort` for this model on
  // /v1/chat/completions ("use /v1/responses instead"). The rica-proxy
  // doesn't expose /v1/responses, so multi-agent runs error 400.
  // 5.5 still works fine in chat mode without tools. Re-enable here if
  // backend gains /v1/responses routing.
  "gpt-5-4",            // GPT 5.4
  "gemini-3-1-pro",     // Gemini Pro (Renesas naming)
];

export function isMultiAgentEligible(model: {
  model_id?: string;
  model?: string;
  multiAgentEligible?: boolean;
}): boolean {
  if (typeof model.multiAgentEligible === "boolean") {
    return model.multiAgentEligible;
  }
  const id = model.model_id?.toLowerCase() ?? "";
  const fullPath = model.model?.toLowerCase() ?? "";
  if (!id && !fullPath) return false;
  return DEFAULT_MULTI_AGENT_TIER_S.some((needle) => {
    const lower = needle.toLowerCase();
    return id.includes(lower) || fullPath.includes(lower);
  });
}
