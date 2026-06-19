import * as fs from "node:fs";
import {
  getLocalEnvironmentDotFilePath,
  getStagingEnvironmentDotFilePath,
} from "../../utils/paths";
import type { IdeSettings } from "../config/env";

export type ServiceEnvironment =
  | "production"
  | "staging"
  | "local"
  | "test"
  | "none";

export interface ElasticsearchEndpointConfig {
  endpoint: string;
  apiKey: string;
  indexName: string;
  vectorFieldName: string;
}

export interface DatabricksEndpointConfig {
  endpoint: string;
  embeddingEndpoint: string;
  embeddingModel: string;
}

export const ELASTICSEARCH_ENDPOINTS: Record<
  string,
  ElasticsearchEndpointConfig
> = {
  production: {
    endpoint:
      "https://es-ai-prod-coding-001-d9f587.es.eastus.azure.elastic.cloud:443",
    apiKey: "QzlyOENKd0JJTHZfVlhkTXRBM0M6bmtPX3MzRk53a2pIcUJUbFRERGRWUQ==",
    indexName: "user_documents",
    vectorFieldName: "multimodal_vector",
  },
  staging: {
    endpoint:
      "https://es-ai-nonprod-coding-001-cd8a1b.es.eastus.azure.elastic.cloud:443",
    apiKey: "eFU4b1Jwc0ItaHR3ZW10ZV9wN1M6Qld6WGh2ZENvbUZWUkFzLXFHRW5YQQ==",
    indexName: "multimodal-model",
    vectorFieldName: "multimodal_vector",
  },
};

ELASTICSEARCH_ENDPOINTS.local = ELASTICSEARCH_ENDPOINTS.staging;
ELASTICSEARCH_ENDPOINTS.test = ELASTICSEARCH_ENDPOINTS.staging;
ELASTICSEARCH_ENDPOINTS.none = ELASTICSEARCH_ENDPOINTS.production;

export const DATABRICKS_ENDPOINTS: Record<string, DatabricksEndpointConfig> = {
  production: {
    endpoint: "https://adb-4265374015564621.1.azuredatabricks.net",
    embeddingEndpoint:
      "https://adb-4265374015564621.1.azuredatabricks.net/serving-endpoints/",
    embeddingModel: "ai-prd-coding-jina-query",
  },
  staging: {
    endpoint: "https://adb-893717583776115.15.azuredatabricks.net",
    embeddingEndpoint:
      "https://adb-893717583776115.15.azuredatabricks.net/serving-endpoints/",
    embeddingModel: "jina-embeddings-v4-query",
  },
};

DATABRICKS_ENDPOINTS.local = DATABRICKS_ENDPOINTS.staging;
DATABRICKS_ENDPOINTS.test = DATABRICKS_ENDPOINTS.staging;
DATABRICKS_ENDPOINTS.none = DATABRICKS_ENDPOINTS.production;

export function getElasticsearchConfig(
  env: ServiceEnvironment,
): ElasticsearchEndpointConfig {
  return ELASTICSEARCH_ENDPOINTS[env] || ELASTICSEARCH_ENDPOINTS.production;
}

export function getDatabricksConfig(
  env: ServiceEnvironment,
): DatabricksEndpointConfig {
  return DATABRICKS_ENDPOINTS[env] || DATABRICKS_ENDPOINTS.production;
}

export function resolveServiceEnvironment(
  ideTestEnvironment: IdeSettings["continueTestEnvironment"],
): ServiceEnvironment {
  if (fs.existsSync(getLocalEnvironmentDotFilePath())) {
    return "local";
  }

  if (fs.existsSync(getStagingEnvironmentDotFilePath())) {
    return "staging";
  }

  if (ideTestEnvironment === "production") return "production";
  if (ideTestEnvironment === "staging") return "staging";
  if (ideTestEnvironment === "local") return "local";
  if (ideTestEnvironment === "test") return "test";
  if (ideTestEnvironment === "none") return "none";

  const envVar = process.env.CONTROL_PLANE_ENV;
  if (envVar === "staging") return "staging";
  if (envVar === "local") return "local";
  if (envVar === "test") return "test";

  return "production";
}
