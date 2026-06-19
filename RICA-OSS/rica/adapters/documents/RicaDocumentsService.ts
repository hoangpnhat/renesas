import { IDE } from "../../../core";
import type {
  DocumentListResponse,
  ElasticsearchQueryRequest,
  ElasticsearchQueryResponse,
  KnowledgeBaseListResponse,
  RicaDocument,
} from "./types";
import { IdeSettings } from "../config/env";
import { DocumentService } from "./DocumentService";
import { ElasticsearchService } from "./ElasticsearchService";
import { ImageService } from "./ImageService";
import { KnowledgeBaseService } from "./KnowledgeBaseService";

export class RicaDocumentsService {
  private static instance: RicaDocumentsService | null = null;

  private documentService: DocumentService;
  private elasticsearchService: ElasticsearchService;
  private imageService: ImageService;
  private knowledgeBaseService: KnowledgeBaseService;

  private constructor(
    getEntraIDToken: () => Promise<string | undefined>,
    getMdpToken: () => Promise<string | undefined>,
    ide: IDE,
    ideSettingsPromise: Promise<IdeSettings>,
  ) {
    this.documentService = new DocumentService(
      getEntraIDToken,
      ideSettingsPromise,
    );
    this.elasticsearchService = new ElasticsearchService(
      getMdpToken,
      getEntraIDToken,
      ideSettingsPromise,
    );
    this.imageService = new ImageService(getMdpToken, ide, ideSettingsPromise);
    this.knowledgeBaseService = new KnowledgeBaseService(
      getEntraIDToken,
      ideSettingsPromise,
    );
  }

  static createInstance(
    getEntraIDToken: () => Promise<string | undefined>,
    getMdpToken: () => Promise<string | undefined>,
    ide: IDE,
    ideSettingsPromise: Promise<IdeSettings>,
  ): RicaDocumentsService {
    if (!RicaDocumentsService.instance) {
      RicaDocumentsService.instance = new RicaDocumentsService(
        getEntraIDToken,
        getMdpToken,
        ide,
        ideSettingsPromise,
      );
    }
    return RicaDocumentsService.instance;
  }

  static getInstance(): RicaDocumentsService | null {
    return RicaDocumentsService.instance;
  }

  async listDocuments(): Promise<DocumentListResponse> {
    return await this.documentService.listDocuments();
  }

  async refreshDocuments(): Promise<DocumentListResponse> {
    return await this.documentService.refreshDocuments();
  }

  async queryDocuments(
    request: ElasticsearchQueryRequest,
  ): Promise<ElasticsearchQueryResponse> {
    return await this.elasticsearchService.query(request);
  }

  async queryByPageNumbers(
    docId: string,
    pageNumbers: number[],
  ): Promise<ElasticsearchQueryResponse> {
    return await this.elasticsearchService.queryByPageNumbers(
      docId,
      pageNumbers,
    );
  }

  async getIndexMapping(): Promise<any> {
    return await this.elasticsearchService.getIndexMapping();
  }

  async getSampleDocuments(size?: number): Promise<any> {
    return await this.elasticsearchService.getSampleDocuments(size);
  }

  getDocumentById(docId: string): RicaDocument | undefined {
    return this.documentService.getDocumentById(docId);
  }

  async fetchImageAsBase64(imagePath: string): Promise<string | undefined> {
    return await this.imageService.fetchImageAsBase64(imagePath);
  }

  async fetchImagesAsBase64(
    imagePaths: string[],
  ): Promise<Map<string, string>> {
    return await this.imageService.fetchImagesAsBase64(imagePaths);
  }

  async fetchImagesWithLocalPaths(
    imagePaths: string[],
  ): Promise<Map<string, { localPath: string; base64: string }>> {
    return await this.imageService.fetchImagesWithLocalPaths(imagePaths);
  }

  async listKnowledgeBases(
    page?: number,
    limit?: number,
    useCache?: boolean,
  ): Promise<KnowledgeBaseListResponse> {
    return await this.knowledgeBaseService.listKnowledgeBases(
      page,
      limit,
      useCache,
    );
  }

  async refreshKnowledgeBases(
    page?: number,
    limit?: number,
  ): Promise<KnowledgeBaseListResponse> {
    return await this.knowledgeBaseService.refreshKnowledgeBases(page, limit);
  }

  getKnowledgeBaseById(kbId: string) {
    return this.knowledgeBaseService.getKnowledgeBaseById(kbId);
  }

  clearCache() {
    this.documentService.clearCache();
    this.knowledgeBaseService.clearCache();
  }
}
