/**
 * RICA CLI Config Loader Shim
 *
 * Drop-in replacement for extensions/cli/src/configLoader.ts.
 * Replaces the upstream Continue hub config loading with:
 *   1. CLI --config flag (local YAML)
 *   2. ~/.rica/config.yaml (local YAML)
 *   3. RICA backend dynamic models via RicaProfileLoader
 */

import * as fs from "fs";
import * as path from "path";

import {
  type AssistantUnrolled,
  type PackageIdentifier,
  RegistryClient,
  unrollAssistantFromContent,
} from "@continuedev/config-yaml";
import type { DefaultApiInterface } from "@continuedev/sdk/dist/api/dist/index.js";

import { loadRicaDynamicConfig } from "../config/cliConfigBridge.js";
import { ricaCliEnv } from "../config/cliEnv.js";

import type { AuthConfig } from "../auth/index.js";

export interface ConfigLoadResult {
  config: AssistantUnrolled;
  source: ConfigSource;
}

export type ConfigSource =
  | { type: "cli-flag"; path: string }
  | { type: "saved-uri"; uri: string }
  | { type: "user-assistant"; slug: string }
  | { type: "local-config-yaml" }
  | { type: "remote-default-config" }
  | { type: "no-config" };

const DEFAULT_CONFIG_YAML = `name: RICA
version: "1.0"
schema: v1
models:
  - name: RICA Proxy
    provider: continue-proxy
    model: databricks-claude-sonnet-4-6
    roles:
      - chat
`;

export async function loadConfiguration(
  authConfig: AuthConfig,
  cliConfigPath: string | undefined,
  _apiClient: DefaultApiInterface,
  _injectBlocks: PackageIdentifier[],
  _isHeadless: boolean | undefined,
): Promise<ConfigLoadResult> {
  // Priority 1: CLI --config flag
  if (cliConfigPath) {
    const config = await loadLocalYaml(cliConfigPath);
    return { config, source: { type: "cli-flag", path: cliConfigPath } };
  }

  // Priority 2: RICA backend dynamic models (if authenticated)
  if (authConfig && authConfig.accessToken) {
    try {
      const dynamicConfig = await loadRicaDynamicConfig();
      if (dynamicConfig && dynamicConfig.models.length > 0) {
        const assistant: AssistantUnrolled = {
          name: "RICA Dynamic",
          version: "dynamic",
          schema: "v1",
          models: dynamicConfig.models.map((m) => ({
            name: m.name,
            provider: m.provider as any,
            model: m.model,
            roles: m.roles as any,
            apiBase: m.apiBase,
            apiKey: m.apiKey,
          })),
        };
        return {
          config: assistant,
          source: { type: "remote-default-config" },
        };
      }
    } catch {
      // Fall through to local config
    }
  }

  // Priority 3: Local ~/.rica/config.yaml
  const localConfigPath = path.join(ricaCliEnv.ricaHome, "config.yaml");
  if (fs.existsSync(localConfigPath)) {
    const config = await loadLocalYaml(localConfigPath);
    return { config, source: { type: "local-config-yaml" } };
  }

  // Fallback: create default config.yaml and return it
  const ricaHome = ricaCliEnv.ricaHome;
  if (!fs.existsSync(ricaHome)) {
    fs.mkdirSync(ricaHome, { recursive: true });
  }
  fs.writeFileSync(localConfigPath, DEFAULT_CONFIG_YAML, "utf8");

  const config = await loadLocalYaml(localConfigPath);
  return { config, source: { type: "local-config-yaml" } };
}

async function loadLocalYaml(filePath: string): Promise<AssistantUnrolled> {
  const absolutePath = path.isAbsolute(filePath)
    ? filePath
    : path.resolve(process.cwd(), filePath);

  if (!fs.existsSync(absolutePath)) {
    throw new Error(`Config file not found: ${absolutePath}`);
  }

  const content = fs.readFileSync(absolutePath, "utf8");

  const unrollResult = await unrollAssistantFromContent(
    { uriType: "file", fileUri: absolutePath },
    content,
    new RegistryClient({
      accessToken: undefined,
      apiBase: ricaCliEnv.apiBase,
      rootPath: path.dirname(absolutePath),
    }),
    {
      currentUserSlug: "",
      onPremProxyUrl: null,
      orgScopeId: null,
      renderSecrets: false,
      injectBlocks: [],
    },
  );

  if (!unrollResult.config) {
    const errors = unrollResult.errors?.map((e) => e.message).join("\n");
    throw new Error(`Failed to parse config: ${errors || "unknown error"}`);
  }

  return unrollResult.config;
}

export async function unrollPackageIdentifiersAsConfigYaml(
  _packageIdentifiers: PackageIdentifier[],
  _accessToken: string | null,
  _organizationId: string | null,
  _apiClient: DefaultApiInterface,
): Promise<AssistantUnrolled> {
  return {
    name: "RICA",
    version: "1.0",
    schema: "v1",
    models: [],
  };
}
