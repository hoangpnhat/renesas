import {
  DocumentTextIcon,
  FolderIcon,
  ArrowPathIcon,
  XMarkIcon,
  ArrowLeftIcon,
  MagnifyingGlassIcon,
} from "@heroicons/react/24/outline";
import { useCallback, useContext, useEffect, useRef, useState } from "react";
import { IdeMessengerContext } from "../../context/IdeMessenger";
import { useMainEditor } from "../../components/mainInput/TipTapEditor/MainEditorProvider";
import { Mention } from "../../components/mainInput/TipTapEditor/extensions";

interface RicaDocument {
  id: string;
  name: string;
  uploadedAt?: string;
  total_pages?: number;
}

interface KnowledgeBase {
  id: string;
  name: string;
  owner: string;
  file_count: number;
  files: { file_id: string; file_name?: string }[];
}

interface RicaDocsPopoverProps {
  onClose: () => void;
}

export function RicaDocsPopover({ onClose }: RicaDocsPopoverProps) {
  const ideMessenger = useContext(IdeMessengerContext);
  const { mainEditor } = useMainEditor();
  const popoverRef = useRef<HTMLDivElement>(null);
  const pageInputRef = useRef<HTMLInputElement>(null);

  const [documents, setDocuments] = useState<RicaDocument[]>([]);
  const [knowledgeBases, setKnowledgeBases] = useState<KnowledgeBase[]>([]);
  const [loading, setLoading] = useState(false);
  const [activeTab, setActiveTab] = useState<"docs" | "kb">("docs");
  const [selectedDoc, setSelectedDoc] = useState<RicaDocument | null>(null);
  const [pageInput, setPageInput] = useState("");
  const [searchQuery, setSearchQuery] = useState("");

  const fetchDocuments = useCallback(async () => {
    setLoading(true);
    try {
      const response = await ideMessenger.request("ricaDocs/list", undefined);
      if (response.status === "success" && response.content.documents) {
        setDocuments(response.content.documents);
      }
    } catch {
      // silently fail
    } finally {
      setLoading(false);
    }
  }, [ideMessenger]);

  const fetchKnowledgeBases = useCallback(async () => {
    setLoading(true);
    try {
      const response = await ideMessenger.request("knowledgeBase/list", {
        page: 1,
        limit: 100,
      });
      if (response.status === "success" && response.content.data) {
        setKnowledgeBases(response.content.data);
      }
    } catch {
      // silently fail
    } finally {
      setLoading(false);
    }
  }, [ideMessenger]);

  useEffect(() => {
    if (activeTab === "docs") {
      fetchDocuments();
    } else {
      fetchKnowledgeBases();
    }
  }, [activeTab, fetchDocuments, fetchKnowledgeBases]);

  useEffect(() => {
    function handleClickOutside(event: MouseEvent) {
      if (
        popoverRef.current &&
        !popoverRef.current.contains(event.target as Node)
      ) {
        onClose();
      }
    }
    document.addEventListener("mousedown", handleClickOutside);
    return () => document.removeEventListener("mousedown", handleClickOutside);
  }, [onClose]);

  useEffect(() => {
    if (selectedDoc && pageInputRef.current) {
      pageInputRef.current.focus();
    }
  }, [selectedDoc]);

  const insertMention = (label: string, query: string) => {
    if (!mainEditor) return;

    const node = mainEditor.schema.nodes[Mention.name].create({
      id: "rica-docs",
      label,
      itemType: "contextProvider",
      query,
    });

    mainEditor
      .chain()
      .focus()
      .insertContent([node.toJSON(), { type: "text", text: " " }])
      .run();

    onClose();
  };

  const handleDocClick = (doc: RicaDocument) => {
    setSelectedDoc(doc);
    setPageInput("");
  };

  const handlePageConfirm = () => {
    if (!selectedDoc) return;
    const trimmed = pageInput.trim();
    const query = trimmed ? `${selectedDoc.id}:${trimmed}` : selectedDoc.id;
    const label = trimmed
      ? `${selectedDoc.name} (p.${trimmed})`
      : selectedDoc.name;
    insertMention(label, query);
  };

  const handlePageKeyDown = (e: React.KeyboardEvent) => {
    if (e.key === "Enter") {
      e.preventDefault();
      handlePageConfirm();
    } else if (e.key === "Escape") {
      setSelectedDoc(null);
    }
  };

  const handleKBClick = (kb: KnowledgeBase) => {
    const docIds = kb.files?.map((f) => f.file_id).join(",") || kb.id;
    insertMention(`📚 ${kb.name}`, docIds);
  };

  const handleRefresh = () => {
    if (activeTab === "docs") {
      void ideMessenger.request("ricaDocs/refresh", undefined).then((res) => {
        if (res.status === "success" && res.content.documents) {
          setDocuments(res.content.documents);
        }
      });
    } else {
      void ideMessenger
        .request("knowledgeBase/refresh", { page: 1, limit: 100 })
        .then((res) => {
          if (res.status === "success" && res.content.data) {
            setKnowledgeBases(res.content.data);
          }
        });
    }
  };

  return (
    <div
      ref={popoverRef}
      className="absolute bottom-full left-0 z-50 mb-1 w-72 overflow-hidden rounded-md border border-solid border-zinc-700 bg-[var(--vscode-input-background,#1e1e1e)] shadow-lg"
    >
      <div className="flex items-center justify-between border-b border-solid border-zinc-700 px-3 py-2">
        {selectedDoc ? (
          <>
            <button
              onClick={() => setSelectedDoc(null)}
              className="mr-1 rounded p-0.5 text-gray-400 hover:bg-white/10 hover:text-white"
            >
              <ArrowLeftIcon className="h-3.5 w-3.5" />
            </button>
            <span className="flex-1 truncate text-xs font-semibold">
              {selectedDoc.name}
            </span>
          </>
        ) : (
          <span className="text-xs font-semibold">RICA Documents</span>
        )}
        <div className="flex items-center gap-1">
          {!selectedDoc && (
            <button
              onClick={handleRefresh}
              className="rounded p-0.5 text-gray-400 hover:bg-white/10 hover:text-white"
              title="Refresh"
            >
              <ArrowPathIcon className="h-3.5 w-3.5" />
            </button>
          )}
          <button
            onClick={onClose}
            className="rounded p-0.5 text-gray-400 hover:bg-white/10 hover:text-white"
          >
            <XMarkIcon className="h-3.5 w-3.5" />
          </button>
        </div>
      </div>

      {selectedDoc ? (
        <div className="p-3">
          <div className="mb-2 text-[11px] text-gray-400">
            {selectedDoc.total_pages
              ? `${selectedDoc.total_pages} pages available`
              : "Pages"}
          </div>
          <input
            ref={pageInputRef}
            type="text"
            value={pageInput}
            onChange={(e) => setPageInput(e.target.value)}
            onKeyDown={handlePageKeyDown}
            placeholder="e.g. 1-5, 8, 12-15 (leave empty to use AI search)"
            className="mb-2 w-full rounded border border-solid border-zinc-600 bg-transparent px-2 py-1.5 text-[11px] text-white placeholder-gray-500 outline-none focus:border-blue-500"
          />
          <div className="flex gap-2">
            <button
              onClick={handlePageConfirm}
              className="flex-1 rounded bg-blue-600 px-2 py-1 text-[11px] font-medium text-white hover:bg-blue-500"
            >
              {pageInput.trim() ? "Insert pages" : "Insert all"}
            </button>
          </div>
        </div>
      ) : (
        <>
          <div className="flex border-b border-solid border-zinc-700">
            <button
              onClick={() => setActiveTab("docs")}
              className={`flex-1 border-b-2 px-3 py-1.5 text-[11px] font-medium transition-colors ${
                activeTab === "docs"
                  ? "border-blue-500 text-blue-400"
                  : "border-transparent text-gray-400 hover:text-gray-200"
              }`}
            >
              Documents
            </button>
            <button
              onClick={() => setActiveTab("kb")}
              className={`flex-1 border-b-2 px-3 py-1.5 text-[11px] font-medium transition-colors ${
                activeTab === "kb"
                  ? "border-blue-500 text-blue-400"
                  : "border-transparent text-gray-400 hover:text-gray-200"
              }`}
            >
              Knowledge Bases
            </button>
          </div>

          <div className="relative mx-1.5 my-1.5">
            <MagnifyingGlassIcon className="absolute left-2 top-1/2 h-3 w-3 -translate-y-1/2 text-gray-500" />
            <input
              type="text"
              placeholder="Search..."
              value={searchQuery}
              onChange={(e) => setSearchQuery(e.target.value)}
              onClick={(e) => e.stopPropagation()}
              className="box-border w-full rounded border border-solid border-zinc-700 bg-transparent py-1 pl-6 pr-2 text-[11px] text-white placeholder-gray-500 outline-none focus:border-blue-500"
            />
          </div>

          <div className="max-h-48 overflow-y-auto px-1.5 pb-1.5">
            {loading ? (
              <p className="py-4 text-center text-[11px] text-gray-500">
                Loading...
              </p>
            ) : activeTab === "docs" ? (
              (() => {
                const filtered = documents.filter((doc) =>
                  doc.name.toLowerCase().includes(searchQuery.toLowerCase()),
                );
                return filtered.length === 0 ? (
                  <p className="py-4 text-center text-[11px] text-gray-500">
                    {searchQuery ? "No matches" : "No documents found"}
                  </p>
                ) : (
                  filtered.map((doc) => (
                    <button
                      key={doc.id}
                      onClick={() => handleDocClick(doc)}
                      className="flex w-full items-center gap-2 rounded px-2 py-1.5 text-left text-[11px] hover:bg-white/10"
                    >
                      <DocumentTextIcon className="h-3.5 w-3.5 flex-shrink-0 text-blue-400" />
                      <span className="flex-1 truncate">{doc.name}</span>
                      {doc.total_pages && (
                        <span className="text-[9px] text-gray-500">
                          {doc.total_pages}p
                        </span>
                      )}
                    </button>
                  ))
                );
              })()
            ) : (
              (() => {
                const filtered = knowledgeBases.filter((kb) =>
                  kb.name.toLowerCase().includes(searchQuery.toLowerCase()),
                );
                return filtered.length === 0 ? (
                  <p className="py-4 text-center text-[11px] text-gray-500">
                    {searchQuery ? "No matches" : "No knowledge bases found"}
                  </p>
                ) : (
                  filtered.map((kb) => (
                    <button
                      key={kb.id}
                      onClick={() => handleKBClick(kb)}
                      className="flex w-full items-center gap-2 rounded px-2 py-1.5 text-left text-[11px] hover:bg-white/10"
                    >
                      <FolderIcon className="h-3.5 w-3.5 flex-shrink-0 text-blue-400" />
                      <div className="min-w-0 flex-1">
                        <div className="truncate">{kb.name}</div>
                        <div className="text-[9px] text-gray-500">
                          {kb.files?.length || kb.file_count || 0} files •{" "}
                          {kb.owner || ""}
                        </div>
                      </div>
                    </button>
                  ))
                );
              })()
            )}
          </div>
        </>
      )}
    </div>
  );
}
