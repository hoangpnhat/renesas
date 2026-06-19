/**
 * RICA CLI Environment Shim
 *
 * Drop-in replacement for extensions/cli/src/env.ts.
 * Injected at build time via esbuild onResolve plugin.
 *
 * Points all environment values to RICA infrastructure instead of Continue.
 */

import { ricaCliEnv } from "../config/cliEnv.js";

export const env = {
  apiBase: ricaCliEnv.apiBase,
  workOsClientId: "",
  appUrl: ricaCliEnv.appUrl,
  continueHome: ricaCliEnv.ricaHome,
};
