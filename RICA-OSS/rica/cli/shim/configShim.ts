/**
 * RICA CLI Config Shim
 *
 * Drop-in replacement for extensions/cli/src/config.ts.
 * Handles rica-proxy models via RicaProxyAdapter (Databricks serving endpoints)
 * instead of routing through ContinueProxyApi which uses the wrong URL format.
 */

import type { AssistantUnrolled, ModelConfig } from "@continuedev/config-yaml";
import {
  type BaseLlmApi,
  constructLlmApi,
  type LLMConfig,
} from "@continuedev/openai-adapters";

import { type AuthConfig, getAccessToken } from "./workosShim.js";
import { env } from "./envShim.js";
import { posthogService } from "./posthogShim.js";
import { getVersion } from "./versionShim.js";
import { RicaProxyAdapter } from "./ricaProxyAdapter.js";

function getUserAgent(): string {
  const version = getVersion();
  return `RICA-CLI/${version}`;
}

function mergeUserAgentIntoRequestOptions(
  requestOptions: ModelConfig["requestOptions"],
): ModelConfig["requestOptions"] {
  return {
    ...requestOptions,
    headers: {
      ...requestOptions?.headers,
      "user-agent": getUserAgent(),
      "x-continue-unique-id": posthogService.uniqueId,
    },
  };
}

export function createLlmApi(
  model: ModelConfig,
  authConfig: AuthConfig | null,
): BaseLlmApi | null {
  // RICA-specific: handle rica-proxy via RicaProxyAdapter (Databricks serving endpoints)
  // Token is fetched lazily by the adapter via ensureValidToken()
  if (model.provider === "rica-proxy" || model.provider === "continue-proxy") {
    const apiBase = model.apiBase || env.apiBase;
    return new RicaProxyAdapter({
      apiBase,
      model: model.model,
    });
  }

  // Fallback: other providers use standard constructLlmApi
  const accessToken = getAccessToken(authConfig);
  const config: LLMConfig = {
    provider: model.provider as any,
    model: model.model,
    apiKey: model.apiKey ?? accessToken ?? undefined,
    apiBase: model.apiBase,
    requestOptions: mergeUserAgentIntoRequestOptions(model.requestOptions),
    env: model.env,
  };

  return constructLlmApi(config) ?? null;
}

export function getLlmApi(
  assistant: AssistantUnrolled,
  authConfig: AuthConfig,
): [BaseLlmApi, ModelConfig] {
  if (!assistant.models || assistant.models.length === 0) {
    throw new Error("No models found in the configured assistant");
  }

  const model = assistant.models?.find(
    (model) =>
      model?.roles?.includes("chat") || (model && model.roles === undefined),
  );

  if (!model) {
    throw new Error("No chat-capable model found in configuration");
  }

  const api = createLlmApi(model, authConfig);
  if (!api) {
    throw new Error(`Failed to initialize LLM API for model: ${model.name}`);
  }

  return [api, model];
}

export function getApiClient(_authConfig: AuthConfig) {
  // Return a minimal stub — the SDK's DefaultApi is not used for LLM calls.
  // Org selection and hub features are disabled in RICA CLI.
  return {
    listOrganizations: async () => ({ organizations: [] }),
    getDefaultAssistant: async () => null,
  } as any;
}
