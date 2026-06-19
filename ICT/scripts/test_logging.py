"""
ICT Trading System - Logging Test Script
=========================================
Demonstrates all logging features
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import time
import numpy as np
from utils import (
    get_logger,
    get_trading_logger,
    get_rl_logger,
    get_tensorboard_logger,
    log_trade,
    log_signal,
    log_error,
    log_performance
)


def test_basic_logging():
    """Test basic component logging"""
    print("\n" + "="*80)
    print("TEST 1: Basic Component Logging")
    print("="*80)

    logger = get_logger('TestComponent')

    logger.debug('This is a DEBUG message - detailed diagnostic info')
    logger.info('This is an INFO message - general information')
    logger.warning('This is a WARNING message - potential issue')
    logger.error('This is an ERROR message - something went wrong')

    print("[OK] Check logs/ directory for output")


def test_trading_logging():
    """Test trading-specific logging"""
    print("\n" + "="*80)
    print("TEST 2: Trading Logging")
    print("="*80)

    # Log some signals
    log_signal('BUY', 92.5, 'Strong bullish confluence: OB + BOS + Liquidity')
    log_signal('HOLD', 45.0, 'Neutral market structure')
    log_signal('SELL', 78.0, 'Bearish divergence detected')

    time.sleep(0.5)

    # Log some trades
    log_trade('BUY', 50000.0, 0.1, 'Entry at bullish OB in discount')
    time.sleep(0.2)
    log_trade('SELL', 51500.0, 0.1, 'Take profit hit', pnl=150.0)

    time.sleep(0.3)

    log_trade('SELL', 51000.0, 0.05, 'Entry at bearish FVG')
    time.sleep(0.2)
    log_trade('BUY', 50500.0, 0.05, 'Take profit hit', pnl=25.0)

    print("[OK] Check logs/trading.log for trade history")


def test_rl_logging():
    """Test RL training logging"""
    print("\n" + "="*80)
    print("TEST 3: RL Training Logging")
    print("="*80)

    rl_logger = get_rl_logger()

    # Simulate training episodes
    for episode in range(5):
        reward = np.random.uniform(50, 150)
        steps = np.random.randint(100, 300)
        win_rate = np.random.uniform(0.5, 0.8)
        balance = 10000 + episode * 500 + np.random.uniform(-100, 100)

        rl_logger.info(
            f'Episode {episode:3d} | Reward: {reward:6.2f} | '
            f'Steps: {steps:3d} | Win Rate: {win_rate:.1%} | '
            f'Balance: ${balance:,.2f}'
        )

        time.sleep(0.1)

    print("[OK] Check logs/rl_training.log for training history")


def test_error_logging():
    """Test error logging"""
    print("\n" + "="*80)
    print("TEST 4: Error Logging")
    print("="*80)

    logger = get_logger('ErrorTest')

    # Simulate various errors
    try:
        raise ValueError("Invalid parameter value")
    except ValueError as e:
        log_error('ErrorTest', e, 'During parameter validation')

    try:
        result = 10 / 0
    except ZeroDivisionError as e:
        logger.error(f'Math error: {e}', exc_info=True)

    try:
        data = {'key': 'value'}
        _ = data['missing_key']
    except KeyError as e:
        logger.error(f'Key not found: {e}')

    print("[OK] Check logs/errors.log for error details")


def test_performance_logging():
    """Test performance metrics logging"""
    print("\n" + "="*80)
    print("TEST 5: Performance Metrics Logging")
    print("="*80)

    # Simulate session metrics
    session_metrics = {
        'Session': 'NY AM Kill Zone',
        'Duration': '3 hours',
        'Total Trades': 25,
        'Winning Trades': 17,
        'Losing Trades': 8,
        'Win Rate': 0.68,
        'Profit Factor': 2.3,
        'Sharpe Ratio': 1.85,
        'Max Drawdown': 0.15,
        'Total PnL': 3250.50,
        'Best Trade': 450.0,
        'Worst Trade': -180.0,
        'Avg Win': 250.0,
        'Avg Loss': -125.0
    }

    log_performance(session_metrics)

    print("[OK] Check logs/trading.log for formatted metrics")


def test_tensorboard_logging():
    """Test TensorBoard logging"""
    print("\n" + "="*80)
    print("TEST 6: TensorBoard Logging")
    print("="*80)

    tb_logger = get_tensorboard_logger('test_experiment')

    # Log some training data
    print("Logging training data to TensorBoard...")

    for step in range(20):
        # Training metrics
        loss = 1.0 - step * 0.04 + np.random.uniform(-0.05, 0.05)
        reward = step * 5 + np.random.uniform(-2, 2)
        learning_rate = 0.0003 * (0.99 ** step)

        tb_logger.log_training_step(step, loss, reward, learning_rate)

        # Episode metrics (every 5 steps)
        if step % 5 == 0:
            episode = step // 5
            total_reward = step * 50 + np.random.uniform(-20, 20)
            num_steps = 100 + step * 10
            win_rate = 0.5 + step * 0.015
            balance = 10000 + step * 200

            tb_logger.log_episode(episode, total_reward, num_steps, win_rate, balance)

        time.sleep(0.05)

    # Log trading metrics
    trading_metrics = {
        'win_rate': 0.68,
        'profit_factor': 2.3,
        'sharpe_ratio': 1.85,
        'max_drawdown': 0.15,
        'total_pnl': 3250.50
    }

    tb_logger.log_trading_metrics(step=20, metrics=trading_metrics)

    # Log hyperparameters
    hparams = {
        'learning_rate': 0.0003,
        'batch_size': 64,
        'gamma': 0.99,
        'n_steps': 2048
    }

    final_metrics = {
        'final_reward': 500.0,
        'final_win_rate': 0.68,
        'final_sharpe': 1.85
    }

    tb_logger.log_hyperparameters(hparams, final_metrics)

    # Log text
    tb_logger.log_text('Experiment/Notes', 'Test run of logging system', step=0)

    tb_logger.close()

    print("[OK] TensorBoard logs saved to tensorboard_logs/test_experiment")
    print("   Start TensorBoard: tensorboard --logdir=tensorboard_logs")
    print("   Then open: http://localhost:6006")


def test_component_logger():
    """Test component-specific logger"""
    print("\n" + "="*80)
    print("TEST 7: Component-Specific Logger")
    print("="*80)

    # Multiple components
    pd_logger = get_logger('PDDetector')
    structure_logger = get_logger('MarketStructure')
    liquidity_logger = get_logger('LiquidityDetector')

    pd_logger.info('Starting PD array detection...')
    pd_logger.info('Detected 42 order blocks')

    structure_logger.info('Analyzing market structure...')
    structure_logger.info('Found 8 BOS and 3 CHOCH')

    liquidity_logger.info('Detecting liquidity zones...')
    liquidity_logger.info('Found 15 liquidity zones')

    print("[OK] Each component has its own logger instance")


def main():
    """Run all logging tests"""
    print("\n" + "="*80)
    print("=" + " "*20 + "ICT LOGGING SYSTEM TEST" + " "*35 + "=")
    print("="*80)

    try:
        test_basic_logging()
        test_trading_logging()
        test_rl_logging()
        test_error_logging()
        test_performance_logging()
        test_tensorboard_logging()
        test_component_logger()

        print("\n" + "="*80)
        print("ALL TESTS COMPLETED SUCCESSFULLY")
        print("="*80)

        print("\nOutput Locations:")
        print(f"   Regular Logs: logs/")
        print(f"   TensorBoard:  tensorboard_logs/")

        print("\nView TensorBoard:")
        print(f"   1. Run: tensorboard --logdir=tensorboard_logs")
        print(f"   2. Open: http://localhost:6006")

        print("\nLog Files Created:")
        print(f"   - logs/ict_trading_*.log    (main log with timestamp)")
        print(f"   - logs/errors.log            (all errors)")
        print(f"   - logs/trading.log           (trades & signals)")
        print(f"   - logs/rl_training.log       (RL training)")

        print("\nLogging system is ready to use!")
        print("="*80)

    except Exception as e:
        print(f"\n[ERROR] Test failed: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main()
