/**
 * RICA Control Plane Environment Configuration
 *
 * This file re-exports from RICA adapter to override Continue's upstream config.
 * VSCode extension files should import from this file instead of core/control-plane/env
 */

export {
  EXTENSION_NAME,
  getControlPlaneEnv,
  getControlPlaneEnvSync,
  useHub,
  enableHubContinueDev,
} from "../../../rica/adapters/config/env";

// Re-export types
export type {
  ControlPlaneEnv,
  AuthType,
} from "../../../rica/adapters/config/env";
