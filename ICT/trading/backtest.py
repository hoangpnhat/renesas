"""
ICT Trading System - Backtesting Engine
========================================
Backtest trading strategies on historical data
"""

import sys
import io
import pandas as pd
import numpy as np
from datetime import datetime, timedelta
from typing import Dict, List

# Fix Windows console encoding
if sys.platform == 'win32':
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', line_buffering=True)
    sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8', line_buffering=True)

import functools
print = functools.partial(print, flush=True)

from trading_system import ICTCompleteTradingSystem
from data_fetcher import BinanceDataFetcher
from ict_indicators import ICTIndicators
from paper_trading import PaperTradingAccount


class Backtester:
    """Backtest ICT trading system on historical data"""

    def __init__(self, symbol: str = 'BTCUSDT',
                 timeframes: list = None,
                 initial_balance: float = 10000,
                 risk_per_trade: float = 1.0,
                 min_confidence: float = 75.0,
                 use_llm: bool = True):

        self.symbol = symbol
        self.timeframes = timeframes or ['1h', '4h', '1d']
        self.initial_balance = initial_balance
        self.risk_per_trade = risk_per_trade
        self.min_confidence = min_confidence
        self.use_llm = use_llm

        self.fetcher = BinanceDataFetcher()
        self.results = []
        self.trades_log = []

        print("=" * 80)
        print("ICT BACKTESTING ENGINE")
        print("=" * 80)
        print(f"Symbol: {symbol}")
        print(f"Timeframes: {', '.join(self.timeframes)}")
        print(f"Initial Balance: ${initial_balance:,.2f}")
        print(f"Risk per Trade: {risk_per_trade}%")
        print(f"Min Confidence: {min_confidence}%")
        print("=" * 80)

    def fetch_historical_data(self, days: int = 60):
        """Fetch historical data for backtesting"""
        print(f"\n📊 Fetching {days} days of historical data...")

        # Calculate number of candles needed for each timeframe
        candles_needed = {
            '1m': days * 1440,   # 1440 minutes per day
            '5m': days * 288,    # 288 5-min candles per day
            '15m': days * 96,    # 96 15-min candles per day
            '30m': days * 48,    # 48 30-min candles per day
            '1h': days * 24,     # 24 hours per day
            '4h': days * 6,      # 6 4-hour candles per day
            '1d': days * 1,      # 1 daily candle
        }

        data = {}
        for tf in self.timeframes:
            limit = min(candles_needed.get(tf, 1000), 1000)  # Binance max 1000
            print(f"   Fetching {tf}: {limit} candles...")

            df = self.fetcher.get_klines(self.symbol, tf, limit=limit)

            if df.empty:
                print(f"   ❌ Failed to fetch {tf} data")
                return None

            # Prepare with ICT indicators
            df = ICTIndicators.prepare_dataframe(df)
            data[tf] = df

            print(f"   ✅ {tf}: {len(df)} candles from {df['timestamp'].iloc[0]} to {df['timestamp'].iloc[-1]}")

        return data

    def run_backtest(self, days: int = 60):
        """Run backtest on historical data"""
        print("\n" + "=" * 80)
        print("STARTING BACKTEST")
        print("=" * 80)

        # Fetch data
        data = self.fetch_historical_data(days)
        if not data:
            print("❌ Failed to fetch data. Aborting backtest.")
            return

        # Use the smallest timeframe for iteration
        smallest_tf = self.timeframes[0]
        df_primary = data[smallest_tf]

        print(f"\n🔧 Initializing trading system...")

        # Initialize trading system
        system = ICTCompleteTradingSystem(
            primary_asset_data=data,
            secondary_asset_data=None,
            primary_name=self.symbol,
            use_llm=self.use_llm,
            use_smt=False,
            use_rl=False
        )

        # Analyze market once
        print(f"📈 Analyzing market structure...")
        system.analyze_market()

        # Initialize paper trading account
        account = PaperTradingAccount(self.initial_balance, self.risk_per_trade)

        # Calculate warm-up period (skip first 20% for indicator stabilization)
        warmup_candles = int(len(df_primary) * 0.2)
        backtest_candles = len(df_primary) - warmup_candles

        print(f"\n⏳ Backtesting on {backtest_candles} candles...")
        print(f"   Period: {df_primary['timestamp'].iloc[warmup_candles]} to {df_primary['timestamp'].iloc[-1]}")
        print(f"   Skipping {warmup_candles} warm-up candles")
        print("=" * 80)

        # Backtest loop
        for i in range(warmup_candles, len(df_primary)):
            current_candle = df_primary.iloc[i]
            current_price = current_candle['close']
            current_time = current_candle['timestamp']

            # Update open positions (check stop loss / take profit)
            account.update_positions(current_price, self.symbol)

            # Generate signal every N candles (to reduce computation)
            if i % 5 == 0 or len(account.open_positions) == 0:  # Check every 5 candles or when no position
                try:
                    signal = system.get_trading_signal(current_price, timeframe=smallest_tf)

                    # Check if signal meets criteria
                    if signal['confidence'] >= self.min_confidence and signal['alignment'] != 'WEAK':
                        action = signal['signal']

                        # Check if already have position
                        if self.symbol in account.open_positions:
                            position = account.open_positions[self.symbol]

                            # Reverse position if signal changes
                            if (position.side == 'BUY' and action == 'SELL') or \
                               (position.side == 'SELL' and action == 'BUY'):
                                account.close_trade(self.symbol, current_price, "SIGNAL_REVERSAL")

                        # Open new position
                        if action in ['BUY', 'SELL'] and self.symbol not in account.open_positions:
                            # Calculate stop loss and take profit
                            atr = current_price * 0.02  # 2% ATR estimate

                            if action == 'BUY':
                                stop_loss = current_price - (2 * atr)
                                take_profit = current_price + (3 * atr)
                            else:
                                stop_loss = current_price + (2 * atr)
                                take_profit = current_price - (3 * atr)

                            trade = account.open_trade(
                                symbol=self.symbol,
                                side=action,
                                entry_price=current_price,
                                stop_loss=stop_loss,
                                take_profit=take_profit,
                                signal_confidence=signal['confidence'],
                                signal_components=signal['component_signals']
                            )

                            if trade:
                                self.trades_log.append({
                                    'timestamp': current_time,
                                    'trade_id': trade.id,
                                    'action': 'OPEN',
                                    'side': action,
                                    'price': current_price,
                                    'confidence': signal['confidence']
                                })

                except Exception as e:
                    print(f"⚠️  Error at candle {i}: {e}")
                    continue

            # Log progress every 10%
            if i % (backtest_candles // 10) == 0:
                progress = ((i - warmup_candles) / backtest_candles) * 100
                print(f"   Progress: {progress:.0f}% | Balance: ${account.balance:,.2f} | Trades: {len([t for t in account.trades if t.status != 'OPEN'])}")

        # Close any remaining open positions at the end
        if self.symbol in account.open_positions:
            final_price = df_primary.iloc[-1]['close']
            account.close_trade(self.symbol, final_price, "BACKTEST_END")

        # Generate results
        print("\n" + "=" * 80)
        print("BACKTEST COMPLETE")
        print("=" * 80)

        account.print_statistics()

        # Save results
        self._save_results(account, df_primary, warmup_candles)

        return account

    def _save_results(self, account: PaperTradingAccount, df: pd.DataFrame, warmup: int):
        """Save backtest results to file"""
        timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        filename = f"backtest_{self.symbol}_{timestamp}.json"

        account.save_to_file(filename)

        # Also create a detailed report
        report_file = f"backtest_report_{self.symbol}_{timestamp}.txt"

        stats = account.get_statistics()

        with open(report_file, 'w', encoding='utf-8') as f:
            f.write("=" * 80 + "\n")
            f.write("ICT BACKTEST REPORT\n")
            f.write("=" * 80 + "\n\n")

            f.write(f"Symbol: {self.symbol}\n")
            f.write(f"Timeframes: {', '.join(self.timeframes)}\n")
            f.write(f"Period: {df['timestamp'].iloc[warmup]} to {df['timestamp'].iloc[-1]}\n")
            f.write(f"Candles: {len(df) - warmup}\n")
            f.write(f"Min Confidence: {self.min_confidence}%\n\n")

            f.write("PERFORMANCE SUMMARY\n")
            f.write("=" * 80 + "\n")
            f.write(f"Initial Balance: ${self.initial_balance:,.2f}\n")
            f.write(f"Final Balance: ${account.balance:,.2f}\n")
            f.write(f"Total P&L: ${stats['total_pnl']:,.2f} ({stats.get('total_pnl_percent', 0):+.2f}%)\n")
            f.write(f"ROI: {stats.get('roi', 0):+.2f}%\n\n")

            f.write("TRADE STATISTICS\n")
            f.write("=" * 80 + "\n")
            f.write(f"Total Trades: {stats['total_trades']}\n")
            f.write(f"Winning Trades: {stats['winning_trades']} ({stats['win_rate']:.1f}%)\n")
            f.write(f"Losing Trades: {stats['losing_trades']}\n")
            f.write(f"Average Win: ${stats['avg_win']:,.2f}\n")
            f.write(f"Average Loss: ${stats['avg_loss']:,.2f}\n")
            f.write(f"Profit Factor: {stats['profit_factor']:.2f}\n")
            f.write(f"Max Drawdown: {stats['max_drawdown']:.2f}%\n\n")

            f.write("TRADE LOG\n")
            f.write("=" * 80 + "\n")
            for trade in account.trades:
                if trade.status != 'OPEN':
                    f.write(f"\nTrade #{trade.id}:\n")
                    f.write(f"  Entry: {trade.timestamp} @ ${trade.entry_price:,.2f} ({trade.side})\n")
                    f.write(f"  Exit: {trade.exit_timestamp} @ ${trade.exit_price:,.2f} ({trade.status})\n")
                    f.write(f"  P&L: ${trade.pnl:,.2f} ({trade.pnl_percent:+.2f}%)\n")
                    f.write(f"  Confidence: {trade.signal_confidence:.1f}%\n")

        print(f"\n📄 Report saved to: {report_file}")


def main():
    """Main entry point"""
    import argparse

    parser = argparse.ArgumentParser(description='ICT Backtesting Engine')
    parser.add_argument('--symbol', default='BTCUSDT', help='Trading pair')
    parser.add_argument('--days', type=int, default=60, help='Days of history to backtest')
    parser.add_argument('--balance', type=float, default=10000, help='Initial balance')
    parser.add_argument('--risk', type=float, default=1.0, help='Risk per trade (%)')
    parser.add_argument('--confidence', type=float, default=75.0, help='Min confidence (%)')
    parser.add_argument('--timeframes', nargs='+', default=['1h', '4h', '1d'], help='Timeframes to use')
    parser.add_argument('--no-llm', action='store_true', help='Disable LLM analysis')

    args = parser.parse_args()

    # Create backtester
    backtester = Backtester(
        symbol=args.symbol,
        timeframes=args.timeframes,
        initial_balance=args.balance,
        risk_per_trade=args.risk,
        min_confidence=args.confidence,
        use_llm=not args.no_llm
    )

    # Run backtest
    backtester.run_backtest(days=args.days)


if __name__ == "__main__":
    main()
