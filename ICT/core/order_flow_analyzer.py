"""
ICT Trading System - Order Flow & Liquidity Engineering
========================================================
Smart Money order flow analysis and liquidity engineering detection
"""

import pandas as pd
import numpy as np
from typing import Dict, List, Optional
from dataclasses import dataclass


@dataclass
class OrderFlowSignal:
    """Order flow signal"""
    type: str  # 'absorption', 'exhaustion', 'accumulation', 'distribution'
    direction: str  # 'bullish' or 'bearish'
    strength: float
    index: int
    price: float
    volume: float
    description: str = ""


class OrderFlowAnalyzer:
    """Analyze order flow and liquidity engineering"""

    def __init__(self):
        self.order_flow_signals = []
        self.volume_profile = {}
        self.delta_cumulative = []

    def analyze_order_flow(self, df: pd.DataFrame) -> Dict:
        """Complete order flow analysis"""
        df = df.copy()

        # 1. Volume analysis
        volume_analysis = self.analyze_volume_behavior(df)

        # 2. Detect absorption
        self.detect_volume_absorption(df)

        # 3. Detect exhaustion
        self.detect_exhaustion_moves(df)

        # 4. Accumulation/Distribution
        self.detect_accumulation_distribution(df)

        # 5. Delta analysis
        delta_analysis = self.calculate_delta_analysis(df)

        # 6. Liquidity engineering patterns
        engineering_patterns = self.detect_liquidity_engineering(df)

        return {
            'order_flow_signals': self.order_flow_signals,
            'volume_analysis': volume_analysis,
            'delta_analysis': delta_analysis,
            'engineering_patterns': engineering_patterns
        }

    def analyze_volume_behavior(self, df: pd.DataFrame) -> Dict:
        """Analyze volume patterns and behavior"""
        if 'volume' not in df.columns:
            return self._default_volume_analysis()

        current_volume = df['volume'].iloc[-1]
        avg_volume = df['volume'].tail(20).mean()
        volume_std = df['volume'].tail(20).std()

        # Volume surge detection
        volume_surge = current_volume > (avg_volume + 2 * volume_std)

        # Volume trend
        recent_volumes = df['volume'].tail(10)
        volume_trend = 'increasing' if recent_volumes.is_monotonic_increasing else (
            'decreasing' if recent_volumes.is_monotonic_decreasing else 'mixed'
        )

        # Volume at price levels (simplified)
        price_bins = pd.cut(df['close'].tail(100), bins=20)
        volume_at_price = df['volume'].tail(100).groupby(price_bins).sum()

        # Find high volume nodes
        hvn_threshold = volume_at_price.quantile(0.80)
        high_volume_nodes = volume_at_price[volume_at_price >= hvn_threshold]

        # Find low volume nodes (liquidity voids)
        lvn_threshold = volume_at_price.quantile(0.20)
        low_volume_nodes = volume_at_price[volume_at_price <= lvn_threshold]

        return {
            'current_volume': current_volume,
            'avg_volume': avg_volume,
            'volume_surge': volume_surge,
            'volume_trend': volume_trend,
            'high_volume_node_count': len(high_volume_nodes),
            'low_volume_node_count': len(low_volume_nodes),
            'volume_ratio': current_volume / (avg_volume + 1),
        }

    def detect_volume_absorption(self, df: pd.DataFrame):
        """Detect volume absorption (high volume, little price movement)"""
        if 'volume' not in df.columns:
            return

        for i in range(20, len(df)):
            row = df.iloc[i]

            # High volume
            avg_volume = df['volume'].iloc[i-20:i].mean()
            if row['volume'] < avg_volume * 1.5:
                continue

            # But small price movement
            price_range = row['high'] - row['low']
            avg_range = (df['high'] - df['low']).iloc[i-20:i].mean()

            if price_range < avg_range * 0.7:  # Smaller than average range
                # This is absorption
                direction = 'bullish' if row['close'] > row['open'] else 'bearish'

                signal = OrderFlowSignal(
                    type='absorption',
                    direction=direction,
                    strength=75,
                    index=i,
                    price=row['close'],
                    volume=row['volume'],
                    description=f"Volume absorption at {row['close']:.2f}"
                )
                self.order_flow_signals.append(signal)

    def detect_exhaustion_moves(self, df: pd.DataFrame):
        """Detect exhaustion (large price move on increasing volume)"""
        if 'volume' not in df.columns:
            return

        for i in range(5, len(df)):
            row = df.iloc[i]
            prev = df.iloc[i-1]

            # Large price movement
            price_change = abs(row['close'] - prev['close']) / prev['close']

            if price_change < 0.015:  # Less than 1.5% move
                continue

            # On high volume
            avg_volume = df['volume'].iloc[i-20:i].mean()
            if row['volume'] < avg_volume * 1.3:
                continue

            # Check if this is a climactic move
            recent_volumes = df['volume'].iloc[i-5:i+1]
            if row['volume'] == recent_volumes.max():
                # Potential exhaustion
                direction = 'bearish' if row['close'] > prev['close'] else 'bullish'

                signal = OrderFlowSignal(
                    type='exhaustion',
                    direction=direction,
                    strength=70,
                    index=i,
                    price=row['close'],
                    volume=row['volume'],
                    description=f"Exhaustion move at {row['close']:.2f}"
                )
                self.order_flow_signals.append(signal)

    def detect_accumulation_distribution(self, df: pd.DataFrame):
        """Detect accumulation and distribution phases"""
        if 'volume' not in df.columns or len(df) < 30:
            return

        # Use Money Flow Index or simplified A/D logic
        for i in range(20, len(df), 5):  # Check every 5 bars
            window = df.iloc[i-20:i+1]

            # Price range
            price_range = window['high'].max() - window['low'].min()
            if price_range == 0:
                continue

            # Price movement
            price_change = window['close'].iloc[-1] - window['close'].iloc[0]

            # Volume characteristics
            avg_volume = window['volume'].mean()
            volume_increasing = window['volume'].tail(5).mean() > window['volume'].head(5).mean()

            # Accumulation: Price stable/rising, volume increasing
            if price_change >= 0 and price_change < price_range * 0.3 and volume_increasing:
                signal = OrderFlowSignal(
                    type='accumulation',
                    direction='bullish',
                    strength=65,
                    index=i,
                    price=window['close'].iloc[-1],
                    volume=avg_volume,
                    description=f"Accumulation phase at {window['close'].iloc[-1]:.2f}"
                )
                self.order_flow_signals.append(signal)

            # Distribution: Price stable/falling, volume increasing
            elif price_change <= 0 and abs(price_change) < price_range * 0.3 and volume_increasing:
                signal = OrderFlowSignal(
                    type='distribution',
                    direction='bearish',
                    strength=65,
                    index=i,
                    price=window['close'].iloc[-1],
                    volume=avg_volume,
                    description=f"Distribution phase at {window['close'].iloc[-1]:.2f}"
                )
                self.order_flow_signals.append(signal)

    def calculate_delta_analysis(self, df: pd.DataFrame) -> Dict:
        """Calculate buy/sell delta (simplified without tick data)"""
        if 'volume' not in df.columns:
            return {'delta': 0, 'cumulative_delta': 0, 'delta_divergence': False}

        # Simplified delta: estimate based on close vs open
        df = df.copy()
        df['delta'] = 0.0

        for i in range(len(df)):
            row = df.iloc[i]

            # If close > open, assume more buying
            if row['close'] > row['open']:
                df.loc[df.index[i], 'delta'] = row['volume'] * 0.6  # 60% buy
            elif row['close'] < row['open']:
                df.loc[df.index[i], 'delta'] = -row['volume'] * 0.6  # 60% sell
            else:
                df.loc[df.index[i], 'delta'] = 0

        # Cumulative delta
        df['cumulative_delta'] = df['delta'].cumsum()
        self.delta_cumulative = df['cumulative_delta'].tolist()

        # Delta divergence
        recent_price = df['close'].tail(20)
        recent_delta = df['cumulative_delta'].tail(20)

        # Bullish divergence: price down, delta up
        price_falling = recent_price.iloc[-1] < recent_price.iloc[0]
        delta_rising = recent_delta.iloc[-1] > recent_delta.iloc[0]
        bullish_divergence = price_falling and delta_rising

        # Bearish divergence: price up, delta down
        price_rising = recent_price.iloc[-1] > recent_price.iloc[0]
        delta_falling = recent_delta.iloc[-1] < recent_delta.iloc[0]
        bearish_divergence = price_rising and delta_falling

        return {
            'delta': df['delta'].iloc[-1],
            'cumulative_delta': df['cumulative_delta'].iloc[-1],
            'delta_divergence': bullish_divergence or bearish_divergence,
            'bullish_divergence': bullish_divergence,
            'bearish_divergence': bearish_divergence,
        }

    def detect_liquidity_engineering(self, df: pd.DataFrame) -> Dict:
        """Detect liquidity engineering patterns (stop hunts, fake moves)"""
        patterns = {
            'stop_hunts': [],
            'fake_breakouts': [],
            'liquidity_grabs': []
        }

        # 1. Stop hunts (spike through level then reverse)
        for i in range(20, len(df) - 1):
            current = df.iloc[i]
            next_bar = df.iloc[i + 1]

            # Recent high
            recent_high = df['high'].iloc[i-20:i].max()

            # Spike above then close back below
            if current['high'] > recent_high * 1.001 and current['close'] < recent_high:
                patterns['stop_hunts'].append({
                    'index': i,
                    'type': 'buy_side_hunt',
                    'level': recent_high,
                    'spike_price': current['high']
                })

            # Recent low
            recent_low = df['low'].iloc[i-20:i].min()

            # Spike below then close back above
            if current['low'] < recent_low * 0.999 and current['close'] > recent_low:
                patterns['stop_hunts'].append({
                    'index': i,
                    'type': 'sell_side_hunt',
                    'level': recent_low,
                    'spike_price': current['low']
                })

        # 2. Fake breakouts
        for i in range(30, len(df) - 5):
            # Breakout above resistance
            resistance = df['high'].iloc[i-30:i-10].max()

            if df['high'].iloc[i] > resistance * 1.003:
                # Check if it failed
                failed = df['close'].iloc[i:i+5].min() < resistance

                if failed:
                    patterns['fake_breakouts'].append({
                        'index': i,
                        'type': 'failed_bullish_breakout',
                        'level': resistance
                    })

            # Breakout below support
            support = df['low'].iloc[i-30:i-10].min()

            if df['low'].iloc[i] < support * 0.997:
                # Check if it failed
                failed = df['close'].iloc[i:i+5].max() > support

                if failed:
                    patterns['fake_breakouts'].append({
                        'index': i,
                        'type': 'failed_bearish_breakout',
                        'level': support
                    })

        return patterns

    def get_order_flow_features(self, df: pd.DataFrame, current_idx: int) -> np.ndarray:
        """Get order flow features for ML"""
        if current_idx < 20:
            return np.zeros(10)

        volume_analysis = self.analyze_volume_behavior(df.iloc[:current_idx+1])
        delta_analysis = self.calculate_delta_analysis(df.iloc[:current_idx+1])

        # Recent order flow signals
        recent_signals = [s for s in self.order_flow_signals
                         if s.index >= current_idx - 20 and s.index <= current_idx]

        absorption_count = sum(1 for s in recent_signals if s.type == 'absorption')
        exhaustion_count = sum(1 for s in recent_signals if s.type == 'exhaustion')
        accumulation_count = sum(1 for s in recent_signals if s.type == 'accumulation')
        distribution_count = sum(1 for s in recent_signals if s.type == 'distribution')

        features = np.array([
            volume_analysis['volume_ratio'],
            1.0 if volume_analysis['volume_surge'] else 0.0,
            1.0 if volume_analysis['volume_trend'] == 'increasing' else 0.0,
            volume_analysis['high_volume_node_count'] / 10.0,
            delta_analysis['delta'] / (volume_analysis['avg_volume'] + 1),
            delta_analysis['cumulative_delta'] / 10000.0,
            1.0 if delta_analysis['bullish_divergence'] else 0.0,
            absorption_count / 5.0,
            accumulation_count / 5.0,
            distribution_count / 5.0,
        ])

        return features

    def _default_volume_analysis(self) -> Dict:
        """Default volume analysis when volume not available"""
        return {
            'current_volume': 0,
            'avg_volume': 0,
            'volume_surge': False,
            'volume_trend': 'mixed',
            'high_volume_node_count': 0,
            'low_volume_node_count': 0,
            'volume_ratio': 1.0,
        }
