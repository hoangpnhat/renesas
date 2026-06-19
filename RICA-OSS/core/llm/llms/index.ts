import Handlebars from "handlebars";
import {
  BaseCompletionOptions,
  IdeSettings,
  ILLM,
  ILLMLogger,
  JSONModelDescription,
  LLMOptions,
} from "../..";
import { renderTemplatedString } from "../../util/handlebars/renderTemplatedString";
import { BaseLLM } from "../index";
import Anthropic from "./Anthropic";
import Asksage from "./Asksage";
import Azure from "./Azure";
import Bedrock from "./Bedrock";
import BedrockImport from "./BedrockImport";
import Cerebras from "./Cerebras";
import ClawRouter from "./ClawRouter";
import Cloudflare from "./Cloudflare";
import Cohere from "./Cohere";
import CometAPI from "./CometAPI";
import DeepInfra from "./DeepInfra";
import Deepseek from "./Deepseek";
import Docker from "./Docker";
import Fireworks from "./Fireworks";
import Flowise from "./Flowise";
import FunctionNetwork from "./FunctionNetwork";
import Gemini from "./Gemini";
import Groq from "./Groq";
import HuggingFaceInferenceAPI from "./HuggingFaceInferenceAPI";
import HuggingFaceTEIEmbeddingsProvider from "./HuggingFaceTEI";
import HuggingFaceTGI from "./HuggingFaceTGI";
import Inception from "./Inception";
import Kindo from "./Kindo";
import Lemonade from "./Lemonade";
import LlamaCpp from "./LlamaCpp";
import Llamafile from "./Llamafile";
import LlamaStack from "./LlamaStack";
import LMStudio from "./LMStudio";
import Mimo from "./Mimo";
import MiniMax from "./MiniMax";
import Mistral from "./Mistral";
import MockLLM from "./Mock";
import Moonshot from "./Moonshot";
import Msty from "./Msty";
import NCompass from "./NCompass";
import Nebius from "./Nebius";
import Nous from "./Nous";
import Novita from "./Novita";
import Nvidia from "./Nvidia";
import Ollama from "./Ollama";
import OpenAI from "./OpenAI";
import OpenRouter from "./OpenRouter";
import OVHcloud from "./OVHcloud";
// RICA: register RicaProxy in LLMClasses so models with provider "rica-proxy"
// (the default for RICA dynamic-config models) resolve to the Rica adapter
// instead of returning undefined from getModelClass.
import RicaProxy from "../../../rica/adapters/llm/RicaProxy";
import { Relace } from "./Relace";
import Replicate from "./Replicate";
import SageMaker from "./SageMaker";
import SambaNova from "./SambaNova";
import Scaleway from "./Scaleway";
import SiliconFlow from "./SiliconFlow";
// RICA (LLMClass severed): ContinueProxy is the `continue-proxy` provider
// class — it sets apiBase to `${controlPlaneProxyUrl}/model-proxy/v1/` (i.e.
// api.continue.dev) and posts a Bearer token from a WorkOS access token. In
// the RICA build no model ever lands with providerName === "continue-proxy"
// (the apiKey/env scrub + the databricks→rica-proxy re-point both run first),
// so the class is dead code at runtime. Removed from LLMClasses as
// defence-in-depth: if a stray config sneaks in with `provider: continue-proxy`,
// getModelClass returns undefined and the model fails to instantiate (clear
// surfaceable error) instead of quietly POSTing to api.continue.dev. Source
// file (./stubs/ContinueProxy.ts) is left on disk because doLoadConfig.ts
// retains type-cast references; removing the class would break compile for
// those casts.
import TARS from "./TARS";
import Tensorix from "./Tensorix";
import TestLLM from "./Test";
import TextGenWebUI from "./TextGenWebUI";
import Together from "./Together";
import Venice from "./Venice";
import VertexAI from "./VertexAI";
import Vllm from "./Vllm";
import Voyage from "./Voyage";
import WatsonX from "./WatsonX";
import xAI from "./xAI";
import zAI from "./zAI";
export const LLMClasses = [
  Anthropic,
  Cohere,
  CometAPI,
  FunctionNetwork,
  Gemini,
  Llamafile,
  Moonshot,
  Ollama,
  Replicate,
  TextGenWebUI,
  Together,
  Novita,
  HuggingFaceTGI,
  HuggingFaceTEIEmbeddingsProvider,
  HuggingFaceInferenceAPI,
  Kindo,
  LlamaCpp,
  OpenAI,
  OVHcloud,
  Lemonade,
  LMStudio,
  Mistral,
  Mimo,
  MiniMax,
  Bedrock,
  BedrockImport,
  SageMaker,
  DeepInfra,
  Flowise,
  Groq,
  Fireworks,
  NCompass,
  // ContinueProxy removed in RICA sever. See header import.
  Cloudflare,
  Deepseek,
  Docker,
  Msty,
  Azure,
  WatsonX,
  OpenRouter,
  ClawRouter,
  Nvidia,
  Vllm,
  SambaNova,
  MockLLM,
  TestLLM,
  Cerebras,
  Asksage,
  Nebius,
  Nous,
  Venice,
  VertexAI,
  xAI,
  SiliconFlow,
  Tensorix,
  Scaleway,
  Relace,
  Inception,
  Voyage,
  LlamaStack,
  TARS,
  zAI,
  RicaProxy,
];

