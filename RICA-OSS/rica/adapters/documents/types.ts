export interface RicaDocument {
  id: string;
  name: string;
  path: string;
  uploadedAt?: string;
  status?: string;
  total_pages?: number;
  metadata?: Record<string, any>;
}

export interface DocumentListResponse {
  documents: RicaDocument[];
  total?: number;
  error?: string;
}

export interface ElasticsearchQueryRequest {
  docIds: string[];
  query: string;
  topK?: number;
}

export interface ElasticsearchResult {
  id: string;
  content: string;
  imageIds?: string[];
  imagePaths?: string[];
  metadata?: Record<string, any>;
  score?: number;
}

export interface ElasticsearchQueryResponse {
  results: ElasticsearchResult[];
  error?: string;
}

export interface KnowledgeBaseFile {
  file_id: string;
  added_at: string;
  file_name?: string;
}

export interface KnowledgeBase {
  id: string;
  name: string;
  created_at: string;
  owner: string;
  modified: string;
  member_count: number;
  file_count: number;
  files: KnowledgeBaseFile[];
  is_owner: boolean;
}

export interface PaginationInfo {
  page: number;
  limit: number;
  total: number;
}

export interface KnowledgeBaseListResponse {
  data: KnowledgeBase[];
  pagination: PaginationInfo;
  error?: string;
}
