# ICT Trading Bot - Quick Start Guide

## ✅ Ready to Trade!

Your ICT trading bot is now configured with:
- ✅ Modular code structure
- ✅ Real-time data from Binance
- ✅ Paper trading system
- ✅ GPT-4o-mini LLM analysis (when API key provided)
- ✅ Reinforcement learning training
- ✅ Lower SMT reliability on small timeframes (1m=30%, 5m=50%, etc.)

## 🚀 Three Ways to Start

### Option 1: Interactive Setup (Recommended)
```bash
python setup_and_run.py
```
This will guide you through:
1. Setting OpenAI API key (optional)
2. Configuring trading parameters
3. RL training options
4. Starting the bot

### Option 2: Quick Start with Defaults
```bash
# Set your API key first (optional but recommended)
set OPENAI_API_KEY=your_api_key_here

# Run bot with defaults
python bot_trader.py --duration 30 --interval 60
```

### Option 3: Custom Configuration
```bash
python bot_trader.py \
  --symbol BTCUSDT \
  --balance 10000 \
  --risk 1.0 \
  --confidence 75 \
  --duration 60 \
  --interval 60
```

## 🔑 Setting Up OpenAI API Key

### Method 1: Environment Variable (Recommended)
```bash
# Windows CMD
set OPENAI_API_KEY=sk-your-key-here

# Windows PowerShell
$env:OPENAI_API_KEY="sk-your-key-here"

# Linux/Mac
export OPENAI_API_KEY=sk-your-key-here
```

### Method 2: In Code
Edit `config.py` and set:
```python
OPENAI_API_KEY = "sk-your-key-here"
```

### Method 3: Interactive
Use `setup_and_run.py` - it will ask for your API key.

## 📝 What Happens Without API Key?

If no OpenAI API key is provided:
```
⚠️  No OpenAI API key provided
   Using rule-based analysis. Set OPENAI_API_KEY or pass api_key to enable GPT-4o-mini
```

The bot will work fine using **rule-based analysis** instead. This is:
- ✅ Free (no API costs)
- ✅ Fast (no network calls)
- ✅ Reliable (deterministic logic)
- ❌ Less sophisticated than GPT-4o-mini

## 🤖 With GPT-4o-mini API Key

When you provide the API key:
```
✅ LLM Analyzer: Using OpenAI GPT-4o-mini
```

Benefits:
- ✅ Advanced market context understanding
- ✅ Natural language reasoning
- ✅ Adaptive to complex market conditions
- ✅ Explains decision-making process
- ❌ Costs ~$0.15 per 1M input tokens ($0.60 per 1M output)

Typical cost: **~$0.001-0.005 per signal** (very cheap!)

## 📊 Example Session

```bash
python setup_and_run.py
```

**Output:**
```
================================================================================
ICT TRADING BOT - SETUP
================================================================================

📝 OpenAI API Key Configuration

To enable GPT-4o-mini analysis, you need an OpenAI API key.
Get your key from: https://platform.openai.com/api-keys

Press Enter to skip and use rule-based analysis instead.
================================================================================

Enter your OpenAI API key: sk-proj-xxxxx

✅ API key configured (length: 56)
✅ OpenAI API key configured! GPT-4o-mini will be used for market analysis.

================================================================================
BOT CONFIGURATION
================================================================================

Default settings:
  Symbol: BTCUSDT
  Initial Balance: $10,000.00
  Risk per Trade: 1.0%
  Min Confidence: 75.0%
  Duration: 60 minutes
  Update Interval: 60 seconds

Use default settings? (Y/n): y

================================================================================
REINFORCEMENT LEARNING
================================================================================

Train RL model before trading? (y/N): n
Use RL for trading (if already trained)? (y/N): n

================================================================================
STARTING TRADING BOT
================================================================================

📊 Trading: BTCUSDT
💰 Balance: $10,000.00
⚠️  Risk: 1.0% per trade
🎯 Min Confidence: 75.0%
⏱️  Duration: 60 minutes
🔄 Interval: 60 seconds
🤖 GPT-4o-mini: ENABLED
🧠 RL Training: NO
🧠 RL Trading: NO

Press Enter to start...
```

## 🎓 Training RL Model

To train the RL model first, then use it for trading:

```bash
python bot_trader.py --train-rl --use-rl --rl-steps 50000 --duration 60
```

