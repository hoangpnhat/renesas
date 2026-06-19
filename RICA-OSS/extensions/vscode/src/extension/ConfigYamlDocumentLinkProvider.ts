import * as path from "path";

import * as vscode from "vscode";

export class ConfigYamlDocumentLinkProvider
  implements vscode.DocumentLinkProvider
{
  private usesPattern = /^\s*#?\s*-\s*uses:\s*(.+)$/;
  provideDocumentLinks(
    document: vscode.TextDocument,
    token: vscode.CancellationToken,
  ): vscode.ProviderResult<vscode.DocumentLink[]> {
    const links: vscode.DocumentLink[] = [];

    for (let lineIndex = 0; lineIndex < document.lineCount; lineIndex++) {
      if (token.isCancellationRequested) {
        return [];
      }
      const line = document.lineAt(lineIndex);
      const match = this.usesPattern.exec(line.text);

      if (match) {
        let slug = match[1].trim();
        // Remove any leading comment symbols (#)
        slug = slug.replace(/^\s*(#\s*)+/, "");

        // Check for surrounding quotes
        const quoteMatch = slug.match(/^(['"])(.*)\1/);
        if (quoteMatch) {
          // If quoted, remove the quotes but keep everything inside (including #)
          slug = quoteMatch[2].trim();
        } else {
          // If not quoted, remove any trailing comment
          slug = slug.replace(/\s*#.*$/, "").trim();
        }

        if (slug === "") {
          continue; // Skip empty slugs
        }

        if (/^(https?:\/\/|file:\/\/)/.test(slug)) {
          // VS Code already handles external links, so skip them
          continue;
        }
        const startPos = line.text.indexOf(slug);
        const range = new vscode.Range(
          lineIndex,
          startPos,
          lineIndex,
          startPos + slug.length,
        );

        // Phase 5.1.28 (Group 6 — direct continue.dev hardcodes): the
        // upstream non-relative branch resolved bare slugs (e.g. `uses:
        // continue/some-block`) to `https://continue.dev/${slug}`, the
        // Continue Hub block-browser URL. RICA does not have a hub with
        // sharable block slugs — every Rica config is locally scoped or
        // pulled via Sang's backend — so the hyperlink would dead-end
        // on continue.dev for users who copy a config snippet from
        // upstream docs. Skip non-relative slugs entirely; relative
        // file paths (./ ../) still produce file links below.
        if (!(slug.startsWith("./") || slug.startsWith("../"))) {
          continue;
        }

        const currentFilePath = document.uri.fsPath;
        const parentPath = path.dirname(currentFilePath);
        const resolvedPath = path.resolve(parentPath, slug);
        const linkUri = vscode.Uri.file(resolvedPath);

        const link = new vscode.DocumentLink(range, linkUri);
        links.push(link);
      }
    }

    return links;
  }
  resolveDocumentLink(
    link: vscode.DocumentLink,
    token: vscode.CancellationToken,
  ): vscode.ProviderResult<vscode.DocumentLink> {
    return link;
  }
}
