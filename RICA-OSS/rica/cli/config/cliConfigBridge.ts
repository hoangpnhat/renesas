/**
 * RICA CLI Config Bridge
 *
 * Bridges rica/adapters/config/RicaProfileLoader into the format
 * expected by the CLI's config system (AssistantUnrolled from @continuedev/config-yaml).
 *
 * This allows the CLI to load RICA dynamic models using the same
 * dual-token architecture as the VS Code extension.
 */

import { RicaProfileLoader } from "../../adapters/config/RicaProfileLoader.js";
import type { IdeSettings } from "../../adapters/config/env.js";
import { getMdpToken } from "../auth/index.js";
import { detectRicaEnvironment } from "./cliEnv.js";

/**
 * Create a RicaProfileLoader configured for CLI usage.
 * Uses the CLI's token storage for dual-token auth.
 */
export function createCliProfileLoader(): RicaProfileLoader {
  const environment = detectRicaEnvironment();
  const ideSettingsPromise: Promise<IdeSettings> = Promise.resolve({
    continueTestEnvironment: environment,
  });

  return new RicaProfileLoader(
    ideSettingsPromise,
    fetch,
    () => getMdpToken(),
    () => getMdpToken(),
  );
}

/**
 * Load RICA dynamic models and return in a format the CLI can consume.
 * Returns null if not authenticated or no models available.
 */
export async function loadRicaDynamicConfig(): Promise<{
  models: Array<{
    name: string;
    provider: string;
    model: string;
    apiBase?: string;
    apiKey?: string;
    roles?: string[];
  }>;
  defaultModel?: string;
} | null> {
  try {
    const loader = createCliProfileLoader();
    const assistant = await loader.loadDynamicAssistant();

    if (!assistant) {
      return null;
    }

    return {
      models: assistant.models.map((m) => ({
        name: m.name,
        provider: m.provider,
        model: m.model,
        apiBase: m.apiBase,
        apiKey: m.apiKey,
        roles: m.roles,
      })),
      defaultModel: assistant.defaultModel,
    };
  } catch {
    return null;
  }
}
