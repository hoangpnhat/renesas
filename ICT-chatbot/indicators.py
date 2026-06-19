"""
ICT Indicator Detection Functions
Implements: Liquidity Sweeps, Market Structure Shifts, Fair Value Gaps
"""
import numpy as np
import pandas as pd
from typing import List, Optional, Tuple
from datetime import datetime

from structures import FVG, SwingPoint, MSS, LiquiditySweep, BiasDirection
import config


def calculate_atr(ohlc: pd.DataFrame, period: int = 14) -> pd.Series:
    """
    Calculate Average True Range

    Args:
        ohlc: DataFrame with OHLC data
        period: ATR period

    Returns:
        Series of ATR values
    """
    high = ohlc['high']
    low = ohlc['low']
    close = ohlc['close']

    tr1 = high - low
    tr2 = abs(high - close.shift(1))
    tr3 = abs(low - close.shift(1))

    tr = pd.concat([tr1, tr2, tr3], axis=1).max(axis=1)
    atr = tr.rolling(window=period).mean()

    return atr


def detect_swing_points(
    ohlc: pd.DataFrame,
    pivot_window: int = 5,
    lookback: int = 50
) -> Tuple[List[SwingPoint], List[SwingPoint]]:
    """
    Detect swing highs and swing lows using N-bar pivot method

    Args:
        ohlc: DataFrame with OHLC data
        pivot_window: Number of bars on each side for pivot
        lookback: How many bars to look back

    Returns:
        (swing_highs, swing_lows) lists
    """
    swing_highs = []
    swing_lows = []

    start_idx = max(pivot_window, len(ohlc) - lookback)

    for i in range(start_idx, len(ohlc) - pivot_window):
        # Check for swing high
        window_highs = ohlc['high'].iloc[i - pivot_window:i + pivot_window + 1]
        if ohlc['high'].iloc[i] == window_highs.max():
            swing_highs.append(SwingPoint(
                swing_type='HIGH',
                price=ohlc['high'].iloc[i],
                bar_index=i,
                timestamp=ohlc.index[i],
                swept=False
            ))

        # Check for swing low
        window_lows = ohlc['low'].iloc[i - pivot_window:i + pivot_window + 1]
        if ohlc['low'].iloc[i] == window_lows.min():
            swing_lows.append(SwingPoint(
                swing_type='LOW',
                price=ohlc['low'].iloc[i],
                bar_index=i,
                timestamp=ohlc.index[i],
                swept=False
            ))

    return swing_highs, swing_lows


def detect_liquidity_sweep(
    ohlc: pd.DataFrame,
    current_idx: int,
    swing_highs: List[SwingPoint],
    swing_lows: List[SwingPoint],
    reversal_bars: int = 10
) -> Optional[LiquiditySweep]:
    """
    Detect liquidity sweep: price breaks swing level then reverses

    Args:
        ohlc: 1M OHLC DataFrame
        current_idx: Current bar index
        swing_highs: List of recent swing highs
        swing_lows: List of recent swing lows
        reversal_bars: Max bars to confirm reversal

    Returns:
        LiquiditySweep or None
    """
    if current_idx < reversal_bars:
        return None

    current_bar = ohlc.iloc[current_idx]

    # Check for bearish sweep (sweep above swing high, then close below)
    for swing in swing_highs:
        if swing.swept:
            continue

        # Price broke above swing high
        if current_bar['high'] > swing.price:
            # But closed below it
            if current_bar['close'] < swing.price:
                # Check if it stays below in next few bars
                future_bars = ohlc.iloc[current_idx:current_idx + reversal_bars]
                if len(future_bars) > 0 and (future_bars['close'] < swing.price).any():
                    swing.swept = True
                    return LiquiditySweep(
                        direction='BEARISH',
                        level=swing.price,
                        sweep_bar=current_idx,
                        sweep_time=current_bar.name,
                        bars_since=0
                    )

    # Check for bullish sweep (sweep below swing low, then close above)
    for swing in swing_lows:
        if swing.swept:
            continue

        # Price broke below swing low
        if current_bar['low'] < swing.price:
            # But closed above it
            if current_bar['close'] > swing.price:
                # Check if it stays above in next few bars
                future_bars = ohlc.iloc[current_idx:current_idx + reversal_bars]
                if len(future_bars) > 0 and (future_bars['close'] > swing.price).any():
                    swing.swept = True
                    return LiquiditySweep(
                        direction='BULLISH',
                        level=swing.price,
                        sweep_bar=current_idx,
                        sweep_time=current_bar.name,
                        bars_since=0
                    )

    return None


