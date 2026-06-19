# RICA AI Gateway Migration Strategy

> **Status:** Planning
> **Last updated:** 2026-06-10
> **Location:** `rica/docs/AI_GATEWAY_MIGRATION_STRATEGY.md`

---

## 1. Context

RICA uses Databricks serving endpoints as a multi-provider proxy (GPT, Claude, Gemini, Llama...). Currently, `RicaProxy.ts` must handle provider-specific parameter restrictions client-side because the serving endpoint passes requests directly to each model without filtering.

Databricks AI Gateway (MLflow-compatible) will sit between RICA and the serving endpoints, providing a unified OpenAI-compatible interface.

### Current Pain Points

| Problem                          | Example                                         |
| -------------------------------- | ----------------------------------------------- |
| Model rejects unsupported params | Claude Opus 4.8 rejects `temperature`           |
| Param combos rejected            | GPT-5.5 rejects `reasoning_effort` + `tools`    |
| Response format varies           | Gemini returns content as array, not string     |
| New models require code changes  | Each new model family needs regex + strip logic |

---

## 2. Architecture Evolution

```
Phase 1 (Current - Serving Endpoints)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Client (RicaProxy)
  ├── Detect model type (regex)
  ├── Strip unsupported params per model
  ├── Handle response format differences
  └── POST {apiBase}/{modelName}/invocations

Phase 2 (Transition - Strategy Pattern)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Client (RicaProxy + modelParamStrategies)
  ├── Lookup param rules from strategy table
  ├── Apply strip/rename rules
  └── POST {apiBase}/{modelName}/invocations
      OR  {apiBase}/ai-gateway/mlflow/v1/chat/completions

Phase 3 (Target - AI Gateway fully handles)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Client (RicaProxy thin wrapper)
  ├── Standard OpenAI format (no param filtering)
  ├── model field in body (not URL)
  └── POST {apiBase}/ai-gateway/mlflow/v1/chat/completions
      Gateway handles: param filtering, response normalization
```

---

## 3. Strategy Pattern Design (Phase 2)

### 3.1 File Layout

```
rica/adapters/llm/
├── RicaProxy.ts                 ← Main LLM class
├── modelParamStrategies.ts      ← Param rules per model family
└── index.ts
```

### 3.2 Strategy Interface

```typescript
// rica/adapters/llm/modelParamStrategies.ts

interface ParamStrategy {
  /** Regex to match model name (after _convertModelName) */
  match: RegExp;

  /** Params to always remove from request body */
  strip: string[];

  /** Params to rename (e.g., max_tokens → max_completion_tokens) */
  rename: Record<string, string>;

  /** Params to conditionally remove based on other options */
  conditionalStrip?: Record<string, (options: CompletionOptions) => boolean>;

  /** Default values to add if not present */
  defaults?: Record<string, any>;
}
```

### 3.3 Strategy Table

```typescript
const MODEL_PARAM_STRATEGIES: ParamStrategy[] = [
  {
    // Claude 4.x (Opus, Haiku, Sonnet) on Databricks
    // Rejects: temperature
    match: /claude-(opus|haiku|sonnet)-4/,
    strip: ["temperature", "prediction"],
    rename: {},
  },
  {
    // GPT-5 family on Databricks
    // Rejects: temperature, stop
    // Renames: max_tokens → max_completion_tokens
    match: /\bgpt-5\b/,
    strip: ["temperature", "stop"],
    rename: { max_tokens: "max_completion_tokens" },
    conditionalStrip: {
      // GPT-5.5 rejects reasoning_effort when tools are present
      reasoning_effort: (opts) => !!opts.tools?.length,
    },
  },
  {
    // Gemini models — currently no param restrictions known
    match: /gemini/,
    strip: [],
    rename: {},
  },
  {
    // Default: standard OpenAI-compatible (GPT-4o, Llama, etc.)
    match: /.*/,
    strip: [],
    rename: {},
    defaults: { temperature: 0.7 },
  },
];
```

### 3.4 Usage in RicaProxy

```typescript
// In _streamChat():
const strategy = getParamStrategy(modelName);
const body = buildRequestBody(options, formattedMessages, strategy, {
  stream: useStreaming,
  tools: options.tools,
  client_request_id: clientRequestId,
});
```

### 3.5 buildRequestBody Implementation

