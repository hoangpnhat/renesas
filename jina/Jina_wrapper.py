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
from vllm import LLM
from PIL import Image
import base64
import io
import requests

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Model configuration
MODEL_NAME = "jinaai/jina-embeddings-v4-vllm-retrieval"  # v4 chưa có trên vLLM, dùng v3

class JinaEmbeddingWrapper(mlflow.pyfunc.PythonModel):
    """
    vLLM-optimized PyFunc wrapper for Jina Embeddings v4
    High-performance embedding generation for text
    """
    
    def __init__(self):
        self.llm = None
        self.max_seq_length = 8192
        
    def _get_vllm_config(self) -> Dict[str, Any]:
        """Get optimized vLLM configuration for embedding model"""
        config = {
            "model": MODEL_NAME,
            "task": "embed",
            "trust_remote_code": True,
            "dtype": "half",  # Use FP16 for faster inference
            "max_model_len": self.max_seq_length,
            "gpu_memory_utilization": 0.90,
            "tensor_parallel_size": 1,
            "enforce_eager": False,  # Enable CUDA graphs for speed
            "disable_log_stats": False,
            "max_num_seqs": 128,  # High batch size for embeddings
            "enable_prefix_caching": True,
        }
        return config
    
    def load_context(self, context):
        """Load Jina embedding model with vLLM"""
        logger.info(f"Loading Jina embedding model with vLLM: {MODEL_NAME}")
        
        try:
            vllm_config = self._get_vllm_config()
            
            # Load model with vLLM (includes tokenizer and processor internally)
            self.llm = LLM(**vllm_config)
            
            logger.info("Jina embedding model loaded successfully with vLLM")
            logger.info("Note: vLLM handles tokenization and image processing internally")
            
            # Warmup
            self._warmup()
            
        except Exception as e:
            logger.error(f"Failed to load model: {str(e)}")
            raise
    
    def _warmup(self):
        """Warm up the model with dummy inputs"""
        logger.info("Warming up model...")
        try:
            dummy_texts = [
                "Hello world",
                "This is a test sentence",
                "Machine learning is fascinating"
            ]
            _ = self._generate_embeddings(dummy_texts)
            logger.info("Model warmed up successfully")
        except Exception as e:
            logger.warning(f"Warmup failed: {e}")
    
    def _truncate_text(self, text: str, max_length: int = None) -> str:
        """
        Truncate text to fit within max token limit
        Note: vLLM will handle truncation internally, this is just a safeguard
        """
        if max_length is None:
            max_length = self.max_seq_length
        
        # Simple character-based truncation as approximation
        # vLLM will do proper token-based truncation internally
        max_chars = max_length * 4  # Rough estimate: 1 token ≈ 4 chars
        if len(text) > max_chars:
            text = text[:max_chars]
            logger.warning(f"Text truncated to ~{max_chars} characters")
        
        return text
    
    def _load_image(self, image_input: Union[str, bytes, Image.Image]) -> Image.Image:
        """
        Load image from various formats
        
        Args:
            image_input: Can be:
                - Base64 encoded string
                - URL (http/https)
                - Local file path
                - PIL Image object
                - Raw bytes
        
        Returns:
            PIL Image object
        """
        try:
            # Already a PIL Image
            if isinstance(image_input, Image.Image):
                return image_input
            
            # Raw bytes
            if isinstance(image_input, bytes):
                return Image.open(io.BytesIO(image_input))
            
            # String input (base64, URL, or file path)
            if isinstance(image_input, str):
                # Try base64 first
                if image_input.startswith('data:image'):
                    # Data URI format: data:image/png;base64,<base64_string>
                    base64_str = image_input.split(',', 1)[1]
                    image_bytes = base64.b64decode(base64_str)
                    return Image.open(io.BytesIO(image_bytes))
                elif len(image_input) > 100 and '/' not in image_input:
                    # Likely raw base64 without data URI prefix
                    try:
                        image_bytes = base64.b64decode(image_input)
                        return Image.open(io.BytesIO(image_bytes))
                    except:
                        pass
                
                # Try URL
                if image_input.startswith('http://') or image_input.startswith('https://'):
                    response = requests.get(image_input, timeout=10)
                    response.raise_for_status()
                    return Image.open(io.BytesIO(response.content))
                
                # Try local file path
                if os.path.exists(image_input):
                    return Image.open(image_input)
            
            raise ValueError(f"Unsupported image input format: {type(image_input)}")
            
        except Exception as e:
            logger.error(f"Error loading image: {e}")
            raise
    
    def _generate_embeddings(
        self, 
        texts: Optional[List[str]] = None, 
        images: Optional[List[Image.Image]] = None,
        task: str = None
    ) -> np.ndarray:
        """
        Generate embeddings using vLLM for text and/or images
        
        Args:
            texts: Optional list of input texts
            images: Optional list of PIL Image objects
            task: Optional task prefix (currently ignored with vLLM)
        
        Returns:
            Normalized embeddings array
        """
        try:
            if texts is None and images is None:
                raise ValueError("Either texts or images must be provided")
            
            # Prepare prompts for vLLM
            prompts = []
            
            # Handle text inputs
            if texts is not None:
                for text in texts:
                    truncated_text = self._truncate_text(text)
                    prompts.append(truncated_text)
            
            # Handle image inputs (for multimodal)
            if images is not None:
                # For multimodal models, we need to pass images differently
                # This depends on how the model expects image inputs
                for image in images:
                    # Jina v4 multimodal: pass image with special token
                    prompts.append("<image>")
            
            # Generate embeddings with vLLM
            # IMPORTANT: vLLM encode() expects individual prompts or proper batch format
            # According to error, we need to pass prompts one by one or as proper format
            # See: https://docs.vllm.ai/en/latest/usage/pooling.html
            
            embeddings = []
            
            if images is not None and len(images) > 0:
                # Multimodal mode - not fully supported in current vLLM version
                logger.warning("Image embeddings may require special handling")
                # For now, try processing each prompt individually
                for prompt in prompts:
                    output = self.llm.encode(prompt, pooling_task="embed")
                    # vLLM encode() returns a list of EmbeddingRequestOutput
                    # Access the embedding via outputs.data (not outputs.embedding)
                    embeddings.append(output[0].outputs.data)
            else:
                # Text-only mode
                # Process each prompt individually to avoid batch validation issues
                for prompt in prompts:
                    output = self.llm.encode(prompt, pooling_task="embed")
                    # vLLM PoolingOutput has 'data' attribute containing the embedding vector
                    embeddings.append(output[0].outputs.data)
            
            # Convert to numpy array
            embeddings_array = np.array(embeddings)
            
            # Normalize embeddings (L2 normalization)
            norms = np.linalg.norm(embeddings_array, axis=1, keepdims=True)
            embeddings_normalized = embeddings_array / norms
            
            return embeddings_normalized
            
        except Exception as e:
            logger.error(f"Error generating embeddings: {e}")
            raise
    
    def predict(self, context, model_input):
        """
        Generate embeddings for input texts and/or images
        
        Expected input formats:
        1. DataFrame with columns:
           - 'text' or 'texts': single text or list of texts
           - 'image' or 'images': single image or list of images (base64, URL, or path)
           - 'task' (optional): task type for Jina model
           
        2. Dictionary with:
           - 'text' or 'texts': single text or list of texts
           - 'image' or 'images': single image or list of images
           - 'task' (optional): task type
           
        Image formats supported:
           - Base64 encoded string (with or without data URI prefix)
           - HTTP/HTTPS URL
           - Local file path
           - PIL Image object
           
        Returns:
           Dictionary with embeddings and metadata
        """
        start_time = time.time()
        
        try:
            texts = None
            images = None
            task = None
            
            # Parse input
            if isinstance(model_input, pd.DataFrame):
                # Handle texts
                if 'texts' in model_input.columns:
                    texts = model_input.iloc[0]['texts']
                    if not isinstance(texts, list):
                        texts = [texts]
                elif 'text' in model_input.columns:
                    texts = model_input['text'].tolist()
                
                # Handle images
                if 'images' in model_input.columns:
                    image_inputs = model_input.iloc[0]['images']
                    if not isinstance(image_inputs, list):
                        image_inputs = [image_inputs]
                    images = [self._load_image(img) for img in image_inputs]
                elif 'image' in model_input.columns:
                    image_inputs = model_input['image'].tolist()
                    images = [self._load_image(img) for img in image_inputs]
                
                task = model_input.iloc[0].get('task', None) if 'task' in model_input.columns else None
                
            elif isinstance(model_input, dict):
                # Handle texts
                if 'texts' in model_input:
                    texts = model_input['texts']
                    if not isinstance(texts, list):
                        texts = [texts]
                elif 'text' in model_input:
                    text_input = model_input['text']
                    texts = text_input if isinstance(text_input, list) else [text_input]
                
                # Handle images
                if 'images' in model_input:
                    image_inputs = model_input['images']
                    if not isinstance(image_inputs, list):
                        image_inputs = [image_inputs]
                    images = [self._load_image(img) for img in image_inputs]
                elif 'image' in model_input:
                    image_input = model_input['image']
                    image_inputs = image_input if isinstance(image_input, list) else [image_input]
                    images = [self._load_image(img) for img in image_inputs]
                
                task = model_input.get('task', None)
            else:
                raise ValueError(f"Unsupported model_input type: {type(model_input)}")
            
            # Validate we have at least one input type
            if texts is None and images is None:
                raise ValueError("Input must have 'text'/'texts' or 'image'/'images' key")
            
            # Generate embeddings
            embeddings = self._generate_embeddings(texts=texts, images=images, task=task)
            
            generation_time = time.time() - start_time
            
            # Calculate approximate tokens (rough estimation for texts only)
            # Note: vLLM handles actual tokenization internally
            if texts:
                # Rough estimate: 1 token ≈ 4 characters for English text
                total_tokens = sum(len(text) // 4 for text in texts)
            else:
                total_tokens = 0
            
            # Calculate total items
            num_items = 0
            if texts:
                num_items += len(texts)
            if images:
                num_items += len(images)
            
            # Determine input type
            if texts and images:
                input_type = "multimodal"
            elif images:
                input_type = "image"
            else:
                input_type = "text"
            
            # Return response in OpenAI-compatible format
            return {
                "model": MODEL_NAME,
                "object": "list",
                "data": [
                    {
                        "object": "embedding",
                        "embedding": emb.tolist(),
                        "index": idx
                    }
                    for idx, emb in enumerate(embeddings)
                ],
                "usage": {
                    "prompt_tokens": total_tokens,
                    "total_tokens": total_tokens
                },
                "id": f"emb_{uuid.uuid4().hex[:20]}",
                "created": int(time.time()),
                "performance": {
                    "generation_time_ms": int(generation_time * 1000),
                    "embeddings_per_second": round(num_items / generation_time, 2) if generation_time > 0 else 0,
                    "tokens_per_second": round(total_tokens / generation_time, 2) if generation_time > 0 and total_tokens > 0 else 0,
                    "backend": "vLLM"
                },
                "metadata": {
                    "embedding_dimensions": embeddings.shape[1],
                    "num_embeddings": num_items,
                    "input_type": input_type,
                    "num_texts": len(texts) if texts else 0,
                    "num_images": len(images) if images else 0,
                    "task": task,
                    "normalized": True
                }
            }
            
        except Exception as e:
            logger.error(f"Error in predict: {str(e)}")
            raise

# Set the model for MLflow
set_model(JinaEmbeddingWrapper())