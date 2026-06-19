# ESP32 Integration Guide

## Kiến Trúc Hệ Thống (Luồng Mới - Web-Controlled)

```
┌─────────────────────────────────┐         ┌──────────────────┐         ┌─────────────┐
│       Máy Tính (Website)        │         │   Backend API    │         │    ESP32    │
│                                 │◄────────┤   (FastAPI)      │◄────────┤  (Thùng rác)│
│  Camera 1: Face Check-in        │  HTTP   │   localhost:8000 │  HTTP   │             │
│  Camera 2: Trash Classification │         │                  │         │ - Servo     │
│                                 │         │  - Face Recog    │         │ - LED       │
│  ┌──────────────────────────┐   │         │  - Trash Classify│         │ - Display   │
│  │  Web điều khiển toàn bộ  │   │         │  - Bin Control   │         │             │
│  └──────────────────────────┘   │         │                  │         │             │
└─────────────────────────────────┘         └──────────────────┘         └─────────────┘
          │                                           │                         │
          │ 1. Check-in (Camera 1)                    │                         │
          ├──────────────────────────────────────────►│                         │
          │                                           │                         │
          │ 2. Response (checkin_id, session)         │                         │
          │◄──────────────────────────────────────────┤                         │
          │                                           │                         │
          │ 3. Capture trash (Camera 2)               │                         │
          ├──────────────────────────────────────────►│                         │
          │                                           │                         │
          │ 4. Classification result                  │                         │
          │◄──────────────────────────────────────────┤                         │
          │                                           │                         │
          │ 5. Send control signal (open/close)       │                         │
          ├──────────────────────────────────────────►│                         │
          │                                           │                         │
          │                                           │ 6. Forward signal       │
          │                                           ├────────────────────────►│
          │                                           │                         │
          │                                           │                         │ 7. Control servo
          │                                           │                         │    & LED
```

## Luồng Hoạt Động (UPDATED - Web-Controlled)

### 1. Check-in (Website → Backend)
```
Học sinh đứng trước Camera 1 (Face Recognition)
    ↓
Website capture ảnh từ Camera 1
    ↓
POST /api/esp32/face-checkin
    {image: base64, bin_id: "WEB001"}
    ↓
Backend: Face Recognition
    ↓
Tạo Session (60s timeout)
    ↓
Response: {
  status: "success",
  student_name: "Phạm Nhật Hoàng",
  class: "10A",
  total_points: 15,
  checkin_id: "CHK0001"
}
    ↓
Website hiển thị: "🎉 Chào mừng Phạm Nhật Hoàng!"
```

### 2. Bật Camera Rác (Website Auto)
```
Sau check-in thành công
    ↓
Website tự động bật Camera 2 (Trash Camera)
    ↓
Hiển thị: "🗑️ Hãy bỏ rác vào thùng"
    ↓
Chờ 3 giây cho học sinh chuẩn bị
```

### 3. Phân Loại Rác (Website → Backend)
```
Học sinh bỏ rác vào thùng
    ↓
Website tự động capture ảnh từ Camera 2
    ↓
POST /api/esp32/classify-trash
    {image: base64, checkin_id: "CHK0001", bin_id: "WEB001"}
    ↓
Backend:
  - Validate session (check timeout)
  - Classify trash (MobileNetV2)
  - Award points
  - Complete session
    ↓
Response: {
  status: "success",
  trash_type: "recyclable",
  accepted: true,
  action: "open",
  points_awarded: 5,
  message: "Rác tái chế! +5 điểm"
}
    ↓
Website hiển thị kết quả
```

### 4. Gửi Tín Hiệu Điều Khiển (Website → Backend → ESP32) **[NEW]**
```
Website nhận kết quả phân loại
    ↓
POST /api/esp32/bin-control
    {
      action: "open" hoặc "keep_closed",
      bin_id: "WEB001",
      trash_type: "recyclable",
      accepted: true
    }
    ↓
Backend log tín hiệu và forward đến ESP32
    ↓
ESP32 nhận tín hiệu:
    ↓
  Nếu action = "open":
    → Mở nắp thùng (servo 90°)
    → LED xanh sáng
    → Display: "✓ Rác được chấp nhận"
    → Sau 3s: Đóng nắp (servo 0°)
    ↓
  Nếu action = "keep_closed":
    → Giữ nắp đóng
    → LED đỏ sáng
    → Display: "✗ Rác không hợp lệ"
    ↓
Website reset về chế độ chờ check-in
```

