export function parsePageNumbers(input: string): number[] {
  if (!input || input.trim() === "") {
    return [];
  }

  const pageSet = new Set<number>();
  const parts = input.split(",").map((p) => p.trim());

  for (const part of parts) {
    if (part.includes("-")) {
      const [startStr, endStr] = part.split("-").map((s) => s.trim());
      const start = parseInt(startStr, 10);
      const end = parseInt(endStr, 10);

      if (isNaN(start) || isNaN(end)) continue;

      for (let i = Math.min(start, end); i <= Math.max(start, end); i++) {
        pageSet.add(i);
      }
    } else {
      const page = parseInt(part, 10);
      if (isNaN(page)) continue;
      pageSet.add(page);
    }
  }

  return Array.from(pageSet).sort((a, b) => a - b);
}

export function parseDocIdWithPages(query: string): {
  docIds: string[];
  pages: number[] | undefined;
} {
  if (!query || query.trim() === "") {
    return { docIds: [], pages: undefined };
  }

  const colonIndex = query.indexOf(":");
  if (colonIndex === -1) {
    const docIds = query
      .split(",")
      .map((id) => id.trim())
      .filter((id) => id.length > 0);
    return { docIds, pages: undefined };
  }

  const docIdPart = query.substring(0, colonIndex).trim();
  const pagesPart = query.substring(colonIndex + 1).trim();

  const docIds = docIdPart
    .split(",")
    .map((id) => id.trim())
    .filter((id) => id.length > 0);
  const pages = pagesPart.length > 0 ? parsePageNumbers(pagesPart) : undefined;

  return { docIds, pages };
}
