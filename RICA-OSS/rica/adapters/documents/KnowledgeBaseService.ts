import fetch from "node-fetch";

import { KnowledgeBase, KnowledgeBaseListResponse } from "./types";
import { getControlPlaneEnv, IdeSettings } from "../config/env";

export class KnowledgeBaseService {
  private static readonly REQUEST_TIMEOUT = 15000;
  private static cachedKnowledgeBases: KnowledgeBase[] | null = null;
  private static lastFetchTime: number = 0;
  private static readonly CACHE_DURATION = 5 * 60 * 1000;

  constructor(
    private readonly getEntraIDToken: () => Promise<string | undefined>,
    private readonly ideSettingsPromise: Promise<IdeSettings>,
  ) {}

  private async getKnowledgeBaseListUrl(
    page: number = 1,
    limit: number = 100,
  ): Promise<string> {
    const env = await getControlPlaneEnv(this.ideSettingsPromise);
    const baseUrl = env.APP_URL.replace(/\/$/, "");
    return `${baseUrl}/api/knowledge?page=${page}&limit=${limit}&sort_field=last_modified&sort_direction=desc`;
  }

  async listKnowledgeBases(
    page: number = 1,
    limit: number = 100,
    useCache: boolean = true,
  ): Promise<KnowledgeBaseListResponse> {
    if (
      page === 1 &&
      useCache &&
      KnowledgeBaseService.cachedKnowledgeBases &&
      Date.now() - KnowledgeBaseService.lastFetchTime <
        KnowledgeBaseService.CACHE_DURATION
    ) {
      return {
        data: KnowledgeBaseService.cachedKnowledgeBases,
        pagination: {
          page: 1,
          limit: KnowledgeBaseService.cachedKnowledgeBases.length,
          total: KnowledgeBaseService.cachedKnowledgeBases.length,
        },
      };
    }

    return await this.fetchKnowledgeBasesFromBackend(page, limit);
  }

  async refreshKnowledgeBases(
    page: number = 1,
    limit: number = 100,
  ): Promise<KnowledgeBaseListResponse> {
    return await this.fetchKnowledgeBasesFromBackend(page, limit);
  }

  private async fetchKnowledgeBasesFromBackend(
    page: number,
    limit: number,
  ): Promise<KnowledgeBaseListResponse> {
    try {
      const entraToken = await this.getEntraIDToken();
      if (!entraToken) {
        return {
          data: [],
          pagination: { page, limit, total: 0 },
          error: "Entra ID token not available. Please sign in.",
        };
      }

      const url = await this.getKnowledgeBaseListUrl(page, limit);

      const controller = new AbortController();
      const timeoutId = setTimeout(
        () => controller.abort(),
        KnowledgeBaseService.REQUEST_TIMEOUT,
      );

      const response = await fetch(url, {
        method: "GET",
        headers: {
          Authorization: `Bearer ${entraToken}`,
          "Content-Type": "application/json",
          Accept: "application/json",
        },
        signal: controller.signal,
      });

      clearTimeout(timeoutId);

      if (!response.ok) {
        const errorText = await response.text();
        console.error(
          `KnowledgeBaseService: HTTP ${response.status} - ${errorText}`,
        );
        return {
          data: [],
          pagination: { page, limit, total: 0 },
          error: `Failed to fetch knowledge bases: ${response.status}`,
        };
      }

      const data = (await response.json()) as any;
      const knowledgeBases = this.parseResponse(data);

      if (page === 1) {
        KnowledgeBaseService.cachedKnowledgeBases = knowledgeBases;
        KnowledgeBaseService.lastFetchTime = Date.now();
      }

      return {
        data: knowledgeBases,
        pagination: data.pagination || {
          page,
          limit,
          total: knowledgeBases.length,
        },
      };
    } catch (error) {
      console.error(
        "KnowledgeBaseService: Failed to fetch knowledge bases",
        error,
      );
      return {
        data: [],
        pagination: { page, limit, total: 0 },
        error: `Failed to fetch knowledge bases: ${error}`,
      };
    }
  }

  private parseResponse(data: any): KnowledgeBase[] {
    let items: any[] = [];

    if (Array.isArray(data)) {
      items = data;
    } else if (data.data && Array.isArray(data.data)) {
      items = data.data;
    } else {
      return [];
    }

    return items
      .filter((item) => item && typeof item === "object" && item.id)
      .map((item) => ({
        id: String(item.id),
        name: item.name || "Unnamed",
        created_at: item.created_at || "",
        owner: item.owner || "",
        modified: item.modified || "",
        member_count: item.member_count || 0,
        file_count: item.file_count || 0,
        files: item.files || [],
        is_owner: item.is_owner || false,
      }));
  }

  clearCache(): void {
    KnowledgeBaseService.cachedKnowledgeBases = null;
    KnowledgeBaseService.lastFetchTime = 0;
  }

  getKnowledgeBaseById(kbId: string): KnowledgeBase | undefined {
    return KnowledgeBaseService.cachedKnowledgeBases?.find(
      (kb) => kb.id === kbId,
    );
  }
}