---

## Cài Đặt Backend

### 1. Cài đặt Python dependencies
```bash
cd backend
python -m venv .venv
.venv\Scripts\activate  # Windows
# source .venv/bin/activate  # Linux/Mac

# Install dependencies
pip install -r requirements.txt
```

### 2. Cấu hình mạng
File `backend/app/config.py`:
```python
class Settings(BaseSettings):
    HOST: str = "0.0.0.0"  # Cho phép ESP32 kết nối từ mạng LAN
    PORT: int = 8000
```

### 3. Chạy server
```bash
cd backend
python -m uvicorn app.main:app --host 0.0.0.0 --port 8000
```

Server sẽ chạy tại:
- **Website**: `http://localhost:8000/`
- **API Docs**: `http://localhost:8000/api/docs`

### 4. Lấy địa chỉ IP của máy tính
```bash
# Windows
ipconfig

# Linux/Mac
ifconfig
```

Ví dụ: `192.168.1.100`

---

## Code ESP32 (Arduino) - UPDATED

> **⚠️ LƯU Ý**: Trong luồng mới, ESP32 **KHÔNG CẦN CAMERA** nữa.
> ESP32 chỉ cần:
> - **Servo motor** để mở/đóng nắp
> - **LED** để hiển thị trạng thái
> - **Display** (optional) để hiển thị thông báo
> - **Lắng nghe tín hiệu** từ backend để điều khiển

### 1. Cấu hình WiFi và Server

```cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Backend server IP (địa chỉ máy tính chạy backend)
const char* serverIP = "192.168.1.100";  // ← THAY ĐỔI ĐỊA CHỈ NÀY
const int serverPort = 8000;

// API endpoints (ESP32 chỉ cần polling endpoint)
String binControlPollURL = "http://" + String(serverIP) + ":" + String(serverPort) + "/api/esp32/bin-status?bin_id=BIN001";

// Hardware pins
#define SERVO_PIN 13
#define LED_GREEN_PIN 14
#define LED_RED_PIN 15

Servo lidServo;
```

### 2. Khởi Tạo Hardware

```cpp
void setup() {
    Serial.begin(115200);

    // Setup pins
    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_RED_PIN, OUTPUT);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_RED_PIN, LOW);

    // Setup servo
    lidServo.attach(SERVO_PIN);
    closeLid();  // Start with closed lid

    // Connect to WiFi
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println("ESP32 ready. Waiting for bin control signals...");
}
```

### 3. Điều Khiển Servo và LED

```cpp
// Mở nắp thùng
void openLid() {
    Serial.println("Opening lid...");
    lidServo.write(90);  // Mở 90 độ
    digitalWrite(LED_GREEN_PIN, HIGH);
    digitalWrite(LED_RED_PIN, LOW);

    // Auto close after 3 seconds
    delay(3000);
    closeLid();
}

// Đóng nắp thùng
void closeLid() {
    Serial.println("Closing lid...");
    lidServo.write(0);  // Đóng về 0 độ
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_RED_PIN, LOW);
}

// Hiển thị lỗi (nắp đóng + LED đỏ)
void showError(String message) {
    Serial.println("Error: " + message);
    digitalWrite(LED_RED_PIN, HIGH);
    digitalWrite(LED_GREEN_PIN, LOW);
    delay(2000);
    digitalWrite(LED_RED_PIN, LOW);
}
```

### 4. Nhận Tín Hiệu Điều Khiển từ Backend

> **Có 2 cách để ESP32 nhận tín hiệu:**
> 1. **Polling** (đơn giản, dễ implement)
> 2. **WebSocket** (realtime, tối ưu hơn)

#### **Cách 1: HTTP Polling (Recommended)**

