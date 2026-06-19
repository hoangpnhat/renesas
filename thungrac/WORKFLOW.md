# Luồng Hoạt Động Hệ Thống Thùng Rác Thông Minh

## Luồng Mới (Web-Based Control)

### 1. Check-in (Camera 1 - Camera Nhận Diện Khuôn Mặt)
```
User đứng trước camera
    ↓
Web capture hình từ camera check-in
    ↓
Gửi ảnh lên Server (POST /esp32/face-checkin)
    ↓
Server nhận diện khuôn mặt
    ↓
Nếu thành công:
    - Tạo session (60s timeout)
    - Trả về: student_name, class, total_points, checkin_id
    - Web hiển thị: "Chào mừng [Tên]! Lớp [Lớp] | Điểm: [X] điểm"
```

### 2. Bật Camera Rác (Camera 2 - Camera Phân Loại Rác)
```
Sau khi check-in thành công
    ↓
Web tự động bật camera thứ 2 (camera chụp rác)
    ↓
Hiển thị thông báo: "🗑️ Hãy bỏ rác vào thùng"
    ↓
Chờ 3 giây cho user chuẩn bị
```

### 3. Chụp & Phân Loại Rác (Camera 2)
```
Sau 3 giây delay
    ↓
Web tự động chụp ảnh từ camera rác
    ↓
Gửi ảnh + checkin_id lên Server (POST /esp32/classify-trash)
    ↓
Server phân loại rác:
    - Xác định loại rác (recyclable/organic/non_recyclable/hazardous)
    - Kiểm tra confidence score
    - Quyết định accept/reject
    - Tính điểm (nếu accepted)
    ↓
Server trả về kết quả:
    - status, trash_type, confidence, accepted
    - points_awarded, total_points
    - message
```

### 4. Gửi Tín Hiệu Điều Khiển ESP32
```
Web nhận kết quả phân loại
    ↓
Hiển thị kết quả cho user:
    - ✅ Accepted: "Rác được chấp nhận! (+X điểm)"
    - ❌ Rejected: "Rác không được chấp nhận"
    ↓
Gửi tín hiệu đến ESP32 (POST /esp32/bin-control):
    - action: "open" hoặc "keep_closed"
    - bin_id: "WEB001"
    - trash_type: loại rác
    - accepted: true/false
    ↓
ESP32 nhận tín hiệu và điều khiển servo:
    - Nếu action = "open" → Mở nắp thùng
    - Nếu action = "keep_closed" → Giữ nắp đóng
```

### 5. Reset & Chờ Check-in Tiếp Theo
```
Sau 5 giây hiển thị kết quả
    ↓
Tắt camera rác
    ↓
Reset về trạng thái chờ check-in
    ↓
Hiển thị: "👋 Xin chào! Đứng trước camera để check-in"
```

## Sơ Đồ Luồng Đầy Đủ

```
┌─────────────────────────────────────────────────────────────────┐
│                         WEB INTERFACE                           │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌──────────────┐                 ┌──────────────┐            │
│  │  Camera 1    │                 │  Camera 2    │            │
│  │  (Check-in)  │                 │  (Trash)     │            │
│  └──────┬───────┘                 └──────┬───────┘            │
│         │                                 │                     │
│         │ 1. Capture face                 │ 3. Capture trash   │
│         ▼                                 ▼                     │
│  ┌──────────────────────────────────────────────────┐          │
│  │          JavaScript (auto-checkin.js)            │          │
│  └──────┬───────────────────────────────────┬───────┘          │
│         │                                   │                   │
└─────────┼───────────────────────────────────┼───────────────────┘
          │                                   │
          │ 2. POST /esp32/face-checkin       │ 4. POST /esp32/classify-trash
          │    {image, bin_id, timestamp}     │    {image, checkin_id, bin_id}
          ▼                                   ▼
┌─────────────────────────────────────────────────────────────────┐
│                         SERVER API                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  Face Recognition ──→ Session Manager ──→ CSV Database         │
│        │                                         ▲               │
│        └─────────────────────────────────────────┘               │
│                                                                 │
│  Trash Classification ──→ Points Service ──→ CSV Database      │
│                                                                 │
└──────────────────────────────┬──────────────────────────────────┘
                               │
                               │ 5. POST /esp32/bin-control
                               │    {action, bin_id, trash_type}
                               ▼
                      ┌─────────────────┐
                      │     ESP32       │
                      │  (Servo Motor)  │
                      └─────────────────┘
                               │
                               ▼
                      ┌─────────────────┐
                      │   Open/Close    │
                      │    Bin Lid      │
                      └─────────────────┘
```

