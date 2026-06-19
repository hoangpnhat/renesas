import { getControlPlaneEnv, type IdeSettings } from "./env";

type RicaModelCapabilities = {
  tools?: boolean;
  uploadImage?: boolean;
  nextEdit?: boolean;
  /**
   * Phase 5.x — backend-declared reasoning support. Drives lightbulb
   * visibility in InputToolbar (modelSupportsReasoning reads this first,
   * falls back to upstream regex when undefined). Backend can declare
   * either as a boolean flag in the object form, or as the string
   * "reasoning" / "thinking" inside the array form. See the
   * normalizeCapabilities helper below for both shapes.
   */
  reasoning?: boolean;
  /**
   * Optional cap on the reasoning budget for this model. UI uses this
   * to clamp/initialize the budget slider; RicaProxy passes it through
   * to body.thinking.budget_tokens. When undefined, the runtime falls
   * back to options.reasoningBudgetTokens or the 2048 default.
   */
  reasoningBudgetMax?: number;
  [key: string]: unknown;
};

/**
 * Per-model consumption limit returned by the backend. Drives the
 * SessionUsageBar, weighted gating, and Tier-S availability lock.
 * `tokens_in` and `tokens_out` are caps within `rangeHours`. `blockedUntil`
 * is the backend-set sticky ms-epoch when the user has exceeded the cap.
 */
export interface RicaConsumptionLimit {
  rangeHours?: number;
  tokens_in?: number;
  tokens_out?: number;
  blockedUntil?: number;
  [key: string]: unknown;
}

export interface RicaDynamicModel {
  title?: string;
  name?: string;
  provider?: string;
  underlyingProviderName?: string;
  model?: string;
  /** Backend Mongo ObjectId — preferred over the path-tail `model_id`. */
  id?: string;
  model_id?: string;
  contextLength?: number;
  context_length?: number;
  capabilities?: RicaModelCapabilities | string[];
  apiBase?: string;
  apiKey?: string;
  apiKeyLocation?: string;
  roles?: string[];
  defaultCompletionOptions?: {
    maxTokens?: number;
    contextLength?: number;
    temperature?: number;
    [key: string]: unknown;
  };
  requestOptions?: Record<string, unknown>;
  env?: Record<string, string | number | boolean>;
  orgScopeId?: string | null;
  onPremProxyUrl?: string | null;
  maxEmbeddingChunkSize?: number;
  maxEmbeddingBatchSize?: number;
  /** Per-model consumption window. Drives gating + lock UI. */
  consumptionLimit?: RicaConsumptionLimit;
  /** Backend admin override for multi-agent eligibility. Falls back to
   *  substring match on model id when omitted. */
  multiAgentEligible?: boolean;
  [key: string]: unknown;
}

export interface RicaNormalizedModel extends RicaDynamicModel {
  title: string;
  provider: string;
  model: string;
  contextLength?: number;
  capabilities?: RicaModelCapabilities;
  roles?: string[];
}

// Phase 5.1.3 (close Trap B): the old `databricks → openai` alias rewrote
// Sang-backend models so they instantiated upstream `OpenAI` — which routed
// via [defaultOptions.apiBase = "https://api.openai.com/v1/"]
// (core/llm/llms/OpenAI.ts), exfiltrating Renesas prompts to OpenAI. The
// alias is now `databricks → rica-proxy` instead, so any backend payload
// that arrives with `provider: "databricks"` lands on the Rica adapter
// (which talks to the Databricks workspace via Sang's proxy) rather than
// upstream OpenAI. Missing-provider default is also `rica-proxy` (line
// below). See recon-supplement Bug 2 Trap B.
const PROVIDER_ALIAS_MAP: Record<string, string> = {
  databricks: "rica-proxy",
};

function normalizeCapabilities(
  capabilities: RicaDynamicModel["capabilities"],
): RicaModelCapabilities | undefined {
  if (!capabilities) {
    return undefined;
  }

  if (Array.isArray(capabilities)) {
    return {
      tools: capabilities.includes("tool_use"),
      uploadImage: capabilities.includes("image_input"),
      nextEdit: capabilities.includes("next_edit"),
      // Phase 5.x — accept either spelling so backend tickbox copy can be
      // either "reasoning" or "thinking" without coordination with the
      // client. Object-form callers (defined below) bypass this and use
      // the boolean directly.
      reasoning:
        capabilities.includes("reasoning") || capabilities.includes("thinking"),
    };
  }

  return capabilities;
}

function normalizeRoles(
  roles: RicaDynamicModel["roles"],
  provider: string,
): string[] {
  if (Array.isArray(roles) && roles.length > 0) {
    // Backend uses "embedding"; the runtime config schema expects "embed".
    // Mirrors v1.2.0 EntraIDDynamicConfigService.validateAndNormalizeConfig.
    return roles.map((role) => (role === "embedding" ? "embed" : role));
  }

  const defaults = ["chat"];

  if (provider === "openai") {
    return [...defaults, "summarize", "apply", "edit"];
  }

  if (provider === "rica-proxy") {
    return [...defaults, "summarize", "apply", "edit"];
  }

  if (provider === "continue-proxy") {
    return [...defaults, "summarize", "apply", "edit"];
  }

  return defaults;
}

