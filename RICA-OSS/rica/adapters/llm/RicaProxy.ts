import { streamSse } from "../../../packages/fetch/src/stream.js";

import type { ControlPlaneProxyInfo } from "../../../core/control-plane/analytics/IAnalyticsProvider.js";
import type {
  ChatMessage,
  CompletionOptions,
  LLMOptions,
} from "../../../core/index.js";
import { fromChatCompletionChunk } from "../../../core/llm/openaiTypeConverters.js";
import OpenAI from "../../../core/llm/llms/OpenAI.js";

console.warn("[RICA-PROXY-LOADED] build=2026-06-07 (v1.2.1 grafts: gpt-5/claude-4 strips, tool-call sanitize, _lastApiUsage capture)");

class RicaProxy extends OpenAI {
  static providerName = "rica-proxy";
  static defaultOptions: Partial<LLMOptions> = {
    useLegacyCompletionsEndpoint: false,
  };

  maxStopWords: number | undefined = 16;

  set controlPlaneProxyInfo(value: ControlPlaneProxyInfo) {
    const fresh = value.workOsAccessToken;
    const old = this.apiKey;
    this.apiKey = fresh;
    // Phase 5.x TEMP — verifies the rotation pipe re-wired in
    // doLoadConfig.ts is firing. Decodes the JWT aud claim so we can
    // distinguish MDP token (correct) from EntraID raw JWT (wrong —
    // the prior failed attempts would surface as aud starting with
    // "api://"). Strip alongside other Phase 5 diagnostics in 5.1.8
    // after long-session smoke test (60-75 min) confirms zero 401s.
    try {
      let aud: string | undefined;
      let exp: number | undefined;
      if (fresh && fresh.split(".").length === 3) {
        const payload = JSON.parse(
          Buffer.from(fresh.split(".")[1], "base64").toString("utf-8"),
        );
        aud = typeof payload.aud === "string" ? payload.aud : undefined;
        exp = typeof payload.exp === "number" ? payload.exp : undefined;
      }
      console.warn(
        `[RICA-PROXY:rotate] controlPlaneProxyInfo setter fired model=${this.title ?? "?"} oldLen=${old?.length ?? 0} newLen=${fresh?.length ?? 0} aud=${aud ?? "?"} expIn=${exp ? Math.round(exp - Date.now() / 1000) + "s" : "?"}`,
      );
    } catch {
      // diagnostic — never block setter on it
    }
  }

  constructor(options: LLMOptions) {
    super(options);
    if (options.apiBase) {
      this.apiBase = options.apiBase;
    }
  }

  get underlyingProviderName(): string {
    const parts = this.model.split("/");
    if (parts.length >= 4) {
      return parts[2];
    }
    return "databricks";
  }

  protected _convertModelName(model: string): string {
    const parts = model.split("/");
    return parts[parts.length - 1];
  }

  supportsFim(): boolean {
    return false;
  }

  supportsImages(): boolean {
    return true;
  }

  supportsCompletions(): boolean {
    return true;
  }

  private _shouldStream(): boolean {
    const modelLower = this.model.toLowerCase();
    return (
      modelLower.includes("gpt") ||
      modelLower.includes("claude") ||
      modelLower.includes("chatgpt")
    );
  }

