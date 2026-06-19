import * as fs from "node:fs";
import {
  getLocalEnvironmentDotFilePath,
  getStagingEnvironmentDotFilePath,
} from "../../utils/paths";
import { AuthType, ControlPlaneEnv } from "../auth/AuthTypes";

// Re-export types for convenience
export type { AuthType, ControlPlaneEnv } from "../auth/AuthTypes";

// IdeSettings type - minimal definition for this module
export interface IdeSettings {
  continueTestEnvironment?:
    | "production"
    | "staging"
    | "local"
    | "test"
    | "none";
}

export const EXTENSION_NAME = "rica";

const ENTRA_ID_PROD = "30a4cd4a-6cd0-42d7-86aa-45e7cefc5b3f";
const ENTRA_ID_NON_PROD = "0fe63045-971f-47fd-ac08-314142ab8f0c";

const PRODUCTION_HUB_ENV: ControlPlaneEnv = {
  DEFAULT_CONTROL_PLANE_PROXY_URL:
    "https://login.microsoftonline.com/53d82571-da19-47e4-9cb4-625a166a4a2a/",
  CONTROL_PLANE_URL:
    "https://login.microsoftonline.com/53d82571-da19-47e4-9cb4-625a166a4a2a/",
  AUTH_TYPE: AuthType.RicaProd,
  ENTRA_ID_CLIENT_ID: ENTRA_ID_PROD,
  APP_URL:
    // "https://waps-dev-ai-nonprod-ricadevuat-westus2-001.azurewebsites.net/",
    "https://rica.global.renesas.com",
};

const STAGING_ENV: ControlPlaneEnv = {
  DEFAULT_CONTROL_PLANE_PROXY_URL:
    "https://login.microsoftonline.com/53d82571-da19-47e4-9cb4-625a166a4a2a/",
  CONTROL_PLANE_URL:
    "https://login.microsoftonline.com/53d82571-da19-47e4-9cb4-625a166a4a2a/",
  AUTH_TYPE: AuthType.RicaStaging,
  ENTRA_ID_CLIENT_ID: ENTRA_ID_NON_PROD,
  APP_URL:
    // "https://waps-dev-ai-nonprod-ricadevuat-westus2-001.azurewebsites.net/",
    "https://rica-uat.global.renesas.com",
};

const TEST_ENV: ControlPlaneEnv = {
  DEFAULT_CONTROL_PLANE_PROXY_URL:
    "https://login.microsoftonline.com/53d82571-da19-47e4-9cb4-625a166a4a2a/",
  CONTROL_PLANE_URL:
    "https://login.microsoftonline.com/53d82571-da19-47e4-9cb4-625a166a4a2a/",
  AUTH_TYPE: AuthType.RicaStaging,
  ENTRA_ID_CLIENT_ID: ENTRA_ID_NON_PROD,
  APP_URL:
    // "https://waps-dev-ai-nonprod-ricadevuat-westus2-001.azurewebsites.net/",
    "https://rica-uat.global.renesas.com",
};

const LOCAL_ENV: ControlPlaneEnv = {
  DEFAULT_CONTROL_PLANE_PROXY_URL:
    "https://login.microsoftonline.com/53d82571-da19-47e4-9cb4-625a166a4a2a/",
  CONTROL_PLANE_URL:
    "https://login.microsoftonline.com/53d82571-da19-47e4-9cb4-625a166a4a2a/",
  AUTH_TYPE: AuthType.RicaStaging,
  ENTRA_ID_CLIENT_ID: ENTRA_ID_NON_PROD,
  APP_URL:
    // "https://waps-dev-ai-nonprod-ricadevuat-westus2-001.azurewebsites.net/",
    "http://localhost:8082",
};

export async function enableHubContinueDev() {
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
  // MDM override
  // const licenseKeyData = getLicenseKeyData();
  // if (licenseKeyData?.unsignedData?.apiUrl) {
  //   const { apiUrl } = licenseKeyData.unsignedData;
  //   return {
  //     AUTH_TYPE: AuthType.OnPrem,
  //     DEFAULT_CONTROL_PLANE_PROXY_URL: apiUrl,
  //     CONTROL_PLANE_URL: apiUrl,
  //     APP_URL: "https://hub.continue.dev/",
  //   };
  // }

  // Environment dot file detection (in ~/.rica/)
  // Note: .local overrides .staging
  if (fs.existsSync(getLocalEnvironmentDotFilePath())) {
    return LOCAL_ENV;
  }

  if (fs.existsSync(getStagingEnvironmentDotFilePath())) {
    return STAGING_ENV;
  }

  // Fallback to IDE settings or environment variable
  const env =
    ideTestEnvironment === "production"
      ? "hub"
      : ideTestEnvironment === "staging"
        ? "staging"
        : ideTestEnvironment === "local"
          ? "local"
          : ideTestEnvironment === "test"
            ? "test"
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
  const ideSettings = await ideSettingsPromise;
  return ideSettings.continueTestEnvironment !== "none";
}
