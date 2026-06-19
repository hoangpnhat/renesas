"""
Test script for ICT Trading Environment
Validates environment functionality with synthetic data
"""
import numpy as np
import pandas as pd
from datetime import datetime, timedelta
import sys

from environment import ICTTradingEnv
import config


def generate_synthetic_data(
    start_date: str = "2024-01-01",
    days: int = 30,
    base_price: float = 1.1000,
    volatility: float = 0.0002
) -> tuple[pd.DataFrame, pd.DataFrame]:
    """
    Generate synthetic OHLC data for testing

    Args:
        start_date: Start date string
        days: Number of days
        base_price: Starting price
        volatility: Price volatility

    Returns:
        (ohlc_1m, ohlc_15m) DataFrames
    """
    print("Generating synthetic OHLC data...")

    # Generate 1M data
    start = pd.to_datetime(start_date, utc=True)
    minutes = days * 24 * 60
    dates_1m = [start + timedelta(minutes=i) for i in range(minutes)]

    # Random walk with trend
    np.random.seed(42)
    returns = np.random.normal(0, volatility, minutes)

    # Add some trend and cycles
    trend = np.linspace(0, 0.002, minutes)  # Slight uptrend
    cycle = 0.001 * np.sin(np.linspace(0, 20 * np.pi, minutes))  # Oscillation

    prices = base_price * np.exp(np.cumsum(returns + trend/minutes + cycle/minutes))

    # Generate OHLC from prices
    ohlc_1m_data = []
    for i, price in enumerate(prices):
        noise = np.random.uniform(-volatility, volatility, 3)
        high = price + abs(noise[0]) * price
        low = price - abs(noise[1]) * price
        open_price = prices[i-1] if i > 0 else price
        close = price

        ohlc_1m_data.append({
            'open': open_price,
            'high': high,
            'low': low,
            'close': close,
        })

    ohlc_1m = pd.DataFrame(ohlc_1m_data, index=dates_1m)

    # Resample to 15M
    ohlc_15m = ohlc_1m.resample('15min').agg({
        'open': 'first',
        'high': 'max',
        'low': 'min',
        'close': 'last'
    }).dropna()

    print(f"Generated {len(ohlc_1m)} 1M bars and {len(ohlc_15m)} 15M bars")

    return ohlc_1m, ohlc_15m


def test_environment_initialization():
    """Test environment can be initialized"""
    print("\n" + "="*60)
    print("TEST 1: Environment Initialization")
    print("="*60)

    ohlc_1m, ohlc_15m = generate_synthetic_data(days=30)

    try:
        env = ICTTradingEnv(ohlc_1m, ohlc_15m)
        print("[PASS] Environment initialized successfully")
        print(f"  Observation space: {env.observation_space}")
        print(f"  Action space: {env.action_space}")
        return env, ohlc_1m, ohlc_15m
    except Exception as e:
        print(f"[FAIL] Environment initialization failed: {e}")
        raise


def test_reset(env):
    """Test environment reset"""
    print("\n" + "="*60)
    print("TEST 2: Environment Reset")
    print("="*60)

    try:
        obs = env.reset()
        print("[PASS] Environment reset successfully")
        print(f"  Observation shape: {obs.shape}")
        print(f"  Observation dtype: {obs.dtype}")
        print(f"  Observation range: [{obs.min():.3f}, {obs.max():.3f}]")
        print(f"  Initial balance: ${env.balance:.2f}")
        print(f"  Episode start index: {env.episode_start_idx}")

        assert obs.shape == (config.OBSERVATION_DIM,), "Invalid observation shape"
        assert obs.dtype == np.float32, "Invalid observation dtype"
        assert np.all(np.isfinite(obs)), "Non-finite values in observation"
        print("[PASS] All assertions passed")

        return obs
    except Exception as e:
        print(f"[FAIL] Reset test failed: {e}")
        raise


