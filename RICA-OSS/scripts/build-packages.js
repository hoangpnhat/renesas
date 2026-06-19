const { spawn } = require("child_process");
const path = require("path");
const fs = require("fs");
const fsPromises = require("fs/promises");

const npmInstallCmd = process.env.CI === "true" ? "npm ci" : "npm install";

// Mirror of the SKIP_INSTALLS pattern in extensions/vscode/scripts/prepackage.js.
// On every F5 the `continue-packages:build` task calls into here, which used to
// re-run `npm install` for all 7 sub-packages even when nothing in their deps
// had changed. With node_modules already populated each install is a no-op
// network round-trip, but the round-trip itself takes ~25-35s. Setting
// SKIP_INSTALLS=true (set by tasks.json for the build-packages task; also
// already set by the launch.json prepackage path) skips the install step
// entirely and goes straight to `npm run build`. Flip to false / unset whenever
// you bump a package.json or pull a package-lock.json change.
const skipInstalls = process.env.SKIP_INSTALLS === "true";

function runCommand(command, cwd, packageName) {
  return new Promise((resolve, reject) => {
    console.log(`Starting ${packageName}: ${command}`);

    const [cmd, ...args] = command.split(" ");
    const child = spawn(cmd, args, {
      cwd,
      stdio: "pipe",
      shell: true,
    });

    let stdout = "";
    let stderr = "";

    child.stdout.on("data", (data) => {
      stdout += data.toString();
    });

    child.stderr.on("data", (data) => {
      stderr += data.toString();
    });

    child.on("close", (code) => {
      if (code === 0) {
        console.log(`✅ ${packageName}: ${command} completed successfully`);
        resolve({ packageName, command, stdout, stderr });
      } else {
        console.error(`❌ ${packageName}: ${command} failed with code ${code}`);
        console.error(`stderr: ${stderr}`);
        console.error(`stdout: ${stdout}`);
        reject(
          new Error(`${packageName} failed: ${command} (exit code ${code})`),
        );
      }
    });

    child.on("error", (error) => {
      console.error(`❌ ${packageName}: Failed to start ${command}:`, error);
      reject(error);
    });
  });
}

// Helper function to build a package (install + build)
async function buildPackage(packageName, cleanNodeModules = false) {
  const packagePath = path.join(__dirname, "..", "packages", packageName);

  if (!fs.existsSync(packagePath)) {
    throw new Error(`Package directory not found: ${packagePath}`);
  }

  if (cleanNodeModules) {
    const nodeModulesPath = path.join(packagePath, "node_modules");
    if (fs.existsSync(nodeModulesPath)) {
      console.log(`🧹 Cleaning node_modules for ${packageName}`);
      await fsPromises.rm(nodeModulesPath, { recursive: true, force: true });
    }
  }

  if (skipInstalls) {
    const nodeModulesPath = path.join(packagePath, "node_modules");
    if (fs.existsSync(nodeModulesPath)) {
      console.log(
        `⏭️  ${packageName} (install): skipped (SKIP_INSTALLS=true, node_modules present)`,
      );
    } else {
      // Safety net — if the dir is missing we MUST install, the build will
      // fail otherwise and produce a confusing "module not found" error.
      console.log(
        `⚠️  ${packageName} (install): SKIP_INSTALLS=true but node_modules is missing — installing anyway`,
      );
      await runCommand(npmInstallCmd, packagePath, `${packageName} (install)`);
    }
  } else {
    await runCommand(npmInstallCmd, packagePath, `${packageName} (install)`);
  }

  return runCommand("npm run build", packagePath, `${packageName} (build)`);
}

async function buildPackagesInParallel(packages, cleanNodeModules = false) {
  const buildPromises = packages.map((pkg) =>
    buildPackage(pkg, cleanNodeModules),
  );
  return Promise.all(buildPromises);
}

async function main() {
  try {
    console.log("🚀 Starting package builds...\n");

    // Phase 1: Build foundation packages (no local dependencies)
    await buildPackagesInParallel(["config-types", "terminal-security"]);

    // Phase 2: Build packages that depend on config-types
    await buildPackagesInParallel(["fetch", "config-yaml", "llm-info"]);

    // Phase 3: Build packages that depend on other local packages
    await buildPackagesInParallel(["openai-adapters", "continue-sdk"]);

    console.log("🎉 All packages built successfully!");
  } catch (error) {
    console.error("💥 Build failed:", error.message);
    process.exit(1);
  }
}

if (require.main === module) {
  main();
}
