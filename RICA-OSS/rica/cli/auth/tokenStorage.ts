/**
 * RICA CLI Token Storage
 *
 * Manages persistent auth state:
 * - ~/.rica/config.json — long-lived API key (like Claude's config)
 * - ~/.rica/auth.json — ephemeral MDP token session
 */

import * as fs from "node:fs";
import * as path from "node:path";

import { ensureRicaGlobalPath, getRicaGlobalPath } from "../../utils/paths.js";

export interface RicaCliConfig {
  apiKey: string;
}

export interface RicaCliAuthConfig {
  mdpToken: string;
  expiresAt: number;
  userEmail: string;
  userName?: string;
  environment: string;
}

function getConfigFilePath(): string {
  return path.join(getRicaGlobalPath(), "config.json");
}

function getAuthFilePath(): string {
  return path.join(getRicaGlobalPath(), "auth.json");
}

export function loadRicaConfig(): RicaCliConfig | null {
  try {
    const configPath = getConfigFilePath();
    if (!fs.existsSync(configPath)) {
      return null;
    }

    const data = JSON.parse(fs.readFileSync(configPath, "utf8"));
    if (data.apiKey && typeof data.apiKey === "string") {
      return data as RicaCliConfig;
    }

    return null;
  } catch {
    return null;
  }
}

export function saveRicaConfig(config: RicaCliConfig): void {
  try {
    ensureRicaGlobalPath();
    const configPath = getConfigFilePath();
    fs.writeFileSync(configPath, JSON.stringify(config, null, 2), {
      mode: 0o600,
    });
  } catch (error) {
    throw new Error(`Failed to save config: ${error}`);
  }
}

export function clearRicaConfig(): boolean {
  try {
    const configPath = getConfigFilePath();
    if (fs.existsSync(configPath)) {
      fs.unlinkSync(configPath);
      return true;
    }
    return false;
  } catch {
    return false;
  }
}

export function loadRicaAuthConfig(): RicaCliAuthConfig | null {
  try {
    const authPath = getAuthFilePath();
    if (!fs.existsSync(authPath)) {
      return null;
    }

    const data = JSON.parse(fs.readFileSync(authPath, "utf8"));

    if (data.mdpToken && data.expiresAt && data.userEmail) {
      return data as RicaCliAuthConfig;
    }

    return null;
  } catch {
    return null;
  }
}

export function saveRicaAuthConfig(config: RicaCliAuthConfig): void {
  try {
    ensureRicaGlobalPath();
    const authPath = getAuthFilePath();
    fs.writeFileSync(authPath, JSON.stringify(config, null, 2), {
      mode: 0o600,
    });
  } catch (error) {
    throw new Error(`Failed to save auth config: ${error}`);
  }
}

export function clearRicaAuthConfig(): boolean {
  try {
    const authPath = getAuthFilePath();
    if (fs.existsSync(authPath)) {
      fs.unlinkSync(authPath);
      return true;
    }
    return false;
  } catch {
    return false;
  }
}

export function isTokenExpired(config: RicaCliAuthConfig): boolean {
  return Date.now() >= config.expiresAt;
}
