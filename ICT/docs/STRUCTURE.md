# Project Structure

This document explains the organization of the Enhanced ICT Trading System.

## Directory Structure

```
ICT/
│
├── 📁 core/                           # Core ICT Components
│   ├── __init__.py
│   ├── enhanced_pd_detector.py        # PD array detection (OB, FVG, Breaker, EQH/EQL)
│   ├── market_structure.py            # BOS, CHOCH, swing points
│   ├── liquidity_detector.py          # Liquidity zones & vectors
│   ├── session_analyzer.py            # Kill zones & sessions
│   ├── price_attractor.py             # Price attractors & protected arrays
│   ├── order_flow_analyzer.py         # Order flow & liquidity engineering
│   └── smt_analyzer.py                # SMT divergence detection
│
├── 📁 analysis/                       # Analysis & Intelligence
│   ├── __init__.py
│   ├── enhanced_llm_analyzer.py       # LLM integration (strategy, RL help, etc.)
│   └── feature_extraction_pipeline.py # Core integration layer (100+ features)
│
├── 📁 rl/                             # Reinforcement Learning
│   ├── __init__.py
│   └── enhanced_rl_environment.py     # RL environment with curriculum learning
│
├── 📁 trading/                        # Trading Execution
│   ├── __init__.py
│   ├── bot_trader.py                  # Automated trading bot
│   ├── paper_trading.py               # Paper trading implementation
│   └── backtest.py                    # Backtesting framework
│
├── 📁 data/                           # Data Management
│   ├── __init__.py
│   ├── data_fetcher.py                # Real-time data fetching
│   └── data_structures.py             # Data classes (PDArray, SMTDivergence)
│
├── 📁 utils/                          # Utilities
│   ├── __init__.py
│   ├── utils.py                       # Helper functions
│   ├── config.py                      # Configuration
│   └── check_price.py                 # Price checker
│
├── 📁 scripts/                        # Executable Scripts
│   ├── enhanced_main.py               # Main demo/test script
│   ├── main_realtime.py               # Real-time trading script
│   └── setup_and_run.py               # Setup automation
│
├── 📁 docs/                           # Documentation
│   ├── README.md                      # Main documentation
│   ├── ENHANCED_README.md             # Detailed technical docs
│   ├── STRUCTURE.md                   # This file
│   ├── QUICK_START.md                 # Quick start guide
│   ├── REALTIME_GUIDE.md              # Real-time trading guide
│   └── PAPER_TRADING_GUIDE.md         # Paper trading guide
│
├── 📁 models/                         # Saved ML Models (created at runtime)
├── 📁 logs/                           # Application Logs (created at runtime)
├── 📁 tensorboard_logs/               # TensorBoard Logs (created at runtime)
│
├── __init__.py                        # Root package initializer
├── run.py                             # Main entry point
├── requirements.txt                   # Python dependencies
└── .gitignore                         # Git ignore rules
```

## Package Organization

### Core Components (`core/`)

The heart of the ICT analysis system. Contains all ICT-specific detectors and analyzers:

- **Enhanced PD Detector**: Uses `smartmoneyconcepts` for professional-grade detection
- **Market Structure**: Analyzes BOS, CHOCH, and swing points
- **Liquidity Detector**: Identifies zones, sweeps, and attraction vectors
- **Session Analyzer**: Time-based analysis with kill zones
- **Price Attractor**: Magnetic zones and protected arrays
- **Order Flow Analyzer**: Volume and delta analysis
- **SMT Analyzer**: Multi-asset correlation divergences

### Analysis & Intelligence (`analysis/`)

High-level analysis and feature extraction:

- **Enhanced LLM Analyzer**: AI-powered insights (strategy creation, RL help, price analysis)
- **Feature Extraction Pipeline**: Integrates all components into 100+ dimensional feature space

### Reinforcement Learning (`rl/`)

Machine learning components:

- **Enhanced RL Environment**: Trading environment with teacher-guided learning
- **Callbacks**: Curriculum learning, performance monitoring, early stopping

### Trading Execution (`trading/`)

Trading implementation:

- **Bot Trader**: Automated trading bot
- **Paper Trading**: Risk-free testing
- **Backtest**: Historical performance testing

### Data Management (`data/`)

Data structures and fetching:

- **Data Structures**: PDArray, SMTDivergence classes
- **Data Fetcher**: Real-time data from exchanges

### Utilities (`utils/`)

Helper functions and configuration:

- **Utils**: Sample data generation, position sizing, risk calculations
- **Config**: System configuration
- **Check Price**: Price validation utilities

### Scripts (`scripts/`)

Executable entry points:

- **enhanced_main.py**: Interactive demo and training
- **main_realtime.py**: Live trading with real-time data
- **setup_and_run.py**: Automated setup

