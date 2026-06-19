// RICA-only logic lives in the overlay (rica/adapters/config/multiAgentTier.ts)
// to keep this upstream-managed directory clean for syncs. This file is a thin
// re-export so existing `core/config/multiAgentTier` import paths keep working
// (gui resolves `core/...` via the node_modules/core symlink; the relative hop
// into rica/ resolves at build time for both the core tsc and the gui bundler).
export {
  DEFAULT_MULTI_AGENT_TIER_S,
  isMultiAgentEligible,
} from "../../rica/adapters/config/multiAgentTier";
