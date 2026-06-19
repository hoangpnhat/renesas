"""
ICT Trading System - Enhanced PD Array Detector
================================================
Advanced PD Array detection using smartmoneyconcepts package
"""

import pandas as pd
import numpy as np
from typing import Dict, List, Optional
from dataclasses import dataclass
import smartmoneyconcepts as smc


@dataclass
class EnhancedPDArray:
    """Enhanced Premium/Discount Array with protection and confidence"""
    type: str  # 'OB', 'FVG', 'Breaker', 'EQH', 'EQL', 'BISI', 'SIBI'
    direction: str  # 'bullish' or 'bearish'
    timeframe: str
    top: float
    bottom: float
    index: int
    start_time: Optional[pd.Timestamp] = None
    end_time: Optional[pd.Timestamp] = None
    volume: float = 0.0
    strength: float = 0.0  # 0-100
    pd_status: str = 'equilibrium'  # 'premium', 'discount', 'equilibrium'
    mitigated: bool = False
    protected: bool = False
    protection_score: float = 0.0  # 0-100
    confluence_count: int = 0
    distance_from_price: float = 0.0

    def to_dict(self):
        return {
            'type': self.type,
            'direction': self.direction,
            'timeframe': self.timeframe,
            'top': self.top,
            'bottom': self.bottom,
            'index': self.index,
            'strength': self.strength,
            'pd_status': self.pd_status,
            'protected': self.protected,
            'protection_score': self.protection_score,
            'confluence_count': self.confluence_count,
            'distance_from_price': self.distance_from_price
        }