```typescript
function buildRequestBody(
  options: CompletionOptions,
  messages: any[],
  strategy: ParamStrategy,
  extraFields: Record<string, any>,
): Record<string, any> {
  const body: Record<string, any> = { messages, ...extraFields };

  // Apply defaults
  if (strategy.defaults) {
    for (const [key, value] of Object.entries(strategy.defaults)) {
      body[key] = (options as any)[key] ?? value;
    }
  }

  // Add standard params (if not in strip list)
  const standardParams = {
    temperature: options.temperature,
    max_tokens: options.maxTokens,
    stop: options.stop,
    top_p: options.topP,
  };

  for (const [param, value] of Object.entries(standardParams)) {
    if (value === undefined) continue;
    if (strategy.strip.includes(param)) continue;

    // Check conditional strip
    const condition = strategy.conditionalStrip?.[param];
    if (condition && condition(options)) continue;

    // Apply rename
    const outputKey = strategy.rename[param] ?? param;
    body[outputKey] = value;
  }

  // Remove undefined/null entries
  for (const key of Object.keys(body)) {
    if (body[key] === undefined || body[key] === null) delete body[key];
  }

  return body;
}
```

---

## 4. AI Gateway Migration Steps

### 4.1 Prerequisites

- [ ] AI Gateway endpoint deployed and accessible
- [ ] Verify all model families respond correctly (GPT, Claude, Gemini, Llama)
- [ ] Confirm gateway handles param filtering server-side
- [ ] Confirm gateway normalizes response format (content always string, standard SSE)
- [ ] MDP token auth works with new endpoint

### 4.2 Migration Checklist

| Step | Action                                      | File                                    |
| ---- | ------------------------------------------- | --------------------------------------- |
| 1    | Add AI Gateway endpoint support (dual-mode) | `RicaProxy.ts`                          |
| 2    | Add `model` field to request body           | `RicaProxy.ts`                          |
| 3    | Test each model family through gateway      | Manual validation                       |
| 4    | Verify param filtering happens server-side  | Test: send temperature to Claude via GW |
| 5    | Verify response normalization               | Test: Gemini content format via GW      |
| 6    | Remove param strategies (if GW handles all) | `modelParamStrategies.ts`               |
| 7    | Switch default endpoint to AI Gateway       | `RicaProxy.ts` or dynamic config        |
| 8    | Remove old serving endpoint code path       | `RicaProxy.ts`                          |

### 4.3 Scope: Which Models Migrate

**AI Gateway applies ONLY to external/third-party models** hosted on Databricks. Self-hosted models continue using serving endpoints directly.

| Model Type                  | Examples                                          | Endpoint After Migration                                      |
| --------------------------- | ------------------------------------------------- | ------------------------------------------------------------- |
| **External (migrate)**      | GPT-4o, GPT-5.x, Claude Opus/Sonnet/Haiku, Gemini | AI Gateway: `{apiBase}/ai-gateway/mlflow/v1/chat/completions` |
| **Self-hosted (no change)** | Jina Embedding, Jina Reranker, custom fine-tuned  | Serving Endpoint: `{apiBase}/{modelName}/invocations`         |

**Why self-hosted stays on serving endpoints:**

- Jina models are deployed directly on Databricks compute (not routed through external providers)
- No provider-specific param quirks — they accept what they're configured for
- AI Gateway adds unnecessary latency for models already running locally
- Embedding/reranker APIs use different request format (`input` field, not `messages`)

### 4.4 Endpoint Changes (Chat/LLM only)

```diff
// Before (Serving Endpoints):
- const endpoint = `${this.apiBase}${modelName}/invocations`;
- const body = { messages, stream, ... };

// After (AI Gateway — external models only):
+ const endpoint = `${this.apiBase}/ai-gateway/mlflow/v1/chat/completions`;
+ const body = { model: modelName, messages, stream, ... };
```

### 4.5 Embed/Rerank Endpoints (NO CHANGE)

Self-hosted Jina embedding and reranker models **stay on serving endpoints**:

```typescript
// _embed() — NO CHANGE (Jina self-hosted)
const resp = await this.fetch(`${this.apiBase}${modelName}/invocations`, {
  body: JSON.stringify({ input: chunk }),
});

// rerank() — NO CHANGE (Jina self-hosted)
const resp = await this.fetch(`${this.apiBase}${modelName}/invocations`, {
  body: JSON.stringify({ dataframe_split: { ... } }),
});
```

Only if embedding models are moved to AI Gateway in the future:

```diff
// Future (if Jina moves to AI Gateway):
+ const resp = await this.fetch(`${this.apiBase}/ai-gateway/mlflow/v1/embeddings`, {
+   body: JSON.stringify({ model: modelName, input: chunk }),
+ });
```

---

## 5. Validation Matrix

After migration, verify each cell:

| Model                        | Chat | Streaming | Tools/Agent | Embed | Rerank |
| ---------------------------- | ---- | --------- | ----------- | ----- | ------ |
| databricks-gpt-4o            |      |           |             |       |        |
| databricks-gpt-5-4           |      |           |             | N/A   | N/A    |
| databricks-gpt-5-5           |      |           |             | N/A   | N/A    |
| databricks-claude-sonnet-4-6 |      |           |             | N/A   | N/A    |
| databricks-claude-opus-4-8   |      |           |             | N/A   | N/A    |
| databricks-gemini-3-1-flash  |      |           |             | N/A   | N/A    |
| embedding model              | N/A  | N/A       | N/A         |       | N/A    |
| reranker model               | N/A  | N/A       | N/A         | N/A   |        |

