import type { ControlPlaneSessionInfo } from "../../../core/control-plane/AuthTypes.js";
import { ControlPlaneClient } from "../../../core/control-plane/client.js";
import { Core } from "../../../core/core.js";
import type { IDE } from "../../../core/index.js";
import { Providers } from "../../../core/context/providers/index.js";
import { LLMClasses } from "../../../core/llm/llms/index.js";
import type {
  FromCoreProtocol,
  ToCoreProtocol,
} from "../../../core/protocol/index.js";
import type { IMessenger } from "../../../core/protocol/messenger/index.js";
import {
  getControlPlaneSessionInfo,
  getEntraIDAccessToken,
} from "../auth/EntraIDAuthProvider.js";
import { RicaControlPlaneClient } from "../control-plane/RicaControlPlaneClient.js";
import RicaDocsContextProvider from "../documents/RicaDocsContextProvider.js";
import { RicaDocumentsService } from "../documents/RicaDocumentsService.js";
import RicaProxy from "../llm/RicaProxy.js";

export class RicaCore extends Core {
  // 60s to match v1.2.0/v1.2.1 (ConfigHandler.DYNAMIC_CONFIG_REFRESH_INTERVAL_MS
  // = 1000 * 60). Refork had bumped this to 5 min; restored to v1.2.0 parity so
  // consumption/lock state surfaces within ~1 refresh cycle.
  private static readonly DYNAMIC_CONFIG_REFRESH_INTERVAL_MS = 60 * 1000;
  private dynamicConfigRefreshInterval?: ReturnType<typeof setInterval>;
  private dynamicConfigRefreshInFlight = false;
  private readonly ricaIde: IDE;
  private readonly ricaMessenger: IMessenger<ToCoreProtocol, FromCoreProtocol>;
  private ricaDocumentsService: RicaDocumentsService | null = null;

  constructor(
    messenger: IMessenger<ToCoreProtocol, FromCoreProtocol>,
    ide: IDE,
  ) {
    super(messenger, ide);
    this.ricaIde = ide;
    this.ricaMessenger = messenger;

    RicaCore.registerRicaProviders();
    this.installControlPlaneClientTrap();
    this.startDynamicConfigAutoRefresh();
    this.initRicaDocuments();
    this.registerRicaDocumentHandlers();
  }

  private static providersRegistered = false;

  private static registerRicaProviders() {
    if (RicaCore.providersRegistered) {
      return;
    }
    if (!LLMClasses.some((cls) => cls.providerName === "rica-proxy")) {
      LLMClasses.push(RicaProxy);
    }
    if (!Providers.some((cls) => cls.description.title === "rica-docs")) {
      Providers.push(RicaDocsContextProvider as any);
    }
    RicaCore.providersRegistered = true;
  }

  /**
   * Uses a property descriptor trap on configHandler.controlPlaneClient.
   * When upstream code (e.g. updateControlPlaneSessionInfo) assigns a vanilla
   * ControlPlaneClient, the setter intercepts it and creates a
   * RicaControlPlaneClient instead — ensuring cascadeInit always uses RICA.
   */
  private installControlPlaneClientTrap() {
    const ide = this.ricaIde;
    const currentClient = this.configHandler.controlPlaneClient;
    const entraGetter = () => getEntraIDAccessToken(true);
    // Always-fresh MDP token source. getControlPlaneSessionInfo runs
    // ensureValidToken() under the hood — a no-network no-op while the MDP
    // JWT is still valid, and a refresh_token re-mint once it expires. This
    // is what RicaControlPlaneClient.getAccessToken() drains to keep
    // model.apiKey current and stop the recurring "401 Token is expired".
    // (silent=true → never pops a sign-in dialog from a background tick.)
    const mdpGetter = async (): Promise<string | undefined> => {
      const sessionInfo = await getControlPlaneSessionInfo(true, false);
      return sessionInfo && "accessToken" in sessionInfo
        ? (sessionInfo as { accessToken?: string }).accessToken
        : undefined;
    };
    let ricaClient = new RicaControlPlaneClient(
      currentClient.sessionInfoPromise,
      ide,
      entraGetter,
      mdpGetter,
    );

    Object.defineProperty(this.configHandler, "controlPlaneClient", {
      get() {
        return ricaClient;
      },
      set(newClient: ControlPlaneClient) {
        ricaClient = new RicaControlPlaneClient(
          newClient.sessionInfoPromise,
          ide,
          entraGetter,
          mdpGetter,
        );
      },
      configurable: true,
      enumerable: true,
    });
  }

