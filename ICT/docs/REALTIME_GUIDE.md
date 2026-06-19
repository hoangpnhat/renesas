# ICT Trading System - Real-Time Trading Guide

## ✅ Successfully Tested with Real Data!

Your system is now working with **real-time data from Binance**!

**Current BTC Price: $111,536.37**

## Quick Start - Real Data

### 1. Quick Test (Single Analysis)
```bash
python main_realtime.py --mode quick
```
This fetches real BTC and ETH data once and performs analysis.

### 2. Real-Time Monitoring (Continuous Updates)
```bash
python main_realtime.py --mode realtime
```
This continuously monitors the market and updates signals every 60 seconds.

### 3. Compare Exchanges
```bash
python main_realtime.py --mode compare
```
Compare prices across different data sources.

## Real-Time Results (Just Tested!)

```
📊 Fetched Real Data:
   - BTC: 200 candles per timeframe (1h, 4h, 1d)
   - ETH: 200 candles per timeframe (1h, 4h, 1d)

🔍 PD Arrays Detected (Real Market):
   - 1h: 36 arrays (12 order blocks + 24 FVGs)
   - 4h: 86 arrays (46 order blocks + 40 FVGs)
   - 1d: 103 arrays (65 order blocks + 38 FVGs)

⚡ SMT Divergences Detected:
   - 1h: 4 bearish SMT divergences
   - 4h: 1 bullish + 1 bearish
   - 1d: 4 bullish + 2 bearish

🎯 Trading Signal at $111,536.37:
   - PD Array: BUY (87.8% confidence)
   - SMT: SELL (75.0% confidence)
   - LLM: HOLD (50.0% confidence)
   - FINAL: HOLD (40.7% - WEAK alignment due to conflicting signals)
```

## How It Works

### Data Fetching
The system uses Binance's public API to fetch:
- **Historical data**: Up to 1000 candles per request
- **Current price**: Real-time price updates
- **Multiple timeframes**: 1h, 4h, 1d (configurable)

### Real-Time Updates
In real-time mode, the system:
1. Fetches latest candle data every 60 seconds
2. Updates ICT indicators (Order Blocks, FVGs, Market Structure)
3. Re-detects PD arrays and SMT divergences
4. Generates new trading signal
5. Displays results in terminal

### Supported Exchanges

#### Binance (Currently Implemented)
- Symbol format: `BTCUSDT`, `ETHUSDT`, etc.
- Timeframes: `1m`, `5m`, `15m`, `1h`, `4h`, `1d`, `1w`
- No authentication required for public data
- Rate limit: ~1200 requests/minute

#### Coinbase (Partial Implementation)
- Symbol format: `BTC-USD`, `ETH-USD`
- Granularity: 60, 300, 900, 3600, 21600, 86400 (seconds)

## Configuration

### Change Trading Pair
Edit `main_realtime.py`:
```python
PRIMARY_SYMBOL = 'BTCUSDT'    # Change to any pair: ETHUSDT, BNBUSDT, etc.
SECONDARY_SYMBOL = 'ETHUSDT'  # For SMT analysis
```

### Change Timeframes
```python
TIMEFRAMES = ['1h', '4h', '1d']  # Add/remove timeframes
```

### Change Update Interval
```python
UPDATE_INTERVAL = 60  # seconds (60 = 1 minute)
```

### Enable/Disable Components
```python
system = ICTCompleteTradingSystem(
    primary_asset_data=btc_data,
    secondary_asset_data=eth_data,
    use_llm=True,   # LLM analysis
    use_smt=True,   # SMT divergence
    use_rl=False    # RL bot (requires training)
)
```

## Available Trading Pairs

### Binance Popular Pairs
```python
# Cryptocurrencies vs USDT
'BTCUSDT'   # Bitcoin
'ETHUSDT'   # Ethereum
'BNBUSDT'   # Binance Coin
'SOLUSDT'   # Solana
'XRPUSDT'   # Ripple
'ADAUSDT'   # Cardano
'DOGEUSDT'  # Dogecoin
'MATICUSDT' # Polygon
'DOTUSDT'   # Polkadot
'AVAXUSDT'  # Avalanche
'LINKUSDT'  # Chainlink
'UNIUSDT'   # Uniswap

# Fiat pairs
'BTCBUSD'   # Bitcoin vs BUSD
'ETHBUSD'   # Ethereum vs BUSD
```

## Real-Time Mode Output

When running in real-time mode, you'll see:

```
================================================================================
Update #1 - 2025-10-25 14:30:45
================================================================================

📍 Current Price: $111,536.37
🎯 Signal: BUY
💪 Confidence: 82.5%
🔗 Alignment: STRONG
💭 Reasoning: Votes: 3 BUY, 0 SELL, 0 NEUTRAL...

📊 Component Signals:
   • PD_ARRAY: BUY (87.8%)
   • SMT: BUY (75.0%)
   • LLM: BUY (80.0%)

⏳ Next update in 60 seconds...
```

