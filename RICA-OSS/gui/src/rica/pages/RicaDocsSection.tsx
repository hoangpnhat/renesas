import {
  ArrowPathIcon,
  ChevronLeftIcon,
  ChevronRightIcon,
  DocumentTextIcon,
  FolderIcon,
  MagnifyingGlassIcon,
} from "@heroicons/react/24/outline";
import { useCallback, useContext, useEffect, useState } from "react";
import { IdeMessengerContext } from "../../context/IdeMessenger";
import { Card, EmptyState } from "../../components/ui";
import { ConfigHeader } from "../../pages/config/components/ConfigHeader";
import { fontSize } from "../../util";

interface RicaDocument {
  id: string;
  name: string;
  path: string;
  uploadedAt?: string;
  status?: string;
  total_pages?: number;
}

interface KnowledgeBase {
  id: string;
  name: string;
  owner: string;
  file_count: number;
  member_count: number;
  files: { file_id: string; file_name?: string }[];
}

type TabType = "docs" | "kb";

export function RicaDocsSection() {
  const ideMessenger = useContext(IdeMessengerContext);
  const [documents, setDocuments] = useState<RicaDocument[]>([]);
  const [knowledgeBases, setKnowledgeBases] = useState<KnowledgeBase[]>([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [activeTab, setActiveTab] = useState<TabType>("docs");
  const [searchQuery, setSearchQuery] = useState("");
  const [currentPage, setCurrentPage] = useState(1);
  const pageSize = 20;

  const fetchDocuments = useCallback(
    async (refresh = false) => {
      setLoading(true);
      setError(null);
      try {
        const method = refresh ? "ricaDocs/refresh" : "ricaDocs/list";
        const response = await ideMessenger.request(method, undefined);
        if (response.status === "success") {
          const data = response.content;
          if (data.error) {
            setError(data.error);
          } else {
            setDocuments(data.documents || []);
          }
        } else {
          setError("Failed to communicate with extension");
        }
      } catch (err) {
        setError(`Failed to load documents: ${err}`);
      } finally {
        setLoading(false);
      }
    },
    [ideMessenger],
  );

  const fetchKnowledgeBases = useCallback(
    async (refresh = false, page = 1, limit = 100) => {
      setLoading(true);
      setError(null);
      try {
        const method = refresh ? "knowledgeBase/refresh" : "knowledgeBase/list";
        const response = await ideMessenger.request(method, { page, limit });
        if (response.status === "success") {
          const data = response.content;
          if (data.error) {
            setError(data.error);
          } else {
            setKnowledgeBases(data.data || []);
          }
        } else {
          setError("Failed to communicate with extension");
        }
      } catch (err) {
        setError(`Failed to load knowledge bases: ${err}`);
      } finally {
        setLoading(false);
      }
    },
    [ideMessenger],
  );

  useEffect(() => {
    setSearchQuery("");
    setCurrentPage(1);
    if (activeTab === "docs") {
      fetchDocuments();
    } else {
      fetchKnowledgeBases();
    }
  }, [activeTab, fetchDocuments, fetchKnowledgeBases]);

  const handleRefresh = () => {
    if (activeTab === "docs") {
      fetchDocuments(true);
    } else {
      fetchKnowledgeBases(true);
    }
  };

  const handleAddDocs = () => {
    void ideMessenger.post(
      "openUrl",
      "https://rica.global.renesas.com/management/file",
    );
  };

  const handleAddKB = () => {
    void ideMessenger.post(
      "openUrl",
      "https://rica.global.renesas.com/management/knowledgeBase",
    );
  };

  const filteredDocuments = documents.filter((doc) =>
    doc.name.toLowerCase().includes(searchQuery.toLowerCase()),
  );

  const filteredKBs = knowledgeBases.filter((kb) =>
    kb.name.toLowerCase().includes(searchQuery.toLowerCase()),
  );

  const totalDocsPages = Math.max(
    1,
    Math.ceil(filteredDocuments.length / pageSize),
  );
  const totalKBPages = Math.max(1, Math.ceil(filteredKBs.length / pageSize));
  const totalPages = activeTab === "docs" ? totalDocsPages : totalKBPages;

  const paginatedDocs = filteredDocuments.slice(
    (currentPage - 1) * pageSize,
    currentPage * pageSize,
  );

  const paginatedKBs = filteredKBs.slice(
    (currentPage - 1) * pageSize,
    currentPage * pageSize,
  );

  return (
    <div>
      <ConfigHeader
        title="RICA Documents"
        subtext="Use @rica-docs in chat to mention documents as context."
        onAddClick={activeTab === "docs" ? handleAddDocs : handleAddKB}
        addButtonTooltip={
          activeTab === "docs"
            ? "Add documents via RICA portal"
            : "Add knowledge base via RICA portal"
        }
      />

      <div className="mb-4 flex items-center justify-between">
        <div className="flex gap-1">
          <button
            onClick={() => setActiveTab("docs")}
            className={`rounded-md px-3 py-1.5 text-xs font-medium transition-colors ${
              activeTab === "docs"
                ? "bg-editor text-foreground"
                : "text-description hover:text-foreground"
            }`}
          >
            Documents ({documents.length})
          </button>
          <button
            onClick={() => setActiveTab("kb")}
            className={`rounded-md px-3 py-1.5 text-xs font-medium transition-colors ${
              activeTab === "kb"
                ? "bg-editor text-foreground"
                : "text-description hover:text-foreground"
            }`}
          >
            Knowledge Bases ({knowledgeBases.length})
          </button>
        </div>
        <button
          onClick={handleRefresh}
          disabled={loading}
          className="text-description hover:text-foreground flex items-center gap-1 rounded-md px-2 py-1 text-xs transition-colors disabled:opacity-50"
        >
          <ArrowPathIcon
            className={`h-3.5 w-3.5 ${loading ? "animate-spin" : ""}`}
          />
          Refresh
        </button>
      </div>

      <div className="relative mb-4">
        <MagnifyingGlassIcon className="text-description absolute left-3 top-1/2 h-3.5 w-3.5 -translate-y-1/2" />
        <input
          type="text"
          placeholder={
            activeTab === "docs"
              ? "Search documents..."
              : "Search knowledge bases..."
          }
          value={searchQuery}
          onChange={(e) => {
            setSearchQuery(e.target.value);
            setCurrentPage(1);
          }}
          className="bg-input text-input-foreground placeholder-input-placeholder border-input-border focus:border-border-focus box-border w-full rounded-md border border-solid py-2 pl-9 pr-3 text-xs outline-none"
        />
      </div>

      {error && (
        <Card className="mb-4 border border-solid border-red-800/50 bg-red-900/10">
          <p className="text-xs text-red-400">{error}</p>
        </Card>
      )}

      {activeTab === "docs" && (
        <Card>
          {loading && documents.length === 0 ? (
            <EmptyState message="Loading documents..." />
          ) : filteredDocuments.length === 0 ? (
            <EmptyState
              message={
                searchQuery
                  ? "No documents match your search."
                  : "No documents found. Upload documents through the RICA portal."
              }
            />
          ) : (
            <div className="flex flex-col gap-1">
              {paginatedDocs.map((doc) => (
                <div
                  key={doc.id}
                  className="hover:bg-list-hover flex items-center gap-3 rounded-md px-2 py-1.5"
                  style={{ fontSize: fontSize(-3) }}
                >
                  <DocumentTextIcon className="h-4 w-4 flex-shrink-0 text-blue-400" />
                  <div className="min-w-0 flex-1">
                    <div className="text-foreground truncate font-medium">
                      {doc.name}
                    </div>
                    <div className="text-description mt-0.5 text-[11px]">
                      {[
                        doc.total_pages ? `${doc.total_pages} pages` : null,
                        doc.uploadedAt
                          ? `Uploaded: ${new Date(doc.uploadedAt).toLocaleDateString()}`
                          : doc.status || "Available",
                      ]
                        .filter(Boolean)
                        .join(" · ")}
                    </div>
                  </div>
                </div>
              ))}
            </div>
          )}
        </Card>
      )}

      {activeTab === "kb" && (
        <Card>
          {loading && knowledgeBases.length === 0 ? (
            <EmptyState message="Loading knowledge bases..." />
          ) : filteredKBs.length === 0 ? (
            <EmptyState
              message={
                searchQuery
                  ? "No knowledge bases match your search."
                  : "No knowledge bases found. Create one through the RICA portal."
              }
            />
          ) : (
            <div className="flex flex-col gap-2">
              {paginatedKBs.map((kb) => (
                <div
                  key={kb.id}
                  className="hover:bg-list-hover rounded-md px-2 py-2"
                  style={{ fontSize: fontSize(-3) }}
                >
                  <div className="flex items-center gap-2">
                    <FolderIcon className="h-4 w-4 flex-shrink-0 text-blue-400" />
                    <span className="text-foreground truncate font-medium">
                      {kb.name}
                    </span>
                  </div>
                  <div className="text-description mt-1 flex gap-3 pl-6 text-[11px]">
                    <span>{kb.files?.length || kb.file_count || 0} files</span>
                    <span>{kb.member_count || 0} members</span>
                    <span>Owner: {kb.owner}</span>
                  </div>
                </div>
              ))}
            </div>
          )}
        </Card>
      )}

      {totalPages > 1 && (
        <div className="mt-3 flex items-center justify-between">
          <span className="text-description text-xs">
            Page {currentPage} of {totalPages}
          </span>
          <div className="flex items-center gap-1">
            <button
              onClick={() => setCurrentPage((p) => Math.max(1, p - 1))}
              disabled={currentPage <= 1}
              className="text-description hover:text-foreground rounded-md p-1 transition-colors disabled:opacity-30"
            >
              <ChevronLeftIcon className="h-3.5 w-3.5" />
            </button>
            <button
              onClick={() => setCurrentPage((p) => Math.min(totalPages, p + 1))}
              disabled={currentPage >= totalPages}
              className="text-description hover:text-foreground rounded-md p-1 transition-colors disabled:opacity-30"
            >
              <ChevronRightIcon className="h-3.5 w-3.5" />
            </button>
          </div>
        </div>
      )}
    </div>
  );
}
