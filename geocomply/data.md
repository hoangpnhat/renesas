📊 Giải thích về Data và Features
1. TỔNG QUAN VỀ DATASET
Cấu trúc dataset

📁 Data
├── train.csv: 14,889 events (17.2% bị spoof)
└── test.csv: 4,922 events (17.8% bị spoof)

Tổng: ~20,000 location events từ ~3,000 thiết bị giả lập
Thời gian: 1 năm (2024)
Mục đích
Dataset này mô phỏng dữ liệu location từ mobile SDK (iOS/Android) để train model phát hiện location spoofing (giả mạo vị trí GPS).

2. CÁC LOẠI FEATURES (50 FEATURES TỔNG CỘNG)
A. 33 BASE FEATURES (Features cơ bản - single event)
📍 GPS Features (9 features)
Feature	Giải thích	Ví dụ	Thu thập từ
latitude	Vĩ độ GPS	37.7749 (San Francisco)	GPS sensor
longitude	Kinh độ GPS	-122.4194	GPS sensor
accuracy	Độ chính xác GPS (meters)	5.2m	GPS sensor
altitude	Độ cao	15m	GPS + barometer
speed	Vận tốc tức thời	25 m/s (90 km/h)	GPS
bearing	Hướng di chuyển	45° (đông bắc)	GPS + magnetometer
speed_kmh	Vận tốc (km/h)	90 km/h	Derived từ speed
is_stationary	Đang đứng yên?	True/False	Derived (speed < 1 m/s)
is_fast_moving	Đang di chuyển nhanh?	True/False	Derived (speed > 20 m/s)
Giá trị phát hiện spoofing:

Perfect accuracy (< 2m): Nghi ngờ - GPS giả thường có accuracy quá hoàn hảo
Speed bất thường: Vận tốc quá cao báo hiệu teleportation
🌐 Network Features (8 features)
Feature	Giải thích	Giá trị phát hiện
ip_address	Địa chỉ IP	192.168.1.45
network_type	Loại mạng	WiFi, 4G, 5G
wifi_count	Số WiFi nhìn thấy	5 networks
cell_tower_count	Số trạm BTS	3 towers
ip_region	Vùng IP	west/east/central
gps_region	Vùng GPS	west/east/central
ip_matches_gps	IP và GPS khớp?	True/False
Giá trị phát hiện: IP ở New York nhưng GPS ở California → VPN + spoofing

📱 Sensor Features (6 features)
Feature	Giải thích	Phát hiện gì?
accelerometer_variance	Biến thiên gia tốc kế	0.15 (đang di chuyển)
gyroscope_variance	Biến thiên con quay hồi chuyển	0.08
low_accel_variance	Accel quá thấp?	True → sensor_mismatch
low_gyro_variance	Gyro quá thấp?	True → sensor_mismatch
Giá trị phát hiện: Đang lái xe 80 km/h nhưng accelerometer = 0.001 → GPS giả (không có chuyển động thật)

🔋 Device Features (5 features)
Feature	Giải thích	Phát hiện gì?
battery_level	% pin	75%
is_charging	Đang sạc?	True/False
device_timezone_offset	Múi giờ thiết bị	-8 (PST)
tz_expected	Múi giờ dự kiến từ GPS	-8
tz_mismatch	Múi giờ không khớp?	True/False
Giá trị phát hiện: GPS ở California (UTC-8) nhưng timezone thiết bị là UTC-5 (New York) → timezone_mismatch

🔐 Security Features (5 features)
Feature	Giải thích	Phát hiện
mock_location_enabled	Bật "Fake GPS"?	True = 100% spoofing
developer_options_enabled	Bật developer mode?	True = risk factor
location_permission	Quyền truy cập location	ALWAYS/WHILE_IN_USE
Giá trị phát hiện: mock_location_enabled = True → High-confidence spoofing

B. 17 SEQUENTIAL FEATURES (Temporal context - breakthrough!)
Đây là KEY INNOVATION giúp F1 tăng từ 88.6% lên 95.6% (+7 points)!

🔄 Previous/Next Event Context
Feature	Công thức	Ý nghĩa
prev_latitude	lat của event trước	Vị trí trước đó
prev_longitude	lon của event trước	
prev_speed	speed của event trước	
prev_accuracy	accuracy của event trước	
next_latitude	lat của event sau	Vị trí tiếp theo
next_longitude	lon của event sau	
time_delta	timestamp[i] - timestamp[i-1]	Khoảng thời gian (giây)
🚀 Velocity & Acceleration (Quan trọng nhất!)

distance_from_prev = haversine(prev_lat, prev_lon, current_lat, current_lon)  # km
time_delta = current_time - prev_time  # seconds

velocity_from_prev = distance_from_prev / (time_delta / 3600)  # km/h
acceleration = velocity[i] - velocity[i-1]  # km/h change
Ví dụ phát hiện teleportation:


