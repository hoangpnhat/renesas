import fetch from "node-fetch";

import { DocumentListResponse, RicaDocument } from "./types";
import { getControlPlaneEnv, IdeSettings } from "../config/env";

export class DocumentService {
  private static readonly REQUEST_TIMEOUT = 15000;
  private static cachedDocuments: RicaDocument[] | null = null;
  private static lastFetchTime: number = 0;
  private static readonly CACHE_DURATION = 5 * 60 * 1000;

  constructor(
    private readonly getEntraIDToken: () => Promise<string | undefined>,
    private readonly ideSettingsPromise: Promise<IdeSettings>,
  ) {}

  private async getDocumentListUrl(): Promise<string> {
    const env = await getControlPlaneEnv(this.ideSettingsPromise);
    const baseUrl = env.APP_URL.replace(/\/$/, "");
    return `${baseUrl}/api/file?sort_logic=last_modified:desc&page=1&limit=100`;
  }

  async listDocuments(useCache: boolean = true): Promise<DocumentListResponse> {
    if (
      useCache &&
      DocumentService.cachedDocuments &&
      Date.now() - DocumentService.lastFetchTime <
        DocumentService.CACHE_DURATION
    ) {
      return {
        documents: DocumentService.cachedDocuments,
        total: DocumentService.cachedDocuments.length,
      };
    }
    return await this.fetchDocumentsFromBackend();
  }

  async refreshDocuments(): Promise<DocumentListResponse> {
    return await this.fetchDocumentsFromBackend();
  }

  private async fetchDocumentsFromBackend(): Promise<DocumentListResponse> {
    try {
      const entraToken = await this.getEntraIDToken();
      if (!entraToken) {
        return {
          documents: [],
          error: "Entra ID token not available. Please sign in.",
        };
      }

      const documentListUrl = await this.getDocumentListUrl();

      const controller = new AbortController();
      const timeoutId = setTimeout(
        () => controller.abort(),
        DocumentService.REQUEST_TIMEOUT,
      );

      const response = await fetch(documentListUrl, {
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
          `DocumentService: HTTP ${response.status} - ${errorText}`,
        );
        return {
          documents: [],
          error: `Failed to fetch documents: ${response.status}`,
        };
      }

      const responseText = await response.text();

      if (
        responseText.trim().startsWith("<!DOCTYPE") ||
        responseText.trim().startsWith("<html")
      ) {
        return {
          documents: [],
          error: "API endpoint not available (received HTML response)",
        };
      }

      let data: any;
      try {
        data = JSON.parse(responseText);
      } catch {
        return {
          documents: [],
          error: "Invalid JSON response from server",
        };
      }

      const documents = this.parseDocumentListResponse(data);

      DocumentService.cachedDocuments = documents;
      DocumentService.lastFetchTime = Date.now();

      const total = data.pagination?.total || documents.length;
      return { documents, total };
    } catch (error) {
      console.error("DocumentService: Failed to fetch documents", error);
      return {
        documents: [],
        error: `Failed to fetch documents: ${error}`,
      };
    }
  }

  private parseDocumentListResponse(data: any): RicaDocument[] {
    let documentsArray: any[] = [];

    if (Array.isArray(data)) {
      documentsArray = data;
    } else if (data.documents && Array.isArray(data.documents)) {
      documentsArray = data.documents;
    } else if (data.data && Array.isArray(data.data)) {
      documentsArray = data.data;
    } else {
      return [];
    }

    const documents: RicaDocument[] = [];

    for (const item of documentsArray) {
      if (!item || typeof item !== "object") continue;

      const id = item.id;
      const name = item.name || item.file_name || item.fileName;
      const path = item.path || item.folder_path || item.folderPath;

      if (!id || !name || !path) continue;

      const status = item.status || "available";
      if (status !== "completed") continue;

      documents.push({
        id: String(id),
        name: String(name),
        path: String(path),
        uploadedAt: item.uploadedAt || item.uploaded_at || item.createdAt,
        status,
        total_pages: item.total_pages,
        metadata: item.metadata || {},
      });
    }

    return documents;
  }

  clearCache(): void {
    DocumentService.cachedDocuments = null;
    DocumentService.lastFetchTime = 0;
  }

  getDocumentById(docId: string): RicaDocument | undefined {
    return DocumentService.cachedDocuments?.find((doc) => doc.id === docId);
  }
}
