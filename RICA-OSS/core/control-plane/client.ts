import { ConfigJson } from "@continuedev/config-types";
import {
  AssistantUnrolled,
  ConfigResult,
  FQSN,
  FullSlug,
  Policy,
  SecretResult,
  SecretType,
} from "@continuedev/config-yaml";
import fetch from "node-fetch";

type RequestInit = NonNullable<Parameters<typeof fetch>[1]>;
type Response = Awaited<ReturnType<typeof fetch>>;

import { OrganizationDescription } from "../config/ProfileLifecycleManager.js";
import {
  BaseSessionMetadata,
  IDE,
  ModelDescription,
  Session,
} from "../index.js";
import { Logger } from "../util/Logger.js";

import {
  ControlPlaneSessionInfo,
  HubSessionInfo,
  isOnPremSession,
} from "./AuthTypes.js";
import { getControlPlaneEnv } from "./env.js";

export interface PolicyResponse {
  orgSlug?: string;
  policy?: Policy;
}

export interface ControlPlaneWorkspace {
  id: string;
  name: string;
  settings: ConfigJson;
}

export interface ControlPlaneModelDescription extends ModelDescription {}

export interface CreditStatus {
  optedInToFreeTrial: boolean;
  hasCredits: boolean;
  creditBalance: number;
  hasPurchasedCredits: boolean;
}

export const TRIAL_PROXY_URL =
  "https://proxy-server-blue-l6vsfbzhba-uw.a.run.app";

export interface RemoteSessionMetadata extends BaseSessionMetadata {
  isRemote: true;
  remoteId: string;
}

export interface AgentSessionMetadata {
  createdBy: string;
  github_repo: string;
  organizationId?: string;
  idempotencyKey?: string;
  source?: string;
  continueApiKeyId?: string;
  s3Url?: string;
  prompt?: string | null;
  createdBySlug?: string;
}

export interface AgentSessionView {
  id: string;
  devboxId: string | null;
  name: string | null;
  icon: string | null;
  status: string;
  agentStatus: string | null;
  unread: boolean;
  state: string;
  metadata: AgentSessionMetadata;
  repoUrl: string;
  branch: string | null;
  pullRequestUrl: string | null;
  pullRequestStatus: string | null;
  tunnelUrl: string | null;
  createdAt: string;
  updatedAt: string;
  create_time_ms: string;
  end_time_ms: string;
}

export class ControlPlaneClient {
  constructor(
    readonly sessionInfoPromise: Promise<ControlPlaneSessionInfo | undefined>,
    private readonly ide: IDE,
  ) {}

  async resolveFQSNs(
    fqsns: FQSN[],
    orgScopeId: string | null,
  ): Promise<(SecretResult | undefined)[]> {
    if (!(await this.isSignedIn())) {
      return fqsns.map((fqsn) => ({
        found: false,
        fqsn,
        secretLocation: {
          secretName: fqsn.secretName,
          secretType: SecretType.NotFound,
        },
      }));
    }

    const resp = await this.requestAndHandleError("ide/sync-secrets", {
      method: "POST",
      body: JSON.stringify({ fqsns, orgScopeId }),
    });
    return (await resp.json()) as any;
  }

  async isSignedIn(): Promise<boolean> {
    const sessionInfo = await this.sessionInfoPromise;
    return !!sessionInfo;
  }

  async getAccessToken(): Promise<string | undefined> {
    const sessionInfo = await this.sessionInfoPromise;
    return isOnPremSession(sessionInfo) ? undefined : sessionInfo?.accessToken;
  }

  private async request(path: string, init: RequestInit): Promise<Response> {
    const sessionInfo = await this.sessionInfoPromise;
    const onPremSession = isOnPremSession(sessionInfo);
    const accessToken = await this.getAccessToken();

    // Bearer token not necessary for on-prem auth type
    if (!accessToken && !onPremSession) {
      throw new Error("No access token");
    }

    const env = await getControlPlaneEnv(this.ide.getIdeSettings());
    const url = new URL(path, env.CONTROL_PLANE_URL).toString();
    const ideInfo = await this.ide.getIdeInfo();

    const resp = await fetch(url, {
      ...init,
      headers: {
        ...init.headers,
        Authorization: `Bearer ${accessToken}`,
        ...{
          "x-extension-version": ideInfo.extensionVersion,
          "x-is-prerelease": String(ideInfo.isPrerelease),
        },
      },
    });

    return resp;
  }

  private async requestAndHandleError(
    path: string,
    init: RequestInit,
  ): Promise<Response> {
    const resp = await this.request(path, init);

    if (!resp.ok) {
      throw new Error(
        `Control plane request failed: ${resp.status} ${await resp.text()}`,
      );
    }

    return resp;
  }

