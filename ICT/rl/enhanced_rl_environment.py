"""
ICT Trading System - Enhanced RL Environment
=============================================
Advanced RL environment with 100+ dimensional state space integrating all ICT features
"""

import numpy as np
import pandas as pd
import gymnasium as gym
from gymnasium import spaces
from typing import Dict, Optional
from stable_baselines3.common.callbacks import BaseCallback


class EnhancedICTTradingEnv(gym.Env):
    """Enhanced RL environment with comprehensive ICT features"""

    def __init__(self, feature_extractor, initial_balance: float = 10000,
                 teacher_weight: float = 1.0):
        """
        Args:
            feature_extractor: FeatureExtractionPipeline instance
            initial_balance: Starting capital
            teacher_weight: Weight for teacher guidance (0-1)
        """
        super().__init__()

        self.feature_extractor = feature_extractor
        self.initial_balance = initial_balance
        self.teacher_weight = teacher_weight

        # Action space: 0=Hold, 1=Buy, 2=Sell
        self.action_space = spaces.Discrete(3)

        # Observation space: 100+ features
        # Dynamically calculate based on feature extractor output
        sample_features = self.feature_extractor.extract_features(0)
        feature_dim = len(sample_features)

        print(f"✅ Enhanced RL Environment initialized with {feature_dim} features")

        self.observation_space = spaces.Box(
            low=-np.inf, high=np.inf, shape=(feature_dim,), dtype=np.float32
        )

        # Trading state
        self.current_step = 0
        self.balance = initial_balance
        self.position = 0  # -1=short, 0=none, 1=long
        self.entry_price = 0
        self.total_profit = 0
        self.num_trades = 0
        self.winning_trades = 0

        # Episode history
        self.episode_rewards = []
        self.episode_actions = []

    def reset(self, seed=None):
        """Reset environment"""
        super().reset(seed=seed)

        # Reset to start of data (skip initial lookback)
        self.current_step = 50
        self.balance = self.initial_balance
        self.position = 0
        self.entry_price = 0
        self.total_profit = 0
        self.num_trades = 0
        self.winning_trades = 0

        self.episode_rewards = []
        self.episode_actions = []

        return self._get_observation(), {}

    def _get_observation(self) -> np.ndarray:
        """Get current state observation with all ICT features"""
        # Extract all features from feature extraction pipeline
        features = self.feature_extractor.extract_features(self.current_step)

        # Add position and portfolio features
        position_features = np.array([
            float(self.position),  # -1, 0, or 1
            self.balance / self.initial_balance,  # Normalized balance
            self.total_profit,
            float(self.entry_price) / 10000.0 if self.entry_price > 0 else 0.0,
            self.teacher_weight,
        ])

        # Combine all features
        full_observation = np.concatenate([features, position_features])

        return full_observation.astype(np.float32)

    def _get_teacher_signal(self) -> Dict:
        """Get combined teacher signal from ICT rules and LLM"""
        analysis = self.feature_extractor.get_trading_signal(self.current_step)

        signal = analysis.get('recommendation', 'HOLD')
        confidence = analysis.get('confidence', 50) / 100.0

        if signal == 'BUY':
            return {'action': 1, 'confidence': confidence}
        elif signal == 'SELL':
            return {'action': 2, 'confidence': confidence}
        else:
            return {'action': 0, 'confidence': confidence}

    def _calculate_reward(self, action: int) -> float:
        """Calculate reward with teacher guidance"""
        current_price = self.feature_extractor.get_current_price(self.current_step)

        # 1. PnL Reward
        pnl_reward = 0.0

        if action == 1 and self.position == 0:  # Buy
            # Opening long position
            self.position = 1
            self.entry_price = current_price
            self.num_trades += 1
            pnl_reward = -0.001  # Small penalty for transaction cost

        elif action == 2 and self.position == 0:  # Sell
            # Opening short position
            self.position = -1
            self.entry_price = current_price
            self.num_trades += 1
            pnl_reward = -0.001

        elif action == 0 and self.position != 0:  # Close position
            # Closing position
            if self.position == 1:  # Close long
                profit = (current_price - self.entry_price) / self.entry_price
            else:  # Close short
                profit = (self.entry_price - current_price) / self.entry_price

            self.balance += self.balance * profit
            self.total_profit += profit

            if profit > 0:
                self.winning_trades += 1

            # Scale PnL reward
            pnl_reward = profit * 100  # Scale up

            self.position = 0
            self.entry_price = 0

        # 2. Unrealized PnL for open positions
        if self.position != 0:
            if self.position == 1:
                unrealized = (current_price - self.entry_price) / self.entry_price
            else:
                unrealized = (self.entry_price - current_price) / self.entry_price

            pnl_reward += unrealized * 0.1  # Small reward for unrealized gains

        # 3. Teacher Reward (curriculum learning)
        teacher_signal = self._get_teacher_signal()
        teacher_action = teacher_signal['action']
        teacher_confidence = teacher_signal['confidence']

        # Reward for following teacher
        if action == teacher_action:
            teacher_reward = 1.0 * teacher_confidence * self.teacher_weight
        else:
            teacher_reward = -0.5 * teacher_confidence * self.teacher_weight

        # 4. Structure Alignment Reward
        structure_features = self.feature_extractor.get_structure_features(self.current_step)
        structure_alignment = structure_features.get('structure_alignment', 0.0)

        structure_reward = 0.0
        if action == 1 and structure_alignment > 0.3:  # Buy with bullish structure
            structure_reward = 0.3
        elif action == 2 and structure_alignment < -0.3:  # Sell with bearish structure
            structure_reward = 0.3

        # 5. Session Quality Reward
        session_features = self.feature_extractor.get_session_features(self.current_step)
        is_killzone = session_features.get('is_killzone', 0)

        session_reward = 0.0
        if action != 0 and is_killzone:  # Taking action during kill zone
            session_reward = 0.2

        # 6. Risk Management Reward
        risk_reward = 0.0

        # Penalty for overtrading
        if self.num_trades > 20 and self.current_step < 100:
            risk_reward = -0.5

        # Reward for good win rate
        if self.num_trades > 5:
            win_rate = self.winning_trades / self.num_trades
            if win_rate > 0.6:
                risk_reward += 0.5

        # Total reward
        total_reward = (
            pnl_reward * 5.0 +  # Most important
            teacher_reward * 1.0 +
            structure_reward * 0.5 +
            session_reward * 0.3 +
            risk_reward * 0.2
        )

        return total_reward

    def step(self, action: int):
        """Execute one step"""
        # Calculate reward
        reward = self._calculate_reward(action)

        # Record
        self.episode_rewards.append(reward)
        self.episode_actions.append(action)

        # Move to next step
        self.current_step += 1

        # Check if episode is done
        done = (
            self.current_step >= self.feature_extractor.get_data_length() - 1 or
            self.balance < self.initial_balance * 0.5  # Stop if lost 50%
        )

        truncated = False

        # Get next observation
        obs = self._get_observation()

        # Info dict
        info = {
            'balance': self.balance,
            'position': self.position,
            'total_profit': self.total_profit,
            'num_trades': self.num_trades,
            'win_rate': self.winning_trades / max(self.num_trades, 1),
        }

        return obs, reward, done, truncated, info

    def get_episode_statistics(self) -> Dict:
        """Get statistics for the completed episode"""
        return {
            'total_reward': sum(self.episode_rewards),
            'avg_reward': np.mean(self.episode_rewards) if self.episode_rewards else 0,
            'final_balance': self.balance,
            'total_profit': self.total_profit,
            'num_trades': self.num_trades,
            'win_rate': self.winning_trades / max(self.num_trades, 1),
            'max_reward': max(self.episode_rewards) if self.episode_rewards else 0,
            'min_reward': min(self.episode_rewards) if self.episode_rewards else 0,
        }


