import { DevDataSqliteDb } from "../../../core/data/devdataSqlite.js";
import type { IdeSettings } from "./env";
import {
  getRicaModelsEndpoint,
  normalizeRicaModels,
  type RicaDynamicModel,
  type RicaNormalizedModel,
} from "./modelMapping";

/**
 * Backend response shape for /api/user-model/configs.
 * Mirrors v1.2.0's `EntraIDDynamicConfig` plus the `consumptionLimit` /
 * `multiAgentEligible` fields the multi-agent feature relies on.
 *
 * Top-level passthrough fields (embeddingsProvider, tabAutocompleteModel,
 * systemMessage, completionOptions, requestOptions, ui, experimental,
 * analytics, docs, context) are forwarded verbatim from the backend so
 * downstream consumers can merge them into the runtime config — same
 * behaviour as v1.2.0's `validateAndNormalizeConfig` + `mergeDynamicConfig`.
 */
export interface RicaDynamicConfigResponse {
  models?: RicaDynamicModel[];
  data?: RicaDynamicModel[];
  config?: {
    models?: RicaDynamicModel[];
    name?: string;
    version?: string;
    schema?: string;
    context?: unknown[];
    embeddingsProvider?: unknown;
    tabAutocompleteModel?: unknown;
    systemMessage?: string;
    completionOptions?: Record<string, unknown>;
    requestOptions?: Record<string, unknown>;
    ui?: Record<string, unknown>;
    experimental?: Record<string, unknown>;
    analytics?: Record<string, unknown>;
    docs?: unknown[];
  };
  embeddingsProvider?: unknown;
  tabAutocompleteModel?: unknown;
  systemMessage?: string;
  completionOptions?: Record<string, unknown>;
  requestOptions?: Record<string, unknown>;
  ui?: Record<string, unknown>;
  experimental?: Record<string, unknown>;
  analytics?: Record<string, unknown>;
  docs?: unknown[];
  context?: unknown[];
  updatedAt?: string;
  timestamp?: number;
  version?: string;
  [key: string]: unknown;
}

/**
 * Snapshot returned to the profile loader. Extended from Bui's original to
 * carry every passthrough field v1.2.0's `validateAndNormalizeConfig`
 * surfaced. Consumers that don't need a field can ignore it; consumers
 * that need it (multi-agent panel, embeddings provider, tab autocomplete)
 * see exactly what v1.2.0 saw.
 */
export interface RicaDynamicConfigSnapshot {
  endpoint: string;
  models: RicaNormalizedModel[];
  updatedAt?: string;
  fetchedAt: string;
  embeddingsProvider?: unknown;
  tabAutocompleteModel?: unknown;
  systemMessage?: string;
  completionOptions?: Record<string, unknown>;
  requestOptions?: Record<string, unknown>;
  ui?: Record<string, unknown>;
  experimental?: Record<string, unknown>;
  analytics?: Record<string, unknown>;
  docs?: unknown[];
  context?: unknown[];
}

export interface RicaDynamicConfigFetchOptions {
  method?: "GET" | "POST";
  body?: Record<string, unknown>;
  accessToken?: string;
}

/**
 * Backend requires `model_id` as a 24-char Mongo ObjectId. Filter out any
 * legacy rows logged with slug-style ids (pre-v1.1.x) so one bad row doesn't
 * 422 the whole batch. These rows stay in SQLite as `synced=0` and simply
 * get ignored on every drain — harmless dead weight. Mirrors v1.2.0
 * EntraIDDynamicConfigService.fetchDynamicConfig:64-78.
 */
const OBJECT_ID_REGEX = /^[0-9a-f]{24}$/;

function extractModels(payload: RicaDynamicConfigResponse): RicaDynamicModel[] {
  if (Array.isArray(payload.models)) {
    return payload.models;
  }

  if (Array.isArray(payload.config?.models)) {
    return payload.config.models;
  }

  if (Array.isArray(payload.data)) {
    return payload.data;
  }

  return [];
}

/**
 * Pull the v1.2.0 passthrough fields off either the top-level response or
 * the nested `config` envelope. Backend has historically returned both
 * shapes; v1.2.0 normalised that ambiguity in `validateAndNormalizeConfig`.
 */
function extractPassthrough(
  payload: RicaDynamicConfigResponse,
): Pick<
  RicaDynamicConfigSnapshot,
  | "embeddingsProvider"
  | "tabAutocompleteModel"
  | "systemMessage"
  | "completionOptions"
  | "requestOptions"
  | "ui"
  | "experimental"
  | "analytics"
  | "docs"
  | "context"