ESP32 định kỳ gọi API để kiểm tra có tín hiệu điều khiển mới không:

```cpp
#include <ArduinoJson.h>

String lastAction = "";

void loop() {
    // Poll for bin control signals every 1 second
    static unsigned long lastPoll = 0;
    if (millis() - lastPoll > 1000) {
        checkBinControlSignal();
        lastPoll = millis();
    }
}

// Kiểm tra tín hiệu điều khiển từ server
void checkBinControlSignal() {
    HTTPClient http;
    http.begin(binControlPollURL);
    http.setTimeout(5000);  // 5 second timeout

    int httpCode = http.GET();

    if (httpCode == 200) {
        String response = http.getString();

        // Parse JSON response
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, response);

        if (!error) {
            String action = doc["action"].as<String>();
            String trashType = doc["trash_type"].as<String>();
            bool accepted = doc["accepted"];

            // Chỉ xử lý nếu action mới (tránh lặp lại)
            if (action != "" && action != lastAction) {
                Serial.println("New control signal: " + action);
                lastAction = action;

                if (action == "open") {
                    Serial.println("Trash accepted: " + trashType);
                    openLid();
                } else if (action == "keep_closed") {
                    Serial.println("Trash rejected: " + trashType);
                    showError("Rác không hợp lệ");
                }

                // Clear action after processing
                delay(1000);
                lastAction = "";
            }
        }
    } else if (httpCode > 0) {
        Serial.println("HTTP Error: " + String(httpCode));
    }

    http.end();
}
```

**Backend cần thêm endpoint GET /api/esp32/bin-status:**

```python
# backend/app/api/routes/esp32.py

# Global variable to store latest control signal (simple approach)
latest_bin_control = {
    "action": "",
    "trash_type": "",
    "accepted": False,
    "timestamp": None
}

@router.post("/bin-control")
async def bin_control(request: BinControlRequest):
    """Store control signal for ESP32 to poll"""
    global latest_bin_control

    latest_bin_control = {
        "action": request.action,
        "trash_type": request.trash_type,
        "accepted": request.accepted,
        "timestamp": datetime.now().isoformat()
    }

    print(f"[BIN CONTROL] Stored signal: {request.action}")

    return {
        "status": "success",
        "action": request.action,
        "message": f"Signal stored for bin {request.bin_id}"
    }

@router.get("/bin-status")
async def get_bin_status(bin_id: str):
    """ESP32 polls this endpoint to get latest control signal"""
    global latest_bin_control

    # Return and clear the signal
    signal = latest_bin_control.copy()
    latest_bin_control["action"] = ""  # Clear after reading

    return signal
```

#### **Cách 2: WebSocket (Advanced)**

```cpp
#include <WebSocketsClient.h>

WebSocketsClient webSocket;

void setup() {
    // ... WiFi setup ...

    // Connect to WebSocket
    webSocket.begin(serverIP, serverPort, "/ws/esp32/bin001");
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("[WebSocket] Disconnected!");
            break;

        case WStype_CONNECTED:
            Serial.println("[WebSocket] Connected!");
            break;

        case WStype_TEXT:
            Serial.printf("[WebSocket] Message: %s\n", payload);

            // Parse JSON
            DynamicJsonDocument doc(512);
            deserializeJson(doc, payload, length);

            String eventType = doc["type"].as<String>();

            if (eventType == "bin_control") {
                String action = doc["action"].as<String>();
                String trashType = doc["trash_type"].as<String>();
                bool accepted = doc["accepted"];

                if (action == "open") {
                    Serial.println("Opening bin for: " + trashType);
                    openLid();
                } else if (action == "keep_closed") {
                    Serial.println("Bin closed - trash rejected");
                    showError("Rác không hợp lệ");
                }
            }
            break;
    }
}

void loop() {
    webSocket.loop();
}
```

---

## API Endpoints Chi Tiết (UPDATED)

### 1. Face Check-in (Website → Backend)

**Endpoint**: `POST /api/esp32/face-checkin`

**Request Body**:
```json
{
  "image": "base64_encoded_image",
  "bin_id": "WEB001",
  "timestamp": "2026-04-16T10:00:00"
}
```

