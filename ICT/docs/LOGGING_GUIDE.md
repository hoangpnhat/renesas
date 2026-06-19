# ICT Trading System - Logging Guide

## Overview

The ICT Trading System includes a comprehensive logging system with:

- **Standard Logging**: File and console output with colors
- **TensorBoard Integration**: Real-time metrics visualization
- **Component-Specific Loggers**: Separate logs for different modules
- **Trading Logs**: Dedicated logging for trades and signals
- **RL Training Logs**: Training metrics and episode data

---

## Quick Start

### Basic Logging

```python
from utils import get_logger

# Get a logger for your component
logger = get_logger('MyComponent')

# Log messages at different levels
logger.debug('Detailed debug information')
logger.info('General information')
logger.warning('Warning message')
logger.error('Error occurred')
logger.critical('Critical issue')
```

### Trading Logging

```python
from utils import log_trade, log_signal

# Log a trade
log_trade(
    action='BUY',
    price=50000.0,
    quantity=0.1,
    reason='Bullish OB at discount + BOS',
    pnl=None  # Set after closing position
)

# Log a trading signal
log_signal(
    signal='BUY',
    confidence=85.5,
    reasoning='HTF-LTF confluence detected'
)
```

### TensorBoard Logging

```python
from utils import get_tensorboard_logger

# Initialize TensorBoard logger
tb_logger = get_tensorboard_logger('my_experiment')

# Log scalars
tb_logger.log_scalar('Training/Loss', 0.5, step=100)
tb_logger.log_scalar('Training/Reward', 25.0, step=100)

# Log episode metrics
tb_logger.log_episode(
    episode=50,
    total_reward=1500.0,
    steps=200,
    win_rate=0.68,
    balance=12500.0
)

# Close when done
tb_logger.close()
```

---

## Log Files

All logs are saved to the `logs/` directory:

```
logs/
├── ict_trading_20250129_143052.log    # Main application log (timestamped)
├── errors.log                         # All errors (persistent)
├── trading.log                        # Trading activity (persistent)
└── rl_training.log                    # RL training (persistent)
```

TensorBoard logs are saved to `tensorboard_logs/`:

```
tensorboard_logs/
├── 20250129_143052/                   # Experiment by timestamp
│   └── events.out.tfevents...
└── my_experiment/                     # Named experiment
    └── events.out.tfevents...
```

---

## Usage Examples

### 1. Component Logger

```python
from utils import get_logger

class PDDetector:
    def __init__(self):
        self.logger = get_logger('PDDetector')

    def detect_order_blocks(self, df):
        self.logger.info('Starting order block detection...')

        try:
            # Your detection logic
            order_blocks = self._detect(df)
            self.logger.info(f'Detected {len(order_blocks)} order blocks')
            return order_blocks

        except Exception as e:
            self.logger.error(f'Detection failed: {e}', exc_info=True)
            raise
```

### 2. Trading Logger

```python
from utils import log_trade, log_signal, log_performance

# Log signals
log_signal('BUY', 92.0, 'Strong bullish confluence')
log_signal('HOLD', 45.0, 'Neutral market structure')

# Log trades
log_trade('BUY', 50000.0, 0.1, 'Entry at bullish OB')
log_trade('SELL', 52000.0, 0.1, 'Take profit hit', pnl=200.0)

# Log performance summary
metrics = {
    'Total Trades': 150,
    'Win Rate': 0.68,
    'Profit Factor': 2.3,
    'Sharpe Ratio': 1.85,
    'Max Drawdown': 0.15,
    'Total PnL': 15420.50
}
log_performance(metrics)
```

### 3. RL Training Logger

```python
from utils import get_rl_logger, get_tensorboard_logger

# Standard logging
rl_logger = get_rl_logger()

# TensorBoard logging
tb_logger = get_tensorboard_logger('rl_training_v1')

# During training
for episode in range(1000):
    # ... training code ...

    # Log to file
    rl_logger.info(f'Episode {episode}: reward={total_reward:.2f}')

    # Log to TensorBoard
    tb_logger.log_episode(
        episode=episode,
        total_reward=total_reward,
        steps=num_steps,
        win_rate=win_rate,
        balance=current_balance
    )

    tb_logger.log_scalars('Metrics', {
        'reward': total_reward,
        'steps': num_steps,
        'win_rate': win_rate
    }, episode)

tb_logger.close()
```

### 4. Error Logging

```python
from utils import log_error, get_logger

logger = get_logger('MyComponent')

try:
    # Your code that might fail
    risky_operation()

except ValueError as e:
    # Log error with context
    log_error('MyComponent', e, 'During data validation')

except Exception as e:
    # Log unexpected errors
    logger.error(f'Unexpected error: {e}', exc_info=True)
    raise
```

### 5. Performance Metrics

