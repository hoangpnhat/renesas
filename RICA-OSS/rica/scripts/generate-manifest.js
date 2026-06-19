#!/usr/bin/env node

/**
 * RICA VS Code Manifest Generator
 *
 * Generates extensions/vscode/package.json from:
 * - brand.json (single source of truth)
 * - vscode.manifest.overrides.json (VS Code specific overrides)
 * - extensions/vscode/package.json (base from upstream)
 *
 * Per RICA_REPO_STANDARD.md section 7:
 * - Branding must have single source of truth
 * - package.json is generated, not manually edited
 * - All RICA branding applied at build time
 *
 * Usage:
 *   node rica/scripts/generate-manifest.js
 */

const fs = require("fs");
const path = require("path");

const ROOT_DIR = path.join(__dirname, "../..");
const BRAND_CONFIG_PATH = path.join(__dirname, "../branding/brand.json");
const OVERRIDES_PATH = path.join(
  __dirname,
  "../branding/vscode.manifest.overrides.json",
);
const PACKAGE_JSON_PATH = path.join(ROOT_DIR, "extensions/vscode/package.json");

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
  console.error(`❌ Failed to load manifest overrides: ${e.message}`);
  process.exit(1);
}

try {
  baseManifest = JSON.parse(fs.readFileSync(PACKAGE_JSON_PATH, "utf8"));
} catch (e) {
  console.error(`❌ Failed to load base package.json: ${e.message}`);
  process.exit(1);
}

console.log("🔧 Generating VS Code manifest for RICA...\n");

/**
 * Deep merge objects
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

/**
 * Apply RICA branding from brand.json
 */
function applyBranding(manifest) {
  // Core identity
  manifest.name = brandConfig.extension.name;
  manifest.displayName = brandConfig.extension.displayName;
  manifest.publisher = brandConfig.extension.publisher;
  manifest.version = brandConfig.product.version;
  manifest.description = brandConfig.product.description;

  // URLs
  manifest.homepage = brandConfig.urls.homepage;
  if (!manifest.repository) manifest.repository = {};
  manifest.repository.url = brandConfig.urls.repository;
  if (!manifest.bugs) manifest.bugs = {};
  manifest.bugs.url = brandConfig.urls.issues;

  // Assets
  manifest.icon = brandConfig.assets.icon;

  // Pricing
  manifest.pricing = brandConfig.extension.pricing;

  return manifest;
}

/**
 * Normalize command and config namespaces to 'rica'
 */