class CurriculumLearningCallback(BaseCallback):
    """Curriculum learning: gradually reduce teacher weight"""

    def __init__(self, initial_weight: float = 1.0, final_weight: float = 0.1,
                 decay_steps: int = 100000, verbose: int = 0):
        super().__init__(verbose)
        self.initial_weight = initial_weight
        self.final_weight = final_weight
        self.decay_steps = decay_steps

    def _on_step(self) -> bool:
        """Update teacher weight at each step"""
        progress = min(self.num_timesteps / self.decay_steps, 1.0)
        current_weight = self.initial_weight - (self.initial_weight - self.final_weight) * progress

        # Update environment teacher weight
        if hasattr(self.training_env, 'envs'):
            for env in self.training_env.envs:
                if hasattr(env, 'teacher_weight'):
                    env.teacher_weight = current_weight
        elif hasattr(self.training_env, 'teacher_weight'):
            self.training_env.teacher_weight = current_weight

        # Log periodically
        if self.num_timesteps % 1000 == 0 and self.verbose > 0:
            print(f"Step {self.num_timesteps}: Teacher weight = {current_weight:.3f}")

        return True


class PerformanceMonitorCallback(BaseCallback):
    """Monitor and log training performance"""

    def __init__(self, check_freq: int = 1000, verbose: int = 1):
        super().__init__(verbose)
        self.check_freq = check_freq
        self.episode_rewards = []
        self.episode_lengths = []

    def _on_step(self) -> bool:
        """Check performance periodically"""
        if self.n_calls % self.check_freq == 0:
            # Get recent episode rewards from info
            if len(self.model.ep_info_buffer) > 0:
                recent_rewards = [ep_info['r'] for ep_info in self.model.ep_info_buffer]
                recent_lengths = [ep_info['l'] for ep_info in self.model.ep_info_buffer]

                avg_reward = np.mean(recent_rewards)
                avg_length = np.mean(recent_lengths)

                if self.verbose > 0:
                    print(f"\n{'='*60}")
                    print(f"Step {self.num_timesteps}")
                    print(f"Avg Episode Reward: {avg_reward:.2f}")
                    print(f"Avg Episode Length: {avg_length:.0f}")
                    print(f"{'='*60}\n")

        return True


class EarlyStoppingCallback(BaseCallback):
    """Stop training if performance doesn't improve"""

    def __init__(self, check_freq: int = 5000, patience: int = 3,
                 min_reward_threshold: float = 0.0, verbose: int = 1):
        super().__init__(verbose)
        self.check_freq = check_freq
        self.patience = patience
        self.min_reward_threshold = min_reward_threshold
        self.best_mean_reward = -np.inf
        self.no_improvement_count = 0

    def _on_step(self) -> bool:
        """Check if training should stop"""
        if self.n_calls % self.check_freq == 0:
            if len(self.model.ep_info_buffer) > 0:
                recent_rewards = [ep_info['r'] for ep_info in self.model.ep_info_buffer]
                mean_reward = np.mean(recent_rewards)

                if mean_reward > self.best_mean_reward:
                    self.best_mean_reward = mean_reward
                    self.no_improvement_count = 0

                    if self.verbose > 0:
                        print(f"✅ New best mean reward: {mean_reward:.2f}")
                else:
                    self.no_improvement_count += 1

                    if self.verbose > 0:
                        print(f"⚠️  No improvement for {self.no_improvement_count} checks")

                # Early stopping
                if self.no_improvement_count >= self.patience:
                    if self.verbose > 0:
                        print(f"\n🛑 Early stopping triggered after {self.no_improvement_count} checks without improvement")
                    return False

        return True
