"""
ICT Trading System - Data Structures
=====================================
Contains core data classes for PD Arrays and SMT Divergences
"""

from dataclasses import dataclass
from typing import Optional
from datetime import datetime


@dataclass
class PDArray:
    """Premium/Discount Array structure"""
    type: str  # 'order_block', 'fvg', 'breaker', 'mitigation', 'liquidity_void'
    direction: str  # 'bullish' or 'bearish'
    timeframe: str
    high: float
    low: float
    start_time: Optional[datetime]
    end_time: Optional[datetime]
    volume: float
    strength: float  # 0-100
    pd_status: str  # 'premium', 'discount', 'equilibrium'
    mitigated: bool = False
    mitigation_time: Optional[datetime] = None


@dataclass
class SMTDivergence:
    """Smart Money Technique Divergence"""
    type: str  # 'bullish_smt' or 'bearish_smt'
    asset1: str
    asset2: str
    timeframe: str
    asset1_high: float
    asset1_low: float
    asset2_high: float
    asset2_low: float
    divergence_time: Optional[datetime]
    strength: float
    description: str
