import fetch from "node-fetch";

import {
  ElasticsearchQueryRequest,
  ElasticsearchQueryResponse,
  ElasticsearchResult,
} from "./types";
import type { IdeSettings } from "../config/env";
import { EmbeddingService } from "./EmbeddingService";
import {
  getElasticsearchConfig,
  resolveServiceEnvironment,
  type ServiceEnvironment,
} from "./ServiceEndpoints";

export interface ElasticsearchConfig {
  endpoint: string;
  indexName: string;
  apiKey?: string;
  vectorFieldName?: string;
  useHybridSearch?: boolean;
}

export class ElasticsearchService {
  private static readonly REQUEST_TIMEOUT = 30000;

  private embeddingService: EmbeddingService;
  private ideSettingsPromise: Promise<IdeSettings>;

  constructor(
    private readonly getMdpToken: () => Promise<string | undefined>,
    private readonly getEntraIDToken: () => Promise<string | undefined>,
    ideSettingsPromise: Promise<IdeSettings>,
    private readonly config?: ElasticsearchConfig,
  ) {
    this.ideSettingsPromise = ideSettingsPromise;
    this.embeddingService = new EmbeddingService(
      getMdpToken,
      ideSettingsPromise,
    );
  }

  private async getEnvironment(): Promise<ServiceEnvironment> {
    const settings = await this.ideSettingsPromise;
    return resolveServiceEnvironment(settings.continueTestEnvironment);
  }

  async query(
    request: ElasticsearchQueryRequest,
  ): Promise<ElasticsearchQueryResponse> {
    try {
      const env = await this.getEnvironment();
      const envConfig = getElasticsearchConfig(env);

      const endpoint = this.config?.endpoint || envConfig.endpoint;
      const indexName = this.config?.indexName || envConfig.indexName;
      const apiKey = this.config?.apiKey || envConfig.apiKey;
      const vectorFieldName =
        this.config?.vectorFieldName || envConfig.vectorFieldName;
      const useHybridSearch = this.config?.useHybridSearch ?? true;

      let queryEmbedding: number[] | number[][] | undefined;
      if (useHybridSearch) {
        queryEmbedding = await this.embeddingService.embed(request.query);
        if (!queryEmbedding) {
          console.warn(
            "ElasticsearchService: Failed to generate embedding, falling back to text-only search",
          );
        }
      }

      const esQuery = queryEmbedding
        ? this.buildHybridSearchQuery(request, queryEmbedding, vectorFieldName)
        : this.buildTextSearchQuery(request);

      const controller = new AbortController();
      const timeoutId = setTimeout(
        () => controller.abort(),
        ElasticsearchService.REQUEST_TIMEOUT,
      );

      const response = await fetch(`${endpoint}/${indexName}/_search`, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: `ApiKey ${apiKey}`,
        },
        body: JSON.stringify(esQuery),
        signal: controller.signal,
      });

      clearTimeout(timeoutId);

      if (!response.ok) {
        const errorText = await response.text();
        console.error(
          `ElasticsearchService: HTTP ${response.status} - ${errorText}`,
        );
        return {
          results: [],
          error: `Elasticsearch query failed: ${response.status}`,
        };
      }

      const data = (await response.json()) as any;
      const results = this.parseElasticsearchResponse(data);

