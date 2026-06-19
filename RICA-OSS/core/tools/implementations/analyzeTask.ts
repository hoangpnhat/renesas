import { ToolImpl } from ".";
import { TaskDecomposer } from "../multiagent/TaskDecomposer.js";

const decomposer = new TaskDecomposer();

export const analyzeTaskImpl: ToolImpl = async (args, extras) => {
  const { task, paths } = args;

  if (!task) {
    throw new Error("AnalyzeTask requires a 'task' parameter.");
  }

  const analysis = await decomposer.analyzeTask(
    task,
    extras.ide,
    paths,
  );

  const formatted = decomposer.formatAnalysis(analysis);

  return [
    {
      name: "Task Analysis",
      description: `Complexity: ${analysis.estimatedComplexity}, Pattern: ${analysis.suggestedPattern}, ${analysis.subtasks.length} subtask(s)`,
      content: formatted,
    },
  ];
};
