"""
Quick Price Checker
==================
Simple script to check current crypto prices
"""

import sys
from data_fetcher import BinanceDataFetcher


def check_prices():
    """Check current prices for popular cryptocurrencies"""
    fetcher = BinanceDataFetcher()

    pairs = [
        'BTCUSDT',
        'ETHUSDT',
        'BNBUSDT',
        'SOLUSDT',
        'XRPUSDT',
        'ADAUSDT',
        'DOGEUSDT'
    ]

    print("=" * 60)
    print("CURRENT CRYPTOCURRENCY PRICES (Binance)")
    print("=" * 60)
    print()

    for pair in pairs:
        try:
            price = fetcher.get_current_price(pair)
            # Extract coin name (remove USDT)
            coin = pair.replace('USDT', '')
            print(f"  {coin:8s}  ${price:>12,.2f}")
        except Exception as e:
            print(f"  {pair:8s}  Error: {e}")

    print()
    print("=" * 60)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        # Check specific pair
        fetcher = BinanceDataFetcher()
        symbol = sys.argv[1].upper()
        if not symbol.endswith('USDT'):
            symbol += 'USDT'

        try:
            price = fetcher.get_current_price(symbol)
            print(f"{symbol}: ${price:,.2f}")
        except Exception as e:
            print(f"Error: {e}")
    else:
        # Check all popular pairs
        check_prices()