function normalizeNamespaces(manifest) {
  // Commands
  if (manifest.contributes && manifest.contributes.commands) {
    manifest.contributes.commands = manifest.contributes.commands.map((cmd) => {
      if (cmd.command && cmd.command.startsWith("continue.")) {
        cmd.command = cmd.command.replace("continue.", "rica.");
      }
      if (cmd.category === "Continue") {
        cmd.category = "RICA";
      }
      // Normalize group names: "Continue" → "RICA"
      if (cmd.group === "Continue") {
        cmd.group = "RICA";
      }
      return cmd;
    });
  }

  // Keybindings
  if (manifest.contributes && manifest.contributes.keybindings) {
    manifest.contributes.keybindings = manifest.contributes.keybindings.map(
      (kb) => {
        if (kb.command && kb.command.startsWith("continue.")) {
          kb.command = kb.command.replace("continue.", "rica.");
        }
        return kb;
      },
    );
  }

  // Menus
  if (manifest.contributes && manifest.contributes.menus) {
    Object.keys(manifest.contributes.menus).forEach((menuKey) => {
      manifest.contributes.menus[menuKey] = manifest.contributes.menus[
        menuKey
      ].map((item) => {
        if (item.command && item.command.startsWith("continue.")) {
          item.command = item.command.replace("continue.", "rica.");
        }
        // Normalize view references in when clauses (handle both upstream and partially-branded states)
        if (item.when) {
          item.when = item.when
            .replace(/continue\.continueGUIView/g, "rica.ricaGUIView")
            .replace(/rica\.continueGUIView/g, "rica.ricaGUIView")
            .replace(/continue\.continueConsoleView/g, "rica.ricaConsoleView")
            .replace(/rica\.continueConsoleView/g, "rica.ricaConsoleView")
            .replace(/continue\./g, "rica.");
        }
        // Normalize group names: "Continue" → "RICA"
        if (item.group && item.group === "Continue") {
          item.group = "RICA";
        }
        return item;
      });
    });
  }

  // Configuration
  if (manifest.contributes && manifest.contributes.configuration) {
    const config = manifest.contributes.configuration;

    // Update title
    if (config.title === "Continue") {
      config.title = "RICA";
    }

    // Update property keys from continue.* to rica.*
    if (config.properties) {
      const newProps = {};
      Object.keys(config.properties).forEach((key) => {
        const newKey = key.startsWith("continue.")
          ? key.replace("continue.", "rica.")
          : key;
        newProps[newKey] = config.properties[key];
      });
      config.properties = newProps;
    }
  }

  // Views containers
  if (manifest.contributes && manifest.contributes.viewsContainers) {
    if (manifest.contributes.viewsContainers.activitybar) {
      manifest.contributes.viewsContainers.activitybar =
        manifest.contributes.viewsContainers.activitybar.map((container) => {
          if (container.id === "continue") {
            container.id = "rica";
            container.title = "RICA";
            container.icon = brandConfig.assets.sidebarIcon;
          }
          return container;
        });
    }

    // Panel: continueConsole → ricaConsole (handle already-branded state too)
    if (manifest.contributes.viewsContainers.panel) {
      manifest.contributes.viewsContainers.panel =
        manifest.contributes.viewsContainers.panel.map((panel) => {
          if (panel.id === "continueConsole" || panel.id === "ricaConsole") {
            panel.id = "ricaConsole";
            panel.title = "RICA Console";
          }
          return panel;
        });
    }
  }

  // Views — handle both upstream ("continue") and partially-branded ("rica") keys
  if (manifest.contributes && manifest.contributes.views) {
    // Normalize GUI view: source key may be "continue" or already "rica"
    const guiSourceKey = manifest.contributes.views.continue
      ? "continue"
      : manifest.contributes.views.rica
        ? "rica"
        : null;

    if (guiSourceKey) {
      manifest.contributes.views.rica = manifest.contributes.views[
        guiSourceKey
      ].map((view) => {
        // Normalize ID: any variant → rica.ricaGUIView
        if (view.id) {
          view.id = view.id
            .replace(/^continue\./, "rica.")
            .replace(/continueGUIView/, "ricaGUIView");
          // Ensure final form even if partially branded (rica.continueGUIView → rica.ricaGUIView)
          if (view.id === "rica.continueGUIView") {
            view.id = "rica.ricaGUIView";
          }
        }
        if (view.name === "Continue") {
          view.name = "RICA";
        }
        return view;
      });
      if (guiSourceKey !== "rica") {
        delete manifest.contributes.views[guiSourceKey];
      }
    }

    // Normalize Console view: source key may be "continueConsole" or already "ricaConsole"
    const consoleSourceKey = manifest.contributes.views.continueConsole
      ? "continueConsole"
      : manifest.contributes.views.ricaConsole
        ? "ricaConsole"
        : null;

    if (consoleSourceKey) {
      manifest.contributes.views.ricaConsole = manifest.contributes.views[
        consoleSourceKey
      ].map((view) => {
        // Normalize ID: any variant → rica.ricaConsoleView
        if (view.id) {
          view.id = view.id
            .replace(/^continue\./, "rica.")
            .replace(/continueConsoleView/, "ricaConsoleView");
          if (view.id === "rica.continueConsoleView") {
            view.id = "rica.ricaConsoleView";
          }
        }
        if (view.name === "Continue Console") {
          view.name = "RICA Console";
        }
        if (view.when) {
          view.when = view.when.replace(/config\.continue\./g, "config.rica.");
        }
        return view;
      });
      if (consoleSourceKey !== "ricaConsole") {
        delete manifest.contributes.views[consoleSourceKey];
      }
    }
  }

  // Submenus
  if (manifest.contributes && manifest.contributes.submenus) {
    manifest.contributes.submenus = manifest.contributes.submenus.map(
      (submenu) => {
        if (submenu.label === "Continue") {
          submenu.label = "RICA";
        }
        return submenu;
      },
    );
  }

  // Activation events
  if (manifest.activationEvents) {
    manifest.activationEvents = manifest.activationEvents.map((event) => {
      if (event === "onView:continueGUIView") {
        return "onView:rica.ricaGUIView";
      }
      // Handle both upstream (continue.) and partially-branded (rica.) states
      if (event.includes("continueGUIView")) {
        return "onView:rica.ricaGUIView";
      }
      if (event.includes("continueConsoleView")) {
        return "onView:rica.ricaConsoleView";
      }
      if (event.startsWith("onView:continue.")) {
        return event.replace("onView:continue.", "onView:rica.");
      }
      if (event.startsWith("onCommand:continue.")) {
        return event.replace("onCommand:continue.", "onCommand:rica.");
      }
      return event;
    });
  }

  return manifest;
}

// Generate manifest
let generatedManifest = baseManifest;

// Step 1: Apply branding from brand.json
generatedManifest = applyBranding(generatedManifest);

// Step 2: Normalize namespaces
generatedManifest = normalizeNamespaces(generatedManifest);

// Step 3: Apply VS Code specific overrides
generatedManifest = deepMerge(generatedManifest, overrides);

// Write generated manifest
try {
  fs.writeFileSync(
    PACKAGE_JSON_PATH,
    JSON.stringify(generatedManifest, null, 2) + "\n",
    "utf8",
  );
  console.log("✅ Generated manifest successfully");
  console.log(`   Output: ${PACKAGE_JSON_PATH}\n`);
} catch (e) {
  console.error(`❌ Failed to write manifest: ${e.message}`);
  process.exit(1);
}

// Report what was changed
console.log("📝 Applied branding:");
console.log(`   - Name: ${generatedManifest.name}`);
console.log(`   - Display Name: ${generatedManifest.displayName}`);
console.log(`   - Publisher: ${generatedManifest.publisher}`);
console.log(`   - Homepage: ${generatedManifest.homepage}`);
console.log(`   - Icon: ${generatedManifest.icon}\n`);

console.log("✨ Done! Package.json is ready for RICA build.");
