/**
 * RICA Path Utilities
 *
 * This module provides RICA-specific path utilities that are separate from
 * Continue's core path utilities. This follows REPO_STANDARD.md principles:
 * - RICA-specific logic belongs in rica/**
 * - Prefer wrappers/adapters over direct upstream modification
 *
 * The RICA global directory (~/.rica) is independent from Continue's ~/.continue
 * directory, allowing both to coexist on the same machine without conflicts.
 */

import * as fs from "node:fs";
import * as os from "os";
import * as path from "path";

// ============================================================================
// RICA Global Directory
// ============================================================================

/**
 * Get the RICA global directory path.
 *
 * Defaults to ~/.rica, but can be overridden via RICA_GLOBAL_DIR environment variable.
 * This is analogous to Continue's CONTINUE_GLOBAL_DIR but for RICA-specific data.
 *
 * @returns The absolute path to the RICA global directory
 */
export function getRicaGlobalPath(): string {
  const configPath = process.env.RICA_GLOBAL_DIR;
  if (configPath) {
    return path.isAbsolute(configPath)
      ? configPath
      : path.resolve(process.cwd(), configPath);
  }
  return path.join(os.homedir(), ".rica");
}

/**
 * Ensure the RICA global directory exists.
 * Creates it if it doesn't exist.
 *
 * @returns The absolute path to the RICA global directory
 */
export function ensureRicaGlobalPath(): string {
  const ricaPath = getRicaGlobalPath();
  if (!fs.existsSync(ricaPath)) {
    fs.mkdirSync(ricaPath, { recursive: true });
  }
  return ricaPath;
}

// ============================================================================
// Environment Dot Files
// ============================================================================

/**
 * Get the path to the .local environment dot file.
 * When this file exists at ~/.rica/.local, RICA switches to LOCAL_ENV.
 *
 * @returns The absolute path to ~/.rica/.local
 */
export function getLocalEnvironmentDotFilePath(): string {
  return path.join(getRicaGlobalPath(), ".local");
}

/**
 * Get the path to the .staging environment dot file.
 * When this file exists at ~/.rica/.staging, RICA switches to STAGING_ENV (non-prod).
 *
 * @returns The absolute path to ~/.rica/.staging
 */
export function getStagingEnvironmentDotFilePath(): string {
  return path.join(getRicaGlobalPath(), ".staging");
}

// ============================================================================
// RICA Subdirectories
// ============================================================================

/**
 * Get the path to a named subdirectory within the RICA global directory.
 *
 * @param name - The subdirectory name
 * @returns The absolute path to ~/.rica/{name}
 */
export function getRicaGlobalFolderWithName(name: string): string {
  return path.join(getRicaGlobalPath(), name);
}

/**
 * Get the path to RICA logs directory.
 *
 * @returns The absolute path to ~/.rica/logs
 */
export function getRicaLogsPath(): string {
  const logsPath = getRicaGlobalFolderWithName("logs");
  if (!fs.existsSync(logsPath)) {
    fs.mkdirSync(logsPath, { recursive: true });
  }
  return logsPath;
}

/**
 * Get the path to RICA sessions directory.
 *
 * @returns The absolute path to ~/.rica/sessions
 */
export function getRicaSessionsPath(): string {
  const sessionsPath = getRicaGlobalFolderWithName("sessions");
  if (!fs.existsSync(sessionsPath)) {
    fs.mkdirSync(sessionsPath, { recursive: true });
  }
  return sessionsPath;
}
