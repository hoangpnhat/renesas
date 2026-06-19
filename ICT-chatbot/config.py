"""
Configuration file for ICT RL Trading Bot
All parameters locked from Phase 0, 1, and 2
"""

# ============================================================================
# TIMEFRAME CONFIGURATION
# ============================================================================
STRUCTURE_TIMEFRAME = '15T'  # 15 minutes (for MSS, bias, PD zones)
EXECUTION_TIMEFRAME = '1T'   # 1 minute (for sweeps, FVG, entries)
INTERMEDIATE_TIMEFRAME = '30T'  # 30 minutes (intermediate structure / confluence)

# ============================================================================
# SESSION CONFIGURATION
# ============================================================================
LONDON_KILLZONE_START = 7  # 07:00 GMT
LONDON_KILLZONE_END = 10   # 10:00 GMT
TIMEZONE = 'GMT'

# ============================================================================
# RISK MANAGEMENT (HARD-CODED, NON-NEGOTIABLE)
# ============================================================================
INITIAL_BALANCE = 10000.0
RISK_PER_TRADE = 0.01  # 1% of account
COMMISSION = 5.0       # $5 per round trip
MIN_RISK_REWARD = 2.0  # Minimum 2:1 RR
MAX_TRADES_PER_SESSION = 3
MAX_HOLD_BARS = 100    # Maximum bars to hold position (100 minutes)

# ============================================================================
# ICT INDICATOR PARAMETERS
# ============================================================================
# Liquidity Sweep (1M)
SWEEP_PIVOT_WINDOW = 5   # N-bar pivot for swing highs/lows
SWEEP_REVERSAL_BARS = 10  # Max bars for reversal confirmation
SWEEP_LOOKBACK = 50      # Bars to look back for swing levels

# Market Structure Shift (15M)
MSS_LOOKBACK = 30        # Bars to analyze structure
MSS_STRUCTURE_PIVOTS = 3  # Pivot window for structure points
MSS_MIN_MOVE_ATR_MULT = 2.0  # Minimum move = 2 * ATR(14)
MSS_VALIDITY_BARS = 8    # MSS valid for 8 bars (2 hours on 15M)

# Fair Value Gap (1M)
FVG_MIN_SIZE_ATR_MULT = 0.3  # Minimum gap = 0.3 * ATR(14)
FVG_MAX_AGE = 100            # FVG expires after 100 bars
FVG_MAX_TRACKED = 5          # Track max 5 FVGs per direction

# Premium/Discount (15M)
PD_SWING_WINDOW = 50     # Bars for swing high/low
PD_THRESHOLD = 0.1       # 10% buffer around equilibrium

# 30M OHLC Parameters
OHLC_30M_SWING_WINDOW = 20   # Bars for 30M swing high/low (20 bars = ~10 hours)
OHLC_30M_LOOKBACK = 48       # Minimum 30M bars before episode can start (24 hours)

# ATR Parameters
ATR_PERIOD = 14

# ============================================================================
# ENVIRONMENT PARAMETERS
# ============================================================================
LOOKBACK_1M = 200       # Minimum 1M bars before episode can start
LOOKBACK_15M = 80       # Minimum 15M bars before episode can start
LOOKBACK_30M = 48       # Minimum 30M bars before episode can start (24 hours)
EPISODE_LENGTH = 1800   # Max steps per episode (1 week of 1M bars)
ACCOUNT_STOP_PCT = 0.5  # Stop if balance < 50% of initial

# ============================================================================
# OBSERVATION SPACE
# ============================================================================
OBSERVATION_DIM = 38    # Expanded from 32 → 38 (added 6 30M features)
OBSERVATION_LOW = -10.0
OBSERVATION_HIGH = 10.0

# ============================================================================
# ACTION SPACE
# ============================================================================
ACTION_HOLD = 0
ACTION_LONG = 1
ACTION_SHORT = 2
NUM_ACTIONS = 3

# ============================================================================
# FEATURE NAMES (for reference and debugging)
# ============================================================================
FEATURE_NAMES = [
    # Group A: Market Structure 15M (6)
    'htf_bias',
    'bars_since_mss',
    'distance_to_structure',
    'swing_high_15m',
    'swing_low_15m',
    'pd_zone',

    # Group A2: 30M OHLC Structure (6)  ← NEW
    'swing_high_30m',
    'swing_low_30m',
    'candle_body_ratio_30m',
    'candle_direction_30m',
    'atr_30m',
    'pd_zone_30m',

    # Group B: Liquidity (5)
    'sweep_detected',
    'bars_since_sweep',
    'sweep_level_distance',
    'num_swing_highs',
    'num_swing_lows',

    # Group C: FVG (6)
    'num_bullish_fvg',
    'num_bearish_fvg',
    'nearest_bull_fvg_dist',
    'nearest_bear_fvg_dist',
    'nearest_fvg_age',
    'price_in_fvg',

    # Group D: Time/Session (5)
    'in_london_killzone',
    'minutes_into_session',
    'hour_sin',
    'hour_cos',
    'day_of_week',

    # Group E: Price Action (5)
    'atr_1m',
    'atr_15m',
    'candle_body_ratio',
    'candle_direction',
    'price_velocity',

    # Group F: Position/Risk (5)
    'has_position',
    'position_direction',
    'unrealized_pnl',
    'bars_in_position',
    'trades_taken_today',
]

assert len(FEATURE_NAMES) == OBSERVATION_DIM, "Feature names mismatch observation dim"

# ============================================================================
# REWARD PARAMETERS (Placeholder for Phase 3)
# ============================================================================
REWARD_INVALID_ACTION = -20
REWARD_MISSED_OPPORTUNITY = -10
REWARD_CORRECT_PATIENCE = 1
REWARD_WRONG_DIRECTION = -50
REWARD_CORRECT_ENTRY = 50