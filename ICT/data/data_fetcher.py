"""
ICT Trading System - Real Data Fetcher
======================================
Fetch real-time and historical data from cryptocurrency exchanges
"""

import pandas as pd
import requests
from datetime import datetime, timedelta
from typing import Dict, List, Optional
import time
import urllib3

# Disable SSL warnings for corporate proxies
urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)


class BinanceDataFetcher:
    """Fetch real-time data from Binance"""

    def __init__(self):
        self.base_url = "https://api.binance.com/api/v3"

    def get_klines(self, symbol: str, interval: str, limit: int = 1000,
                   start_time: Optional[int] = None) -> pd.DataFrame:
        """
        Fetch historical kline/candlestick data

        Args:
            symbol: Trading pair (e.g., 'BTCUSDT', 'ETHUSDT')
            interval: Timeframe ('1m', '5m', '15m', '1h', '4h', '1d', '1w')
            limit: Number of candles (max 1000)
            start_time: Start time in milliseconds

        Returns:
            DataFrame with OHLCV data
        """
        endpoint = f"{self.base_url}/klines"
        params = {
            'symbol': symbol,
            'interval': interval,
            'limit': limit
        }

        if start_time:
            params['startTime'] = start_time

        try:
            response = requests.get(endpoint, params=params, timeout=10, verify=False)
            response.raise_for_status()
            data = response.json()

            # Parse response
            df = pd.DataFrame(data, columns=[
                'timestamp', 'open', 'high', 'low', 'close', 'volume',
                'close_time', 'quote_volume', 'trades', 'taker_buy_base',
                'taker_buy_quote', 'ignore'
            ])

            # Convert types
            df['timestamp'] = pd.to_datetime(df['timestamp'], unit='ms')
            df['open'] = df['open'].astype(float)
            df['high'] = df['high'].astype(float)
            df['low'] = df['low'].astype(float)
            df['close'] = df['close'].astype(float)
            df['volume'] = df['volume'].astype(float)

            # Keep only necessary columns
            df = df[['timestamp', 'open', 'high', 'low', 'close', 'volume']]

            return df

        except requests.exceptions.RequestException as e:
            print(f"Error fetching data: {e}")
            return pd.DataFrame()

    def get_multi_timeframe_data(self, symbol: str,
                                  timeframes: List[str],
                                  limit: int = 500) -> Dict[str, pd.DataFrame]:
        """
        Fetch data for multiple timeframes

        Args:
            symbol: Trading pair (e.g., 'BTCUSDT')
            timeframes: List of timeframes (e.g., ['1h', '4h', '1d'])
            limit: Number of candles per timeframe

        Returns:
            Dict of timeframe -> DataFrame
        """
        data = {}

        for tf in timeframes:
            print(f"Fetching {symbol} {tf} data...")
            df = self.get_klines(symbol, tf, limit)

            if not df.empty:
                data[tf] = df
                print(f"  ✅ Fetched {len(df)} candles")
            else:
                print(f"  ❌ Failed to fetch {tf} data")

            # Respect rate limits
            time.sleep(0.2)

        return data

    def get_current_price(self, symbol: str) -> float:
        """
        Get current price for a symbol

        Args:
            symbol: Trading pair (e.g., 'BTCUSDT')

        Returns:
            Current price
        """
        endpoint = f"{self.base_url}/ticker/price"
        params = {'symbol': symbol}

        try:
            response = requests.get(endpoint, params=params, timeout=10, verify=False)
            response.raise_for_status()
            data = response.json()
            return float(data['price'])
        except requests.exceptions.RequestException as e:
            print(f"Error fetching price: {e}")
            return 0.0