## Advanced Usage

### Custom Data Fetcher

Create your own fetcher for other exchanges:

```python
from data_fetcher import BinanceDataFetcher

class MyCustomFetcher(BinanceDataFetcher):
    def __init__(self):
        self.base_url = "https://api.myexchange.com"

    def get_klines(self, symbol, interval, limit):
        # Implement your API logic
        pass
```

### Save Signals to File

Add to `main_realtime.py`:

```python
import json
from datetime import datetime

# After getting signal
signal_data = {
    'timestamp': datetime.now().isoformat(),
    'price': current_price,
    'signal': signal['signal'],
    'confidence': signal['confidence']
}

with open('signals.json', 'a') as f:
    f.write(json.dumps(signal_data) + '\n')
```

### Telegram Notifications

Install python-telegram-bot:
```bash
pip install python-telegram-bot
```

Add notification function:
```python
async def send_telegram_alert(message):
    bot = telegram.Bot(token='YOUR_BOT_TOKEN')
    await bot.send_message(chat_id='YOUR_CHAT_ID', text=message)

# When strong signal detected
if signal['alignment'] == 'STRONG':
    await send_telegram_alert(
        f"🚨 STRONG {signal['signal']} signal at ${current_price}"
    )
```

### Export to CSV

```python
import csv

# Create signal logger
def log_signal_to_csv(signal, price):
    with open('trading_signals.csv', 'a', newline='') as f:
        writer = csv.writer(f)
        writer.writerow([
            datetime.now(),
            price,
            signal['signal'],
            signal['confidence'],
            signal['alignment']
        ])
```

## API Rate Limits

### Binance
- **Weight limit**: 1200 per minute
- **Order limit**: 10 per second
- Public data: No authentication needed
- Weight per request: 1-5 depending on limit parameter

### Best Practices
1. Use reasonable update intervals (≥60 seconds)
2. Cache data when possible
3. Handle rate limit errors gracefully
4. Use websockets for high-frequency updates (future enhancement)

## Troubleshooting

### SSL Certificate Error
Already fixed in code with `verify=False`. If issues persist:
```python
import os
os.environ['REQUESTS_CA_BUNDLE'] = ''
```

### Network/Proxy Issues
If behind corporate proxy:
```python
proxies = {
    'http': 'http://proxy.company.com:8080',
    'https': 'http://proxy.company.com:8080'
}
response = requests.get(url, proxies=proxies, verify=False)
```

### Data Not Updating
- Check internet connection
- Verify symbol exists on exchange
- Check API status: https://api.binance.com/api/v3/ping

### Missing Indicators
Some indicators need minimum data:
- Order Blocks: Minimum 10 candles
- FVGs: Minimum 3 candles
- SMT: Minimum 2 swing points

## Performance Tips

### Reduce Data Fetching
```python
# Instead of 500 candles
limit=200  # Faster but less historical context

# Use fewer timeframes
timeframes=['1h', '4h']  # Instead of ['1h', '4h', '1d', '1w']
```

### Disable Heavy Components
```python
use_smt=False  # Skip SMT if no correlation analysis needed
use_llm=False  # Skip LLM for pure ICT signals
```

### Async Data Fetching (Future)
```python
import asyncio
import aiohttp

async def fetch_all_timeframes():
    # Fetch all timeframes in parallel
    pass
```

## Next Steps

1. ✅ **Test with different pairs**: Try ETHUSDT, SOLUSDT, etc.
2. ✅ **Run real-time mode**: Monitor live market for 15-30 minutes
3. 📊 **Add logging**: Save signals to CSV/JSON for analysis
4. 🔔 **Add alerts**: Set up notifications for strong signals
5. 📈 **Backtest**: Compare historical signals vs actual results
6. 🤖 **Train RL model**: Use real data to train the RL bot

## Example Trading Workflow

```python
# 1. Start real-time monitoring
python main_realtime.py --mode realtime

# 2. Wait for STRONG signal with high confidence (>80%)

# 3. When signal appears:
#    - STRONG BUY at discount PD array → Enter long
#    - STRONG SELL at premium PD array → Enter short

# 4. Set stop loss below/above the PD array

# 5. Target: Risk-reward ratio of 1:2 or better
```

## Disclaimer

⚠️ **This is for educational purposes only. Not financial advice.**

- Always backtest strategies before live trading
- Start with small position sizes
- Use proper risk management
- Never risk more than 1-2% per trade
- Be aware of exchange fees and slippage

## Support

For issues:
1. Check Binance API status: https://www.binance.com/en/support/announcement
2. Verify your internet connection
3. Check the logs for error messages
4. Test with smaller data sets first

---

**Happy Trading! 🚀📈**