```python
from utils import log_performance

# After trading session
session_metrics = {
    'Session': 'NY AM Kill Zone',
    'Duration': '3 hours',
    'Trades': 12,
    'Wins': 8,
    'Losses': 4,
    'Win Rate': 0.667,
    'Total PnL': 1250.0,
    'Best Trade': 350.0,
    'Worst Trade': -120.0,
    'Avg Win': 200.0,
    'Avg Loss': -95.0,
    'Profit Factor': 2.1
}

log_performance(session_metrics)
```

---

## TensorBoard Features

### Starting TensorBoard

```bash
# View all experiments
tensorboard --logdir=tensorboard_logs

# View specific experiment
tensorboard --logdir=tensorboard_logs/my_experiment

# With custom port
tensorboard --logdir=tensorboard_logs --port=6007
```

Then open: http://localhost:6006

### Logging Scalars

```python
tb_logger = get_tensorboard_logger('experiment_1')

# Single scalar
tb_logger.log_scalar('Training/Loss', 0.5, step=100)

# Multiple scalars
tb_logger.log_scalars('Rewards', {
    'mean_reward': 50.0,
    'max_reward': 100.0,
    'min_reward': 10.0
}, step=100)
```

### Logging Histograms

```python
import numpy as np

# Log weight distributions
weights = np.random.randn(1000)
tb_logger.log_histogram('Model/Weights', weights, step=100)
```

### Logging Hyperparameters

```python
# Define hyperparameters
hparams = {
    'learning_rate': 0.0003,
    'batch_size': 64,
    'gamma': 0.99,
    'epsilon': 0.2
}

# Define metrics
metrics = {
    'final_reward': 1500.0,
    'win_rate': 0.68,
    'sharpe_ratio': 1.85
}

tb_logger.log_hyperparameters(hparams, metrics)
```

### Logging Training Steps

```python
# Convenient wrapper for common training metrics
for step in range(1000):
    loss = compute_loss()
    reward = get_reward()
    lr = scheduler.get_last_lr()[0]

    tb_logger.log_training_step(
        step=step,
        loss=loss,
        reward=reward,
        learning_rate=lr
    )
```

### Logging Trading Metrics

```python
# Log trading performance to TensorBoard
trading_metrics = {
    'win_rate': 0.68,
    'profit_factor': 2.3,
    'sharpe_ratio': 1.85,
    'max_drawdown': 0.15,
    'total_pnl': 15420.50
}

tb_logger.log_trading_metrics(step=500, metrics=trading_metrics)
```

---

## Log Levels

Standard Python logging levels:

| Level | Numeric Value | When to Use |
|-------|---------------|-------------|
| `DEBUG` | 10 | Detailed diagnostic information |
| `INFO` | 20 | General informational messages |
| `WARNING` | 30 | Warning messages for potentially harmful situations |
| `ERROR` | 40 | Error messages for serious problems |
| `CRITICAL` | 50 | Critical messages for very serious problems |

### Setting Log Level

```python
import logging
from utils import ICTLogger

# Set global log level
logger_instance = ICTLogger()
logger_instance.setup_logging(level=logging.DEBUG)

# Set component-specific level
logger = get_logger('MyComponent')
logger.setLevel(logging.WARNING)  # Only warnings and above
```

---

## Console Output

Console output includes colored log levels:

- 🔵 **DEBUG**: Cyan
- 🟢 **INFO**: Green
- 🟡 **WARNING**: Yellow
- 🔴 **ERROR**: Red
- 🟣 **CRITICAL**: Magenta

Example output:
```
INFO | ICT.PDDetector | Starting order block detection...
INFO | ICT.PDDetector | Detected 42 order blocks
WARNING | ICT.Trading | Low confidence signal ignored
ERROR | ICT.DataFetcher | Connection timeout: retrying...
```

---

## Integration Examples

### In Feature Extraction Pipeline

```python
from utils import get_logger

class FeatureExtractionPipeline:
    def __init__(self, df, timeframe):
        self.logger = get_logger('Pipeline')
        self.logger.info('Initializing feature extraction pipeline')

        # ... initialization code ...

    def extract_features(self, idx):
        self.logger.debug(f'Extracting features at index {idx}')

        try:
            features = self._compute_features(idx)
            self.logger.info(f'Extracted {len(features)} features')
            return features

        except Exception as e:
            self.logger.error(f'Feature extraction failed: {e}', exc_info=True)
            raise
```

### In RL Environment

```python
from utils import get_rl_logger, get_tensorboard_logger

class EnhancedICTTradingEnv:
    def __init__(self, feature_extractor):
        self.logger = get_rl_logger()
        self.tb_logger = get_tensorboard_logger('rl_training')
        self.episode_count = 0

    def step(self, action):
        # ... step logic ...

        # Log to file
        if done:
            self.logger.info(
                f'Episode {self.episode_count} finished: '
                f'reward={episode_reward:.2f}, steps={num_steps}'
            )

            # Log to TensorBoard
            self.tb_logger.log_episode(
                episode=self.episode_count,
                total_reward=episode_reward,
                steps=num_steps,
                win_rate=self.win_rate,
                balance=self.balance
            )

            self.episode_count += 1

        return obs, reward, done, truncated, info
```

