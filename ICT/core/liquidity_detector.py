"""
ICT Trading System - Liquidity Zones and Vectors
=================================================
Liquidity pool detection, liquidity grab detection, and vector analysis
"""

import pandas as pd
import numpy as np
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass


@dataclass
class LiquidityZone:
    """Liquidity accumulation zone"""
    type: str  # 'buy_side', 'sell_side', 'internal'
    price_level: float
    strength: float  # 0-100
    volume: float
    index: int
    timeframe: str
    swept: bool = False  # Has liquidity been taken?
    sweep_index: Optional[int] = None
    description: str = ""


@dataclass
class LiquidityVector:
    """Liquidity attraction/repulsion vector"""
    source_price: float
    target_price: float
    vector_type: str  # 'attract' or 'repel'
    strength: float  # 0-100
    distance: float
    timeframe: str
    description: str = ""


class LiquidityDetector:
    """Detect liquidity zones, sweeps, and calculate liquidity vectors"""

    def __init__(self):
        self.liquidity_zones = []
        self.swept_liquidity = []
        self.liquidity_vectors = []

    def detect_all_liquidity(self, df: pd.DataFrame, timeframe: str) -> Dict:
        """Complete liquidity analysis"""
        df = df.copy()

        # 1. Detect liquidity zones
        self.detect_liquidity_zones(df, timeframe)

        # 2. Detect liquidity sweeps
        self.detect_liquidity_sweeps(df)

        # 3. Calculate liquidity vectors
        current_price = df['close'].iloc[-1]
        self.calculate_liquidity_vectors(current_price, timeframe)

        # 4. Analyze liquidity profile
        liquidity_profile = self.analyze_liquidity_profile(df, current_price)

        return {
            'liquidity_zones': self.liquidity_zones,
            'swept_liquidity': self.swept_liquidity,
            'liquidity_vectors': self.liquidity_vectors,
            'liquidity_profile': liquidity_profile
        }

    def detect_liquidity_zones(self, df: pd.DataFrame, timeframe: str):
        """Detect buy-side and sell-side liquidity zones"""
        self.liquidity_zones = []

        # 1. Swing highs and lows (major liquidity)
        self._detect_swing_liquidity(df, timeframe)

        # 2. Equal highs/lows (liquidity pools)
        self._detect_equal_level_liquidity(df, timeframe)

        # 3. Round number liquidity
        self._detect_round_number_liquidity(df, timeframe)

        # 4. Volume-based liquidity
        self._detect_volume_liquidity(df, timeframe)

    def _detect_swing_liquidity(self, df: pd.DataFrame, timeframe: str, lookback: int = 10):
        """Detect liquidity at swing highs and lows"""
        for i in range(lookback, len(df) - lookback):
            # Buy-side liquidity (above swing highs)
            if df['high'].iloc[i] == df['high'].iloc[i-lookback:i+lookback+1].max():
                # There's liquidity above this swing high (stop losses of shorts)
                zone = LiquidityZone(
                    type='buy_side',
                    price_level=df['high'].iloc[i],
                    strength=70,
                    volume=df['volume'].iloc[i] if 'volume' in df.columns else 0,
                    index=i,
                    timeframe=timeframe,
                    description=f"Buy-side liquidity at swing high {df['high'].iloc[i]:.2f}"
                )
                self.liquidity_zones.append(zone)

            # Sell-side liquidity (below swing lows)
            if df['low'].iloc[i] == df['low'].iloc[i-lookback:i+lookback+1].min():
                # There's liquidity below this swing low (stop losses of longs)
                zone = LiquidityZone(
                    type='sell_side',
                    price_level=df['low'].iloc[i],
                    strength=70,
                    volume=df['volume'].iloc[i] if 'volume' in df.columns else 0,
                    index=i,
                    timeframe=timeframe,
                    description=f"Sell-side liquidity at swing low {df['low'].iloc[i]:.2f}"
                )
                self.liquidity_zones.append(zone)

    def _detect_equal_level_liquidity(self, df: pd.DataFrame, timeframe: str):
        """Detect liquidity at equal highs/lows (EQH/EQL)"""
        tolerance = 0.002  # 0.2% tolerance for "equal"

        # Find equal highs
        highs = []
        for i in range(10, len(df) - 10):
            if df['high'].iloc[i] >= df['high'].iloc[i-10:i+10+1].quantile(0.95):
                highs.append((i, df['high'].iloc[i]))

        # Group equal highs
        for i in range(len(highs) - 1):
            for j in range(i + 1, min(i + 5, len(highs))):
                idx1, price1 = highs[i]
                idx2, price2 = highs[j]

                if abs(price1 - price2) / price1 < tolerance:
                    # Equal highs found - strong buy-side liquidity
                    zone = LiquidityZone(
                        type='buy_side',
                        price_level=(price1 + price2) / 2,
                        strength=85,  # Higher strength for equal levels
                        volume=0,
                        index=idx2,
                        timeframe=timeframe,
                        description=f"EQH liquidity pool at {(price1 + price2) / 2:.2f}"
                    )
                    self.liquidity_zones.append(zone)

        # Find equal lows
        lows = []
        for i in range(10, len(df) - 10):
            if df['low'].iloc[i] <= df['low'].iloc[i-10:i+10+1].quantile(0.05):
                lows.append((i, df['low'].iloc[i]))

        # Group equal lows
        for i in range(len(lows) - 1):
            for j in range(i + 1, min(i + 5, len(lows))):
                idx1, price1 = lows[i]
                idx2, price2 = lows[j]

                if abs(price1 - price2) / price1 < tolerance:
                    # Equal lows found - strong sell-side liquidity
                    zone = LiquidityZone(
                        type='sell_side',
                        price_level=(price1 + price2) / 2,
                        strength=85,
                        volume=0,
                        index=idx2,
                        timeframe=timeframe,
                        description=f"EQL liquidity pool at {(price1 + price2) / 2:.2f}"
                    )
                    self.liquidity_zones.append(zone)

    def _detect_round_number_liquidity(self, df: pd.DataFrame, timeframe: str):
        """Detect liquidity at psychological round numbers"""
        current_price = df['close'].iloc[-1]

        # Determine round number levels based on price magnitude
        if current_price > 10000:
            step = 1000  # e.g., 50000, 51000, 52000
        elif current_price > 1000:
            step = 100
        elif current_price > 100:
            step = 10
        else:
            step = 1

        # Find nearby round numbers
        lower_bound = current_price * 0.95
        upper_bound = current_price * 1.05

        round_num = int(lower_bound / step) * step
        while round_num <= upper_bound:
            # Check if price has interacted with this level recently
            recent_df = df.tail(100)
            touched = ((recent_df['low'] <= round_num) & (recent_df['high'] >= round_num)).any()

            if touched:
                # Liquidity exists at this round number
                zone = LiquidityZone(
                    type='internal',
                    price_level=round_num,
                    strength=60,
                    volume=0,
                    index=len(df) - 1,
                    timeframe=timeframe,
                    description=f"Round number liquidity at {round_num:.0f}"
                )
                self.liquidity_zones.append(zone)

            round_num += step

    def _detect_volume_liquidity(self, df: pd.DataFrame, timeframe: str):
        """Detect liquidity at high volume nodes"""
        if 'volume' not in df.columns:
            return

        # Find high volume candles
        volume_threshold = df['volume'].quantile(0.90)
        high_volume_bars = df[df['volume'] >= volume_threshold]

        for idx in high_volume_bars.index[-20:]:  # Last 20 high volume bars
            row = df.loc[idx]

            # High volume suggests liquidity absorption
            zone = LiquidityZone(
                type='internal',
                price_level=(row['high'] + row['low']) / 2,
                strength=50 + (row['volume'] / df['volume'].max()) * 30,
                volume=row['volume'],
                index=idx,
                timeframe=timeframe,
                description=f"High volume liquidity at {(row['high'] + row['low']) / 2:.2f}"
            )
            self.liquidity_zones.append(zone)

    def detect_liquidity_sweeps(self, df: pd.DataFrame):
        """Detect when liquidity zones have been swept/grabbed"""
        self.swept_liquidity = []

        for zone in self.liquidity_zones:
            if zone.swept:
                continue

            # Check if price swept this liquidity after the zone was formed
            future_df = df.iloc[zone.index + 1:]

            if zone.type == 'buy_side':
                # Check if high exceeded the level
                sweep_mask = future_df['high'] >= zone.price_level
                if sweep_mask.any():
                    sweep_idx = future_df[sweep_mask].index[0]
                    zone.swept = True
                    zone.sweep_index = sweep_idx
                    self.swept_liquidity.append(zone)

            elif zone.type == 'sell_side':
                # Check if low went below the level
                sweep_mask = future_df['low'] <= zone.price_level
                if sweep_mask.any():
                    sweep_idx = future_df[sweep_mask].index[0]
                    zone.swept = True
                    zone.sweep_index = sweep_idx
                    self.swept_liquidity.append(zone)

    def calculate_liquidity_vectors(self, current_price: float, timeframe: str):
        """Calculate attraction/repulsion vectors from current price to liquidity zones"""
        self.liquidity_vectors = []

        # Active (unswept) liquidity zones
        active_zones = [z for z in self.liquidity_zones if not z.swept]

        for zone in active_zones:
            distance = abs(zone.price_level - current_price)
            distance_pct = distance / current_price

            # Nearby liquidity attracts price more
            # Distance decay: strength decreases with distance
            distance_factor = np.exp(-distance_pct * 20)  # Exponential decay
            vector_strength = zone.strength * distance_factor

            # Determine vector type
            if zone.type == 'buy_side':
                # Buy-side liquidity above price attracts upward
                vector_type = 'attract' if zone.price_level > current_price else 'repel'
            elif zone.type == 'sell_side':
                # Sell-side liquidity below price attracts downward
                vector_type = 'attract' if zone.price_level < current_price else 'repel'
            else:
                # Internal liquidity always attracts
                vector_type = 'attract'

            vector = LiquidityVector(
                source_price=current_price,
                target_price=zone.price_level,
                vector_type=vector_type,
                strength=vector_strength,
                distance=distance,
                timeframe=timeframe,
                description=f"{vector_type.capitalize()} to {zone.type} liq at {zone.price_level:.2f}"
            )
            self.liquidity_vectors.append(vector)

    def analyze_liquidity_profile(self, df: pd.DataFrame, current_price: float) -> Dict:
        """Analyze overall liquidity profile"""
        active_zones = [z for z in self.liquidity_zones if not z.swept]

        # Separate by direction
        buy_side = [z for z in active_zones if z.type == 'buy_side']
        sell_side = [z for z in active_zones if z.type == 'sell_side']

        # Find nearest liquidity in each direction
        above_price = [z for z in active_zones if z.price_level > current_price]
        below_price = [z for z in active_zones if z.price_level < current_price]

        nearest_above = min(above_price, key=lambda z: z.price_level - current_price) if above_price else None
        nearest_below = max(below_price, key=lambda z: current_price - z.price_level) if below_price else None

        # Calculate liquidity imbalance
        buy_side_strength = sum(z.strength for z in buy_side)
        sell_side_strength = sum(z.strength for z in sell_side)

        total_strength = buy_side_strength + sell_side_strength
        imbalance = 0
        if total_strength > 0:
            imbalance = (buy_side_strength - sell_side_strength) / total_strength  # -1 to +1

        # Attraction vectors
        attraction_vectors = [v for v in self.liquidity_vectors if v.vector_type == 'attract']
        repulsion_vectors = [v for v in self.liquidity_vectors if v.vector_type == 'repel']

        # Net attraction direction
        upward_attraction = sum(v.strength for v in attraction_vectors if v.target_price > current_price)
        downward_attraction = sum(v.strength for v in attraction_vectors if v.target_price < current_price)

        net_direction = 'neutral'
        if upward_attraction > downward_attraction * 1.5:
            net_direction = 'upward'
        elif downward_attraction > upward_attraction * 1.5:
            net_direction = 'downward'

        return {
            'total_zones': len(active_zones),
            'buy_side_zones': len(buy_side),
            'sell_side_zones': len(sell_side),
            'nearest_above': nearest_above.price_level if nearest_above else None,
            'nearest_below': nearest_below.price_level if nearest_below else None,
            'liquidity_imbalance': imbalance,
            'net_attraction_direction': net_direction,
            'upward_attraction': upward_attraction,
            'downward_attraction': downward_attraction,
            'swept_count': len(self.swept_liquidity),
            'active_count': len(active_zones),
            'attraction_vector_count': len(attraction_vectors),
            'repulsion_vector_count': len(repulsion_vectors)
        }

    def get_liquidity_features(self, current_price: float) -> np.ndarray:
        """Get liquidity features as numpy array for ML"""
        profile = self.analyze_liquidity_profile(None, current_price)

        features = np.array([
            profile['total_zones'] / 100.0,  # Normalized
            profile['buy_side_zones'] / 50.0,
            profile['sell_side_zones'] / 50.0,
            profile['liquidity_imbalance'],  # Already -1 to 1
            1.0 if profile['net_attraction_direction'] == 'upward' else (-1.0 if profile['net_attraction_direction'] == 'downward' else 0.0),
            profile['upward_attraction'] / 100.0,
            profile['downward_attraction'] / 100.0,
            profile['swept_count'] / 50.0,
            profile['active_count'] / 100.0,
            profile['attraction_vector_count'] / 20.0,
            profile['repulsion_vector_count'] / 20.0,
        ])

        return features
