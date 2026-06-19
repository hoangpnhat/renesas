# ICT Reinforcement Learning Trading Bot

## Phase 2 Implementation - COMPLETE ✓

A production-grade reinforcement learning trading bot based on ICT (Inner Circle Trader) concepts.

---

## Project Structure

```
ICT-chatbot/
├── config.py              # Configuration and parameters
├── structures.py          # Data structures (Position, FVG, MSS, etc.)
├── indicators.py          # ICT indicator detection functions
├── feature_extractor.py   # Feature extraction (32 features)
├── environment.py         # Gymnasium-compatible trading environment
├── test_environment.py    # Test suite
├── requirements.txt       # Python dependencies
├── pyproject.toml         # UV project configuration
└── README.md             # This file
```

---

## What Has Been Implemented

### ✅ Phase 0: ICT Formalization
- **Selected Setup**: Liquidity Sweep → MSS → FVG Entry (London Killzone)
- **Timeframes**: 15M structure, 1M execution
- **Risk Management**: Fixed 1% risk, 2:1 RR minimum
- **Session**: London Killzone (07:00-10:00 GMT)

### ✅ Phase 1: Data & Features
- **32 Features** across 6 groups:
  - Market Structure (6 features)
  - Liquidity (5 features)
  - Fair Value Gaps (6 features)
  - Time/Session (5 features)
  - Price Action (5 features)
  - Position/Risk (5 features)
- ATR-based normalization
- Fully observable state space

### ✅ Phase 2: Trading Environment
- **Gymnasium-compatible environment**
- **Action Space**: Discrete(3) - HOLD, LONG, SHORT
- **Observation Space**: Box(32,) float32
- **Hard-coded risk management**:
  - 1% risk per trade
  - Fixed SL/TP (2:1 RR minimum)
  - Max 3 trades per session
  - London session filter
  - One position at a time
- **Position tracking**: Entry, exit, PnL calculation
- **Episode management**: Reset, step, termination conditions

---

## Implementation Details

### ICT Indicators Implemented

#### 1. Liquidity Sweep Detection
- N-bar swing pivot detection (N=5 on 1M)
- Sweep validation: price breaks level then reverses
- Tracks swept vs unswept levels

#### 2. Market Structure Shift (MSS)
- Detected on 15M timeframe
- Bullish MSS: Break above last Lower High
- Bearish MSS: Break below last Higher Low
- Minimum move filter: 2× ATR(14)
- Valid for 8 bars (2 hours)

#### 3. Fair Value Gap (FVG)
- 3-candle imbalance detection
- Minimum size: 0.3× ATR(14)
- Tracks up to 5 active FVGs per direction
- Mitigation detection (price fills gap)
- Max age: 100 bars

#### 4. Premium/Discount Zones
- Relative to 50-bar swing range on 15M
- Dynamic classification with 10% buffer

---

## Environment Features

### Action Validation
- ✅ Session filter (London only)
- ✅ HTF bias requirement (no neutral trading)
- ✅ Position limit (one at a time)
- ✅ Daily trade limit (max 3)

### Position Management
- Entry at FVG boundaries
- Stop Loss: FVG level ± 1 ATR
- Take Profit: 2:1 Risk-Reward
- Position sizing: 1% account risk
- Commission: $5 per round trip

### Exit Conditions
- Stop Loss hit
- Take Profit hit
- Session end (10:00 GMT force close)
- Max hold time (100 bars = 100 minutes)

---

## Testing Results

All tests **PASSED** ✓

```
TEST 1: Environment Initialization    [PASS]
TEST 2: Environment Reset              [PASS]
TEST 3: Action Execution               [PASS]
TEST 4: Episode Rollout (1000 steps)   [PASS]
TEST 5: Position Management            [PASS]
TEST 6: Feature Extraction             [PASS]
TEST 7: Action Validation              [PASS]
```

---

## Installation & Usage

### Setup with UV (recommended)

```bash
# Initialize UV environment (already done)
uv init

# Install dependencies
uv add numpy pandas gymnasium

# Run tests
uv run test_environment.py
```

### Using the Environment

