import { BaseContextProvider } from "../../../core/context";
import {
  ContextItem,
  ContextProviderDescription,
  ContextProviderExtras,
  ContextSubmenuItem,
  LoadSubmenuItemsArgs,
} from "../../../core";
import { ElasticsearchResult } from "./types";
import { INSTRUCTIONS_BASE_ITEM } from "../../../core/context/providers/utils";
import { parseDocIdWithPages } from "./pageNumberParser";
import { RicaDocumentsService } from "./RicaDocumentsService";

class RicaDocsContextProvider extends BaseContextProvider {
  static nRetrieve = 30;
  static nFinal = 15;
  static description: ContextProviderDescription = {
    title: "rica-docs",
    displayTitle: "RICA Docs",
    description: "Search your uploaded RICA documents",
    type: "submenu",
    renderInlineAs: "",
  };

  async getContextItems(
    query: string,
    extras: ContextProviderExtras,
  ): Promise<ContextItem[]> {
    try {
      const ricaDocsService = RicaDocumentsService.getInstance();
      if (!ricaDocsService) {
        console.error("RicaDocumentsService not initialized");
        return [];
      }

      const nRetrieve =
        this.options?.nRetrieve ?? RicaDocsContextProvider.nRetrieve;
      const nFinal = this.options?.nFinal ?? RicaDocsContextProvider.nFinal;
      const useReranking = this.options?.useReranking ?? true;

      const { docIds, pages } = parseDocIdWithPages(query);

      if (docIds.length === 0) return [];

      // If specific pages requested, fetch directly without search/rerank
      if (pages && pages.length > 0) {
        const allResults: ElasticsearchResult[] = [];
        for (const docId of docIds) {
          const pageResponse = await ricaDocsService.queryByPageNumbers(
            docId,
            pages,
          );
          if (pageResponse.error) {
            console.error(
              `RicaDocsContextProvider: Error querying pages for ${docId}:`,
              pageResponse.error,
            );
          } else {
            allResults.push(...pageResponse.results);
          }
        }

        if (allResults.length === 0) {
          void extras.ide.showToast(
            "info",
            `No content found for pages ${pages.join(", ")}`,
          );
          return [];
        }
        return this.buildContextItems(allResults, docIds, ricaDocsService);
      }

      const response = await ricaDocsService.queryDocuments({
        docIds,
        query: extras.fullInput,
        topK: nRetrieve,
      });

      if (response.error) {
        void extras.ide.showToast("error", response.error);
        return [];
      }

      if (response.results.length === 0) {
        void extras.ide.showToast(
          "info",
          "No relevant content found in the selected document",
        );
        return [];
      }

      let results = response.results;

      if (useReranking && extras.reranker) {
        try {
          const allImagePaths: string[] = [];
          results.forEach((r) => {
            if (r.imagePaths && r.imagePaths.length > 0) {
              allImagePaths.push(r.imagePaths[0]);
            }
          });

          let imageCache = new Map<string, string>();
          if (allImagePaths.length > 0) {
            try {
              imageCache =
                await ricaDocsService.fetchImagesAsBase64(allImagePaths);
            } catch {
              // Proceed with text-only reranking
            }
          }

          const chunks = results.map((r) => ({
            digest: r.id,
            content: r.content,
            filepath: `rica://document/${docIds.join(",")}`,
            startLine: 0,
            endLine: 0,
            index: 0,
            imagePaths: r.imagePaths,
            imageIds: r.imageIds,
            imageBase64:
              r.imagePaths && r.imagePaths[0]
                ? imageCache.get(r.imagePaths[0])
                : undefined,
          }));

          const scores = await extras.reranker.rerank(extras.fullInput, chunks);

          const scoredResults = results.map((result, idx) => ({
            result,
            score: scores[idx],
          }));
          scoredResults.sort((a, b) => b.score - a.score);
          results = scoredResults.slice(0, nFinal).map((sr) => sr.result);
        } catch (error) {
          console.warn(
            "RicaDocsContextProvider: Reranking failed, using ES scores",
            error,
          );
          results = results.slice(0, nFinal);
        }
      } else {
        results = results.slice(0, nFinal);
      }

      const allVolumePaths: string[] = [];
      results.forEach((r) => {
        if (r.imagePaths && r.imagePaths.length > 0) {
          allVolumePaths.push(...r.imagePaths);
        }
      });

      let imageBase64Cache = new Map<string, string>();
      let imageLocalPathCache = new Map<string, string>();
      if (allVolumePaths.length > 0) {
        try {
          const imageResults =
            await ricaDocsService.fetchImagesWithLocalPaths(allVolumePaths);
          imageResults.forEach((result, volumePath) => {
            imageBase64Cache.set(volumePath, result.base64);
            imageLocalPathCache.set(volumePath, result.localPath);
          });
        } catch {
          // Continue without images
        }
      }

      const contextItems: ContextItem[] = results.map(
        (result: ElasticsearchResult, index: number) => {
          let content = result.content || "";

          if (result.imagePaths && result.imagePaths.length > 0) {
            const volumePath = result.imagePaths[0];
            const fileName = result.metadata?.file_name || "Unknown";
            const pageName = volumePath.split("/").pop() || "";
            content += `\n\n${pageName} - ${fileName}`;
          }

          const fileId = result.metadata?.file_id || "Unknown";

          let localPath: string | undefined;
          if (result.imagePaths && result.imagePaths.length > 0) {
            localPath = imageLocalPathCache.get(result.imagePaths[0]);
          }

          const imageUrls: string[] = [];
          if (result.imagePaths) {
            result.imagePaths.forEach((volumePath) => {
              const imageBase64 = imageBase64Cache.get(volumePath);
              if (imageBase64) imageUrls.push(imageBase64);
            });
          }

          const contentPreview =
            content.length > 100
              ? content.substring(0, 100).trim() + "..."
              : content.trim();

          return {
            name: `Result ${index + 1} - Score: ${result.score?.toFixed(3) || "N/A"}`,
            description: contentPreview,
            content,
            imageUrls,
            uri: localPath
              ? { type: "file" as const, value: localPath }
              : {
                  type: "url" as const,
                  value: `rica://document/${fileId}#${index}`,
                },
          };
        },
      );

      contextItems.push({
        ...INSTRUCTIONS_BASE_ITEM,
        content:
          "Use the above document excerpts to answer the question. Prioritize " +
          "information from these documents, but supplement with general knowledge if needed. " +
          "If the documents don't contain enough information, say so clearly.",
      });

      return contextItems;
    } catch (error) {
      console.error(
        "RicaDocsContextProvider: Error getting context items",
        error,
      );
      void extras.ide.showToast(
        "error",
        `Failed to retrieve document context: ${error}`,
      );
      return [];
    }
  }

