"""
Image Converter Stage

Stage 2: Convert PDF pages to images.
CPU-bound operation - limited workers.
"""

import asyncio
from io import BytesIO
from loguru import logger
from pdf2image import convert_from_bytes
import PIL.Image

# Disable PIL decompression bomb check
PIL.Image.MAX_IMAGE_PIXELS = None

from .base import BasePipelineStage, StageResult
from ..models import PageBatch, ImageBatch, ImageData, PageStatus


class ImageConverterStage(BasePipelineStage[PageBatch, ImageBatch]):
    """
    Stage 2: Convert PDF pages to images.
    CPU-bound operation - limited workers.
    """

    stage_name = "converter"
    input_status = PageStatus.CONVERTING
    output_status = PageStatus.UPLOADING

    @property
    def num_workers(self) -> int:
        return self.config.convert_workers

    @property
    def timeout(self) -> int:
        return self.config.convert_timeout

    async def process_batch(self, batch: PageBatch) -> StageResult:
        """Convert a batch of PDF pages to images"""
        try:
            logger.debug(f"[{batch.file_id}] Converting pages {batch.page_numbers}")

            if not batch.pdf_bytes:
                return StageResult(
                    success=False,
                    error="No PDF bytes provided",
                    failed_pages=batch.page_numbers
                )

            # Convert pages in thread (CPU bound)
            first_page = min(batch.page_numbers)
            last_page = max(batch.page_numbers)

            poppler_path = self.config.poppler_path
            pil_images = await asyncio.to_thread(
                convert_from_bytes,
                batch.pdf_bytes,
                dpi=self.config.dpi,
                first_page=first_page,
                last_page=last_page,
                fmt=self.config.image_format.lower(),
                poppler_path=poppler_path  # type: ignore[arg-type]
            )

            # Build image data list
            images = []
            for idx, page_num in enumerate(range(first_page, last_page + 1)):
                if page_num not in batch.page_numbers:
                    continue

                if idx >= len(pil_images):
                    continue

                pil_image = pil_images[idx]

                # Convert to bytes
                img_buffer = BytesIO()
                pil_image.save(img_buffer, format=self.config.image_format)
                img_buffer.seek(0)

                images.append(ImageData(
                    page_num=page_num,
                    image_bytes=img_buffer.getvalue(),
                    filename=f"page_{page_num}.{self.config.image_format.lower()}"
                ))

                # Free memory
                pil_image.close()

            output = ImageBatch(
                file_id=batch.file_id,
                batch_id=batch.batch_id,
                images=images
            )

            logger.info(f"[{batch.file_id}] Converted {len(images)} pages")

            return StageResult(success=True, output=output)

        except Exception as e:
            logger.exception(f"[{batch.file_id}] Conversion failed: {e}")
            return StageResult(
                success=False,
                error=str(e),
                failed_pages=batch.page_numbers
            )