  private startDynamicConfigAutoRefresh() {
    if (this.dynamicConfigRefreshInterval) {
      clearInterval(this.dynamicConfigRefreshInterval);
    }

    this.dynamicConfigRefreshInterval = setInterval(() => {
      void this.refreshDynamicConfig();
    }, RicaCore.DYNAMIC_CONFIG_REFRESH_INTERVAL_MS);
  }

  private async refreshDynamicConfig() {
    if (this.dynamicConfigRefreshInFlight) {
      return;
    }

    this.dynamicConfigRefreshInFlight = true;
    try {
      const client = this.configHandler.controlPlaneClient;
      if (client instanceof RicaControlPlaneClient) {
        client.invalidateCache();
      }
      await this.configHandler.reloadConfig(
        "RICA dynamic config auto-refresh interval",
      );
      console.warn(
        "[RICA-REFRESH]",
        `dynamic config auto-refresh OK (interval=${RicaCore.DYNAMIC_CONFIG_REFRESH_INTERVAL_MS}ms)`,
      );
    } catch (e) {
      // Background refresh must never crash the extension — but it must NOT
      // fail silently either. A swallowed error here (token expiry, network,
      // backend 4xx) looks identical to "UI frozen / consumption not
      // updating", with no way to diagnose live. Log it so the debug log
      // shows whether the 60s tick is firing and succeeding.
      console.warn(
        "[RICA-REFRESH]",
        `dynamic config auto-refresh FAILED: ${e instanceof Error ? e.message : String(e)}`,
      );
    } finally {
      this.dynamicConfigRefreshInFlight = false;
    }
  }

  private initRicaDocuments() {
    const entraGetter = () => getEntraIDAccessToken(true);
    const getMdpToken = async (): Promise<string | undefined> => {
      try {
        const sessionInfo = await this.ricaMessenger.request(
          "getControlPlaneSessionInfo",
          { silent: true, useOnboarding: false },
        );
        if (sessionInfo && "accessToken" in sessionInfo) {
          return (sessionInfo as any).accessToken;
        }
        return undefined;
      } catch {
        return undefined;
      }
    };

    const ideSettingsPromise = this.ricaMessenger.request(
      "getIdeSettings",
      undefined,
    );

    this.ricaDocumentsService = RicaDocumentsService.createInstance(
      entraGetter,
      getMdpToken,
      this.ricaIde,
      ideSettingsPromise,
    );
  }

  private registerRicaDocumentHandlers() {
    const on = this.ricaMessenger.on.bind(this.ricaMessenger);

    on("ricaDocs/list", async () => {
      if (!this.ricaDocumentsService) {
        return { documents: [], error: "Document service not initialized" };
      }
      return await this.ricaDocumentsService.listDocuments();
    });

    on("ricaDocs/refresh", async () => {
      if (!this.ricaDocumentsService) {
        return { documents: [], error: "Document service not initialized" };
      }
      return await this.ricaDocumentsService.refreshDocuments();
    });

    on("ricaDocs/query", async (msg) => {
      if (!this.ricaDocumentsService) {
        return { results: [], error: "Document service not initialized" };
      }
      return await this.ricaDocumentsService.queryDocuments(msg.data);
    });

    on("knowledgeBase/list", async (msg) => {
      if (!this.ricaDocumentsService) {
        return {
          data: [],
          pagination: { page: 1, limit: 0, total: 0 },
          error: "Knowledge base service not initialized",
        };
      }
      const { page, limit } = msg.data || {};
      return await this.ricaDocumentsService.listKnowledgeBases(page, limit);
    });

    on("knowledgeBase/refresh", async (msg) => {
      if (!this.ricaDocumentsService) {
        return {
          data: [],
          pagination: { page: 1, limit: 0, total: 0 },
          error: "Knowledge base service not initialized",
        };
      }
      const { page, limit } = msg.data || {};
      return await this.ricaDocumentsService.refreshKnowledgeBases(page, limit);
    });
  }

  public async shutdown(): Promise<void> {
    if (this.dynamicConfigRefreshInterval) {
      clearInterval(this.dynamicConfigRefreshInterval);
      this.dynamicConfigRefreshInterval = undefined;
    }
  }
}
