"""
ICT Trading System - Paper Trading Engine
=========================================
Simulate real trading with paper money
"""

import json
import pandas as pd
from datetime import datetime
from typing import Dict, List, Optional
from dataclasses import dataclass, asdict


@dataclass
class Trade:
    """Represents a single trade"""
    id: int
    timestamp: str
    symbol: str
    side: str  # 'BUY' or 'SELL'
    entry_price: float
    size: float
    stop_loss: Optional[float] = None
    take_profit: Optional[float] = None
    exit_price: Optional[float] = None
    exit_timestamp: Optional[str] = None
    pnl: float = 0.0
    pnl_percent: float = 0.0
    status: str = 'OPEN'  # OPEN, CLOSED, STOPPED_OUT
    signal_confidence: float = 0.0
    signal_components: Dict = None

    def __post_init__(self):
        if self.signal_components is None:
            self.signal_components = {}


class PaperTradingAccount:
    """Paper trading account with position management"""

    def __init__(self, initial_balance: float = 10000.0, max_risk_per_trade: float = 1.0):
        self.initial_balance = initial_balance
        self.balance = initial_balance
        self.max_risk_per_trade = max_risk_per_trade  # Percentage
        self.trades: List[Trade] = []
        self.open_positions: Dict[str, Trade] = {}
        self.trade_counter = 0
        self.equity_curve = []

    def calculate_position_size(self, entry_price: float, stop_loss: float) -> float:
        """Calculate position size based on risk management"""
        risk_amount = self.balance * (self.max_risk_per_trade / 100)
        price_risk = abs(entry_price - stop_loss)

        if price_risk == 0:
            return 0

        position_size = risk_amount / price_risk
        max_size = (self.balance * 0.95) / entry_price  # Max 95% of balance

        return min(position_size, max_size)

    def open_trade(self, symbol: str, side: str, entry_price: float,
                   stop_loss: float, take_profit: float,
                   signal_confidence: float = 0.0,
                   signal_components: Dict = None) -> Optional[Trade]:
        """Open a new trade"""

        # Check if already have position
        if symbol in self.open_positions:
            print(f"⚠️  Already have open position for {symbol}")
            return None

        # Calculate position size
        size = self.calculate_position_size(entry_price, stop_loss)

        if size <= 0:
            print(f"⚠️  Invalid position size: {size}")
            return None

        # Create trade
        self.trade_counter += 1
        trade = Trade(
            id=self.trade_counter,
            timestamp=datetime.now().isoformat(),
            symbol=symbol,
            side=side,
            entry_price=entry_price,
            size=size,
            stop_loss=stop_loss,
            take_profit=take_profit,
            signal_confidence=signal_confidence,
            signal_components=signal_components or {}
        )

        self.trades.append(trade)
        self.open_positions[symbol] = trade

        print(f"✅ Opened {side} position #{trade.id}")
        print(f"   Symbol: {symbol}")
        print(f"   Entry: ${entry_price:,.2f}")
        print(f"   Size: {size:.4f}")
        print(f"   Stop Loss: ${stop_loss:,.2f}")
        print(f"   Take Profit: ${take_profit:,.2f}")
        print(f"   Risk: {self.max_risk_per_trade}% (${self.balance * self.max_risk_per_trade / 100:,.2f})")

        return trade

    def close_trade(self, symbol: str, exit_price: float, reason: str = "MANUAL") -> Optional[Trade]:
        """Close an open trade"""

        if symbol not in self.open_positions:
            print(f"⚠️  No open position for {symbol}")
            return None

        trade = self.open_positions[symbol]

        # Calculate P&L
        if trade.side == 'BUY':
            pnl = (exit_price - trade.entry_price) * trade.size
            pnl_percent = ((exit_price - trade.entry_price) / trade.entry_price) * 100
        else:  # SELL
            pnl = (trade.entry_price - exit_price) * trade.size
            pnl_percent = ((trade.entry_price - exit_price) / trade.entry_price) * 100

        # Update trade
        trade.exit_price = exit_price
        trade.exit_timestamp = datetime.now().isoformat()
        trade.pnl = pnl
        trade.pnl_percent = pnl_percent
        trade.status = reason

        # Update balance
        self.balance += pnl

        # Record equity
        self.equity_curve.append({
            'timestamp': trade.exit_timestamp,
            'balance': self.balance,
            'trade_id': trade.id,
            'pnl': pnl
        })

        # Remove from open positions
        del self.open_positions[symbol]

        pnl_emoji = "💰" if pnl > 0 else "📉"
        print(f"{pnl_emoji} Closed {trade.side} position #{trade.id} - {reason}")
        print(f"   Exit: ${exit_price:,.2f}")
        print(f"   P&L: ${pnl:,.2f} ({pnl_percent:+.2f}%)")
        print(f"   New Balance: ${self.balance:,.2f}")

        return trade

    def update_positions(self, current_price: float, symbol: str = None):
        """Check and update open positions (stop loss / take profit)"""

        if symbol and symbol in self.open_positions:
            positions_to_check = [self.open_positions[symbol]]
        else:
            positions_to_check = list(self.open_positions.values())

        for trade in positions_to_check:
            # Check stop loss
            if trade.side == 'BUY' and current_price <= trade.stop_loss:
                self.close_trade(trade.symbol, trade.stop_loss, "STOPPED_OUT")
            elif trade.side == 'SELL' and current_price >= trade.stop_loss:
                self.close_trade(trade.symbol, trade.stop_loss, "STOPPED_OUT")

            # Check take profit
            elif trade.side == 'BUY' and current_price >= trade.take_profit:
                self.close_trade(trade.symbol, trade.take_profit, "TAKE_PROFIT")
            elif trade.side == 'SELL' and current_price <= trade.take_profit:
                self.close_trade(trade.symbol, trade.take_profit, "TAKE_PROFIT")

    def get_unrealized_pnl(self, current_price: float, symbol: str) -> float:
        """Calculate unrealized P&L for open position"""

        if symbol not in self.open_positions:
            return 0.0

        trade = self.open_positions[symbol]

        if trade.side == 'BUY':
            return (current_price - trade.entry_price) * trade.size
        else:
            return (trade.entry_price - current_price) * trade.size

    def get_statistics(self) -> Dict:
        """Get trading statistics"""

        closed_trades = [t for t in self.trades if t.status != 'OPEN']

        if not closed_trades:
            return {
                'total_trades': 0,
                'winning_trades': 0,
                'losing_trades': 0,
                'win_rate': 0.0,
                'total_pnl': 0.0,
                'total_pnl_percent': 0.0,
                'avg_win': 0.0,
                'avg_loss': 0.0,
                'profit_factor': 0.0,
                'max_drawdown': 0.0,
                'current_balance': self.balance,
                'roi': 0.0
            }

        winning_trades = [t for t in closed_trades if t.pnl > 0]
        losing_trades = [t for t in closed_trades if t.pnl <= 0]

        total_wins = sum(t.pnl for t in winning_trades)
        total_losses = abs(sum(t.pnl for t in losing_trades))

        # Calculate max drawdown
        peak = self.initial_balance
        max_dd = 0
        for equity in self.equity_curve:
            if equity['balance'] > peak:
                peak = equity['balance']
            dd = (peak - equity['balance']) / peak * 100
            max_dd = max(max_dd, dd)

        return {
            'total_trades': len(closed_trades),
            'winning_trades': len(winning_trades),
            'losing_trades': len(losing_trades),
            'win_rate': (len(winning_trades) / len(closed_trades) * 100) if closed_trades else 0,
            'total_pnl': sum(t.pnl for t in closed_trades),
            'total_pnl_percent': ((self.balance - self.initial_balance) / self.initial_balance * 100),
            'avg_win': (total_wins / len(winning_trades)) if winning_trades else 0,
            'avg_loss': (total_losses / len(losing_trades)) if losing_trades else 0,
            'profit_factor': (total_wins / total_losses) if total_losses > 0 else float('inf'),
            'max_drawdown': max_dd,
            'current_balance': self.balance,
            'roi': ((self.balance - self.initial_balance) / self.initial_balance * 100)
        }

    def print_statistics(self):
        """Print formatted statistics"""
        stats = self.get_statistics()

        print("\n" + "=" * 80)
        print("PAPER TRADING STATISTICS")
        print("=" * 80)
        print(f"\n💼 Account Balance:")
        print(f"   Initial: ${self.initial_balance:,.2f}")
        print(f"   Current: ${self.balance:,.2f}")
        if stats['total_trades'] > 0:
            print(f"   P&L: ${stats['total_pnl']:,.2f} ({stats.get('total_pnl_percent', 0):+.2f}%)")
        else:
            print(f"   P&L: $0.00 (0.00%)")
        print(f"   ROI: {stats.get('roi', 0):+.2f}%")

        print(f"\n📊 Trade Statistics:")
        print(f"   Total Trades: {stats['total_trades']}")
        print(f"   Winning: {stats['winning_trades']} ({stats['win_rate']:.1f}%)")
        print(f"   Losing: {stats['losing_trades']}")
        print(f"   Open Positions: {len(self.open_positions)}")

        print(f"\n💰 Performance Metrics:")
        print(f"   Average Win: ${stats['avg_win']:,.2f}")
        print(f"   Average Loss: ${stats['avg_loss']:,.2f}")
        print(f"   Profit Factor: {stats['profit_factor']:.2f}")
        print(f"   Max Drawdown: {stats['max_drawdown']:.2f}%")

        print("=" * 80)

    def save_to_file(self, filename: str = "paper_trading_account.json"):
        """Save account state to file"""
        data = {
            'initial_balance': self.initial_balance,
            'balance': self.balance,
            'max_risk_per_trade': self.max_risk_per_trade,
            'trade_counter': self.trade_counter,
            'trades': [asdict(t) for t in self.trades],
            'equity_curve': self.equity_curve,
            'statistics': self.get_statistics()
        }

        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)

        print(f"💾 Account saved to {filename}")

    def load_from_file(self, filename: str = "paper_trading_account.json"):
        """Load account state from file"""
        try:
            with open(filename, 'r') as f:
                data = json.load(f)

            self.initial_balance = data['initial_balance']
            self.balance = data['balance']
            self.max_risk_per_trade = data['max_risk_per_trade']
            self.trade_counter = data['trade_counter']
            self.equity_curve = data['equity_curve']

            # Restore trades
            self.trades = []
            self.open_positions = {}
            for trade_dict in data['trades']:
                trade = Trade(**trade_dict)
                self.trades.append(trade)
                if trade.status == 'OPEN':
                    self.open_positions[trade.symbol] = trade

            print(f"📂 Account loaded from {filename}")
            print(f"   Balance: ${self.balance:,.2f}")
            print(f"   Total Trades: {len(self.trades)}")
            print(f"   Open Positions: {len(self.open_positions)}")

        except FileNotFoundError:
            print(f"⚠️  File {filename} not found. Starting fresh.")


# Example usage
if __name__ == "__main__":
    # Create paper trading account
    account = PaperTradingAccount(initial_balance=10000, max_risk_per_trade=1.0)

    print("Testing Paper Trading Account...")

    # Open a trade
    account.open_trade(
        symbol='BTCUSDT',
        side='BUY',
        entry_price=50000,
        stop_loss=49000,
        take_profit=52000,
        signal_confidence=85.0
    )

    # Simulate price movement
    print("\n📈 Simulating price movement...")
    account.update_positions(51000, 'BTCUSDT')  # Price moves up

    # Close manually
    account.close_trade('BTCUSDT', 51500, "MANUAL")

    # Print statistics
    account.print_statistics()

    # Save account
    account.save_to_file("test_account.json")
