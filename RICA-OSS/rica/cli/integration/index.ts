/**
 * RICA CLI Integration Layer
 *
 * Single entry point for CLI patches to import from.
 * Analogous to rica/vscode/integration/index.ts for the VS Code extension.
 *
 * Usage in extensions/cli/:
 *   import { ricaAuth, cliStrings, ricaCliEnv } from "../../../../rica/cli/integration";
 */

// Auth (replaces WorkOS)
export {
  login,
  logout,
  loadAuthConfig,
  saveAuthConfig,
  isAuthenticated,
  getMdpToken,
} from "../auth/index.js";
export type {
  AuthConfig,
  AuthenticatedConfig,
  EnvironmentAuthConfig,
} from "../auth/index.js";

// Branding
export { cliStrings } from "../branding/cliStrings.js";

// Environment
export {
  ricaCliEnv,
  detectRicaEnvironment,
  getRicaControlPlaneEnv,
  ensureRicaHome,
} from "../config/cliEnv.js";

// Config bridge
export {
  createCliProfileLoader,
  loadRicaDynamicConfig,
} from "../config/cliConfigBridge.js";
