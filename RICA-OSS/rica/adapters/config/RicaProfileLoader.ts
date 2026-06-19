import {
  fetchRicaDynamicConfig,
  type RicaDynamicConfigFetchOptions,
  type RicaDynamicConfigSnapshot,
} from "./dynamicConfig";
import type { IdeSettings } from "./env";
import type { EntraIDDynamicConfig } from "../../../core/index.js";
import type { RicaConsumptionLimit } from "./modelMapping";

type RicaModelRole =
  | "autocomplete"
  | "chat"
  | "embed"
  | "rerank"
  | "edit"
  | "apply"
  | "summarize"
  | "subagent";

interface RicaAssistantModel {
  name: string;
  provider: string;
  model: string;
  roles?: RicaModelRole[];
  apiBase?: string;
  apiKey?: string;
  apiKeyLocation?: string;
  contextLength?: number;
  capabilities?: string[];
  defaultCompletionOptions?: Record<string, unknown>;
  env?: Record<string, string | number | boolean>;
  requestOptions?: Record<string, unknown>;
  embedOptions?: {
    maxBatchSize?: number;
    maxChunkSize?: number;
  };
  // v1.2.0 parity — consumption + multi-agent fields forwarded from
  // RicaDynamicModel through to the runtime ModelDescription. Without
  // these, the multi-agent Tier-S lock + SessionUsageBar + auto-switch
  // logic can't read what the backend is telling them.
  model_id?: string;
  consumptionLimit?: RicaConsumptionLimit;
  multiAgentEligible?: boolean;
  orgScopeId?: string | null;
  onPremProxyUrl?: string | null;
}

interface RicaAssistantUnrolled {
  name: string;
  version: string;
  schema: "v1";
  models: RicaAssistantModel[];
  defaultModel?: string;
  context?: unknown[];
  tabAutocompleteModel?: {
    name: string;
    provider: string;
    model: string;
  };
  embeddingsProvider?: {
    provider: string;
    model: string;
    apiBase?: string;
    apiKey?: string;
    requestOptions?: Record<string, unknown>;
    maxEmbeddingBatchSize?: number;
    maxEmbeddingChunkSize?: number;
  };
  reranker?: {
    name: string;
    params: Record<string, unknown>;
  };
}

/**
 * Phase 5.1.2: detect unrendered `${{ secrets.X }}` templates so we can
 * substitute a literal token before clientRender sees the model. If any
 * such template reaches `useProxyForUnrenderedSecrets`, that helper rewrites
 * the model to `provider: "continue-proxy"` and re-bases the apiBase against
 * `DEFAULT_CONTROL_PLANE_PROXY_URL` — which Rica points at Microsoft login —
 * yielding a Microsoft-login URL as the chat endpoint. See recon-supplement
 * Bug 2 Trap A. Closing the trap means: never let a template-shaped string
 * occupy the apiKey field in the unrolled assistant.
 */
