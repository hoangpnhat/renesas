import { Tool } from "../..";
import { BUILT_IN_GROUP_NAME, BuiltInToolNames } from "../builtIn";

export const spawnAgentsTool: Tool = {
  type: "function",
  displayTitle: "Spawn Agents (Batch)",
  wouldLikeTo: "spawn multiple sub-agents in parallel",
  isCurrently: "running {{{ agents.length }}} sub-agents in parallel",
  hasAlready: "completed batch agent spawn",
  readonly: false,
  group: BUILT_IN_GROUP_NAME,
  function: {
    name: BuiltInToolNames.SpawnAgents,
    description:
      "Spawn multiple autonomous sub-agents in parallel with a single tool call. " +
      "Each agent runs independently in its own context with full access to tools. " +
      "Use this instead of calling spawn_agent multiple times — it guarantees all agents " +
      "launch simultaneously via code-level parallelism. " +
      "Do NOT use this for sequential tasks where one agent depends on another's results.",
    parameters: {
      type: "object",
      required: ["agents"],
      properties: {
        agents: {
          type: "array",
          description:
            "Array of agent specifications. Each agent runs independently in parallel. " +
            "Maximum 10 agents per batch.",
          items: {
            type: "object",
            required: ["task", "description"],
            properties: {
              task: {
                type: "string",
                description:
                  "The detailed task instructions for the sub-agent. Be specific about " +
                  "what files to look at, what changes to make, and what the expected outcome is.",
              },
              description: {
                type: "string",
                description:
                  "A SHORT one-line summary (max 80 chars) shown in the UI. Must NOT repeat the task content. " +
                  "Example: 'Fix ISO date regex in dataValidator.js'. Do NOT include steps, code, or details here.",
              },
              role: {
                type: "string",
                description:
                  "The specialized role of this agent, determining its focus and expertise.",
                enum: [
                  "Researcher",
                  "Implementer",
                  "Tester",
                  "Reviewer",
                  "Analyst",
                  "Planner",
                ],
              },
              context: {
                type: "string",
                description:
                  "Structured context from prior agent results or your own analysis. " +
                  "Include specific file paths, line numbers, and findings that will " +
                  "help this agent work more efficiently.",
              },
            },
          },
        },
      },
    },
  },
};
