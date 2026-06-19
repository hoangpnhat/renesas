/**
 * RICA Authentication Types
 *
 * This file re-exports Continue's auth types which have been extended
 * with RICA-specific auth types (RicaProd, RicaStaging) in core/control-plane/AuthTypes.ts.
 *
 * Architecture Decision:
 * RICA auth types are defined in core/control-plane/AuthTypes.ts (minimal patch)
 * rather than here to maintain type compatibility with core protocol.
 * See rica/vscode/patches/INTEGRATION_PATCHES.md for rationale.
 */

// Re-export all types from core for RICA usage
export {
  AuthType,
  ControlPlaneSessionInfo,
  HubSessionInfo,
  OnPremSessionInfo,
  HubEnv,
  OnPremEnv,
  ControlPlaneEnv,
  isOnPremSession,
} from "../../../core/control-plane/AuthTypes";

/**
 * Type guard to check if environment is Hub (not OnPrem)
 */
import {
  AuthType,
  ControlPlaneEnv,
} from "../../../core/control-plane/AuthTypes";

export function isHubEnv(
  env: ControlPlaneEnv,
): env is Extract<
  ControlPlaneEnv,
  { AUTH_TYPE: Exclude<AuthType, AuthType.OnPrem> }
> {
  return "AUTH_TYPE" in env && env.AUTH_TYPE !== AuthType.OnPrem;
}
