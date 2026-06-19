#!/usr/bin/env node

/**
 * RICA CLI Manifest Generator
 *
 * Generates extensions/cli/package.json with RICA branding overrides.
 * Analogous to generate-manifest.js for the VS Code extension.
 *
 * Takes:
 *   - brand.json (single source of truth)
 *   - cli.manifest.overrides.json (CLI-specific overrides)
 *   - extensions/cli/package.json (upstream base)
 *
 * Usage:
 *   node rica/scripts/generate-cli-manifest.js [--dry-run]
 */

const fs = require("fs");
const path = require("path");

const ROOT_DIR = path.join(__dirname, "../..");
const BRAND_CONFIG_PATH = path.join(__dirname, "../branding/brand.json");
const OVERRIDES_PATH = path.join(
  __dirname,
  "../branding/cli.manifest.overrides.json",
);
const PACKAGE_JSON_PATH = path.join(ROOT_DIR, "extensions/cli/package.json");

const isDryRun = process.argv.includes("--dry-run");

// Load configurations
let brandConfig, overrides, baseManifest;

try {
  brandConfig = JSON.parse(fs.readFileSync(BRAND_CONFIG_PATH, "utf8"));
} catch (e) {
  console.error(`❌ Failed to load brand config: ${e.message}`);
  process.exit(1);
}

try {
  overrides = JSON.parse(fs.readFileSync(OVERRIDES_PATH, "utf8"));
} catch (e) {
  console.error(`❌ Failed to load CLI manifest overrides: ${e.message}`);
  process.exit(1);
}

try {
  baseManifest = JSON.parse(fs.readFileSync(PACKAGE_JSON_PATH, "utf8"));
} catch (e) {
  console.error(`❌ Failed to load base package.json: ${e.message}`);
  process.exit(1);
}

console.log("🔧 Generating CLI manifest for RICA...\n");

/**
 * Apply branding from brand.json
 */
function applyBranding(manifest) {
  manifest.version = brandConfig.product.version;
  return manifest;
}

/**
 * Deep merge objects (source overrides target)
 */
function deepMerge(target, source) {
  const output = Object.assign({}, target);

  if (isObject(target) && isObject(source)) {
    Object.keys(source).forEach((key) => {
      if (isObject(source[key])) {
        if (!(key in target)) {
          Object.assign(output, { [key]: source[key] });
        } else {
          output[key] = deepMerge(target[key], source[key]);
        }
      } else {
        Object.assign(output, { [key]: source[key] });
      }
    });
  }

  return output;
}

function isObject(item) {
  return item && typeof item === "object" && !Array.isArray(item);
}

// Step 1: Start with base manifest
let result = { ...baseManifest };

// Step 2: Apply brand.json values
result = applyBranding(result);

// Step 3: Deep merge CLI-specific overrides
result = deepMerge(result, overrides);

// Report changes
console.log("  Applied overrides:");
console.log(`    name: ${result.name}`);
console.log(`    version: ${result.version}`);
console.log(`    bin: ${JSON.stringify(result.bin)}`);
console.log(`    author: ${result.author}`);
console.log(`    homepage: ${result.homepage}`);
console.log(`    description: ${result.description.substring(0, 60)}...`);

if (isDryRun) {
  console.log("\n📋 Dry run — no files written.");
  console.log(JSON.stringify(result, null, 2));
} else {
  fs.writeFileSync(PACKAGE_JSON_PATH, JSON.stringify(result, null, 2) + "\n");
  console.log(`\n✅ Written to: ${PACKAGE_JSON_PATH}`);
}
