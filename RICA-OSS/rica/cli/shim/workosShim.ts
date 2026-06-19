/**
 * RICA CLI WorkOS Shim
 *
 * Drop-in replacement for extensions/cli/src/auth/workos.ts.
 * Injected at build time via esbuild onResolve plugin in rica/scripts/build-cli.mjs.
 *
 * Delegates auth operations to rica/cli/auth/ (API key flow)
 * while maintaining the same export surface as the upstream WorkOS module.
 */

import {
  login as ricaLogin,
  logout as ricaLogout,
  loadAuthConfig as ricaLoadAuthConfig,
  saveAuthConfig as ricaSaveAuthConfig,
  isAuthenticated as ricaIsAuthenticated,
  getMdpToken,
  type AuthConfig,
  type AuthenticatedConfig,
  type EnvironmentAuthConfig,
} from "../auth/index.js";
import {
  loadRicaAuthConfig,
  saveRicaAuthConfig,
  type RicaCliAuthConfig,
} from "../auth/tokenStorage.js";

// Re-export types
export type { AuthConfig, AuthenticatedConfig, EnvironmentAuthConfig };

// Type guards
export function isAuthenticatedConfig(
  config: AuthConfig,
): config is AuthenticatedConfig {
  return config !== null && "userId" in config;
}

export function isEnvironmentAuthConfig(
  config: AuthConfig,
): config is EnvironmentAuthConfig {
  return config !== null && !("userId" in config);
}

// Accessors
export function getAccessToken(config: AuthConfig): string | null {
  if (!config) return null;
  return config.accessToken ?? null;
}

export function getOrganizationId(
  config: AuthConfig,
): string | null | undefined {
  if (!config) return null;
  return config.organizationId;
}

export function getConfigUri(config: AuthConfig): string | null {
  if (!config) return null;
  return (config as AuthenticatedConfig).configUri ?? null;
}

export function getModelName(config: AuthConfig): string | null {
  if (!config) return null;
  return (config as AuthenticatedConfig).modelName ?? null;
}

export function getAssistantSlug(_config: AuthConfig): string | null {
  return null;
}

export function getLocalConfigPath(_config: AuthConfig): string | null {
  return null;
}

// Auth state operations
export function loadAuthConfig(): AuthConfig {
  return ricaLoadAuthConfig();
}

export function saveAuthConfig(config: AuthenticatedConfig): void {
  ricaSaveAuthConfig(config);
}

export function updateConfigUri(_configUri: string | null): void {
  // RICA doesn't use config URIs (no slug:// or file:// config sources from hub)
}

export function updateModelName(modelName: string | null): AuthConfig {
  const auth = loadRicaAuthConfig();
  if (auth) {
    saveRicaAuthConfig({ ...auth, selectedModel: modelName } as any);
  }
  return ricaLoadAuthConfig();
}

export function updateAssistantSlug(_assistantSlug: string | null): void {
  // RICA doesn't use assistant slugs
}

export function updateLocalConfigPath(_localConfigPath: string | null): void {
  // RICA doesn't use this
}

// Auth flow
export async function isAuthenticated(): Promise<boolean> {
  return ricaIsAuthenticated();
}

export async function login(): Promise<AuthConfig> {
  return ricaLogin();
}

export function logout(): void {
  ricaLogout();
}

// Org management (RICA is single-org — stubs)
export async function ensureOrganization(
  authConfig: AuthConfig,
  _isHeadless?: boolean,
  _cliOrganizationSlug?: string,
): Promise<AuthConfig> {
  return authConfig;
}

export async function listUserOrganizations(): Promise<Array<{
  id: string;
  name: string;
  slug: string;
}> | null> {
  return null;
}

export async function hasMultipleOrganizations(): Promise<boolean> {
  return false;
}
