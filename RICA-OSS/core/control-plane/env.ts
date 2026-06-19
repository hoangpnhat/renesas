import * as fs from "node:fs";
import { IdeSettings } from "..";
import {
  getLocalEnvironmentDotFilePath,
  getStagingEnvironmentDotFilePath,
} from "../util/paths";
import { AuthType, ControlPlaneEnv } from "./AuthTypes";
import { getLicenseKeyData } from "./mdm/mdm";

export const EXTENSION_NAME = "rica";

const WORKOS_CLIENT_ID_PRODUCTION = "client_01J0FW6XN8N2XJAECF7NE0Y65J";
const WORKOS_CLIENT_ID_STAGING = "client_01J0FW6XCPMJMQ3CG51RB4HBZQ";

// RICA (control-plane env wire): re-point every Continue
// default URL for the RICA build. The upstream URLs below stay in the file
// only as a paper trail for what was severed — getControlPlaneEnvSync()
// short-circuits to the RICA_* envs first when EXTENSION_NAME === "rica",
// so PRODUCTION_HUB_ENV / STAGING_ENV / TEST_ENV / LOCAL_ENV are unreachable
// in production. Mirrors rica/adapters/config/env.ts which the vscode shim
// already exports — but core files import THIS file directly (ConfigHandler,
// doLoadConfig, loadYaml, PlatformProfileLoader, RulesContextProvider,
// core.ts, NextEditLoggingService → 14+ env.APP_URL callsites in supplement
// Bug 3), so the redirect has to live here too.
const RICA_PROD_ENV: ControlPlaneEnv = {
  DEFAULT_CONTROL_PLANE_PROXY_URL:
    "https://login.microsoftonline.com/53d82571-da19-47e4-9cb4-625a166a4a2a/",
  CONTROL_PLANE_URL:
    "https://login.microsoftonline.com/53d82571-da19-47e4-9cb4-625a166a4a2a/",
  AUTH_TYPE: AuthType.WorkOsProd,
  WORKOS_CLIENT_ID: WORKOS_CLIENT_ID_PRODUCTION,
  APP_URL: "https://rica.global.renesas.com/",
};

const RICA_STAGING_ENV: ControlPlaneEnv = {
  DEFAULT_CONTROL_PLANE_PROXY_URL:
    "https://login.microsoftonline.com/53d82571-da19-47e4-9cb4-625a166a4a2a/",
  CONTROL_PLANE_URL:
    "https://login.microsoftonline.com/53d82571-da19-47e4-9cb4-625a166a4a2a/",
  AUTH_TYPE: AuthType.WorkOsStaging,
  WORKOS_CLIENT_ID: WORKOS_CLIENT_ID_STAGING,
  APP_URL: "https://rica-uat.global.renesas.com/",
};

// Upstream defaults — kept ONLY as documentation of what was severed. Not
// reachable when EXTENSION_NAME === "rica". Do not import these, do not
// reference these — getControlPlaneEnvSync below returns the RICA_* envs.
const PRODUCTION_HUB_ENV: ControlPlaneEnv = {
  DEFAULT_CONTROL_PLANE_PROXY_URL: "https://api.continue.dev/",
  CONTROL_PLANE_URL: "https://api.continue.dev/",
  AUTH_TYPE: AuthType.WorkOsProd,
  WORKOS_CLIENT_ID: WORKOS_CLIENT_ID_PRODUCTION,
  APP_URL: "https://continue.dev/",
};

const STAGING_ENV: ControlPlaneEnv = {
  DEFAULT_CONTROL_PLANE_PROXY_URL: "https://api.continue-stage.tools/",
  CONTROL_PLANE_URL: "https://api.continue-stage.tools/",
  AUTH_TYPE: AuthType.WorkOsStaging,
  WORKOS_CLIENT_ID: WORKOS_CLIENT_ID_STAGING,
  APP_URL: "https://hub.continue-stage.tools/",
};

const TEST_ENV: ControlPlaneEnv = {
  DEFAULT_CONTROL_PLANE_PROXY_URL: "https://api-test.continue.dev/",
  CONTROL_PLANE_URL: "https://api-test.continue.dev/",
  AUTH_TYPE: AuthType.WorkOsStaging,
  WORKOS_CLIENT_ID: WORKOS_CLIENT_ID_STAGING,
  APP_URL: "https://app-test.continue.dev/",
};

