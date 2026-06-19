/**
 * RICA CLI Version Shim
 *
 * Drop-in replacement for extensions/cli/src/version.ts.
 * Returns RICA version from brand.json instead of package.json.
 * Disables upstream version-check against api.continue.dev.
 */

// @ts-ignore - JSON import resolved by esbuild bundler
import brandConfig from "../../branding/brand.json";
import { loadAuthConfig, isAuthenticatedConfig } from "../shim/workosShim.js";

export function getVersion(): string {
  return brandConfig.product.version;
}

export function getEventUserId(): string {
  const authConfig = loadAuthConfig();
  if (isAuthenticatedConfig(authConfig)) {
    return authConfig.userId;
  }
  try {
    const { machineIdSync } = require("node-machine-id");
    return machineIdSync();
  } catch {
    return "anonymous";
  }
}

export async function getLatestVersion(): Promise<string | null> {
  // RICA doesn't check for updates against continue.dev
  return null;
}

export async function checkForUpdate(): Promise<{
  isUpdateAvailable: boolean;
  latestVersion: string | null;
}> {
  return { isUpdateAvailable: false, latestVersion: null };
}

export function compareVersions(a: string, b: string): number {
  const partsA = a.split(".").map(Number);
  const partsB = b.split(".").map(Number);
  for (let i = 0; i < Math.max(partsA.length, partsB.length); i++) {
    const diff = (partsA[i] || 0) - (partsB[i] || 0);
    if (diff !== 0) return diff;
  }
  return 0;
}
