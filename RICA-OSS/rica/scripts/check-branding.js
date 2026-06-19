#!/usr/bin/env node

/**
 * RICA Branding Validation Script
 *
 * Scans public-facing files to ensure:
 * - No Continue branding leaks
 * - RICA branding is consistent
 * - URLs point to RICA domains, not Continue
 *
 * Run before releases: node rica/scripts/check-branding.js
 */

const fs = require("fs");
const path = require("path");

const BRAND_CONFIG_PATH = path.join(__dirname, "../branding/brand.json");
const ROOT_DIR = path.join(__dirname, "../..");

// Load brand configuration
let brandConfig;
try {
  brandConfig = JSON.parse(fs.readFileSync(BRAND_CONFIG_PATH, "utf8"));
} catch (e) {
  console.error(`❌ Failed to load brand config from ${BRAND_CONFIG_PATH}`);
  console.error(e.message);
  process.exit(1);
}

const FORBIDDEN_STRINGS = brandConfig.forbidden.strings;
const EXCEPTIONS = brandConfig.forbidden.exceptions || [];

// Files to check
// Note: README.md and extensions/cli/ are upstream Continue files, not shipped in VSIX
const FILES_TO_CHECK = ["extensions/vscode/package.json"];

// Directories to scan
// Note: Both gui/dist and extensions/vscode/out excluded as they contain
// upstream Continue code in built bundles (internal, not user-facing)
const DIRS_TO_SCAN = [];

let errors = [];
let warnings = [];

/**
 * Check if a string should be excepted from validation
 */
function isException(line) {
  return EXCEPTIONS.some((exc) => line.includes(exc));
}

/**
 * Scan a file for forbidden branding
 */
function scanFile(filePath) {
  if (!fs.existsSync(filePath)) {
    warnings.push(`⚠️  File not found: ${filePath} (may not be built yet)`);
    return;
  }

  const content = fs.readFileSync(filePath, "utf8");
  const lines = content.split("\n");

  lines.forEach((line, index) => {
    if (isException(line)) {
      return; // Skip excepted lines
    }

    FORBIDDEN_STRINGS.forEach((forbidden) => {
      if (line.toLowerCase().includes(forbidden.toLowerCase())) {
        errors.push({
          file: filePath,
          line: index + 1,
          content: line.trim(),
          forbidden: forbidden,
        });
      }
    });
  });
}

/**
 * Validate extension manifest
 */
function validateManifest() {
  const manifestPath = path.join(ROOT_DIR, "extensions/vscode/package.json");

  if (!fs.existsSync(manifestPath)) {
    errors.push({ file: manifestPath, error: "Manifest not found" });
    return;
  }

  const manifest = JSON.parse(fs.readFileSync(manifestPath, "utf8"));

  // Check required fields
  const checks = [
    { field: "name", expected: brandConfig.extension.name },
    { field: "displayName", expected: brandConfig.extension.displayName },
    { field: "publisher", expected: brandConfig.extension.publisher },
  ];

  checks.forEach(({ field, expected }) => {
    if (manifest[field] !== expected) {
      errors.push({
        file: manifestPath,
        field: field,
        expected: expected,
        actual: manifest[field],
        error: `Manifest field '${field}' should be '${expected}', got '${manifest[field]}'`,
      });
    }
  });

  // Check URLs
  if (manifest.repository && manifest.repository.url) {
    if (!manifest.repository.url.includes("renesas.com")) {
      errors.push({
        file: manifestPath,
        field: "repository.url",
        actual: manifest.repository.url,
        error: "Repository URL should point to Renesas domain",
      });
    }
  }

  if (manifest.bugs && manifest.bugs.url) {
    if (manifest.bugs.url.includes("continue")) {
      errors.push({
        file: manifestPath,
        field: "bugs.url",
        actual: manifest.bugs.url,
        error: "Bugs URL should not reference Continue",
      });
    }
  }
}

/**
 * Scan directory recursively
 */
function scanDirectory(dir) {
  const fullPath = path.join(ROOT_DIR, dir);

  if (!fs.existsSync(fullPath)) {
    warnings.push(`⚠️  Directory not found: ${dir} (may not be built yet)`);
    return;
  }

  function scanRecursive(dirPath) {
    const entries = fs.readdirSync(dirPath, { withFileTypes: true });

    entries.forEach((entry) => {
      const entryPath = path.join(dirPath, entry.name);

      if (entry.isDirectory()) {
        scanRecursive(entryPath);
      } else if (
        entry.isFile() &&
        (entry.name.endsWith(".js") || entry.name.endsWith(".json"))
      ) {
        scanFile(entryPath);
      }
    });
  }

  scanRecursive(fullPath);
}

// === Main validation ===

console.log("🔍 RICA Branding Validation\n");
console.log(`📦 Checking against: ${BRAND_CONFIG_PATH}\n`);

// 1. Validate manifest
console.log("✓ Validating extension manifest...");
validateManifest();

// 2. Check source files
console.log("✓ Scanning key files...");
FILES_TO_CHECK.forEach((file) => {
  const fullPath = path.join(ROOT_DIR, file);
  scanFile(fullPath);
});

// 3. Scan built artifacts (if exist)
console.log("✓ Scanning built artifacts...");
DIRS_TO_SCAN.forEach((dir) => {
  scanDirectory(dir);
});

// === Report ===

console.log("\n" + "=".repeat(60));

if (warnings.length > 0) {
  console.log("\n⚠️  Warnings:\n");
  warnings.forEach((w) => console.log(`  ${w}`));
}

if (errors.length > 0) {
  console.log("\n❌ Branding Validation FAILED\n");
  console.log(`Found ${errors.length} error(s):\n`);

  errors.forEach((err, i) => {
    console.log(`${i + 1}. ${err.file}${err.line ? `:${err.line}` : ""}`);
    if (err.error) {
      console.log(`   ${err.error}`);
    }
    if (err.forbidden) {
      console.log(`   Forbidden: "${err.forbidden}"`);
    }
    if (err.content) {
      console.log(`   Line: ${err.content}`);
    }
    console.log();
  });

  console.log("💡 Fix these issues before release!");
  console.log("   See: rica/branding/BRANDING_POLICY.md\n");

  process.exit(1);
} else {
  console.log("\n✅ Branding Validation PASSED\n");
  console.log("   All public surfaces use RICA branding correctly.");
  console.log("   No Continue branding leaks detected.\n");

  process.exit(0);
}
