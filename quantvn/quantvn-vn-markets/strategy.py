"""
Chiến lược: EMA Trend Following + RSI Mean Reversion Entry + Volume Confirmation
=================================================================================
Ý tưởng cốt lõi:
- Xác định xu hướng chính bằng EMA dài hạn (EMA50) vs EMA ngắn hạn (EMA20)
- Dùng RSI làm bộ lọc thời điểm vào lệnh (entry timing):
  + Trong uptrend: Mua khi RSI thoát vùng quá bán (hồi về vùng hợp lý)
  + Trong downtrend: Bán khi RSI từ vùng quá mua quay xuống
- Volume trên trung bình để xác nhận tín hiệu mạnh

Logic kinh tế:
- Trend following khai thác hiện tượng momentum (giá có xu hướng tiếp tục di chuyển theo hướng hiện tại)
- RSI entry giúp tránh mua đuổi giá ở đỉnh sóng, thay vào đó mua khi có pullback
- Volume confirmation loại bỏ tín hiệu yếu (breakout giả khi không có dòng tiền hỗ trợ)
"""

import numpy as np
import pandas as pd


def compute_ema(series: pd.Series, span: int) -> pd.Series:
    """Tính Exponential Moving Average."""
    return series.ewm(span=span, adjust=False).mean()


def compute_rsi(series: pd.Series, period: int = 14) -> pd.Series:
    """
    Tính Relative Strength Index (RSI).
    RSI = 100 - 100 / (1 + RS)
    RS = Average Gain / Average Loss (dùng EMA của gain/loss)
    """
    delta = series.diff()
    gain = delta.where(delta > 0, 0.0)
    loss = (-delta).where(delta < 0, 0.0)

    avg_gain = gain.ewm(span=period, adjust=False).mean()
    avg_loss = loss.ewm(span=period, adjust=False).mean()

    rs = avg_gain / avg_loss.replace(0, np.nan)
    rsi = 100 - (100 / (1 + rs))
    return rsi


def compute_volume_ratio(volume: pd.Series, lookback: int = 20) -> pd.Series:
    """
    Tính tỷ lệ volume hiện tại so với trung bình N phiên.
    Volume ratio > 1 nghĩa là khối lượng cao hơn bình thường → tín hiệu mạnh hơn.
    """
    avg_volume = volume.rolling(window=lookback, min_periods=1).mean()
    return volume / avg_volume.replace(0, np.nan)


def gen_position(df: pd.DataFrame) -> pd.DataFrame:
    """
    Hàm chính tạo tín hiệu giao dịch.

    Input:
        df - DataFrame với các cột: Date, time, Open, High, Low, Close, volume

    Output:
        df - DataFrame gốc được bổ sung các cột chỉ báo và cột 'position':
             position = 1 (Mua/Giữ Long)
             position = 0 (Không giao dịch / Đứng ngoài)
             position = -1 (Bán/Short) — tuy nhiên cổ phiếu VN chủ yếu long-only,
                          nên -1 ở đây được hiểu là "tín hiệu thoát vị thế"

    Tham số chiến lược:
        - EMA ngắn: 20 phiên (phản ứng nhanh với xu hướng ngắn hạn)
        - EMA dài: 50 phiên (xác định xu hướng trung hạn)
        - RSI: 14 phiên (chuẩn Wilder)
        - RSI quá bán: < 35 (nới lỏng hơn chuẩn 30 để có nhiều tín hiệu hơn)
        - RSI quá mua: > 70
        - Volume ratio threshold: > 1.0 (khối lượng trên trung bình)
    """
    df = df.copy()

    # ===== BƯỚC 1: Tính toán các chỉ báo kỹ thuật =====

    # EMA 20 (ngắn hạn) và EMA 50 (dài hạn) để xác định trend
    df["ema_20"] = compute_ema(df["Close"], span=20)
    df["ema_50"] = compute_ema(df["Close"], span=50)

    # RSI 14 phiên để xác định vùng quá mua/quá bán
    df["rsi"] = compute_rsi(df["Close"], period=14)

    # Volume ratio để xác nhận sức mạnh tín hiệu
    df["volume_ratio"] = compute_volume_ratio(df["volume"], lookback=20)

    # ===== BƯỚC 2: Xác định xu hướng (Trend Filter) =====
    # Uptrend: EMA20 > EMA50 VÀ giá đóng cửa > EMA20
    # Downtrend: EMA20 < EMA50 VÀ giá đóng cửa < EMA20
    df["uptrend"] = (df["ema_20"] > df["ema_50"]) & (df["Close"] > df["ema_20"])
    df["downtrend"] = (df["ema_20"] < df["ema_50"]) & (df["Close"] < df["ema_20"])

    # ===== BƯỚC 3: Tạo tín hiệu giao dịch =====
    # Khởi tạo position = 0 (không giao dịch)
    df["position"] = 0

    # RSI thresholds
    rsi_oversold = 35
    rsi_overbought = 70
    volume_threshold = 1.0

    # Tín hiệu MUA (position = 1):
    # Điều kiện: Đang trong uptrend + RSI vừa thoát vùng quá bán + Volume xác nhận
    # Logic: Trong xu hướng tăng, khi giá pullback (RSI giảm) rồi hồi lại → điểm mua tốt
    buy_signal = (
        df["uptrend"]
        & (df["rsi"] > rsi_oversold)
        & (df["rsi"] < 60)  # Chưa vào vùng quá mua
        & (df["volume_ratio"] > volume_threshold)
    )

    # Tín hiệu BÁN / Thoát vị thế (position = -1):
    # Điều kiện: Đang trong downtrend + RSI vào vùng quá mua (divergence) → bán ra
    # Hoặc: RSI vượt 70 trong khi trend đang yếu → chốt lời
    sell_signal = (
        df["downtrend"]
        & (df["rsi"] > rsi_overbought)
    ) | (
        # Stop-loss logic: Giá phá dưới EMA50 + RSI giảm mạnh
        (df["Close"] < df["ema_50"])
        & (df["rsi"] < 30)
        & (df["volume_ratio"] > volume_threshold)
    )

    # Gán tín hiệu
    df.loc[buy_signal, "position"] = 1
    df.loc[sell_signal, "position"] = -1

    # ===== BƯỚC 4: Forward-fill position (giữ vị thế cho đến khi có tín hiệu mới) =====
    # Khi có tín hiệu mua → giữ long cho đến khi có tín hiệu bán
    # Khi có tín hiệu bán → đứng ngoài cho đến khi có tín hiệu mua mới
    df["position"] = df["position"].replace(0, np.nan).ffill().fillna(0).astype(int)

    # ===== BƯỚC 5: Áp dụng cool-down period =====
    # Tránh overtrading bằng cách không đổi vị thế quá nhanh (tối thiểu 3 bar giữa 2 lệnh)
    # Điều này cũng phù hợp với quy tắc T+2.5 của TTCK Việt Nam
    position_changes = df["position"].diff().fillna(0)
    cooldown = 3
    last_change_idx = -cooldown - 1

    for i in range(len(df)):
        if position_changes.iloc[i] != 0:
            if i - last_change_idx < cooldown:
                # Quá gần lệnh trước → giữ nguyên vị thế cũ
                df.iloc[i, df.columns.get_loc("position")] = df.iloc[
                    i - 1, df.columns.get_loc("position")
                ]
            else:
                last_change_idx = i

    return df


