/**
 * Backward-compatible re-export of AgentManager as SubAgentRegistry.
 * AgentManager extends the original SubAgentRegistry with auto-naming,
 * deduplication, and versioning while maintaining the same API surface.
 */
export { AgentManager as SubAgentRegistry } from "./AgentManager.js";