      return { results };
    } catch (error) {
      console.error("ElasticsearchService: Query failed", error);
      return {
        results: [],
        error: `Elasticsearch query failed: ${error}`,
      };
    }
  }

  async queryByPageNumbers(
    docId: string,
    pageNumbers: number[],
  ): Promise<ElasticsearchQueryResponse> {
    try {
      const env = await this.getEnvironment();
      const envConfig = getElasticsearchConfig(env);

      const endpoint = this.config?.endpoint || envConfig.endpoint;
      const indexName = this.config?.indexName || envConfig.indexName;
      const apiKey = this.config?.apiKey || envConfig.apiKey;

      const esQuery = {
        size: pageNumbers.length,
        query: {
          bool: {
            filter: [
              { term: { "metadata.file_id": docId } },
              { terms: { "metadata.page_num": pageNumbers } },
            ],
          },
        },
        _source: [
          "content",
          "image_ids",
          "image_paths",
          "volume_path",
          "metadata",
          "doc_id",
        ],
        sort: [{ "metadata.page_num": { order: "asc" } }],
      };

      const controller = new AbortController();
      const timeoutId = setTimeout(
        () => controller.abort(),
        ElasticsearchService.REQUEST_TIMEOUT,
      );

      const response = await fetch(`${endpoint}/${indexName}/_search`, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: `ApiKey ${apiKey}`,
        },
        body: JSON.stringify(esQuery),
        signal: controller.signal,
      });

      clearTimeout(timeoutId);

      if (!response.ok) {
        const errorText = await response.text();
        console.error(
          `ElasticsearchService: HTTP ${response.status} - ${errorText}`,
        );
        return {
          results: [],
          error: `Elasticsearch page query failed: ${response.status}`,
        };
      }

      const data = (await response.json()) as any;
      return { results: this.parseElasticsearchResponse(data) };
    } catch (error) {
      console.error("ElasticsearchService: Page query failed", error);
      return {
        results: [],
        error: `Elasticsearch page query failed: ${error}`,
      };
    }
  }

  private buildHybridSearchQuery(
    request: ElasticsearchQueryRequest,
    queryEmbedding: number[] | number[][],
    vectorField: string,
  ): Record<string, any> {
    const topK = request.topK || 30;

    const filters: any[] = [];
    if (request.docIds && request.docIds.length > 0) {
      filters.push({
        terms: { "metadata.file_id": request.docIds },
      });
    }

    return {
      size: topK,
      query: {
        script_score: {
          query: {
            bool: {
              filter: filters.length > 0 ? filters : { match_all: {} },
              must: [{ exists: { field: vectorField } }],
            },
          },
          script: {
            source: `
              if (!doc.containsKey('${vectorField}') || doc['${vectorField}'].size() == 0) {
                return 0.0;
              }
              return maxSimDotProduct(params.query_vector, '${vectorField}');
            `,
            params: {
              query_vector: queryEmbedding,
            },
          },
        },
      },
      _source: [
        "content",
        "image_ids",
        "image_paths",
        "volume_path",
        "metadata",
        "doc_id",
      ],
    };
  }

  private buildTextSearchQuery(
    request: ElasticsearchQueryRequest,
  ): Record<string, any> {
    const topK = request.topK || 30;

    const filters: any[] = [];
    if (request.docIds && request.docIds.length > 0) {
      filters.push({
        terms: { "metadata.file_id": request.docIds },
      });
    }

    return {
      size: topK,
      query: {
        bool: {
          must: [
            {
              multi_match: {
                query: request.query,
                fields: ["content^2", "title"],
                type: "best_fields",
                fuzziness: "AUTO",
              },
            },
          ],
          filter: filters,
        },
      },
      _source: [
        "content",
        "image_ids",
        "image_paths",
        "volume_path",
        "metadata",
        "doc_id",
      ],
      sort: [{ _score: { order: "desc" } }],
    };
  }

  private parseElasticsearchResponse(esResponse: any): ElasticsearchResult[] {
    if (!esResponse.hits?.hits) {
      return [];
    }

    return esResponse.hits.hits.map((hit: any) => {
      const source = hit._source || {};

      let imagePaths: string[] = [];
      if (source.metadata?.volume_path) {
        imagePaths = [source.metadata.volume_path];
      } else if (source.volume_path) {
        imagePaths = [source.volume_path];
      } else if (source.image_paths && Array.isArray(source.image_paths)) {
        imagePaths = source.image_paths;
      }

      return {
        id: source.doc_id || hit._id,
        content: source.content || "",
        imageIds: source.image_ids || [],
        imagePaths,
        metadata: source.metadata || {},
        score: hit._score,
      };
    });
  }

  async getIndexMapping(): Promise<any> {
    try {
      const env = await this.getEnvironment();
      const envConfig = getElasticsearchConfig(env);

      const endpoint = this.config?.endpoint || envConfig.endpoint;
      const indexName = this.config?.indexName || envConfig.indexName;
      const apiKey = this.config?.apiKey || envConfig.apiKey;

      const response = await fetch(`${endpoint}/${indexName}/_mapping`, {
        headers: { Authorization: `ApiKey ${apiKey}` },
      });

      if (!response.ok) return null;
      return await response.json();
    } catch (error) {
      console.error("ElasticsearchService: getIndexMapping failed", error);
      return null;
    }
  }

  async getSampleDocuments(size: number = 5): Promise<any> {
    try {
      const env = await this.getEnvironment();
      const envConfig = getElasticsearchConfig(env);

      const endpoint = this.config?.endpoint || envConfig.endpoint;
      const indexName = this.config?.indexName || envConfig.indexName;
      const apiKey = this.config?.apiKey || envConfig.apiKey;

      const response = await fetch(`${endpoint}/${indexName}/_search`, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: `ApiKey ${apiKey}`,
        },
        body: JSON.stringify({ size, query: { match_all: {} } }),
      });

      if (!response.ok) return null;
      return await response.json();
    } catch (error) {
      console.error("ElasticsearchService: getSampleDocuments failed", error);
      return null;
    }
  }
}