class CoinbaseDataFetcher:
    """Fetch real-time data from Coinbase"""

    def __init__(self):
        self.base_url = "https://api.exchange.coinbase.com"

    def get_candles(self, product_id: str, granularity: int,
                    limit: int = 300) -> pd.DataFrame:
        """
        Fetch historical candle data

        Args:
            product_id: Trading pair (e.g., 'BTC-USD', 'ETH-USD')
            granularity: Candle size in seconds (60, 300, 900, 3600, 21600, 86400)
            limit: Number of candles

        Returns:
            DataFrame with OHLCV data
        """
        endpoint = f"{self.base_url}/products/{product_id}/candles"

        end = datetime.now()
        start = end - timedelta(seconds=granularity * limit)

        params = {
            'start': start.isoformat(),
            'end': end.isoformat(),
            'granularity': granularity
        }

        try:
            response = requests.get(endpoint, params=params, timeout=10, verify=False)
            response.raise_for_status()
            data = response.json()

            # Parse response [time, low, high, open, close, volume]
            df = pd.DataFrame(data, columns=[
                'timestamp', 'low', 'high', 'open', 'close', 'volume'
            ])

            df['timestamp'] = pd.to_datetime(df['timestamp'], unit='s')
            df = df.sort_values('timestamp').reset_index(drop=True)

            return df[['timestamp', 'open', 'high', 'low', 'close', 'volume']]

        except requests.exceptions.RequestException as e:
            print(f"Error fetching data: {e}")
            return pd.DataFrame()


class RealTimeDataManager:
    """Manage real-time data updates"""

    def __init__(self, fetcher, symbol: str, timeframes: List[str]):
        self.fetcher = fetcher
        self.symbol = symbol
        self.timeframes = timeframes
        self.data = {}
        self.last_update = {}

    def initialize(self, limit: int = 500):
        """Initialize with historical data"""
        print(f"\n📊 Initializing real-time data for {self.symbol}...")
        self.data = self.fetcher.get_multi_timeframe_data(
            self.symbol, self.timeframes, limit
        )

        for tf in self.timeframes:
            self.last_update[tf] = datetime.now()

        print(f"✅ Initialized with {len(self.data)} timeframes")
        return self.data

    def update(self):
        """Update data with latest candles"""
        updated = False

        for tf in self.timeframes:
            # Fetch latest candle
            df_new = self.fetcher.get_klines(self.symbol, tf, limit=1)

            if not df_new.empty and tf in self.data:
                # Check if it's a new candle
                latest_time = self.data[tf]['timestamp'].iloc[-1]
                new_time = df_new['timestamp'].iloc[0]

                if new_time > latest_time:
                    # Append new candle
                    self.data[tf] = pd.concat([
                        self.data[tf], df_new
                    ]).reset_index(drop=True)
                    self.last_update[tf] = datetime.now()
                    updated = True
                    print(f"  🔄 Updated {tf} with new candle at {new_time}")
                else:
                    # Update current candle (still forming)
                    self.data[tf].iloc[-1] = df_new.iloc[0]

        return updated

    def get_current_price(self) -> float:
        """Get current market price"""
        return self.fetcher.get_current_price(self.symbol)


def create_binance_fetcher():
    """Create Binance data fetcher"""
    return BinanceDataFetcher()


def create_coinbase_fetcher():
    """Create Coinbase data fetcher"""
    return CoinbaseDataFetcher()


# Example usage
if __name__ == "__main__":
    # Test Binance fetcher
    print("Testing Binance Data Fetcher...")
    fetcher = BinanceDataFetcher()

    # Get single timeframe
    df = fetcher.get_klines('BTCUSDT', '1h', limit=100)
    print(f"\nFetched {len(df)} candles")
    print(df.tail())

    # Get current price
    price = fetcher.get_current_price('BTCUSDT')
    print(f"\nCurrent BTC price: ${price:,.2f}")

    # Get multi-timeframe data
    data = fetcher.get_multi_timeframe_data(
        'BTCUSDT',
        timeframes=['1h', '4h', '1d'],
        limit=100
    )

    print(f"\nFetched data for {len(data)} timeframes")
    for tf, df in data.items():
        print(f"  {tf}: {len(df)} candles, latest: {df['close'].iloc[-1]:.2f}")
