#!/usr/bin/env node

/**
 * RICA VSIX Build Automation Script
 *
 * Complete end-to-end build process:
 * 1. Generate manifest (apply RICA branding)
 * 2. Build GUI assets
 * 3. TypeScript type check
 * 4. Prepackage (npm install, copy binaries — clears out/)
 * 5. Build extension (esbuild into out/)
 * 6. Rebrand URLs (Continue → RICA)
 * 7. Validate branding
 * 8. Package VSIX (vsce)
 *
 * Usage:
 *   node rica/scripts/build-vsix.js [--pre-release] [--target <platform>] [--skip-validation]
 *
 * Examples:
 *   node rica/scripts/build-vsix.js                    # Standard build
 *   node rica/scripts/build-vsix.js --pre-release      # Pre-release version
 *   node rica/scripts/build-vsix.js --target win32-x64 # Platform-specific
 *
 * Platforms:
 *   win32-x64, win32-arm64, linux-x64, linux-arm64,
 *   darwin-x64, darwin-arm64, alpine-x64, alpine-arm64
 */

const { execSync } = require("child_process");
const fs = require("fs");
const path = require("path");

const ROOT_DIR = path.join(__dirname, "../..");
const VSCODE_DIR = path.join(ROOT_DIR, "extensions/vscode");
const GUI_DIR = path.join(ROOT_DIR, "gui");

// Parse arguments
const args = process.argv.slice(2);
const isPreRelease = args.includes("--pre-release");
const skipValidation = args.includes("--skip-validation");
let target = null;

const targetIndex = args.indexOf("--target");
if (targetIndex !== -1 && args[targetIndex + 1]) {
  target = args[targetIndex + 1];
}

// Utility: Run command with live output
function run(command, cwd = ROOT_DIR, description = "") {
  if (description) {
    console.log(`\n${"=".repeat(60)}`);
    console.log(`▶ ${description}`);
    console.log(`${"=".repeat(60)}`);
  }

  console.log(`💻 Running: ${command}`);
  console.log(`📂 Working dir: ${cwd}\n`);

  try {
    execSync(command, {
      cwd,
      stdio: "inherit",
      shell: true,
    });
    console.log(`\n✅ ${description || "Command"} completed successfully\n`);
    return true;
  } catch (error) {
    console.error(`\n❌ ${description || "Command"} failed!`);
    console.error(`Error: ${error.message}\n`);
    process.exit(1);
  }
}

// Utility: Check if file exists
function fileExists(filePath) {
  return fs.existsSync(filePath);
}