const LOCAL_ENV: ControlPlaneEnv = {
  DEFAULT_CONTROL_PLANE_PROXY_URL: "http://localhost:3001/",
  CONTROL_PLANE_URL: "http://localhost:3001/",
  AUTH_TYPE: AuthType.WorkOsStaging,
  WORKOS_CLIENT_ID: WORKOS_CLIENT_ID_STAGING,
  APP_URL: "http://localhost:3000/",
};

export async function enableHubContinueDev() {
  // RICA: RICA build never enables Continue's Hub. Mirrors useHub()
  // below. Several upstream callers gate Hub-only UI on this — returning
  // false hides the "create assistant on continue.dev" prompts entirely.
  if (EXTENSION_NAME === "rica") {
    return false;
  }
  return true;
}

export async function getControlPlaneEnv(
  ideSettingsPromise: Promise<IdeSettings>,
): Promise<ControlPlaneEnv> {
  const ideSettings = await ideSettingsPromise;
  return getControlPlaneEnvSync(ideSettings.continueTestEnvironment);
}

export function getControlPlaneEnvSync(
  ideTestEnvironment: IdeSettings["continueTestEnvironment"],
): ControlPlaneEnv {
  // RICA short-circuit. Every core file imports THIS
  // module (ConfigHandler, doLoadConfig, loadYaml, PlatformProfileLoader,
  // RulesContextProvider, core.ts, NextEditLoggingService) — bypassing the
  // vscode-shim re-export at extensions/vscode/src/control-plane-env.ts that
  // already pointed at rica/adapters/config/env.ts. Without this branch,
  // env.APP_URL still resolves to "https://continue.dev/" for those callers
  // (14+ callsites in supplement Bug 3), leaking ${APP_URL}${profileId} to
  // continue.dev anywhere the GUI posts config/openProfile, the auth flow
  // builds a redirect_uri, RulesContextProvider mints a rule URI, etc.
  // Honour staging dotfile by routing to RICA_STAGING_ENV; everything else
  // (including production, local, test, and "none") routes to RICA_PROD_ENV
  // since we never run a real local control-plane.
  if (EXTENSION_NAME === "rica") {
    if (fs.existsSync(getStagingEnvironmentDotFilePath())) {
      return RICA_STAGING_ENV;
    }
    if (
      ideTestEnvironment === "staging" ||
      ideTestEnvironment === "local"
    ) {
      return RICA_STAGING_ENV;
    }
    return RICA_PROD_ENV;
  }

  // MDM override
  const licenseKeyData = getLicenseKeyData();
  if (licenseKeyData?.unsignedData?.apiUrl) {
    const { apiUrl } = licenseKeyData.unsignedData;
    return {
      AUTH_TYPE: AuthType.OnPrem,
      DEFAULT_CONTROL_PLANE_PROXY_URL: apiUrl,
      CONTROL_PLANE_URL: apiUrl,
      APP_URL: "https://continue.dev/",
    };
  }

  // Note .local overrides .staging
  if (fs.existsSync(getLocalEnvironmentDotFilePath())) {
    return LOCAL_ENV;
  }

  if (fs.existsSync(getStagingEnvironmentDotFilePath())) {
    return STAGING_ENV;
  }

  const env =
    ideTestEnvironment === "production"
      ? "hub"
      : ideTestEnvironment === "staging"
        ? "staging"
        : ideTestEnvironment === "local"
          ? "local"
          : process.env.CONTROL_PLANE_ENV;

  return env === "local"
    ? LOCAL_ENV
    : env === "staging"
      ? STAGING_ENV
      : env === "test"
        ? TEST_ENV
        : PRODUCTION_HUB_ENV;
}

export async function useHub(
  ideSettingsPromise: Promise<IdeSettings>,
): Promise<boolean> {
  // RICA uses its own backend, not Continue Hub. Mirrors enableHubContinueDev
  // above; both gate Hub-only UI/auth paths.
  if (EXTENSION_NAME === "rica") {
    return false;
  }
  const ideSettings = await ideSettingsPromise;
  return ideSettings.continueTestEnvironment !== "none";
}
