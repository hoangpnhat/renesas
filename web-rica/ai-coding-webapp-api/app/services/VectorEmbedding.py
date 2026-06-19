"""
Vector Embedding Service
Generate and manage embeddings for images and text using Databricks serving endpoint
"""

import asyncio
import base64
import json
import logging
import math
from dataclasses import dataclass
from io import BytesIO
from typing import List, Dict, Any, Optional, cast

import pandas as pd
import requests
from PIL import Image


from core.config import settings

# Capture PIL's default pixel limit before disabling the check.
_PIL_MAX_PIXELS = Image.MAX_IMAGE_PIXELS  # 178_956_970
Image.MAX_IMAGE_PIXELS = None

from core import settings

logger = logging.getLogger(__name__)


@dataclass
class EmbeddingResult:
    """Result of embedding generation"""
    success: bool
    embeddings: List[List[float]] | None = None  # List of vectors
    error: Optional[str] = None
    total_processed: int = 0


class VectorEmbeddingService:
    """
    Service to generate embeddings for images and text
    Uses Databricks serving endpoint with ColPali model
    """

    def __init__(self):
        # self.endpoint_url = f"https://{settings.databricks.DATABRICKS_SERVER_HOSTNAME}/serving-endpoints/{settings.databricks.DATABRICKS_ENDPOINT_NAME}/invocations"
        self.endpoint_url = settings.databricks.DATABRICKS_EMBEDDING_ENDPOINT
        self.token = settings.databricks.DATABRICKS_TOKEN
        self.batch_size = 1
        self.max_concurrent_requests = 4

        logger.info(f"VectorEmbeddingService initialized")
        logger.info(f"   Endpoint: {self.endpoint_url}")
        logger.info(f"   Batch size: {self.batch_size}")

    def _image_bytes_to_base64(self, image_bytes: bytes) -> str:
        """
        Convert image bytes to base64 string

        Args:
            image_bytes: Image content as bytes

        Returns:
            Base64 encoded string with data URI prefix
        """
        try:
            # Open image from bytes
            with Image.open(BytesIO(image_bytes)) as img:
                # Convert to RGB if necessary
                if img.mode != 'RGB':
                    img = img.convert('RGB')

                # Resize only if image exceeds the endpoint's pixel limit
                if img.width * img.height > _PIL_MAX_PIXELS:
                #     scale = math.sqrt(_PIL_MAX_PIXELS / (img.width * img.height))
                #     new_w, new_h = int(img.width * scale), int(img.height * scale)
                #     logger.warning(
                #         f"Image exceeds pixel limit ({img.width}x{img.height} = "
                #         f"{img.width * img.height:,} px > {_PIL_MAX_PIXELS:,} px), "
                #         f"resizing to {new_w}x{new_h} (scale={scale:.4f})"
                #     )
                #     img = img.resize((new_w, new_h), Image.LANCZOS)
                    logger.warning(
                        f"Image exceeds pixel limit ({img.width}x{img.height} = "
                        f"{img.width * img.height:,} px > {_PIL_MAX_PIXELS:,} px), "
                        f"resizing to fit within {settings.databricks.FORCE_REDUCE_EMBEDDING_SIZE}x{settings.databricks.FORCE_REDUCE_EMBEDDING_SIZE} pixels"
                    )
                    force_size = settings.databricks.FORCE_REDUCE_EMBEDDING_SIZE
                    img.thumbnail((force_size, force_size), Image.Resampling.LANCZOS)

                # Save to bytes buffer as JPEG
                buffer = BytesIO()
                img.save(buffer, format='JPEG', quality=85)
                buffer.seek(0)

                # Encode to base64
                img_base64 = base64.b64encode(buffer.read()).decode('utf-8')
                return f"data:image/jpeg;base64,{img_base64}"

        except Exception as e:
            logger.exception(f"Failed to convert image to base64: {e}")
            raise

    # @retry(
    #     stop=stop_after_attempt(3),
    #     wait=wait_exponential(multiplier=1, min=2, max=10),
    #     reraise=True
    # )
    def _call_embedding_endpoint(self, dataset: pd.DataFrame) -> Dict[str, Any]:
        """
        Call Databricks embedding endpoint with retry logic

        Args:
            dataset: DataFrame with 'input' column

        Returns:
            Response JSON
        """
        headers = {
            'Authorization': f'Bearer {self.token}',
            'Content-Type': 'application/json'
        }

        # Format as dataframe_split
        data_dict = {
            'dataframe_split': dataset.to_dict(orient='split')
        }

        data_json = json.dumps(data_dict, allow_nan=True)

        logger.debug(f"Calling embedding endpoint (batch size: {len(dataset)})")

        response = requests.post(
            url=self.endpoint_url,
            headers=headers,
            data=data_json,
        )
        if response.status_code != 200:
            error_msg = f"Endpoint returned {response.status_code}: {response.text}"
            logger.error(f"{error_msg}")
            raise Exception(error_msg)

        return cast(Dict[str, Any], response.json())

    def _extract_embeddings_from_response(self, response: Dict[str, Any]) -> List[List[float]]:
        """
        Extract embedding vectors from endpoint response

        Args:
            response: Response JSON from endpoint

        Returns:
            List of embedding vectors
        """
        try:
            predictions = response.get('predictions', {})
            data = predictions.get('data', [])

            embeddings = []
            for item in data:
                embedding = item.get('embedding', [])
                if not embedding:
                    logger.warning("Empty embedding in response")
                embeddings.append(embedding)

            return embeddings

        except Exception as e:
            logger.exception(f"Failed to extract embeddings: {e}")
            raise

    async def _process_single_batch(
            self,
            batch: List[bytes],
            batch_num: int,
            total_batches: int
    ) -> List[List[float]]:
        """
        Process a single batch of images

        Args:
            batch: List of image bytes
            batch_num: Batch number (for logging)
            total_batches: Total number of batches

        Returns:
            List of embeddings
        """
        try:
            logger.info(f"Batch {batch_num}/{total_batches}: Processing {len(batch)} images")

            # Convert to base64
            base64_images: list[str | None] = []
            for idx, img_bytes in enumerate(batch):
                try:
                    base64_img = self._image_bytes_to_base64(img_bytes)
                    base64_images.append(base64_img)
                except Exception as e:
                    logger.error(f"Failed to convert image {idx}: {e}")
                    base64_images.append(None)

            # Filter out None values
            valid_images = [img for img in base64_images if img is not None]

            if not valid_images:
                logger.warning(f"Batch {batch_num}: No valid images")
                return [[]] * len(batch)  # Return empty embeddings

            # Create DataFrame
            dataset = pd.DataFrame({"input": valid_images})

            # Call endpoint (run in thread to not block event loop)
            response = await asyncio.to_thread(
                self._call_embedding_endpoint,
                dataset
            )

            # Extract embeddings
            embeddings = self._extract_embeddings_from_response(response)

            logger.info(f"Batch {batch_num}/{total_batches}: {len(embeddings)} embeddings generated")

            # Pad with empty embeddings for failed conversions
            result: list[Any] = []
            valid_idx = 0
            for img in base64_images:
                if img is None:
                    result.append([])
                else:
                    result.append(embeddings[valid_idx] if valid_idx < len(embeddings) else [])
                    valid_idx += 1

            return result

        except Exception as e:
            logger.exception(f"Batch {batch_num}/{total_batches} failed: {e}")
            # Return empty embeddings for failed batch
            return [[]] * len(batch)

    async def generate_image_embeddings_batch(
            self,
            image_bytes_list: List[bytes],
            batch_size: Optional[int] = None,
            max_concurrent: Optional[int] = None
    ) -> EmbeddingResult:
        """
        Generate embeddings for multiple images in batches

        Args:
            image_bytes_list: List of image bytes
            batch_size: Override default batch size
            max_concurrent: Override max concurrent requests (default: 4)

        Returns:
            EmbeddingResult with all embeddings
        """
        if batch_size is None:
            batch_size = self.batch_size

        if max_concurrent is None:
            max_concurrent = self.max_concurrent_requests

        total_images = len(image_bytes_list)
        logger.info(f"Generating embeddings for {total_images} images (batch size: {batch_size})")

        all_embeddings: List[List[float]] = []

        try:
            # Split into batches
            batches = []
            for i in range(0, total_images, batch_size):
                batch = image_bytes_list[i:i + batch_size]
                batches.append(batch)

            total_batches = len(batches)
            logger.info(f"Total batches: {total_batches}")

            # Process batches with concurrency limit using semaphore
            semaphore = asyncio.Semaphore(max_concurrent)

            async def process_with_semaphore(batch, batch_num):
                async with semaphore:
                    return await self._process_single_batch(batch, batch_num, total_batches)

            # Create tasks for all batches
            tasks = [
                process_with_semaphore(batch, idx + 1)
                for idx, batch in enumerate(batches)
            ]

            # Run tasks concurrently (respecting semaphore limit)
            logger.info(f"Processing {total_batches} batches with max {max_concurrent} concurrent requests...")
            batch_results = await asyncio.gather(*tasks, return_exceptions=True)

            # Flatten results
            all_embeddings = []
            for result in batch_results:
                if isinstance(result, Exception):
                    logger.error(f"Batch failed: {result}")
                    continue
                all_embeddings.extend(cast(list, result))

            # Calculate success rate
            successful_embeddings = sum(1 for emb in all_embeddings if len(emb) > 0)
            success_rate = (successful_embeddings / total_images) * 100 if total_images > 0 else 0

            logger.info(f"   Total processed: {len(all_embeddings)}/{total_images}")
            logger.info(f"   Success rate: {success_rate:.1f}%")

            success = success_rate > 50

            return EmbeddingResult(
                success=success,
                embeddings=all_embeddings,
                total_processed=len(all_embeddings)
            )

        except Exception as e:
            logger.exception(f"Failed to generate embeddings: {e}")
            return EmbeddingResult(
                success=False,
                error=str(e),
                embeddings=all_embeddings if all_embeddings else None,
                total_processed=len(all_embeddings)
            )

    async def generate_text_embedding(self, text: str) -> List[float]:
        """
        Generate embedding for a single text query

        Args:
            text: Text query

        Returns:
            Embedding vector
        """
        try:
            dataset = pd.DataFrame({
                "input": [text]
            })

            response = await asyncio.to_thread(
                self._call_embedding_endpoint,
                dataset
            )

            embeddings = self._extract_embeddings_from_response(response)

            if embeddings and len(embeddings) > 0:
                return embeddings[0]

            return []

        except Exception as e:
            logger.exception(f"Failed to generate text embedding: {e}")
            raise


# Singleton

_embedding_service = None


def get_embedding_service() -> VectorEmbeddingService:
    """Get embedding service singleton"""
    global _embedding_service
    if _embedding_service is None:
        _embedding_service = VectorEmbeddingService()
    return _embedding_service
