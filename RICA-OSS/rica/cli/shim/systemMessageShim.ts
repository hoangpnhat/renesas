/**
 * RICA CLI System Message Shim
 *
 * Drop-in replacement for extensions/cli/src/systemMessage.ts.
 * Replaces "Continue CLI" identity with RICA identity.
 */

import { execSync } from "child_process";
import * as fs from "fs";
import * as path from "path";

import { parseMarkdownRule, type RuleObject } from "@continuedev/config-yaml";

import { ricaCliEnv } from "../config/cliEnv.js";

const env = { continueHome: ricaCliEnv.ricaHome };

function isGitRepo(): boolean {
  try {
    execSync("git rev-parse --is-inside-work-tree", { stdio: "ignore" });
    return true;
  } catch {
    return false;
  }
}

function getGitStatus(): string {
  try {
    if (!isGitRepo()) {
      return "Not a git repository";
    }
    const result = execSync("git status --porcelain", {
      encoding: "utf-8",
      cwd: process.cwd(),
    });
    return result.trim() || "Working tree clean";
  } catch {
    return "Git status not available";
  }
}

const baseSystemMessage = `You are RICA, Renesas's AI coding assistant integrated into the CLI. When asked who or what you are, identify yourself as RICA — never as any other product.

Given the user's prompt, you should use the tools available to you to answer the user's question.

Notes:
1. IMPORTANT: You should be concise, direct, and to the point, since your responses will be displayed on a command line interface.
2. When relevant, share file names and code snippets relevant to the query
Here is useful information about the environment you are running in:
<env>
Working directory: ${process.cwd()}
Is directory a git repo: ${isGitRepo()}
Platform: ${process.platform}
Today's date: ${new Date().toISOString().split("T")[0]}
</env>

As you answer the user's questions, you can use the following context:

<context name="gitStatus">This is the git status at the start of the conversation. Note that this status is a snapshot in time, and will not update during the conversation.

${getGitStatus()}
</context>`;

async function getConfigYamlRules(): Promise<string[]> {
  return [];
}

function getRuleNameFromPath(filePath: string): string {
  const segments = filePath.split(/[/\\]/);
  const lastTwoParts = segments.slice(-2);
  return lastTwoParts.filter(Boolean).join("/").replace(/\.md$/, "");
}

export function loadMarkdownRulesWithMetadata(): RuleObject[] {
  const cwd = process.cwd();
  const rulesDirs = [
    path.join(cwd, ".continue", "rules"),
    path.join(env.continueHome, "rules"),
  ];

  const rules: RuleObject[] = [];

  for (const dir of rulesDirs) {
    if (!fs.existsSync(dir)) continue;

    let files: string[];
    try {
      files = fs.readdirSync(dir, { recursive: true }) as string[];
    } catch {
      continue;
    }

    for (const file of files) {
      if (!String(file).endsWith(".md")) continue;

      const filePath = path.join(dir, String(file));
      try {
        const stat = fs.statSync(filePath);
        if (!stat.isFile()) continue;
      } catch {
        continue;
      }

      try {
        const content = fs.readFileSync(filePath, "utf-8");
        const { frontmatter, markdown } = parseMarkdownRule(content);

        if (frontmatter.invokable) continue;

        const isAlwaysApply =
          frontmatter.alwaysApply === true ||
          (frontmatter.alwaysApply === undefined &&
            !frontmatter.globs &&
            !frontmatter.regex);

        if (isAlwaysApply && markdown.trim()) {
          const ruleName =
            frontmatter.name || getRuleNameFromPath(String(file));
          rules.push({
            name: ruleName,
            rule: markdown,
            description: frontmatter.description,
            globs: frontmatter.globs,
            regex: frontmatter.regex,
            alwaysApply: true,
            sourceFile: filePath,
          });
        }
      } catch {
        // Skip files that can't be read or parsed
      }
    }
  }

  return rules;
}

export async function constructSystemMessage(
  mode: string,
  additionalRules?: string[],
  format?: "json",
  headless?: boolean,
): Promise<string> {
  const agentFiles = ["AGENTS.md", "AGENT.md", "CLAUDE.md", "CODEX.md"];

  let agentContent = "";

  try {
    for (const fileName of agentFiles) {
      const filePath = path.join(process.cwd(), fileName);

      if (fs.existsSync(filePath)) {
        agentContent = fs.readFileSync(filePath, "utf-8");
        break;
      }
    }
  } catch {
    // Continue without agent content
  }

  const processedRules: string[] = [];
  if (additionalRules && additionalRules.length > 0) {
    processedRules.push(...additionalRules);
  }

  const configYamlRules = await getConfigYamlRules();
  processedRules.push(...configYamlRules);

  const markdownRules = loadMarkdownRulesWithMetadata();
  const existingRulesSet = new Set(processedRules);
  for (const rule of markdownRules) {
    if (!existingRulesSet.has(rule.rule)) {
      processedRules.push(rule.rule);
      existingRulesSet.add(rule.rule);
    }
  }

  let systemMessage = baseSystemMessage;

  if (mode === "plan") {
    systemMessage +=
      '\n<context name="planMode">You are operating in _Plan Mode_, which means that your goal is to help the user investigate their ideas and develop a plan before taking action. You only have access to read-only tools and should not attempt to circumvent them to write / delete / create files. Ask the user to switch to agent mode if they want to make changes.</context>\n';
  } else {
    if (!process.env.RICA_CLI_DISABLE_COMMIT_SIGNATURE) {
      systemMessage += `\n<context name="commitSignature">When creating commits using any CLI or tool, include the following in the commit message:
Generated with RICA

Co-Authored-By: RICA <noreply@renesas.com>
</context>\n`;
    }
  }

  if (headless) {
    systemMessage += `

IMPORTANT: You are running in headless mode. Provide ONLY your final answer to the user's question. Do not include explanations, reasoning, or additional commentary unless specifically requested. Be direct and concise.`;
  }

  if (format === "json") {
    systemMessage += `

IMPORTANT: You are operating in JSON output mode. Your final response MUST be valid JSON that can be parsed by JSON.parse(). The JSON should contain properties relevant to answer the user's question. Do not include any text before or after the JSON - the entire response must be parseable JSON.

Example response format:
{
  "property": "value"
}`;
  }

  if (agentContent || processedRules.length > 0) {
    systemMessage += '\n\n<context name="userRules">';

    if (agentContent) {
      systemMessage += `\n${agentContent}`;
    }

    if (processedRules.length > 0) {
      const separator = agentContent ? "\n\n" : "\n";
      systemMessage += `${separator}${processedRules.join("\n\n")}`;
    }

    systemMessage += "\n</context>";
  }

  return systemMessage;
}
