# ICT Trading Bot - Paper Trading & RL Training Guide

## 🎯 Overview

Your ICT trading bot now supports:
- **Paper Trading**: Trade with fake money on real market data
- **RL Training**: Train reinforcement learning model on historical data
- **Automated Trading**: Let the bot make decisions based on ICT signals
- **Risk Management**: Automatic position sizing and stop-loss management

## 🚀 Quick Start

### 1. Simple Paper Trading (No RL)
```bash
# Run bot for 30 minutes, check every 60 seconds
python bot_trader.py --duration 30 --interval 60
```

### 2. Paper Trading with RL
```bash
# First train the RL model, then trade
python bot_trader.py --train-rl --use-rl --duration 60
```

### 3. Custom Configuration
```bash
python bot_trader.py \
  --symbol ETHUSDT \
  --balance 50000 \
  --risk 2.0 \
  --confidence 80.0 \
  --duration 120 \
  --interval 30
```

## 📋 Command-Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `--symbol` | Trading pair (e.g., BTCUSDT, ETHUSDT) | BTCUSDT |
| `--balance` | Initial account balance ($) | 10000 |
| `--risk` | Risk per trade (% of balance) | 1.0 |
| `--confidence` | Minimum signal confidence to trade (%) | 75.0 |
| `--duration` | How long to run bot (minutes) | 60 |
| `--interval` | Update interval (seconds) | 60 |
| `--train-rl` | Train RL model before trading | False |
| `--rl-steps` | Number of RL training steps | 50000 |
| `--use-rl` | Use RL model for decisions | False |

## 📊 Bot Behavior

### Signal Generation
The bot analyzes market using:
1. **PD Arrays**: Order blocks, FVGs across multiple timeframes
2. **LLM Analyzer**: Rule-based market context analysis
3. **RL Model** (optional): Trained AI agent

### Trading Rules

#### Entry Conditions:
- Signal confidence ≥ minimum confidence threshold
- Signal alignment must be MODERATE or STRONG
- No existing position OR signal reverses current position

#### Position Sizing:
- Risk per trade: 1% of account (configurable)
- Automatically calculated based on stop-loss distance
- Maximum position: 95% of account balance

#### Exit Conditions:
- **Stop Loss**: Automatically set at 2× ATR from entry
- **Take Profit**: Set at 3× ATR (1.5:1 reward:risk ratio)
- **Signal Reversal**: Close when opposite signal appears

### Bot Update Cycle (Every 60 seconds):

```
1. Fetch current price
2. Check stop-loss / take-profit on open positions
3. Analyze market and generate signal
4. Execute signal if conditions met
5. Display statistics
6. Sleep until next update
```

## 🤖 RL Training

### What is RL Training?

The bot uses **Reinforcement Learning** (PPO algorithm) to learn from market data:
- **Teacher Guidance**: Starts by following ICT rules and LLM analysis
- **Curriculum Learning**: Gradually reduces teacher influence
- **Self-Learning**: Eventually makes independent decisions

### Training Process:

```bash
# Train with 50,000 steps (takes 5-10 minutes)
python bot_trader.py --train-rl --rl-steps 50000 --use-rl
```

**Training Stages:**
1. **Early**: 100% teacher guidance (follows ICT rules strictly)
2. **Middle**: 50% teacher, 50% self-learning
3. **Late**: 10% teacher, 90% independent decisions

### RL Training Output:
```
Training RL Bot with Teacher Guidance
================================================================================
| rollout/                |          |
|    ep_len_mean          | 156      |
|    ep_rew_mean          | 23.4     |
| time/                   |          |
|    fps                  | 234      |
|    iterations           | 100      |
|    time_elapsed         | 876      |
|    total_timesteps      | 50000    |
| train/                  |          |
|    learning_rate        | 0.0003   |
|    loss                 | 0.0234   |
================================================================================
```

## 📈 Example Session

### Scenario: 30-minute paper trading session