function normalizeProviderName(providerName: string): string {
  // (close Trap B, deep hop): the second `databricks → openai`
  // rewrite — this one bypasses rica/adapters/config/modelMapping.ts and
  // fires inside the Continue core LLM factory. Without removing it,
  // any model surfaced as `provider: "databricks"` (e.g. a config.yaml
  // hand-rolled by a developer, or a future backend payload) would
  // re-route through the upstream `OpenAI` class with apiBase =
  // https://api.openai.com/v1/. RICA models reach this factory via the
  // dynamic-config flow (provider `rica-proxy`), so the alias is dead
  // code for the runtime path AND a foot-gun for any future databricks-
  // labelled model. Drop it.
  return providerName;
}

export async function llmFromDescription(
  desc: JSONModelDescription,
  readFile: (filepath: string) => Promise<string>,
  getUriFromPath: (path: string) => Promise<string | undefined>,
  uniqueId: string,
  ideSettings: IdeSettings,
  llmLogger: ILLMLogger,
  completionOptions?: BaseCompletionOptions,
): Promise<BaseLLM | undefined> {
  const normalizedProvider = normalizeProviderName(desc.provider);
  const normalizedDesc =
    normalizedProvider === desc.provider
      ? desc
      : { ...desc, provider: normalizedProvider };
  const cls = LLMClasses.find(
    (llm) => llm.providerName === normalizedDesc.provider,
  );

  if (!cls) {
    return undefined;
  }

  const finalCompletionOptions = {
    ...completionOptions,
    ...normalizedDesc.completionOptions,
  };

  let baseChatSystemMessage: string | undefined = undefined;
  if (normalizedDesc.systemMessage !== undefined) {
    // baseChatSystemMessage = DEFAULT_CHAT_SYSTEM_MESSAGE;
    // baseChatSystemMessage += "\n\n";
    baseChatSystemMessage = await renderTemplatedString(
      Handlebars,
      normalizedDesc.systemMessage,
      {},
      [],
      readFile,
      getUriFromPath,
    );
  }

  let options: LLMOptions = {
    ...normalizedDesc,
    completionOptions: {
      ...finalCompletionOptions,
      model:
        (normalizedDesc.model || cls.defaultOptions?.model) ?? "codellama-7b",
      maxTokens:
        finalCompletionOptions.maxTokens ??
        cls.defaultOptions?.completionOptions?.maxTokens,
    },
    baseChatSystemMessage,
    basePlanSystemMessage: baseChatSystemMessage,
    baseAgentSystemMessage: baseChatSystemMessage,
    logger: llmLogger,
    uniqueId,
  };

  if (normalizedDesc.provider === "continue-proxy") {
    options.apiKey = ideSettings.userToken;
    if (ideSettings.remoteConfigServerUrl) {
      options.apiBase = new URL(
        "/proxy/v1",
        ideSettings.remoteConfigServerUrl,
      ).toString();
    }
  }

  return new cls(options);
}

export function llmFromProviderAndOptions(
  providerName: string,
  llmOptions: LLMOptions,
): ILLM {
  const normalizedProviderName = normalizeProviderName(providerName);
  const cls = LLMClasses.find(
    (llm) => llm.providerName === normalizedProviderName,
  );

  if (!cls) {
    throw new Error(`Unknown LLM provider type "${providerName}"`);
  }

  return new cls(llmOptions);
}
