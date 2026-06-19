import type {
  AssistantUnrolled,
  ConfigResult,
} from "../../../packages/config-yaml/src/index.js";
import type { OrganizationDescription } from "../../../core/config/ProfileLifecycleManager.js";
import type { ControlPlaneSessionInfo } from "../../../core/control-plane/AuthTypes.js";
import type {
  CreditStatus,
  PolicyResponse,
  RemoteSessionMetadata,
} from "../../../core/control-plane/client.js";
import { ControlPlaneClient } from "../../../core/control-plane/client.js";
import type { EntraIDDynamicConfig, IDE } from "../../../core/index.js";
import { Logger } from "../../../core/util/Logger.js";
import fetch from "node-fetch";
import { RicaProfileLoader } from "../config/RicaProfileLoader.js";

type DynamicAssistantEntry = {
  configResult: ConfigResult<AssistantUnrolled>;
  ownerSlug: string;
  packageSlug: string;
  iconUrl: string;
  rawYaml: string;
};

export class RicaControlPlaneClient extends ControlPlaneClient {
  private static readonly CACHE_TTL_MS = 30_000;

  private readonly profileLoader: RicaProfileLoader;
  private cachedAssistant: DynamicAssistantEntry | null = null;
  private cacheTimestamp = 0;
  private inflightPromise?: Promise<DynamicAssistantEntry | null>;

  /**
   * v1.2.0 parity — cached EntraIDDynamicConfig block from Sang's backend.
   * Refreshed alongside the assistant cache so the merge into runtime
   * ContinueConfig (in core/config/load.ts) sees the latest backend state.
   * Mirrors the behaviour of v1.2.0's `EntraIDDynamicConfigService.fetchDynamicConfig`
   * cache.
   */
  private cachedEntraIDDynamicConfig: EntraIDDynamicConfig | null = null;

  /** Short-lived cache for the freshly-minted MDP token (see getAccessToken). */
  private cachedMdpToken?: string;
  private cachedMdpExpMs = 0;

  constructor(
    sessionInfoPromise: Promise<ControlPlaneSessionInfo | undefined>,
    ide: IDE,
    private readonly entraTokenGetter?: () => Promise<string | undefined>,
    private readonly mdpTokenGetter?: () => Promise<string | undefined>,
  ) {
    super(sessionInfoPromise, ide);
    this.profileLoader = new RicaProfileLoader(
      ide.getIdeSettings(),
      fetch as unknown as typeof globalThis.fetch,
      entraTokenGetter ?? (() => this.getAccessToken()),
      () => this.getAccessToken(),
    );
  }

  /**
   * Upstream's `isSignedIn` reads the WorkOS sessionInfoPromise. RICA
   * authenticates via EntraID, so we override to gate on the EntraID token
   * getter. Without this, ConfigHandler.getOrgs() falls through to the
   * local-only path and the "RICA Dynamic" profile never appears in the
   * config dropdown — even though buildDynamicAssistant successfully
   * fetches models from Sang's backend.
   */
  public async isSignedIn(): Promise<boolean> {
    if (this.entraTokenGetter) {
      try {
        const entraToken = await this.entraTokenGetter();
        if (entraToken) {
          return true;
        }
      } catch {
        // fall through to upstream check
      }
    }
    return super.isSignedIn();
  }

  /**
   * Phase 5.x — the actual fix for the recurring "HTTP 401 Token is expired".
   *
   * The base ControlPlaneClient.getAccessToken() reads sessionInfo.accessToken
   * off a promise FROZEN at client construction. It never re-mints. Every
   * 5-min RicaCore.refreshDynamicConfig tick re-rotates that same frozen MDP
   * token into model.apiKey (verified: [RICA-PROXY:rotate] showed
   * oldLen===newLen with an exp claim ~12h in the past). Once the cached
   * token's exp passes (~60-75 min, or after the machine sleeps), every
   * Databricks /invocations call 401s until a full window reload rebuilds the
   * client stack.
   *
   * The real fresh-MDP source is getControlPlaneSessionInfo (wired here as
   * mdpTokenGetter): it calls EntraIDAuthProvider.ensureValidToken(), which is
   * a no-network no-op when the MDP JWT is still valid and only triggers a
   * refresh_token re-mint when it has expired. We cache the result until ~2min
   * before expiry so the steady-state path stays a cheap in-memory read and we
   * don't hit the backend on every getAccessToken() call (the base method is
   * called by request(), buildDynamicAssistant, doLoadConfig, and the per-tick
   * apiKey rotation).
   *
   * Why this does NOT reintroduce the "Unable to load OAuth Config" 400 that
   * reverted the earlier attempt (0d1ac9498/4ed943229): that attempt returned
   * the EntraID token (audience api://<entra-client-id>). This returns the MDP
   * token (audience = Databricks workspace OAuth Config registry) — the SAME
   * token shape the frozen path returned, just kept current. Audience contract
   * is preserved; only staleness is fixed.
   *
   * Falls back to the frozen base token if no mdpTokenGetter was wired or the
   * re-mint fails, so behaviour is never worse than before this fix.
   */
  public async getAccessToken(): Promise<string | undefined> {
    if (!this.mdpTokenGetter) {
      return super.getAccessToken();
    }

    const REFRESH_SKEW_MS = 2 * 60 * 1000;
    if (
      this.cachedMdpToken &&
      this.cachedMdpExpMs - Date.now() > REFRESH_SKEW_MS
    ) {
      return this.cachedMdpToken;
    }

    try {
      const fresh = await this.mdpTokenGetter();
      if (fresh) {
        this.cachedMdpToken = fresh;
        this.cachedMdpExpMs = this.readJwtExpMs(fresh) ?? 0;
        return fresh;
      }
    } catch (e) {
      Logger.warn(
        `[RICA] getAccessToken re-mint failed, falling back to frozen token: ${e}`,
      );
    }

    return this.cachedMdpToken ?? (await super.getAccessToken());
  }

