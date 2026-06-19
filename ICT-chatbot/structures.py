"""
Data structures for ICT RL Trading Bot
"""
from dataclasses import dataclass, field
from datetime import datetime
from enum import Enum
from typing import Optional


class Direction(Enum):
    """Trade direction"""
    LONG = 1
    SHORT = -1
    NEUTRAL = 0


class BiasDirection(Enum):
    """Higher timeframe bias"""
    BULLISH = 1
    BEARISH = -1
    NEUTRAL = 0


class PDZone(Enum):
    """Premium/Discount zone classification"""
    PREMIUM = 1
    EQUILIBRIUM = 0
    DISCOUNT = -1


class ExitReason(Enum):
    """Position exit reasons"""
    TAKE_PROFIT = "TP"
    STOP_LOSS = "SL"
    SESSION_END = "SESSION_END"
    MAX_HOLD = "MAX_HOLD"


@dataclass
class FVG:
    """Fair Value Gap structure"""
    fvg_type: str  # 'BULLISH' or 'BEARISH'
    top: float
    bottom: float
    created_bar: int
    created_time: datetime
    age: int = 0
    mitigated: bool = False

    def get_size(self) -> float:
        """Return gap size"""
        return self.top - self.bottom

    def contains_price(self, price: float) -> bool:
        """Check if price is inside FVG zone"""
        return self.bottom <= price <= self.top


@dataclass
class SwingPoint:
    """Swing high/low structure"""
    swing_type: str  # 'HIGH' or 'LOW'
    price: float
    bar_index: int
    timestamp: datetime
    swept: bool = False


@dataclass
class MSS:
    """Market Structure Shift"""
    direction: BiasDirection
    structure_level: float
    break_bar: int
    break_time: datetime
    bars_since: int = 0


@dataclass
class LiquiditySweep:
    """Liquidity sweep event"""
    direction: str  # 'BULLISH' or 'BEARISH'
    level: float
    sweep_bar: int
    sweep_time: datetime
    bars_since: int = 0


@dataclass
class Position:
    """Trading position state"""
    direction: Direction
    entry_price: float
    entry_time: datetime
    entry_bar_idx: int

    stop_loss: float
    take_profit: float

    position_size: float
    risk_amount: float

    # Tracking
    unrealized_pnl: float = 0.0
    bars_held: int = 0
    highest_pnl: float = 0.0
    lowest_pnl: float = 0.0

    # Exit info (filled on close)
    exit_price: Optional[float] = None
    exit_time: Optional[datetime] = None
    exit_reason: Optional[ExitReason] = None
    realized_pnl: Optional[float] = None

    def update_unrealized_pnl(self, current_price: float):
        """Update unrealized PnL based on current price"""
        if self.direction == Direction.LONG:
            self.unrealized_pnl = (current_price - self.entry_price) * self.position_size
        else:  # SHORT
            self.unrealized_pnl = (self.entry_price - current_price) * self.position_size

        # Track extremes
        self.highest_pnl = max(self.highest_pnl, self.unrealized_pnl)
        self.lowest_pnl = min(self.lowest_pnl, self.unrealized_pnl)

    def get_risk_multiple(self) -> float:
        """Return current PnL in risk multiples (R)"""
        if self.risk_amount == 0:
            return 0.0
        return self.unrealized_pnl / self.risk_amount

    def to_dict(self) -> dict:
        """Convert to dictionary for logging"""
        return {
            'direction': self.direction.name,
            'entry_price': self.entry_price,
            'entry_time': self.entry_time,
            'stop_loss': self.stop_loss,
            'take_profit': self.take_profit,
            'position_size': self.position_size,
            'unrealized_pnl': self.unrealized_pnl,
            'bars_held': self.bars_held,
            'exit_price': self.exit_price,
            'exit_time': self.exit_time,
            'exit_reason': self.exit_reason.value if self.exit_reason else None,
            'realized_pnl': self.realized_pnl,
        }


@dataclass
class TradeRecord:
    """Completed trade record"""
    entry_time: datetime
    exit_time: datetime
    direction: str
    entry_price: float
    exit_price: float
    exit_reason: str
    pnl: float
    pnl_r: float  # PnL in R multiples
    balance_after: float
    bars_held: int
    session_number: int

    def to_dict(self) -> dict:
        """Convert to dictionary"""
        return {
            'entry_time': self.entry_time,
            'exit_time': self.exit_time,
            'direction': self.direction,
            'entry_price': self.entry_price,
            'exit_price': self.exit_price,
            'exit_reason': self.exit_reason,
            'pnl': self.pnl,
            'pnl_r': self.pnl_r,
            'balance_after': self.balance_after,
            'bars_held': self.bars_held,
            'session_number': self.session_number,
        }


@dataclass
class EpisodeStats:
    """Episode statistics"""
    total_trades: int = 0
    winning_trades: int = 0
    losing_trades: int = 0
    total_pnl: float = 0.0
    max_drawdown: float = 0.0
    final_balance: float = 0.0
    sharpe_ratio: float = 0.0
    max_consecutive_losses: int = 0

    def win_rate(self) -> float:
        """Calculate win rate"""
        if self.total_trades == 0:
            return 0.0
        return self.winning_trades / self.total_trades

    def profit_factor(self, gross_profit: float, gross_loss: float) -> float:
        """Calculate profit factor"""
        if gross_loss == 0:
            return float('inf') if gross_profit > 0 else 0.0
        return gross_profit / abs(gross_loss)