const UNRENDERED_SECRET_PATTERN = /\$\{\{\s*secrets\./;

function isUnrenderedSecret(value: unknown): boolean {
  return typeof value === "string" && UNRENDERED_SECRET_PATTERN.test(value);
}

/**
 * Phase 5.1.2: clientRender's `useProxyForUnrenderedSecrets` ALSO scans
 * each model's `env` map and rewrites the model to `provider: "continue-proxy"`
 * if any value is an unrendered template. Strip those keys from env before
 * the model reaches clientRender, so a stray template in env can't trigger
 * the rewrite even if the apiKey itself is fine.
 */
function scrubEnvSecrets(
  env: Record<string, string | number | boolean> | undefined,
): Record<string, string | number | boolean> | undefined {
  if (!env) return env;
  const cleaned: Record<string, string | number | boolean> = {};
  for (const [key, value] of Object.entries(env)) {
    if (isUnrenderedSecret(value)) continue;
    cleaned[key] = value;
  }
  return cleaned;
}

export class RicaProfileLoader {
  constructor(
    private readonly ideSettingsPromise: Promise<IdeSettings>,
    private readonly fetchImpl: typeof fetch = fetch,
    private readonly getBackendToken?: () => Promise<string | undefined>,
    private readonly getModelToken?: () => Promise<string | undefined>,
  ) {}

  async loadDynamicModels(
    options?: RicaDynamicConfigFetchOptions,
  ): Promise<RicaDynamicConfigSnapshot> {
    const backendToken = await this.getBackendToken?.();
    const mergedOptions: RicaDynamicConfigFetchOptions = {
      ...options,
      accessToken: backendToken ?? options?.accessToken,
    };
    return fetchRicaDynamicConfig(
      this.ideSettingsPromise,
      this.fetchImpl,
      mergedOptions,
    );
  }

  /**
   * v1.2.0 parity: expose the full backend dynamic-config block (every
   * passthrough field that v1.2.0's `EntraIDDynamicConfig` carried) so
   * core/config/load.ts can merge it into the runtime ContinueConfig via
   * mergeEntraIDDynamicConfig. This is the channel that lets backend-set
   * systemMessage / completionOptions / requestOptions / ui /
   * experimental / analytics / docs / context providers reach the runtime
   * — the same way v1.2.0's `EntraIDDynamicConfigService.fetchDynamicConfig`
   * + `mergeDynamicConfig` did.
   */
  async loadDynamicEntraIDConfig(
    options?: RicaDynamicConfigFetchOptions,
  ): Promise<EntraIDDynamicConfig | null> {
    const snapshot = await this.loadDynamicModels(options);
    if (snapshot.models.length === 0 && !snapshot.context && !snapshot.docs) {
      return null;
    }
    return {
      models: snapshot.models,
      embeddingsProvider: snapshot.embeddingsProvider,
      tabAutocompleteModel: snapshot.tabAutocompleteModel as any,
      context: snapshot.context as any,
      systemMessage: snapshot.systemMessage,
      completionOptions: snapshot.completionOptions as any,
      requestOptions: snapshot.requestOptions as any,
      ui: snapshot.ui as any,
      experimental: snapshot.experimental as any,
      analytics: snapshot.analytics as any,
      docs: snapshot.docs as any,
    };
  }

  async loadDynamicAssistant(
    options?: RicaDynamicConfigFetchOptions,
  ): Promise<RicaAssistantUnrolled | null> {
    const modelToken = await this.getModelToken?.();
    const snapshot = await this.loadDynamicModels(options);
    if (snapshot.models.length === 0) {
      return null;
    }

    const chatModel =
      snapshot.models.find((model) => model.roles?.includes("chat")) ??
      snapshot.models[0];
    const autocompleteModel =
      snapshot.models.find((model) => model.roles?.includes("autocomplete")) ??
      snapshot.models.find((model) => model.capabilities?.nextEdit);
    const embedModel = snapshot.models.find((model) =>
      model.roles?.includes("embed"),
    );
    const rerankModel = snapshot.models.find((model) =>
      model.roles?.includes("rerank"),
    );

    return {
      name: "RICA Dynamic",
      version: snapshot.updatedAt ?? "dynamic",
      schema: "v1",
      models: snapshot.models.map((model) => ({
        name: model.title,
        provider: model.provider,
        model: model.model,
        roles: model.roles as RicaModelRole[] | undefined,
        apiBase: typeof model.apiBase === "string" ? model.apiBase : undefined,
        // Phase 5.1.2 (close Trap A): if backend returned an unrendered
        // `${{ secrets.X }}` template (or no apiKey at all), substitute a
        // real token before the model reaches clientRender —
        // `useProxyForUnrenderedSecrets` must NEVER see one of those, or
        // it rewrites provider to "continue-proxy" and re-bases the
        // apiBase against DEFAULT_CONTROL_PLANE_PROXY_URL (a Microsoft
        // login URL in the Rica build). modelToken is the
        // EntraID-via-Sang-backend bearer; clientRender accepts a literal.
        apiKey:
          typeof model.apiKey === "string" && !isUnrenderedSecret(model.apiKey)
            ? model.apiKey
            : modelToken,
        apiKeyLocation:
          typeof model.apiKeyLocation === "string"
            ? model.apiKeyLocation
            : undefined,
        contextLength: model.contextLength,
        defaultCompletionOptions: model.defaultCompletionOptions,
        capabilities: model.capabilities
          ? [
              ...(model.capabilities.tools ? ["tool_use"] : []),
              ...(model.capabilities.uploadImage ? ["image_input"] : []),
              ...(model.capabilities.nextEdit ? ["next_edit"] : []),
            ]
          : undefined,
        // Phase 5.1.2: scrub env to keep `useProxyForUnrenderedSecrets`
        // from firing via the env-block path.
        env: scrubEnvSecrets(model.env),
        requestOptions: model.requestOptions,
        embedOptions:
          model.maxEmbeddingBatchSize || model.maxEmbeddingChunkSize
            ? {
                maxBatchSize: model.maxEmbeddingBatchSize,
                maxChunkSize: model.maxEmbeddingChunkSize,
              }
            : undefined,
        // v1.2.0 parity passthrough — see RicaAssistantModel definition.
        model_id: model.model_id,
        consumptionLimit: model.consumptionLimit,
        multiAgentEligible: model.multiAgentEligible,
        orgScopeId: model.orgScopeId,
        onPremProxyUrl: model.onPremProxyUrl,
      })),
      defaultModel: chatModel?.title,
      tabAutocompleteModel: autocompleteModel
        ? {
            name: autocompleteModel.title,
            provider: autocompleteModel.provider,
            model: autocompleteModel.model,
          }
        : undefined,
      embeddingsProvider: embedModel
        ? {
            provider: embedModel.provider,
            model: embedModel.model,
            apiBase:
              typeof embedModel.apiBase === "string"
                ? embedModel.apiBase
                : undefined,
            // Phase 5.1.2: same Trap A scrub as chat models.
            apiKey:
              typeof embedModel.apiKey === "string" &&
              !isUnrenderedSecret(embedModel.apiKey)
                ? embedModel.apiKey
                : modelToken,
            requestOptions: embedModel.requestOptions,
            maxEmbeddingBatchSize: embedModel.maxEmbeddingBatchSize,
            maxEmbeddingChunkSize: embedModel.maxEmbeddingChunkSize,
          }
        : undefined,
      reranker: rerankModel
        ? {
            name: rerankModel.provider,
            params: {
              model: rerankModel.model,
              apiBase:
                typeof rerankModel.apiBase === "string"
                  ? rerankModel.apiBase
                  : undefined,
            },
          }
        : undefined,
      context: snapshot.context,
    };
  }
}