def detect_market_structure_shift(
    ohlc_15m: pd.DataFrame,
    current_idx_15m: int,
    atr_15m: pd.Series,
    lookback: int = 30,
    min_move_mult: float = 2.0
) -> Optional[MSS]:
    """
    Detect Market Structure Shift on 15M timeframe

    Bullish MSS: Break above last Lower High during downtrend
    Bearish MSS: Break below last Higher Low during uptrend

    Args:
        ohlc_15m: 15M OHLC DataFrame
        current_idx_15m: Current 15M bar index
        atr_15m: ATR series for 15M
        lookback: Bars to analyze structure
        min_move_mult: Minimum move as ATR multiple

    Returns:
        MSS or None
    """
    if current_idx_15m < lookback:
        return None

    start_idx = current_idx_15m - lookback
    window = ohlc_15m.iloc[start_idx:current_idx_15m + 1]

    current_bar = ohlc_15m.iloc[current_idx_15m]
    current_atr = atr_15m.iloc[current_idx_15m]

    if pd.isna(current_atr) or current_atr == 0:
        return None

    # Find swing points in lookback window
    swing_highs, swing_lows = detect_swing_points(
        window,
        pivot_window=config.MSS_STRUCTURE_PIVOTS,
        lookback=lookback
    )

    if len(swing_highs) < 2 or len(swing_lows) < 2:
        return None

    # Check for bullish MSS (break above last LH in downtrend)
    # Downtrend = sequence of lower highs
    recent_highs = sorted(swing_highs[-3:], key=lambda x: x.timestamp)
    if len(recent_highs) >= 2:
        # Check if making lower highs (downtrend)
        if recent_highs[-1].price < recent_highs[-2].price:
            last_lh = recent_highs[-1].price

            # Check if current close breaks above last LH
            if current_bar['close'] > last_lh:
                move_size = current_bar['close'] - last_lh

                # Must be significant move
                if move_size > min_move_mult * current_atr:
                    return MSS(
                        direction=BiasDirection.BULLISH,
                        structure_level=last_lh,
                        break_bar=current_idx_15m,
                        break_time=current_bar.name,
                        bars_since=0
                    )

    # Check for bearish MSS (break below last HL in uptrend)
    # Uptrend = sequence of higher lows
    recent_lows = sorted(swing_lows[-3:], key=lambda x: x.timestamp)
    if len(recent_lows) >= 2:
        # Check if making higher lows (uptrend)
        if recent_lows[-1].price > recent_lows[-2].price:
            last_hl = recent_lows[-1].price

            # Check if current close breaks below last HL
            if current_bar['close'] < last_hl:
                move_size = last_hl - current_bar['close']

                # Must be significant move
                if move_size > min_move_mult * current_atr:
                    return MSS(
                        direction=BiasDirection.BEARISH,
                        structure_level=last_hl,
                        break_bar=current_idx_15m,
                        break_time=current_bar.name,
                        bars_since=0
                    )

    return None


