/**
 * RICA CLI Onboarding Shim
 *
 * Drop-in replacement for extensions/cli/src/onboarding.ts.
 * RICA onboarding = API key login only (no Anthropic key option, no WorkOS device flow).
 */

import * as fs from "fs";
import * as path from "path";

import chalk from "chalk";

import { login, isAuthenticated, loadAuthConfig } from "../auth/index.js";
import { cliStrings } from "../branding/cliStrings.js";
import { ricaCliEnv } from "../config/cliEnv.js";

import type { AuthConfig } from "../auth/index.js";

export async function checkHasAcceptableModel(
  _configPath: string,
): Promise<boolean> {
  // RICA models are loaded dynamically from backend — always acceptable if authenticated
  return isAuthenticated();
}

export async function createOrUpdateConfig(_apiKey: string): Promise<void> {
  // Not used in RICA — config comes from backend
}

export async function runOnboardingFlow(
  configPath: string | undefined,
): Promise<boolean> {
  if (configPath !== undefined) {
    return false;
  }

  if (await isAuthenticated()) {
    return false;
  }

  console.log(chalk.yellow(`Welcome to ${cliStrings.productName}!`));
  console.log(chalk.white(`\n1. Log in with ${cliStrings.productName}`));

  try {
    await login();
    return true;
  } catch (error: any) {
    console.error(chalk.red(`Login failed: ${error.message}`));
    return false;
  }
}

export async function isFirstTime(): Promise<boolean> {
  const flagPath = path.join(ricaCliEnv.ricaHome, ".onboarding_complete");
  return !fs.existsSync(flagPath);
}

export async function markOnboardingComplete(): Promise<void> {
  const flagPath = path.join(ricaCliEnv.ricaHome, ".onboarding_complete");
  const flagDir = path.dirname(flagPath);

  if (!fs.existsSync(flagDir)) {
    fs.mkdirSync(flagDir, { recursive: true });
  }

  fs.writeFileSync(flagPath, new Date().toISOString());
}

export async function initializeWithOnboarding(
  _authConfig: AuthConfig,
  configPath: string | undefined,
) {
  const firstTime = await isFirstTime();
  if (!firstTime) return;

  const wasOnboarded = await runOnboardingFlow(configPath);
  if (wasOnboarded) {
    await markOnboardingComplete();
  }
}
