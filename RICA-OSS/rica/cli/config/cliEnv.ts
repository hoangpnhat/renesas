/**
 * RICA CLI Environment Configuration
 *
 * Provides RICA-specific environment values for the CLI.
 * Replaces upstream Continue environment (api.continue.dev, WorkOS, etc.)
 * with RICA backend URLs and EntraID configuration.
 *
 * Per REPO_STANDARD.md: RICA config belongs in rica/adapters/config/
 * This module bridges rica/adapters/config/ into CLI-consumable format.
 */

import { getRicaGlobalPath, ensureRicaGlobalPath } from "../../utils/paths.js";
import {
  getControlPlaneEnvSync,
  type IdeSettings,
} from "../../adapters/config/env.js";

function resolveAppUrl(): string {
  if (process.env.RICA_APP_URL) return process.env.RICA_APP_URL;
  if (process.env.HUB_URL) return process.env.HUB_URL;
  const env = getControlPlaneEnvSync(undefined);
  return env.APP_URL ?? "https://rica.global.renesas.com";
}

export const ricaCliEnv = {
  apiBase:
    process.env.RICA_API_BASE ??
    process.env.CONTINUE_API_BASE ??
    resolveAppUrl(),
  appUrl: resolveAppUrl(),
  ricaHome: getRicaGlobalPath(),
};

/**
 * Detect the current RICA environment using dot files in ~/.rica/
 * Same mechanism as VS Code extension (reuses rica/adapters/config/env.ts)
 */
export function detectRicaEnvironment(): IdeSettings["continueTestEnvironment"] {
  const env = getControlPlaneEnvSync(undefined);
  // Map ControlPlaneEnv back to environment name for IdeSettings
  if (env.APP_URL?.includes("localhost")) return "local";
  if (env.APP_URL?.includes("rica-uat")) return "staging";
  return "production";
}

/**
 * Get the full ControlPlaneEnv for the current RICA environment.
 * Contains tenant ID, client ID, auth URLs needed for EntraID flow.
 */
export function getRicaControlPlaneEnv() {
  return getControlPlaneEnvSync(detectRicaEnvironment());
}

/**
 * Ensure RICA home directory exists and return its path.
 */
export function ensureRicaHome(): string {
  return ensureRicaGlobalPath();
}
