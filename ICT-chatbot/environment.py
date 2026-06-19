"""
ICT Trading Environment - Gym Compatible
Implements fixed risk management and ICT-based trading logic
"""
import gymnasium as gym
from gymnasium import spaces
import numpy as np
import pandas as pd
from typing import Optional, Tuple, Dict, List
from datetime import datetime, timedelta

from structures import (
    Position, Direction, BiasDirection, ExitReason,
    TradeRecord, EpisodeStats
)
from feature_extractor import FeatureExtractor
import config


class ICTTradingEnv(gym.Env):
    """
    Gym-compatible environment for ICT-based RL trading

    Single-position, fixed risk management, London session only
    """

    metadata = {'render.modes': ['human']}

    def __init__(
        self,
        ohlc_1m: pd.DataFrame,
        ohlc_15m: pd.DataFrame,
        initial_balance: float = config.INITIAL_BALANCE,
        risk_per_trade: float = config.RISK_PER_TRADE,
        commission: float = config.COMMISSION
    ):
        """
        Initialize trading environment

        Args:
            ohlc_1m: 1-minute OHLC DataFrame (GMT timezone)
            ohlc_15m: 15-minute OHLC DataFrame (GMT timezone)
            initial_balance: Starting account balance
            risk_per_trade: Risk per trade as decimal (0.01 = 1%)
            commission: Commission per round trip
        """
        super(ICTTradingEnv, self).__init__()

        # Data
        self.ohlc_1m = ohlc_1m
        self.ohlc_15m = ohlc_15m

        # Validate data alignment
        assert len(ohlc_1m) > config.LOOKBACK_1M, "Insufficient 1M data"
        assert len(ohlc_15m) > config.LOOKBACK_15M, "Insufficient 15M data"

        # Feature extractor
        self.feature_extractor = FeatureExtractor(ohlc_1m, ohlc_15m)

        # Risk management
        self.initial_balance = initial_balance
        self.risk_per_trade = risk_per_trade
        self.commission = commission

        # Gym spaces
        self.observation_space = spaces.Box(
            low=config.OBSERVATION_LOW,
            high=config.OBSERVATION_HIGH,
            shape=(config.OBSERVATION_DIM,),
            dtype=np.float32
        )

        self.action_space = spaces.Discrete(config.NUM_ACTIONS)
        # 0 = HOLD, 1 = LONG, 2 = SHORT

        # Episode state (will be initialized in reset())
        self.current_idx = 0
        self.episode_start_idx = 0
        self.step_count = 0

        # Account state
        self.balance = initial_balance
        self.equity = initial_balance
        self.peak_balance = initial_balance

        # Position state
        self.current_position: Optional[Position] = None
        self.trade_history: List[TradeRecord] = []

        # Session tracking
        self.trades_today = 0
        self.current_session_date = None

        # Episode statistics
        self.episode_stats = EpisodeStats()

    def reset(self) -> np.ndarray:
        """
        Reset environment to start new episode

        Returns:
            Initial observation
        """
        # Reset account
        self.balance = self.initial_balance
        self.equity = self.initial_balance
        self.peak_balance = self.initial_balance

        # Reset position
        self.current_position = None
        self.trade_history = []

        # Reset counters
        self.trades_today = 0
        self.current_session_date = None
        self.step_count = 0

        # Reset episode stats
        self.episode_stats = EpisodeStats()

        # Get random valid episode start
        valid_starts = self._get_valid_episode_starts()
        if len(valid_starts) == 0:
            raise ValueError("No valid episode start points found")

        self.episode_start_idx = np.random.choice(valid_starts)
        self.current_idx = self.episode_start_idx

        # Reset feature extractor
        self.feature_extractor.reset(self.episode_start_idx)

        # Update indicators for first bar
        self.feature_extractor.update_indicators(self.current_idx)

        # Get initial observation
        observation = self._get_observation()

        return observation

    def step(self, action: int) -> Tuple[np.ndarray, float, bool, dict]:
        """
        Execute one environment step

        Args:
            action: 0 (HOLD), 1 (LONG), 2 (SHORT)

        Returns:
            (observation, reward, done, info)
        """
        self.step_count += 1

        # Get current bar info
        current_bar = self.ohlc_1m.iloc[self.current_idx]
        current_time = current_bar.name

        # Check for new session (reset daily counter)
        self._check_new_session(current_time)

        # Validate action
        action_valid, rejection_reason = self._validate_action(action)

        if not action_valid:
            # Invalid action penalty
            reward = config.REWARD_INVALID_ACTION
            observation = self._get_observation()
            info = {
                'invalid_action': True,
                'rejection_reason': rejection_reason,
                'balance': self.balance,
                'equity': self.equity,
                'position': self.current_position.to_dict() if self.current_position else None,
                'trades_today': self.trades_today,
                'total_trades': len(self.trade_history),
                'timestamp': current_time,
            }
            return observation, reward, False, info

        # Execute action (if entering)
        if action == config.ACTION_LONG:
            self._enter_long()
        elif action == config.ACTION_SHORT:
            self._enter_short()

        # Update position tracking
        if self.current_position is not None:
            self._update_position(current_bar)

            # Check if position should be closed
            should_close, exit_reason = self._check_position_exit(current_bar)
            if should_close:
                self._close_position(exit_reason, current_bar)

        # Advance to next bar
        self.current_idx += 1

        # Check if episode should end
        done = self._check_episode_end()

        # Update indicators for next bar (if not done)
        if not done:
            self.feature_extractor.update_indicators(self.current_idx)

        # Calculate reward (placeholder - will be Phase 3)
        reward = self._calculate_reward(action, action_valid)

        # Get next observation
        observation = self._get_observation()

        # Info dict
        info = {
            'balance': self.balance,
            'equity': self.equity,
            'position': self.current_position.to_dict() if self.current_position else None,
            'trades_today': self.trades_today,
            'total_trades': len(self.trade_history),
            'timestamp': current_time,
        }

        return observation, reward, done, info

    def _validate_action(self, action: int) -> Tuple[bool, str]:
        """
        Validate if action is allowed

        Returns:
            (is_valid, rejection_reason)
        """
        current_bar = self.ohlc_1m.iloc[self.current_idx]
        current_time = current_bar.name

        # HOLD is always valid
        if action == config.ACTION_HOLD:
            return True, ""

        # Can only enter if no current position
        if self.current_position is not None:
            return False, "Already in position"

        # Must be in London Killzone
        if not self._in_london_killzone(current_time):
            return False, "Outside London Killzone"

        # Max trades per session
        if self.trades_today >= config.MAX_TRADES_PER_SESSION:
            return False, "Max trades per session reached"

        # Must have valid HTF bias
        if self.feature_extractor.htf_bias == BiasDirection.NEUTRAL:
            return False, "No HTF bias"

        return True, ""

    def _enter_long(self):
        """Enter long position with fixed risk management"""
        current_bar = self.ohlc_1m.iloc[self.current_idx]
        current_price = current_bar['close']
        current_time = current_bar.name

        atr_1m = self.feature_extractor.atr_1m.iloc[self.current_idx]
        if pd.isna(atr_1m) or atr_1m == 0:
            atr_1m = 0.0001

        # Find nearest bullish FVG
        nearest_fvg = self.feature_extractor.get_nearest_fvg('BULLISH', current_price)

        if nearest_fvg is None:
            # No valid FVG, use current price
            entry_price = current_price
            stop_loss = current_price - (1.0 * atr_1m)
        else:
            # Enter at bottom of FVG
            entry_price = nearest_fvg.bottom
            stop_loss = nearest_fvg.bottom - (1.0 * atr_1m)

        # Fixed 2:1 RR
        risk = entry_price - stop_loss
        take_profit = entry_price + (config.MIN_RISK_REWARD * risk)

        # Position sizing (1% risk)
        risk_amount = self.balance * self.risk_per_trade
        position_size = risk_amount / risk

        # Create position
        self.current_position = Position(
            direction=Direction.LONG,
            entry_price=entry_price,
            entry_time=current_time,
            entry_bar_idx=self.current_idx,
            stop_loss=stop_loss,
            take_profit=take_profit,
            position_size=position_size,
            risk_amount=risk_amount
        )

        # Deduct commission
        self.balance -= self.commission
        self.trades_today += 1

    def _enter_short(self):
        """Enter short position with fixed risk management"""
        current_bar = self.ohlc_1m.iloc[self.current_idx]
        current_price = current_bar['close']
        current_time = current_bar.name

        atr_1m = self.feature_extractor.atr_1m.iloc[self.current_idx]
        if pd.isna(atr_1m) or atr_1m == 0:
            atr_1m = 0.0001

        # Find nearest bearish FVG
        nearest_fvg = self.feature_extractor.get_nearest_fvg('BEARISH', current_price)

        if nearest_fvg is None:
            entry_price = current_price
            stop_loss = current_price + (1.0 * atr_1m)
        else:
            entry_price = nearest_fvg.top
            stop_loss = nearest_fvg.top + (1.0 * atr_1m)

        risk = stop_loss - entry_price
        take_profit = entry_price - (config.MIN_RISK_REWARD * risk)

        risk_amount = self.balance * self.risk_per_trade
        position_size = risk_amount / risk

        self.current_position = Position(
            direction=Direction.SHORT,
            entry_price=entry_price,
            entry_time=current_time,
            entry_bar_idx=self.current_idx,
            stop_loss=stop_loss,
            take_profit=take_profit,
            position_size=position_size,
            risk_amount=risk_amount
        )

        self.balance -= self.commission
        self.trades_today += 1

    def _update_position(self, current_bar: pd.Series):
        """Update position tracking"""
        if self.current_position is None:
            return

        current_price = current_bar['close']

        # Update unrealized PnL
        self.current_position.update_unrealized_pnl(current_price)

        # Update bars held
        self.current_position.bars_held += 1

        # Update equity
        self.equity = self.balance + self.current_position.unrealized_pnl

    def _check_position_exit(self, current_bar: pd.Series) -> Tuple[bool, Optional[ExitReason]]:
        """
        Check if position should be closed

        Returns:
            (should_close, exit_reason)
        """
        if self.current_position is None:
            return False, None

        pos = self.current_position

        # Check Stop Loss
        if pos.direction == Direction.LONG:
            if current_bar['low'] <= pos.stop_loss:
                return True, ExitReason.STOP_LOSS
        else:  # SHORT
            if current_bar['high'] >= pos.stop_loss:
                return True, ExitReason.STOP_LOSS

        # Check Take Profit
        if pos.direction == Direction.LONG:
            if current_bar['high'] >= pos.take_profit:
                return True, ExitReason.TAKE_PROFIT
        else:  # SHORT
            if current_bar['low'] <= pos.take_profit:
                return True, ExitReason.TAKE_PROFIT

        # Check Session End (force close at 10:00 GMT)
        current_time = current_bar.name
        if current_time.hour >= config.LONDON_KILLZONE_END:
            if pos.entry_time.hour < config.LONDON_KILLZONE_END:
                return True, ExitReason.SESSION_END

        # Check Maximum Hold Time
        if pos.bars_held >= config.MAX_HOLD_BARS:
            return True, ExitReason.MAX_HOLD

        return False, None

    def _close_position(self, exit_reason: ExitReason, current_bar: pd.Series):
        """Close current position and realize PnL"""
        if self.current_position is None:
            return

        pos = self.current_position

        # Determine exit price
        if exit_reason == ExitReason.STOP_LOSS:
            exit_price = pos.stop_loss
        elif exit_reason == ExitReason.TAKE_PROFIT:
            exit_price = pos.take_profit
        else:  # SESSION_END or MAX_HOLD
            exit_price = current_bar['close']

        # Calculate realized PnL
        if pos.direction == Direction.LONG:
            pnl = (exit_price - pos.entry_price) * pos.position_size
        else:  # SHORT
            pnl = (pos.entry_price - exit_price) * pos.position_size

        # Deduct commission
        pnl -= self.commission

        # Update balance
        self.balance += pnl
        self.equity = self.balance

        # Track peak for drawdown
        self.peak_balance = max(self.peak_balance, self.balance)

        # Calculate PnL in R multiples
        pnl_r = pnl / pos.risk_amount if pos.risk_amount > 0 else 0

        # Update position exit info
        pos.exit_price = exit_price
        pos.exit_time = current_bar.name
        pos.exit_reason = exit_reason
        pos.realized_pnl = pnl

        # Record trade
        trade_record = TradeRecord(
            entry_time=pos.entry_time,
            exit_time=pos.exit_time,
            direction=pos.direction.name,
            entry_price=pos.entry_price,
            exit_price=exit_price,
            exit_reason=exit_reason.value,
            pnl=pnl,
            pnl_r=pnl_r,
            balance_after=self.balance,
            bars_held=pos.bars_held,
            session_number=self._get_session_number(pos.entry_time)
        )
        self.trade_history.append(trade_record)

        # Update episode stats
        self.episode_stats.total_trades += 1
        self.episode_stats.total_pnl += pnl

        if pnl > 0:
            self.episode_stats.winning_trades += 1
        else:
            self.episode_stats.losing_trades += 1

        # Update max drawdown
        drawdown = (self.peak_balance - self.balance) / self.peak_balance
        self.episode_stats.max_drawdown = max(self.episode_stats.max_drawdown, drawdown)

        # Clear position
        self.current_position = None

    def _check_episode_end(self) -> bool:
        """Check if episode should terminate"""

        # Reached end of data
        if self.current_idx >= len(self.ohlc_1m) - 1:
            return True

        # Account blown (balance < 50%)
        if self.balance < self.initial_balance * config.ACCOUNT_STOP_PCT:
            return True

        # Max episode length
        if self.step_count >= config.EPISODE_LENGTH:
            return True

        return False

    def _get_observation(self) -> np.ndarray:
        """Extract current observation"""
        observation = self.feature_extractor.extract_features(
            current_idx_1m=self.current_idx,
            position=self.current_position,
            trades_today=self.trades_today
        )
        return observation

    def _calculate_reward(self, action: int, action_valid: bool) -> float:
        """
        Calculate reward (PLACEHOLDER - Phase 3 will implement fully)

        For now: simple PnL-based reward
        """
        if not action_valid:
            return config.REWARD_INVALID_ACTION

        # If position just closed, reward based on PnL
        if len(self.trade_history) > 0:
            last_trade = self.trade_history[-1]
            # Check if this trade was just closed this step
            if last_trade.exit_time == self.ohlc_1m.iloc[self.current_idx - 1].name:
                # Reward in R multiples
                return last_trade.pnl_r * 10  # Scale factor

        # Small reward for patience (holding)
        if action == config.ACTION_HOLD:
            return 0.1

        return 0.0

    def _get_valid_episode_starts(self) -> List[int]:
        """
        Get valid episode start indices

        Requirements:
        - Sufficient lookback
        - Start at beginning of London session
        - Sufficient lookahead
        """
        valid_starts = []

        min_lookback = config.LOOKBACK_1M
        min_lookahead = config.EPISODE_LENGTH

        for idx in range(min_lookback, len(self.ohlc_1m) - min_lookahead):
            timestamp = self.ohlc_1m.index[idx]

            # Check if 07:00 GMT (session start)
            if timestamp.hour == config.LONDON_KILLZONE_START and timestamp.minute == 0:
                valid_starts.append(idx)

        return valid_starts

    def _in_london_killzone(self, timestamp: datetime) -> bool:
        """Check if timestamp is in London Killzone"""
        return config.LONDON_KILLZONE_START <= timestamp.hour < config.LONDON_KILLZONE_END

    def _check_new_session(self, current_time: datetime):
        """Check if new session started and reset daily counters"""
        current_date = current_time.date()

        if self.current_session_date is None:
            self.current_session_date = current_date
        elif current_date != self.current_session_date:
            # New day
            self.trades_today = 0
            self.current_session_date = current_date

    def _get_session_number(self, timestamp: datetime) -> int:
        """Get session number since episode start"""
        days_since_start = (timestamp.date() - self.ohlc_1m.index[self.episode_start_idx].date()).days
        return days_since_start

    def render(self, mode='human'):
        """Render environment state"""
        if mode == 'human':
            current_bar = self.ohlc_1m.iloc[self.current_idx]
            print(f"\n{'='*60}")
            print(f"Time: {current_bar.name}")
            print(f"Balance: ${self.balance:.2f} | Equity: ${self.equity:.2f}")
            print(f"Trades Today: {self.trades_today}/{config.MAX_TRADES_PER_SESSION}")
            print(f"Total Trades: {self.episode_stats.total_trades}")

            if self.current_position:
                print(f"\nPosition: {self.current_position.direction.name}")
                print(f"  Entry: ${self.current_position.entry_price:.5f}")
                print(f"  Current: ${current_bar['close']:.5f}")
                print(f"  Unrealized PnL: ${self.current_position.unrealized_pnl:.2f} "
                      f"({self.current_position.get_risk_multiple():.2f}R)")
                print(f"  Bars Held: {self.current_position.bars_held}")
            else:
                print(f"\nPosition: None")

            print(f"\nHTF Bias: {self.feature_extractor.htf_bias.name}")
            print(f"Active FVGs: {len(self.feature_extractor.active_fvgs)}")
            print(f"{'='*60}\n")

    def get_episode_stats(self) -> dict:
        """Get episode statistics"""
        self.episode_stats.final_balance = self.balance

        return {
            'total_trades': self.episode_stats.total_trades,
            'winning_trades': self.episode_stats.winning_trades,
            'losing_trades': self.episode_stats.losing_trades,
            'win_rate': self.episode_stats.win_rate(),
            'total_pnl': self.episode_stats.total_pnl,
            'final_balance': self.episode_stats.final_balance,
            'max_drawdown': self.episode_stats.max_drawdown,
            'return_pct': ((self.balance - self.initial_balance) / self.initial_balance) * 100,
        }