  /** Decode a JWT's exp claim (seconds) → epoch ms. undefined if unparseable. */
  private readJwtExpMs(jwt: string): number | undefined {
    try {
      const parts = jwt.split(".");
      if (parts.length !== 3) {
        return undefined;
      }
      const payload = JSON.parse(
        Buffer.from(parts[1], "base64").toString("utf-8"),
      );
      return typeof payload.exp === "number" ? payload.exp * 1000 : undefined;
    } catch {
      return undefined;
    }
  }

  private async buildDynamicAssistant(): Promise<DynamicAssistantEntry | null> {
    // Prefer EntraID token; fall back to upstream session token only if
    // the entraTokenGetter wasn't wired (defensive — shouldn't happen).
    const token = this.entraTokenGetter
      ? (await this.entraTokenGetter()) || (await this.getAccessToken())
      : await this.getAccessToken();
    if (!token) {
      return null;
    }

    const assistant =
      (await this.profileLoader.loadDynamicAssistant()) as AssistantUnrolled | null;

    if (!assistant) {
      Logger.warn("[RICA] Dynamic config returned no models");
      return null;
    }

    // v1.2.0 parity — also cache the full EntraIDDynamicConfig block so
    // core/config/load.ts can merge backend-driven systemMessage / ui /
    // experimental / context-providers / etc. into the runtime config.
    // The fetch shares the same SQLite drain + POST body as
    // loadDynamicAssistant via fetchRicaDynamicConfig — but each call
    // hits the endpoint independently, so we re-use the fact that
    // loadDynamicEntraIDConfig is called immediately after assistant load
    // (the dynamic-config snapshot is small; backend round-trip is the
    // dominant cost and the refork CACHE_TTL_MS already amortises that).
    try {
      this.cachedEntraIDDynamicConfig =
        (await this.profileLoader.loadDynamicEntraIDConfig()) ?? null;
    } catch (err) {
      Logger.warn("[RICA] Failed to load EntraIDDynamicConfig", err);
      this.cachedEntraIDDynamicConfig = null;
    }

    return {
      configResult: {
        config: assistant,
        errors: [],
        configLoadInterrupted: false,
      },
      ownerSlug: "rica",
      packageSlug: "dynamic-models",
      iconUrl: "",
      rawYaml: "",
    };
  }

  /**
   * Returns the most recently fetched EntraIDDynamicConfig block, or null
   * if none has been fetched yet. Callers (core/config/load.ts) read this
   * to merge backend dynamic fields into the runtime ContinueConfig.
   * Mirrors v1.2.0's `EntraIDDynamicConfigService.fetchDynamicConfig` access
   * pattern minus the fetch — fetch is initiated by buildDynamicAssistant
   * so we don't double-hit the endpoint.
   */
  public getCachedEntraIDDynamicConfig(): EntraIDDynamicConfig | null {
    return this.cachedEntraIDDynamicConfig;
  }

  private async getDynamicAssistant(): Promise<DynamicAssistantEntry | null> {
    const now = Date.now();
    if (
      this.cachedAssistant &&
      now - this.cacheTimestamp < RicaControlPlaneClient.CACHE_TTL_MS
    ) {
      return this.cachedAssistant;
    }

    if (this.inflightPromise) {
      return this.inflightPromise;
    }

    this.inflightPromise = this.buildDynamicAssistant()
      .then((result) => {
        this.cachedAssistant = result;
        this.cacheTimestamp = Date.now();
        return result;
      })
      .finally(() => {
        this.inflightPromise = undefined;
      });

    return this.inflightPromise;
  }

  public invalidateCache(): void {
    this.cachedAssistant = null;
    this.cacheTimestamp = 0;
    this.cachedEntraIDDynamicConfig = null;
    // Force the next getAccessToken() to re-validate (ensureValidToken is a
    // no-op when the MDP JWT is still valid, so this is cheap).
    this.cachedMdpToken = undefined;
    this.cachedMdpExpMs = 0;
  }

  public async shouldEnableRemoteSessions(): Promise<boolean> {
    return false;
  }

  public async listRemoteSessions(): Promise<RemoteSessionMetadata[]> {
    return [];
  }

  public async getCreditStatus(): Promise<CreditStatus | null> {
    return null;
  }

  public async listOrganizations(): Promise<Array<OrganizationDescription>> {
    try {
      const dynamicAssistant = await this.getDynamicAssistant();
      if (!dynamicAssistant) {
        return [];
      }

      return [
        {
          id: "rica-dynamic",
          iconUrl: "",
          name: "Renesas",
          slug: "renesas",
        },
      ];
    } catch (error) {
      Logger.error(error, {
        context: "rica_dynamic_config_list_organizations",
      });
      return [];
    }
  }

  public async listAssistants(
    _organizationId: string | null,
  ): Promise<DynamicAssistantEntry[]> {
    try {
      const dynamicAssistant = await this.getDynamicAssistant();
      return dynamicAssistant ? [dynamicAssistant] : [];
    } catch (error) {
      Logger.error(error, {
        context: "rica_dynamic_config_list_assistants",
      });
      return [];
    }
  }

  public async getPolicy(): Promise<PolicyResponse | null> {
    return null;
  }
}