## API Endpoints

### 1. Face Check-in
**POST** `/esp32/face-checkin`

Request:
```json
{
  "image": "base64_encoded_image",
  "bin_id": "WEB001",
  "timestamp": "2026-04-20T10:30:00"
}
```

Response (Success):
```json
{
  "status": "success",
  "student_id": "STU0001",
  "student_name": "Phạm Nhật Hoàng",
  "class": "10A",
  "points_awarded": 0,
  "total_points": 15,
  "message": "Chào mừng, Phạm Nhật Hoàng! Bạn có 60s để bỏ rác.",
  "checkin_id": "CHK0001",
  "confidence_score": 0.95
}
```

### 2. Trash Classification
**POST** `/esp32/classify-trash`

Request:
```json
{
  "image": "base64_encoded_image",
  "checkin_id": "CHK0001",
  "bin_id": "WEB001",
  "timestamp": "2026-04-20T10:30:05"
}
```

Response (Success):
```json
{
  "status": "success",
  "trash_type": "recyclable",
  "confidence": 0.92,
  "accepted": true,
  "action": "open",
  "message": "Rác tái chế - Được chấp nhận! Bạn nhận được +5 điểm!",
  "log_id": "TRH0001",
  "points_awarded": 5,
  "total_points": 20
}
```

### 3. Bin Control (NEW)
**POST** `/esp32/bin-control`

Request:
```json
{
  "action": "open",
  "bin_id": "WEB001",
  "trash_type": "recyclable",
  "accepted": true
}
```

Response:
```json
{
  "status": "success",
  "action": "open",
  "bin_id": "WEB001",
  "message": "Signal 'open' sent to bin WEB001"
}
```

## Thay Đổi So Với Luồng Cũ

### Trước (ESP32-Based):
1. ESP32 capture ảnh khuôn mặt
2. ESP32 gửi lên server
3. Server trả kết quả
4. User bỏ rác
5. ESP32 capture ảnh rác
6. ESP32 tự điều khiển servo

### Sau (Web-Based):
1. ✅ Web capture ảnh khuôn mặt
2. ✅ Web gửi lên server
3. ✅ Server trả kết quả
4. ✅ Web tự động bật camera rác
5. ✅ Web capture ảnh rác
6. ✅ Web gửi lên server phân loại
7. ✅ **Web gửi tín hiệu cho ESP32 mở/đóng cửa**

## Ưu Điểm Luồng Mới

1. **Kiểm soát tập trung**: Web interface điều khiển toàn bộ luồng
2. **Linh hoạt camera**: Có thể sử dụng 2 camera riêng biệt
3. **Trải nghiệm tốt hơn**: User nhìn thấy kết quả phân loại trên màn hình lớn
4. **Dễ debug**: Log tập trung tại web console
5. **Mở rộng dễ**: Có thể thêm nhiều tính năng UI

## Cấu Hình Camera

- **Camera 1 (Check-in)**: Thường là webcam chính của máy tính
- **Camera 2 (Trash)**: Có thể là:
  - Webcam thứ 2 (nếu có)
  - USB camera riêng
  - Fallback về camera 1 nếu không có camera thứ 2

## Timeout & Cooldown

- **Session Timeout**: 60 giây (sau check-in thành công)
- **Check-in Cooldown**: 5 giây (giữa các lần check-in)
- **Trash Capture Delay**: 3 giây (sau khi bật camera rác)
- **Result Display**: 5 giây (trước khi reset về chế độ check-in)
