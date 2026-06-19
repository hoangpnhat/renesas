import { createSelector } from "@reduxjs/toolkit";
import { RootState } from "../store";

export interface LLMCallDetail {
  index: number;
  model: string;
  promptTokens: number;
  completionTokens: number;
  reasoningTokens: number;
  cacheReadTokens: number;
  cacheCreationTokens: number;
  totalTokens: number;
  isActualUsage: boolean;
}

export const selectSessionTokens = createSelector(
  [
    (state: RootState) => state.session.history,
    (state: RootState) => state.session.subAgentProgress,
  ],
  (history, subAgentProgress) => {
    // Sum from all promptLogs across history items (orchestrator/chat/agent)
    let orchestratorPrompt = 0;
    let orchestratorCompletion = 0;
    let orchestratorReasoning = 0;
    let orchestratorCacheRead = 0;
    let orchestratorCacheCreation = 0;
    let hasAnyActualUsage = false;
    let hasAnyEstimated = false;

    // Per-call breakdown
    const llmCalls: LLMCallDetail[] = [];
    let callIndex = 0;

    for (const item of history) {
      if (item.promptLogs) {
        for (const log of item.promptLogs) {
          const pt = log.promptTokens ?? 0;
          const ct = log.completionTokens ?? 0;
          const rt = (log as any).reasoningTokens ?? 0;
          const cr = (log as any).cacheReadTokens ?? 0;
          const cc = (log as any).cacheCreationTokens ?? 0;
          const actual = (log as any).isActualUsage ?? false;

          orchestratorPrompt += pt;
          orchestratorCompletion += ct;
          orchestratorReasoning += rt;
          orchestratorCacheRead += cr;
          orchestratorCacheCreation += cc;

          if (actual) {
            hasAnyActualUsage = true;
          } else if (pt > 0 || ct > 0) {
            hasAnyEstimated = true;
          }

          if (pt > 0 || ct > 0) {
            llmCalls.push({
              index: ++callIndex,
              model: log.modelTitle || "unknown",
              promptTokens: pt,
              completionTokens: ct,
              reasoningTokens: rt,
              cacheReadTokens: cr,
              cacheCreationTokens: cc,
              totalTokens: pt + ct,
              isActualUsage: actual,
            });
          }
        }
      }
    }

    // Sum from all sub-agent progress records
    let agentPrompt = 0;
    let agentCompletion = 0;
    let agentReasoning = 0;
    let agentCacheRead = 0;
    let agentCacheCreation = 0;
    for (const agent of Object.values(subAgentProgress)) {
      if (agent.tokenUsage) {
        agentPrompt += agent.tokenUsage.promptTokens;
        agentCompletion += agent.tokenUsage.completionTokens;
        agentReasoning += (agent.tokenUsage as any).reasoningTokens ?? 0;
        agentCacheRead += (agent.tokenUsage as any).cacheReadTokens ?? 0;
        agentCacheCreation += (agent.tokenUsage as any).cacheCreationTokens ?? 0;
        if ((agent.tokenUsage as any).isActualUsage) {
          hasAnyActualUsage = true;
        } else if (agent.tokenUsage.totalTokens > 0) {
          hasAnyEstimated = true;
        }
      }
    }

    const orchestratorTotal = orchestratorPrompt + orchestratorCompletion;
    const agentTotal = agentPrompt + agentCompletion;

    const totalCacheRead = orchestratorCacheRead + agentCacheRead;
    const totalCacheCreation = orchestratorCacheCreation + agentCacheCreation;

    // Determine usage source: "api" = all from API, "estimated" = all tiktoken, "mixed" = both
    let usageSource: "api" | "estimated" | "mixed" = "estimated";
    if (hasAnyActualUsage && !hasAnyEstimated) {
      usageSource = "api";
    } else if (hasAnyActualUsage && hasAnyEstimated) {
      usageSource = "mixed";
    }

    return {
      orchestratorTokens: {
        promptTokens: orchestratorPrompt,
        completionTokens: orchestratorCompletion,
        reasoningTokens: orchestratorReasoning,
        cacheReadTokens: orchestratorCacheRead,
        cacheCreationTokens: orchestratorCacheCreation,
        total: orchestratorTotal,
      },
      agentTokens: {
        promptTokens: agentPrompt,
        completionTokens: agentCompletion,
        reasoningTokens: agentReasoning,
        cacheReadTokens: agentCacheRead,
        cacheCreationTokens: agentCacheCreation,
        total: agentTotal,
      },
      sessionTotal: orchestratorTotal + agentTotal,
      totalReasoning: orchestratorReasoning + agentReasoning,
      totalCacheRead,
      totalCacheCreation,
      usageSource,
      llmCalls,
      llmCallCount: llmCalls.length,
      agentCount: Object.keys(subAgentProgress).length,
    };
  },
);
