"""
Pipeline stages

Each stage processes batches and passes results to the next stage.
"""

from .base import BasePipelineStage, StageResult
from .splitter import PDFSplitterStage
from .converter import ImageConverterStage
from .uploader import VolumeUploaderStage
from .embedder import EmbeddingGeneratorStage
from .indexer import ElasticsearchIndexerStage
from .preprocess import ExcelPreprocessStage

__all__ = [
    "BasePipelineStage",
    "StageResult",
    "PDFSplitterStage",
    "ImageConverterStage",
    "VolumeUploaderStage",
    "EmbeddingGeneratorStage",
    "ElasticsearchIndexerStage",
    "ExcelPreprocessStage"
]
