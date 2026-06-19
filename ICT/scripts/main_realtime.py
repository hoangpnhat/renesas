"""
ICT Trading System - Real-Time Demo
===================================
Demo with real-time data from cryptocurrency exchanges
"""

import sys
import io
import time
from datetime import datetime

# Fix Windows console encoding for emojis
if sys.platform == 'win32':
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', line_buffering=True)
    sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8', line_buffering=True)

from trading_system import ICTCompleteTradingSystem
from data_fetcher import BinanceDataFetcher, RealTimeDataManager
from ict_indicators import ICTIndicators


def realtime_demo():
    """Real-time trading demo with live data"""
    print("=" * 80)
    print("ICT TRADING SYSTEM - REAL-TIME MODE")
    print("=" * 80)

    # Configuration
    PRIMARY_SYMBOL = 'BTCUSDT'
    SECONDARY_SYMBOL = 'ETHUSDT'
    TIMEFRAMES = ['1h', '4h', '1d']
    UPDATE_INTERVAL = 60  # seconds

    # Initialize data fetcher
    print("\n🔌 Connecting to Binance...")
    fetcher = BinanceDataFetcher()

    # Test connection
    try:
        price = fetcher.get_current_price(PRIMARY_SYMBOL)
        print(f"✅ Connected! Current BTC price: ${price:,.2f}")
    except Exception as e:
        print(f"❌ Connection failed: {e}")
        return

    # Fetch initial data
    print("\n📊 Fetching historical data...", flush=True)
    btc_data = fetcher.get_multi_timeframe_data(PRIMARY_SYMBOL, TIMEFRAMES, limit=500)
    eth_data = fetcher.get_multi_timeframe_data(SECONDARY_SYMBOL, TIMEFRAMES, limit=500)

    if not btc_data or not eth_data:
        print("❌ Failed to fetch data")
        return

    # Prepare data with ICT indicators
    print("\n🔧 Preparing ICT indicators...")
    for tf in TIMEFRAMES:
        btc_data[tf] = ICTIndicators.prepare_dataframe(btc_data[tf])
        eth_data[tf] = ICTIndicators.prepare_dataframe(eth_data[tf])

    # Initialize trading system
    print("\n🚀 Initializing ICT Trading System...")
    system = ICTCompleteTradingSystem(
        primary_asset_data=btc_data,
        secondary_asset_data=eth_data,
        primary_name="BTC",
        secondary_name="ETH",
        use_llm=True,
        use_smt=True,
        use_rl=False
    )

    # Initial analysis
    print("\n" + "=" * 80)
    print("INITIAL MARKET ANALYSIS")
    print("=" * 80)
    system.analyze_market()

    # Print report
    print("\n" + system.generate_report())

    # Real-time monitoring loop
    print("\n" + "=" * 80)
    print("STARTING REAL-TIME MONITORING")
    print("=" * 80)
    print(f"Update interval: {UPDATE_INTERVAL} seconds")
    print("Press Ctrl+C to stop\n")

    iteration = 0
    try:
        while True:
            iteration += 1
            current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

            print(f"\n{'=' * 80}")
            print(f"Update #{iteration} - {current_time}")
            print(f"{'=' * 80}")

            # Fetch latest data
            latest_btc = fetcher.get_klines(PRIMARY_SYMBOL, '1m', limit=1)
            if not latest_btc.empty:
                current_price = latest_btc['close'].iloc[0]

                # Update system data (simulate by appending latest candle)
                for tf in TIMEFRAMES:
                    latest_tf = fetcher.get_klines(PRIMARY_SYMBOL, tf, limit=1)
                    if not latest_tf.empty:
                        latest_tf = ICTIndicators.prepare_dataframe(latest_tf)
                        # Update the last candle in the system
                        system.primary_data[tf].iloc[-1] = latest_tf.iloc[0]

                # Re-detect PD arrays with updated data
                system.pd_detector.detect_all_pd_arrays()

                # Generate trading signal
                signal = system.get_trading_signal(current_price, timeframe='1h')

                # Display summary
                print(f"\n📍 Current Price: ${current_price:,.2f}")
                print(f"🎯 Signal: {signal['signal']}")
                print(f"💪 Confidence: {signal['confidence']:.1f}%")
                print(f"🔗 Alignment: {signal['alignment']}")
                print(f"💭 Reasoning: {signal['reasoning']}")

                # Component signals breakdown
                print(f"\n📊 Component Signals:")
                for component, data in signal['component_signals'].items():
                    sig = data.get('signal', 'N/A')
                    conf = data.get('confidence', 0)
                    print(f"   • {component.upper()}: {sig} ({conf:.1f}%)")

            else:
                print("⚠️  Failed to fetch latest price")

            # Wait for next update
            print(f"\n⏳ Next update in {UPDATE_INTERVAL} seconds...")
            time.sleep(UPDATE_INTERVAL)

    except KeyboardInterrupt:
        print("\n\n✋ Stopping real-time monitoring...")
        print(f"Total updates: {iteration}")
        print("=" * 80)


