import { FolderIcon } from "@heroicons/react/24/outline";
import { useCallback, useContext, useEffect, useState } from "react";
import { IdeMessengerContext } from "../../../context/IdeMessenger";
import { Card, EmptyState } from "../../../components/ui";
import { fontSize } from "../../../util";

interface KnowledgeBase {
  id: string;
  name: string;
  owner: string;
  file_count: number;
  member_count: number;
  created_at: string;
}

interface KnowledgeBaseViewProps {
  searchQuery?: string;
}

export default function KnowledgeBaseView({
  searchQuery = "",
}: KnowledgeBaseViewProps) {
  const ideMessenger = useContext(IdeMessengerContext);
  const [knowledgeBases, setKnowledgeBases] = useState<KnowledgeBase[]>([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const fetchKnowledgeBases = useCallback(
    async (refresh = false) => {
      setLoading(true);
      setError(null);
      try {
        const method = refresh ? "knowledgeBase/refresh" : "knowledgeBase/list";
        const response = await ideMessenger.request(method, {
          page: 1,
          limit: 100,
        });
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
    fetchKnowledgeBases();
  }, [fetchKnowledgeBases]);

  if (error) {
    return (
      <Card className="border border-solid border-red-800/50 bg-red-900/10">
        <p className="text-xs text-red-400">{error}</p>
      </Card>
    );
  }

  if (loading && knowledgeBases.length === 0) {
    return (
      <Card>
        <EmptyState message="Loading knowledge bases..." />
      </Card>
    );
  }

  const filteredKBs = knowledgeBases.filter((kb) =>
    kb.name.toLowerCase().includes(searchQuery.toLowerCase()),
  );

  if (knowledgeBases.length === 0) {
    return (
      <Card>
        <EmptyState message="No knowledge bases found. Create one through the RICA portal." />
      </Card>
    );
  }

  return (
    <Card>
      {filteredKBs.length === 0 ? (
        <EmptyState
          message={
            searchQuery
              ? "No knowledge bases match your search."
              : "No knowledge bases found. Create one through the RICA portal."
          }
        />
      ) : (
        <div className="flex flex-col gap-2">
          {filteredKBs.map((kb) => (
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
                <span>{kb.file_count} files</span>
                <span>{kb.member_count} members</span>
                <span>Owner: {kb.owner}</span>
              </div>
            </div>
          ))}
        </div>
      )}
    </Card>
  );
}