```bash
python bot_trader.py --duration 30 --interval 60 --confidence 80
```

**Output:**
```
================================================================================
ICT TRADING BOT INITIALIZED
================================================================================
Symbol: BTCUSDT
Timeframes: 1h, 4h, 1d
Initial Balance: $10,000.00
Risk per Trade: 1.0%
Min Confidence: 80.0%
RL Enabled: False
================================================================================

📊 Fetching market data...
Fetching BTCUSDT 1h data...
  ✅ Fetched 500 candles
...

================================================================================
Update #1/30 - 2025-10-25 14:30:00
================================================================================
📍 Current BTCUSDT Price: $111,569.99

🤖 Analyzing market...

📊 Signal Analysis:
   Decision: BUY
   Confidence: 87.8%
   Alignment: STRONG

🎯 Opening BUY position...
✅ Opened BUY position #1
   Symbol: BTCUSDT
   Entry: $111,569.99
   Size: 0.0089
   Stop Loss: $109,335.49
   Take Profit: $115,022.98
   Risk: 1.0% ($100.00)

💰 Account Status:
   Balance: $10,000.00
   Total Trades: 0
   Win Rate: 0.0%
   Total P&L: $0.00 (+0.00%)

⏳ Next update in 60 seconds...

================================================================================
Update #2/30 - 2025-10-25 14:31:00
================================================================================
📍 Current BTCUSDT Price: $111,823.45

💼 Open BUY Position:
   Entry: $111,569.99
   Unrealized P&L: $2.26

🤖 Analyzing market...

📊 Signal Analysis:
   Decision: BUY
   Confidence: 85.2%
   Alignment: STRONG

✋ Already have BUY position, holding...

💰 Account Status:
   Balance: $10,000.00
   Total Trades: 0
   Win Rate: 0.0%
   Total P&L: $0.00 (+0.00%)

⏳ Next update in 60 seconds...

... (continues for 30 minutes)

💰 Closed BUY position #1 - TAKE_PROFIT
   Exit: $115,022.98
   P&L: $30.76 (+3.09%)
   New Balance: $10,030.76

================================================================================
BOT SESSION COMPLETE
================================================================================

PAPER TRADING STATISTICS
================================================================================

💼 Account Balance:
   Initial: $10,000.00
   Current: $10,030.76
   P&L: $30.76 (+0.31%)
   ROI: +0.31%

📊 Trade Statistics:
   Total Trades: 1
   Winning: 1 (100.0%)
   Losing: 0
   Open Positions: 0

💰 Performance Metrics:
   Average Win: $30.76
   Average Loss: $0.00
   Profit Factor: inf
   Max Drawdown: 0.00%

================================================================================
💾 Account saved to paper_trading_BTCUSDT_20251025_143000.json
```

## 💡 Trading Strategies

### Conservative (Recommended for Beginners)
```bash
python bot_trader.py \
  --risk 0.5 \
  --confidence 85.0 \
  --duration 120
```
- Low risk (0.5% per trade)
- High confidence requirement (85%)
- Longer observation period

### Aggressive (For Experienced Traders)
```bash
python bot_trader.py \
  --risk 2.0 \
  --confidence 70.0 \
  --interval 30
```
- Higher risk (2% per trade)
- Lower confidence threshold (70%)
- More frequent updates (30s)

### RL-Powered Trading
```bash
# Train once (takes 5-10 minutes)
python bot_trader.py --train-rl --rl-steps 100000 --duration 0

# Then use for trading
python bot_trader.py --use-rl --duration 240
```
- Uses trained AI for decisions
- Combines RL with ICT signals
- More adaptive to market conditions

## 📁 Saved Data

### Account Files
Every session saves a JSON file:
```
paper_trading_BTCUSDT_20251025_143000.json
```

Contains:
- All trades (entry, exit, P&L)
- Account balance history
- Equity curve
- Statistics