class EnhancedPDDetector:
    """Enhanced PD Array detector using smartmoneyconcepts + custom logic"""

    def __init__(self):
        self.pd_arrays_cache = {}

    def detect_all_pd_arrays(self, df: pd.DataFrame, timeframe: str) -> List[EnhancedPDArray]:
        """Detect all PD arrays using smartmoneyconcepts"""
        df = df.copy()
        all_arrays = []

        # 1. Order Blocks
        obs = self.detect_order_blocks(df, timeframe)
        all_arrays.extend(obs)

        # 2. Fair Value Gaps
        fvgs = self.detect_fair_value_gaps(df, timeframe)
        all_arrays.extend(fvgs)

        # 3. Breaker Blocks
        breakers = self.detect_breaker_blocks(df, timeframe)
        all_arrays.extend(breakers)

        # 4. Equal Highs/Lows
        eqh_eql = self.detect_equal_highs_lows(df, timeframe)
        all_arrays.extend(eqh_eql)

        # 5. Buy/Sell Side Imbalance (BISI/SIBI)
        imbalances = self.detect_imbalances(df, timeframe)
        all_arrays.extend(imbalances)

        return all_arrays

    def detect_order_blocks(self, df: pd.DataFrame, timeframe: str) -> List[EnhancedPDArray]:
        """Detect Order Blocks using smartmoneyconcepts"""
        obs = []

        try:
            # Use smartmoneyconcepts to detect OBs
            ob_df = smc.ob(df, close_mitigation=False)

            # Extract bullish OBs
            if 'OB' in ob_df.columns:
                bullish_obs = ob_df[ob_df['OB'] == 1]
                for idx in bullish_obs.index:
                    row = df.loc[idx]

                    # Calculate strength based on impulse
                    impulse = self._calculate_impulse(df, idx, 'bullish')
                    strength = min(50 + impulse * 100, 95)

                    # Determine PD status
                    pd_status = self._calculate_pd_status(df, idx, row['low'])

                    ob = EnhancedPDArray(
                        type='OB',
                        direction='bullish',
                        timeframe=timeframe,
                        top=row['high'],
                        bottom=row['low'],
                        index=idx,
                        start_time=row.get('timestamp') if 'timestamp' in df.columns else None,
                        volume=row.get('volume', 0),
                        strength=strength,
                        pd_status=pd_status
                    )
                    obs.append(ob)

                # Extract bearish OBs
                bearish_obs = ob_df[ob_df['OB'] == -1]
                for idx in bearish_obs.index:
                    row = df.loc[idx]

                    impulse = self._calculate_impulse(df, idx, 'bearish')
                    strength = min(50 + impulse * 100, 95)
                    pd_status = self._calculate_pd_status(df, idx, row['high'])

                    ob = EnhancedPDArray(
                        type='OB',
                        direction='bearish',
                        timeframe=timeframe,
                        top=row['high'],
                        bottom=row['low'],
                        index=idx,
                        start_time=row.get('timestamp') if 'timestamp' in df.columns else None,
                        volume=row.get('volume', 0),
                        strength=strength,
                        pd_status=pd_status
                    )
                    obs.append(ob)

        except Exception as e:
            print(f"Warning: smartmoneyconcepts OB detection failed: {e}")
            # Fallback to manual detection
            obs = self._detect_order_blocks_manual(df, timeframe)

        return obs

    def detect_fair_value_gaps(self, df: pd.DataFrame, timeframe: str) -> List[EnhancedPDArray]:
        """Detect Fair Value Gaps using smartmoneyconcepts"""
        fvgs = []

        try:
            # Use smartmoneyconcepts to detect FVGs
            fvg_df = smc.fvg(df)

            # Extract bullish FVGs
            if 'FVG' in fvg_df.columns:
                bullish_fvgs = fvg_df[fvg_df['FVG'] == 1]
                for idx in bullish_fvgs.index:
                    if idx < 2:
                        continue

                    # Get the gap boundaries
                    bottom = df.loc[idx - 2, 'high']
                    top = df.loc[idx, 'low']

                    if top > bottom:
                        gap_size = (top - bottom) / df.loc[idx, 'close']
                        strength = min(60 + gap_size * 500, 95)

                        fvg = EnhancedPDArray(
                            type='FVG',
                            direction='bullish',
                            timeframe=timeframe,
                            top=top,
                            bottom=bottom,
                            index=idx,
                            start_time=df.loc[idx - 2].get('timestamp') if 'timestamp' in df.columns else None,
                            strength=strength,
                            pd_status='discount'
                        )
                        fvgs.append(fvg)

                # Extract bearish FVGs
                bearish_fvgs = fvg_df[fvg_df['FVG'] == -1]
                for idx in bearish_fvgs.index:
                    if idx < 2:
                        continue

                    top = df.loc[idx - 2, 'low']
                    bottom = df.loc[idx, 'high']

                    if top > bottom:
                        gap_size = (top - bottom) / df.loc[idx, 'close']
                        strength = min(60 + gap_size * 500, 95)

                        fvg = EnhancedPDArray(
                            type='FVG',
                            direction='bearish',
                            timeframe=timeframe,
                            top=top,
                            bottom=bottom,
                            index=idx,
                            start_time=df.loc[idx - 2].get('timestamp') if 'timestamp' in df.columns else None,
                            strength=strength,
                            pd_status='premium'
                        )
                        fvgs.append(fvg)

        except Exception as e:
            print(f"Warning: smartmoneyconcepts FVG detection failed: {e}")
            fvgs = self._detect_fvgs_manual(df, timeframe)

        return fvgs

    def detect_breaker_blocks(self, df: pd.DataFrame, timeframe: str) -> List[EnhancedPDArray]:
        """Detect Breaker Blocks (mitigated OBs that break structure)"""
        breakers = []

        # First detect OBs
        obs = self.detect_order_blocks(df, timeframe)

        # Check which OBs have been broken and turned into breakers
        for ob in obs:
            ob_idx = ob.index

            # Check if OB was breached in opposite direction
            if ob.direction == 'bullish':
                # Check if price broke below the bullish OB
                future_lows = df.loc[ob_idx + 1:, 'low']
                if len(future_lows) > 0 and future_lows.min() < ob.bottom:
                    breach_idx = future_lows.idxmin()

                    # Now it's a bearish breaker
                    breaker = EnhancedPDArray(
                        type='Breaker',
                        direction='bearish',
                        timeframe=timeframe,
                        top=ob.top,
                        bottom=ob.bottom,
                        index=breach_idx,
                        strength=ob.strength * 1.2,  # Breakers are stronger
                        pd_status='premium'
                    )
                    breakers.append(breaker)

            else:  # bearish OB
                # Check if price broke above the bearish OB
                future_highs = df.loc[ob_idx + 1:, 'high']
                if len(future_highs) > 0 and future_highs.max() > ob.top:
                    breach_idx = future_highs.idxmax()

                    # Now it's a bullish breaker
                    breaker = EnhancedPDArray(
                        type='Breaker',
                        direction='bullish',
                        timeframe=timeframe,
                        top=ob.top,
                        bottom=ob.bottom,
                        index=breach_idx,
                        strength=ob.strength * 1.2,
                        pd_status='discount'
                    )
                    breakers.append(breaker)

        return breakers

    def detect_equal_highs_lows(self, df: pd.DataFrame, timeframe: str) -> List[EnhancedPDArray]:
        """Detect Equal Highs (EQH) and Equal Lows (EQL) - Liquidity pools"""
        eqs = []

        try:
            # Use smartmoneyconcepts swing detection
            swing_df = smc.swing_highs_lows(df)

            # Find equal highs
            if 'HighLow' in swing_df.columns:
                highs = swing_df[swing_df['HighLow'] == 1]
                high_indices = highs.index.tolist()

                for i in range(len(high_indices) - 1):
                    for j in range(i + 1, min(i + 5, len(high_indices))):
                        idx1, idx2 = high_indices[i], high_indices[j]
                        price1 = df.loc[idx1, 'high']
                        price2 = df.loc[idx2, 'high']

                        # Check if prices are equal (within 0.1% tolerance)
                        if abs(price1 - price2) / price1 < 0.001:
                            eqh = EnhancedPDArray(
                                type='EQH',
                                direction='bearish',  # EQH is bearish liquidity
                                timeframe=timeframe,
                                top=max(price1, price2) * 1.0005,
                                bottom=min(price1, price2) * 0.9995,
                                index=idx2,
                                strength=70,
                                pd_status='premium'
                            )
                            eqs.append(eqh)

                # Find equal lows
                lows = swing_df[swing_df['HighLow'] == -1]
                low_indices = lows.index.tolist()

                for i in range(len(low_indices) - 1):
                    for j in range(i + 1, min(i + 5, len(low_indices))):
                        idx1, idx2 = low_indices[i], low_indices[j]
                        price1 = df.loc[idx1, 'low']
                        price2 = df.loc[idx2, 'low']

                        # Check if prices are equal (within 0.1% tolerance)
                        if abs(price1 - price2) / price1 < 0.001:
                            eql = EnhancedPDArray(
                                type='EQL',
                                direction='bullish',  # EQL is bullish liquidity
                                timeframe=timeframe,
                                top=max(price1, price2) * 1.0005,
                                bottom=min(price1, price2) * 0.9995,
                                index=idx2,
                                strength=70,
                                pd_status='discount'
                            )
                            eqs.append(eql)

        except Exception as e:
            print(f"Warning: EQH/EQL detection failed: {e}")

        return eqs

    def detect_imbalances(self, df: pd.DataFrame, timeframe: str) -> List[EnhancedPDArray]:
        """Detect Buy Side Imbalance (BISI) and Sell Side Imbalance (SIBI)"""
        imbalances = []

        for i in range(3, len(df)):
            # BISI: Strong buying pressure leaving gap below
            if df.loc[i, 'close'] > df.loc[i, 'open']:
                body_size = df.loc[i, 'close'] - df.loc[i, 'open']
                lower_wick = df.loc[i, 'open'] - df.loc[i, 'low']

                # Strong bullish candle with small lower wick
                if body_size > 0 and lower_wick < body_size * 0.3:
                    prev_low = df.loc[i - 1, 'low']
                    current_open = df.loc[i, 'open']

                    if current_open > prev_low:
                        imb = EnhancedPDArray(
                            type='BISI',
                            direction='bullish',
                            timeframe=timeframe,
                            top=current_open,
                            bottom=prev_low,
                            index=i,
                            strength=65,
                            pd_status='discount'
                        )
                        imbalances.append(imb)

            # SIBI: Strong selling pressure leaving gap above
            elif df.loc[i, 'close'] < df.loc[i, 'open']:
                body_size = df.loc[i, 'open'] - df.loc[i, 'close']
                upper_wick = df.loc[i, 'high'] - df.loc[i, 'open']

                # Strong bearish candle with small upper wick
                if body_size > 0 and upper_wick < body_size * 0.3:
                    prev_high = df.loc[i - 1, 'high']
                    current_open = df.loc[i, 'open']

                    if current_open < prev_high:
                        imb = EnhancedPDArray(
                            type='SIBI',
                            direction='bearish',
                            timeframe=timeframe,
                            top=prev_high,
                            bottom=current_open,
                            index=i,
                            strength=65,
                            pd_status='premium'
                        )
                        imbalances.append(imb)

        return imbalances

    def _calculate_impulse(self, df: pd.DataFrame, idx: int, direction: str) -> float:
        """Calculate impulse strength"""
        if idx >= len(df) - 1:
            return 0.0

        current = df.loc[idx, 'close']
        next_close = df.loc[idx + 1, 'close']

        if direction == 'bullish':
            return max(0, (next_close - current) / current)
        else:
            return max(0, (current - next_close) / current)

    def _calculate_pd_status(self, df: pd.DataFrame, idx: int, price: float) -> str:
        """Calculate premium/discount status"""
        lookback = min(50, idx)
        if lookback < 2:
            return 'equilibrium'

        recent = df.loc[max(0, idx - lookback):idx]
        high = recent['high'].max()
        low = recent['low'].min()

        if high == low:
            return 'equilibrium'

        position = (price - low) / (high - low)

        if position < 0.4:
            return 'discount'
        elif position > 0.6:
            return 'premium'
        else:
            return 'equilibrium'

    def _detect_order_blocks_manual(self, df: pd.DataFrame, timeframe: str) -> List[EnhancedPDArray]:
        """Manual OB detection fallback"""
        obs = []

        for i in range(10, len(df) - 1):
            current = df.loc[i]
            next_candle = df.loc[i + 1]

            # Bullish OB
            if current['close'] < current['open']:
                impulse = (next_candle['close'] - current['close']) / current['close']
                if impulse > 0.005:
                    strength = min(50 + impulse * 100, 95)
                    pd_status = self._calculate_pd_status(df, i, current['low'])

                    ob = EnhancedPDArray(
                        type='OB',
                        direction='bullish',
                        timeframe=timeframe,
                        top=current['high'],
                        bottom=current['low'],
                        index=i,
                        volume=current.get('volume', 0),
                        strength=strength,
                        pd_status=pd_status
                    )
                    obs.append(ob)

            # Bearish OB
            if current['close'] > current['open']:
                impulse = (current['close'] - next_candle['close']) / current['close']
                if impulse > 0.005:
                    strength = min(50 + impulse * 100, 95)
                    pd_status = self._calculate_pd_status(df, i, current['high'])

                    ob = EnhancedPDArray(
                        type='OB',
                        direction='bearish',
                        timeframe=timeframe,
                        top=current['high'],
                        bottom=current['low'],
                        index=i,
                        volume=current.get('volume', 0),
                        strength=strength,
                        pd_status=pd_status
                    )
                    obs.append(ob)

        return obs

    def _detect_fvgs_manual(self, df: pd.DataFrame, timeframe: str) -> List[EnhancedPDArray]:
        """Manual FVG detection fallback"""
        fvgs = []

        for i in range(2, len(df)):
            c1, c2, c3 = df.loc[i-2], df.loc[i-1], df.loc[i]

            # Bullish FVG
            if c3['low'] > c1['high']:
                gap_size = (c3['low'] - c1['high']) / c2['close']
                if gap_size > 0.001:
                    strength = min(60 + gap_size * 500, 95)

                    fvg = EnhancedPDArray(
                        type='FVG',
                        direction='bullish',
                        timeframe=timeframe,
                        top=c3['low'],
                        bottom=c1['high'],
                        index=i,
                        strength=strength,
                        pd_status='discount'
                    )
                    fvgs.append(fvg)

            # Bearish FVG
            if c3['high'] < c1['low']:
                gap_size = (c1['low'] - c3['high']) / c2['close']
                if gap_size > 0.001:
                    strength = min(60 + gap_size * 500, 95)

                    fvg = EnhancedPDArray(
                        type='FVG',
                        direction='bearish',
                        timeframe=timeframe,
                        top=c1['low'],
                        bottom=c3['high'],
                        index=i,
                        strength=strength,
                        pd_status='premium'
                    )
                    fvgs.append(fvg)

        return fvgs
