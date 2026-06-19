"""
ICT Trading System - Feature Extraction Pipeline
=================================================
Integrated pipeline combining all ICT analysis components
"""

import pandas as pd
import numpy as np
from typing import Dict, List, Optional

# Import all analyzers
from core.enhanced_pd_detector import EnhancedPDDetector
from core.market_structure import MarketStructureDetector
from core.liquidity_detector import LiquidityDetector
from core.session_analyzer import SessionAnalyzer
from core.price_attractor import PriceAttractorSystem
from core.order_flow_analyzer import OrderFlowAnalyzer
from analysis.enhanced_llm_analyzer import EnhancedLLMAnalyzer


class FeatureExtractionPipeline:
    """
    Comprehensive feature extraction pipeline integrating all ICT components.

    Pipeline Flow:
    Price OHLC → Detect PD Arrays (OB, FVG, EQH/EQL)
               → Market Structure (BOS, CHOCH)
               → Liquidity vectors (attract / repel)
               → Session features (London, NY)
               → LLM summary
               → state_t (100+ features)
               → RL Agent
    """

    def __init__(self, df: pd.DataFrame, timeframe: str = '1h',
                 use_llm: bool = True, llm_api_key: Optional[str] = None):
        """
        Initialize the feature extraction pipeline

        Args:
            df: OHLCV DataFrame with timestamp
            timeframe: Trading timeframe
            use_llm: Whether to use LLM analysis
            llm_api_key: OpenAI API key
        """
        self.df = df.copy().reset_index(drop=True)
        self.timeframe = timeframe
        self.use_llm = use_llm

        print("\n" + "="*70)
        print("INITIALIZING ENHANCED ICT FEATURE EXTRACTION PIPELINE")
        print("="*70)

        # Initialize all components
        print("📊 Initializing PD Array Detector...")
        self.pd_detector = EnhancedPDDetector()

        print("🔧 Initializing Market Structure Detector...")
        self.structure_detector = MarketStructureDetector()

        print("💧 Initializing Liquidity Detector...")
        self.liquidity_detector = LiquidityDetector()

        print("🕐 Initializing Session Analyzer...")
        self.session_analyzer = SessionAnalyzer()

        print("🧲 Initializing Price Attractor System...")
        self.attractor_system = PriceAttractorSystem()

        print("📈 Initializing Order Flow Analyzer...")
        self.order_flow_analyzer = OrderFlowAnalyzer()

        if use_llm:
            print("🤖 Initializing Enhanced LLM Analyzer...")
            self.llm_analyzer = EnhancedLLMAnalyzer(api_key=llm_api_key)
        else:
            self.llm_analyzer = None

        # Run initial analysis
        print("\n🔍 Running initial market analysis...")
        self._run_full_analysis()

        print("✅ Pipeline initialization complete!")
        print("="*70 + "\n")

    def _run_full_analysis(self):
        """Run complete analysis on the entire dataset"""
        # 1. Detect PD Arrays
        self.pd_arrays = self.pd_detector.detect_all_pd_arrays(self.df, self.timeframe)
        print(f"   ✓ Detected {len(self.pd_arrays)} PD arrays")

        # 2. Analyze Market Structure
        self.structure_analysis = self.structure_detector.analyze_structure(self.df, self.timeframe)
        print(f"   ✓ Market Structure: {self.structure_analysis['current_trend']}")
        print(f"   ✓ Structure Breaks: {len(self.structure_analysis['structure_breaks'])}")

        # 3. Detect Liquidity Zones
        self.liquidity_analysis = self.liquidity_detector.detect_all_liquidity(self.df, self.timeframe)
        print(f"   ✓ Liquidity Zones: {len(self.liquidity_analysis['liquidity_zones'])}")

        # 4. Analyze Order Flow
        self.order_flow_analysis = self.order_flow_analyzer.analyze_order_flow(self.df)
        print(f"   ✓ Order Flow Signals: {len(self.order_flow_analysis['order_flow_signals'])}")

        # 5. Calculate Protected PD Arrays and Attractors
        current_price = self.df['close'].iloc[-1]

        self.protected_arrays = self.attractor_system.calculate_pd_array_protection(
            self.pd_arrays,
            self.structure_analysis['structure_breaks'],
            self.liquidity_analysis['liquidity_zones']
        )
        print(f"   ✓ Protected PD Arrays: {len(self.protected_arrays)}")

        self.attractors = self.attractor_system.calculate_all_attractors(
            self.df, current_price, self.pd_arrays,
            self.liquidity_analysis['liquidity_zones'], self.timeframe
        )
        print(f"   ✓ Price Attractors: {len(self.attractors)}")

    def extract_features(self, current_idx: int) -> np.ndarray:
        """
        Extract all features for a given index

        Returns:
            Feature vector (100+ dimensions)
        """
        if current_idx < 20 or current_idx >= len(self.df):
            # Return zero features for invalid indices
            return np.zeros(110, dtype=np.float32)

        # Get current price data
        current_row = self.df.iloc[current_idx]
        current_price = current_row['close']

        # === 1. BASIC PRICE FEATURES (10) ===
        price_features = self._extract_price_features(current_idx)

        # === 2. PD ARRAY FEATURES (15) ===
        pd_features = self._extract_pd_array_features(current_idx, current_price)

        # === 3. MARKET STRUCTURE FEATURES (12) ===
        structure_features = self.structure_detector.get_structure_features(self.df, current_idx)
        structure_vector = np.array([
            1.0 if structure_features['trend'] == 'bullish' else (-1.0 if structure_features['trend'] == 'bearish' else 0.0),
            structure_features['recent_bos_count'] / 10.0,
            structure_features['recent_choch_count'] / 5.0,
            structure_features['bullish_structure_strength'] / 100.0,
            structure_features['bearish_structure_strength'] / 100.0,
            structure_features['hh_count'] / 10.0,
            structure_features['hl_count'] / 10.0,
            structure_features['lh_count'] / 10.0,
            structure_features['ll_count'] / 10.0,
            structure_features['structure_alignment'],
            float(structure_features['hh_count'] > structure_features['lh_count']),
            float(structure_features['hl_count'] > structure_features['ll_count']),
        ])

        # === 4. LIQUIDITY FEATURES (11) ===
        liquidity_features = self.liquidity_detector.get_liquidity_features(current_price)

        # === 5. SESSION FEATURES (11) ===
        if 'timestamp' in self.df.columns:
            timestamp = current_row['timestamp']
        else:
            timestamp = pd.Timestamp.now()  # Fallback

        session_features = self.session_analyzer.get_session_vector_features(timestamp)

        # === 6. PRICE ATTRACTOR FEATURES (10) ===
        attractor_features = self.attractor_system.get_attractor_features(current_price)

        # === 7. PROTECTION FEATURES (5) ===
        protection_features = self.attractor_system.get_protection_features()

        # === 8. ORDER FLOW FEATURES (10) ===
        order_flow_features = self.order_flow_analyzer.get_order_flow_features(self.df, current_idx)

        # === 9. TECHNICAL INDICATORS (15) ===
        technical_features = self._extract_technical_features(current_idx)

        # === 10. LLM FEATURES (if enabled) (5) ===
        if self.use_llm and self.llm_analyzer:
            llm_features = self._extract_llm_features(current_idx)
        else:
            llm_features = np.zeros(5, dtype=np.float32)

        # === COMBINE ALL FEATURES ===
        all_features = np.concatenate([
            price_features,        # 10
            pd_features,           # 15
            structure_vector,      # 12
            liquidity_features,    # 11
            session_features,      # 11
            attractor_features,    # 10
            protection_features,   # 5
            order_flow_features,   # 10
            technical_features,    # 15
            llm_features,          # 5
        ])

        # Ensure all features are valid (no NaN/Inf)
        all_features = np.nan_to_num(all_features, nan=0.0, posinf=1.0, neginf=-1.0)

        return all_features.astype(np.float32)

    def _extract_price_features(self, idx: int) -> np.ndarray:
        """Extract basic price features"""
        current = self.df.iloc[idx]
        prev = self.df.iloc[idx-1]

        # Normalize prices
        close_norm = current['close'] / 10000.0
        high_norm = current['high'] / 10000.0
        low_norm = current['low'] / 10000.0
        open_norm = current['open'] / 10000.0

        # Price changes
        price_change = (current['close'] - prev['close']) / prev['close']
        high_low_range = (current['high'] - current['low']) / current['close']

        # Moving averages
        sma_10 = self.df['close'].iloc[max(0, idx-10):idx+1].mean()
        sma_20 = self.df['close'].iloc[max(0, idx-20):idx+1].mean()

        features = np.array([
            close_norm,
            high_norm,
            low_norm,
            open_norm,
            price_change,
            high_low_range,
            (current['close'] - sma_10) / sma_10 if sma_10 > 0 else 0,
            (current['close'] - sma_20) / sma_20 if sma_20 > 0 else 0,
            (sma_10 - sma_20) / sma_20 if sma_20 > 0 else 0,
            float(current['close'] > current['open']),  # Bullish candle
        ])

        return features

    def _extract_pd_array_features(self, idx: int, current_price: float) -> np.ndarray:
        """Extract PD array features"""
        # Find nearby PD arrays
        nearby_arrays = [arr for arr in self.pd_arrays
                        if arr.index <= idx and not arr.mitigated]

        # Count by type and direction
        bullish_ob = sum(1 for arr in nearby_arrays if arr.type == 'OB' and arr.direction == 'bullish')
        bearish_ob = sum(1 for arr in nearby_arrays if arr.type == 'OB' and arr.direction == 'bearish')
        bullish_fvg = sum(1 for arr in nearby_arrays if arr.type == 'FVG' and arr.direction == 'bullish')
        bearish_fvg = sum(1 for arr in nearby_arrays if arr.type == 'FVG' and arr.direction == 'bearish')
        breakers = sum(1 for arr in nearby_arrays if arr.type == 'Breaker')

        # Find nearest arrays
        nearby_bullish = [arr for arr in nearby_arrays if arr.direction == 'bullish']
        nearby_bearish = [arr for arr in nearby_arrays if arr.direction == 'bearish']

        nearest_bullish_dist = min(
            [abs((arr.top + arr.bottom) / 2 - current_price) / current_price for arr in nearby_bullish],
            default=1.0
        )

        nearest_bearish_dist = min(
            [abs((arr.top + arr.bottom) / 2 - current_price) / current_price for arr in nearby_bearish],
            default=1.0
        )

        # Average strength
        avg_strength = np.mean([arr.strength for arr in nearby_arrays]) if nearby_arrays else 0

        # Premium/Discount status
        discount_arrays = sum(1 for arr in nearby_arrays if arr.pd_status == 'discount')
        premium_arrays = sum(1 for arr in nearby_arrays if arr.pd_status == 'premium')

        features = np.array([
            bullish_ob / 10.0,
            bearish_ob / 10.0,
            bullish_fvg / 10.0,
            bearish_fvg / 10.0,
            breakers / 10.0,
            len(nearby_arrays) / 20.0,
            nearest_bullish_dist,
            nearest_bearish_dist,
            avg_strength / 100.0,
            discount_arrays / 10.0,
            premium_arrays / 10.0,
            float(discount_arrays > premium_arrays),
            float(bullish_ob + bullish_fvg > bearish_ob + bearish_fvg),
            len(self.protected_arrays) / 10.0,
            float(any(arr.protected for arr in nearby_arrays)),
        ])

        return features

    def _extract_technical_features(self, idx: int) -> np.ndarray:
        """Extract technical indicators"""
        if idx < 20:
            return np.zeros(15)

        recent = self.df.iloc[max(0, idx-20):idx+1]

        # RSI approximation
        delta = recent['close'].diff()
        gain = (delta.where(delta > 0, 0)).rolling(window=14).mean()
        loss = (-delta.where(delta < 0, 0)).rolling(window=14).mean()
        rs = gain / (loss + 0.0001)
        rsi = 100 - (100 / (1 + rs))
        rsi_value = rsi.iloc[-1] if not pd.isna(rsi.iloc[-1]) else 50

        # Volatility
        volatility = recent['close'].std() / recent['close'].mean() if len(recent) > 1 else 0

        # Volume features
        if 'volume' in recent.columns:
            volume_ratio = recent['volume'].iloc[-1] / (recent['volume'].mean() + 1)
            volume_trend = float(recent['volume'].iloc[-1] > recent['volume'].iloc[-5:].mean())
        else:
            volume_ratio = 1.0
            volume_trend = 0.0

        # Momentum
        momentum_5 = (recent['close'].iloc[-1] - recent['close'].iloc[-5]) / recent['close'].iloc[-5]
        momentum_10 = (recent['close'].iloc[-1] - recent['close'].iloc[-10]) / recent['close'].iloc[-10]

        # Bollinger Bands
        sma_20 = recent['close'].rolling(20).mean().iloc[-1]
        std_20 = recent['close'].rolling(20).std().iloc[-1]
        bb_upper = sma_20 + 2 * std_20
        bb_lower = sma_20 - 2 * std_20
        bb_position = (recent['close'].iloc[-1] - bb_lower) / (bb_upper - bb_lower + 0.0001)

        features = np.array([
            rsi_value / 100.0,
            volatility * 10,
            volume_ratio,
            volume_trend,
            momentum_5,
            momentum_10,
            bb_position,
            float(rsi_value > 70),  # Overbought
            float(rsi_value < 30),  # Oversold
            recent['high'].max() / recent['close'].iloc[-1] - 1,
            1 - recent['low'].min() / recent['close'].iloc[-1],
            float(recent['close'].iloc[-1] > sma_20),
            (recent['close'].iloc[-1] - recent['open'].iloc[-1]) / recent['close'].iloc[-1],
            float(recent['close'].is_monotonic_increasing),
            float(recent['close'].is_monotonic_decreasing),
        ])

        return features

    def _extract_llm_features(self, idx: int) -> np.ndarray:
        """Extract LLM-based features (cached for performance)"""
        # Simple features based on LLM analysis
        # In production, you'd cache these to avoid repeated API calls

        features = np.array([
            0.0,  # LLM confidence placeholder
            0.0,  # LLM bias (-1 to 1)
            0.0,  # LLM structure alignment
            0.0,  # LLM risk score
            0.0,  # LLM recommendation (0=hold, 1=buy, -1=sell)
        ])

        return features

    def get_trading_signal(self, idx: int) -> Dict:
        """Get comprehensive trading signal"""
        current_price = self.df['close'].iloc[idx]

        # Combine all signals
        structure_features = self.structure_detector.get_structure_features(self.df, idx)
        liquidity_profile = self.liquidity_detector.analyze_liquidity_profile(None, current_price)
        net_attraction = self.attractor_system.calculate_net_attraction(current_price)

        # Calculate overall signal
        bullish_score = 0
        bearish_score = 0

        # Structure
        if structure_features['trend'] == 'bullish':
            bullish_score += 30
        elif structure_features['trend'] == 'bearish':
            bearish_score += 30

        # Liquidity
        if liquidity_profile['net_attraction_direction'] == 'upward':
            bullish_score += 20
        elif liquidity_profile['net_attraction_direction'] == 'downward':
            bearish_score += 20

        # Attractors
        if net_attraction['net_direction'] == 'upward':
            bullish_score += 15
        elif net_attraction['net_direction'] == 'downward':
            bearish_score += 15

        # Protected arrays
        bullish_protected = len([arr for arr in self.protected_arrays if arr.direction == 'bullish'])
        bearish_protected = len([arr for arr in self.protected_arrays if arr.direction == 'bearish'])

        bullish_score += bullish_protected * 5
        bearish_score += bearish_protected * 5

        # Determine recommendation
        if bullish_score > bearish_score + 20:
            recommendation = 'BUY'
            confidence = min(bullish_score, 95)
        elif bearish_score > bullish_score + 20:
            recommendation = 'SELL'
            confidence = min(bearish_score, 95)
        else:
            recommendation = 'HOLD'
            confidence = 50

        return {
            'recommendation': recommendation,
            'confidence': confidence,
            'bullish_score': bullish_score,
            'bearish_score': bearish_score,
        }

    def get_current_price(self, idx: int) -> float:
        """Get current price"""
        return self.df['close'].iloc[idx]

    def get_data_length(self) -> int:
        """Get data length"""
        return len(self.df)

    def get_structure_features(self, idx: int) -> Dict:
        """Get structure features"""
        return self.structure_detector.get_structure_features(self.df, idx)

    def get_session_features(self, idx: int) -> Dict:
        """Get session features"""
        if 'timestamp' in self.df.columns:
            timestamp = self.df['timestamp'].iloc[idx]
        else:
            timestamp = pd.Timestamp.now()

        return self.session_analyzer.get_session_features(timestamp)