**Response (Success)**:
```json
{
  "status": "success",
  "student_id": "STU001",
  "student_name": "Nguyen Van A",
  "class_name": "10A1",
  "points_awarded": 0,
  "total_points": 125,
  "message": "Chào mừng, Nguyen Van A! Bạn có 60s để bỏ rác và nhận điểm.",
  "checkin_id": "CHK_1234567890",
  "confidence_score": 0.95
}
```

**Response (Not Registered)**:
```json
{
  "status": "not_registered",
  "message": "Khuôn mặt không được nhận diện. Vui lòng đăng ký trước!"
}
```

**Response (Session Busy)**:
```json
{
  "status": "session_busy",
  "message": "Vui lòng đợi! Session already active for Nguyen Van A. Please wait 45 seconds."
}
```

### 2. Trash Classification (Website → Backend)

**Endpoint**: `POST /api/esp32/classify-trash`

**Request Body**:
```json
{
  "image": "base64_encoded_trash_image",
  "checkin_id": "CHK_1234567890",  // ← Quan trọng!
  "bin_id": "BIN001",
  "timestamp": "2026-04-16T10:00:30"
}
```

**Response (Accepted)**:
```json
{
  "status": "success",
  "trash_type": "recyclable",
  "confidence": 0.95,
  "accepted": true,
  "action": "open_lid",  // ← ESP32 dựa vào đây để mở nắp
  "message": "Rác tái chế! Cảm ơn bạn đã bảo vệ môi trường! Bạn nhận được +5 điểm!",
  "log_id": "LOG_1234567891",
  "points_awarded": 5,
  "total_points": 130
}
```

**Response (Rejected - Low Confidence)**:
```json
{
  "status": "success",
  "trash_type": "recyclable",
  "confidence": 0.65,
  "accepted": false,
  "action": "keep_closed",  // ← Giữ nắp đóng
  "message": "Không chắc chắn về loại rác (confidence: 65%). Vui lòng thử lại.",
  "log_id": "LOG_1234567892",
  "points_awarded": 0,
  "total_points": 125
}
```

**Response (Session Expired)**:
```json
{
  "status": "session_expired",
  "trash_type": "recyclable",
  "confidence": 0.95,
  "accepted": false,
  "action": "keep_closed",
  "message": "Phiên làm việc đã hết hạn! Vui lòng check-in lại.",
  "log_id": "",
  "points_awarded": 0,
  "total_points": null
}
```

### 3. Bin Control Signal (Website → Backend → ESP32) **[NEW]**

**Endpoint**: `POST /api/esp32/bin-control`

**Description**: Website gửi tín hiệu điều khiển đến ESP32 sau khi nhận kết quả phân loại rác.

**Request Body**:
```json
{
  "action": "open",           // "open" hoặc "keep_closed"
  "bin_id": "WEB001",
  "trash_type": "recyclable", // loại rác đã phân loại
  "accepted": true            // rác có được chấp nhận không
}
```

**Response**:
```json
{
  "status": "success",
  "action": "open",
  "bin_id": "WEB001",
  "message": "Signal 'open' sent to bin WEB001"
}
```

**Flow**:
1. Website gọi `/classify-trash` và nhận kết quả
2. Website gọi `/bin-control` với action tương ứng
3. Backend lưu signal vào memory/database
4. ESP32 polling endpoint `/bin-status` để lấy signal
5. ESP32 điều khiển servo/LED dựa trên signal

### 4. Bin Status Polling (ESP32 → Backend) **[NEW]**

**Endpoint**: `GET /api/esp32/bin-status?bin_id=BIN001`

**Description**: ESP32 polling endpoint này để kiểm tra có tín hiệu điều khiển mới không.

**Response (Có tín hiệu)**:
```json
{
  "action": "open",
  "trash_type": "recyclable",
  "accepted": true,
  "timestamp": "2026-04-20T10:30:05"
}
```

**Response (Không có tín hiệu)**:
```json
{
  "action": "",
  "trash_type": "",
  "accepted": false,
  "timestamp": null
}
```

