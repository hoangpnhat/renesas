import { Tool } from "../..";
import { BUILT_IN_GROUP_NAME, BuiltInToolNames } from "../builtIn";

export const analyzeTaskTool: Tool = {
  type: "function",
  displayTitle: "Analyze Task",
  wouldLikeTo: "analyze task complexity: {{{ task }}}",
  isCurrently: "analyzing task: {{{ task }}}",
  hasAlready: "analyzed task complexity",
  readonly: true,
  group: BUILT_IN_GROUP_NAME,
  function: {
    name: BuiltInToolNames.AnalyzeTask,
    description:
      "Analyze a complex task before spawning agents. Checks file sizes, estimates complexity, " +
      "suggests subtask breakdown with roles, identifies dependencies between subtasks, and " +
      "warns about large files that should be split across multiple agents. " +
      "Use this for complex tasks to plan your agent spawning strategy.",
    parameters: {
      type: "object",
      required: ["task"],
      properties: {
        task: {
          type: "string",
          description:
            "The full task description to analyze. Include specific file paths " +
            "and expected outcomes for better analysis.",
        },
        paths: {
          type: "array",
          items: { type: "string" },
          description:
            "Optional additional file or directory paths to check sizes for. " +
            "Paths mentioned in the task are automatically detected.",
        },
      },
    },
  },
};
