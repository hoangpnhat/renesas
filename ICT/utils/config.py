"""
ICT Trading System - Configuration
==================================
Configuration settings and API keys
"""

import os

# OpenAI API Configuration
OPENAI_API_KEY = os.getenv('OPENAI_API_KEY', '')

# Trading Configuration
DEFAULT_INITIAL_BALANCE = 10000
DEFAULT_RISK_PER_TRADE = 1.0
DEFAULT_MIN_CONFIDENCE = 75.0

# Data Fetching Configuration
DEFAULT_TIMEFRAMES = ['1h', '4h', '1d']
DEFAULT_CANDLES_LIMIT = 500

# Bot Configuration
DEFAULT_UPDATE_INTERVAL = 60  # seconds
DEFAULT_DURATION = 60  # minutes

# RL Configuration
DEFAULT_RL_TIMESTEPS = 50000
DEFAULT_TEACHER_WEIGHT_INITIAL = 1.0
DEFAULT_TEACHER_WEIGHT_FINAL = 0.1


def set_openai_key(api_key: str):
    """Set OpenAI API key"""
    global OPENAI_API_KEY
    OPENAI_API_KEY = api_key
    os.environ['OPENAI_API_KEY'] = api_key
    print(f"✅ OpenAI API key configured (length: {len(api_key)})")


def get_openai_key() -> str:
    """Get OpenAI API key"""
    return OPENAI_API_KEY or os.getenv('OPENAI_API_KEY', '')