**Usage**: ESP32 gọi endpoint này mỗi 1 giây, nếu `action != ""` thì thực hiện điều khiển.

---

## Session Management

### Quy tắc Session

1. **1 Session tại 1 thời điểm**: Chỉ 1 học sinh được check-in
2. **Timeout 60s**: Session tự động hủy sau 60s nếu không có rác
3. **Checkin ID**: ESP32 phải gửi `checkin_id` khi classify trash
4. **Auto Cleanup**: Background task check expired sessions mỗi 10s

### States

```
IDLE → CHECKED_IN → WAITING_TRASH → COMPLETED
                         ↓
                      (60s)
                         ↓
                     EXPIRED
```

---

## Testing

### 1. Test Face Check-in (từ Postman/curl)

```bash
curl -X POST http://localhost:8000/api/esp32/face-checkin \
  -H "Content-Type: application/json" \
  -d '{
    "image": "BASE64_IMAGE_HERE",
    "bin_id": "TEST001",
    "timestamp": "2026-04-16T10:00:00"
  }'
```

### 2. Test Trash Classification

```bash
curl -X POST http://localhost:8000/api/esp32/classify-trash \
  -H "Content-Type: application/json" \
  -d '{
    "image": "BASE64_TRASH_IMAGE_HERE",
    "checkin_id": "CHK_1234567890",
    "bin_id": "BIN001",
    "timestamp": "2026-04-16T10:00:30"
  }'
```

### 3. Test từ ESP32

1. Upload code lên ESP32
2. Mở Serial Monitor (115200 baud)
3. ESP32 sẽ in log:
   ```
   Connecting to WiFi...
   WiFi connected
   IP address: 192.168.1.200
   Checking for active session...
   Active session: Nguyen Van A
   Trash detected! Sending image...
   Response: {"action":"open_lid","accepted":true,...}
   Opening lid...
   ```

---

## Troubleshooting

### ESP32 không kết nối được

1. **Kiểm tra IP**: `ipconfig` (Windows) / `ifconfig` (Linux)
2. **Kiểm tra Firewall**: Tắt Windows Firewall tạm thời
3. **Kiểm tra port**: Backend phải chạy với `--host 0.0.0.0`
4. **Ping test**:
   ```bash
   # Từ ESP32
   ping 192.168.1.100
   ```

### Session bị expire ngay

- Kiểm tra timeout config: `SESSION_TIMEOUT_SECONDS = 60`
- Check log backend: `[Session] Created for...`
- Verify checkin_id được gửi đúng từ ESP32

### Model classify sai

- Check model weights loaded: `[OK] Loaded trash classification model`
- Test accuracy: `python test_model.py`
- Verify image quality từ ESP32 (resolution, lighting)

---

## Hardware Requirements (UPDATED)

### ESP32 (hoặc ESP32-CAM không cần dùng camera)
- **Flash**: 4MB
- **RAM**: 520KB
- **WiFi**: 802.11 b/g/n
- **Note**: ❌ **KHÔNG CẦN CAMERA** trong luồng mới (Web xử lý camera)

### Servo Motor
- **Voltage**: 5V
- **Torque**: >10 kg·cm (đủ mạnh để mở nắp thùng)
- **Pin**: GPIO 13

### LED (Status Indicator)
- **Green LED**: GPIO 14 (rác được chấp nhận)
- **Red LED**: GPIO 15 (rác bị từ chối)

### Optional: LCD Display
- **I2C LCD 16x2** hoặc **OLED 128x64**
- **Pins**: SDA (GPIO 21), SCL (GPIO 22)
- **Purpose**: Hiển thị thông báo cho user

### Wiring
```
ESP32        │  Servo    │  LED         │  LCD (optional)
─────────────┼───────────┼──────────────┼─────────────────
GPIO 13      │  Signal   │              │
5V           │  VCC      │              │  VCC
GND          │  GND      │  GND (共通)  │  GND
GPIO 14      │           │  Green +     │
GPIO 15      │           │  Red +       │
GPIO 21      │           │              │  SDA
GPIO 22      │           │              │  SCL
```