# ===== PHẦN CHẠY ĐỘC LẬP ĐỂ TEST =====
if __name__ == "__main__":
    import os
    from dotenv import load_dotenv

    # Load API key từ biến môi trường (bảo mật)
    load_dotenv()
    API_KEY = os.getenv("QUANTVN_API_KEY")

    if not API_KEY:
        print("⚠️  Vui lòng tạo file .env với nội dung: QUANTVN_API_KEY=<your_key>")
        print("    Hoặc set biến môi trường QUANTVN_API_KEY trước khi chạy.")
        exit(1)

    # Khởi tạo client
    from quantvn.vn.data.utils import client
    client(apikey=API_KEY)

    # Lấy dữ liệu cổ phiếu VIC (Vingroup) - timeframe 1 giờ
    from quantvn.vn.data import get_stock_hist
    print("📊 Đang lấy dữ liệu VIC (1H)...")
    df = get_stock_hist("VIC", resolution="1H")
    print(f"   Tổng số bar: {len(df)}")
    print(f"   Columns: {list(df.columns)}")
    print(f"   Date range: {df['Date'].iloc[0]} → {df['Date'].iloc[-1]}")
    print()

    # Chạy chiến lược
    print("🔧 Đang chạy chiến lược...")
    df_result = gen_position(df)

    # Thống kê kết quả
    total_bars = len(df_result)
    long_bars = (df_result["position"] == 1).sum()
    short_bars = (df_result["position"] == -1).sum()
    flat_bars = (df_result["position"] == 0).sum()
    n_trades = df_result["position"].diff().fillna(0).abs().sum() / 2

    print("📈 Kết quả chiến lược:")
    print(f"   Tổng bar: {total_bars}")
    print(f"   Long (1):  {long_bars} ({long_bars/total_bars*100:.1f}%)")
    print(f"   Short (-1): {short_bars} ({short_bars/total_bars*100:.1f}%)")
    print(f"   Flat (0):  {flat_bars} ({flat_bars/total_bars*100:.1f}%)")
    print(f"   Số lần đổi vị thế: ~{int(n_trades)}")
    print()

    # Hiển thị mẫu dữ liệu
    print("📋 Mẫu 10 bar cuối:")
    print(df_result[["Date", "time", "Close", "ema_20", "ema_50", "rsi", "volume_ratio", "position"]].tail(10).to_string(index=False))
    print()

    # Backtest đơn giản (nếu có đủ data)
    if len(df_result) > 50:
        from quantvn.vn.metrics.backtest import Backtest_Stock
        from quantvn.vn.metrics.metrics import Metrics

        print("🧪 Chạy backtest...")
        bt = Backtest_Stock(df_result, pnl_type="after_fees", min_hold_days=3)
        metrics = Metrics(bt)

        print(f"   Sharpe Ratio: {metrics.sharpe():.2f}")
        print(f"   Win Rate: {metrics.win_rate()*100:.1f}%")
        print(f"   Max Drawdown: {metrics.max_drawdown()*100:.2f}%")
        print(f"   Profit Factor: {metrics.profit_factor():.2f}")
        print(f"   Sortino Ratio: {metrics.sortino():.2f}")
        print(f"   Vốn tối thiểu ước tính: {bt.estimate_minimum_capital():,.0f} VND")
        print()
        print("✅ Backtest hoàn tất!")
