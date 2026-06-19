/**
 * RICA CLI Auth Module
 *
 * Simple API key-based authentication:
 * 1. User runs `rica login` → pastes API key
 * 2. CLI saves key to ~/.rica/config.json
 * 3. CLI exchanges key for MDP token via POST /api/cli/token
 * 4. MDP token used for all LLM requests
 *
 * Token refresh: when MDP token expires, re-exchange API key automatically.
 */

import * as readline from "node:readline";

import chalk from "chalk";

import brandConfig from "../../branding/brand.json";
import { ricaCliEnv } from "../config/cliEnv.js";
import { ensureValidToken } from "./tokenRefresh.js";
import {
  type RicaCliAuthConfig,
  type RicaCliConfig,
  clearRicaAuthConfig,
  clearRicaConfig,
  loadRicaConfig,
  saveRicaConfig,
} from "./tokenStorage.js";

export type { RicaCliAuthConfig, RicaCliConfig } from "./tokenStorage.js";

export interface AuthenticatedConfig {
  userId: string;
  userEmail: string;
  accessToken: string;
  refreshToken: string;
  expiresAt: number;
  organizationId: string | null | undefined;
  configUri?: string;
  modelName?: string;
}

export interface EnvironmentAuthConfig {
  userId?: undefined;
  accessToken: string;
  organizationId: string | null;
  configUri?: string;
  modelName?: string;
}

export type AuthConfig = AuthenticatedConfig | EnvironmentAuthConfig | null;

interface TokenExchangeResponse {
  access_token: string;
  expires_in: number;
  user_email?: string;
}

/**
 * Exchange API key for MDP token via backend.
 */
export async function exchangeApiKeyForToken(apiKey: string): Promise<{
  mdpToken: string;
  expiresInMs: number;
  userEmail: string;
}> {
  const appUrl = ricaCliEnv.appUrl;

  const response = await fetch(`${appUrl}/api/extension/token`, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
      "X-API-Key": apiKey,
    },
    body: JSON.stringify({ version: brandConfig.product.version }),
  });

  if (!response.ok) {
    const errorText = await response.text();
    throw new Error(
      `Token exchange failed: HTTP ${response.status} - ${errorText}`,
    );
  }

  const data = (await response.json()) as TokenExchangeResponse;

  const expiresInMs = data.expires_in
    ? data.expires_in * 1000
    : 12 * 60 * 60 * 1000;

  return {
    mdpToken: data.access_token,
    expiresInMs,
    userEmail: data.user_email || "unknown",
  };
}

/**
 * Prompt user to enter API key via readline.
 */
function promptForApiKey(): Promise<string> {
  return new Promise((resolve, reject) => {
    const rl = readline.createInterface({
      input: process.stdin,
      output: process.stderr,
    });

    rl.question(chalk.cyan("Enter your RICA API key: "), (answer) => {
      rl.close();
      const trimmed = answer.trim();
      if (!trimmed) {
        reject(new Error("API key cannot be empty."));
        return;
      }
      resolve(trimmed);
    });
  });
}

/**
 * Load auth config, mapped to CLI AuthConfig shape.
 * For API key auth, returns the key directly (token exchange happens at request time).
 */
export function loadAuthConfig(): AuthConfig {
  if (process.env.RICA_API_KEY || process.env.CONTINUE_API_KEY) {
    return {
      accessToken: (process.env.RICA_API_KEY || process.env.CONTINUE_API_KEY)!,
      organizationId: null,
    };
  }

  const config = loadRicaConfig();
  if (!config) {
    return null;
  }

  // accessToken here is the API key — RicaProxyAdapter resolves MDP token via getMdpToken()
  return {
    userId: "",
    userEmail: "",
    accessToken: config.apiKey,
    refreshToken: "",
    expiresAt: 0,
    organizationId: null,
    modelName: undefined,
  };
}

/**
 * Check if user is authenticated (has API key stored or in env).
 */
export async function isAuthenticated(): Promise<boolean> {
  if (process.env.RICA_API_KEY || process.env.CONTINUE_API_KEY) {
    return true;
  }

  const config = loadRicaConfig();
  return config !== null;
}

/**
 * Login: prompt for API key → verify it works → save API key only.
 */
export async function login(): Promise<AuthConfig> {
  if (process.env.RICA_API_KEY || process.env.CONTINUE_API_KEY) {
    return {
      accessToken: (process.env.RICA_API_KEY || process.env.CONTINUE_API_KEY)!,
      organizationId: null,
    };
  }

  const apiKey = await promptForApiKey();

  // Verify key is valid by exchanging once (token not persisted)
  const { userEmail } = await exchangeApiKeyForToken(apiKey);

  // Only save the API key, never the MDP token
  saveRicaConfig({ apiKey });

  return {
    userId: userEmail,
    userEmail,
    accessToken: apiKey,
    refreshToken: "",
    expiresAt: 0,
    organizationId: null,
  };
}

/**
 * Logout: clear both config.json and auth.json.
 */
export function logout(): void {
  if (process.env.RICA_API_KEY || process.env.CONTINUE_API_KEY) {
    console.info(
      "Using API key from environment variables, nothing to log out",
    );
    return;
  }

  const removedAuth = clearRicaAuthConfig();
  const removedConfig = clearRicaConfig();

  if (removedAuth || removedConfig) {
    console.info("Successfully logged out");
  } else {
    console.info("No active session found");
  }
}

/**
 * Save auth config (compatibility with workos.ts interface).
 * No-op — MDP tokens are never persisted to disk.
 */
export function saveAuthConfig(_config: AuthenticatedConfig): void {
  // No-op: RICA CLI never saves MDP tokens to disk
}

/**
 * Get the MDP token (for model serving). Auto-refreshes if expired.
 */
export async function getMdpToken(): Promise<string | undefined> {
  try {
    const config = await ensureValidToken();
    return config.mdpToken;
  } catch {
    return undefined;
  }
}
