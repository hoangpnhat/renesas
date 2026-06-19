/**
 * RICA CLI ASCII Art Shim
 *
 * Drop-in replacement for extensions/cli/src/asciiArt.ts.
 * Replaces Continue branding with RICA ASCII art.
 */

import chalk from "chalk";

// @ts-ignore - JSON import resolved by esbuild bundler
import brandConfig from "../../branding/brand.json";

const d = chalk.dim;
const b = chalk.blueBright;

const RICA_VERSION = brandConfig.product.version;

export const CONTINUE_ASCII_ART = `
${b(`  ██████╗  ██╗ ██████╗  █████╗ `)}
${b(`  ██╔══██╗ ██║██╔════╝ ██╔══██╗`)}
${b(`  ██████╔╝ ██║██║      ███████║`)}
${b(`  ██╔══██╗ ██║██║      ██╔══██║`)}
${b(`  ██║  ██║ ██║╚██████╗ ██║  ██║`)}
${b(`  ╚═╝  ╚═╝ ╚═╝ ╚═════╝ ╚═╝  ╚═╝`)}
                          ${d("v" + RICA_VERSION)}`;

const RICA_SHORT = `
${b(`  ██████╗  ██╗ ██████╗  █████╗ `)}
${b(`  ██╔══██╗ ██║██╔════╝ ██╔══██╗`)}
${b(`  ██████╔╝ ██║██║      ███████║`)}
${b(`  ██╔══██╗ ██║██║      ██╔══██║`)}
${b(`  ██║  ██║ ██║╚██████╗ ██║  ██║`)}
${b(`  ╚═╝  ╚═╝ ╚═╝ ╚═════╝ ╚═╝  ╚═╝`)}
  ${d("v" + RICA_VERSION)}`;

const MIN_WIDTH_FOR_ASCII_ART = 40;

export function getDisplayableAsciiArt(): string {
  const terminalWidth = process.stdout.columns || 80;
  if (terminalWidth >= MIN_WIDTH_FOR_ASCII_ART) {
    return CONTINUE_ASCII_ART;
  }
  return RICA_SHORT;
}

export const CONTINUE_LOGO_ASCII_ART = CONTINUE_ASCII_ART;
