import {
  ArrowPathIcon,
  DocumentTextIcon,
  MagnifyingGlassIcon,
} from "@heroicons/react/24/outline";
import { useCallback, useContext, useEffect, useState } from "react";
import { IdeMessengerContext } from "../../../context/IdeMessenger";
import { Card, EmptyState } from "../../../components/ui";
import { fontSize } from "../../../util";
import KnowledgeBaseView from "./KnowledgeBaseView";

interface RicaDocument {
  id: string;
  name: string;
  path: string;
  uploadedAt?: string;
  status?: string;
  total_pages?: number;
}

type TabType = "docs" | "kb";

export default function DocsPage() {
  const ideMessenger = useContext(IdeMessengerContext);
  const [documents, setDocuments] = useState<RicaDocument[]>([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [activeTab, setActiveTab] = useState<TabType>("docs");
  const [searchQuery, setSearchQuery] = useState("");

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

  useEffect(() => {
    fetchDocuments();
  }, [fetchDocuments]);

  const filteredDocuments = documents.filter((doc) =>
    doc.name.toLowerCase().includes(searchQuery.toLowerCase()),
  );

  return (
    <div className="h-full overflow-y-auto p-4">
      <div className="mb-6 flex items-center justify-between">
        <h2 className="my-0 text-xl font-semibold">RICA Documents</h2>
        <button
          onClick={() => fetchDocuments(true)}
          disabled={loading}
          className="text-description hover:text-foreground flex items-center gap-1.5 rounded-md px-2.5 py-1.5 text-xs transition-colors disabled:opacity-50"
        >
          <ArrowPathIcon
            className={`h-3.5 w-3.5 ${loading ? "animate-spin" : ""}`}
          />
          Refresh
        </button>
      </div>

      <div className="mb-4 flex gap-1">
        <button
          onClick={() => setActiveTab("docs")}
          className={`rounded-md px-3 py-1.5 text-xs font-medium transition-colors ${
            activeTab === "docs"
              ? "bg-editor text-foreground"
              : "text-description hover:text-foreground"
          }`}
        >
          Documents
        </button>
        <button
          onClick={() => setActiveTab("kb")}
          className={`rounded-md px-3 py-1.5 text-xs font-medium transition-colors ${
            activeTab === "kb"
              ? "bg-editor text-foreground"
              : "text-description hover:text-foreground"
          }`}
        >
          Knowledge Bases
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
          onChange={(e) => setSearchQuery(e.target.value)}
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
              {filteredDocuments.map((doc) => (
                <div
                  key={doc.id}
                  className="hover:bg-list-hover flex items-center gap-3 rounded-md px-2 py-2"
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

      {activeTab === "kb" && <KnowledgeBaseView searchQuery={searchQuery} />}
    </div>
  );
}
