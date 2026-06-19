import { Tool } from "../..";
import { BUILT_IN_GROUP_NAME, BuiltInToolNames } from "../builtIn";

export const spawnAgentTool: Tool = {
  type: "function",
  displayTitle: "Spawn Agent",
  wouldLikeTo: "delegate a task to a sub-agent: {{{ description }}}",
  isCurrently: "running sub-agent: {{{ description }}}",
  hasAlready: "completed sub-agent task: {{{ description }}}",
  readonly: false,
  group: BUILT_IN_GROUP_NAME,
  function: {
    name: BuiltInToolNames.SpawnAgent,
    description:
      "Spawn an autonomous sub-agent to handle a specific task independently. " +
      "The sub-agent runs in its own isolated context with full access to tools " +
      "(read files, edit files, search, run terminal commands, etc.). " +
      "Use this for tasks that require reading files, writing code, or running commands. " +
      "Do NOT use this for greetings, simple questions, or trivial requests — reply directly instead.",
    parameters: {
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
            "help this agent work more efficiently. This context is shared via " +
            "the ContextBridge and made available to the agent.",
        },
      },
    },
  },
};
