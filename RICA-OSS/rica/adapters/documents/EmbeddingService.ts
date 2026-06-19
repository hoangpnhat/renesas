import fetch from "node-fetch";
import type { IdeSettings } from "../config/env";
import {
  getDatabricksConfig,
  resolveServiceEnvironment,
  type ServiceEnvironment,
} from "./ServiceEndpoints";

export class EmbeddingService {
  private static readonly REQUEST_TIMEOUT = 30000;

  constructor(
    private readonly getMdpToken: () => Promise<string | undefined>,
    private readonly ideSettingsPromise: Promise<IdeSettings>,
    private readonly config?: {
      endpoint?: string;
      modelName?: string;
    },
  ) {}

  private async getEnvironment(): Promise<ServiceEnvironment> {
    const settings = await this.ideSettingsPromise;
    return resolveServiceEnvironment(settings.continueTestEnvironment);
  }

  async embed(text: string): Promise<number[] | number[][] | undefined> {
    try {
      const env = await this.getEnvironment();
      const envConfig = getDatabricksConfig(env);

      const endpoint = this.config?.endpoint || envConfig.embeddingEndpoint;
      const modelName = this.config?.modelName || envConfig.embeddingModel;
      const token = await this.getMdpToken();

      if (!token) {
        console.error("[EmbeddingService] No MDP token available");
        return undefined;
      }

      const controller = new AbortController();
      const timeoutId = setTimeout(
        () => controller.abort(),
        EmbeddingService.REQUEST_TIMEOUT,
      );

      const response = await fetch(`${endpoint}${modelName}/invocations`, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: `Bearer ${token}`,
        },
        body: JSON.stringify({
          dataframe_split: {
            columns: ["input"],
            data: [[text]],
          },
        }),
        signal: controller.signal,
      });

      clearTimeout(timeoutId);

      if (!response.ok) {
        const errorText = await response.text();
        console.error(
          `EmbeddingService: HTTP ${response.status} - ${errorText}`,
        );
        return undefined;
      }

      const data = (await response.json()) as any;

      if (data.predictions?.data?.[0]?.embedding) {
        return data.predictions.data[0].embedding;
      }

      if (
        data.predictions &&
        Array.isArray(data.predictions) &&
        data.predictions.length > 0
      ) {
        return data.predictions[0];
      }

      if (
        data.embeddings &&
        Array.isArray(data.embeddings) &&
        data.embeddings.length > 0
      ) {
        return data.embeddings[0];
      }

      if (Array.isArray(data) && data.length > 0) {
        return data[0];
      }

      console.error("EmbeddingService: Unexpected response format", data);
      return undefined;
    } catch (error) {
      if ((error as any).name === "AbortError") {
        console.error("EmbeddingService: Request timeout");
      } else {
        console.error("EmbeddingService: Embedding failed", error);
      }
      return undefined;
    }
  }
}
