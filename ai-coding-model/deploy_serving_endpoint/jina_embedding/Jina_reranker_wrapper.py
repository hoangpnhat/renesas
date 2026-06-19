import os
import json
import time
import uuid
import pandas as pd
import numpy as np
from typing import List, Dict, Any, Optional, Union
import logging
import mlflow.pyfunc
from mlflow.models import set_model
import torch
from transformers import AutoModel
from PIL import Image
import base64
import io
import requests

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Model configuration
MODEL_NAME = "jinaai/jina-reranker-m0"

class JinaRerankerWrapper(mlflow.pyfunc.PythonModel):
    """
    Transformers-based PyFunc wrapper for Jina Reranker m0
    Multilingual multimodal document reranker for ranking visual and textual documents
    Supports text-to-text, text-to-image, image-to-text, and image-to-image reranking
    """
    
    def __init__(self):
        self.model = None
        self.max_length = 10240  # Max context length for reranker
        self.device = "cuda" if torch.cuda.is_available() else "cpu"
    
    def load_context(self, context):
        """Load Jina reranker model with Transformers"""
        logger.info(f"Loading Jina reranker model with Transformers: {MODEL_NAME}")

        try:
            # Detect GPU type to choose appropriate attention implementation
            # T4 GPUs don't support FlashAttention-2 (requires Ampere or newer)
            # Also Qwen2VL models have config issues with certain attn implementations
            attn_impl = "eager"  # Safe default for all GPUs including T4
            use_flash_attn = False

            if torch.cuda.is_available():
                gpu_name = torch.cuda.get_device_name(0).lower()
                logger.info(f"Detected GPU: {gpu_name}")

                # A100, H100, etc. support FlashAttention-2
                if any(x in gpu_name for x in ['a100', 'h100', 'a10', 'l40']):
                    try:
                        import flash_attn
                        use_flash_attn = True
                        attn_impl = "flash_attention_2"
                        logger.info(f"FlashAttention-2 available on {gpu_name}")
                    except ImportError:
                        logger.warning("FlashAttention not installed, using eager attention")
                        attn_impl = "eager"
                else:
                    # T4 and older GPUs - must use eager attention
                    # SDPA can also have issues with Qwen2VL on T4
                    attn_impl = "eager"
                    logger.info(f"Using eager attention for GPU: {gpu_name} (FlashAttention requires Ampere+)")

            # Load model with the pre-determined attention implementation
            # This avoids the fallback issue where Qwen2VLConfig.hidden_size is missing
            logger.info(f"Loading model with attn_implementation={attn_impl}")
            self.model = AutoModel.from_pretrained(
                MODEL_NAME,
                trust_remote_code=True,
                torch_dtype=torch.float16 if self.device == "cuda" else torch.float32,
                attn_implementation=attn_impl,
                low_cpu_mem_usage=True
            )

            if use_flash_attn:
                logger.info("Using FlashAttention-2 for acceleration")
            else:
                logger.info("Using standard eager attention")

            # Move model to GPU if available
            self.model.to(self.device)
            self.model.eval()

            logger.info(f"Jina reranker model loaded successfully on {self.device}")

            # Warmup
            self._warmup()

        except Exception as e:
            logger.error(f"Failed to load model: {str(e)}")
            import traceback
            logger.error(f"Traceback:\n{traceback.format_exc()}")
            raise
    
    def _warmup(self):
        """Warm up the model with dummy inputs"""
        logger.info("Warming up model...")
        try:
            dummy_query = "test query"
            dummy_documents = ["test document 1", "test document 2"]
            _ = self._compute_scores(
                query=dummy_query,
                documents=dummy_documents,
                max_length=1024,
                doc_type="text",
                query_type="text"
            )
            logger.info("Model warmed up successfully")
        except Exception as e:
            logger.warning(f"Warmup failed: {e}")
    
    
    def _is_image_input(self, value: str) -> bool:
        """
        Detect if string value is an image (base64, URL, or file path)
        
        Args:
            value: String to check
            
        Returns:
            True if value looks like image data, False if it's text
        """
        if not isinstance(value, str):
            return False
        
        # Check for image data URI
        if value.startswith('data:image'):
            return True
        
        # Check for URL
        if value.startswith('http://') or value.startswith('https://'):
            # Additional check: does URL end with image extension?
            lower_val = value.lower()
            if any(ext in lower_val for ext in ['.png', '.jpg', '.jpeg', '.gif', '.bmp', '.tiff', '.webp']):
                return True
        
        # Check for base64 (very long string without spaces, likely base64)
        if len(value) > 200 and ' ' not in value:
            # Additional check: base64 strings typically don't have common text patterns
            if not any(word in value.lower() for word in ['the', 'is', 'and', 'or', 'but', 'in', 'on', 'at']):
                return True
        
        # Check for file path with image extension (only for shorter strings)
        if len(value) < 260:  # Windows MAX_PATH limit
            if value.lower().endswith(('.png', '.jpg', '.jpeg', '.gif', '.bmp', '.tiff', '.webp')):
                if os.path.exists(value):
                    return True
        
        # Otherwise, treat as text
        return False
    
    def _compute_scores(
        self,
        query: Union[str, List[str]],
        documents: List[str],
        max_length: int = 1024,
        doc_type: str = "text",
        query_type: str = "text"
    ) -> List[float]:
        """
        Compute relevance scores for query-document pairs using Jina reranker
        
        Args:
            query: Query text or image (URL, base64, or file path)
            documents: List of documents (text or images)
            max_length: Maximum sequence length (default 1024 for text, 2048 for images)
            doc_type: Type of documents - "text" or "image"
            query_type: Type of query - "text" or "image"
        
        Returns:
            List of relevance scores for each document
        """
        try:
            # Ensure query is a string (not list)
            if isinstance(query, list):
                query = query[0] if query else ""
            
            # Construct sentence pairs: [[query, doc1], [query, doc2], ...]
            pairs = [[query, doc] for doc in documents]
            
            # Compute scores using model's compute_score method
            scores = self.model.compute_score(
                pairs, 
                max_length=max_length,
                doc_type=doc_type,
                query_type=query_type
            )
            
            # Ensure scores is a list
            if not isinstance(scores, list):
                scores = scores.tolist() if hasattr(scores, 'tolist') else [scores]
            
            return scores
            
        except Exception as e:
            logger.error(f"Error computing scores: {e}")
            raise
    
    def predict(self, context, model_input):
        """
        Rerank documents based on query relevance
        
        Expected input formats:
        1. DataFrame with columns:
           - 'query': single query text or image (URL, base64, or path)
           - 'documents': list of documents (text or images)
           - 'max_length' (optional): max sequence length (default 1024 for text, 2048 for images)
           - 'doc_type' (optional): "text" or "image" (default "text")
           - 'query_type' (optional): "text" or "image" (default "text")
           - 'return_documents' (optional): whether to return document content (default False)
           
        2. Dictionary with:
           - 'query': query text or image
           - 'documents': list of documents
           - 'max_length' (optional)
           - 'doc_type' (optional)
           - 'query_type' (optional)
           - 'return_documents' (optional)
           
        Supported use cases:
           - Text-to-Text: Query text, rank text documents
           - Text-to-Image: Query text, rank image documents
           - Image-to-Text: Query image, rank text documents
           - Image-to-Image: Query image, rank image documents
           
        Returns:
           Dictionary with reranking results (similar to Jina API format)
        """
        start_time = time.time()
        
        try:
            query = None
            documents = None
            max_length = 1024  # Default for text
            doc_type = "text"  # Default
            query_type = "text"  # Default
            return_documents = False  # Default
            
            # Parse input
            if isinstance(model_input, pd.DataFrame):
                # Required fields
                if 'query' not in model_input.columns:
                    raise ValueError("Input must have 'query' column")
                if 'documents' not in model_input.columns:
                    raise ValueError("Input must have 'documents' column")
                
                query = model_input.iloc[0]['query']
                documents = model_input.iloc[0]['documents']
                
                # Ensure documents is a list
                if not isinstance(documents, list):
                    documents = [documents]
                
                # Optional parameters
                max_length = model_input.iloc[0].get('max_length', max_length) if 'max_length' in model_input.columns else max_length
                doc_type = model_input.iloc[0].get('doc_type', doc_type) if 'doc_type' in model_input.columns else doc_type
                query_type = model_input.iloc[0].get('query_type', query_type) if 'query_type' in model_input.columns else query_type
                return_documents = model_input.iloc[0].get('return_documents', return_documents) if 'return_documents' in model_input.columns else return_documents
                
            elif isinstance(model_input, dict):
                # Required fields
                if 'query' not in model_input:
                    raise ValueError("Input must have 'query' key")
                if 'documents' not in model_input:
                    raise ValueError("Input must have 'documents' key")
                
                query = model_input['query']
                documents = model_input['documents']
                
                # Ensure documents is a list
                if not isinstance(documents, list):
                    documents = [documents]
                
                # Optional parameters
                max_length = model_input.get('max_length', max_length)
                doc_type = model_input.get('doc_type', doc_type)
                query_type = model_input.get('query_type', query_type)
                return_documents = model_input.get('return_documents', return_documents)
            else:
                raise ValueError(f"Unsupported model_input type: {type(model_input)}")
            
            # Auto-detect query type if not specified
            if query_type == "text" and self._is_image_input(query):
                query_type = "image"
                logger.info("Auto-detected query type as 'image'")
            
            # Auto-detect doc type if not specified
            if doc_type == "text" and documents and self._is_image_input(documents[0]):
                doc_type = "image"
                max_length = 2048  # Use larger max_length for images
                logger.info("Auto-detected doc type as 'image'")
            
            # Adjust max_length for image documents if not explicitly set
            if doc_type == "image" and max_length == 1024:
                max_length = 2048
            
            # Compute relevance scores
            scores = self._compute_scores(
                query=query,
                documents=documents,
                max_length=max_length,
                doc_type=doc_type,
                query_type=query_type
            )

            generation_time = time.time() - start_time

            # Build Cohere-style results (KEEP ORIGINAL INDEX)
            results = [
                {
                    "index": idx,
                    "relevance_score": float(score)
                }
                for idx, score in enumerate(scores)
            ]

            # Sort by relevance_score DESC (but index stays original)
            results.sort(key=lambda x: x["relevance_score"], reverse=True)

            # Optional: attach document only if requested
            if return_documents:
                for item in results:
                    item["document"] = {
                        doc_type: documents[item["index"]]
                    }

            return {
                "id": f"rerank-{uuid.uuid4().hex[:16]}",
                "results": results,
                "meta": {
                    "model": MODEL_NAME,
                    "backend": "transformers",
                    "query_type": query_type,
                    "doc_type": doc_type,
                    "num_documents": len(documents),
                    "latency_ms": int(generation_time * 1000),
                    "max_length": max_length
                }
            }

            
        except Exception as e:
            logger.error(f"Error in predict: {str(e)}")
            raise

# Set the model for MLflow
set_model(JinaRerankerWrapper())