> {
  const c = payload.config ?? {};
  return {
    embeddingsProvider: payload.embeddingsProvider ?? c.embeddingsProvider,
    tabAutocompleteModel:
      payload.tabAutocompleteModel ?? c.tabAutocompleteModel,
    systemMessage: payload.systemMessage ?? c.systemMessage,
    completionOptions: payload.completionOptions ?? c.completionOptions,
    requestOptions: payload.requestOptions ?? c.requestOptions,
    ui: payload.ui ?? c.ui,
    experimental: payload.experimental ?? c.experimental,
    analytics: payload.analytics ?? c.analytics,
    docs: payload.docs ?? c.docs,
    context: payload.context ?? c.context,
  };
}

/**
 * Drain unsynced consumption records from local SQLite. ObjectId-filtered.
 * Timestamps converted to ms-since-epoch (SQLite's CURRENT_TIMESTAMP returns
 * UTC formatted as "YYYY-MM-DD HH:MM:SS" — append " UTC" before parsing or
 * JS interprets it as local time and the backend rejects the row).
 *
 * Returns `{records, payload, ids}` so caller can build the POST body and
 * mark the same id set as synced after success.
 *
 * Mirrors v1.2.0 EntraIDDynamicConfigService.fetchDynamicConfig:55-94.
 */
async function drainConsumptionRecords(): Promise<{
  records: Array<{ id: number; model_id: string }>;
  consumption: Array<{
    model_id: string;
    tokens_in: number;
    tokens_out: number;
    client_timestamp: number;
  }>;
}> {
  let unsynced: Awaited<
    ReturnType<typeof DevDataSqliteDb.getUnsyncedTokenRecords>
  > = [];
  try {
    unsynced = await DevDataSqliteDb.getUnsyncedTokenRecords();
  } catch (err) {
    console.warn(
      "[RICA-DYNAMIC] Failed to read unsynced consumption records, continuing without sync:",
      err,
    );
    return { records: [], consumption: [] };
  }

  const before = unsynced.length;
  const filtered = unsynced.filter(
    (r) => typeof r.model_id === "string" && OBJECT_ID_REGEX.test(r.model_id),
  );
  const skipped = before - filtered.length;
  if (skipped > 0) {
    console.warn(
      `[RICA-DYNAMIC] Skipping ${skipped} legacy consumption record(s) with non-ObjectId model_id`,
    );
  }

  const consumption = filtered.map((record) => ({
    model_id: record.model_id,
    tokens_in: record.tokens_in,
    tokens_out: record.tokens_out,
    // ms-epoch fix: SQLite returns UTC string without timezone; appending " UTC"
    // forces correct parsing.
    client_timestamp: new Date(record.timestamp + " UTC").getTime(),
  }));

  return {
    records: filtered.map((r) => ({ id: r.id, model_id: r.model_id })),
    consumption,
  };
}

function buildRequestInit(
  options: RicaDynamicConfigFetchOptions | undefined,
  forcedMethod?: "GET" | "POST",
  forcedBody?: Record<string, unknown>,
): RequestInit {
  const method = forcedMethod ?? options?.method ?? "GET";
  const headers: Record<string, string> = {
    Accept: "application/json",
    "User-Agent": "RICA-Extension/1.0",
  };

  if (options?.accessToken) {
    headers["Authorization"] = `Bearer ${options.accessToken}`;
  }

  if (method === "GET") {
    return { method, headers, redirect: "manual" };
  }

  headers["Content-Type"] = "application/json";
  return {
    method,
    headers,
    redirect: "manual",
    body: JSON.stringify(forcedBody ?? options?.body ?? {}),
  };
}

/**
 * Fetch dynamic configuration from Sang's backend.
 *
 * Behavioural parity with v1.2.0 EntraIDDynamicConfigService.fetchDynamicConfig:
 *  1. Drain SQLite consumption records (ObjectId-filtered, ms-epoch-correct)
 *     and POST them as `{ consumption: [...] }` body.
 *  2. If POST returns 405 / 404 (PROD does not yet accept POST), retry as GET.
 *  3. Reject HTML responses (corporate proxy / Netskope landing page).
 *  4. Reject 3xx redirects (mid-session bearer-token expiry).
 *  5. After 200 OK, mark drained records as synced.
 *  6. Log [CONSUMPTION] with the raw `consumptionLimit` block for every
 *     model (drives multi-agent Tier-S lock — diagnostic ground truth).
 *  7. Return a snapshot carrying the normalised models PLUS every
 *     passthrough field v1.2.0's `validateAndNormalizeConfig` surfaced.
 */