// Phase 5.4.1 — backend contextLength floor (client-side stop-gap for
// [[project-bug-backend-contextlength-4096]]). Sang's /api/user-model/configs
// currently returns `contextLength: 4096, maxTokens: 0` for several models
// (Opus 4-8, GPT 4.1/Mini/nano, Gemini 3 pro/Flash, Claude 4 Sonnet, GPT 5.1).
// Chat/Agent tolerate 4096 (their system prompt is ~1.7K) but the Multi-Agent
// orchestrator system prompt is ~5.4K, so it overflows 4096 → upstream
// compileChatMessages throws "Message exceeds context limit" and Multi-Agent
// is unusable on those models. The real fix is Sang correcting the backend
// payload; until then, when the backend reports an implausibly small window
// (≤ FLOOR_TRIGGER), substitute a safe family-derived default so the
// orchestrator prompt fits. Models that legitimately advertise a large window
// are untouched. Remove this once the backend payload is corrected.
const CONTEXT_LENGTH_FLOOR_TRIGGER = 8192;

function familyContextDefault(modelId: string): number {
  const id = modelId.toLowerCase();
  // Anthropic Claude (Opus / Sonnet / Haiku) — 200K.
  if (id.includes("claude") || id.includes("opus") || id.includes("sonnet") || id.includes("haiku")) {
    return 200_000;
  }
  // OpenAI GPT-4.1 family — ~1M; GPT-5 family — ~400K.
  if (id.includes("gpt-4.1") || id.includes("gpt-41") || id.includes("gpt4.1")) {
    return 1_000_000;
  }
  if (id.includes("gpt-5") || id.includes("gpt5")) {
    return 400_000;
  }
  // Google Gemini — 1M.
  if (id.includes("gemini")) {
    return 1_000_000;
  }
  // Conservative generic floor — comfortably fits the ~5.4K orchestrator
  // prompt plus headroom, without over-promising on unknown models.
  return 128_000;
}

/**
 * Apply the floor: if the backend-reported window is missing or implausibly
 * small (≤ trigger), return a family default; otherwise trust the backend.
 */
function applyContextLengthFloor(
  reported: number | undefined,
  modelId: string,
): number | undefined {
  if (typeof reported === "number" && reported > CONTEXT_LENGTH_FLOOR_TRIGGER) {
    return reported;
  }
  return familyContextDefault(modelId);
}

export function normalizeProviderAlias(provider?: string): string {
  // Phase 5.1.3 (close Trap B): flip the missing-provider default from
  // `"openai"` to `"rica-proxy"`. Any backend model that arrives without
  // an explicit provider now routes through Rica's proxy instead of
  // silently leaking to api.openai.com via the upstream `OpenAI` class.
  if (!provider) {
    return "rica-proxy";
  }

  const normalized = provider.trim();
  return PROVIDER_ALIAS_MAP[normalized] ?? normalized;
}

export function normalizeRicaModel(
  model: RicaDynamicModel,
): RicaNormalizedModel {
  const rawProvider =
    typeof model.provider === "string"
      ? model.provider
      : typeof model.underlyingProviderName === "string"
        ? model.underlyingProviderName
        : undefined;

  const provider = normalizeProviderAlias(rawProvider);
  const modelName =
    typeof model.model === "string"
      ? model.model
      : typeof model.model_id === "string"
        ? model.model_id
        : typeof model.name === "string"
          ? model.name
          : "";

  // Prefer backend's `id` (Mongo ObjectId — required by Sang's consumption
  // POST endpoint, validated against /^[0-9a-f]{24}$/). Fall back to the
  // path tail for older configs that don't include `id`. Mirrors v1.2.0
  // EntraIDDynamicConfigService.validateAndNormalizeConfig.
  const resolvedModelId =
    typeof model.id === "string"
      ? model.id
      : typeof model.model_id === "string"
        ? model.model_id
        : typeof model.model === "string"
          ? model.model.split("/").pop()
          : undefined;

  return {
    ...model,
    title:
      typeof model.title === "string"
        ? model.title
        : typeof model.name === "string"
          ? model.name
          : modelName,
    provider,
    // Phase 5.1.3: stale `databricks → openai` rewrite removed alongside
    // the alias map clear above.
    underlyingProviderName: model.underlyingProviderName,
    model: modelName,
    model_id: resolvedModelId,
    // Phase 5.4.1: apply the backend contextLength floor. Family detection
    // keys off both the model string and the title (e.g. title "Opus 4-8"
    // matches "opus", model "databricks-claude-opus-4-8" matches "claude").
    contextLength: applyContextLengthFloor(
      typeof model.contextLength === "number"
        ? model.contextLength
        : typeof model.context_length === "number"
          ? model.context_length
          : undefined,
      `${modelName} ${typeof model.title === "string" ? model.title : ""}`,
    ),
    capabilities: normalizeCapabilities(model.capabilities),
    roles: normalizeRoles(model.roles, provider),
    // consumptionLimit + multiAgentEligible flow through ...model spread.
  };
}

export function normalizeRicaModels(
  models: RicaDynamicModel[],
): RicaNormalizedModel[] {
  return models
    .map(normalizeRicaModel)
    .filter((model) => !!model.provider && !!model.model && !!model.title);
}

export async function getRicaModelsEndpoint(
  ideSettingsPromise: Promise<IdeSettings>,
): Promise<string> {
  const env = await getControlPlaneEnv(ideSettingsPromise);
  return new URL("/api/user-model/configs", env.APP_URL).toString();
}
