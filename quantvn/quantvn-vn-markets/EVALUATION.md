# Phiếu Tự Đánh Giá Chiến Lược

**Họ tên:** Phạm Nhật Hoàng

**GitHub:** https://github.com/hoangpnhat/quantvn-vn-markets

**Mã cổ phiếu test:** VIC (Vingroup) — Khung thời gian 1 giờ

---

# 1. Mô Tả Chiến Lược

## Tên chiến lược

**EMA Trend Following + RSI Entry + Volume Confirmation**

## Ý tưởng chính

Mục tiêu của chiến lược là tìm các cơ hội mua khi cổ phiếu đang có xu hướng tăng nhưng vừa trải qua một nhịp điều chỉnh ngắn hạn. Thay vì mua đuổi khi giá tăng mạnh, chiến lược sẽ chờ tín hiệu hồi về hợp lý rồi mới tham gia.

Chiến lược sử dụng ba yếu tố:

* **EMA 20 và EMA 50** để xác định xu hướng chính.
* **RSI 14** để tìm điểm vào lệnh sau khi giá điều chỉnh.
* **Khối lượng giao dịch (Volume)** để xác nhận tín hiệu.

## Điều kiện mua

Mở vị thế mua khi:

1. EMA20 nằm trên EMA50 và giá đóng cửa nằm trên EMA20.
2. RSI nằm trong khoảng 35–60.
3. Khối lượng giao dịch lớn hơn mức trung bình 20 phiên.

## Điều kiện bán

Thoát lệnh khi:

* Xu hướng suy yếu và RSI ở vùng quá mua.
* Hoặc giá giảm xuống dưới EMA50 kèm RSI thấp và khối lượng tăng mạnh.

## Quản lý vị thế

* Sau khi mua sẽ giữ vị thế cho đến khi xuất hiện tín hiệu bán.
* Có khoảng nghỉ 3 cây nến giữa các giao dịch để tránh giao dịch quá thường xuyên.

---

# 2. Kết Quả Backtest

## Historical (Sau phí giao dịch)

| Chỉ số                   | Giá trị |
| ------------------------ | ------- |
| Lợi nhuận trung bình năm | 41.94%  |
| Max Drawdown             | -30.41% |
| Sharpe Ratio             | 1.38    |

## Out-of-Sample (Sau phí giao dịch)

| Chỉ số                   | Giá trị |
| ------------------------ | ------- |
| Lợi nhuận trung bình năm | 25.52%  |
| Max Drawdown             | -24.52% |
| Sharpe Ratio             | 1.04    |

## Một số chỉ số khác

| Chỉ số                              | Giá trị |
| ----------------------------------- | ------- |
| Tỷ lệ thắng                         | 52.20%  |
| Profit Factor                       | 1.26    |
| Lợi nhuận trung bình mỗi lệnh thắng | 12.58%  |
| Mức lỗ trung bình mỗi lệnh thua     | -10.91% |
| Sharpe Ratio                        | 1.21    |
| Sortino Ratio                       | 1.73    |
| Volatility                          | 17.68%  |

---

# 3. Điểm Mạnh

### 1. Logic đơn giản và dễ hiểu

Chiến lược được xây dựng từ các chỉ báo phổ biến như EMA, RSI và Volume. Các điều kiện vào và ra lệnh tương đối rõ ràng, dễ giải thích và dễ kiểm tra.

### 2. Kết quả kiểm thử khá ổn định

Khi thử trên tập dữ liệu chưa dùng để xây dựng chiến lược (out-of-sample), hiệu suất giảm nhưng vẫn duy trì kết quả dương. Điều này cho thấy chiến lược có khả năng hoạt động ngoài dữ liệu huấn luyện ban đầu.

### 3. Phù hợp với thị trường cổ phiếu Việt Nam

Chiến lược chỉ thực hiện lệnh mua và không sử dụng đòn bẩy, phù hợp với cách giao dịch phổ biến trên thị trường Việt Nam.

### 4. Tỷ lệ thắng và lợi nhuận tương đối cân bằng

Tỷ lệ thắng khoảng 52% không quá cao nhưng lợi nhuận trung bình của các lệnh thắng lớn hơn mức lỗ trung bình, giúp chiến lược duy trì hiệu quả tổng thể.

---

# 4. Điểm Yếu Và Hạn Chế

### 1. Drawdown còn khá lớn

Mức sụt giảm vốn hơn 30% là điểm yếu lớn nhất của chiến lược. Trong thực tế, việc chịu thua lỗ kéo dài có thể gây áp lực tâm lý cho nhà đầu tư.

### 2. Hiệu quả chưa thực sự nổi bật

Profit Factor đạt 1.26 cho thấy chiến lược có lợi nhuận nhưng chưa tạo được khoảng cách quá lớn giữa lợi nhuận và thua lỗ.

### 3. Hoạt động tốt hơn trong thị trường có xu hướng

Khi thị trường đi ngang trong thời gian dài, chiến lược có thể xuất hiện nhiều tín hiệu nhiễu và giao dịch kém hiệu quả.

### 4. Phạm vi kiểm thử còn hẹp

Hiện tại chiến lược mới được kiểm tra trên cổ phiếu VIC. Cần thử thêm trên nhiều mã khác để đánh giá độ ổn định và khả năng áp dụng rộng hơn.

### 5. Code vẫn có thể tối ưu thêm

Một số phần xử lý còn sử dụng vòng lặp Python đơn giản. Với dữ liệu lớn hơn, có thể cần tối ưu để cải thiện tốc độ chạy.

---

# 5. Hướng Cải Thiện Trong Tương Lai

### 1. Bổ sung cơ chế cắt lỗ linh hoạt

Thêm trailing stop để giảm mức drawdown khi thị trường biến động mạnh.

### 2. Lọc điều kiện thị trường tốt hơn

Bổ sung các chỉ báo đánh giá sức mạnh xu hướng nhằm hạn chế giao dịch trong giai đoạn thị trường đi ngang.

### 3. Thử nghiệm trên nhiều cổ phiếu

Kiểm tra chiến lược trên các mã có thanh khoản cao như VIC, FPT, VNM, HPG hoặc MWG để đánh giá độ ổn định.

### 4. Cải thiện quản lý vốn

Thay vì luôn sử dụng cùng một tỷ trọng vốn, có thể nghiên cứu các phương pháp phân bổ vốn linh hoạt hơn.

### 5. Tối ưu và kiểm thử thêm

Thử nghiệm các bộ tham số khác nhau và đánh giá hiệu quả trên nhiều giai đoạn thị trường để tăng độ tin cậy của kết quả.

---

# Kết Luận

Đây là một chiến lược đơn giản được xây dựng từ các chỉ báo kỹ thuật phổ biến với mục tiêu tận dụng xu hướng tăng và tìm điểm vào lệnh hợp lý sau các nhịp điều chỉnh ngắn hạn.

Kết quả backtest cho thấy chiến lược có khả năng tạo lợi nhuận và vẫn duy trì hiệu quả ở tập dữ liệu kiểm thử. Tuy nhiên, drawdown còn khá cao và phạm vi kiểm thử vẫn còn hạn chế.

Thông qua bài làm này, tôi học được cách xây dựng quy tắc giao dịch, kiểm thử chiến lược trên dữ liệu lịch sử và đánh giá hiệu quả bằng các chỉ số cơ bản. Trong tương lai, tôi muốn tiếp tục cải thiện phần quản lý rủi ro, mở rộng phạm vi đánh giá và tối ưu hệ thống để tăng tính ổn định khi áp dụng thực tế.