Event 1: San Francisco (37.77, -122.41) at 10:00:00
Event 2: Los Angeles (34.05, -118.24) at 10:01:00

distance_from_prev = 559 km (haversine)
time_delta = 60 seconds
velocity_from_prev = 559 / (60/3600) = 33,540 km/h ❌ (Impossible!)
⚠️ Anomaly Flags (3 features quan trọng)
Flag	Điều kiện	Phát hiện
extreme_velocity	velocity > 150 km/h	Teleportation
sudden_stop	prev_speed > 10 AND speed < 1	GPS đột ngột dừng (bất thường)
sudden_acceleration	|acceleration| > 50 km/h	Tăng tốc đột ngột (vật lý không hợp lý)
3. 5 LOẠI SPOOFING PATTERNS
1️⃣ Teleportation (20% spoofs)

Indicators:
- extreme_velocity = 1 (velocity > 150 km/h)
- distance_from_prev rất lớn (>100 km)
- time_delta rất nhỏ (<1 phút)

Example: NYC → LA trong 2 phút
2️⃣ IP/Geo Mismatch (20% spoofs)

Indicators:
- ip_matches_gps = False
- IP region khác GPS region
- Khoảng cách IP-GPS > 5 degrees

Example: GPS ở California, IP ở New York (VPN)
3️⃣ Mock Location (20% spoofs)

Indicators:
- mock_location_enabled = True
- developer_options_enabled = True
- Thường có accuracy rất thấp (~1m)

Example: User cài "Fake GPS" app
4️⃣ Sensor Mismatch (20% spoofs)

Indicators:
- is_fast_moving = True (speed > 20 m/s)
- low_accel_variance = True (<0.01)
- low_gyro_variance = True (<0.01)

Example: Đang lái xe 80 km/h nhưng accelerometer = 0
→ GPS giả không mô phỏng sensor data
5️⃣ Timezone Mismatch (20% spoofs)

Indicators:
- tz_mismatch = True
- |tz_expected - device_timezone_offset| > 1

Example: GPS ở California (UTC-8) nhưng device timezone = UTC-5
4. TẠI SAO SEQUENTIAL FEATURES LÀ BREAKTHROUGH?
❌ Problem với Base Features (33 features)

Single event: lat=34.05, lon=-118.24, speed=25 m/s
→ Không phát hiện được: Có thể là legitimate hoặc spoof!
✅ Solution với Sequential Features (+17 features)

Event sequence:
t=0:  lat=37.77, lon=-122.41 (SF)
t=60: lat=34.05, lon=-118.24 (LA)

velocity_from_prev = 33,540 km/h ❌
extreme_velocity = 1
→ DETECTED: Teleportation spoof!
📈 Impact

Model                      F1 Score  Recall  Missed Spoofs
─────────────────────────  ────────  ──────  ─────────────
XGBoost (Base features)    88.6%     89.8%   89/874
XGBoost + Sequential       95.6%     98.7%   11/874 ⭐

Improvement: +7% F1, -78 missed spoofs (-88%!)
5. FEATURE ENGINEERING WORKFLOW

📥 Raw SDK Data (33 fields)
    ↓
🔧 Feature Engineering
    ├─ Derived features (speed_kmh, is_stationary, etc.)
    ├─ IP/GPS region mapping
    ├─ Timezone calculations
    └─ Sensor thresholds
    ↓
📊 Base Features (33 dimensions)
    ↓
🔄 Sequential Feature Engineering (ADVANCED)
    ├─ Sort by device_id + timestamp
    ├─ Calculate prev/next context
    ├─ Haversine distance
    ├─ Velocity & acceleration
    └─ Anomaly flags
    ↓
📊 Full Feature Set (50 dimensions)
    ↓
🤖 ML Models (XGBoost, LSTM, Ensemble)
    ↓
🎯 Spoof Detection (95.6% F1)
6. LỰA CHỌN FEATURES THEO USE CASE
🚀 For High Recall (Catch all spoofs)

Top features for XGBoost + Sequential:
1. extreme_velocity (teleportation flag)
2. mock_location_enabled (direct indicator)
3. velocity_from_prev (temporal)
4. distance_from_prev (trajectory)
5. ip_matches_gps (network)
6. low_accel_variance (sensor)
🎯 For High Precision (No false alarms)

Top features for LSTM:
- Learns patterns from 5-event sequences
- Considers: velocity trajectory, acceleration pattern, sensor consistency
- Automatically captures temporal dependencies
🎓 TÓM TẮT
33 Base Features: Đủ cho baseline model (F1 = 88.6%)
+17 Sequential Features: Breakthrough → F1 = 95.6% ⭐
Key Innovation: Temporal context (velocity, acceleration, trajectory)
5 Spoof Types: Mỗi loại có indicators riêng
Real-world applicable: Tất cả features đều thu thập được từ mobile SDK
Sequential features giúp detect:

✅ Teleportation (extreme velocity)
✅ Impossible acceleration
✅ Trajectory anomalies
✅ Movement inconsistencies