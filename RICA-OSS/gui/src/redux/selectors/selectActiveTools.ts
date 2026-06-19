import { createSelector } from "@reduxjs/toolkit";
import { Tool } from "core";
import { BUILT_IN_GROUP_NAME, BuiltInToolNames } from "core/tools/builtIn";
import { DEFAULT_TOOL_SETTING } from "../slices/uiSlice";
import { RootState } from "../store";

// Multi-agent orchestration tools. The orchestrator (multi-agent mode) calls
// these to delegate work; in plain agent mode they must be excluded or the
// model leaks parallel-spawn capability into a single-agent context.
const ORCHESTRATOR_ONLY_TOOLS = [
  BuiltInToolNames.SpawnAgent,
  BuiltInToolNames.SpawnAgents,
  BuiltInToolNames.AnalyzeTask,
];

export const selectActiveTools = createSelector(
  [
    (store: RootState) => store.session.mode,
    (store: RootState) => store.session.workflowPattern,
    (store: RootState) => store.config.config.tools,
    (store: RootState) => store.ui.toolSettings,
    (store: RootState) => store.ui.toolGroupSettings,
  ],
  (mode, workflowPattern, tools, policies, groupPolicies): Tool[] => {
    if (mode === "chat") {
      return [];
    }

    const enabledTools = tools.filter((tool) => {
      const toolPolicy =
        policies[tool.function.name] ??
        tool.defaultToolPolicy ??
        DEFAULT_TOOL_SETTING;
      return (
        toolPolicy !== "disabled" && groupPolicies[tool.group] !== "exclude"
      );
    });

    if (mode === "plan") {
      // Read-only/MCP tools only. Non-readonly built-ins are excluded, which
      // already drops spawn_agent/spawn_agents. AnalyzeTask is readonly so it
      // would otherwise survive — strip orchestrator tools explicitly too.
      return enabledTools.filter(
        (t) =>
          (t.group !== BUILT_IN_GROUP_NAME || t.readonly) &&
          !ORCHESTRATOR_ONLY_TOOLS.includes(
            t.function.name as BuiltInToolNames,
          ),
      );
    }

    if (mode === "agent") {
      // Agent mode: all tools EXCEPT the multi-agent orchestration tools.
      // (v1.2.0 parity — refork's rewrite dropped this exclusion, leaking
      // spawn_agent/spawn_agents/analyze_task into single-agent mode.)
      return enabledTools.filter(
        (t) =>
          !ORCHESTRATOR_ONLY_TOOLS.includes(t.function.name as BuiltInToolNames),
      );
    }

    if (mode === "multi-agent") {
      // In parallel/collaborative patterns, exclude AnalyzeTask — it wastes a
      // turn and the orchestrator should spawn agents immediately.
      const skipAnalyze =
        workflowPattern === "parallel" || workflowPattern === "collaborative";
      // In sequential/iterative patterns, exclude SpawnAgents (batch) — batch
      // doesn't make sense when agents must run one at a time.
      const isSequential =
        workflowPattern === "sequential" || workflowPattern === "iterative";

      return enabledTools.filter((t) => {
        if (skipAnalyze && t.function.name === BuiltInToolNames.AnalyzeTask) {
          return false;
        }
        if (isSequential && t.function.name === BuiltInToolNames.SpawnAgents) {
          return false;
        }
        return true;
      });
    }

    return enabledTools;
  },
);
