import { ModelDescription } from "core";
import { isMultiAgentEligible } from "core/config/multiAgentTier";

/**
 * Determines if a blocked model should auto-switch to an unblocked alternative.
 * Pure function for testability.
 *
 * @param activeModel The currently selected model for a role
 * @param alternativeModels All available models for the role
 * @param role The role being checked
 * @param isSessionLocked True when the current session has committed to this
 *   model (multi-agent Tier S lock). When true, never auto-switch — the user
 *   chose this model deliberately and a silent fallback would violate the
 *   "no surprise downgrade" guarantee.
 * @returns The first unblocked model to switch to, or null if no switch needed
 */
export function findUnblockedAlternative(
  activeModel: ModelDescription | null | undefined,
  alternativeModels: ModelDescription[],
  role: "chat" | "edit" | string,
  isSessionLocked: boolean = false,
): ModelDescription | null {
  // Session lock: caller has committed to this model — do not auto-switch
  if (isSessionLocked) {
    return null;
  }

  // Only applicable for chat and edit roles
  if (role !== "chat" && role !== "edit") {
    return null;
  }

  // If no active model, no switch needed
  if (!activeModel) {
    return null;
  }

  // Check if active model is blocked
  const isActiveBlocked =
    activeModel.consumptionLimit?.enabled === true &&
    activeModel.consumptionLimit?.blocked === true;

  if (!isActiveBlocked) {
    return null;
  }

  // Find first unblocked alternative
  const unblocked = alternativeModels.find(
    (model) =>
      model.consumptionLimit?.enabled !== true ||
      model.consumptionLimit?.blocked !== true,
  );

  return unblocked ?? null;
}

/**
 * Returns the first Tier S (multi-agent eligible) model that is NOT
 * `blocked: true`. Used by ModeSelect to gate the multi-agent option:
 * when this returns null AND the current chat model isn't a usable
 * Tier S, the multi-agent dropdown entry should be disabled.
 *
 * "Unblocked" here means `consumptionLimit.blocked !== true` — so models
 * with no consumptionLimit at all also count as unblocked.
 */
export function findUnblockedTierSModel(
  chatModels: ModelDescription[],
): ModelDescription | null {
  return (
    chatModels.find(
      (m) =>
        isMultiAgentEligible(m as any) &&
        m.consumptionLimit?.blocked !== true,
    ) ?? null
  );
}