  protected async *_streamChat(
    messages: ChatMessage[],
    signal: AbortSignal,
    options: CompletionOptions,
  ): AsyncGenerator<ChatMessage> {
    const modelName = this._convertModelName(this.model);
    const useStreaming = this._shouldStream() && options.stream !== false;

    // GPT-5 family on Databricks rejects several legacy OpenAI params:
    // - `temperature` must be 1.0 or omitted (forced reasoning behavior)
    // - `max_tokens` is renamed `max_completion_tokens`
    // - `stop` sequences are not supported
    // Detect by model id rather than provider, since the proxy routes many
    // model families through the same endpoint.
    const isGpt5 = /\bgpt-5\b|gpt-5-\d/.test(modelName);
    // Anthropic Claude 4-family (Opus 4.x, Haiku 4.x) rejects `temperature`:
    // backend returns 400 with "Model global.anthropic.claude-opus-4-7 does
    // not support the temperature parameter." Same constraint observed on
    // Opus 4.7 (2026-05-13). Sonnet 4.x has not shown this so far; expand
    // the regex if it surfaces.
    const isClaude4NoTemp = /claude-(opus|haiku)-4/.test(modelName);
    console.warn(
      `[RICA-PROXY:_streamChat] entered. model=${this.model} resolved=${modelName} isGpt5=${isGpt5} isClaude4NoTemp=${isClaude4NoTemp} streaming=${useStreaming} temperature=${(options as any).temperature}`,
    );

    const formattedMessages = messages.map((msg) => ({
      role: msg.role,
      content: Array.isArray(msg.content)
        ? msg.content.map((part: any) => {
            if (part.type === "imageUrl") {
              return {
                type: "image_url",
                image_url: { url: part.imageUrl.url, detail: "auto" },
              };
            }
            if (part.type === "text") {
              return { type: "text", text: part.text };
            }
            return part;
          })
        : msg.content,
      ...("toolCalls" in msg && msg.toolCalls
        ? { tool_calls: msg.toolCalls }
        : {}),
      ...("toolCallId" in msg && msg.toolCallId
        ? { tool_call_id: msg.toolCallId }
        : {}),
    }));

    // Sanitize tool_calls with malformed `arguments` JSON. Real-world cause:
    // agent-mode (or multi-agent) streams that aborted mid-tool-call leave a
    // tool_call in history whose `arguments` field is truncated. The backend
    // rejects the conversation with HTTP 400 "Param 'arguments' ... is not a
    // valid JSON string". Replace unparseable arguments with `"{}"` so the
    // structure stays valid.
    let sanitizedCount = 0;
    for (const m of formattedMessages as any[]) {
      const calls = m?.tool_calls;
      if (!Array.isArray(calls)) continue;
      for (const c of calls) {
        const args = c?.function?.arguments;
        if (typeof args !== "string") continue;
        try {
          JSON.parse(args);
        } catch {
          c.function.arguments = "{}";
          sanitizedCount++;
        }
      }
    }
    if (sanitizedCount > 0) {
      console.warn(
        `[RICA-PROXY] Sanitized ${sanitizedCount} tool_call(s) with malformed JSON arguments before send`,
      );
    }

    // Sanitize empty text blocks. Anthropic-on-Databricks rejects content
    // arrays containing `{type:"text", text:""}` blocks with HTTP 400
    // "messages: text content blocks must be non-empty". Real-world cause:
    // an orchestrator turn (or multi-agent recap) that emitted a thinking
    // block followed by real text gets serialized back with a leading empty
    // text block. Drop empty text blocks; if the array empties out, collapse
    // to "" — assistant turns with `tool_calls` and empty string content are
    // accepted by the backend, and user turns shouldn't have empty content
    // anyway (defensive fallback).
    let emptyBlocksDropped = 0;
    let messagesCollapsed = 0;
    for (const m of formattedMessages as any[]) {
      if (!Array.isArray(m.content)) continue;
      const before = m.content.length;
      m.content = m.content.filter((p: any) => {
        if (p?.type !== "text") return true;
        const t = typeof p.text === "string" ? p.text : "";
        // Treat whitespace-only as empty too — backend trims server-side.
        return t.trim().length > 0;
      });
      emptyBlocksDropped += before - m.content.length;
      if (m.content.length === 0) {
        m.content = "";
        messagesCollapsed++;
      }
    }
    if (emptyBlocksDropped > 0) {
      console.warn(
        `[RICA-PROXY] Sanitized ${emptyBlocksDropped} empty text block(s) before send (${messagesCollapsed} message(s) collapsed to empty string)`,
      );
    }

    const body: Record<string, any> = {
      messages: formattedMessages,
      stream: useStreaming,
      ...(options.tools ? { tools: options.tools } : {}),
    };

    if (isGpt5) {
      // Reasoning-style: skip temperature/stop, use max_completion_tokens
      if (options.maxTokens) body.max_completion_tokens = options.maxTokens;
    } else if (isClaude4NoTemp) {
      // Skip temperature only; max_tokens and stop still accepted
      if (options.maxTokens) body.max_tokens = options.maxTokens;
      if (options.stop) body.stop = options.stop;
    } else {
      body.temperature = options.temperature ?? 0.7;
      if (options.maxTokens) body.max_tokens = options.maxTokens;
      if (options.stop) body.stop = options.stop;
    }

    // Phase 5.x — reasoning body wire reverted (commits f548cef34 + 7d8d0cc88).
    // Live test against Claude Opus 4.6 with body.thinking=adaptive +
    // output_config.effort=low confirmed Databricks accepts the shape and
    // the model reasons (prompt=4792 completion=0 — purely thinking, no
    // visible content). BUT the streamed `delta.reasoning_content` chunks
    // hit refork's chunk decoder which can't parse them into the chat
    // surface, surfacing as "Cannot read properties of undefined (reading
    // 'split')" with no visible response. Need a proper reasoning_content
    // → thinking-block render path before re-enabling. The lightbulb still
    // shows for capable models (autodetect/capability gate intact) but
    // toggling it currently is a no-op at the body level. Re-enable by
    // restoring the `if (options.reasoning && isClaude4NoTemp)` block
    // AFTER fixing the chunk decoder.

    const endpoint = `${this.apiBase}${modelName}/invocations`;

    // Phase 5.1.x TEMP — pinpoint Databricks 400. Dump message structure +
    // tool_call arguments BEFORE send so we can correlate the rejected body
    // with the response error message. Strip alongside [RICA-DYNAMIC] /
    // [CONTEXT-DEBUG] in 5.1.8.
    const reqBodyJson = JSON.stringify(body);
    try {
      const msgSummary = formattedMessages.map((m: any, i: number) => {
        const role = m.role;
        const contentKind = Array.isArray(m.content)
          ? `array[${m.content.length}]`
          : typeof m.content === "string"
            ? `str(${m.content.length})`
            : m.content == null
              ? "null"
              : typeof m.content;
        const callsInfo = Array.isArray(m.tool_calls)
          ? `tool_calls[${m.tool_calls.length}]:` +
            m.tool_calls
              .map((c: any) => {
                const args = c?.function?.arguments;
                const argsLen =
                  typeof args === "string" ? args.length : "non-string";
                let argsValid = "n/a";
                if (typeof args === "string") {
                  try {
                    JSON.parse(args);
                    argsValid = "ok";
                  } catch {
                    argsValid = "INVALID";
                  }
                }
                return `${c?.function?.name ?? "?"}(args=${argsLen},${argsValid})`;
              })
              .join(",")
          : "";
        const tcid = m.tool_call_id ? `tool_call_id=${m.tool_call_id}` : "";
        let preview = "";
        if (typeof m.content === "string") {
          preview = m.content.slice(0, 80).replace(/\n/g, "\\n");
        } else if (Array.isArray(m.content)) {
          preview = m.content
            .map((p: any) => {
              if (p?.type === "text") {
                return `text(${(p.text ?? "").length})`;
              }
              return p?.type ?? "?";
            })
            .join(",");
        }
        return `[${i}] role=${role} content=${contentKind} ${callsInfo} ${tcid} preview="${preview}"`;
      });
      console.warn(
        `[RICA-PROXY:REQUEST] ${modelName} bodyBytes=${reqBodyJson.length} messages=${formattedMessages.length} tools=${Array.isArray(options.tools) ? options.tools.length : 0}\n  ` +
          msgSummary.join("\n  "),
      );
    } catch (diagErr) {
      console.warn(
        `[RICA-PROXY:REQUEST] diag dump threw — ${(diagErr as any)?.message}`,
      );
    }

    const response = await this.fetch(endpoint, {
      method: "POST",
      headers: this._getHeaders(),
      body: reqBodyJson,
      signal,
    });

    if (!response.ok) {
      const errorText = await response.text();
      // Phase 5.1.x TEMP — full error body (un-truncated) + the offending
      // request body (capped at 8KB) so we can see exactly which field
      // Databricks rejected. Strip in 5.1.8.
      console.error(
        `[RICA-PROXY:400] ${modelName} status=${response.status}\n` +
          `  error body (full):\n${errorText}\n` +
          `  request body (first 8000 chars):\n${reqBodyJson.slice(0, 8000)}`,
      );
      // 401 from Databricks always means the cached EntraID access token has
      // expired between config-load and now (the LLM holds apiKey from
      // controlPlaneProxyInfo at construction; ConfigHandler.reloadConfig
      // doesn't refresh sessionInfoPromise so the token stays stale). Surface
      // an actionable message instead of the raw "Token is expired" Databricks
      // payload so users know what to do — proper auto-refresh requires
      // threading the EntraID token-getter through to the LLM (deferred).
      if (response.status === 401) {
        throw new Error(
          `Your RICA session token has expired. Please reload the VS Code window ` +
          `(Ctrl+Shift+P → "Developer: Reload Window") to refresh authentication. ` +
          `[Databricks ${response.status}: ${errorText.slice(0, 200)}]`,
        );
      }
      throw new Error(`RicaProxy API error: ${response.status} ${errorText}`);
    }

    if (!useStreaming) {
      // Non-streaming path. Multi-agent sub-agent runs go through here so
      // they can capture real backend token counts for the consumption
      // endpoint. Streaming chats skip usage capture by design — the SSE
      // chunks rarely carry a usage block on this proxy.
      if (response.status === 499) {
        return; // Aborted by user
      }
      const data = await response.json();

      // Capture real backend token usage for PromptLog / consumption endpoint.
      // Mirrors the shape the consumer at core/llm/index.ts reads from
      // `_lastApiUsage`. Sang's POST /api/user-model/configs depends on this.
      if (data?.usage) {
        (this as any)._lastApiUsage = {
          promptTokens: data.usage.prompt_tokens ?? 0,
          completionTokens: data.usage.completion_tokens ?? 0,
          totalTokens:
            data.usage.total_tokens
            ?? ((data.usage.prompt_tokens ?? 0) + (data.usage.completion_tokens ?? 0)),
          cacheReadTokens:
            data.usage.cache_read_input_tokens
            ?? data.usage.prompt_tokens_details?.cached_tokens
            ?? 0,
          cacheCreationTokens: data.usage.cache_creation_input_tokens ?? 0,
          isActualUsage: true,
        };
        console.log(
          `[CONSUMPTION] ${modelName} prompt=${data.usage.prompt_tokens} completion=${data.usage.completion_tokens} total=${data.usage.total_tokens}`,
        );
      }

      const message = this._extractAssistantMessage(data, modelName);
      if (!message) {
        console.error(
          `[RICA-PROXY:${modelName}] could not extract assistant message — raw response:`,
          JSON.stringify(data).slice(0, 4000),
        );
        throw new Error(
          `RicaProxy response from ${modelName} did not contain a parseable assistant message. See logs for raw payload.`,
        );
      }
      yield message;
      return;
    }

    // Streaming path (chat mode for gpt/claude/chatgpt). Databricks/Anthropic
    // ship a final SSE chunk carrying { usage: { prompt_tokens, completion_tokens, ... } }
    // with an empty/omitted message — fromChatCompletionChunk returns null for
    // those, so we used to drop the usage payload on the floor. Capture it
    // here into _lastApiUsage so core/llm/index.ts streamChat can return real
    // API counts on the PromptLog (otherwise SessionTokenBadge sees prompt=0
    // /completion=0 and stays hidden — same display path as v1.2.0). Falls
    // back to tiktoken upstream when the provider doesn't ship a usage chunk.
    for await (const value of streamSse(response)) {
      // Capture usage from any chunk that carries it. Last-chunk wins.
      const u = (value as any)?.usage;
      if (u && (u.prompt_tokens != null || u.completion_tokens != null)) {
        (this as any)._lastApiUsage = {
          promptTokens: u.prompt_tokens ?? 0,
          completionTokens: u.completion_tokens ?? 0,
          totalTokens:
            u.total_tokens ??
            (u.prompt_tokens ?? 0) + (u.completion_tokens ?? 0),
          cacheReadTokens:
            u.cache_read_input_tokens ??
            u.prompt_tokens_details?.cached_tokens ??
            0,
          cacheCreationTokens: u.cache_creation_input_tokens ?? 0,
          isActualUsage: true,
        };
      }
      const chunk = fromChatCompletionChunk(value);
      if (chunk) {
        yield chunk;
      }
    }
    if ((this as any)._lastApiUsage?.isActualUsage) {
      const u = (this as any)._lastApiUsage;
      console.log(
        `[CONSUMPTION] ${modelName} (stream) prompt=${u.promptTokens} completion=${u.completionTokens} total=${u.totalTokens}`,
      );
    }
  }

