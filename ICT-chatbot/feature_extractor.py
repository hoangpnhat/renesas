"""
Feature Extractor for ICT RL Trading Bot
Extracts 32 features as defined in Phase 1
"""
import numpy as np
import pandas as pd
from typing import List, Optional
from datetime import datetime

from structures import FVG, SwingPoint, MSS, LiquiditySweep, BiasDirection, Position
import indicators
import config


class FeatureExtractor:
    """

    """

    def __init__(self, ohlc_1m: pd.DataFrame, ohlc_15m: pd.DataFrame):
        """
        Initialize feature extractor

        Args:
            ohlc_1m: 1-minute OHLC DataFrame
            ohlc_15m: 15-minute OHLC DataFrame
        """
        self.ohlc_1m = ohlc_1m
        self.ohlc_15m = ohlc_15m

        # Pre-calculate ATR
        self.atr_1m = indicators.calculate_atr(ohlc_1m, period=config.ATR_PERIOD)
        self.atr_15m = indicators.calculate_atr(ohlc_15m, period=config.ATR_PERIOD)

        # State trackers
        self.active_fvgs: List[FVG] = []
        self.swing_highs: List[SwingPoint] = []
        self.swing_lows: List[SwingPoint] = []
        self.current_mss: Optional[MSS] = None
        self.current_sweep: Optional[LiquiditySweep] = None
        self.htf_bias: BiasDirection = BiasDirection.NEUTRAL

    def reset(self, start_idx: int):
        """Reset state to episode start"""
        self.active_fvgs = []
        self.swing_highs = []
        self.swing_lows = []
        self.current_mss = None
        self.current_sweep = None
        self.htf_bias = BiasDirection.NEUTRAL

    def _map_1m_to_15m_idx(self, idx_1m: int) -> int:
        """Map 1M index to corresponding 15M index"""
        timestamp_1m = self.ohlc_1m.index[idx_1m]
        # Find nearest 15M bar
        idx_15m = self.ohlc_15m.index.get_indexer([timestamp_1m], method='ffill')[0]
        return max(0, idx_15m)

    def update_indicators(self, current_idx_1m: int):
        """
        Update ICT indicators at current bar

        Args:
            current_idx_1m: Current 1M bar index
        """
        # Update swing points
        self.swing_highs, self.swing_lows = indicators.detect_swing_points(
            self.ohlc_1m,
            pivot_window=config.SWEEP_PIVOT_WINDOW,
            lookback=config.SWEEP_LOOKBACK
        )

        # Update liquidity sweep
        new_sweep = indicators.detect_liquidity_sweep(
            self.ohlc_1m,
            current_idx_1m,
            self.swing_highs,
            self.swing_lows,
            reversal_bars=config.SWEEP_REVERSAL_BARS
        )
        if new_sweep:
            self.current_sweep = new_sweep
        elif self.current_sweep:
            self.current_sweep.bars_since += 1

        # Update MSS on 15M
        idx_15m = self._map_1m_to_15m_idx(current_idx_1m)
        new_mss = indicators.detect_market_structure_shift(
            self.ohlc_15m,
            idx_15m,
            self.atr_15m,
            lookback=config.MSS_LOOKBACK,
            min_move_mult=config.MSS_MIN_MOVE_ATR_MULT
        )
        if new_mss:
            self.current_mss = new_mss
            self.htf_bias = new_mss.direction
        elif self.current_mss:
            self.current_mss.bars_since += 1
            # Invalidate if too old
            if self.current_mss.bars_since > config.MSS_VALIDITY_BARS:
                self.htf_bias = BiasDirection.NEUTRAL

        # Update FVGs
        self.active_fvgs = indicators.detect_fair_value_gaps(
            self.ohlc_1m,
            current_idx_1m,
            self.atr_1m,
            self.active_fvgs,
            min_gap_mult=config.FVG_MIN_SIZE_ATR_MULT,
            max_age=config.FVG_MAX_AGE
        )

    def extract_features(
        self,
        current_idx_1m: int,
        position: Optional[Position] = None,
        trades_today: int = 0
    ) -> np.ndarray:
        """
        Extract 32-dimensional feature vector

        Args:
            current_idx_1m: Current 1M bar index
            position: Current position (if any)
            trades_today: Number of trades taken in current session

        Returns:
            32-dim numpy array
        """
        features = {}

        current_bar = self.ohlc_1m.iloc[current_idx_1m]
        current_price = current_bar['close']
        current_time = current_bar.name

        idx_15m = self._map_1m_to_15m_idx(current_idx_1m)

        # Get ATR values
        atr_1m_val = self.atr_1m.iloc[current_idx_1m]
        atr_15m_val = self.atr_15m.iloc[idx_15m]

        # Handle NaN ATR
        if pd.isna(atr_1m_val) or atr_1m_val == 0:
            atr_1m_val = 0.0001
        if pd.isna(atr_15m_val) or atr_15m_val == 0:
            atr_15m_val = 0.0001

        # ============================================================
        # GROUP A: MARKET STRUCTURE (15M basis)
        # ============================================================
        features['htf_bias'] = self.htf_bias.value

        if self.current_mss:
            features['bars_since_mss'] = min(self.current_mss.bars_since, 60) / 60.0
            features['distance_to_structure'] = (
                (current_price - self.current_mss.structure_level) / atr_15m_val
            )
        else:
            features['bars_since_mss'] = 1.0  # Maxed out
            features['distance_to_structure'] = 0.0

        # Swing high/low distances (15M)
        start_idx_15m = max(0, idx_15m - config.PD_SWING_WINDOW)
        swing_window = self.ohlc_15m.iloc[start_idx_15m:idx_15m + 1]
        swing_high = swing_window['high'].max()
        swing_low = swing_window['low'].min()

        features['swing_high_15m'] = (swing_high - current_price) / atr_15m_val
        features['swing_low_15m'] = (current_price - swing_low) / atr_15m_val

        # Premium/Discount zone
        pd_zone = indicators.classify_premium_discount(
            current_price,
            self.ohlc_15m,
            idx_15m,
            window=config.PD_SWING_WINDOW,
            threshold=config.PD_THRESHOLD
        )
        features['pd_zone'] = pd_zone

        # ============================================================
        # GROUP B: LIQUIDITY (1M basis)
        # ============================================================
        if self.current_sweep:
            if self.current_sweep.direction == 'BULLISH':
                features['sweep_detected'] = 1
            else:
                features['sweep_detected'] = -1
            features['bars_since_sweep'] = min(self.current_sweep.bars_since, 50) / 50.0
            features['sweep_level_distance'] = (
                (current_price - self.current_sweep.level) / atr_1m_val
            )
        else:
            features['sweep_detected'] = 0
            features['bars_since_sweep'] = 1.0
            features['sweep_level_distance'] = 0.0

        # Count unswept swing points
        unswept_highs = [s for s in self.swing_highs if not s.swept]
        unswept_lows = [s for s in self.swing_lows if not s.swept]
        features['num_swing_highs'] = min(len(unswept_highs), 10) / 10.0
        features['num_swing_lows'] = min(len(unswept_lows), 10) / 10.0

        # ============================================================
        # GROUP C: FAIR VALUE GAP (1M basis)
        # ============================================================
        bullish_fvgs = [f for f in self.active_fvgs if f.fvg_type == 'BULLISH']
        bearish_fvgs = [f for f in self.active_fvgs if f.fvg_type == 'BEARISH']

        features['num_bullish_fvg'] = min(len(bullish_fvgs), 5) / 5.0
        features['num_bearish_fvg'] = min(len(bearish_fvgs), 5) / 5.0

        # Nearest FVG distances
        if bullish_fvgs:
            nearest_bull = min(bullish_fvgs, key=lambda f: abs(current_price - f.bottom))
            features['nearest_bull_fvg_dist'] = (current_price - nearest_bull.bottom) / atr_1m_val
            nearest_bull_age = nearest_bull.age
            price_in_bull_fvg = nearest_bull.contains_price(current_price)
        else:
            features['nearest_bull_fvg_dist'] = 10.0  # Far away
            nearest_bull_age = 100
            price_in_bull_fvg = False

        if bearish_fvgs:
            nearest_bear = min(bearish_fvgs, key=lambda f: abs(current_price - f.top))
            features['nearest_bear_fvg_dist'] = (nearest_bear.top - current_price) / atr_1m_val
            nearest_bear_age = nearest_bear.age
            price_in_bear_fvg = nearest_bear.contains_price(current_price)
        else:
            features['nearest_bear_fvg_dist'] = 10.0
            nearest_bear_age = 100
            price_in_bear_fvg = False

        # Use nearest of either direction
        nearest_age = min(nearest_bull_age, nearest_bear_age)
        features['nearest_fvg_age'] = min(nearest_age, 100) / 100.0
        features['price_in_fvg'] = 1.0 if (price_in_bull_fvg or price_in_bear_fvg) else 0.0

        # ============================================================
        # GROUP D: TIME & SESSION
        # ============================================================
        hour = current_time.hour
        minute = current_time.minute

        in_killzone = (config.LONDON_KILLZONE_START <= hour < config.LONDON_KILLZONE_END)
        features['in_london_killzone'] = 1.0 if in_killzone else 0.0

        if in_killzone:
            minutes_into_session = (hour - config.LONDON_KILLZONE_START) * 60 + minute
            features['minutes_into_session'] = minutes_into_session / 180.0  # Max 3 hours
        else:
            features['minutes_into_session'] = 0.0

        # Cyclic time encoding
        features['hour_sin'] = np.sin(2 * np.pi * hour / 24)
        features['hour_cos'] = np.cos(2 * np.pi * hour / 24)

        # Day of week (0 = Monday, 4 = Friday)
        day_of_week = current_time.weekday()
        features['day_of_week'] = day_of_week / 4.0

        # ============================================================
        # GROUP E: PRICE ACTION (1M basis)
        # ============================================================
        features['atr_1m'] = np.log(atr_1m_val + 1e-8)
        features['atr_15m'] = np.log(atr_15m_val + 1e-8)

        # Candle body ratio
        candle_range = current_bar['high'] - current_bar['low']
        if candle_range > 0:
            body_size = abs(current_bar['close'] - current_bar['open'])
            features['candle_body_ratio'] = body_size / candle_range
        else:
            features['candle_body_ratio'] = 0.0

        # Candle direction
        if current_bar['close'] > current_bar['open']:
            features['candle_direction'] = 1  # Bullish
        elif current_bar['close'] < current_bar['open']:
            features['candle_direction'] = -1  # Bearish
        else:
            features['candle_direction'] = 0  # Doji

        # Price velocity (5-bar momentum)
        if current_idx_1m >= 5:
            close_5_bars_ago = self.ohlc_1m.iloc[current_idx_1m - 5]['close']
            features['price_velocity'] = (current_price - close_5_bars_ago) / atr_1m_val
        else:
            features['price_velocity'] = 0.0

        # ============================================================
        # GROUP F: POSITION & RISK
        # ============================================================
        if position:
            features['has_position'] = 1.0
            features['position_direction'] = position.direction.value
            features['unrealized_pnl'] = position.get_risk_multiple()  # In R multiples
            features['bars_in_position'] = min(position.bars_held, 100) / 100.0
        else:
            features['has_position'] = 0.0
            features['position_direction'] = 0
            features['unrealized_pnl'] = 0.0
            features['bars_in_position'] = 0.0

        features['trades_taken_today'] = min(trades_today, 3) / 3.0

        # ============================================================
        # Convert to ordered array
        # ============================================================
        feature_vector = np.array([
            features[name] for name in config.FEATURE_NAMES
        ], dtype=np.float32)

        # Clip extremes
        feature_vector = np.clip(feature_vector, config.OBSERVATION_LOW, config.OBSERVATION_HIGH)

        return feature_vector

    def get_nearest_fvg(self, direction: str, current_price: float) -> Optional[FVG]:
        """
        Get nearest FVG of specified direction

        Args:
            direction: 'BULLISH' or 'BEARISH'
            current_price: Current price

        Returns:
            Nearest FVG or None
        """
        fvgs = [f for f in self.active_fvgs if f.fvg_type == direction]
        if not fvgs:
            return None

        if direction == 'BULLISH':
            return min(fvgs, key=lambda f: abs(current_price - f.bottom))
        else:
            return min(fvgs, key=lambda f: abs(current_price - f.top))

    def evaluate_setup_quality(self, current_idx_1m: int) -> dict:
        """
        Evaluate current setup quality (for reward calculation)
        This is a HIDDEN feature not given to agent

        Returns:
            Dict with setup evaluation
        """
        in_killzone = self._in_killzone(self.ohlc_1m.iloc[current_idx_1m].name)

        confluence_score = 0
        setup_valid = False
        true_direction = None

        # Check all setup conditions
        has_bias = (self.htf_bias != BiasDirection.NEUTRAL)
        has_sweep = (self.current_sweep is not None and self.current_sweep.bars_since < 50)
        has_recent_mss = (self.current_mss is not None and
                         self.current_mss.bars_since <= config.MSS_VALIDITY_BARS)

        if has_bias:
            confluence_score += 30

        if has_sweep:
            confluence_score += 20
            # Check if sweep is opposite to bias (correct)
            if self.htf_bias == BiasDirection.BULLISH and self.current_sweep.direction == 'BEARISH':
                confluence_score += 10
            elif self.htf_bias == BiasDirection.BEARISH and self.current_sweep.direction == 'BULLISH':
                confluence_score += 10

        if has_recent_mss:
            confluence_score += 20

        if in_killzone:
            confluence_score += 20

        # Check if setup is complete
        if has_bias and has_sweep and has_recent_mss and in_killzone:
            setup_valid = True
            true_direction = 'LONG' if self.htf_bias == BiasDirection.BULLISH else 'SHORT'

        return {
            'setup_valid': setup_valid,
            'confluence_score': confluence_score,
            'true_direction': true_direction,
            'has_bias': has_bias,
            'has_sweep': has_sweep,
            'has_recent_mss': has_recent_mss,
            'in_killzone': in_killzone,
        }

    def _in_killzone(self, timestamp: datetime) -> bool:
        """Check if timestamp is in London Killzone"""
        return config.LONDON_KILLZONE_START <= timestamp.hour < config.LONDON_KILLZONE_END
