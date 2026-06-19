"""
ICT Trading System - Session Analyzer
======================================
Trading session analysis: London, New York, Asian Kill Zones
"""

import pandas as pd
import numpy as np
from typing import Dict, List, Optional
from dataclasses import dataclass
from datetime import datetime, time


@dataclass
class TradingSession:
    """Trading session information"""
    name: str
    start_time: time  # UTC
    end_time: time  # UTC
    is_active: bool
    killzone: bool  # Is this a kill zone?
    priority: int  # 1-10


class SessionAnalyzer:
    """Analyze trading sessions and kill zones"""

    def __init__(self):
        # Define ICT Kill Zones (all times in UTC)
        self.sessions = {
            'asian_killzone': TradingSession(
                name='Asian Kill Zone',
                start_time=time(0, 0),   # 00:00 UTC
                end_time=time(3, 0),     # 03:00 UTC
                is_active=False,
                killzone=True,
                priority=6
            ),
            'london_open': TradingSession(
                name='London Open',
                start_time=time(2, 0),   # 02:00 UTC
                end_time=time(5, 0),     # 05:00 UTC
                is_active=False,
                killzone=True,
                priority=9
            ),
            'london_killzone': TradingSession(
                name='London Kill Zone',
                start_time=time(6, 0),   # 06:00 UTC
                end_time=time(9, 0),     # 09:00 UTC
                is_active=False,
                killzone=True,
                priority=8
            ),
            'ny_am_killzone': TradingSession(
                name='New York AM Kill Zone',
                start_time=time(12, 0),  # 12:00 UTC (8am EST)
                end_time=time(15, 0),    # 15:00 UTC (11am EST)
                is_active=False,
                killzone=True,
                priority=10  # Highest priority
            ),
            'ny_lunch': TradingSession(
                name='NY Lunch',
                start_time=time(16, 0),  # 16:00 UTC
                end_time=time(18, 0),    # 18:00 UTC
                is_active=False,
                killzone=False,
                priority=3
            ),
            'ny_pm_killzone': TradingSession(
                name='New York PM Kill Zone',
                start_time=time(18, 30),  # 18:30 UTC (2:30pm EST)
                end_time=time(21, 0),     # 21:00 UTC (5pm EST)
                is_active=False,
                killzone=True,
                priority=7
            ),
        }

        self.current_session = None
        self.session_stats = {}

    def get_current_session(self, timestamp: pd.Timestamp) -> Optional[TradingSession]:
        """Determine which session is currently active"""
        current_time = timestamp.time()

        for session_name, session in self.sessions.items():
            if self._is_time_in_session(current_time, session.start_time, session.end_time):
                session.is_active = True
                self.current_session = session
                return session
            else:
                session.is_active = False

        return None

    def _is_time_in_session(self, current: time, start: time, end: time) -> bool:
        """Check if current time is within session"""
        if start <= end:
            return start <= current <= end
        else:  # Session crosses midnight
            return current >= start or current <= end

    def analyze_session_behavior(self, df: pd.DataFrame) -> Dict:
        """Analyze price behavior during different sessions"""
        if 'timestamp' not in df.columns:
            # Try to use index if it's datetime
            if isinstance(df.index, pd.DatetimeIndex):
                df = df.copy()
                df['timestamp'] = df.index
            else:
                return self._default_session_features()

        df = df.copy()

        # Mark each row with its session
        df['session'] = df['timestamp'].apply(lambda x: self._get_session_name(x))

        # Calculate session statistics
        stats = {}
        for session_name in self.sessions.keys():
            session_data = df[df['session'] == session_name]

            if len(session_data) > 0:
                # Calculate volatility, volume, direction
                session_range = (session_data['high'].max() - session_data['low'].min())
                avg_range = session_range / len(session_data) if len(session_data) > 0 else 0

                price_change = session_data['close'].iloc[-1] - session_data['open'].iloc[0]
                direction = 1 if price_change > 0 else (-1 if price_change < 0 else 0)

                avg_volume = session_data['volume'].mean() if 'volume' in session_data.columns else 0

                stats[session_name] = {
                    'avg_range': avg_range,
                    'direction': direction,
                    'avg_volume': avg_volume,
                    'candle_count': len(session_data),
                    'volatility': session_data['close'].std() if len(session_data) > 1 else 0
                }
            else:
                stats[session_name] = {
                    'avg_range': 0,
                    'direction': 0,
                    'avg_volume': 0,
                    'candle_count': 0,
                    'volatility': 0
                }

        self.session_stats = stats
        return stats

    def _get_session_name(self, timestamp: pd.Timestamp) -> str:
        """Get session name for a timestamp"""
        current_time = timestamp.time()

        for session_name, session in self.sessions.items():
            if self._is_time_in_session(current_time, session.start_time, session.end_time):
                return session_name

        return 'other'

    def get_session_features(self, timestamp: pd.Timestamp) -> Dict:
        """Get session features for current timestamp"""
        current_session = self.get_current_session(timestamp)

        features = {
            'is_killzone': 0,
            'session_priority': 0,
            'asian_killzone': 0,
            'london_open': 0,
            'london_killzone': 0,
            'ny_am_killzone': 0,
            'ny_lunch': 0,
            'ny_pm_killzone': 0,
            'hour_of_day': timestamp.hour,
            'day_of_week': timestamp.dayofweek,
            'is_weekend': 1 if timestamp.dayofweek >= 5 else 0,
        }

        if current_session:
            features['is_killzone'] = 1 if current_session.killzone else 0
            features['session_priority'] = current_session.priority / 10.0

            # One-hot encode session
            session_key = [k for k, v in self.sessions.items() if v == current_session][0]
            features[session_key] = 1

        return features

    def get_optimal_trading_time(self, timestamp: pd.Timestamp) -> Dict:
        """Determine if current time is optimal for trading"""
        current_session = self.get_current_session(timestamp)

        if not current_session:
            return {
                'is_optimal': False,
                'quality': 0,
                'reason': 'Outside major trading sessions'
            }

        # Kill zones are optimal
        if current_session.killzone:
            quality = current_session.priority / 10.0

            return {
                'is_optimal': True,
                'quality': quality,
                'reason': f'{current_session.name} is active'
            }
        else:
            return {
                'is_optimal': False,
                'quality': 0.3,
                'reason': f'{current_session.name} - low activity period'
            }

    def analyze_day_structure(self, df: pd.DataFrame, current_idx: int) -> Dict:
        """Analyze the current day's price structure"""
        if 'timestamp' not in df.columns:
            return self._default_day_structure()

        current_row = df.iloc[current_idx]
        current_date = current_row['timestamp'].date()

        # Get today's data
        today_data = df[df['timestamp'].dt.date == current_date]

        if len(today_data) == 0:
            return self._default_day_structure()

        # Asian session low/high
        asian_data = today_data[today_data['timestamp'].dt.hour < 8]
        asian_low = asian_data['low'].min() if len(asian_data) > 0 else None
        asian_high = asian_data['high'].max() if len(asian_data) > 0 else None

        # London session low/high
        london_data = today_data[(today_data['timestamp'].dt.hour >= 2) & (today_data['timestamp'].dt.hour < 12)]
        london_low = london_data['low'].min() if len(london_data) > 0 else None
        london_high = london_data['high'].max() if len(london_data) > 0 else None

        # NY session low/high
        ny_data = today_data[today_data['timestamp'].dt.hour >= 12]
        ny_low = ny_data['low'].min() if len(ny_data) > 0 else None
        ny_high = ny_data['high'].max() if len(ny_data) > 0 else None

        # Current price relative to session ranges
        current_price = current_row['close']

        structure = {
            'asian_low': asian_low,
            'asian_high': asian_high,
            'london_low': london_low,
            'london_high': london_high,
            'ny_low': ny_low,
            'ny_high': ny_high,
            'day_low': today_data['low'].min(),
            'day_high': today_data['high'].max(),
            'current_price': current_price,
        }

        # Calculate position in day's range
        day_range = structure['day_high'] - structure['day_low']
        if day_range > 0:
            structure['position_in_day'] = (current_price - structure['day_low']) / day_range
        else:
            structure['position_in_day'] = 0.5

        return structure

    def _default_session_features(self) -> Dict:
        """Default session features when timestamp not available"""
        return {
            'is_killzone': 0,
            'session_priority': 0.5,
            'asian_killzone': 0,
            'london_open': 0,
            'london_killzone': 0,
            'ny_am_killzone': 0,
            'ny_lunch': 0,
            'ny_pm_killzone': 0,
            'hour_of_day': 12,
            'day_of_week': 2,
            'is_weekend': 0,
        }

    def _default_day_structure(self) -> Dict:
        """Default day structure when data not available"""
        return {
            'asian_low': None,
            'asian_high': None,
            'london_low': None,
            'london_high': None,
            'ny_low': None,
            'ny_high': None,
            'day_low': None,
            'day_high': None,
            'current_price': None,
            'position_in_day': 0.5
        }

    def get_session_vector_features(self, timestamp: pd.Timestamp) -> np.ndarray:
        """Get session features as vector for ML"""
        features = self.get_session_features(timestamp)

        vector = np.array([
            features['is_killzone'],
            features['session_priority'],
            features['asian_killzone'],
            features['london_open'],
            features['london_killzone'],
            features['ny_am_killzone'],
            features['ny_lunch'],
            features['ny_pm_killzone'],
            features['hour_of_day'] / 24.0,
            features['day_of_week'] / 6.0,
            features['is_weekend'],
        ])

        return vector
