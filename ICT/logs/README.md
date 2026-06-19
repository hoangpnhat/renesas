# Logs Directory

This directory contains all application logs for the ICT Trading System.

## Log Files

### Main Application Log
- **Pattern**: `ict_trading_YYYYMMDD_HHMMSS.log`
- **Content**: All application logs with timestamps
- **Rotation**: New file created each run
- **Level**: DEBUG and above

Example:
```
2025-01-29 14:30:52 | INFO     | ICT.Pipeline | __init__:45 | Initializing feature extraction pipeline
2025-01-29 14:30:52 | DEBUG    | ICT.PDDetector | detect:120 | Processing 500 candles
2025-01-29 14:30:53 | INFO     | ICT.PDDetector | detect:135 | Detected 42 order blocks
```

### Error Log
- **File**: `errors.log`
- **Content**: All ERROR and CRITICAL level messages
- **Persistence**: Appends across runs
- **Level**: ERROR and above
- **Includes**: Full stack traces

Example:
```
2025-01-29 14:31:05 | ERROR    | ICT.DataFetcher | fetch:78 | Connection timeout
Traceback (most recent call last):
  File "data_fetcher.py", line 78, in fetch
    response = requests.get(url, timeout=5)
...
```

### Trading Log
- **File**: `trading.log`
- **Content**: All trading activity (signals, trades, performance)
- **Persistence**: Appends across runs
- **Level**: INFO and above

Example:
```
2025-01-29 14:32:10 | INFO     | SIGNAL | BUY  | Confidence: 92.5% | Bullish OB + BOS
2025-01-29 14:32:15 | INFO     | TRADE | BUY  | Price: $50,000.00 | Qty: 0.1000
2025-01-29 14:35:20 | INFO     | TRADE | SELL | Price: $51,500.00 | Qty: 0.1000 | PnL: +$150.00
```

### RL Training Log
- **File**: `rl_training.log`
- **Content**: Reinforcement learning training progress
- **Persistence**: Appends across runs
- **Level**: DEBUG and above

Example:
```
2025-01-29 14:40:00 | INFO     | Episode   100 | Reward:  +125.50 | Steps:  150 | Win Rate: 65.0% | Balance: $10,500.00
2025-01-29 14:41:00 | INFO     | Episode   200 | Reward:  +185.20 | Steps:  180 | Win Rate: 68.0% | Balance: $12,300.00
```

## Log Levels

| Level | Description | Use Case |
|-------|-------------|----------|
| DEBUG | Detailed diagnostic information | Development, troubleshooting |
| INFO | General informational messages | Normal operation tracking |
| WARNING | Warning messages | Potential issues |
| ERROR | Error messages | Errors that don't stop execution |
| CRITICAL | Critical messages | Severe errors |

## Log Format

### File Logs
```
TIMESTAMP | LEVEL | COMPONENT | FUNCTION:LINE | MESSAGE
```

### Console Logs (Colored)
```
LEVEL | COMPONENT | MESSAGE
```

## Viewing Logs

### Tail Latest Logs (Linux/Mac)
```bash
# Main log (latest)
tail -f logs/ict_trading_*.log | tail -1

# Error log
tail -f logs/errors.log

# Trading log
tail -f logs/trading.log

# RL training log
tail -f logs/rl_training.log
```

### Tail Latest Logs (Windows PowerShell)
```powershell
# Main log (latest)
Get-Content logs\ict_trading_*.log -Wait -Tail 50

# Error log
Get-Content logs\errors.log -Wait -Tail 50
```

### Search Logs
```bash
# Find all errors
grep "ERROR" logs/ict_trading_*.log

# Find specific component logs
grep "PDDetector" logs/ict_trading_*.log

# Find trades
grep "TRADE" logs/trading.log
```

## Log Rotation

- **Main logs**: New file per run (timestamped)
- **Error logs**: Persistent, manual rotation needed
- **Trading logs**: Persistent, manual rotation needed
- **RL logs**: Persistent, manual rotation needed

### Manual Rotation
```bash
# Archive old logs
mkdir -p logs/archive
mv logs/errors.log logs/archive/errors_$(date +%Y%m%d).log
mv logs/trading.log logs/archive/trading_$(date +%Y%m%d).log
mv logs/rl_training.log logs/archive/rl_training_$(date +%Y%m%d).log
```

## Disk Space Management

### Check Log Sizes
```bash
# Total size
du -sh logs/

# Individual files
ls -lh logs/
```

### Clean Old Logs
```bash
# Delete logs older than 30 days
find logs/ -name "*.log" -mtime +30 -delete

# Delete all but latest 10 main logs
ls -t logs/ict_trading_*.log | tail -n +11 | xargs rm -f
```

## Troubleshooting

### No Logs Appearing

1. **Check permissions**:
   ```bash
   ls -la logs/
   chmod 755 logs/
   ```

2. **Check logger initialization**:
   ```python
   from utils import ICTLogger
   logger = ICTLogger()
   print(f"Log dir: {logger.log_dir}")
   ```

3. **Check log level**:
   ```python
   logger.setLevel(logging.DEBUG)
   ```

### Logs Too Large

1. **Reduce log level**:
   ```python
   logger.setLevel(logging.INFO)  # Instead of DEBUG
   ```

2. **Implement rotation**:
   - Use `RotatingFileHandler` instead of `FileHandler`
   - Configure max size and backup count

3. **Archive regularly**:
   - Move old logs to archive directory
   - Compress archived logs

## Integration

### In Your Code
```python
from utils import get_logger

# Get component logger
logger = get_logger('MyComponent')

# Log messages
logger.info('Processing started')
logger.debug(f'Processing {len(data)} items')
logger.warning('Low confidence signal')
logger.error('API request failed')
```

### Trading Logs
```python
from utils import log_trade, log_signal

# Log signal
log_signal('BUY', 85.5, 'Strong bullish confluence')

# Log trade
log_trade('BUY', 50000.0, 0.1, 'Entry at OB')
```

## Best Practices

1. **Use appropriate log levels**
   - DEBUG: Development only
   - INFO: Normal operations
   - WARNING: Potential issues
   - ERROR: Actual errors

2. **Include context**
   ```python
   # Bad
   logger.error('Failed')

   # Good
   logger.error(f'PD detection failed for {timeframe}: {error}')
   ```

3. **Log exceptions with traceback**
   ```python
   try:
       risky_operation()
   except Exception as e:
       logger.error('Operation failed', exc_info=True)
   ```

4. **Regular cleanup**
   - Archive old logs monthly
   - Delete very old logs (>90 days)
   - Monitor disk usage

5. **Monitor critical logs**
   - Set up alerts for ERROR logs
   - Review trading.log daily
   - Check rl_training.log during training

## See Also

- [Logging Guide](../docs/LOGGING_GUIDE.md) - Complete logging documentation
- [TensorBoard Logs](../tensorboard_logs/README.md) - TensorBoard visualization
- `utils/logger.py` - Logger implementation

---

**Note**: This directory is created automatically when the logging system initializes.
