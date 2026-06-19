import { ModelDescription, Tool } from "core";
import {
  DEFAULT_AGENT_SYSTEM_MESSAGE,
  DEFAULT_CHAT_SYSTEM_MESSAGE,
  DEFAULT_PLAN_SYSTEM_MESSAGE,
  RICA_IDENTITY,
} from "core/llm/defaultSystemMessages";
import { getBaseSystemMessage, NO_TOOL_WARNING } from "./getBaseSystemMessage";

// Every mode's system message is prefixed with the stable RICA identity line.
const withIdentity = (msg: string) => `${RICA_IDENTITY}\n\n${msg}`;

test("getBaseSystemMessage should return the correct system message based on mode", () => {
  const mockModel = {
    baseChatSystemMessage: "Custom Chat System Message",
    basePlanSystemMessage: "Custom Plan System Message",
    baseAgentSystemMessage: "Custom Agent System Message",
  } as ModelDescription;

  const mockTool = {
    function: {
      name: "testTool",
      description: "Test tool",
      parameters: {},
    },
  } as Tool;

  // Test agent mode with custom message and tools
  expect(getBaseSystemMessage("agent", mockModel, [mockTool])).toBe(
    withIdentity("Custom Agent System Message"),
  );

  // Test plan mode with custom message and tools
  expect(getBaseSystemMessage("plan", mockModel, [mockTool])).toBe(
    withIdentity("Custom Plan System Message"),
  );

  // Test chat mode with custom message and tools
  expect(getBaseSystemMessage("chat", mockModel, [mockTool])).toBe(
    withIdentity("Custom Chat System Message"),
  );

  // Test agent mode with default message and tools
  expect(
    getBaseSystemMessage("agent", {} as ModelDescription, [mockTool]),
  ).toBe(withIdentity(DEFAULT_AGENT_SYSTEM_MESSAGE));

  // Test plan mode with default message and tools
  expect(getBaseSystemMessage("plan", {} as ModelDescription, [mockTool])).toBe(
    withIdentity(DEFAULT_PLAN_SYSTEM_MESSAGE),
  );

  // Test chat mode with default message and tools
  expect(getBaseSystemMessage("chat", {} as ModelDescription, [mockTool])).toBe(
    withIdentity(DEFAULT_CHAT_SYSTEM_MESSAGE),
  );
});

test("getBaseSystemMessage should append no-tools warning for agent/plan modes without tools", () => {
  const mockModel = {
    baseChatSystemMessage: "Custom Chat System Message",
    basePlanSystemMessage: "Custom Plan System Message",
    baseAgentSystemMessage: "Custom Agent System Message",
  } as ModelDescription;

  // Test agent mode without tools
  expect(getBaseSystemMessage("agent", mockModel, [])).toBe(
    withIdentity("Custom Agent System Message" + NO_TOOL_WARNING),
  );

  // Test plan mode without tools
  expect(getBaseSystemMessage("plan", mockModel, [])).toBe(
    withIdentity("Custom Plan System Message" + NO_TOOL_WARNING),
  );

  // Test chat mode without tools (should not append warning)
  expect(getBaseSystemMessage("chat", mockModel, [])).toBe(
    withIdentity("Custom Chat System Message"),
  );

  // Test agent mode with undefined tools
  expect(getBaseSystemMessage("agent", mockModel)).toBe(
    withIdentity("Custom Agent System Message" + NO_TOOL_WARNING),
  );

  // Test plan mode with undefined tools
  expect(getBaseSystemMessage("plan", mockModel)).toBe(
    withIdentity("Custom Plan System Message" + NO_TOOL_WARNING),
  );
});