def detect_fair_value_gaps(
    ohlc: pd.DataFrame,
    current_idx: int,
    atr: pd.Series,
    active_fvgs: List[FVG],
    min_gap_mult: float = 0.3,
    max_age: int = 100
) -> List[FVG]:
    """
    Detect Fair Value Gaps and manage active FVG list

    Bullish FVG: candle[i-2].low > candle[i].high (gap between them)
    Bearish FVG: candle[i-2].high < candle[i].low

    Args:
        ohlc: OHLC DataFrame
        current_idx: Current bar index
        atr: ATR series
        active_fvgs: Current list of active FVGs
        min_gap_mult: Minimum gap size as ATR multiple
        max_age: Maximum FVG age before expiration

    Returns:
        Updated list of active FVGs
    """
    if current_idx < 3:
        return active_fvgs

    current_bar = ohlc.iloc[current_idx]
    current_atr = atr.iloc[current_idx]

    if pd.isna(current_atr) or current_atr == 0:
        return active_fvgs

    min_gap_size = min_gap_mult * current_atr

    # Check for new bullish FVG
    candle_i = ohlc.iloc[current_idx]
    candle_i_minus_2 = ohlc.iloc[current_idx - 2]

    if candle_i_minus_2['low'] > candle_i['high']:
        gap_size = candle_i_minus_2['low'] - candle_i['high']
        if gap_size >= min_gap_size:
            new_fvg = FVG(
                fvg_type='BULLISH',
                top=candle_i_minus_2['low'],
                bottom=candle_i['high'],
                created_bar=current_idx,
                created_time=current_bar.name,
                age=0,
                mitigated=False
            )
            active_fvgs.append(new_fvg)

    # Check for new bearish FVG
    if candle_i_minus_2['high'] < candle_i['low']:
        gap_size = candle_i['low'] - candle_i_minus_2['high']
        if gap_size >= min_gap_size:
            new_fvg = FVG(
                fvg_type='BEARISH',
                top=candle_i['low'],
                bottom=candle_i_minus_2['high'],
                created_bar=current_idx,
                created_time=current_bar.name,
                age=0,
                mitigated=False
            )
            active_fvgs.append(new_fvg)

    # Update existing FVGs
    updated_fvgs = []
    for fvg in active_fvgs:
        fvg.age += 1

        # Check mitigation (price fills the gap)
        if fvg.fvg_type == 'BULLISH':
            if current_bar['low'] <= fvg.bottom:
                fvg.mitigated = True
        else:  # BEARISH
            if current_bar['high'] >= fvg.top:
                fvg.mitigated = True

        # Keep FVG if not mitigated and not too old
        if not fvg.mitigated and fvg.age < max_age:
            updated_fvgs.append(fvg)

    # Limit number of tracked FVGs
    # Keep most recent ones
    bullish_fvgs = [f for f in updated_fvgs if f.fvg_type == 'BULLISH']
    bearish_fvgs = [f for f in updated_fvgs if f.fvg_type == 'BEARISH']

    bullish_fvgs = sorted(bullish_fvgs, key=lambda x: x.age)[:config.FVG_MAX_TRACKED]
    bearish_fvgs = sorted(bearish_fvgs, key=lambda x: x.age)[:config.FVG_MAX_TRACKED]

    return bullish_fvgs + bearish_fvgs


def classify_premium_discount(
    price: float,
    ohlc_15m: pd.DataFrame,
    current_idx_15m: int,
    window: int = 50,
    threshold: float = 0.1
) -> int:
    """
    Classify if price is in Premium, Equilibrium, or Discount zone

    Args:
        price: Current price
        ohlc_15m: 15M OHLC data
        current_idx_15m: Current 15M index
        window: Swing lookback window
        threshold: Buffer around equilibrium (0.1 = 10%)

    Returns:
        1 (PREMIUM), 0 (EQUILIBRIUM), -1 (DISCOUNT)
    """
    if current_idx_15m < window:
        return 0  # Not enough data

    start_idx = max(0, current_idx_15m - window)
    lookback_window = ohlc_15m.iloc[start_idx:current_idx_15m + 1]

    swing_high = lookback_window['high'].max()
    swing_low = lookback_window['low'].min()

    range_size = swing_high - swing_low
    if range_size == 0:
        return 0

    equilibrium = (swing_high + swing_low) / 2

    # Classify with threshold buffer
    if price > equilibrium + (range_size * threshold):
        return 1  # PREMIUM
    elif price < equilibrium - (range_size * threshold):
        return -1  # DISCOUNT
    else:
        return 0  # EQUILIBRIUM