### Key Tests

1. **Claude + no temperature**: Gateway should NOT pass temperature to Claude backend
2. **GPT-5.5 + tools**: Gateway should strip `reasoning_effort` when tools present
3. **Gemini streaming**: Response chunks should be standard OpenAI SSE format
4. **Tool calls**: Verify function calling works through gateway for all model families

---

## 6. Rollback Plan

If AI Gateway has issues, the strategy pattern supports dual-mode for **chat/LLM only** (embed/rerank are unaffected — always on serving endpoints):

```typescript
// In RicaProxy constructor or dynamic config:
private useAIGateway: boolean = false; // Toggle via config

// In _streamChat():
const endpoint = this.useAIGateway
  ? `${this.apiBase}/ai-gateway/mlflow/v1/chat/completions`
  : `${this.apiBase}${modelName}/invocations`;

const body = this.useAIGateway
  ? { model: modelName, ...baseBody }         // GW mode: model in body, no param filtering
  : buildRequestBody(options, ..., strategy);  // Legacy: param filtering

// _embed() and rerank() are NOT affected by this toggle
// They always use: `${this.apiBase}${modelName}/invocations`
```

This allows:

- Feature flag toggle between old/new endpoints (chat only)
- Per-environment rollout (Non-Prod first, then Prod)
- Immediate rollback without code changes
- Embed/Rerank remain stable regardless of toggle state

---

## 7. Post-Migration: Final RicaProxy (Phase 3)

Once AI Gateway is verified to handle all provider quirks for external models:

```typescript
// rica/adapters/llm/RicaProxy.ts — Final form (reduced, not minimal)
// Chat/LLM → AI Gateway | Embed/Rerank → Serving Endpoints (self-hosted)

class RicaProxy extends OpenAI {
  static providerName = "rica-proxy";
  static defaultOptions: Partial<LLMOptions> = {
    useLegacyCompletionsEndpoint: false,
  };

  set controlPlaneProxyInfo(value: ControlPlaneProxyInfo) {
    this.apiKey = value.workOsAccessToken;
  }

  constructor(options: LLMOptions) {
    super(options);
    if (options.apiBase) {
      this.apiBase = options.apiBase;
    }
  }

  // Chat/LLM: route through AI Gateway
  protected _getEndpoint(): string {
    return `${this.apiBase}/ai-gateway/mlflow/v1/chat/completions`;
  }

  // Inherit from OpenAI for chat:
  // - _streamChat() ✓ (AI Gateway returns standard OpenAI SSE)
  // - param handling ✓ (AI Gateway strips invalid params server-side)
  // - tool calls ✓
  // - streaming ✓

  // Embed: self-hosted Jina — stays on serving endpoint
  protected async _embed(chunks: string[]): Promise<number[][]> {
    const modelName = this._convertModelName(this.model);
    const embeddings: number[][] = [];
    for (const chunk of chunks) {
      const resp = await this.fetch(`${this.apiBase}${modelName}/invocations`, {
        method: "POST",
        headers: this._getHeaders(),
        body: JSON.stringify({ input: chunk }),
      });
      if (!resp.ok) throw new Error(`Embed error: ${resp.status}`);
      const data = await resp.json();
      embeddings.push(data.data[0].embedding);
    }
    return embeddings;
  }

  // Rerank: self-hosted Jina — stays on serving endpoint
  async rerank(query: string, chunks: any[]): Promise<number[]> {
    const modelName = this._convertModelName(this.model);
    const resp = await this.fetch(`${this.apiBase}${modelName}/invocations`, {
      method: "POST",
      headers: this._getHeaders(),
      body: JSON.stringify({
        dataframe_split: {
          columns: ["query", "documents"],
          data: [[query, chunks.map((c) => c.content)]],
        },
      }),
    });
    if (!resp.ok) throw new Error(`Rerank error: ${resp.status}`);
    const data = await resp.json();
    if (data.predictions?.results) {
      return data.predictions.results
        .sort((a: any, b: any) => a.index - b.index)
        .map((r: any) => r.relevance_score);
    }
    return chunks.map(() => 1.0);
  }
}
```

**Benefits of Phase 3:**

- Chat/LLM logic reduced to ~10 lines (inherits from OpenAI)
- Zero maintenance for external model params (AI Gateway handles)
- Upstream community fixes streaming/tools → RICA gets free
- No regex or strategy table for chat models
- Self-hosted embed/rerank remain stable (no AI Gateway dependency)

**Prerequisite for Phase 3:**

- AI Gateway MUST normalize responses to standard OpenAI format
- AI Gateway MUST strip invalid params per model
- If gateway doesn't do this → stay at Phase 2 (strategy pattern)
- Embed/Rerank stay on serving endpoints regardless of AI Gateway status

---
