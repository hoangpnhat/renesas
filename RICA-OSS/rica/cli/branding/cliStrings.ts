/**
 * RICA CLI Branding Strings
 *
 * Centralizes all user-facing strings for the RICA CLI.
 * Imported by minimal patches in extensions/cli/ to replace upstream "Continue" branding.
 *
 * Per REPO_STANDARD.md Section 6: Branding governance
 * - Source of truth: rica/branding/brand.json
 * - User-facing strings must present RICA branding
 */

// @ts-ignore - JSON import resolved by esbuild bundler
import brandConfig from "../../branding/brand.json";

export const cliStrings = {
  productName: brandConfig.product.name,
  fullName: brandConfig.product.fullName,
  cliName: "rica",
  binaryName: "rica",
  description: `${brandConfig.product.name} CLI - ${brandConfig.product.fullName}. Starts an interactive session by default, use -p/--print for non-interactive output.`,

  // Auth messages
  loginMessage: `Signing in with ${brandConfig.product.name}...`,
  loginSuccess: "Successfully logged in!",
  startingCli: `Starting ${brandConfig.product.name} CLI...`,
  logoutEnvMessage:
    "Using API key from environment variables, nothing to log out",
  envKeyDetected: "Using API key from environment variables",

  // Onboarding
  onboardingPrompt: "How do you want to get started?",
  onboardingOption1: `Log in with ${brandConfig.product.name}`,

  // User-Agent
  userAgent: (version: string) => `RICA-CLI/${version}`,

  // URLs
  homepage: brandConfig.urls.homepage,
  repository: brandConfig.urls.repository,
  issues: brandConfig.urls.issues,

  // Environment variable names
  envVars: {
    apiKey: "RICA_API_KEY",
    apiBase: "RICA_API_BASE",
    appUrl: "RICA_APP_URL",
    globalDir: "RICA_GLOBAL_DIR",
  },
};
