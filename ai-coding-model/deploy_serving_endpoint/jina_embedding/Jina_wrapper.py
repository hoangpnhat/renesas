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
MODEL_NAME = "jinaai/jina-embeddings-v4"

class JinaEmbeddingWrapper(mlflow.pyfunc.PythonModel):
    """
    Transformers-based PyFunc wrapper for Jina Embeddings v4
    High-performance embedding generation for text and images
    Returns multiple-vector embeddings (all token embeddings) instead of single pooled vector
    """
    
    def __init__(self):
        self.model = None
        self.max_seq_length = 8192
        self.device = "cuda" if torch.cuda.is_available() else "cpu"
    
    def load_context(self, context):
        """Load Jina embedding model with Transformers"""
        logger.info(f"Loading Jina embedding model with Transformers: {MODEL_NAME}")

        try:
            # Detect GPU type to choose appropriate attention implementation
            # T4 GPUs don't support FlashAttention-2, use eager or sdpa instead
            attn_impl = "eager"  # Safe default for all GPUs

            if torch.cuda.is_available():
                gpu_name = torch.cuda.get_device_name(0).lower()
                # A100, H100, etc. support FlashAttention-2
                if any(x in gpu_name for x in ['a100', 'h100', 'a10', 'l40', 'rtx 40', 'rtx 30']):
                    try:
                        import flash_attn
                        attn_impl = "flash_attention_2"
                        logger.info(f"Using FlashAttention-2 on {gpu_name}")
                    except ImportError:
                        attn_impl = "sdpa"  # Scaled Dot Product Attention
                        logger.info(f"FlashAttention not installed, using SDPA on {gpu_name}")
                else:
                    # T4 and older GPUs - use eager attention
                    attn_impl = "eager"
                    logger.info(f"Using eager attention for GPU: {gpu_name}")

            # Load model with Transformers
            self.model = AutoModel.from_pretrained(
                MODEL_NAME,
                trust_remote_code=True,
                torch_dtype=torch.float16,
                attn_implementation=attn_impl,
                low_cpu_mem_usage=True
            )

            # Move model to GPU if available
            self.model.to(self.device)

            logger.info(f"Jina embedding model loaded successfully on {self.device}")

            # Warmup
            self._warmup()

        except Exception as e:
            logger.error(f"Failed to load model: {str(e)}")
            logger.error(f"Traceback: {e.__class__.__name__}: {str(e)}")
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
            _ = self._generate_text_embeddings(
                texts=dummy_texts,
                task="retrieval",
                prompt_name="query",
                return_multivector=True
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
            return True
        
        # Check for base64 (very long string without spaces, likely base64)
        # Must check this before file path to avoid "File name too long" error
        if len(value) > 200 and ' ' not in value:
            # Additional check: base64 strings typically don't have common text patterns
            if not any(word in value.lower() for word in ['the', 'is', 'and', 'or', 'but', 'in', 'on', 'at']):
                return True
        
        # Check for file path with image extension (only for shorter strings)
        if len(value) < 260:  # Windows MAX_PATH limit
            if value.lower().endswith(('.png', '.jpg', '.jpeg', '.gif', '.bmp', '.tiff')):
                if os.path.exists(value):
                    return True
        
        # Otherwise, treat as text
        return False
    
    
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
                # Try base64 first (check this BEFORE file path to avoid "File name too long" error)
                if image_input.startswith('data:image'):
                    # Data URI format: data:image/png;base64,<base64_string>
                    base64_str = image_input.split(',', 1)[1]
                    image_bytes = base64.b64decode(base64_str)
                    return Image.open(io.BytesIO(image_bytes))
                
                # Try URL
                if image_input.startswith('http://') or image_input.startswith('https://'):
                    response = requests.get(image_input, timeout=10)
                    response.raise_for_status()
                    return Image.open(io.BytesIO(response.content))
                
                # Try raw base64 (long strings without common text patterns)
                if len(image_input) > 100:
                    # Likely raw base64 without data URI prefix
                    try:
                        image_bytes = base64.b64decode(image_input, validate=True)
                        return Image.open(io.BytesIO(image_bytes))
                    except Exception:
                        # Not valid base64, continue to file path check
                        pass
                
                # Try local file path (only if string is not too long)
                if len(image_input) < 260:  # Windows MAX_PATH limit
                    if os.path.exists(image_input):
                        return Image.open(image_input)
            
            raise ValueError(f"Unsupported image input format: {type(image_input)}")
            
        except Exception as e:
            logger.error(f"Error loading image: {e}")
            raise
    
    def _generate_text_embeddings(
        self,
        texts: List[str],
        task: str = "retrieval",
        prompt_name: str = "query",
        return_multivector: bool = True
    ) -> List[torch.Tensor]:
        """
        Generate embeddings for text using Jina v4 encode_text method
        
        Args:
            texts: List of input texts
            task: Task type ("retrieval", "text-matching", "code")
            prompt_name: Prompt name ("query" or "passage")
            return_multivector: If True, return all token embeddings; if False, return pooled embedding
        
        Returns:
            List of embedding tensors (multi-vector or pooled)
        """
        try:
            # Use Jina's encode_text method
            embeddings = self.model.encode_text(
                texts=texts,
                task=task,
                prompt_name=prompt_name,
                return_multivector=return_multivector
            )
            
            # embeddings is already a list of tensors or numpy arrays
            # Convert to list of tensors if needed
            if not isinstance(embeddings, list):
                embeddings = [embeddings]
            
            result = []
            for emb in embeddings:
                if not isinstance(emb, torch.Tensor):
                    emb = torch.tensor(emb, dtype=torch.float16)
                result.append(emb)
            
            return result
            
        except Exception as e:
            logger.error(f"Error generating text embeddings: {e}")
            raise
    
    def _generate_image_embeddings(
        self,
        images: List[Union[str, Image.Image]],
        task: str = "retrieval",
        return_multivector: bool = True
    ) -> List[torch.Tensor]:
        """
        Generate embeddings for images using Jina v4 encode_image method
        
        Args:
            images: List of images (URLs, paths, base64 strings, or PIL Images)
            task: Task type (default "retrieval")
            return_multivector: If True, return all token embeddings; if False, return pooled embedding
        
        Returns:
            List of embedding tensors (multi-vector or pooled)
        """
        try:
            # Convert all images to PIL Image objects to avoid "File name too long" error
            # when base64 strings are passed directly to the model
            pil_images = []
            for img in images:
                if isinstance(img, Image.Image):
                    pil_images.append(img)
                else:
                    # Convert string (base64, URL, or path) to PIL Image
                    pil_images.append(self._load_image(img))
            
            # Use Jina's encode_image method with PIL Images
            embeddings = self.model.encode_image(
                images=pil_images,
                task=task,
                return_multivector=return_multivector
            )
            
            # embeddings is already a list of tensors or numpy arrays
            # Convert to list of tensors if needed
            if not isinstance(embeddings, list):
                embeddings = [embeddings]
            
            result = []
            for emb in embeddings:
                if not isinstance(emb, torch.Tensor):
                    emb = torch.tensor(emb, dtype=torch.float16)
                result.append(emb)
            
            return result
            
        except Exception as e:
            logger.error(f"Error generating image embeddings: {e}")
            raise
    
    def predict(self, context, model_input):
        """
        Generate embeddings for input texts and/or images
        
        Expected input formats:
        1. DataFrame with columns:
           - 'text' or 'texts': single text or list of texts
           - 'image' or 'images': single image or list of images (base64, URL, or path)
           - 'task' (optional): task type ("retrieval", "text-matching", "code")
           - 'prompt_name' (optional): "query" or "passage" (for retrieval task)
           - 'return_multivector' (optional): True/False (default True)
           
        2. Dictionary with:
           - 'text' or 'texts': single text or list of texts
           - 'image' or 'images': single image or list of images
           - 'task' (optional): task type
           - 'prompt_name' (optional): "query" or "passage"
           - 'return_multivector' (optional): True/False
           
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
            task = "retrieval"  # Default task
            prompt_name = "query"  # Default prompt name
            return_multivector = True  # Default to multivector
            
            # Parse input
            if isinstance(model_input, pd.DataFrame):
                # Handle texts
                if 'texts' in model_input.columns:
                    texts = model_input.iloc[0]['texts']
                    if not isinstance(texts, list):
                        texts = [texts]
                elif 'text' in model_input.columns:
                    # Auto-detect: text values vs image values
                    text_values = model_input['text'].tolist()
                    
                    # Check if values are images or text
                    texts_list = []
                    images_list = []
                    
                    for value in text_values:
                        if self._is_image_input(value):
                            # It's an image
                            images_list.append(value)
                        else:
                            # It's text
                            texts_list.append(value)
                    
                    # Set texts and images based on what we found
                    if texts_list:
                        texts = texts_list
                    if images_list:
                        images = images_list  # Keep as strings for now
                elif 'input' in model_input.columns:
                    # Generic 'input' column - auto-detect type (text vs image)
                    input_values = model_input['input'].tolist()
                    
                    texts_list = []
                    images_list = []
                    
                    for value in input_values:
                        if self._is_image_input(value):
                            images_list.append(value)
                        else:
                            texts_list.append(value)
                    
                    # Set texts and images based on what we found
                    if texts_list:
                        texts = texts_list
                    if images_list:
                        images = images_list
                
                # Handle images
                if 'images' in model_input.columns:
                    images = model_input.iloc[0]['images']
                    if not isinstance(images, list):
                        images = [images]
                elif 'image' in model_input.columns and not images:
                    # Only process if not already handled by other columns
                    images = model_input['image'].tolist()
                
                # Get optional parameters
                task = model_input.iloc[0].get('task', task) if 'task' in model_input.columns else task
                prompt_name = model_input.iloc[0].get('prompt_name', prompt_name) if 'prompt_name' in model_input.columns else prompt_name
                return_multivector = model_input.iloc[0].get('return_multivector', return_multivector) if 'return_multivector' in model_input.columns else return_multivector
                
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
                    images = model_input['images']
                    if not isinstance(images, list):
                        images = [images]
                elif 'image' in model_input:
                    image_input = model_input['image']
                    images = image_input if isinstance(image_input, list) else [image_input]
                
                # Get optional parameters
                task = model_input.get('task', task)
                prompt_name = model_input.get('prompt_name', prompt_name)
                return_multivector = model_input.get('return_multivector', return_multivector)
            else:
                raise ValueError(f"Unsupported model_input type: {type(model_input)}")
            
            # Validate we have at least one input type
            if texts is None and images is None:
                raise ValueError("Input must have 'text'/'texts' or 'image'/'images' key")
            
            # Generate embeddings based on input type
            embeddings = []
            
            if texts is not None:
                text_embeddings = self._generate_text_embeddings(
                    texts=texts,
                    task=task,
                    prompt_name=prompt_name,
                    return_multivector=return_multivector
                )
                embeddings.extend(text_embeddings)
            
            if images is not None:
                image_embeddings = self._generate_image_embeddings(
                    images=images,
                    task=task,
                    return_multivector=return_multivector
                )
                embeddings.extend(image_embeddings)
            
            generation_time = time.time() - start_time
            
            # Calculate metrics based on embedding shape
            total_tokens = 0
            for emb in embeddings:
                if emb.ndim == 2:
                    # Multi-vector: [num_tokens, embedding_dim]
                    total_tokens += emb.shape[0]
                else:
                    # Pooled: [embedding_dim]
                    total_tokens += 1
            
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
            
            # Convert embeddings to list format
            embeddings_data = []
            for idx, emb in enumerate(embeddings):
                # Convert tensor to numpy array then to list
                emb_array = emb.cpu().numpy() if isinstance(emb, torch.Tensor) else emb
                
                if emb_array.ndim == 2:
                    # Multi-vector embedding
                    embeddings_data.append({
                        "object": "embedding",
                        "embedding": emb_array.tolist(),  # Multi-vector: shape [num_tokens, dim]
                        "index": idx,
                        "num_tokens": emb_array.shape[0],
                        "embedding_dim": emb_array.shape[1]
                    })
                else:
                    # Pooled embedding
                    embeddings_data.append({
                        "object": "embedding",
                        "embedding": emb_array.tolist(),  # Pooled: shape [dim]
                        "index": idx,
                        "embedding_dim": emb_array.shape[0]
                    })
            
            # Return response in custom format
            return {
                "model": MODEL_NAME,
                "object": "list",
                "data": embeddings_data,
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
                    "backend": "Transformers"
                },
                "metadata": {
                    "embedding_type": "multi-vector" if return_multivector else "pooled",
                    "num_embeddings": num_items,
                    "total_tokens": total_tokens,
                    "input_type": input_type,
                    "num_texts": len(texts) if texts else 0,
                    "num_images": len(images) if images else 0,
                    "task": task,
                    "prompt_name": prompt_name if texts else None,
                    "normalized": True
                }
            }
            
        except Exception as e:
            logger.error(f"Error in predict: {str(e)}")
            raise

# Set the model for MLflow
set_model(JinaEmbeddingWrapper())