  /**
   * Extract the assistant message from a non-streaming response, tolerant to
   * the shape variants observed across model families:
   *
   * - GPT-5.4: `data.choices[0].message = { content, tool_calls }`
   * - GPT-5.5: same envelope but content can be null when tool_calls are
   *   present, and (per Databricks proxy) sometimes nests reasoning under
   *   `reasoning_content` or wraps content as an array `[{type, text}]`
   * - Claude family: same envelope but content may be an array of blocks
   *
   * Returns null if no message can be extracted; caller should treat that
   * as an error and log the raw payload.
   */
  protected _extractAssistantMessage(
    data: any,
    modelName: string,
  ): ChatMessage | null {
    const choice = data?.choices?.[0];
    const msg = choice?.message ?? choice?.delta;
    if (!msg) return null;

    // Normalize content: string | array of blocks | null
    let content: any = msg.content ?? "";
    if (Array.isArray(content)) {
      content = content
        .map((b: any) =>
          typeof b === "string" ? b : (b?.text ?? b?.content ?? ""),
        )
        .filter(Boolean)
        .join("");
    }
    if (!content && typeof msg.reasoning_content === "string") {
      console.log(
        `[RICA-PROXY:${modelName}] using reasoning_content fallback (${msg.reasoning_content.length} chars)`,
      );
    }

    const toolCalls = Array.isArray(msg.tool_calls)
      ? msg.tool_calls
          .filter((tc: any) => tc && tc.function?.name)
          .map((tc: any) => ({
            id: tc.id,
            type: tc.type ?? "function",
            function: {
              name: tc.function.name,
              arguments:
                typeof tc.function.arguments === "string"
                  ? tc.function.arguments
                  : JSON.stringify(tc.function.arguments ?? {}),
            },
          }))
      : undefined;

    if (!content && !toolCalls) {
      console.warn(
        `[RICA-PROXY:${modelName}] response had neither content nor tool_calls. finish_reason=${choice?.finish_reason}`,
      );
    }

    return {
      role: "assistant",
      content: typeof content === "string" ? content : "",
      ...(toolCalls && toolCalls.length > 0 ? { toolCalls } : {}),
    };
  }