export async function fetchRicaDynamicConfig(
  ideSettingsPromise: Promise<IdeSettings>,
  fetchImpl: typeof fetch = fetch,
  options?: RicaDynamicConfigFetchOptions,
): Promise<RicaDynamicConfigSnapshot> {
  const endpoint = await getRicaModelsEndpoint(ideSettingsPromise);

  // 1. Drain SQLite consumption records
  const drained = await drainConsumptionRecords();
  const requestBody: Record<string, unknown> = {};
  if (drained.consumption.length > 0) {
    requestBody.consumption = drained.consumption;
    console.log(
      `[RICA-DYNAMIC] Sending ${drained.consumption.length} unsynced consumption record(s)`,
    );
  }

  // 2. POST first, fall back to GET on 405/404
  const explicitMethod = options?.method;
  const initialMethod: "GET" | "POST" = explicitMethod ?? "POST";
  const initialBody =
    initialMethod === "POST" ? requestBody : undefined;

  let response = await fetchImpl(
    endpoint,
    buildRequestInit(options, initialMethod, initialBody),
  );

  if (
    initialMethod === "POST" &&
    !explicitMethod &&
    (response.status === 405 || response.status === 404)
  ) {
    console.log(
      "[RICA-DYNAMIC] POST not supported, falling back to GET",
    );
    response = await fetchImpl(endpoint, buildRequestInit(options, "GET"));
  }

  // 3. Reject 3xx redirects (corporate proxy / WAF browser-auth)
  if (response.status >= 300 && response.status < 400) {
    const location = response.headers.get("location") ?? "(unknown)";
    throw new Error(
      `RICA dynamic config was redirected (${response.status}) from ${endpoint} → ${location}. ` +
        `This typically means the corporate proxy or WAF requires browser-based authentication.`,
    );
  }

  if (!response.ok) {
    throw new Error(
      `Failed to fetch RICA dynamic config from ${endpoint}: ${response.status}`,
    );
  }

  // 4. Reject HTML responses (proxy landing page)
  const contentType = response.headers.get("content-type") ?? "";
  if (!contentType.includes("application/json")) {
    throw new Error(
      `RICA dynamic config returned non-JSON response (${contentType}) from ${endpoint}. ` +
        `Likely blocked by corporate proxy (Netskope). Ensure this URL is whitelisted.`,
    );
  }

  const responseText = await response.text();
  if (
    responseText.trim().startsWith("<!DOCTYPE") ||
    responseText.trim().startsWith("<html")
  ) {
    throw new Error(
      `RICA dynamic config returned HTML body (likely an error page) from ${endpoint}. ` +
        `Endpoint may be misconfigured or unreachable.`,
    );
  }

  let payload: RicaDynamicConfigResponse;
  try {
    payload = JSON.parse(responseText) as RicaDynamicConfigResponse;
  } catch (err) {
    throw new Error(
      `RICA dynamic config returned unparseable JSON from ${endpoint}: ${err}`,
    );
  }

  const rawModels = extractModels(payload);
  const normalised = normalizeRicaModels(rawModels);

  // 5. [CONSUMPTION] diagnostic log — ground truth for the multi-agent lock
  try {
    console.log(
      `[CONSUMPTION] backend returned ${rawModels.length} model(s) from ${endpoint}`,
    );
    for (const m of rawModels) {
      const id =
        m?.id ??
        (typeof m?.model === "string" ? m.model.split("/").pop() : "?");
      console.log(
        `[CONSUMPTION]   ${id}  name="${m?.name ?? m?.title}"  consumptionLimit=${JSON.stringify(m?.consumptionLimit)}  multiAgentEligible=${m?.multiAgentEligible}`,
      );
    }
  } catch (e) {
    console.warn("[CONSUMPTION] failed to log raw consumption block:", e);
  }

  // 6. Mark drained records as synced (only after we know the request succeeded)
  if (drained.records.length > 0) {
    try {
      await DevDataSqliteDb.markRecordsAsSynced(
        drained.records.map((r) => r.id),
      );
      console.log(
        `[RICA-DYNAMIC] Marked ${drained.records.length} record(s) as synced`,
      );
    } catch (err) {
      console.warn(
        "[RICA-DYNAMIC] Failed to mark records as synced (will be retried next fetch):",
        err,
      );
    }
  }

  const passthrough = extractPassthrough(payload);
  const updatedAtTop =
    typeof payload.updatedAt === "string" ? payload.updatedAt : undefined;

  return {
    endpoint,
    models: normalised,
    updatedAt: updatedAtTop,
    fetchedAt: new Date().toISOString(),
    ...passthrough,
  };
}
