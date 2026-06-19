"""
ICT Trading System - Utility Functions
======================================
Helper functions for data generation and system utilities
"""

import pandas as pd
import numpy as np
from datetime import datetime, timedelta
from typing import Dict


def generate_sample_data(timeframes=['1h', '4h', 'D'], num_candles=1000, start_price=50000):
    """
    Generate sample OHLCV data for testing

    Args:
        timeframes: List of timeframe strings
        num_candles: Number of candles to generate per timeframe
        start_price: Starting price for the asset

    Returns:
        Dict[str, pd.DataFrame]: Multi-timeframe data
    """
    data = {}

    for tf in timeframes:
        dates = pd.date_range(
            start=datetime.now() - timedelta(days=num_candles),
            periods=num_candles,
            freq='1h'
        )

        # Generate realistic price movement
        np.random.seed(42)
        returns = np.random.normal(0.0002, 0.02, num_candles)

        prices = [start_price]
        for ret in returns[:-1]:
            prices.append(prices[-1] * (1 + ret))

        # Create OHLCV data
        opens = prices
        closes = [p * (1 + np.random.normal(0, 0.005)) for p in prices]
        highs = [max(o, c) * (1 + abs(np.random.normal(0, 0.003)))
                for o, c in zip(opens, closes)]
        lows = [min(o, c) * (1 - abs(np.random.normal(0, 0.003)))
               for o, c in zip(opens, closes)]
        volumes = np.random.uniform(100, 1000, num_candles)

        df = pd.DataFrame({
            'timestamp': dates,
            'open': opens,
            'high': highs,
            'low': lows,
            'close': closes,
            'volume': volumes
        })

        data[tf] = df

    return data


def calculate_position_size(balance: float, risk_percent: float,
                           entry_price: float, stop_loss: float) -> float:
    """
    Calculate position size based on risk management

    Args:
        balance: Account balance
        risk_percent: Percentage of balance to risk (e.g., 1.0 for 1%)
        entry_price: Entry price for the trade
        stop_loss: Stop loss price

    Returns:
        Position size in units
    """
    risk_amount = balance * (risk_percent / 100)
    price_risk = abs(entry_price - stop_loss)

    if price_risk == 0:
        return 0

    position_size = risk_amount / price_risk
    return position_size


def calculate_risk_reward_ratio(entry: float, stop_loss: float,
                                take_profit: float) -> float:
    """
    Calculate risk-reward ratio for a trade

    Args:
        entry: Entry price
        stop_loss: Stop loss price
        take_profit: Take profit price

    Returns:
        Risk-reward ratio
    """
    risk = abs(entry - stop_loss)
    reward = abs(take_profit - entry)

    if risk == 0:
        return 0

    return reward / risk


def print_system_info():
    """Print ICT trading system information"""
    print("=" * 80)
    print("ICT COMPLETE TRADING SYSTEM")
    print("=" * 80)
    print("\nComponents:")
    print("  1. PD Array Detection (Multi-timeframe)")
    print("  2. SMT Divergence Analysis")
    print("  3. LLM Market Analyzer")
    print("  4. RL Bot with Teacher Guidance")
    print("  5. HTF-LTF Confluence System")
    print("\nFeatures:")
    print("  - Order Blocks (Bullish/Bearish)")
    print("  - Fair Value Gaps")
    print("  - Breaker Blocks")
    print("  - Mitigation Blocks")
    print("  - Liquidity Voids")
    print("  - Market Structure (HH, LL, HL, LH)")
    print("  - Smart Money Technique (SMT)")
    print("  - Premium/Discount Arrays")
    print("=" * 80)


def validate_dataframe(df: pd.DataFrame) -> bool:
    """
    Validate that a DataFrame has required OHLCV columns

    Args:
        df: DataFrame to validate

    Returns:
        True if valid, False otherwise
    """
    required_columns = ['open', 'high', 'low', 'close', 'volume']

    for col in required_columns:
        if col not in df.columns:
            print(f"Error: Missing required column '{col}'")
            return False

    if len(df) == 0:
        print("Error: DataFrame is empty")
        return False

    return True


def format_price(price: float, decimals: int = 2) -> str:
    """Format price for display"""
    return f"${price:,.{decimals}f}"


def format_percentage(value: float, decimals: int = 2) -> str:
    """Format percentage for display"""
    return f"{value:.{decimals}f}%"