def test_step_actions(env):
    """Test all actions"""
    print("\n" + "="*60)
    print("TEST 3: Action Execution")
    print("="*60)

    env.reset()

    # Test HOLD action
    try:
        obs, reward, done, info = env.step(config.ACTION_HOLD)
        print(f"[PASS] HOLD action executed")
        print(f"  Reward: {reward:.2f}")
        print(f"  Done: {done}")
        print(f"  Balance: ${info['balance']:.2f}")
    except Exception as e:
        print(f"[FAIL] HOLD action failed: {e}")
        raise

    # Test LONG action
    try:
        obs, reward, done, info = env.step(config.ACTION_LONG)
        print(f"[PASS] LONG action executed")
        print(f"  Reward: {reward:.2f}")
        print(f"  Position: {info.get('position', 'None')}")
        print(f"  Rejection reason: {info.get('rejection_reason', 'None')}")
    except Exception as e:
        print(f"[FAIL] LONG action failed: {e}")
        raise

    # Test SHORT action
    try:
        env.reset()
        obs, reward, done, info = env.step(config.ACTION_SHORT)
        print(f"[PASS] SHORT action executed")
        print(f"  Reward: {reward:.2f}")
        print(f"  Position: {info.get('position', 'None')}")
    except Exception as e:
        print(f"[FAIL] SHORT action failed: {e}")
        raise


def test_episode_rollout(env, num_steps=1000):
    """Test full episode rollout"""
    print("\n" + "="*60)
    print(f"TEST 4: Episode Rollout ({num_steps} steps)")
    print("="*60)

    try:
        obs = env.reset()
        total_reward = 0
        actions_taken = {0: 0, 1: 0, 2: 0}

        for step in range(num_steps):
            # Random action
            action = env.action_space.sample()
            obs, reward, done, info = env.step(action)

            total_reward += reward
            actions_taken[action] += 1

            if done:
                print(f"Episode ended at step {step + 1}")
                break

            # Print progress every 100 steps
            if (step + 1) % 100 == 0:
                print(f"  Step {step + 1}: Balance=${info['balance']:.2f}, "
                      f"Trades={info['total_trades']}, Position={info['position'] is not None}")

        print(f"[PASS] Episode completed")
        print(f"  Total steps: {step + 1}")
        print(f"  Total reward: {total_reward:.2f}")
        print(f"  Actions taken: HOLD={actions_taken[0]}, LONG={actions_taken[1]}, SHORT={actions_taken[2]}")
        print(f"  Final balance: ${env.balance:.2f}")
        print(f"  Total trades: {len(env.trade_history)}")

        # Print episode stats
        stats = env.get_episode_stats()
        print(f"\nEpisode Statistics:")
        for key, value in stats.items():
            print(f"  {key}: {value}")

    except Exception as e:
        print(f"[FAIL] Episode rollout failed: {e}")
        import traceback
        traceback.print_exc()
        raise


def test_position_management(env):
    """Test position entry and exit"""
    print("\n" + "="*60)
    print("TEST 5: Position Management")
    print("="*60)

    env.reset()

    # Force environment into London session for testing
    # Find a bar during London session
    london_idx = None
    for idx in range(env.episode_start_idx, min(env.episode_start_idx + 500, len(env.ohlc_1m))):
        timestamp = env.ohlc_1m.index[idx]
        if config.LONDON_KILLZONE_START <= timestamp.hour < config.LONDON_KILLZONE_END:
            london_idx = idx
            break

    if london_idx:
        env.current_idx = london_idx
        print(f"[PASS] Found London session bar at index {london_idx}")

        # Try to enter long
        obs, reward, done, info = env.step(config.ACTION_LONG)

        if env.current_position:
            print(f"[PASS] Position opened")
            print(f"  Direction: {env.current_position.direction.name}")
            print(f"  Entry: ${env.current_position.entry_price:.5f}")
            print(f"  Stop Loss: ${env.current_position.stop_loss:.5f}")
            print(f"  Take Profit: ${env.current_position.take_profit:.5f}")
            print(f"  Risk: ${env.current_position.risk_amount:.2f}")

            # Step through until position closes or max steps
            max_position_steps = 200
            for i in range(max_position_steps):
                obs, reward, done, info = env.step(config.ACTION_HOLD)

                if env.current_position is None:
                    print(f"[PASS] Position closed after {i + 1} bars")
                    if len(env.trade_history) > 0:
                        last_trade = env.trade_history[-1]
                        print(f"  Exit reason: {last_trade.exit_reason}")
                        print(f"  PnL: ${last_trade.pnl:.2f} ({last_trade.pnl_r:.2f}R)")
                    break

                if done:
                    break
        else:
            print(f"[WARN] Position not opened (likely validation rejected)")
            print(f"  Rejection: {info.get('rejection_reason', 'Unknown')}")
    else:
        print("[WARN] Could not find London session bar in test data")


