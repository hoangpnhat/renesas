/**
 * RICA Proxy Adapter for CLI
 *
 * Implements BaseLlmApi for Databricks serving endpoints.
 * URL pattern: {apiBase}/serving-endpoints/{modelName}/invocations
 * Auth: Bearer MDP token (fetched fresh each session via ensureValidToken)
 * Body: OpenAI chat completion format
 */

import type {
  ChatCompletion,
  ChatCompletionChunk,
  ChatCompletionCreateParamsNonStreaming,
  ChatCompletionCreateParamsStreaming,
  Completion,
  CompletionCreateParamsNonStreaming,
  CompletionCreateParamsStreaming,
  CreateEmbeddingResponse,
  EmbeddingCreateParams,
  Model,
} from "openai/resources/index";

import { streamSse } from "@continuedev/fetch";

import type {
  BaseLlmApi,
  CreateRerankResponse,
  FimCreateParamsStreaming,
  RerankCreateParams,
} from "@continuedev/openai-adapters";

import { ensureValidToken } from "../auth/tokenRefresh.js";

export interface RicaProxyConfig {
  apiBase: string;
  apiKey?: string;
  model: string;
}

export class RicaProxyAdapter implements BaseLlmApi {
  private apiBase: string;
  private model: string;

  constructor(config: RicaProxyConfig) {
    this.apiBase = config.apiBase;
    this.model = config.model;
  }

  private getEndpoint(model?: string): string {
    const modelName = model || this.model;
    let base = this.apiBase.endsWith("/") ? this.apiBase : `${this.apiBase}/`;
    if (!base.includes("/serving-endpoints")) {
      base = `${base}serving-endpoints/`;
    }
    return `${base}${modelName}/invocations`;
  }

  private async getHeaders(): Promise<Record<string, string>> {
    const token = await ensureValidToken();
    return {
      "Content-Type": "application/json",
      Authorization: `Bearer ${token.mdpToken}`,
    };
  }

  async chatCompletionNonStream(
    body: ChatCompletionCreateParamsNonStreaming,
    signal: AbortSignal,
  ): Promise<ChatCompletion> {
    const endpoint = this.getEndpoint(body.model);
    const response = await fetch(endpoint, {
      method: "POST",
      headers: await this.getHeaders(),
      body: JSON.stringify({ ...body, stream: false }),
      signal,
    });

    if (!response.ok) {
      const text = await response.text();
      throw new Error(`RICA proxy error: ${response.status} ${text}`);
    }

    return (await response.json()) as ChatCompletion;
  }

  async *chatCompletionStream(
    body: ChatCompletionCreateParamsStreaming,
    signal: AbortSignal,
  ): AsyncGenerator<ChatCompletionChunk> {
    const endpoint = this.getEndpoint(body.model);
    const response = await fetch(endpoint, {
      method: "POST",
      headers: await this.getHeaders(),
      body: JSON.stringify({ ...body, stream: true }),
      signal,
    });

    if (!response.ok) {
      const text = await response.text();
      throw new Error(`RICA proxy error: ${response.status} ${text}`);
    }

    for await (const chunk of streamSse(response as any)) {
      if (chunk === "[DONE]") break;
      yield chunk as unknown as ChatCompletionChunk;
    }
  }

  async completionNonStream(
    body: CompletionCreateParamsNonStreaming,
    signal: AbortSignal,
  ): Promise<Completion> {
    const endpoint = this.getEndpoint(body.model);
    const response = await fetch(endpoint, {
      method: "POST",
      headers: await this.getHeaders(),
      body: JSON.stringify({ ...body, stream: false }),
      signal,
    });

    if (!response.ok) {
      const text = await response.text();
      throw new Error(`RICA proxy error: ${response.status} ${text}`);
    }

    return (await response.json()) as Completion;
  }

  async *completionStream(
    body: CompletionCreateParamsStreaming,
    signal: AbortSignal,
  ): AsyncGenerator<Completion> {
    const endpoint = this.getEndpoint(body.model);
    const response = await fetch(endpoint, {
      method: "POST",
      headers: await this.getHeaders(),
      body: JSON.stringify({ ...body, stream: true }),
      signal,
    });

    if (!response.ok) {
      const text = await response.text();
      throw new Error(`RICA proxy error: ${response.status} ${text}`);
    }

    for await (const chunk of streamSse(response as any)) {
      if (chunk === "[DONE]") break;
      yield chunk as unknown as Completion;
    }
  }

  async *fimStream(
    body: FimCreateParamsStreaming,
    signal: AbortSignal,
  ): AsyncGenerator<ChatCompletionChunk> {
    const endpoint = this.getEndpoint(body.model);
    const response = await fetch(endpoint, {
      method: "POST",
      headers: await this.getHeaders(),
      body: JSON.stringify({ ...body, stream: true }),
      signal,
    });

    if (!response.ok) {
      const text = await response.text();
      throw new Error(`RICA proxy error: ${response.status} ${text}`);
    }

    for await (const chunk of streamSse(response as any)) {
      if (chunk === "[DONE]") break;
      yield chunk as unknown as ChatCompletionChunk;
    }
  }

  async embed(body: EmbeddingCreateParams): Promise<CreateEmbeddingResponse> {
    const endpoint = this.getEndpoint(body.model);
    const response = await fetch(endpoint, {
      method: "POST",
      headers: await this.getHeaders(),
      body: JSON.stringify({ input: body.input }),
    });

    if (!response.ok) {
      const text = await response.text();
      throw new Error(`RICA proxy error: ${response.status} ${text}`);
    }

    return (await response.json()) as CreateEmbeddingResponse;
  }

  async rerank(body: RerankCreateParams): Promise<CreateRerankResponse> {
    const endpoint = this.getEndpoint(body.model);
    const response = await fetch(endpoint, {
      method: "POST",
      headers: await this.getHeaders(),
      body: JSON.stringify(body),
    });

    if (!response.ok) {
      const text = await response.text();
      throw new Error(`RICA proxy error: ${response.status} ${text}`);
    }

    return (await response.json()) as CreateRerankResponse;
  }

  async list(): Promise<Model[]> {
    return [];
  }
}
