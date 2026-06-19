"""
ICT Trading Bot - Paper Trading with RL
========================================
Automated trading bot with RL training capability
"""

import sys
import io
import time
from datetime import datetime

# Fix Windows console encoding and force unbuffered output
if sys.platform == 'win32':
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', line_buffering=True)
    sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8', line_buffering=True)

# Force unbuffered output
import functools
print = functools.partial(print, flush=True)

from trading_system import ICTCompleteTradingSystem
from data_fetcher import BinanceDataFetcher
from ict_indicators import ICTIndicators
from paper_trading import PaperTradingAccount


class ICTTradingBot:
    """Automated ICT trading bot"""

    def __init__(self, symbol: str = 'BTCUSDT',
                 timeframes: list = None,
                 initial_balance: float = 10000,
                 risk_per_trade: float = 1.0,
                 min_confidence: float = 75.0,
                 use_rl: bool = False):

        self.symbol = symbol
        self.timeframes = timeframes or ['1m','5m','15m','30m','1h', '4h']
        self.min_confidence = min_confidence
        self.use_rl = use_rl

        # Initialize components
        self.fetcher = BinanceDataFetcher()
        self.account = PaperTradingAccount(initial_balance, risk_per_trade)
        self.trading_system = None

        print("=" * 80)
        print("ICT TRADING BOT INITIALIZED")
        print("=" * 80)
        print(f"Symbol: {symbol}")
        print(f"Timeframes: {', '.join(self.timeframes)}")
        print(f"Initial Balance: ${initial_balance:,.2f}")
        print(f"Risk per Trade: {risk_per_trade}%")
        print(f"Min Confidence: {min_confidence}%")
        print(f"RL Enabled: {use_rl}")
        print("=" * 80)

    def initialize_system(self):
        """Fetch data and initialize trading system"""
        print("\n📊 Fetching market data...")

        # Fetch primary data
        data = self.fetcher.get_multi_timeframe_data(
            self.symbol, self.timeframes, limit=500
        )

        if not data:
            raise Exception("Failed to fetch data")

        # Prepare with ICT indicators
        print("🔧 Preparing ICT indicators...")
        for tf in self.timeframes:
            data[tf] = ICTIndicators.prepare_dataframe(data[tf])

        # Initialize trading system
        print("🚀 Initializing trading system...")
        self.trading_system = ICTCompleteTradingSystem(
            primary_asset_data=data,
            secondary_asset_data=None,
            primary_name=self.symbol,
            use_llm=True,
            use_smt=False,  # No secondary asset
            use_rl=self.use_rl
        )

        # Analyze market
        print("\n📈 Initial market analysis...")
        self.trading_system.analyze_market()

        print("\n✅ System initialized successfully!")

    def train_rl_model(self, timesteps: int = 50000):
        """Train RL model on historical data"""
        if not self.use_rl:
            print("⚠️  RL is disabled. Set use_rl=True to train.")
            return

        print("\n" + "=" * 80)
        print("TRAINING RL MODEL")
        print("=" * 80)
        print(f"Training steps: {timesteps:,}")
        print("This may take several minutes...")
        print("=" * 80 + "\n")

        self.trading_system.train_rl_bot(
            timesteps=timesteps,
            initial_teacher_weight=1.0,
            final_teacher_weight=0.1
        )

        print("\n✅ RL training complete!")

    def get_trading_decision(self, current_price: float):
        """Get trading decision from system"""

        # Generate signal
        signal = self.trading_system.get_trading_signal(
            current_price, timeframe=self.timeframes[0]
        )

        return signal

    def execute_signal(self, signal: dict, current_price: float):
        """Execute trading signal"""

        # Check if signal meets minimum confidence
        if signal['confidence'] < self.min_confidence:
            print(f"⚠️  Signal confidence {signal['confidence']:.1f}% below minimum {self.min_confidence}%")
            return

        # Check signal strength
        if signal['alignment'] == 'WEAK':
            print(f"⚠️  Signal alignment is WEAK, skipping trade")
            return

        action = signal['signal']

        # Check if already have position
        if self.symbol in self.account.open_positions:
            position = self.account.open_positions[self.symbol]

            # Check if should close position
            if (position.side == 'BUY' and action == 'SELL') or \
               (position.side == 'SELL' and action == 'BUY'):
                print(f"\n🔄 Reversing position based on new signal")
                self.account.close_trade(self.symbol, current_price, "SIGNAL_REVERSAL")
            else:
                print(f"✋ Already have {position.side} position, holding...")
                return

        # Open new position
        if action in ['BUY', 'SELL']:
            print(f"\n🎯 Opening {action} position...")

            # Calculate stop loss and take profit
            atr = current_price * 0.02  # Simple 2% ATR estimate

            if action == 'BUY':
                stop_loss = current_price - (2 * atr)
                take_profit = current_price + (3 * atr)  # 1:1.5 R:R
            else:  # SELL
                stop_loss = current_price + (2 * atr)
                take_profit = current_price - (3 * atr)

            self.account.open_trade(
                symbol=self.symbol,
                side=action,
                entry_price=current_price,
                stop_loss=stop_loss,
                take_profit=take_profit,
                signal_confidence=signal['confidence'],
                signal_components=signal['component_signals']
            )

    def run(self, duration_minutes: int = 60, update_interval: int = 60):
        """Run trading bot for specified duration"""

        print("\n" + "=" * 80)
        print("STARTING PAPER TRADING BOT")
        print("=" * 80)
        print(f"Duration: {duration_minutes} minutes")
        print(f"Update interval: {update_interval} seconds")
        print("Press Ctrl+C to stop early")
        print("=" * 80 + "\n")

        iterations = (duration_minutes * 60) // update_interval

        try:
            for i in range(iterations):
                print(f"\n{'=' * 80}")
                print(f"Update #{i+1}/{iterations} - {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
                print(f"{'=' * 80}")

                # Get current price
                current_price = self.fetcher.get_current_price(self.symbol)
                print(f"📍 Current {self.symbol} Price: ${current_price:,.2f}")

                # Update open positions (check stop loss / take profit)
                self.account.update_positions(current_price, self.symbol)

                # Show unrealized P&L if have position
                if self.symbol in self.account.open_positions:
                    unrealized = self.account.get_unrealized_pnl(current_price, self.symbol)
                    position = self.account.open_positions[self.symbol]
                    print(f"💼 Open {position.side} Position:")
                    print(f"   Entry: ${position.entry_price:,.2f}")
                    print(f"   Unrealized P&L: ${unrealized:,.2f}")

                # Get trading decision
                print(f"\n🤖 Analyzing market...")
                signal = self.get_trading_decision(current_price)

                print(f"\n📊 Signal Analysis:")
                print(f"   Decision: {signal['signal']}")
                print(f"   Confidence: {signal['confidence']:.1f}%")
                print(f"   Alignment: {signal['alignment']}")

                # Execute signal
                self.execute_signal(signal, current_price)

                # Show current stats
                stats = self.account.get_statistics()
                print(f"\n💰 Account Status:")
                print(f"   Balance: ${self.account.balance:,.2f}")
                print(f"   Total Trades: {stats['total_trades']}")
                print(f"   Win Rate: {stats['win_rate']:.1f}%")
                if stats['total_trades'] > 0:
                    print(f"   Total P&L: ${stats['total_pnl']:,.2f} ({stats.get('total_pnl_percent', 0):+.2f}%)")

                # Wait for next update
                if i < iterations - 1:
                    print(f"\n⏳ Next update in {update_interval} seconds...")
                    time.sleep(update_interval)

        except KeyboardInterrupt:
            print("\n\n✋ Bot stopped by user")

        # Final statistics
        print("\n" + "=" * 80)
        print("BOT SESSION COMPLETE")
        print("=" * 80)
        self.account.print_statistics()

        # Save account
        filename = f"paper_trading_{self.symbol}_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        self.account.save_to_file(filename)

    def backtest(self, days: int = 30):
        """Backtest strategy on historical data"""
        print("\n" + "=" * 80)
        print("BACKTESTING MODE")
        print("=" * 80)
        print(f"Testing last {days} days of data...")
        print("=" * 80 + "\n")

        # For backtesting, we'd need to simulate day by day
        # This is a simplified version
        print("⚠️  Full backtesting implementation coming soon!")
        print("Use run() method for forward testing with real-time data")


def main():
    """Main entry point"""
    import argparse

    parser = argparse.ArgumentParser(description='ICT Trading Bot')
    parser.add_argument('--symbol', default='BTCUSDT', help='Trading pair')
    parser.add_argument('--balance', type=float, default=10000, help='Initial balance')
    parser.add_argument('--risk', type=float, default=1.0, help='Risk per trade (%)')
    parser.add_argument('--confidence', type=float, default=75.0, help='Min confidence (%)')
    parser.add_argument('--duration', type=int, default=60, help='Duration (minutes)')
    parser.add_argument('--interval', type=int, default=60, help='Update interval (seconds)')
    parser.add_argument('--train-rl', action='store_true', help='Train RL model first')
    parser.add_argument('--rl-steps', type=int, default=50000, help='RL training steps')
    parser.add_argument('--use-rl', action='store_true', help='Use RL for trading')

    args = parser.parse_args()

    # Create bot
    bot = ICTTradingBot(
        symbol=args.symbol,
        initial_balance=args.balance,
        risk_per_trade=args.risk,
        min_confidence=args.confidence,
        use_rl=args.use_rl
    )

    # Initialize
    bot.initialize_system()

    # Train RL if requested
    if args.train_rl:
        bot.train_rl_model(timesteps=args.rl_steps)

    # Run bot
    bot.run(duration_minutes=args.duration, update_interval=args.interval)


if __name__ == "__main__":
    main()
