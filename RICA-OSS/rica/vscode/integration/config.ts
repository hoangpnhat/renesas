/**
 * RICA VS Code Config Integration Layer
 *
 * This module provides the config boundary between upstream VS Code extension
 * and RICA-specific configuration (environment URLs, client IDs, etc).
 *
 * Per RICA_REPO_STANDARD.md:
 * - Config boundary: all config goes through rica/adapters/config/
 * - Extension boundary: mapping layer between upstream and RICA
 */

import {
  IdeSettings,
  ControlPlaneEnv,
  getControlPlaneEnv as getRicaControlPlaneEnv,
} from "../../adapters/config/env";

/**
 * Get RICA control plane environment configuration
 * This wraps rica/adapters/config/env.ts and provides it to the extension
 */
export async function getRicaEnvironmentConfig(
  ideSettingsPromise: Promise<IdeSettings>,
): Promise<ControlPlaneEnv> {
  return await getRicaControlPlaneEnv(ideSettingsPromise);
}