### In Trading Bot

```python
from utils import get_trading_logger, log_trade, log_signal

class TradingBot:
    def __init__(self):
        self.logger = get_trading_logger()
        self.logger.info('Trading bot initialized')

    def execute_trade(self, signal):
        self.logger.info(f'Received signal: {signal}')

        # Log signal
        log_signal(
            signal=signal['action'],
            confidence=signal['confidence'],
            reasoning=signal['reasoning']
        )

        # Execute trade
        if self.should_trade(signal):
            log_trade(
                action=signal['action'],
                price=self.current_price,
                quantity=self.calculate_position_size(),
                reason=signal['reasoning']
            )
```

---

## Best Practices

### 1. Use Appropriate Log Levels

```python
logger = get_logger('MyComponent')

# DEBUG: Detailed diagnostic info
logger.debug(f'Processing {len(data)} data points')

# INFO: General progress updates
logger.info('Analysis complete')

# WARNING: Potentially problematic situations
logger.warning('Low confidence signal ignored')

# ERROR: Errors that don't stop execution
logger.error('API request failed, retrying...')

# CRITICAL: Severe errors
logger.critical('Database connection lost')
```

### 2. Include Context in Log Messages

```python
# Bad
logger.error('Failed')

# Good
logger.error(f'Order block detection failed for timeframe {timeframe}: {error}')
```

### 3. Use exc_info for Exceptions

```python
try:
    risky_operation()
except Exception as e:
    # Includes full stack trace
    logger.error('Operation failed', exc_info=True)
```

### 4. Log Performance Metrics Regularly

```python
# After each trading session
metrics = calculate_session_metrics()
log_performance(metrics)

# In TensorBoard for visualization
tb_logger.log_trading_metrics(step=session_count, metrics=metrics)
```

### 5. Close TensorBoard Logger

```python
# At end of training
tb_logger.close()

# Or use context manager (if implemented)
with get_tensorboard_logger('experiment') as tb:
    tb.log_scalar('metric', value, step)
```

---

## Troubleshooting

### Logs Not Appearing

```python
# Check if logger is initialized
from utils import ICTLogger
logger_instance = ICTLogger()

# Verify log directory exists
import os
print(f"Log directory: {logger_instance.log_dir}")
print(f"Exists: {os.path.exists(logger_instance.log_dir)}")
```

### TensorBoard Not Working

```bash
# Install TensorBoard
pip install tensorboard

# Check if installed
python -c "import tensorboard; print('OK')"

# Start TensorBoard with verbose output
tensorboard --logdir=tensorboard_logs --verbose
```

### Permission Errors

```bash
# On Windows, run as administrator
# Or change log directory permissions

# In code, specify custom directory
from utils import ICTLogger
logger = ICTLogger()
logger.setup_logging(log_dir='./my_logs')
```

---

## Configuration

### Custom Log Directory

```python
from utils import ICTLogger

logger = ICTLogger()
logger.setup_logging(
    log_dir='custom_logs',
    level=logging.DEBUG
)
```

### Custom TensorBoard Directory

```python
from utils import TensorBoardLogger

tb_logger = TensorBoardLogger(
    log_dir='custom_tb_logs',
    experiment_name='my_experiment'
)
```

### Disable Console Colors

```python
# Edit utils/logger.py
# In ColoredFormatter class, remove color codes or set COLORS to empty dict
```

---

## Summary

| Feature | Function | Purpose |
|---------|----------|---------|
| Component Logger | `get_logger('Component')` | Module-specific logging |
| Trading Logger | `get_trading_logger()` | Trade execution logs |
| RL Logger | `get_rl_logger()` | Training logs |
| TensorBoard Logger | `get_tensorboard_logger()` | Metrics visualization |
| Log Trade | `log_trade()` | Quick trade logging |
| Log Signal | `log_signal()` | Quick signal logging |
| Log Error | `log_error()` | Quick error logging |
| Log Performance | `log_performance()` | Performance metrics |

---

## Next Steps

1. **Start logging in your components**: Add `get_logger()` calls
2. **Monitor training in TensorBoard**: `tensorboard --logdir=tensorboard_logs`
3. **Review logs regularly**: Check `logs/` directory
4. **Track trading performance**: Use `log_trade()` and `log_performance()`
5. **Analyze metrics**: Use TensorBoard for visualization

---

**Happy Logging!** 📝

For more information, see:
- `utils/logger.py` - Source code
- `logs/` - Log files
- `tensorboard_logs/` - TensorBoard data
