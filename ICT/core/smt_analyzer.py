"""
ICT Trading System - SMT Analyzer
==================================
Smart Money Technique (SMT) Divergence Detection
"""

import pandas as pd
from typing import Dict, List, Tuple
from scipy.signal import find_peaks
from data_structures import SMTDivergence


class SMTAnalyzer:
    """Smart Money Technique Divergence Analyzer"""

    def __init__(self, asset1_data: Dict[str, pd.DataFrame],
                 asset2_data: Dict[str, pd.DataFrame],
                 asset1_name: str = "Asset1", asset2_name: str = "Asset2"):
        self.asset1_data = asset1_data
        self.asset2_data = asset2_data
        self.asset1_name = asset1_name
        self.asset2_name = asset2_name
        self.timeframes = list(asset1_data.keys())
        self.smt_divergences = {tf: [] for tf in self.timeframes}

        # Timeframe reliability weights (lower timeframes less reliable for SMT)
        self.tf_weights = {
            '1m': 0.3,   # Very unreliable on 1 minute
            '5m': 0.5,   # Low reliability
            '15m': 0.6,  # Medium-low
            '30m': 0.7,  # Medium
            '1h': 0.8,   # Medium-high
            '4h': 0.9,   # High
            '1d': 1.0,   # Full reliability
            'D': 1.0,
            '1w': 1.0,   # Full reliability
            'W': 1.0,
            '1M': 1.0,
            'M': 1.0
        }

    def detect_swing_points(self, df: pd.DataFrame, lookback: int = 10) -> Tuple[List, List]:
        """Phát hiện swing points"""
        highs = df['high'].values
        lows = df['low'].values
        high_peaks, _ = find_peaks(highs, distance=lookback)
        low_peaks, _ = find_peaks(-lows, distance=lookback)
        return high_peaks.tolist(), low_peaks.tolist()

    def detect_smt_divergences(self):
        """Phát hiện SMT divergences"""
        for tf in self.timeframes:
            df1, df2 = self.asset1_data[tf], self.asset2_data[tf]
            highs1, lows1 = self.detect_swing_points(df1)
            highs2, lows2 = self.detect_swing_points(df2)

            bullish = self._detect_bullish_smt(df1, df2, lows1, lows2, tf)
            bearish = self._detect_bearish_smt(df1, df2, highs1, highs2, tf)
            self.smt_divergences[tf] = bullish + bearish

    def _detect_bullish_smt(self, df1, df2, lows1, lows2, tf) -> List[SMTDivergence]:
        """Bullish SMT: Asset 1 lower low, Asset 2 higher/equal low"""
        smts = []
        if len(lows1) < 2 or len(lows2) < 2:
            return smts

        for i in range(len(lows1) - 1):
            idx1_old, idx1_new = lows1[i], lows1[i + 1]

            if df1['low'].iloc[idx1_new] < df1['low'].iloc[idx1_old]:
                for j in range(len(lows2) - 1):
                    idx2_old, idx2_new = lows2[j], lows2[j + 1]

                    if abs(idx1_old - idx2_old) <= 10 and abs(idx1_new - idx2_new) <= 10:
                        if df2['low'].iloc[idx2_new] >= df2['low'].iloc[idx2_old]:
                            # Adjust strength based on timeframe reliability
                            base_strength = 75
                            tf_weight = self.tf_weights.get(tf, 1.0)
                            adjusted_strength = base_strength * tf_weight

                            smt = SMTDivergence(
                                type='bullish_smt', asset1=self.asset1_name,
                                asset2=self.asset2_name, timeframe=tf,
                                asset1_high=df1['high'].iloc[idx1_new],
                                asset1_low=df1['low'].iloc[idx1_new],
                                asset2_high=df2['high'].iloc[idx2_new],
                                asset2_low=df2['low'].iloc[idx2_new],
                                divergence_time=df1.get('timestamp', {}).get(idx1_new),
                                strength=adjusted_strength,
                                description=f"Bullish SMT detected (TF reliability: {tf_weight*100:.0f}%)"
                            )
                            smts.append(smt)
        return smts

    def _detect_bearish_smt(self, df1, df2, highs1, highs2, tf) -> List[SMTDivergence]:
        """Bearish SMT: Asset 1 higher high, Asset 2 lower/equal high"""
        smts = []
        if len(highs1) < 2 or len(highs2) < 2:
            return smts

        for i in range(len(highs1) - 1):
            idx1_old, idx1_new = highs1[i], highs1[i + 1]

            if df1['high'].iloc[idx1_new] > df1['high'].iloc[idx1_old]:
                for j in range(len(highs2) - 1):
                    idx2_old, idx2_new = highs2[j], highs2[j + 1]

                    if abs(idx1_old - idx2_old) <= 10 and abs(idx1_new - idx2_new) <= 10:
                        if df2['high'].iloc[idx2_new] <= df2['high'].iloc[idx2_old]:
                            # Adjust strength based on timeframe reliability
                            base_strength = 75
                            tf_weight = self.tf_weights.get(tf, 1.0)
                            adjusted_strength = base_strength * tf_weight

                            smt = SMTDivergence(
                                type='bearish_smt', asset1=self.asset1_name,
                                asset2=self.asset2_name, timeframe=tf,
                                asset1_high=df1['high'].iloc[idx1_new],
                                asset1_low=df1['low'].iloc[idx1_new],
                                asset2_high=df2['high'].iloc[idx2_new],
                                asset2_low=df2['low'].iloc[idx2_new],
                                divergence_time=df1.get('timestamp', {}).get(idx1_new),
                                strength=adjusted_strength,
                                description=f"Bearish SMT detected (TF reliability: {tf_weight*100:.0f}%)"
                            )
                            smts.append(smt)
        return smts

    def get_smt_signal(self) -> Dict:
        """Generate signal từ SMT"""
        all_smts = []
        for tf in self.timeframes:
            all_smts.extend(self.smt_divergences[tf])

        if not all_smts:
            return {'signal': 'NEUTRAL', 'confidence': 0, 'reason': 'No SMT'}

        latest = all_smts[-1]
        signal = 'BUY' if latest.type == 'bullish_smt' else 'SELL'

        return {
            'signal': signal,
            'confidence': latest.strength,
            'reason': latest.description,
            'smt': latest
        }
