"""
ICT Trading System - Market Structure Detector
===============================================
Break of Structure (BOS) and Change of Character (CHOCH) detection
"""

import pandas as pd
import numpy as np
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass
import smartmoneyconcepts as smc


@dataclass
class MarketStructurePoint:
    """Market structure swing point"""
    type: str  # 'swing_high' or 'swing_low'
    index: int
    price: float
    timestamp: Optional[pd.Timestamp] = None


@dataclass
class StructureBreak:
    """Structure break event"""
    type: str  # 'BOS' or 'CHOCH'
    direction: str  # 'bullish' or 'bearish'
    index: int
    break_price: float
    broken_level: float
    strength: float  # 0-100
    timeframe: str
    timestamp: Optional[pd.Timestamp] = None
    description: str = ""


class MarketStructureDetector:
    """Detect market structure: BOS, CHOCH, HH, HL, LH, LL"""

    def __init__(self):
        self.swing_highs = []
        self.swing_lows = []
        self.structure_breaks = []
        self.current_trend = 'neutral'  # 'bullish', 'bearish', 'neutral'

    def analyze_structure(self, df: pd.DataFrame, timeframe: str) -> Dict:
        """Complete market structure analysis"""
        df = df.copy()

        # 1. Detect swing points
        self.detect_swing_points(df)

        # 2. Classify swing points (HH, HL, LH, LL)
        swing_classification = self.classify_swing_points(df)

        # 3. Detect BOS and CHOCH
        self.detect_bos_choch(df, timeframe)

        # 4. Determine current trend
        self.determine_trend(df)

        return {
            'swing_highs': self.swing_highs,
            'swing_lows': self.swing_lows,
            'structure_breaks': self.structure_breaks,
            'current_trend': self.current_trend,
            'swing_classification': swing_classification
        }

    def detect_swing_points(self, df: pd.DataFrame, lookback: int = 5):
        """Detect swing highs and lows using smartmoneyconcepts"""
        self.swing_highs = []
        self.swing_lows = []

        try:
            # Use smartmoneyconcepts for swing detection
            swing_df = smc.swing_highs_lows(df, swing_length=lookback)

            if 'HighLow' in swing_df.columns:
                # Extract swing highs
                highs = swing_df[swing_df['HighLow'] == 1]
                for idx in highs.index:
                    self.swing_highs.append(
                        MarketStructurePoint(
                            type='swing_high',
                            index=idx,
                            price=df.loc[idx, 'high'],
                            timestamp=df.loc[idx].get('timestamp') if 'timestamp' in df.columns else None
                        )
                    )

                # Extract swing lows
                lows = swing_df[swing_df['HighLow'] == -1]
                for idx in lows.index:
                    self.swing_lows.append(
                        MarketStructurePoint(
                            type='swing_low',
                            index=idx,
                            price=df.loc[idx, 'low'],
                            timestamp=df.loc[idx].get('timestamp') if 'timestamp' in df.columns else None
                        )
                    )

        except Exception as e:
            print(f"Warning: smartmoneyconcepts swing detection failed: {e}")
            # Fallback to manual detection
            self._detect_swings_manual(df, lookback)

    def _detect_swings_manual(self, df: pd.DataFrame, lookback: int = 5):
        """Manual swing detection fallback"""
        for i in range(lookback, len(df) - lookback):
            # Swing high
            if df.loc[i, 'high'] == df.loc[i-lookback:i+lookback+1, 'high'].max():
                self.swing_highs.append(
                    MarketStructurePoint(
                        type='swing_high',
                        index=i,
                        price=df.loc[i, 'high'],
                        timestamp=df.loc[i].get('timestamp') if 'timestamp' in df.columns else None
                    )
                )

            # Swing low
            if df.loc[i, 'low'] == df.loc[i-lookback:i+lookback+1, 'low'].min():
                self.swing_lows.append(
                    MarketStructurePoint(
                        type='swing_low',
                        index=i,
                        price=df.loc[i, 'low'],
                        timestamp=df.loc[i].get('timestamp') if 'timestamp' in df.columns else None
                    )
                )

    def classify_swing_points(self, df: pd.DataFrame) -> Dict:
        """Classify swing points as HH, HL, LH, LL"""
        classification = {
            'HH': [],  # Higher High
            'HL': [],  # Higher Low
            'LH': [],  # Lower High
            'LL': []   # Lower Low
        }

        # Classify swing highs
        for i in range(1, len(self.swing_highs)):
            current = self.swing_highs[i]
            previous = self.swing_highs[i - 1]

            if current.price > previous.price:
                classification['HH'].append(current)
            else:
                classification['LH'].append(current)

        # Classify swing lows
        for i in range(1, len(self.swing_lows)):
            current = self.swing_lows[i]
            previous = self.swing_lows[i - 1]

            if current.price > previous.price:
                classification['HL'].append(current)
            else:
                classification['LL'].append(current)

        return classification

    def detect_bos_choch(self, df: pd.DataFrame, timeframe: str):
        """Detect Break of Structure (BOS) and Change of Character (CHOCH)"""
        self.structure_breaks = []

        try:
            # Use smartmoneyconcepts for BOS detection
            bos_df = smc.bos_choch(df, swing_length=5)

            if 'BOS' in bos_df.columns:
                # Bullish BOS
                bullish_bos = bos_df[bos_df['BOS'] == 1]
                for idx in bullish_bos.index:
                    if idx >= 1:
                        broken_level = df.loc[:idx-1, 'high'].max()
                        break_price = df.loc[idx, 'close']
                        strength = min(70 + abs(break_price - broken_level) / broken_level * 100, 95)

                        bos = StructureBreak(
                            type='BOS',
                            direction='bullish',
                            index=idx,
                            break_price=break_price,
                            broken_level=broken_level,
                            strength=strength,
                            timeframe=timeframe,
                            timestamp=df.loc[idx].get('timestamp') if 'timestamp' in df.columns else None,
                            description=f"Bullish BOS: Price broke above {broken_level:.2f}"
                        )
                        self.structure_breaks.append(bos)

                # Bearish BOS
                bearish_bos = bos_df[bos_df['BOS'] == -1]
                for idx in bearish_bos.index:
                    if idx >= 1:
                        broken_level = df.loc[:idx-1, 'low'].min()
                        break_price = df.loc[idx, 'close']
                        strength = min(70 + abs(broken_level - break_price) / broken_level * 100, 95)

                        bos = StructureBreak(
                            type='BOS',
                            direction='bearish',
                            index=idx,
                            break_price=break_price,
                            broken_level=broken_level,
                            strength=strength,
                            timeframe=timeframe,
                            timestamp=df.loc[idx].get('timestamp') if 'timestamp' in df.columns else None,
                            description=f"Bearish BOS: Price broke below {broken_level:.2f}"
                        )
                        self.structure_breaks.append(bos)

            # CHOCH detection
            if 'CHOCH' in bos_df.columns:
                # Bullish CHOCH
                bullish_choch = bos_df[bos_df['CHOCH'] == 1]
                for idx in bullish_choch.index:
                    if idx >= 1:
                        broken_level = df.loc[:idx-1, 'high'].max()
                        break_price = df.loc[idx, 'close']
                        strength = min(75 + abs(break_price - broken_level) / broken_level * 100, 98)

                        choch = StructureBreak(
                            type='CHOCH',
                            direction='bullish',
                            index=idx,
                            break_price=break_price,
                            broken_level=broken_level,
                            strength=strength,
                            timeframe=timeframe,
                            timestamp=df.loc[idx].get('timestamp') if 'timestamp' in df.columns else None,
                            description=f"Bullish CHOCH: Trend reversal at {broken_level:.2f}"
                        )
                        self.structure_breaks.append(choch)

                # Bearish CHOCH
                bearish_choch = bos_df[bos_df['CHOCH'] == -1]
                for idx in bearish_choch.index:
                    if idx >= 1:
                        broken_level = df.loc[:idx-1, 'low'].min()
                        break_price = df.loc[idx, 'close']
                        strength = min(75 + abs(broken_level - break_price) / broken_level * 100, 98)

                        choch = StructureBreak(
                            type='CHOCH',
                            direction='bearish',
                            index=idx,
                            break_price=break_price,
                            broken_level=broken_level,
                            strength=strength,
                            timeframe=timeframe,
                            timestamp=df.loc[idx].get('timestamp') if 'timestamp' in df.columns else None,
                            description=f"Bearish CHOCH: Trend reversal at {broken_level:.2f}"
                        )
                        self.structure_breaks.append(choch)

        except Exception as e:
            print(f"Warning: smartmoneyconcepts BOS/CHOCH detection failed: {e}")
            # Fallback to manual detection
            self._detect_bos_choch_manual(df, timeframe)

    def _detect_bos_choch_manual(self, df: pd.DataFrame, timeframe: str):
        """Manual BOS/CHOCH detection fallback"""
        # Simple implementation: detect when price breaks recent highs/lows

        lookback = 20

        for i in range(lookback, len(df)):
            current_high = df.loc[i, 'high']
            current_low = df.loc[i, 'low']

            # Check for bullish BOS (breaking recent high)
            recent_high = df.loc[i-lookback:i-1, 'high'].max()
            if current_high > recent_high:
                strength = 70
                bos = StructureBreak(
                    type='BOS',
                    direction='bullish',
                    index=i,
                    break_price=current_high,
                    broken_level=recent_high,
                    strength=strength,
                    timeframe=timeframe,
                    description=f"Bullish BOS: Price broke above {recent_high:.2f}"
                )
                self.structure_breaks.append(bos)

            # Check for bearish BOS (breaking recent low)
            recent_low = df.loc[i-lookback:i-1, 'low'].min()
            if current_low < recent_low:
                strength = 70
                bos = StructureBreak(
                    type='BOS',
                    direction='bearish',
                    index=i,
                    break_price=current_low,
                    broken_level=recent_low,
                    strength=strength,
                    timeframe=timeframe,
                    description=f"Bearish BOS: Price broke below {recent_low:.2f}"
                )
                self.structure_breaks.append(bos)

    def determine_trend(self, df: pd.DataFrame):
        """Determine current market trend based on structure"""
        if not self.structure_breaks:
            self.current_trend = 'neutral'
            return

        # Look at recent structure breaks
        recent_breaks = sorted(self.structure_breaks, key=lambda x: x.index)[-5:]

        bullish_count = sum(1 for b in recent_breaks if b.direction == 'bullish')
        bearish_count = sum(1 for b in recent_breaks if b.direction == 'bearish')

        if bullish_count > bearish_count + 1:
            self.current_trend = 'bullish'
        elif bearish_count > bullish_count + 1:
            self.current_trend = 'bearish'
        else:
            self.current_trend = 'neutral'

    def get_structure_features(self, df: pd.DataFrame, current_idx: int) -> Dict:
        """Get market structure features for current position"""
        features = {
            'trend': self.current_trend,
            'recent_bos_count': 0,
            'recent_choch_count': 0,
            'bullish_structure_strength': 0.0,
            'bearish_structure_strength': 0.0,
            'hh_count': 0,
            'hl_count': 0,
            'lh_count': 0,
            'll_count': 0,
            'structure_alignment': 0.0  # -1 to 1
        }

        # Count recent structure breaks
        lookback_bars = 50
        recent_breaks = [b for b in self.structure_breaks
                        if b.index >= current_idx - lookback_bars and b.index <= current_idx]

        features['recent_bos_count'] = sum(1 for b in recent_breaks if b.type == 'BOS')
        features['recent_choch_count'] = sum(1 for b in recent_breaks if b.type == 'CHOCH')

        # Calculate structure strength
        bullish_breaks = [b for b in recent_breaks if b.direction == 'bullish']
        bearish_breaks = [b for b in recent_breaks if b.direction == 'bearish']

        if bullish_breaks:
            features['bullish_structure_strength'] = np.mean([b.strength for b in bullish_breaks])
        if bearish_breaks:
            features['bearish_structure_strength'] = np.mean([b.strength for b in bearish_breaks])

        # Count swing patterns
        classification = self.classify_swing_points(df)
        features['hh_count'] = len(classification['HH'])
        features['hl_count'] = len(classification['HL'])
        features['lh_count'] = len(classification['LH'])
        features['ll_count'] = len(classification['LL'])

        # Structure alignment (-1 bearish, 0 neutral, +1 bullish)
        bullish_signals = features['hh_count'] + features['hl_count'] + len(bullish_breaks)
        bearish_signals = features['lh_count'] + features['ll_count'] + len(bearish_breaks)

        total_signals = bullish_signals + bearish_signals
        if total_signals > 0:
            features['structure_alignment'] = (bullish_signals - bearish_signals) / total_signals
        else:
            features['structure_alignment'] = 0.0

        return features
