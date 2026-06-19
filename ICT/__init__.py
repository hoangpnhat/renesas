"""
Enhanced ICT Trading System
============================
Professional-grade Inner Circle Trader (ICT) system with advanced market structure analysis,
liquidity detection, and AI-powered reinforcement learning.

Version: 2.0.0
"""

__version__ = "2.0.0"
__author__ = "ICT Trading System Team"

# Core components
from core import (
    EnhancedPDDetector,
    MarketStructureDetector,
    LiquidityDetector,
    SessionAnalyzer,
    PriceAttractorSystem,
    OrderFlowAnalyzer,
    SMTAnalyzer,
)

# Analysis
from analysis import (
    EnhancedLLMAnalyzer,
    FeatureExtractionPipeline,
)

# RL
from rl import (
    EnhancedICTTradingEnv,
    CurriculumLearningCallback,
    PerformanceMonitorCallback,
    EarlyStoppingCallback,
)

# Data
from data import PDArray, SMTDivergence

# Utils
from utils import generate_sample_data, print_system_info

__all__ = [
    # Core
    'EnhancedPDDetector',
    'MarketStructureDetector',
    'LiquidityDetector',
    'SessionAnalyzer',
    'PriceAttractorSystem',
    'OrderFlowAnalyzer',
    'SMTAnalyzer',
    # Analysis
    'EnhancedLLMAnalyzer',
    'FeatureExtractionPipeline',
    # RL
    'EnhancedICTTradingEnv',
    'CurriculumLearningCallback',
    'PerformanceMonitorCallback',
    'EarlyStoppingCallback',
    # Data
    'PDArray',
    'SMTDivergence',
    # Utils
    'generate_sample_data',
    'print_system_info',
]