This will:
1. Fetch historical data
2. Train RL model for 50,000 steps (~5-10 minutes)
3. Use the trained model for trading
4. Run for 60 minutes

## 🔍 What the Bot Does

### Every Update Cycle (default 60 seconds):

1. **Fetch Current Price** from Binance
2. **Check Open Positions** for stop-loss/take-profit
3. **Analyze Market:**
   - PD Arrays (Order Blocks, FVGs)
   - Market Structure (HH, LL, HL, LH)
   - LLM Analysis (GPT-4o-mini or rule-based)
   - RL Model (if enabled)
4. **Generate Signal:** BUY/SELL/HOLD with confidence
5. **Execute Trade** (if confidence > threshold)
6. **Display Statistics**

## 📈 Trading Logic

### Entry Conditions:
- Signal confidence ≥ minimum (default 75%)
- Signal alignment: MODERATE or STRONG
- No conflicting open position

### Position Sizing:
- Risk-based: 1% of account per trade (configurable)
- Calculated from stop-loss distance

### Stop-Loss & Take-Profit:
- SL: 2× ATR from entry
- TP: 3× ATR from entry (1.5:1 R:R ratio)

### Exit Conditions:
- Hit stop-loss
- Hit take-profit
- Signal reverses

## 💾 Saved Files

After each session:
```
paper_trading_BTCUSDT_20251025_143000.json
```

Contains:
- All trades with entry/exit
- P&L for each trade
- Account balance history
- Equity curve
- Final statistics

## 📊 Reading Results

```python
import json

# Load results
with open('paper_trading_BTCUSDT_20251025_143000.json', 'r') as f:
    data = json.load(f)

print(f"Initial Balance: ${data['initial_balance']:,.2f}")
print(f"Final Balance: ${data['balance']:,.2f}")
print(f"Total Trades: {data['statistics']['total_trades']}")
print(f"Win Rate: {data['statistics']['win_rate']:.1f}%")
print(f"ROI: {data['statistics']['roi']:.2f}%")
```

## 🔧 Advanced Configuration

### Different Trading Pairs
```bash
python bot_trader.py --symbol ETHUSDT
python bot_trader.py --symbol SOLUSDT
```

### Higher Risk (More Aggressive)
```bash
python bot_trader.py --risk 2.0 --confidence 70
```

### Lower Risk (Conservative)
```bash
python bot_trader.py --risk 0.5 --confidence 85
```

### Faster Updates
```bash
python bot_trader.py --interval 30  # Every 30 seconds
```

## ⚠️ Important Notes

### API Costs
- GPT-4o-mini: ~$0.001-0.005 per signal
- 60-minute session @ 60s intervals = ~60 signals
- Total cost: **~$0.06-0.30 per hour** (very cheap!)

### Paper Trading
- Uses fake money (no real risk)
- Great for testing strategies
- No exchange fees or slippage simulated
- Always test thoroughly before live trading

### Rate Limits
- Binance: 1200 requests/minute (plenty for the bot)
- OpenAI: 500 requests/minute (plenty for the bot)

## 🐛 Troubleshooting

### "No OpenAI API key"
- Set OPENAI_API_KEY environment variable
- Or use `setup_and_run.py` to configure interactively
- Bot will use rule-based analysis as fallback

### "Failed to fetch data"
- Check internet connection
- Verify symbol exists (e.g., BTCUSDT, not BTC-USDT)
- Check Binance API status

### "No trades executed"
- Lower `--confidence` threshold
- Check if signals meet criteria
- Verify min_confidence setting

### Bot too slow
- Increase `--interval` (e.g., 120 seconds)
- Reduce number of timeframes
- Disable RL if not needed

## 📚 Next Steps

1. **Test without GPT first** (rule-based analysis)
2. **Add API key** when ready
3. **Try different confidence levels** (70%, 75%, 80%)
4. **Train RL model** for more advanced trading
5. **Analyze results** from saved JSON files
6. **Optimize parameters** based on results

---

## 🎯 Ready to Start!

Choose your path:

**Beginner:** `python setup_and_run.py` (interactive)
**Quick Test:** `python bot_trader.py --duration 10`
**With GPT:** Set API key → `python bot_trader.py`
**With RL:** `python bot_trader.py --train-rl --use-rl`

**Happy Paper Trading! 📈🤖**
