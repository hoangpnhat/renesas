import { ContextItem, ToolExtras } from "../..";
import { resolveRelativePathInDir } from "../../util/ideUtils";
import { getUriPathBasename } from "../../util/uri";
import { FileTracker } from "../multiagent/FileTracker.js";

/** Fast non-cryptographic hash for content comparison. */
function simpleHash(s: string): string {
  let h = 0;
  for (let i = 0; i < s.length; i++) {
    h = ((h << 5) - h + s.charCodeAt(i)) | 0;
  }
  return h.toString(36);
}

/**
 * Core-side file editing for sub-agents.
 * Unlike the client-side EditExistingFile tool (which uses the GUI's diff UX),
 * this implementation writes files directly via ide.writeFile().
 * Sub-agent edits can be reviewed after the fact via git diff.
 */
export const editFileForSubAgentImpl = async (
  args: { filepath: string; changes: string },
  extras: Pick<ToolExtras, "ide" | "llm" | "fetch" | "tool" | "config"> & {
    agentId?: string;
    agentName?: string;
    sessionId?: string;
  },
): Promise<ContextItem[]> => {
  const fileUri = await resolveRelativePathInDir(args.filepath, extras.ide);

  if (!fileUri) {
    throw new Error(`Could not find file ${args.filepath}`);
  }

  // Read the current file content
  let currentContent: string;
  try {
    currentContent = await extras.ide.readFile(fileUri);
  } catch {
    // File doesn't exist yet — treat changes as full content
    currentContent = "";
  }

  // If the changes look like a complete file (no lazy placeholders), write directly
  // Otherwise, use the LLM to apply the changes to the current content
  const hasLazyPlaceholders =
    args.changes.includes("// ... existing") ||
    args.changes.includes("# ... existing") ||
    args.changes.includes("/* ... existing") ||
    args.changes.includes("// ...rest") ||
    args.changes.includes("# ...rest");

  let newContent: string;

  if (!hasLazyPlaceholders || !currentContent) {
    // Direct write — changes represent the full file content
    newContent = args.changes;
  } else {
    // Use LLM to merge changes into existing file
    const mergePrompt = `Apply the following changes to the file. Return ONLY the complete updated file content, nothing else.

Current file content:
\`\`\`
${currentContent}
\`\`\`

Changes to apply:
\`\`\`
${args.changes}
\`\`\`

Return the complete updated file content:`;

    const mergeResult = await extras.llm.complete(mergePrompt, new AbortController().signal, {
      maxTokens: 8192,
    });
    newContent = mergeResult.trim();

    // Strip any markdown code block wrappers the LLM might add
    if (newContent.startsWith("```")) {
      const firstNewline = newContent.indexOf("\n");
      newContent = newContent.slice(firstNewline + 1);
    }
    if (newContent.endsWith("```")) {
      newContent = newContent.slice(0, newContent.lastIndexOf("```"));
    }
    newContent = newContent.trim();
  }

  // Hash before/after for conflict detection
  const hashBefore = simpleHash(currentContent);
  const hashAfter = simpleHash(newContent);

  await extras.ide.writeFile(fileUri, newContent);

  // Record edit for post-flight conflict detection
  if (extras.sessionId && extras.agentId) {
    FileTracker.forSession(extras.sessionId).recordEdit({
      filepath: fileUri.replace(/\\/g, "/").toLowerCase(),
      agentId: extras.agentId,
      agentName: extras.agentName || extras.agentId,
      timestamp: Date.now(),
      contentHashBefore: hashBefore,
      contentHashAfter: hashAfter,
    });
  }

  return [
    {
      name: getUriPathBasename(args.filepath),
      description: `Edited ${args.filepath}`,
      content: `Successfully edited ${args.filepath}`,
      uri: {
        type: "file",
        value: fileUri,
      },
    },
  ];
};
