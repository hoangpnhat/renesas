#!/usr/bin/env node

/**
 * RICA Post-Build URL Rebranding
 *
 * Replaces all Continue URLs in compiled JS bundles with RICA equivalents.
 * Runs AFTER build (post-compile), does NOT modify source files.
 *
 * Usage:
 *   node rica/scripts/rebrand-urls.js [--dry-run] [--verbose]
 *
 * Per REPO_STANDARD.md §6.3:
 *   "user-facing strings should not regress from RICA back to Continue"
 */

const fs = require("fs");
const path = require("path");

const ROOT_DIR = path.join(__dirname, "../..");
const BRAND_CONFIG_PATH = path.join(__dirname, "../branding/brand.json");

// Parse CLI flags
const args = process.argv.slice(2);
const DRY_RUN = args.includes("--dry-run");
const VERBOSE = args.includes("--verbose");

// Load brand config for target URLs
let brandConfig;
try {
  brandConfig = JSON.parse(fs.readFileSync(BRAND_CONFIG_PATH, "utf8"));
} catch (e) {
  console.error(`Failed to load brand config: ${e.message}`);
  process.exit(1);
}

// Target URLs from brand.json
const RICA_DOCS =
  brandConfig.urls.docs || "https://rica.global.renesas.com/docs";
const RICA_HOME =
  brandConfig.urls.homepage || "https://rica.global.renesas.com";
const RICA_ISSUES =
  brandConfig.urls.issues ||
  "https://gitlab.global.renesas.com/ids_ai/rica-oss/-/issues";
const RICA_SUPPORT =
  brandConfig.urls.support || "https://rica.global.renesas.com/support";

// URL replacement rules (order matters — more specific patterns first)
const REPLACEMENTS = [
  // Documentation URLs
  {
    from: "https://docs.continue.dev/troubleshooting",
    to: `${RICA_DOCS}/troubleshooting`,
  },
  {
    from: "https://docs.continue.dev/yaml-migration",
    to: `${RICA_DOCS}/yaml-migration`,
  },
  {
    from: "https://docs.continue.dev/reference/config",
    to: `${RICA_DOCS}/reference/config`,
  },
  {
    from: "https://docs.continue.dev/reference/Model%20Providers/openai",
    to: `${RICA_DOCS}/reference/model-providers/openai`,
  },
  {
    from: "https://docs.continue.dev/reference/Model%20Providers/",
    to: `${RICA_DOCS}/reference/model-providers/`,
  },
  { from: "https://docs.continue.dev/reference", to: `${RICA_DOCS}/reference` },
  {
    from: "https://docs.continue.dev/ide-extensions/",
    to: `${RICA_DOCS}/ide-extensions/`,
  },
  {
    from: "https://docs.continue.dev/customize/",
    to: `${RICA_DOCS}/customize/`,
  },
  {
    from: "https://docs.continue.dev/customization/",
    to: `${RICA_DOCS}/customization/`,
  },
  { from: "https://docs.continue.dev/features/", to: `${RICA_DOCS}/features/` },
  { from: "https://docs.continue.dev/guides/", to: `${RICA_DOCS}/guides/` },
  {
    from: "https://docs.continue.dev/walkthroughs/",
    to: `${RICA_DOCS}/walkthroughs/`,
  },
  { from: "https://docs.continue.dev/setup/", to: `${RICA_DOCS}/setup/` },
  { from: "https://docs.continue.dev/hub/", to: `${RICA_DOCS}/hub/` },
  { from: "https://docs.continue.dev/telemetry", to: `${RICA_DOCS}/telemetry` },
  { from: "https://docs.continue.dev", to: RICA_DOCS },

  // GitHub Issues / Discussions → RICA GitLab
  {
    from: "https://github.com/continuedev/continue/issues/new/choose",
    to: `${RICA_ISSUES}/new`,
  },
  { from: "https://github.com/continuedev/continue/issues", to: RICA_ISSUES },
  {
    from: "https://github.com/continuedev/continue/discussions",
    to: RICA_SUPPORT,
  },
  { from: "https://github.com/continuedev/continue", to: RICA_HOME },

  // continue.dev app URLs
  { from: "https://continue.dev/settings", to: `${RICA_HOME}/settings` },
  { from: "https://continue.dev/docker/", to: `${RICA_HOME}/mcp/` },
  { from: "https://continue.dev/anthropic/", to: `${RICA_HOME}/mcp/` },
  {
    from: "https://continue.dev/settings/secrets",
    to: `${RICA_HOME}/settings`,
  },

  // Catch-all: any remaining continue.dev references
  { from: "https://continue.dev/", to: `${RICA_HOME}/` },
  { from: "https://continue.dev", to: RICA_HOME },
];

// String replacements for user-facing text (applied to GUI bundle only)
// These handle both template literal and concatenation forms from bundler output
const STRING_REPLACEMENTS = [
  // Tool call status: "Continue tried to..." → "RICA tried to..."
  // Template literal form (Vite preserves backtick templates in dev, but may compile in prod)
  { from: "`Continue ${", to: "`RICA ${" },
  // Concatenation form from minifier: "Continue "+intro
  { from: '"Continue "+', to: '"RICA "+' },
  { from: "'Continue '+", to: "'RICA '+" },

  // CLI Install Banner: rebrand or neutralize
  { from: "Try out the Continue CLI", to: "Try out the RICA CLI" },
  { from: "npm i -g @continuedev/cli", to: "rica-cli (coming soon)" },
  {
    from: "deploy Continuous AI workflows",
    to: "deploy RICA AI workflows",
  },
  { from: "the Continue CLI", to: "the RICA CLI" },
];