// Main build process
async function buildVSIX() {
  const startTime = Date.now();

  console.log(`
╔═══════════════════════════════════════════════════════════╗
║                                                           ║
║          RICA VSCode Extension Build Script              ║
║                                                           ║
╚═══════════════════════════════════════════════════════════╝
`);

  console.log("📦 Build Configuration:");
  console.log(`   - Pre-release: ${isPreRelease ? "YES" : "NO"}`);
  console.log(`   - Target platform: ${target || "universal"}`);
  console.log(`   - Skip validation: ${skipValidation ? "YES" : "NO"}`);
  console.log("");

  // ===== Step 1: Generate Manifest =====
  run(
    "node rica/scripts/generate-manifest.js",
    ROOT_DIR,
    "Step 1/8: Generate RICA Manifest",
  );

  // Verify manifest was generated correctly
  const manifestPath = path.join(VSCODE_DIR, "package.json");
  if (!fileExists(manifestPath)) {
    console.error("❌ Manifest generation failed: package.json not found");
    process.exit(1);
  }

  const manifest = JSON.parse(fs.readFileSync(manifestPath, "utf8"));
  if (manifest.name !== "rica") {
    console.error(
      `❌ Manifest error: Expected name 'rica', got '${manifest.name}'`,
    );
    process.exit(1);
  }

  console.log(
    `✅ Manifest verified: ${manifest.displayName} v${manifest.version}`,
  );

  // ===== Step 2: Build GUI =====
  run("npm run build", GUI_DIR, "Step 2/8: Build GUI Assets");

  // Verify GUI build output
  const guiDistPath = path.join(GUI_DIR, "dist");
  if (!fileExists(guiDistPath)) {
    console.error("❌ GUI build failed: dist/ directory not found");
    process.exit(1);
  }

  // ===== Step 3: TypeScript Check =====
  if (!skipValidation) {
    run(
      "npm run tsc:check",
      VSCODE_DIR,
      "Step 3/8: TypeScript Type Check (tsc --noEmit)",
    );
  } else {
    console.log(
      "\n⚠️  Step 3/8: TypeScript check SKIPPED (--skip-validation)\n",
    );
  }

  // ===== Step 4: Prepackage (npm install + copy binaries) =====
  // This step clears out/ and copies runtime dependencies (onnxruntime, ripgrep, lancedb, etc.)
  let prepackageCommand = "node scripts/prepackage.js";
  if (target) {
    prepackageCommand += ` --target ${target}`;
  }
  run(
    prepackageCommand,
    VSCODE_DIR,
    "Step 4/8: Prepackage (install & copy binaries)",
  );

  // ===== Step 5: Build Extension (esbuild) =====
  // Must run AFTER prepackage because prepackage clears out/
  run(
    "npm run esbuild-base -- --minify",
    VSCODE_DIR,
    "Step 5/8: Build VSCode Extension (esbuild --minify)",
  );

  // Verify extension build output
  const extensionOutPath = path.join(VSCODE_DIR, "out/extension.js");
  if (!fileExists(extensionOutPath)) {
    console.error("❌ Extension build failed: out/extension.js not found");
    process.exit(1);
  }

  // ===== Step 6: Rebrand URLs =====
  run(
    "node rica/scripts/rebrand-urls.js",
    ROOT_DIR,
    "Step 6/8: Rebrand URLs (Continue → RICA)",
  );

  // ===== Step 7: Validate Branding =====
  if (!skipValidation) {
    run(
      "node rica/scripts/check-branding.js",
      ROOT_DIR,
      "Step 7/8: Validate RICA Branding",
    );
  } else {
    console.log(
      "\n⚠️  Step 7/8: Branding validation SKIPPED (--skip-validation)\n",
    );
  }

  // ===== Step 8: Package VSIX (vsce only, no prepackage) =====
  // Disable vscode:prepublish to prevent esbuild from running AGAIN
  // (Steps 5+6 already built and rebranded; prepublish would overwrite rebranded output)
  const currentManifest = JSON.parse(fs.readFileSync(manifestPath, "utf8"));
  if (currentManifest.scripts && currentManifest.scripts["vscode:prepublish"]) {
    delete currentManifest.scripts["vscode:prepublish"];
    fs.writeFileSync(
      manifestPath,
      JSON.stringify(currentManifest, null, 2) + "\n",
      "utf8",
    );
    console.log("  Disabled vscode:prepublish (already built in Step 5)");
  }

  let vsceCommand = `npx @vscode/vsce package --out ./build --no-dependencies --baseContentUrl ${manifest.repository.url} --baseImagesUrl ${manifest.repository.url}/-/raw/main`;
  if (isPreRelease) {
    vsceCommand += " --pre-release";
  }
  if (target) {
    vsceCommand += ` --target ${target}`;
  }

  // Brief pause to let file system settle after rebrand writes (Windows Defender scan, etc.)
  console.log("  Waiting for file system to settle...");
  execSync("ping -n 3 127.0.0.1 >nul 2>&1 || sleep 2", {
    shell: true,
    stdio: "ignore",
  });

  // Retry vsce up to 3 times (yazl stream errors are transient on Windows with large extensions)
  const MAX_RETRIES = 3;
  for (let attempt = 1; attempt <= MAX_RETRIES; attempt++) {
    try {
      console.log(`\n${"=".repeat(60)}`);
      console.log(
        `▶ Step 8/8: Package VSIX${attempt > 1 ? ` (attempt ${attempt}/${MAX_RETRIES})` : ""}`,
      );
      console.log(`${"=".repeat(60)}`);
      console.log(`💻 Running: ${vsceCommand}`);
      console.log(`📂 Working dir: ${VSCODE_DIR}\n`);

      execSync(vsceCommand, { cwd: VSCODE_DIR, stdio: "inherit", shell: true });
      console.log(`\n✅ Step 8/8: Package VSIX completed successfully\n`);
      break;
    } catch (error) {
      if (attempt < MAX_RETRIES) {
        console.warn(`\n⚠️  Attempt ${attempt} failed. Retrying in 5s...`);
        execSync("ping -n 6 127.0.0.1 >nul 2>&1 || sleep 5", {
          shell: true,
          stdio: "ignore",
        });
      } else {
        console.error(`\n❌ Step 8/8: Package VSIX failed!`);
        console.error(`Error: ${error.message}\n`);
        process.exit(1);
      }
    }
  }

  // ===== Success Summary =====
  const duration = ((Date.now() - startTime) / 1000).toFixed(1);

  console.log(`
╔═══════════════════════════════════════════════════════════╗
║                                                           ║
║              ✅ BUILD SUCCESSFUL! ✅                      ║
║                                                           ║
╚═══════════════════════════════════════════════════════════╝
`);

  console.log("📦 Build artifacts:");

  // List VSIX files in build directory
  const buildDir = path.join(VSCODE_DIR, "build");
  if (fileExists(buildDir)) {
    const files = fs.readdirSync(buildDir).filter((f) => f.endsWith(".vsix"));
    files.forEach((file) => {
      const filePath = path.join(buildDir, file);
      const stats = fs.statSync(filePath);
      const sizeMB = (stats.size / 1024 / 1024).toFixed(2);
      console.log(`   ✓ ${file} (${sizeMB} MB)`);
    });

    if (files.length === 0) {
      console.log("   ⚠️  No VSIX files found in build/ directory");
    }
  } else {
    console.log("   ⚠️  build/ directory not found");
  }

  console.log("");
  console.log(`⏱️  Build time: ${duration}s`);
  console.log("");
  console.log("🚀 Next steps:");
  console.log(
    "   1. Install: code --install-extension extensions/vscode/build/rica-*.vsix",
  );
  console.log("   2. Test: Open VSCode and verify RICA extension loads");
  console.log(
    "   3. Deploy: Upload VSIX to internal marketplace/GitLab releases",
  );
  console.log("");
}

// Restore package.json to upstream state (prevents accidental commit of branded manifest)
function restorePackageJson() {
  const packageJsonPath = "extensions/vscode/package.json";
  try {
    execSync(`git checkout -- ${packageJsonPath}`, {
      cwd: ROOT_DIR,
      stdio: "pipe",
    });
    console.log(`🔄 Restored ${packageJsonPath} to upstream state`);
  } catch (e) {
    console.warn(
      `⚠️  Could not restore ${packageJsonPath}: ${e.message}\n` +
        `   Please manually run: git checkout -- ${packageJsonPath}`,
    );
  }
}

// Run build
buildVSIX()
  .then(() => {
    restorePackageJson();
  })
  .catch((error) => {
    restorePackageJson();
    console.error("\n❌ Build failed with error:");
    console.error(error);
    process.exit(1);
  });
