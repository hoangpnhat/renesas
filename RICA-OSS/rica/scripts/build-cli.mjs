#!/usr/bin/env node

/**
 * RICA CLI Build Script
 *
 * Builds the RICA CLI by wrapping the upstream esbuild configuration with
 * an onResolve plugin that redirects critical modules to RICA shim files.
 *
 * This avoids patching 50+ files that import from workos.ts, env.ts, etc.
 * Instead, esbuild resolves those imports to RICA-specific implementations
 * at bundle time.
 *
 * Usage:
 *   node rica/scripts/build-cli.mjs [--no-minify]
 *
 * Output:
 *   extensions/cli/dist/index.js  (bundled CLI)
 *   extensions/cli/dist/rica.js   (wrapper with shebang)
 */

import { chmodSync, copyFileSync, writeFileSync, existsSync } from "fs";
import { createRequire } from "module";
import { dirname, resolve } from "path";
import { fileURLToPath } from "url";

const __dirname = dirname(fileURLToPath(import.meta.url));
const ROOT_DIR = resolve(__dirname, "../..");
const CLI_DIR = resolve(ROOT_DIR, "extensions/cli");

// Resolve esbuild from CLI's node_modules
const require = createRequire(resolve(CLI_DIR, "package.json"));
const esbuild = require("esbuild");

const args = process.argv.slice(2);
const noMinify = args.includes("--no-minify");

const RICA_CLI_DIR = resolve(ROOT_DIR, "rica/cli");

// Native modules that cannot be bundled
const external = [
  "@sentry/profiling-node",
  "fsevents",
  "./xhr-sync-worker.js",
];

console.log("🔧 Building RICA CLI with esbuild + shim plugin...\n");

/**
 * RICA Shim Plugin
 *
 * Intercepts module resolution for upstream CLI files and redirects
 * to RICA-specific shim implementations.
 */
