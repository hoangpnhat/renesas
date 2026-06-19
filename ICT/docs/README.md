# Enhanced ICT Trading System

> **Professional-grade Inner Circle Trader (ICT) system with advanced market structure analysis, liquidity detection, and AI-powered reinforcement learning**

[![Python 3.8+](https://img.shields.io/badge/python-3.8+-blue.svg)](https://www.python.org/downloads/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

---

## 🚀 Overview

A comprehensive ICT trading system featuring **100+ dimensional feature space**, integrating premium/discount arrays, market structure analysis, liquidity zones, session-based trading, price attractors, order flow analysis, and AI-powered decision making.

### Key Features

- 🎯 **Advanced PD Array Detection** - OB, FVG, Breaker Blocks, EQH/EQL, BISI/SIBI
- 📊 **Market Structure Analysis** - BOS, CHOCH, swing point classification
- 💧 **Liquidity Intelligence** - Zones, sweeps, attraction vectors
- 🕐 **Session Analysis** - London, New York, Asian kill zones
- 🧲 **Price Attractors** - Magnetic zones with protection scoring
- 📈 **Order Flow Analysis** - Volume, delta, liquidity engineering
- 🤖 **LLM Integration** - Strategy creation, RL training help, agent interpretation
- 🧠 **Advanced RL Environment** - 100+ features with teacher-guided learning

---

## 📦 Installation

### Prerequisites

- Python 3.8 or higher
- pip package manager

### Setup

1. **Clone or navigate to the project directory**
   ```bash
   cd C:\Users\a5153207\Documents\code\ICT
   ```

2. **Activate virtual environment** (if using one)
   ```bash
   # Windows CMD
   .venv\Scripts\activate.bat

   # Windows PowerShell
   .venv\Scripts\activate.ps1

   # Linux/Mac
   source .venv/bin/activate
   ```

3. **Install dependencies**
   ```bash
   pip install -r requirements.txt
   ```

   The system will install:
   - `smartmoneyconcepts` - Professional PD array detection
   - `stable-baselines3` - Reinforcement learning
   - `gymnasium` - RL environment
   - `openai` - LLM integration (optional)
   - `pandas`, `numpy`, `scipy` - Data processing
   - `torch` - Neural networks

---

## 🎮 Quick Start

### Run the Enhanced System

```bash
python enhanced_main.py
```

The system will:
1. ✅ Generate sample market data (or use your own)
2. ✅ Initialize all ICT components
3. ✅ Extract 100+ features from price data
4. ✅ Display comprehensive market analysis
5. ✅ Generate trading signals with confidence scores
6. ✅ (Optional) Train RL agent
7. ✅ (Optional) Test trained agent

### Interactive Demo Output

```
╔══════════════════════════════════════════════════════════════════════════════╗
║                    ENHANCED ICT TRADING SYSTEM                              ║
║                                                                             ║
║  Features: PD Arrays | Market Structure | Liquidity | Sessions | LLM      ║
║            Order Flow | Price Attractors | Protected Arrays | RL          ║
╚══════════════════════════════════════════════════════════════════════════════╝

INITIALIZING ENHANCED ICT FEATURE EXTRACTION PIPELINE
======================================================================
📊 Initializing PD Array Detector...
🔧 Initializing Market Structure Detector...
💧 Initializing Liquidity Detector...
🕐 Initializing Session Analyzer...
🧲 Initializing Price Attractor System...
📈 Initializing Order Flow Analyzer...
🤖 Initializing Enhanced LLM Analyzer...

🔍 Running initial market analysis...
   ✓ Detected 42 PD arrays
   ✓ Market Structure: bullish
   ✓ Structure Breaks: 18
   ✓ Liquidity Zones: 54
   ✓ Order Flow Signals: 12
   ✓ Protected PD Arrays: 8
   ✓ Price Attractors: 67
✅ Pipeline initialization complete!
```

---

## 🏗️ System Architecture

### Pipeline Flow

```
┌─────────────────┐
│  Price OHLC     │
│   Data Input    │
└────────┬────────┘
         ↓
┌─────────────────────────────────────────────────────────────────┐
│                     DETECTION LAYER                             │
├─────────────────────────────────────────────────────────────────┤
│  • Enhanced PD Detector (smartmoneyconcepts)                    │
│    → Order Blocks, FVG, Breakers, EQH/EQL, BISI/SIBI          │
│                                                                 │
│  • Market Structure Detector                                    │
│    → BOS, CHOCH, Swing Points (HH, HL, LH, LL)               │
│                                                                 │
│  • Liquidity Detector                                          │
│    → Zones, Sweeps, Attraction Vectors                         │
└────────┬────────────────────────────────────────────────────────┘
         ↓
┌─────────────────────────────────────────────────────────────────┐
│                     ANALYSIS LAYER                              │
├─────────────────────────────────────────────────────────────────┤
│  • Session Analyzer → Kill Zones & Optimal Times               │
│  • Price Attractor System → Magnetic Zones & Protection        │
│  • Order Flow Analyzer → Volume, Delta, Engineering            │
│  • LLM Analyzer (optional) → AI Insights                       │
└────────┬────────────────────────────────────────────────────────┘
         ↓
┌─────────────────────────────────────────────────────────────────┐
│              FEATURE EXTRACTION PIPELINE                        │
│                                                                 │
│  Combines all components into 100+ dimensional feature vector:  │
│  • Price (10) • PD Arrays (15) • Structure (12)                │
│  • Liquidity (11) • Sessions (11) • Attractors (10)            │
│  • Protection (5) • Order Flow (10) • Technical (15)           │
│  • LLM (5)                                                      │
└────────┬────────────────────────────────────────────────────────┘
         ↓
┌─────────────────────────────────────────────────────────────────┐
│              REINFORCEMENT LEARNING AGENT                       │
│                                                                 │
│  • State: 100+ features                                         │
│  • Actions: BUY / SELL / HOLD                                   │
│  • Rewards: PnL + Teacher + Structure + Session + Risk         │
│  • Training: Curriculum learning with teacher guidance         │
└────────┬────────────────────────────────────────────────────────┘
         ↓
    Trading Signal
  (BUY/SELL/HOLD)
 with Confidence %
```

---

## 📁 Project Structure

```
ICT/
│
├── 🎯 Core Enhanced Components (10 files)
│   ├── enhanced_pd_detector.py          # Advanced PD array detection
│   ├── market_structure.py              # BOS, CHOCH analysis
│   ├── liquidity_detector.py            # Liquidity zones & vectors
│   ├── session_analyzer.py              # Kill zones & sessions
│   ├── price_attractor.py               # Attractors & protection
│   ├── order_flow_analyzer.py           # Order flow analysis
│   ├── enhanced_llm_analyzer.py         # LLM integration
│   ├── enhanced_rl_environment.py       # RL environment (100+ features)
│   ├── feature_extraction_pipeline.py   # Core integration layer
│   └── enhanced_main.py                 # Main execution script
│
├── 🔧 Supporting Files (11 files)
│   ├── data_structures.py               # Data classes (PDArray, etc.)
│   ├── utils.py                         # Utility functions
│   ├── data_fetcher.py                  # Real-time data fetching
│   ├── config.py                        # Configuration
│   ├── backtest.py                      # Backtesting framework
│   ├── paper_trading.py                 # Paper trading
│   ├── bot_trader.py                    # Automated trading bot
│   ├── smt_analyzer.py                  # SMT divergence detection
│   ├── check_price.py                   # Price checker
│   ├── main_realtime.py                 # Real-time trading
│   └── setup_and_run.py                 # Setup script
│
└── 📄 Documentation
    ├── README.md                        # This file
    ├── ENHANCED_README.md               # Detailed technical documentation
    └── requirements.txt                 # Python dependencies
```

---

## 💡 Usage Examples

### Basic Usage - Feature Extraction

```python
from feature_extraction_pipeline import FeatureExtractionPipeline
import pandas as pd

# Load your OHLCV data
df = pd.read_csv('your_data.csv')
# Ensure df has columns: ['open', 'high', 'low', 'close', 'volume', 'timestamp']

# Initialize pipeline (works without LLM)
pipeline = FeatureExtractionPipeline(
    df=df,
    timeframe='1h',
    use_llm=False  # Set to True if you have OpenAI API key
)

# Extract features for latest candle
latest_idx = len(df) - 1
features = pipeline.extract_features(latest_idx)
print(f"Extracted {len(features)} features: {features}")

# Get trading signal
signal = pipeline.get_trading_signal(latest_idx)
print(f"Signal: {signal['recommendation']}")
print(f"Confidence: {signal['confidence']}%")
print(f"Bullish Score: {signal['bullish_score']}")
print(f"Bearish Score: {signal['bearish_score']}")
```

### Advanced Usage - RL Training

```python
from feature_extraction_pipeline import FeatureExtractionPipeline
from enhanced_rl_environment import EnhancedICTTradingEnv, CurriculumLearningCallback
from stable_baselines3 import PPO
from stable_baselines3.common.vec_env import DummyVecEnv

# Initialize pipeline
pipeline = FeatureExtractionPipeline(df=your_df, timeframe='1h', use_llm=False)

# Create RL environment
def make_env():
    return EnhancedICTTradingEnv(
        feature_extractor=pipeline,
        initial_balance=10000,
        teacher_weight=1.0
    )

env = DummyVecEnv([make_env])

# Create PPO agent
model = PPO(
    "MlpPolicy",
    env,
    learning_rate=0.0003,
    n_steps=2048,
    batch_size=64,
    verbose=1
)

# Train with curriculum learning
curriculum_callback = CurriculumLearningCallback(
    initial_weight=1.0,
    final_weight=0.1,
    decay_steps=50000
)

model.learn(total_timesteps=50000, callback=[curriculum_callback])

# Save trained model
model.save("ict_agent.zip")

# Load and use for trading
model = PPO.load("ict_agent.zip")
obs, _ = env.reset()
action, _ = model.predict(obs, deterministic=True)
print(f"Agent action: {['HOLD', 'BUY', 'SELL'][action]}")
```

### LLM-Powered Analysis (Optional)

```python
from enhanced_llm_analyzer import EnhancedLLMAnalyzer
import os

# Set your OpenAI API key
os.environ['OPENAI_API_KEY'] = 'your-api-key'

# Initialize LLM analyzer
llm = EnhancedLLMAnalyzer()

# Strategy Creation from natural language
strategy = llm.create_strategy_from_description(
    "I want to buy when price is at a bullish order block in discount "
    "and there's a BOS confirmation during London kill zone"
)
print(strategy)

# Analyze RL training performance
metrics = {
    'total_steps': 50000,
    'avg_reward': 2.5,
    'win_rate': 0.65,
    'sharpe_ratio': 1.8,
    'max_drawdown': 0.15
}
analysis = llm.analyze_rl_performance(metrics)
print(analysis['recommendations'])

# Price history analysis
price_analysis = llm.analyze_price_history(df, analysis_type="comprehensive")
print(f"Trend: {price_analysis['overall_trend']}")
print(f"Patterns: {price_analysis['patterns_detected']}")
```

---

## 🎯 Component Details

### 1. Enhanced PD Array Detector

**File:** `enhanced_pd_detector.py`

Detects and classifies premium/discount arrays using smartmoneyconcepts library:

- **Order Blocks (OB)** - High-conviction supply/demand zones
- **Fair Value Gaps (FVG)** - Inefficient price gaps
- **Breaker Blocks** - Failed OBs that reverse polarity
- **Equal Highs/Lows (EQH/EQL)** - Liquidity pools
- **BISI/SIBI** - Buy/Sell-side imbalances

Each array includes:
- Strength score (0-100)
- Premium/Discount classification
- Protection score
- Confluence count

### 2. Market Structure Detector

**File:** `market_structure.py`

Analyzes price structure and trends:

- **Break of Structure (BOS)** - Trend continuation signals
- **Change of Character (CHOCH)** - Trend reversal detection
- **Swing Points** - HH, HL, LH, LL classification
- **Trend Determination** - Bullish, bearish, or neutral
- **Structure Alignment** - Multi-level confirmation

### 3. Liquidity Detector

**File:** `liquidity_detector.py`

Identifies and tracks liquidity:

- **Liquidity Zones** - Buy-side, sell-side, internal
- **Liquidity Sweeps** - Stop hunts and grabs
- **Attraction Vectors** - Price magnetism with distance decay
- **Net Attraction** - Overall directional pull
- **Liquidity Profile** - Imbalance and distribution analysis

### 4. Session Analyzer

**File:** `session_analyzer.py`

Time-based trading intelligence:

- **Asian Kill Zone** (00:00-03:00 UTC)
- **London Open** (02:00-05:00 UTC) - Priority: 9/10
- **London Kill Zone** (06:00-09:00 UTC) - Priority: 8/10
- **NY AM Kill Zone** (12:00-15:00 UTC) - **Priority: 10/10**
- **NY Lunch** (16:00-18:00 UTC) - Priority: 3/10
- **NY PM Kill Zone** (18:30-21:00 UTC) - Priority: 7/10

### 5. Price Attractor System

**File:** `price_attractor.py`

Magnetic price zones:

- **PD Array Attractors** - Arrays act as magnets
- **Liquidity Attractors** - Price drawn to liquidity
- **VWAP Attractors** - Volume-weighted average price
- **Fibonacci Attractors** - Key retracement levels
- **Round Number Attractors** - Psychological levels
- **Protected Arrays** - High-confluence zones (protection score 50-100)

### 6. Order Flow Analyzer

**File:** `order_flow_analyzer.py`

Smart money behavior detection:

- **Volume Absorption** - High volume, low movement
- **Exhaustion Moves** - Climactic price action
- **Accumulation/Distribution** - Smart money phases
- **Delta Analysis** - Buy/sell pressure
- **Liquidity Engineering** - Stop hunts, fake breakouts

### 7. Enhanced LLM Analyzer

**File:** `enhanced_llm_analyzer.py`

AI-powered insights (optional):

- **Strategy Creation** - Natural language to rules
- **RL Training Help** - Performance analysis & suggestions
- **Price Analysis** - Pattern recognition
- **Agent Interpretation** - Explain RL decisions
- **Market Context** - Comprehensive analysis

Works with or without OpenAI API - has rule-based fallbacks.

### 8. Enhanced RL Environment

**File:** `enhanced_rl_environment.py`

Advanced reinforcement learning:

- **100+ dimensional state space**
- **Teacher-guided learning** - ICT rules + LLM guide the agent
- **Curriculum learning** - Teacher weight decays over time
- **Multi-reward system**:
  - PnL rewards (primary)
  - Teacher alignment rewards
  - Structure alignment rewards
  - Session quality rewards
  - Risk management rewards

### 9. Feature Extraction Pipeline

**File:** `feature_extraction_pipeline.py`

**The core integration layer** - combines all components:

Extracts 100+ features across 10 categories:
1. Price Features (10)
2. PD Array Features (15)
3. Market Structure Features (12)
4. Liquidity Features (11)
5. Session Features (11)
6. Attractor Features (10)
7. Protection Features (5)
8. Order Flow Features (10)
9. Technical Indicators (15)
10. LLM Features (5)

---

## 📊 Feature Breakdown (100+ Total)

| Category | Count | Examples |
|----------|-------|----------|
| **Price** | 10 | OHLC normalized, price changes, MAs, candle type |
| **PD Arrays** | 15 | OB/FVG counts, distances, strengths, protected count |
| **Structure** | 12 | Trend, BOS/CHOCH counts, swing points, alignment |
| **Liquidity** | 11 | Zone counts, imbalance, net attraction, swept count |
| **Sessions** | 11 | Kill zone status, priority, session encoding, time |
| **Attractors** | 10 | Net direction, attraction strengths, distances |
| **Protection** | 5 | Protected array count, avg score, confluence |
| **Order Flow** | 10 | Volume ratio, delta, absorption, accumulation |
| **Technical** | 15 | RSI, Bollinger Bands, momentum, volatility |
| **LLM** | 5 | Confidence, bias, alignment, risk, recommendation |
| **Total** | **104** | **Comprehensive market state representation** |

---

## 🔧 Configuration

### Session Times (Customizable)

Edit `session_analyzer.py` to adjust kill zone times:

```python
'ny_am_killzone': TradingSession(
    name='New York AM Kill Zone',
    start_time=time(12, 0),   # UTC
    end_time=time(15, 0),
    killzone=True,
    priority=10  # Highest priority
)
```

### RL Hyperparameters

Adjust in your training script:

```python
model = PPO(
    "MlpPolicy",
    env,
    learning_rate=0.0003,    # Learning rate
    n_steps=2048,            # Steps per update
    batch_size=64,           # Batch size
    n_epochs=10,             # Epochs per update
    gamma=0.99,              # Discount factor
    gae_lambda=0.95,         # GAE lambda
    clip_range=0.2,          # PPO clip range
    verbose=1
)
```

---

## 📈 Performance Expectations

| Metric | Value |
|--------|-------|
| Feature Extraction | ~100ms per candle |
| PD Arrays Detected | 20-50 per 500 candles |
| Structure Breaks | 10-30 BOS/CHOCH per 500 candles |
| Liquidity Zones | 30-60 zones per 500 candles |
| RL Training (CPU) | ~5-10 min for 50k steps |
| Signal Generation | <50ms per signal |
| Memory Usage | ~500MB-1GB |

---

## 🔍 Troubleshooting

### Module Import Errors

```bash
# Ensure you're in the ICT directory
cd C:\Users\a5153207\Documents\code\ICT

# Activate virtual environment
.venv\Scripts\activate

# Reinstall dependencies
pip install -r requirements.txt
```

### smartmoneyconcepts Errors

The system automatically falls back to manual detection if smartmoneyconcepts fails. No action needed.

### LLM Not Working

The system works perfectly without LLM - all features have rule-based fallbacks.

To enable LLM:
```bash
# Set your API key
set OPENAI_API_KEY=your-key-here

# Or in Python
import os
os.environ['OPENAI_API_KEY'] = 'your-key-here'
```

### RL Training Too Slow

```python
# Reduce training steps for testing
model.learn(total_timesteps=10000)  # Instead of 50000

# Use smaller dataset
df = df.tail(200)  # Last 200 candles only

# Reduce PPO steps
model = PPO("MlpPolicy", env, n_steps=512)  # Instead of 2048
```

### Memory Issues

```python
# Process in chunks
chunk_size = 100
for i in range(0, len(df), chunk_size):
    chunk = df.iloc[i:i+chunk_size]
    # Process chunk

# Or use smaller lookback periods
pipeline = FeatureExtractionPipeline(df, timeframe='1h', lookback=20)
```

---

## 📚 Additional Resources

### Documentation

- **ENHANCED_README.md** - Detailed technical documentation
- **requirements.txt** - Python dependencies
- **Code comments** - Extensive inline documentation

### External Resources

- [smartmoneyconcepts Documentation](https://github.com/joshyattridge/smart-money-concepts)
- [Stable-Baselines3 Documentation](https://stable-baselines3.readthedocs.io/)
- [Gymnasium Documentation](https://gymnasium.farama.org/)

---

## 🚧 Future Enhancements

Potential additions:

- [ ] Multi-timeframe alignment
- [ ] Real-time WebSocket integration
- [ ] Advanced backtesting with portfolio simulation
- [ ] Web dashboard with live charts
- [ ] Alert system (SMS, email, Discord)
- [ ] Multi-asset correlation (SMT)
- [ ] Risk management module
- [ ] Position sizing calculator
- [ ] Trade journal integration

---

## ⚠️ Disclaimer

**This software is for educational and research purposes only.**

- Trading cryptocurrencies and financial instruments involves substantial risk
- Past performance does not guarantee future results
- This is not financial advice
- Always test thoroughly with paper trading before using real capital
- Use at your own risk

---

## 📄 License

MIT License - Free to use and modify

---

## 🤝 Contributing

Contributions welcome! Areas of interest:

- Additional PD array types
- More sophisticated order flow analysis
- Backtesting improvements
- Performance optimizations
- Documentation improvements

---

## 📞 Support

For issues or questions:

1. Check the **ENHANCED_README.md** for detailed technical information
2. Review code comments for implementation details
3. Check troubleshooting section above

---

## 🎉 Acknowledgments

Built with:
- Python 3.8+
- smartmoneyconcepts
- stable-baselines3
- gymnasium
- OpenAI GPT-4o (optional)

**Version:** 2.0.0 - Complete Enhanced System

**Last Updated:** 2025

---

Made with ❤️ by ICT Trading System Team