  protected async _embed(chunks: string[]): Promise<number[][]> {
    const modelName = this._convertModelName(this.model);
    const embeddings: number[][] = [];

    for (const chunk of chunks) {
      const resp = await this.fetch(`${this.apiBase}${modelName}/invocations`, {
        method: "POST",
        headers: this._getHeaders(),
        body: JSON.stringify({ input: chunk }),
      });

      if (!resp.ok) {
        const errorText = await resp.text();
        throw new Error(
          `RicaProxy embed API error: ${resp.status} ${errorText}`,
        );
      }

      const data = await resp.json();
      if (data.data?.[0]?.embedding) {
        embeddings.push(data.data[0].embedding);
      } else {
        throw new Error("RicaProxy embed API returned unexpected format");
      }
    }

    return embeddings;
  }

  async rerank(query: string, chunks: any[]): Promise<number[]> {
    const modelName = this._convertModelName(this.model);

    const requestBody = {
      dataframe_split: {
        columns: ["query", "documents"],
        data: [[query, chunks.map((chunk) => chunk.content)]],
      },
    };

    const resp = await this.fetch(`${this.apiBase}${modelName}/invocations`, {
      method: "POST",
      headers: this._getHeaders(),
      body: JSON.stringify(requestBody),
    });

    if (!resp.ok) {
      const errorText = await resp.text();
      throw new Error(
        `RicaProxy rerank API error: ${resp.status} ${errorText}`,
      );
    }

    const data = await resp.json();
    if (data.predictions?.results) {
      const results = data.predictions.results.sort(
        (a: any, b: any) => a.index - b.index,
      );
      return results.map((result: any) => result.relevance_score);
    }

    return chunks.map(() => 1.0);
  }
}

export default RicaProxy;
