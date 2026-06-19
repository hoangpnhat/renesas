"""
Elasticsearch Vector Service
Index and search vector embeddings in Elasticsearch
"""

import logging
from dataclasses import dataclass
from typing import List, Dict, Any, Optional

from elasticsearch import Elasticsearch
from elasticsearch.helpers import bulk

logger = logging.getLogger(__name__)


@dataclass
class IndexResult:
    """Result of indexing operation"""
    success: bool
    indexed_count: int = 0
    failed_count: int = 0
    error: Optional[str] = None


@dataclass
class SearchResult:
    """Result of vector search"""
    success: bool
    results: List[Dict[str, Any]] | None = None
    error: Optional[str] = None


class ElasticsearchVectorService:
    """
    Service to index and search vector embeddings in Elasticsearch
    """

    def __init__(self):
        from core.config import settings

        self.host = getattr(
            settings,
            'ELASTICSEARCH_HOST',
            'https://es-ai-nonprod-colossus-001-bbba6d.es.eastus.azure.elastic.cloud:443'
        )
        self.api_key = getattr(
            settings,
            'ELASTICSEARCH_API_KEY',
            'bXJWb2ZKb0JKc09MUHJYdEViSk46cldQQ1FHZEpWcHJJSXNGeTdBMG92UQ=='
        )
        self.index_name = getattr(
            settings,
            'INDEX_NAME',
            'multimodal-model'
        ) or "multimodal-model"
        self.bulk_chunk_size = 50  # Index 50 docs at a time

        # Initialize Elasticsearch client with longer timeout
        self.es = Elasticsearch(
            self.host,
            api_key=self.api_key,
            request_timeout=120,
            max_retries=3,
            retry_on_timeout=True
        )

        logger.info(f"ElasticsearchVectorService initialized")
        logger.info(f"   Host: {self.host}")
        logger.info(f"   Index: {self.index_name}")

        # Ensure index exists
        self._ensure_index_exists()

    _INDEX_MAPPINGS = {
        "mappings": {
            "properties": {
                "multimodal_vector": {
                    "type": "rank_vectors"
                },
                "metadata": {
                    "properties": {
                        "file_id": {"type": "keyword"},
                        "file_name": {"type": "text"},
                        "page_num": {"type": "integer"},
                        "upload_date": {"type": "date"},
                        "author_id": {"type": "keyword"},
                        "volume_path": {"type": "keyword"}
                    }
                }
            }
        }
    }

    def _create_index(self) -> None:
        """Create the index with the canonical rank_vectors mapping."""
        self.es.indices.create(index=self.index_name, body=self._INDEX_MAPPINGS)
        logger.info(f"Index created: {self.index_name}")

    def _ensure_index_exists(self) -> None:
        """Create index if absent; raise on incompatible vector mapping.

        Elasticsearch does not allow changing a field's type after creation.
        If ``multimodal_vector`` is mapped as anything other than ``rank_vectors``
        a ``ValueError`` is raised so the service fails fast and a human operator
        can decide how to handle the existing index before restarting.
        """
        try:
            if not self.es.indices.exists(index=self.index_name):
                logger.info(f"Index not found, creating: {self.index_name}")
                self._create_index()
                return

            # Index exists — verify the vector field mapping
            mapping = self.es.indices.get_mapping(index=self.index_name)
            props = (
                mapping[self.index_name]
                .get("mappings", {})
                .get("properties", {})
            )
            vector_type = props.get("multimodal_vector", {}).get("type")

            if vector_type == "rank_vectors":
                logger.info(f"Index exists with correct mapping: {self.index_name}")
                return

            # Wrong mapping — refuse to start, let an operator decide
            raise ValueError(
                f"Index '{self.index_name}' has multimodal_vector mapped as "
                f"'{vector_type}' but 'rank_vectors' is required. "
                f"Elasticsearch does not allow field type changes after index creation. "
                f"Please manually delete (or rename) the index and restart the service "
                f"so it can be recreated with the correct mapping."
            )

        except Exception as e:
            logger.exception(f"Failed to ensure index exists: {e}")
            raise

    def _create_document(
            self,
            doc_id: str,
            vectors: List[List[float]],
            metadata: Dict[str, Any]
    ) -> Dict[str, Any]:
        """
        Create document for indexing

        Args:
            doc_id: Document ID
            vectors: Embedding vectors
            metadata: Document metadata

        Returns:
            Document dict
        """
        return {
            "_index": self.index_name,
            "_id": doc_id,
            "_source": {
                "multimodal_vector": vectors,
                "metadata": metadata
            }
        }

    async def index_documents_bulk(
            self,
            documents: List[Dict[str, Any]]
    ) -> IndexResult:
        """
        Index multiple documents using bulk API with chunking

        Splits large batches into smaller chunks to avoid timeout

        Args:
            documents: List of dicts with 'doc_id', 'vectors', 'metadata'

        Returns:
            IndexResult with stats
        """
        try:
            total_docs = len(documents)
            logger.info(f"Bulk indexing {total_docs} documents (chunk size: {self.bulk_chunk_size})")

            total_success = 0
            total_failed = 0

            # Process in chunks to avoid timeout
            for chunk_start in range(0, total_docs, self.bulk_chunk_size):
                chunk_end = min(chunk_start + self.bulk_chunk_size, total_docs)
                chunk = documents[chunk_start:chunk_end]

                chunk_num = (chunk_start // self.bulk_chunk_size) + 1
                total_chunks = (total_docs + self.bulk_chunk_size - 1) // self.bulk_chunk_size

                logger.info(
                    f"Processing chunk {chunk_num}/{total_chunks}: docs {chunk_start + 1}-{chunk_end}/{total_docs}")

                # Prepare bulk actions for this chunk
                actions: list[dict[str, Any]] = []
                for doc in chunk:
                    # Log vector structure for debugging (first doc only)
                    if chunk_start == 0 and len(actions) == 0:
                        vectors = doc['vectors']
                        logger.info(f"Vector structure debug - type: {type(vectors)}, "
                                  f"len: {len(vectors) if vectors else 0}, "
                                  f"first_vector_len: {len(vectors[0]) if vectors and len(vectors) > 0 else 0}")

                    action = self._create_document(
                        doc_id=doc['doc_id'],
                        vectors=doc['vectors'],
                        metadata=doc['metadata']
                    )
                    actions.append(action)

                # Execute bulk indexing for this chunk
                try:
                    success, failed = bulk(
                        self.es,
                        actions,
                        raise_on_error=False,
                        raise_on_exception=False,
                        chunk_size=self.bulk_chunk_size,
                    )

                    total_success += success
                    failed_count = len(failed) if isinstance(failed, list) else (failed or 0)
                    total_failed += failed_count

                    logger.info(
                        f"Chunk {chunk_num} complete: {success} success, {failed_count} failed")

                    # Log detailed error information for failed documents
                    if isinstance(failed, list) and failed:
                        for fail_info in failed[:5]:  # Log first 5 failures
                            logger.error(f"Index failed - doc: {fail_info.get('index', {}).get('_id', 'unknown')}, "
                                       f"error: {fail_info.get('index', {}).get('error', 'unknown')}")

                except Exception as e:
                    logger.error(f"Chunk {chunk_num} failed: {e}")
                    total_failed += len(chunk)
                    continue

            logger.info(f"Bulk indexing complete: {total_success} success, {total_failed} failed")

            return IndexResult(
                success=True,
                indexed_count=total_success,
                failed_count=total_failed
            )

        except Exception as e:
            logger.exception(f"Bulk indexing failed: {e}")
            return IndexResult(
                success=False,
                error=str(e)
            )

    async def search_by_text(
            self,
            query_vector: List[float],
            top_k: int = 10,
            filters: Optional[Dict[str, Any]] = None
    ) -> SearchResult:
        """
        Search documents by text query vector

        Args:
            query_vector: Query embedding vector
            top_k: Number of results to return
            filters: Optional metadata filters

        Returns:
            SearchResult with matching documents
        """
        try:
            # Build query
            query = {
                "rank": {
                    "field": "multimodal_vector",
                    "query_vector": query_vector
                }
            }

            # Add filters if provided
            if filters:
                query = {
                    "bool": {
                        "must": [query],
                        "filter": [
                            {"term": {f"metadata.{k}": v}}
                            for k, v in filters.items()
                        ]
                    }
                }

            # Execute search
            response = self.es.search(
                index=self.index_name,
                query=query,
                size=top_k
            )

            # Extract results
            hits = response['hits']['hits']
            results = []

            for hit in hits:
                results.append({
                    'doc_id': hit['_id'],
                    'score': hit['_score'],
                    'metadata': hit['_source']['metadata']
                })

            logger.info(f"Search complete: {len(results)} results found")

            return SearchResult(
                success=True,
                results=results
            )

        except Exception as e:
            logger.exception(f"Search failed: {e}")
            return SearchResult(
                success=False,
                error=str(e)
            )


# Singleton

_es_service = None


def get_elasticsearch_service() -> ElasticsearchVectorService:
    """Get Elasticsearch service singleton"""
    global _es_service
    if _es_service is None:
        _es_service = ElasticsearchVectorService()
    return _es_service