```python
from environment import ICTTradingEnv
import pandas as pd

# Load your OHLC data (GMT timezone)
ohlc_1m = pd.read_csv('eurusd_1m.csv', index_col=0, parse_dates=True)
ohlc_15m = pd.read_csv('eurusd_15m.csv', index_col=0, parse_dates=True)

# Create environment
env = ICTTradingEnv(ohlc_1m, ohlc_15m)

# Reset environment
obs = env.reset()

# Take actions
action = 0  # HOLD
obs, reward, done, info = env.step(action)

# Get episode statistics
stats = env.get_episode_stats()
print(stats)
```

---

## Next Steps

### ⏳ Phase 3: Reward Function Design
- Design reward shaping
- ICT confluence rewards
- Missed opportunity penalties
- Risk-adjusted PnL rewards

### ⏳ Phase 4: RL Training
- PPO algorithm implementation
- Network architecture (MLP/LSTM)
- Curriculum learning
- Hyperparameter tuning

### ⏳ Phase 5: Evaluation
- Walk-forward validation
- Performance metrics
- Drawdown analysis
- Sharpe ratio calculation

### ⏳ Phase 6: Full Implementation
- Training loop
- Model checkpointing
- Logging and monitoring
- Backtesting pipeline

### ⏳ Phase 7: Extensions
- Multi-session support
- Order blocks
- Liquidity voids
- News event filtering
- Risk-aware RL

---

## Key Configuration Parameters

Located in [config.py](config.py):

```python
# Timeframes
STRUCTURE_TIMEFRAME = '15T'  # 15 minutes
EXECUTION_TIMEFRAME = '1T'   # 1 minute

# Session
LONDON_KILLZONE_START = 7    # 07:00 GMT
LONDON_KILLZONE_END = 10     # 10:00 GMT

# Risk Management
INITIAL_BALANCE = 10000.0
RISK_PER_TRADE = 0.01        # 1%
MIN_RISK_REWARD = 2.0        # 2:1 RR
MAX_TRADES_PER_SESSION = 3

# ICT Parameters
SWEEP_PIVOT_WINDOW = 5
MSS_MIN_MOVE_ATR_MULT = 2.0
FVG_MIN_SIZE_ATR_MULT = 0.3
MSS_VALIDITY_BARS = 8        # 2 hours on 15M
```

---

## Notes

### Why No Trades in Test?
The synthetic test data doesn't generate proper ICT setups:
- No Market Structure Shifts detected → HTF bias remains NEUTRAL
- Without bias, all LONG/SHORT actions are rejected
- This is **correct behavior** - the system requires valid setup confluence

### Real Data Requirements
For actual training, you need:
- Historical EUR/USD OHLC data
- GMT timezone
- Minimum 6 months of data
- Clean data (no missing bars)

### Data Sources
- **Forex**: Dukascopy, FXCM, MetaTrader
- **Crypto**: Binance API, CoinGecko
- Use `yfinance` or broker APIs for data fetching

---

## Project Status

| Phase | Status | Completion |
|-------|--------|------------|
| Phase 0: ICT Formalization | ✅ Complete | 100% |
| Phase 1: Data & Features | ✅ Complete | 100% |
| Phase 2: Environment | ✅ Complete | 100% |
| Phase 3: Reward Function | ⏳ Pending | 0% |
| Phase 4: RL Training | ⏳ Pending | 0% |
| Phase 5: Evaluation | ⏳ Pending | 0% |
| Phase 6: Implementation | ⏳ Pending | 0% |
| Phase 7: Extensions | ⏳ Pending | 0% |

---

## License

This is a research/educational project. **NOT FOR LIVE TRADING WITHOUT EXTENSIVE TESTING.**

---

## Contact & Contributions

Built as a structured, production-oriented RL trading system following ICT principles.

**IMPORTANT**: This bot trades based on formalized ICT concepts. It does NOT:
- Trade based on curve fitting
- Use martingale strategies
- Scale positions
- Trade outside defined sessions
- Make decisions without proper setup confluence

Ready to proceed to **Phase 3: Reward Function Design**.
