import { fetchwithRequestOptions } from "@continuedev/fetch";
import { ChatMessage, IDE, PromptLog } from "..";
import { ConfigHandler } from "../config/ConfigHandler";
import { usesCreditsBasedApiKey } from "../config/usesFreeTrialApiKey";
import { FromCoreProtocol, ToCoreProtocol } from "../protocol";
import { IMessenger, Message } from "../protocol/messenger";
import { MultiAgentLogger } from "../tools/multiagent/MultiAgentLogger";
import { Telemetry } from "../util/posthog";
import { TTS } from "../util/tts";
import { isOutOfStarterCredits } from "./utils/starterCredits";

// Network-level errors that warrant retrying the LLM stream — observed
// during long sub-agent runs where backend load balancers drop the
// connection mid-stream. Anything else (auth, validation, abort) is fatal.
const STREAM_RETRYABLE_PATTERNS = [
  /premature close/i,
  /socket hang up/i,
  /ECONNRESET/,
  /EPIPE/,
  /fetch failed/i,
  /network error/i,
  /ETIMEDOUT/,
];

const STREAM_MAX_RETRIES = 2;
const STREAM_RETRY_DELAYS = [2000, 5000];

export async function* llmStreamChat(
  configHandler: ConfigHandler,
  abortController: AbortController,
  msg: Message<ToCoreProtocol["llm/streamChat"][0]>,
  ide: IDE,
  messenger: IMessenger<ToCoreProtocol, FromCoreProtocol>,
): AsyncGenerator<ChatMessage, PromptLog> {
  const { config } = await configHandler.loadConfig();
  if (!config) {
    throw new Error("Config not loaded");
  }

  // Stop TTS on new StreamChat
  if (config.experimental?.readResponseTTS) {
    void TTS.kill();
  }

  const {
    legacySlashCommandData,
    completionOptions,
    messages,
    messageOptions,
  } = msg.data;

  const model = config.selectedModelByRole.chat;

  if (!model) {
    throw new Error("No chat model selected");
  }

  // Log orchestrator entry point for multi-agent tracing
  const userPrompt = messages.filter((m) => m.role === "user").pop();
  const userPromptText =
    typeof userPrompt?.content === "string" ? userPrompt.content : "";
  console.log(
    `[StreamChat] model=${model.model}, provider=${model.providerName}, msgs=${messages.length}, userPrompt=${userPromptText.slice(0, 100)}`,
  );

  // Always log to structured logger so session-level fields are set
  // before sub-agents run (spawnAgents sets model/workflow too, but
  // this captures the original user prompt from the orchestrator).
  if (userPromptText) {
    try {
      const sessionId = await messenger.request(
        "getCurrentSessionId",
        undefined,
      );
      if (sessionId) {
        const logger = MultiAgentLogger.forSession(sessionId);
        logger.setUserPrompt(userPromptText);
        logger.setModel(`${model.providerName}/${model.model}`);
      }
    } catch {
      /* session ID not available */
    }
  }

  // Log to return in case of error
  const errorPromptLog = {
    modelTitle: model?.title ?? model?.model,
    modelProvider: model?.underlyingProviderName ?? "unknown",
    completion: "",
    prompt: "",
    completionOptions: {
      ...msg.data.completionOptions,
      model: model?.model,
    },
  };

  try {
    if (legacySlashCommandData) {
      const { command, contextItems, historyIndex, input, selectedCode } =
        legacySlashCommandData;
      const slashCommand = config.slashCommands?.find(
        (sc) => sc.name === command.name,
      );
      if (!slashCommand) {
        throw new Error(`Unknown slash command ${command.name}`);
      }
      if (!slashCommand.run) {
        console.error(
          `Slash command ${command.name} (${command.source}) has no run function`,
        );
        throw new Error(`Slash command not found`);
      }

      const gen = slashCommand.run({
        input,
        history: messages,
        llm: model,
        contextItems,
        params: command.params,
        ide,
        addContextItem: (item) => {
          void messenger.request("addContextItem", {
            item,
            historyIndex,
          });
        },
        selectedCode,
        config,
        fetch: (url, init) =>
          fetchwithRequestOptions(
            url,
            {
              ...init,
              signal: abortController.signal,
            },
            model.requestOptions,
          ),
        completionOptions,
        abortController,
      });
      let next = await gen.next();
      while (!next.done) {
        if (abortController.signal.aborted) {
          next = await gen.return(errorPromptLog);
          break;
        }
        if (next.value) {
          yield {
            role: "assistant",
            content: next.value,
          };
        }
        next = await gen.next();
      }
      if (!next.done) {
        throw new Error("Will never happen");
      }

      return next.value;
    } else {
      // 90s without a single chunk = backend/proxy went silent; abort so
      // the orchestrator doesn't sit forever staring at a dead socket.
      const STALE_STREAM_TIMEOUT_MS = 90_000;

      let lastError: Error | null = null;
      for (let attempt = 0; attempt <= STREAM_MAX_RETRIES; attempt++) {
        if (attempt > 0) {
          const delay = STREAM_RETRY_DELAYS[attempt - 1] || 5000;
          console.warn(
            `[StreamChat] Retry ${attempt}/${STREAM_MAX_RETRIES} after ${delay}ms...`,
          );
          await new Promise((resolve) => setTimeout(resolve, delay));

          if (abortController.signal.aborted) {
            break;
          }
        }

        try {
          const gen = model.streamChat(
            messages,
            abortController.signal,
            completionOptions,
            messageOptions,
          );

          let staleTimer: ReturnType<typeof setTimeout> | null = null;
          const resetStaleTimer = () => {
            if (staleTimer) clearTimeout(staleTimer);
            staleTimer = setTimeout(() => {
              console.warn(
                `[StreamChat] No data received for ${STALE_STREAM_TIMEOUT_MS / 1000}s — aborting stale LLM call`,
              );
              abortController.abort();
            }, STALE_STREAM_TIMEOUT_MS);
          };

          resetStaleTimer();
          let next = await gen.next();
          while (!next.done) {
            resetStaleTimer();
            if (abortController.signal.aborted) {
              next = await gen.return(errorPromptLog);
              break;
            }

            const chunk = next.value;
            yield chunk;
            next = await gen.next();
          }
          if (staleTimer) clearTimeout(staleTimer);
          if (
            config.experimental?.readResponseTTS &&
            "completion" in next.value
          ) {
            void TTS.read(next.value?.completion);
          }

          void Telemetry.capture(
            "chat",
            {
              model: model.model,
              provider: model.providerName,
            },
            true,
          );

          void checkForOutOfStarterCredits(configHandler, messenger);

          if (!next.done) {
            throw new Error("Will never happen");
          }

          return next.value;
        } catch (streamError: any) {
          lastError = streamError;
          const errorMsg =
            streamError?.message || streamError?.toString() || "";

          if (abortController.signal.aborted) {
            throw streamError;
          }

          const isRetryable = STREAM_RETRYABLE_PATTERNS.some((p) =>
            p.test(errorMsg),
          );
          if (!isRetryable || attempt >= STREAM_MAX_RETRIES) {
            console.error(
              `[StreamChat] Non-retryable error or max retries reached: ${errorMsg}`,
            );
            throw streamError;
          }

          console.warn(
            `[StreamChat] Retryable error on attempt ${attempt + 1}: ${errorMsg}`,
          );
        }
      }

      throw lastError || new Error("Stream failed after retries");
    }
  } catch (error) {
    // Moved error handling that was here to GUI, keeping try/catch for clean diff
    throw error;
  }
}

async function checkForOutOfStarterCredits(
  configHandler: ConfigHandler,
  messenger: IMessenger<ToCoreProtocol, FromCoreProtocol>,
) {
  try {
    const { config } = await configHandler.getSerializedConfig();
    const creditStatus =
      await configHandler.controlPlaneClient.getCreditStatus();

    if (
      config &&
      creditStatus &&
      isOutOfStarterCredits(usesCreditsBasedApiKey(config), creditStatus)
    ) {
      void messenger.request("freeTrialExceeded", undefined);
    }
  } catch (error) {
    console.error("Error checking free trial status:", error);
  }
}
