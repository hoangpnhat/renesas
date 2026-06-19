/**
 * RICA CLI Token Refresh
 *
 * Each CLI invocation exchanges the stored API key for a fresh MDP token.
 * Token is kept in memory only — never written to disk.
 */

import { exchangeApiKeyForToken } from "./index.js";
import { type RicaCliAuthConfig, loadRicaConfig } from "./tokenStorage.js";

let cachedToken: RicaCliAuthConfig | null = null;

/**
 * Exchange API key for a fresh MDP token (in-memory only).
 */
export async function refreshTokens(): Promise<RicaCliAuthConfig> {
  const config = loadRicaConfig();
  if (!config) {
    throw new Error("No API key found. Please run `rica login` first.");
  }

  const { mdpToken, expiresInMs, userEmail } = await exchangeApiKeyForToken(
    config.apiKey,
  );

  cachedToken = {
    mdpToken,
    expiresAt: Date.now() + expiresInMs,
    userEmail,
    environment: "production",
  };

  return cachedToken;
}

/**
 * Ensure we have a valid (non-expired) token.
 * Uses in-memory cache within the same process; re-exchanges if expired.
 */
export async function ensureValidToken(): Promise<RicaCliAuthConfig> {
  if (cachedToken && Date.now() < cachedToken.expiresAt) {
    return cachedToken;
  }

  return refreshTokens();
}
