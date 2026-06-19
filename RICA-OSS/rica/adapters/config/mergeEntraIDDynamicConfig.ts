import type { EntraIDDynamicConfig } from "../../../core/index.js";

/**
 * Merge a dynamic-config block from Sang's backend into the existing
 * (runtime) config object, prioritising dynamic config over local. Mirrors
 * v1.2.0's `EntraIDDynamicConfigService.mergeDynamicConfig` byte-for-byte:
 *
 *  - models, embeddingsProvider, tabAutocompleteModel: OVERRIDE if present
 *    in dynamic config (dynamic wins)
 *  - systemMessage: OVERRIDE if present
 *  - completionOptions, requestOptions, ui, experimental, analytics:
 *    SHALLOW MERGE (dynamic overrides individual keys, local fills the rest)
 *  - docs: APPEND (dynamic added to local)
 *  - context: OVERRIDE if non-empty (dynamic wins for context providers)
 *
 * The function is generic over the existing config so it works with both
 * the runtime ContinueConfig and any in-flight assembly types.
 */
export function mergeEntraIDDynamicConfig<
  T extends {
    models?: any[];
    embeddingsProvider?: any;
    tabAutocompleteModel?: any;
  },
>(existingConfig: T, dynamicConfig: EntraIDDynamicConfig | null): T {
  if (!dynamicConfig) {
    return existingConfig;
  }

  const merged = { ...existingConfig };

  // Override models if provided in dynamic config
  if (dynamicConfig.models && dynamicConfig.models.length > 0) {
    merged.models = dynamicConfig.models;
  }

  // Override embeddings provider if provided
  if (dynamicConfig.embeddingsProvider) {
    merged.embeddingsProvider = dynamicConfig.embeddingsProvider;
  }

  // Override tab autocomplete model if provided
  if (dynamicConfig.tabAutocompleteModel) {
    merged.tabAutocompleteModel = dynamicConfig.tabAutocompleteModel;
  }

  // systemMessage: override
  if (dynamicConfig.systemMessage) {
    (merged as any).systemMessage = dynamicConfig.systemMessage;
  }

  // completionOptions: shallow merge
  if (dynamicConfig.completionOptions) {
    (merged as any).completionOptions = {
      ...(merged as any).completionOptions,
      ...dynamicConfig.completionOptions,
    };
  }

  // requestOptions: shallow merge
  if (dynamicConfig.requestOptions) {
    (merged as any).requestOptions = {
      ...(merged as any).requestOptions,
      ...dynamicConfig.requestOptions,
    };
  }

  // ui: shallow merge
  if (dynamicConfig.ui) {
    (merged as any).ui = {
      ...(merged as any).ui,
      ...dynamicConfig.ui,
    };
  }

  // experimental: shallow merge
  if (dynamicConfig.experimental) {
    (merged as any).experimental = {
      ...(merged as any).experimental,
      ...dynamicConfig.experimental,
    };
  }

  // analytics: shallow merge
  if (dynamicConfig.analytics) {
    (merged as any).analytics = {
      ...(merged as any).analytics,
      ...dynamicConfig.analytics,
    };
  }

  // docs: append (don't override — local + dynamic both contribute)
  if (dynamicConfig.docs) {
    (merged as any).docs = [
      ...((merged as any).docs || []),
      ...dynamicConfig.docs,
    ];
  }

  // context: override if non-empty
  if (dynamicConfig.context && dynamicConfig.context.length > 0) {
    (merged as any).context = dynamicConfig.context;
  }

  return merged;
}
