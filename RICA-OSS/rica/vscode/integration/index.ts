/**
 * RICA VS Code Integration Layer
 *
 * This is the main integration boundary between Continue upstream
 * and RICA product overlay.
 *
 * Architecture per RICA_REPO_STANDARD.md:
 * - Extension Integration Layer: rica/vscode/integration/
 * - Auth boundary: rica/adapters/auth/
 * - Config boundary: rica/adapters/config/
 *
 * Usage:
 * - Import from this module in extensions/vscode/
 * - Minimal patches to VsCodeExtension and VsCodeMessenger
 * - All RICA-specific logic stays in rica/
 */

export { RicaAuthProvider, getRicaControlPlaneSessionInfo } from "./auth";

export { getRicaEnvironmentConfig } from "./config";

// Re-export types from adapters for convenience
export type {
  ControlPlaneSessionInfo,
  HubSessionInfo,
  OnPremSessionInfo,
  AuthType,
} from "../../adapters/auth/AuthTypes";

export type { ControlPlaneEnv } from "../../adapters/config/env";