### Simplified Hardware (So với luồng cũ)
```
TRƯỚC (ESP32-CAM):
✓ Camera OV2640
✓ Servo motor
✓ LED
✓ Complex wiring

SAU (ESP32 thường):
✗ Camera (không cần)
✓ Servo motor
✓ LED
✓ Đơn giản hơn, rẻ hơn
```

---

## Tóm Tắt (UPDATED - Web-Controlled Architecture)

### Máy tính (Backend Server)
- Chạy FastAPI backend: `python -m uvicorn app.main:app --host 0.0.0.0 --port 8000`
- Host website tại `http://IP:8000/`
- Xử lý:
  - ✅ Face recognition (từ Camera 1)
  - ✅ Trash classification (từ Camera 2)
  - ✅ Session management (60s timeout)
  - ✅ Bin control signal management

### Website (Browser) **[UPDATED - Vai trò chính]**
- ✅ **Camera 1**: Check-in học sinh bằng face recognition
- ✅ **Camera 2**: Chụp và phân loại rác
- ✅ Điều khiển luồng hoàn toàn từ web interface
- ✅ Gửi tín hiệu điều khiển đến ESP32
- ✅ Hiển thị thống kê, rankings
- ✅ Giao diện đăng ký học sinh

### ESP32 (Thùng Rác) **[SIMPLIFIED]**
- ❌ **KHÔNG CẦN CAMERA** (camera được xử lý ở web)
- ❌ **KHÔNG CẦN chụp ảnh rác** (web đã chụp)
- ❌ **KHÔNG CẦN gửi request lên server** (chỉ nhận signal)
- ✅ **CHỈ CẦN**:
  - Polling endpoint `/bin-status` mỗi 1 giây
  - Nhận signal: `open` hoặc `keep_closed`
  - Điều khiển servo mở/đóng nắp
  - Điều khiển LED xanh/đỏ
  - (Optional) Hiển thị thông báo trên LCD

### So Sánh Luồng

#### TRƯỚC (ESP32-Based):
```
1. Website check-in face → Backend
2. ESP32 nhận checkin_id
3. User bỏ rác
4. ESP32 chụp ảnh rác → Backend
5. Backend phân loại → Response
6. ESP32 tự điều khiển servo
```

#### SAU (Web-Based): ⭐ **RECOMMENDED**
```
1. Website check-in face (Camera 1) → Backend
2. Backend tạo session
3. Website tự động chụp rác (Camera 2) → Backend
4. Backend phân loại rác
5. Website gửi signal điều khiển → Backend
6. ESP32 polling nhận signal
7. ESP32 điều khiển servo/LED
```

### Ưu Điểm Luồng Mới

1. ✅ **Đơn giản hơn**: ESP32 chỉ là actuator (không cần logic phức tạp)
2. ✅ **Rẻ hơn**: Không cần ESP32-CAM (dùng ESP32 thường)
3. ✅ **Linh hoạt hơn**: Dễ thêm camera, thay đổi logic phân loại
4. ✅ **UX tốt hơn**: User nhìn thấy kết quả trên màn hình lớn
5. ✅ **Dễ debug**: Toàn bộ log tập trung ở web console
6. ✅ **Mở rộng dễ**: Thêm nhiều thùng rác chỉ cần thêm ESP32 đơn giản

---

## Quick Start

### 1. Chạy Backend
```bash
cd backend
python -m uvicorn app.main:app --host 0.0.0.0 --port 8000
```

### 2. Mở Website
```
http://localhost:8000/
```

### 3. Upload Code ESP32
- Sửa WiFi SSID/password
- Sửa `serverIP` = địa chỉ IP máy tính
- Upload code
- ESP32 sẽ tự động polling và nhận tín hiệu

### 4. Test
- Đứng trước Camera 1 → Check-in
- Website tự động chụp Camera 2
- Kết quả hiển thị trên web
- ESP32 tự động mở/đóng nắp

---

Cần hỗ trợ thêm?
- **API Docs**: `http://localhost:8000/api/docs`
- **Workflow Details**: Xem file `WORKFLOW.md`