const ricaShimPlugin = {
  name: "rica-shim",
  setup(build) {
    // Resolve any import path containing "rica/cli/" or "rica/branding/" to absolute paths
    build.onResolve({ filter: /rica\/cli\/|rica\/branding\/|rica\/adapters\/|rica\/utils\// }, (args) => {
      // Convert relative paths from extensions/cli/src/ into absolute paths
      if (args.path.includes("rica/")) {
        const ricaRelative = args.path.substring(args.path.indexOf("rica/"));
        return { path: resolve(ROOT_DIR, ricaRelative.replace(/\.js$/, ".ts")) };
      }
    });

    // Redirect auth/workos.ts → rica/cli/shim/workosShim.ts
    build.onResolve({ filter: /auth\/workos\.js$/ }, (args) => {
      if (args.resolveDir.includes("extensions/cli") ||
          args.resolveDir.includes("extensions\\cli")) {
        return { path: resolve(RICA_CLI_DIR, "shim/workosShim.ts") };
      }
    });

    // Redirect auth/workos-types.ts → rica/cli/shim/workosShim.ts (types re-exported)
    build.onResolve({ filter: /auth\/workos-types\.js$/ }, (args) => {
      if (args.resolveDir.includes("extensions/cli") ||
          args.resolveDir.includes("extensions\\cli")) {
        return { path: resolve(RICA_CLI_DIR, "shim/workosShim.ts") };
      }
    });

    // Redirect env.ts → rica/cli/shim/envShim.ts
    // Be specific: only match the CLI's own env.ts, not env.ts in other packages
    build.onResolve({ filter: /^\.\/env\.js$|^\.\.\/env\.js$/ }, (args) => {
      const resolveDir = args.resolveDir.replace(/\\/g, "/");
      if (resolveDir.includes("extensions/cli/src") &&
          !resolveDir.includes("node_modules")) {
        return { path: resolve(RICA_CLI_DIR, "shim/envShim.ts") };
      }
    });

    // Redirect asciiArt.ts → rica/cli/shim/asciiArtShim.ts
    build.onResolve({ filter: /asciiArt\.js$/ }, (args) => {
      if (args.resolveDir.includes("extensions/cli") ||
          args.resolveDir.includes("extensions\\cli")) {
        return { path: resolve(RICA_CLI_DIR, "shim/asciiArtShim.ts") };
      }
    });

    // Redirect onboarding.ts → rica/cli/shim/onboardingShim.ts
    build.onResolve({ filter: /\/onboarding\.js$|^\.\/onboarding\.js$/ }, (args) => {
      const resolveDir = args.resolveDir.replace(/\\/g, "/");
      if (resolveDir.includes("extensions/cli/src") &&
          !resolveDir.includes("node_modules")) {
        return { path: resolve(RICA_CLI_DIR, "shim/onboardingShim.ts") };
      }
    });

    // Redirect configLoader.ts → rica/cli/shim/configLoaderShim.ts
    build.onResolve({ filter: /\/configLoader\.js$|^\.\/configLoader\.js$/ }, (args) => {
      const resolveDir = args.resolveDir.replace(/\\/g, "/");
      if (resolveDir.includes("extensions/cli/src") &&
          !resolveDir.includes("node_modules")) {
        return { path: resolve(RICA_CLI_DIR, "shim/configLoaderShim.ts") };
      }
    });

    // Redirect version.ts → rica/cli/shim/versionShim.ts
    build.onResolve({ filter: /\/version\.js$|^\.\/version\.js$/ }, (args) => {
      const resolveDir = args.resolveDir.replace(/\\/g, "/");
      if (resolveDir.includes("extensions/cli/src") &&
          !resolveDir.includes("node_modules")) {
        return { path: resolve(RICA_CLI_DIR, "shim/versionShim.ts") };
      }
    });

    // Redirect config.ts → rica/cli/shim/configShim.ts (RICA proxy adapter)
    build.onResolve({ filter: /\/config\.js$|^\.\/config\.js$|^\.\.\/config\.js$/ }, (args) => {
      const resolveDir = args.resolveDir.replace(/\\/g, "/");
      if (resolveDir.includes("extensions/cli/src") &&
          !resolveDir.includes("node_modules") &&
          !resolveDir.includes("config/")) {
        return { path: resolve(RICA_CLI_DIR, "shim/configShim.ts") };
      }
    });

    // Redirect systemMessage.ts → rica/cli/shim/systemMessageShim.ts (RICA identity)
    build.onResolve({ filter: /\/systemMessage\.js$|^\.\/systemMessage\.js$|^\.\.\/systemMessage\.js$/ }, (args) => {
      const resolveDir = args.resolveDir.replace(/\\/g, "/");
      if (resolveDir.includes("extensions/cli/src") &&
          !resolveDir.includes("node_modules")) {
        return { path: resolve(RICA_CLI_DIR, "shim/systemMessageShim.ts") };
      }
    });

    // Redirect posthogService.ts → rica/cli/shim/posthogShim.ts (disable telemetry)
    build.onResolve({ filter: /posthogService\.js$/ }, (args) => {
      const resolveDir = args.resolveDir.replace(/\\/g, "/");
      if (resolveDir.includes("extensions/cli") &&
          !resolveDir.includes("node_modules")) {
        return { path: resolve(RICA_CLI_DIR, "shim/posthogShim.ts") };
      }
    });
  },
};

// Plugin to handle optional react-devtools-core (from upstream build.mjs)
const optionalDevtoolsPlugin = {
  name: "optional-devtools",
  setup(build) {
    build.onResolve({ filter: /^react-devtools-core$/ }, () => {
      return { path: resolve(CLI_DIR, "stubs/react-devtools-core.js") };
    });
  },
};