### Load Previous Session
```python
from paper_trading import PaperTradingAccount

account = PaperTradingAccount()
account.load_from_file("paper_trading_BTCUSDT_20251025_143000.json")
account.print_statistics()
```

## 🎓 RL Training Tips

### Training Duration
- **Quick test**: 10,000 steps (~1 minute)
- **Basic training**: 50,000 steps (~5 minutes)
- **Good training**: 100,000 steps (~10 minutes)
- **Excellent training**: 500,000 steps (~50 minutes)

### Training Data Requirements
- Minimum: 100 candles per timeframe
- Recommended: 500+ candles
- More data = better learning

### Monitor Training
Watch for:
- `ep_rew_mean` (episode reward) should increase
- `loss` should decrease over time
- If reward doesn't improve, try more steps

## 🔧 Advanced Configuration

### Multiple Timeframes
Edit `bot_trader.py`:
```python
bot = ICTTradingBot(
    symbol='BTCUSDT',
    timeframes=['15m', '1h', '4h', '1d'],  # Add more timeframes
    ...
)
```

### Custom Stop-Loss / Take-Profit
Edit `execute_signal()` in `bot_trader.py`:
```python
# Current: 2x ATR stop, 3x ATR target (1.5:1 R:R)
stop_loss = current_price - (2 * atr)
take_profit = current_price + (3 * atr)

# More conservative: 1.5x ATR stop, 3x ATR target (2:1 R:R)
stop_loss = current_price - (1.5 * atr)
take_profit = current_price + (3 * atr)
```

### Different Assets
```bash
# Trade Ethereum
python bot_trader.py --symbol ETHUSDT

# Trade Solana
python bot_trader.py --symbol SOLUSDT

# Trade BNB
python bot_trader.py --symbol BNBUSDT
```

## 📊 Performance Analysis

### View Equity Curve
```python
import json
import matplotlib.pyplot as plt

# Load account data
with open('paper_trading_BTCUSDT_20251025_143000.json', 'r') as f:
    data = json.load(f)

# Plot equity curve
equity = [e['balance'] for e in data['equity_curve']]
plt.plot(equity)
plt.title('Account Equity Curve')
plt.xlabel('Trade Number')
plt.ylabel('Balance ($)')
plt.show()
```

### Compare Strategies
Run multiple bots with different settings and compare results.

## ⚠️ Important Notes

### Risk Warnings
- This is **paper trading** (fake money) for learning/testing
- Always test strategies before live trading
- Past performance ≠ future results
- Start with small risk percentages (0.5-1%)

### Bot Limitations
- Doesn't account for slippage or fees
- Uses simplified ATR calculation
- Market conditions can change rapidly
- No guarantee of profits

### Network Requirements
- Stable internet connection required
- Bot stops if connection lost
- Recommended: Run on VPS for 24/7 operation

## 🚀 Next Steps

1. **Test without RL first**
   ```bash
   python bot_trader.py --duration 30
   ```

2. **Try different confidence levels**
   ```bash
   python bot_trader.py --confidence 70 --duration 30
   python bot_trader.py --confidence 85 --duration 30
   ```

3. **Train RL model**
   ```bash
   python bot_trader.py --train-rl --rl-steps 50000 --duration 0
   ```

4. **Trade with RL**
   ```bash
   python bot_trader.py --use-rl --duration 60
   ```

5. **Analyze results**
   - Compare win rates
   - Check profit factors
   - Review max drawdown
   - Optimize settings

## 🎯 Success Metrics

**Good Performance:**
- Win rate: >55%
- Profit factor: >1.5
- Max drawdown: <20%
- Consistent positive ROI

**Excellent Performance:**
- Win rate: >60%
- Profit factor: >2.0
- Max drawdown: <10%
- Sharpe ratio: >1.0

## 💬 Support

For issues:
1. Check internet connection
2. Verify account balance > 0
3. Check min_confidence setting
4. Review bot logs

---

**Happy Paper Trading! 📈🤖**