  // RICA (Hub API severed):
  //
  // The methods below originally hit `api.continue.dev` (Continue's Hub API)
  // for assistant listings, organisation membership, policy, free-trial
  // credits, and the Models add-on checkout URL. None of those services
  // exist for Rica — Sang's `/api/user-model/configs` backend supplies the
  // dynamic assistant directly via `RicaProfileLoader` and there is no
  // multi-org / credits / checkout flow.
  //
  // At runtime these are overridden by `RicaControlPlaneClient` (installed
  // via the property-descriptor trap in `rica/adapters/core/RicaCore.ts`)
  // to return empty-state values that the UI treats as "no assistants /
  // no orgs / no credits". The base-class severs below are defence in
  // depth: if the trap ever fails, a stray fixture/test path is hit, or
  // a future caller forgets to use `RicaControlPlaneClient`, we throw a
  // Rica-recognisable error rather than firing an HTTP request to the
  // re-pointed `CONTROL_PLANE_URL` (which routes at MS Login →
  // would 404 noisily). The original bodies are gone — keep the methods
  // present so subclass `override` markers (and TypeScript) still resolve.
  public async listAssistants(_organizationId: string | null): Promise<
    {
      configResult: ConfigResult<AssistantUnrolled>;
      ownerSlug: string;
      packageSlug: string;
      iconUrl: string;
      rawYaml: string;
    }[]
  > {
    throw new Error(
      "Rica: not configured — Continue Hub `listAssistants` severed (use RicaControlPlaneClient override)",
    );
  }

  public async listOrganizations(): Promise<Array<OrganizationDescription>> {
    throw new Error(
      "Rica: not configured — Continue Hub `listOrganizations` severed (use RicaControlPlaneClient override)",
    );
  }

  public async listAssistantFullSlugs(
    _organizationId: string | null,
  ): Promise<FullSlug[] | null> {
    throw new Error(
      "Rica: not configured — Continue Hub `listAssistantFullSlugs` severed",
    );
  }

  public async getPolicy(): Promise<PolicyResponse | null> {
    throw new Error(
      "Rica: not configured — Continue Hub `getPolicy` severed (use RicaControlPlaneClient override)",
    );
  }

  public async getCreditStatus(): Promise<CreditStatus | null> {
    throw new Error(
      "Rica: not configured — Continue Hub `getCreditStatus` severed (use RicaControlPlaneClient override)",
    );
  }

  public async getModelsAddOnCheckoutUrl(
    _vsCodeUriScheme?: string,
  ): Promise<{ url: string } | null> {
    throw new Error(
      "Rica: not configured — Continue Hub `getModelsAddOnCheckoutUrl` severed",
    );
  }

  /**
   * Check if remote sessions should be enabled based on feature flags
   */
  public async shouldEnableRemoteSessions(): Promise<boolean> {
    // Check if user is signed in
    if (!(await this.isSignedIn())) {
      return false;
    }

    try {
      const sessionInfo = await this.sessionInfoPromise;
      if (isOnPremSession(sessionInfo) || !sessionInfo) {
        return false;
      }

      return true;
    } catch (e) {
      Logger.error(e, {
        context: "control_plane_check_remote_sessions_enabled",
      });
      return false;
    }
  }

  /**
   * Get current user's session info
   */
  public async getSessionInfo(): Promise<ControlPlaneSessionInfo | undefined> {
    return await this.sessionInfoPromise;
  }

  // RICA (Background Agent severed):
  //
  // Refork added remote / Background Agent support that posts to
  // `api.continue.dev/agents/*` and tunnels into Continue-hosted devboxes.
  // Renesas has no equivalent backend yet — the decision in
  // `extensions/.rica-debug/after-migration-fixes.md` (Phase 5.3.2) is to
  // either hide Background mode from the cycle or render an empty-state
  // panel; either way these methods must never reach the wire.
  //
  // `RicaControlPlaneClient` already overrides
  // `shouldEnableRemoteSessions` → false and `listRemoteSessions` → []
  // so the History tab renders empty. The remaining methods don't have
  // Rica overrides yet — VsCodeMessenger handlers swallow them
  // before they get here, but if a path slips through, throw rather than
  // POST to the now-MS-Login-pointed CONTROL_PLANE_URL.
  public async listRemoteSessions(): Promise<RemoteSessionMetadata[]> {
    throw new Error(
      "Rica: not configured — Background Agent `listRemoteSessions` severed (use RicaControlPlaneClient override)",
    );
  }

  public async loadRemoteSession(_remoteId: string): Promise<Session> {
    throw new Error(
      "Rica: not configured — Background Agent `loadRemoteSession` severed",
    );
  }

  public async createBackgroundAgent(
    _prompt: string,
    _repoUrl: string,
    _name: string,
    _branch?: string,
    _organizationId?: string,
    _contextItems?: any[],
    _selectedCode?: any[],
    _agent?: string,
  ): Promise<{ id: string }> {
    throw new Error(
      "Rica: not configured — Background Agent `createBackgroundAgent` severed",
    );
  }

  public async listBackgroundAgents(
    _organizationId?: string,
    _limit?: number,
  ): Promise<{
    agents: Array<{
      id: string;
      name: string | null;
      status: string;
      repoUrl: string;
      createdAt: string;
      metadata?: {
        github_repo?: string;
      };
    }>;
    totalCount: number;
  }> {
    throw new Error(
      "Rica: not configured — Background Agent `listBackgroundAgents` severed",
    );
  }

  public async getAgentSession(
    _agentSessionId: string,
  ): Promise<AgentSessionView | null> {
    throw new Error(
      "Rica: not configured — Background Agent `getAgentSession` severed",
    );
  }

  public async getAgentState(_agentSessionId: string): Promise<{
    session: Session;
    isProcessing: boolean;
    messageQueueLength: number;
    pendingPermission: any;
  } | null> {
    throw new Error(
      "Rica: not configured — Background Agent `getAgentState` severed",
    );
  }
}