// Plugin to stub out native modules not needed in CLI (sqlite3 from core/)
const nativeStubPlugin = {
  name: "native-stub",
  setup(build) {
    // Stub sqlite3 and better-sqlite3 — only used by core/data/ for VS Code indexing
    build.onResolve({ filter: /^(better-)?sqlite3$/ }, () => {
      return { path: resolve(RICA_CLI_DIR, "shim/sqliteStub.ts") };
    });
    // Stub devdataSqlite.ts specifically — avoids the sqlite3 native binding
    build.onResolve({ filter: /devdataSqlite/ }, () => {
      return { path: resolve(RICA_CLI_DIR, "shim/sqliteStub.ts") };
    });
  },
};

try {
  const result = await esbuild.build({
    entryPoints: [resolve(CLI_DIR, "src/index.ts")],
    bundle: true,
    platform: "node",
    target: "node18",
    format: "esm",
    outfile: resolve(CLI_DIR, "dist/index.js"),
    external,
    sourcemap: true,
    minify: !noMinify,
    metafile: true,
    plugins: [ricaShimPlugin, optionalDevtoolsPlugin, nativeStubPlugin],

    resolveExtensions: [".ts", ".tsx", ".js", ".jsx", ".json"],

    alias: {
      "@continuedev/config-yaml": resolve(
        ROOT_DIR, "packages/config-yaml/dist/index.js",
      ),
      "@continuedev/openai-adapters": resolve(
        ROOT_DIR, "packages/openai-adapters/dist/index.js",
      ),
      "@continuedev/config-types": resolve(
        ROOT_DIR, "packages/config-types/dist/index.js",
      ),
      core: resolve(ROOT_DIR, "core"),
      "@continuedev/fetch": resolve(
        ROOT_DIR, "packages/fetch/dist/index.js",
      ),
      "@continuedev/llm-info": resolve(
        ROOT_DIR, "packages/llm-info/dist/index.js",
      ),
      "@continuedev/terminal-security": resolve(
        ROOT_DIR, "packages/terminal-security/dist/index.js",
      ),
    },

    banner: {
      js: `import { createRequire as __createRequire } from 'module';
import { fileURLToPath as __fileURLToPath } from 'url';
import { dirname as __dirnameFn } from 'path';
const require = __createRequire(import.meta.url);
const __filename = __fileURLToPath(import.meta.url);
const __dirname = __dirnameFn(__filename);`,
    },
  });

  // Write metafile
  writeFileSync(
    resolve(CLI_DIR, "dist/meta.json"),
    JSON.stringify(result.metafile, null, 2),
  );

  // Create RICA wrapper script
  writeFileSync(
    resolve(CLI_DIR, "dist/rica.js"),
    "#!/usr/bin/env node\nimport { runCli } from './index.js';\nawait runCli();\n",
  );

  // Copy worker files needed by JSDOM
  const workerSource = resolve(
    CLI_DIR, "node_modules/jsdom/lib/jsdom/living/xhr/xhr-sync-worker.js",
  );
  const workerDest = resolve(CLI_DIR, "dist/xhr-sync-worker.js");
  try {
    if (existsSync(workerSource)) {
      copyFileSync(workerSource, workerDest);
      console.log("✓ Copied xhr-sync-worker.js");
    }
  } catch (error) {
    console.warn("Warning: Could not copy xhr-sync-worker.js:", error.message);
  }

  // Make wrapper executable
  try {
    chmodSync(resolve(CLI_DIR, "dist/rica.js"), 0o755);
  } catch {
    // Windows doesn't support chmod
  }

  const bundleSize = result.metafile.outputs[Object.keys(result.metafile.outputs).find(k => k.endsWith("index.js"))]?.bytes ?? 0;
  console.log(
    `\n✓ RICA CLI build complete! Bundle size: ${(bundleSize / 1024 / 1024).toFixed(2)} MB`,
  );
  console.log(`  Output: extensions/cli/dist/rica.js`);
} catch (error) {
  console.error("❌ RICA CLI build failed:", error);
  process.exit(1);
}