def quick_realtime_test():
    """Quick test with real data (single fetch)"""
    print("=" * 80)
    print("ICT TRADING SYSTEM - QUICK REAL DATA TEST")
    print("=" * 80)

    # Fetch data
    print("\n📊 Fetching BTC and ETH data from Binance...")
    fetcher = BinanceDataFetcher()

    btc_data = fetcher.get_multi_timeframe_data('BTCUSDT', ['1m','5m','15m','1h', '4h', '1d'], limit=200)
    eth_data = fetcher.get_multi_timeframe_data('ETHUSDT', ['1m','5m','15m','1h', '4h', '1d'], limit=200)

    if not btc_data or not eth_data:
        print("❌ Failed to fetch data")
        return

    # Prepare indicators
    print("\n🔧 Preparing ICT indicators...")
    for tf in btc_data.keys():
        btc_data[tf] = ICTIndicators.prepare_dataframe(btc_data[tf])
        eth_data[tf] = ICTIndicators.prepare_dataframe(eth_data[tf])

    # Initialize system
    print("\n🚀 Initializing trading system...")
    system = ICTCompleteTradingSystem(
        primary_asset_data=btc_data,
        secondary_asset_data=eth_data,
        primary_name="BTC",
        secondary_name="ETH",
        use_llm=True,
        use_smt=True,
        use_rl=False
    )

    # Analyze
    system.analyze_market()

    # Get current price and signal
    current_price = fetcher.get_current_price('BTCUSDT')
    print(f"\n📍 Current BTC Price: ${current_price:,.2f}")

    signal = system.get_trading_signal(current_price, timeframe='1h')

    # Report
    print("\n" + system.generate_report())

    print("\n" + "=" * 80)
    print("✅ Quick test complete!")
    print("=" * 80)


def compare_exchanges():
    """Compare data from multiple exchanges"""
    print("=" * 80)
    print("COMPARING DATA SOURCES")
    print("=" * 80)

    binance = BinanceDataFetcher()

    print("\n📊 Binance BTC Price:")
    try:
        btc_price = binance.get_current_price('BTCUSDT')
        print(f"   ${btc_price:,.2f}")
    except Exception as e:
        print(f"   Error: {e}")

    print("\n📊 Binance ETH Price:")
    try:
        eth_price = binance.get_current_price('ETHUSDT')
        print(f"   ${eth_price:,.2f}")
    except Exception as e:
        print(f"   Error: {e}")

    print("\n" + "=" * 80)


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='ICT Trading System - Real-Time Mode')
    parser.add_argument('--mode', choices=['realtime', 'quick', 'compare'],
                       default='quick',
                       help='Mode to run (default: quick)')

    args = parser.parse_args()

    if args.mode == 'realtime':
        realtime_demo()
    elif args.mode == 'quick':
        quick_realtime_test()
    elif args.mode == 'compare':
        compare_exchanges()