## Import Structure

### Using the Package

From project root:

```python
# Option 1: Import from root package
from core import EnhancedPDDetector, MarketStructureDetector
from analysis import FeatureExtractionPipeline
from rl import EnhancedICTTradingEnv

# Option 2: Import directly
from core.enhanced_pd_detector import EnhancedPDDetector
from analysis.feature_extraction_pipeline import FeatureExtractionPipeline
from rl.enhanced_rl_environment import EnhancedICTTradingEnv
```

### From Scripts

Scripts automatically add parent directory to path:

```python
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from analysis.feature_extraction_pipeline import FeatureExtractionPipeline
from rl.enhanced_rl_environment import EnhancedICTTradingEnv
```

### Relative Imports Within Packages

Within `core/` package:

```python
# In core/price_attractor.py
from .enhanced_pd_detector import EnhancedPDArray
from .liquidity_detector import LiquidityZone
```

## Running the System

### Quick Start

```bash
# From project root
python run.py

# Or directly
python scripts/enhanced_main.py
```

### Real-time Trading

```bash
python scripts/main_realtime.py --mode realtime
```

### Custom Scripts

```python
# your_script.py in project root
from analysis import FeatureExtractionPipeline
from utils import generate_sample_data

# Your code here
data = generate_sample_data(timeframes=['1h'], num_candles=500)
pipeline = FeatureExtractionPipeline(df=data['1h'], timeframe='1h')
```

## Development Workflow

### Adding New Components

1. **Core Component**: Add to `core/` and update `core/__init__.py`
2. **Analysis Tool**: Add to `analysis/` and update `analysis/__init__.py`
3. **RL Feature**: Add to `rl/` and update `rl/__init__.py`
4. **Trading Logic**: Add to `trading/`
5. **Utility**: Add to `utils/` and update `utils/__init__.py`

### Testing Changes

```bash
# Test specific component
python -m pytest tests/test_core.py

# Run full system test
python scripts/enhanced_main.py
```

## File Naming Conventions

- **Modules**: `lowercase_with_underscores.py`
- **Classes**: `PascalCase`
- **Functions**: `lowercase_with_underscores()`
- **Constants**: `UPPERCASE_WITH_UNDERSCORES`

## Dependencies

All dependencies are in `requirements.txt`:

```bash
pip install -r requirements.txt
```

Key packages:
- `smartmoneyconcepts` - PD array detection
- `stable-baselines3` - Reinforcement learning
- `gymnasium` - RL environments
- `openai` - LLM integration (optional)
- `pandas`, `numpy`, `scipy` - Data processing
- `torch` - Neural networks

## Configuration

### Environment Variables

```bash
# Optional: For LLM features
export OPENAI_API_KEY="your-key-here"
```

### Config Files

Edit `utils/config.py` for system-wide settings.

## Logging

Logs are saved to `logs/` directory:

- `logs/trading.log` - Trading activity
- `logs/errors.log` - Error messages
- `tensorboard_logs/` - RL training metrics

## Models

Trained models are saved to `models/`:

- `models/ict_agent.zip` - Trained RL agent
- `models/best_model.zip` - Best performing model

## Best Practices

1. **Always run from project root**: `python run.py` or `python scripts/enhanced_main.py`
2. **Use relative imports within packages**: `from .module import Class`
3. **Use absolute imports from root**: `from core.module import Class`
4. **Keep sensitive data out of repo**: Use `.env` files (gitignored)
5. **Document new components**: Add docstrings and update this file

## Troubleshooting

### Import Errors

```python
# If you get import errors, make sure you're running from project root
import sys
sys.path.insert(0, '.')  # Add current directory to path
```

### Module Not Found

```bash
# Ensure virtual environment is activated
.venv\Scripts\activate  # Windows
source .venv/bin/activate  # Linux/Mac

# Reinstall dependencies
pip install -r requirements.txt
```

## Migration from Old Structure

Old files were organized into packages:

- `pd_arrays.py` → `core/enhanced_pd_detector.py`
- `ict_indicators.py` → integrated into `core/*`
- `rl_environment.py` → `rl/enhanced_rl_environment.py`
- `llm_analyzer.py` → `analysis/enhanced_llm_analyzer.py`
- `trading_system.py` → `analysis/feature_extraction_pipeline.py`
- `main.py` → `scripts/enhanced_main.py`

## Future Structure Plans

Planned additions:

- `tests/` - Unit and integration tests
- `notebooks/` - Jupyter notebooks for analysis
- `configs/` - Multiple configuration profiles
- `strategies/` - Pre-built trading strategies
- `api/` - REST API for remote access

---

**Last Updated**: 2025
**Structure Version**: 2.0.0
