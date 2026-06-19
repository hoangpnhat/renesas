"""
ICT Trading System - Price Attractor & Protected PD Arrays
===========================================================
Magnetic price zones, protected arrays, and protection scoring
"""

import pandas as pd
import numpy as np
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass
from .enhanced_pd_detector import EnhancedPDArray
from .liquidity_detector import LiquidityZone


@dataclass
class PriceAttractor:
    """Price attractor/magnet zone"""
    price_level: float
    strength: float  # 0-100
    type: str  # 'pd_array', 'liquidity', 'vwap', 'fib', 'round_number'
    direction: str  # 'attract' or 'repel'
    distance_from_price: float
    timeframe: str
    active: bool = True
    description: str = ""


class PriceAttractorSystem:
    """Advanced price attractor and protection system"""

    def __init__(self):
        self.attractors = []
        self.protected_pd_arrays = []

    def calculate_all_attractors(self, df: pd.DataFrame, current_price: float,
                                 pd_arrays: List[EnhancedPDArray],
                                 liquidity_zones: List[LiquidityZone],
                                 timeframe: str) -> List[PriceAttractor]:
        """Calculate all price attractors"""
        self.attractors = []

        # 1. PD Array attractors
        self._add_pd_array_attractors(pd_arrays, current_price, timeframe)

        # 2. Liquidity attractors
        self._add_liquidity_attractors(liquidity_zones, current_price, timeframe)

        # 3. VWAP attractors
        self._add_vwap_attractors(df, current_price, timeframe)

        # 4. Fibonacci attractors
        self._add_fibonacci_attractors(df, current_price, timeframe)

        # 5. Round number attractors
        self._add_round_number_attractors(current_price, timeframe)

        return self.attractors

    def _add_pd_array_attractors(self, pd_arrays: List[EnhancedPDArray],
                                 current_price: float, timeframe: str):
        """Add PD arrays as price attractors"""
        for arr in pd_arrays:
            if arr.mitigated:
                continue

            # Calculate midpoint of PD array
            midpoint = (arr.top + arr.bottom) / 2
            distance = abs(midpoint - current_price)
            distance_pct = distance / current_price

            # Nearby arrays attract more
            distance_factor = np.exp(-distance_pct * 15)
            attractor_strength = arr.strength * distance_factor

            # PD arrays in premium/discount zones attract price
            attractor = PriceAttractor(
                price_level=midpoint,
                strength=attractor_strength,
                type='pd_array',
                direction='attract',
                distance_from_price=distance,
                timeframe=timeframe,
                description=f"{arr.direction} {arr.type} at {midpoint:.2f}"
            )
            self.attractors.append(attractor)

    def _add_liquidity_attractors(self, liquidity_zones: List[LiquidityZone],
                                  current_price: float, timeframe: str):
        """Add liquidity zones as attractors"""
        for zone in liquidity_zones:
            if zone.swept:
                continue

            distance = abs(zone.price_level - current_price)
            distance_pct = distance / current_price

            # Liquidity is magnetic
            distance_factor = np.exp(-distance_pct * 10)
            attractor_strength = zone.strength * distance_factor

            attractor = PriceAttractor(
                price_level=zone.price_level,
                strength=attractor_strength,
                type='liquidity',
                direction='attract',
                distance_from_price=distance,
                timeframe=timeframe,
                description=f"{zone.type} liquidity at {zone.price_level:.2f}"
            )
            self.attractors.append(attractor)

    def _add_vwap_attractors(self, df: pd.DataFrame, current_price: float, timeframe: str):
        """Add VWAP as price attractor"""
        if 'volume' not in df.columns or len(df) < 20:
            return

        # Calculate VWAP
        df_recent = df.tail(50)
        typical_price = (df_recent['high'] + df_recent['low'] + df_recent['close']) / 3
        vwap = (typical_price * df_recent['volume']).sum() / df_recent['volume'].sum()

        distance = abs(vwap - current_price)
        distance_pct = distance / current_price

        # VWAP is a strong attractor
        distance_factor = np.exp(-distance_pct * 12)
        strength = 75 * distance_factor

        attractor = PriceAttractor(
            price_level=vwap,
            strength=strength,
            type='vwap',
            direction='attract',
            distance_from_price=distance,
            timeframe=timeframe,
            description=f"VWAP at {vwap:.2f}"
        )
        self.attractors.append(attractor)

    def _add_fibonacci_attractors(self, df: pd.DataFrame, current_price: float, timeframe: str):
        """Add Fibonacci retracement levels as attractors"""
        if len(df) < 50:
            return

        # Find recent swing high and low
        recent = df.tail(100)
        swing_high = recent['high'].max()
        swing_low = recent['low'].min()

        range_size = swing_high - swing_low
        if range_size == 0:
            return

        # Key Fibonacci levels
        fib_levels = {
            0.236: swing_low + range_size * 0.236,
            0.382: swing_low + range_size * 0.382,
            0.500: swing_low + range_size * 0.500,
            0.618: swing_low + range_size * 0.618,
            0.786: swing_low + range_size * 0.786,
        }

        for fib_ratio, fib_price in fib_levels.items():
            distance = abs(fib_price - current_price)
            distance_pct = distance / current_price

            # 0.618 and 0.5 are strongest attractors
            base_strength = 70 if fib_ratio in [0.618, 0.500] else 60

            distance_factor = np.exp(-distance_pct * 15)
            strength = base_strength * distance_factor

            attractor = PriceAttractor(
                price_level=fib_price,
                strength=strength,
                type='fib',
                direction='attract',
                distance_from_price=distance,
                timeframe=timeframe,
                description=f"Fib {fib_ratio:.3f} at {fib_price:.2f}"
            )
            self.attractors.append(attractor)

    def _add_round_number_attractors(self, current_price: float, timeframe: str):
        """Add round numbers as psychological attractors"""
        # Determine step size
        if current_price > 10000:
            step = 1000
        elif current_price > 1000:
            step = 100
        elif current_price > 100:
            step = 10
        else:
            step = 1

        # Find nearby round numbers
        lower = (int(current_price / step) - 1) * step
        upper = (int(current_price / step) + 2) * step

        for round_num in [lower, lower + step, upper]:
            distance = abs(round_num - current_price)
            distance_pct = distance / current_price

            # Closer round numbers attract more
            distance_factor = np.exp(-distance_pct * 20)
            strength = 50 * distance_factor

            attractor = PriceAttractor(
                price_level=round_num,
                strength=strength,
                type='round_number',
                direction='attract',
                distance_from_price=distance,
                timeframe=timeframe,
                description=f"Round number {round_num:.0f}"
            )
            self.attractors.append(attractor)

    def calculate_net_attraction(self, current_price: float) -> Dict:
        """Calculate net attraction direction and strength"""
        upward_attraction = 0
        downward_attraction = 0

        for attractor in self.attractors:
            if not attractor.active:
                continue

            if attractor.price_level > current_price:
                # Attractor above price
                upward_attraction += attractor.strength
            elif attractor.price_level < current_price:
                # Attractor below price
                downward_attraction += attractor.strength

        total_attraction = upward_attraction + downward_attraction
        net_direction = 'neutral'

        if total_attraction > 0:
            if upward_attraction > downward_attraction * 1.3:
                net_direction = 'upward'
            elif downward_attraction > upward_attraction * 1.3:
                net_direction = 'downward'

        return {
            'upward_attraction': upward_attraction,
            'downward_attraction': downward_attraction,
            'net_direction': net_direction,
            'total_attraction': total_attraction,
            'attraction_ratio': upward_attraction / (downward_attraction + 1)
        }

    def find_strongest_attractor(self, current_price: float) -> Optional[PriceAttractor]:
        """Find the strongest active attractor"""
        active_attractors = [a for a in self.attractors if a.active]

        if not active_attractors:
            return None

        return max(active_attractors, key=lambda a: a.strength)

    def calculate_pd_array_protection(self, pd_arrays: List[EnhancedPDArray],
                                     structure_breaks: List,
                                     liquidity_zones: List[LiquidityZone]) -> List[EnhancedPDArray]:
        """Calculate protection scores for PD arrays"""
        self.protected_pd_arrays = []

        for arr in pd_arrays:
            if arr.mitigated:
                continue

            protection_score = 0
            confluence_count = 0
            is_protected = False

            # 1. Check for liquidity protection
            nearby_liquidity = [z for z in liquidity_zones
                              if not z.swept and
                              abs(z.price_level - (arr.top + arr.bottom) / 2) / arr.bottom < 0.02]

            if nearby_liquidity:
                protection_score += 25
                confluence_count += len(nearby_liquidity)

            # 2. Check for higher timeframe alignment
            if arr.pd_status in ['discount', 'premium']:
                protection_score += 20

            # 3. Check for structure protection
            # Protected if there's a structure break in the same direction
            for sb in structure_breaks:
                if sb.direction == arr.direction:
                    protection_score += 15
                    confluence_count += 1
                    break

            # 4. Volume protection
            if arr.volume > 0:  # Has volume data
                protection_score += 10

            # 5. Strength bonus
            protection_score += arr.strength * 0.3

            # Determine if protected
            if protection_score >= 50:
                is_protected = True

            # Update array
            arr.protected = is_protected
            arr.protection_score = min(protection_score, 100)
            arr.confluence_count = confluence_count

            if is_protected:
                self.protected_pd_arrays.append(arr)

        return self.protected_pd_arrays

    def get_attractor_features(self, current_price: float) -> np.ndarray:
        """Get attractor features as numpy array for ML"""
        net_attr = self.calculate_net_attraction(current_price)
        strongest = self.find_strongest_attractor(current_price)

        features = np.array([
            net_attr['upward_attraction'] / 100.0,
            net_attr['downward_attraction'] / 100.0,
            1.0 if net_attr['net_direction'] == 'upward' else (-1.0 if net_attr['net_direction'] == 'downward' else 0.0),
            net_attr['attraction_ratio'] / 10.0,  # Normalize
            strongest.strength / 100.0 if strongest else 0.0,
            abs(strongest.distance_from_price) / current_price if strongest else 0.5,
            len(self.attractors) / 50.0,  # Normalized count
            len(self.protected_pd_arrays) / 20.0,
            len([a for a in self.attractors if a.type == 'pd_array']) / 30.0,
            len([a for a in self.attractors if a.type == 'liquidity']) / 20.0,
        ])

        return features

    def get_protection_features(self) -> np.ndarray:
        """Get protection features for ML"""
        if not self.protected_pd_arrays:
            return np.zeros(5)

        avg_protection = np.mean([arr.protection_score for arr in self.protected_pd_arrays])
        avg_confluence = np.mean([arr.confluence_count for arr in self.protected_pd_arrays])

        bullish_protected = len([arr for arr in self.protected_pd_arrays if arr.direction == 'bullish'])
        bearish_protected = len([arr for arr in self.protected_pd_arrays if arr.direction == 'bearish'])

        features = np.array([
            len(self.protected_pd_arrays) / 10.0,
            avg_protection / 100.0,
            avg_confluence / 5.0,
            bullish_protected / 10.0,
            bearish_protected / 10.0,
        ])

        return features