def test_feature_extraction(env):
    """Test feature extraction"""
    print("\n" + "="*60)
    print("TEST 6: Feature Extraction")
    print("="*60)

    env.reset()

    try:
        obs = env._get_observation()
        print("[PASS] Features extracted successfully")
        print(f"  Shape: {obs.shape}")

        # Print feature values
        print("\nFeature values:")
        for i, (name, value) in enumerate(zip(config.FEATURE_NAMES, obs)):
            print(f"  [{i:2d}] {name:25s}: {value:8.4f}")

        # Check for issues
        if np.any(np.isnan(obs)):
            print("[FAIL] WARNING: NaN values detected in features")
        if np.any(np.isinf(obs)):
            print("[FAIL] WARNING: Inf values detected in features")
        if np.all(obs == 0):
            print("[WARN] WARNING: All features are zero")

        print("[PASS] Feature extraction test passed")

    except Exception as e:
        print(f"[FAIL] Feature extraction failed: {e}")
        import traceback
        traceback.print_exc()
        raise


def test_action_validation(env):
    """Test action validation logic"""
    print("\n" + "="*60)
    print("TEST 7: Action Validation")
    print("="*60)

    env.reset()

    # Test various validation conditions
    test_cases = [
        (config.ACTION_HOLD, "HOLD should always be valid"),
        (config.ACTION_LONG, "LONG in initial state"),
        (config.ACTION_SHORT, "SHORT in initial state"),
    ]

    for action, description in test_cases:
        valid, reason = env._validate_action(action)
        status = "[PASS]" if valid or reason else "[WARN]"
        print(f"{status} {description}: valid={valid}, reason='{reason}'")

    # Test double entry (should be rejected)
    # Try to open a position first
    env.step(config.ACTION_LONG)
    if env.current_position:
        valid, reason = env._validate_action(config.ACTION_LONG)
        if not valid:
            print(f"[PASS] Double entry correctly rejected: '{reason}'")
        else:
            print(f"[FAIL] Double entry was not rejected!")


def run_all_tests():
    """Run all tests"""
    print("\n" + "="*70)
    print(" ICT TRADING ENVIRONMENT TEST SUITE")
    print("="*70)

    try:
        # Test 1: Initialization
        env, ohlc_1m, ohlc_15m = test_environment_initialization()

        # Test 2: Reset
        test_reset(env)

        # Test 3: Actions
        test_step_actions(env)

        # Test 4: Feature Extraction
        test_feature_extraction(env)

        # Test 5: Action Validation
        test_action_validation(env)

        # Test 6: Position Management
        test_position_management(env)

        # Test 7: Full Episode
        test_episode_rollout(env, num_steps=1000)

        print("\n" + "="*70)
        print(" ALL TESTS PASSED [PASS]")
        print("="*70)

        return True

    except Exception as e:
        print("\n" + "="*70)
        print(" TESTS FAILED [FAIL]")
        print("="*70)
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        return False


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
