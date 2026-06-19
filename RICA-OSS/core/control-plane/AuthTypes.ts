export interface HubSessionInfo {
  AUTH_TYPE:
    | AuthType.WorkOsProd
    | AuthType.WorkOsStaging
    | AuthType.RicaProd
    | AuthType.RicaStaging;
  accessToken: string;
  account: {
    label: string;
    id: string;
  };
}

export interface OnPremSessionInfo {
  AUTH_TYPE: AuthType.OnPrem;
}

export type ControlPlaneSessionInfo = HubSessionInfo | OnPremSessionInfo;

export function isOnPremSession(
  sessionInfo: ControlPlaneSessionInfo | undefined,
): sessionInfo is OnPremSessionInfo {
  return sessionInfo !== undefined && sessionInfo.AUTH_TYPE === AuthType.OnPrem;
}

export enum AuthType {
  WorkOsProd = "continue",
  WorkOsStaging = "continue-staging",
  RicaProd = "Rica",
  RicaStaging = "rica-staging",
  OnPrem = "on-prem",
}

export interface HubEnv {
  DEFAULT_CONTROL_PLANE_PROXY_URL: string;
  CONTROL_PLANE_URL: string;
  AUTH_TYPE:
    | AuthType.WorkOsProd
    | AuthType.WorkOsStaging
    | AuthType.RicaProd
    | AuthType.RicaStaging;
  WORKOS_CLIENT_ID?: string;
  ENTRA_ID_CLIENT_ID?: string;
  APP_URL: string;
}

export interface OnPremEnv {
  AUTH_TYPE: AuthType.OnPrem;
  DEFAULT_CONTROL_PLANE_PROXY_URL: string;
  CONTROL_PLANE_URL: string;
  APP_URL: string;
}

export type ControlPlaneEnv = HubEnv | OnPremEnv;

export function isHubEnv(env: ControlPlaneEnv): env is HubEnv {
  return (
    "AUTH_TYPE" in env &&
    env.AUTH_TYPE !== "on-prem" &&
    "WORKOS_CLIENT_ID" in env
  );
}