  private async buildContextItems(
    results: ElasticsearchResult[],
    docIds: string[],
    ricaDocsService: RicaDocumentsService,
  ): Promise<ContextItem[]> {
    const allVolumePaths: string[] = [];
    results.forEach((r) => {
      if (r.imagePaths && r.imagePaths.length > 0) {
        allVolumePaths.push(...r.imagePaths);
      }
    });

    let imageBase64Cache = new Map<string, string>();
    let imageLocalPathCache = new Map<string, string>();
    if (allVolumePaths.length > 0) {
      try {
        const imageResults =
          await ricaDocsService.fetchImagesWithLocalPaths(allVolumePaths);
        imageResults.forEach((result, volumePath) => {
          imageBase64Cache.set(volumePath, result.base64);
          imageLocalPathCache.set(volumePath, result.localPath);
        });
      } catch {
        // Continue without images
      }
    }

    const contextItems: ContextItem[] = results.map(
      (result: ElasticsearchResult, index: number) => {
        let content = result.content || "";

        if (result.imagePaths && result.imagePaths.length > 0) {
          const volumePath = result.imagePaths[0];
          const fileName = result.metadata?.file_name || "Unknown";
          const pageName = volumePath.split("/").pop() || "";
          content += `\n\n${pageName} - ${fileName}`;
        }

        const fileId = result.metadata?.file_id || "Unknown";

        let localPath: string | undefined;
        if (result.imagePaths && result.imagePaths.length > 0) {
          localPath = imageLocalPathCache.get(result.imagePaths[0]);
        }

        const imageUrls: string[] = [];
        if (result.imagePaths) {
          result.imagePaths.forEach((volumePath) => {
            const imageBase64 = imageBase64Cache.get(volumePath);
            if (imageBase64) imageUrls.push(imageBase64);
          });
        }

        const contentPreview =
          content.length > 100
            ? content.substring(0, 100).trim() + "..."
            : content.trim();

        return {
          name: `Page ${result.metadata?.page_num || index + 1} - Score: ${result.score?.toFixed(3) || "N/A"}`,
          description: contentPreview,
          content,
          imageUrls,
          uri: localPath
            ? { type: "file" as const, value: localPath }
            : {
                type: "url" as const,
                value: `rica://document/${fileId}#${index}`,
              },
        };
      },
    );

    contextItems.push({
      ...INSTRUCTIONS_BASE_ITEM,
      content:
        "Use the above document excerpts to answer the question. Prioritize " +
        "information from these documents, but supplement with general knowledge if needed. " +
        "If the documents don't contain enough information, say so clearly.",
    });

    return contextItems;
  }

  async loadSubmenuItems(
    args: LoadSubmenuItemsArgs,
  ): Promise<ContextSubmenuItem[]> {
    try {
      const ricaDocsService = RicaDocumentsService.getInstance();
      if (!ricaDocsService) return [];

      const [docsResponse, kbResponse] = await Promise.all([
        ricaDocsService.listDocuments(),
        ricaDocsService.listKnowledgeBases(1, 100),
      ]);

      const submenuItems: ContextSubmenuItem[] = [];

      if (!docsResponse.error && docsResponse.documents) {
        const docItems = docsResponse.documents.map((doc) => ({
          title: doc.name,
          id: doc.id,
          description: doc.uploadedAt
            ? `📄 Document • ${new Date(doc.uploadedAt).toLocaleDateString()}`
            : "📄 Document",
          icon: "file" as const,
        }));
        submenuItems.push(...docItems);
      }

      if (!kbResponse.error && kbResponse.data) {
        const kbItems = kbResponse.data.map((kb: any) => ({
          title: kb.name,
          id: kb.files?.map((f: any) => f.file_id).join(",") || kb.id,
          description: `📚 Knowledge Base • ${kb.files?.length || kb.file_count || 0} files • ${kb.owner || ""}`,
          icon: "folder" as const,
        }));
        submenuItems.push(...kbItems);
      }

      submenuItems.sort((a, b) =>
        a.title.toString().localeCompare(b.title.toString()),
      );

      return submenuItems;
    } catch (error) {
      console.error(
        "RicaDocsContextProvider: Error loading submenu items",
        error,
      );
      return [];
    }
  }
}

export default RicaDocsContextProvider;