// Folder/path replacements: .continue → .rica (applied to extension + GUI bundles)
// Safe because: ".continue" (quoted) won't match .continueignore or .continuerc.json
// and .continue/ (with slash) won't match .continueignore (no trailing slash on filenames)
const FOLDER_REPLACEMENTS = [
  // Path segments with trailing slash (workspace/.continue/mcpServers, .continue/rules, etc.)
  { from: ".continue/", to: ".rica/" },
  // Standalone quoted string: path.join(homedir(), ".continue"), includes(".continue")
  { from: '".continue"', to: '".rica"' },
  // User-facing instruction strings: ~/.continue
  { from: "~/.continue", to: "~/.rica" },
];

// Files to process (relative to ROOT_DIR)
const TARGET_FILES = [
  "extensions/vscode/package.json",
  "extensions/vscode/gui/assets/index.js",
  "extensions/vscode/out/extension.js",
];

// Stats
let totalReplacements = 0;
let filesProcessed = 0;
let filesModified = 0;

console.log("=== RICA Post-Build URL Rebranding ===\n");
if (DRY_RUN) console.log("[DRY RUN] No files will be modified.\n");

for (const relPath of TARGET_FILES) {
  const filePath = path.join(ROOT_DIR, relPath);

  if (!fs.existsSync(filePath)) {
    console.log(`  SKIP ${relPath} (not found — build may not have run)`);
    continue;
  }

  filesProcessed++;
  let content = fs.readFileSync(filePath, "utf8");
  let fileReplacements = 0;

  // Apply URL replacements
  for (const rule of REPLACEMENTS) {
    const count = content.split(rule.from).length - 1;
    if (count > 0) {
      content = content.split(rule.from).join(rule.to);
      fileReplacements += count;
      if (VERBOSE) {
        console.log(`    ${rule.from}`);
        console.log(`      -> ${rule.to} (${count}x)`);
      }
    }
  }

  // Apply string replacements (GUI bundle only)
  if (relPath.includes("gui/assets/")) {
    for (const rule of STRING_REPLACEMENTS) {
      const count = content.split(rule.from).length - 1;
      if (count > 0) {
        content = content.split(rule.from).join(rule.to);
        fileReplacements += count;
        if (VERBOSE) {
          console.log(`    [STRING] ${rule.from}`);
          console.log(`      -> ${rule.to} (${count}x)`);
        }
      }
    }
  }

  // Apply folder path replacements (extension + GUI bundles)
  if (relPath.includes("gui/assets/") || relPath.includes("out/extension.js")) {
    for (const rule of FOLDER_REPLACEMENTS) {
      const count = content.split(rule.from).length - 1;
      if (count > 0) {
        content = content.split(rule.from).join(rule.to);
        fileReplacements += count;
        if (VERBOSE) {
          console.log(`    [FOLDER] ${rule.from}`);
          console.log(`      -> ${rule.to} (${count}x)`);
        }
      }
    }
  }

  if (fileReplacements > 0) {
    filesModified++;
    totalReplacements += fileReplacements;

    if (!DRY_RUN) {
      fs.writeFileSync(filePath, content, "utf8");
    }
    console.log(
      `  ${DRY_RUN ? "WOULD MODIFY" : "MODIFIED"} ${relPath} (${fileReplacements} replacements)`,
    );
  } else {
    console.log(`  OK ${relPath} (no Continue URLs found)`);
  }
}

// Summary
console.log("\n--- Summary ---");
console.log(`  Files scanned:  ${filesProcessed}`);
console.log(`  Files modified: ${filesModified}`);
console.log(`  Total URLs replaced: ${totalReplacements}`);

if (DRY_RUN) {
  console.log("\n[DRY RUN] Re-run without --dry-run to apply changes.");
}

// Validation: check if any continue.dev URLs remain
if (!DRY_RUN && filesModified > 0) {
  console.log("\n--- Validation ---");
  let remainingCount = 0;

  for (const relPath of TARGET_FILES) {
    const filePath = path.join(ROOT_DIR, relPath);
    if (!fs.existsSync(filePath)) continue;

    const content = fs.readFileSync(filePath, "utf8");
    // Match continue.dev URLs but exclude @continuedev package names and import paths
    const matches = content.match(
      /https?:\/\/(docs\.)?continue\.dev[^"'\s)}\]\\]*/g,
    );
    if (matches) {
      remainingCount += matches.length;
      if (VERBOSE) {
        console.log(`  REMAINING in ${relPath}:`);
        const unique = [...new Set(matches)];
        unique.forEach((url) => console.log(`    ${url}`));
      } else {
        console.log(
          `  WARNING: ${matches.length} Continue URLs still remain in ${relPath}`,
        );
      }
    }
  }

  if (remainingCount === 0) {
    console.log("  All Continue URLs successfully replaced.");
  } else {
    console.log(
      `\n  ${remainingCount} URLs still remain. Run with --verbose to see details.`,
    );
    console.log("  Add missing patterns to REPLACEMENTS array in this script.");
  }
}

console.log("\nDone.");
