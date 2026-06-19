# 🗑️ Hệ Thống Thùng Rác Thông Minh

Hệ thống thùng rác thông minh cho trường học sử dụng AI để nhận diện khuôn mặt học sinh và phân loại rác tự động. Khuyến khích học sinh tham gia bảo vệ môi trường thông qua gamification (điểm số, xếp hạng).

## ✨ Tính Năng Chính

### 1. **Nhận Diện Khuôn Mặt (Face Recognition)**
- Đăng ký học sinh với ảnh khuôn mặt
- Tự động nhận diện và check-in khi học sinh sử dụng thùng rác
- Cộng điểm tự động vào tài khoản học sinh

### 2. **Phân Loại Rác Thông Minh (Trash Classification)**
- ESP32 gửi ảnh rác lên server
- AI phân loại: Tái chế, Hữu cơ, Thông thường, Nguy hại
- Tự động mở/đóng nắp thùng dựa trên kết quả phân loại

### 3. **Web Application**
- **Trang Đăng Ký**: Đăng ký học sinh mới với webcam
- **Trang Check-in**: Hiển thị hoạt động real-time
- **Trang Thống Kê**: Xếp hạng học sinh, lớp và thống kê rác

### 4. **Hệ Thống Điểm**
- Check-in cơ bản: +10 điểm
- Rác tái chế: +5 điểm
- Rác hữu cơ: +5 điểm
- Rác thông thường: +3 điểm

## 🏗️ Kiến Trúc Hệ Thống

```
smart-trash-bin/
├── backend/                     # Backend API (Python + FastAPI)
│   ├── app/
│   │   ├── main.py             # FastAPI application
│   │   ├── config.py           # Configuration
│   │   ├── models/             # Data models
│   │   ├── services/           # Business logic services
│   │   │   ├── csv_service.py
│   │   │   ├── face_recognition_service.py
│   │   │   ├── trash_classification_service.py
│   │   │   └── points_service.py
│   │   ├── api/routes/         # API endpoints
│   │   │   ├── esp32.py        # ESP32 hardware routes
│   │   │   ├── students.py     # Student management
│   │   │   └── statistics.py   # Statistics & rankings
│   │   └── utils/              # Utilities
│   ├── data/                   # CSV database
│   │   ├── students.csv
│   │   ├── checkins.csv
│   │   └── trash_logs.csv
│   ├── ml_models/              # ML models
│   │   └── trash_classifier/
│   └── requirements.txt
├── frontend/                    # Frontend (HTML/CSS/JavaScript)
│   ├── index.html
│   ├── register.html
│   ├── checkin.html
│   ├── statistics.html
│   ├── css/styles.css
│   └── js/
└── README.md
```

## 🚀 Hướng Dẫn Cài Đặt

### Yêu Cầu Hệ Thống

- **Python 3.9+**
- **Webcam** (cho đăng ký học sinh)
- **ESP32 với camera module** (cho hardware)
- **dlib** dependencies (xem chi tiết bên dưới)

### 1. Cài Đặt Dependencies

#### Windows

```bash
# Install CMake (required for dlib)
# Download from: https://cmake.org/download/

# Install Visual Studio Build Tools
# Download from: https://visualstudio.microsoft.com/downloads/

# Install Python dependencies
cd backend
pip install -r requirements.txt
```

**Lưu ý cho Windows**: Nếu gặp lỗi khi cài `dlib`, tải pre-built wheel từ:
- https://github.com/sachadee/Dlib
- Cài đặt: `pip install dlib-19.24.0-cp39-cp39-win_amd64.whl`

#### Linux/macOS

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install cmake build-essential

# macOS
brew install cmake

# Install Python dependencies
cd backend
pip install -r requirements.txt
```

### 2. Khởi Động Server

```bash
# Di chuyển vào thư mục backend
cd backend

# Chạy server
python run.py
```

Server sẽ khởi động tại: **http://localhost:8000**

### 3. Truy Cập Web Application

Mở trình duyệt và truy cập:
- **Trang chủ**: http://localhost:8000/
- **API Documentation**: http://localhost:8000/api/docs

## 📱 Hướng Dẫn Sử Dụng

### Đăng Ký Học Sinh

1. Truy cập: http://localhost:8000/register.html
2. Nhập họ tên và chọn lớp
3. Bấm "Bật Camera" và chụp ảnh khuôn mặt
4. Bấm "Đăng Ký"
5. Lưu lại **Mã học sinh** được cấp

**Lưu ý khi chụp ảnh:**
- Khuôn mặt đối diện thẳng với camera
- Ánh sáng đủ sáng
- Không đeo kính đen hoặc khẩu trang
- Chỉ có một khuôn mặt trong khung hình

### Sử Dụng Thùng Rác (ESP32)

1. **Bước 1**: Đứng trước camera ESP32
2. **Bước 2**: Hệ thống nhận diện khuôn mặt và check-in
3. **Bước 3**: Đưa rác vào camera để phân loại
4. **Bước 4**: Nếu rác hợp lệ, nắp thùng tự động mở

### Xem Thống Kê

Truy cập: http://localhost:8000/statistics.html

- **Tab Xếp Hạng Học Sinh**: Top học sinh có điểm cao nhất
- **Tab Xếp Hạng Lớp**: So sánh điểm số giữa các lớp
- **Tab Thống Kê Rác**: Phân tích loại rác đã thu gom

## 🔌 API Endpoints

### ESP32 Endpoints

#### POST `/api/esp32/face-checkin`
Nhận diện khuôn mặt và check-in học sinh.

**Request:**
```json
{
  "image": "base64_encoded_image",
  "bin_id": "BIN001",
  "timestamp": "2024-01-20T10:30:00"
}
```

**Response:**
```json
{
  "status": "success",
  "student_id": "STU001",
  "student_name": "Nguyễn Văn A",
  "class": "10A",
  "points_awarded": 10,
  "total_points": 150,
  "message": "Chào mừng trở lại, Nguyễn Văn A!",
  "checkin_id": "CHK001"
}
```

#### POST `/api/esp32/classify-trash`
Phân loại rác và trả về tín hiệu điều khiển.

**Request:**
```json
{
  "image": "base64_encoded_image",
  "checkin_id": "CHK001",
  "bin_id": "BIN001"
}
```

**Response:**
```json
{
  "status": "success",
  "trash_type": "recyclable",
  "confidence": 0.89,
  "accepted": true,
  "action": "open_lid",
  "message": "Rác tái chế! Cảm ơn bạn!",
  "log_id": "TRH001"
}
```

### Student Management

- `POST /api/students/register` - Đăng ký học sinh mới
- `GET /api/students` - Lấy danh sách học sinh
- `GET /api/students/{student_id}` - Lấy thông tin học sinh

### Statistics

- `GET /api/statistics/student-rankings` - Xếp hạng học sinh
- `GET /api/statistics/class-rankings` - Xếp hạng lớp
- `GET /api/statistics/trash-breakdown` - Thống kê rác
- `GET /api/statistics/recent-checkins` - Hoạt động gần đây
- `GET /api/statistics/dashboard` - Tổng quan dashboard

Xem full API documentation tại: http://localhost:8000/api/docs

## 🤖 ESP32 Integration

### Hardware Requirements

- ESP32-CAM hoặc ESP32 + OV2640 camera module
- Servo motor (để điều khiển nắp thùng)
- LCD display 16x2 (optional)

### Arduino Code Structure

```cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "esp_camera.h"

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverUrl = "http://YOUR_SERVER_IP:8000";

void setup() {
  // Initialize WiFi
  // Initialize camera
  // Initialize servo motor
}

void loop() {
  // 1. Capture face image
  // 2. Send to /api/esp32/face-checkin
  // 3. If success, wait for trash
  // 4. Capture trash image
  // 5. Send to /api/esp32/classify-trash
  // 6. Control servo based on action
}
```

### Configuration

Cập nhật server URL trong code Arduino:
```cpp
const char* serverUrl = "http://192.168.1.100:8000";  // Thay đổi IP
```

## 🧪 Testing

### Test Backend API

```bash
# Test face registration
curl -X POST http://localhost:8000/api/students/register \
  -F "name=Test Student" \
  -F "class=10A" \
  -F "face_image=@test_face.jpg"

# Test statistics
curl http://localhost:8000/api/statistics/dashboard
```

### Test Frontend

1. Mở http://localhost:8000/register.html
2. Đăng ký một học sinh thử nghiệm
3. Kiểm tra trang check-in và statistics

## 📊 Database Schema (CSV)

### students.csv
```csv
student_id,name,class,registration_date,total_points,face_encoding_path,status
STU001,Nguyễn Văn A,10A,2024-01-15,150,face_encodings/STU001.npy,active
```

### checkins.csv
```csv
checkin_id,student_id,student_name,class,timestamp,points_awarded,confidence_score
CHK001,STU001,Nguyễn Văn A,10A,2024-01-20T10:30:00,10,0.95
```

### trash_logs.csv
```csv
log_id,checkin_id,student_id,timestamp,trash_type,confidence_score,accepted,image_path
TRH001,CHK001,STU001,2024-01-20T10:30:15,recyclable,0.89,true,trash_images/TRH001.jpg
```

## ⚙️ Configuration

Cấu hình hệ thống tại: `backend/app/config.py`

```python
# Face Recognition
FACE_RECOGNITION_TOLERANCE = 0.6  # Ngưỡng nhận diện (thấp hơn = nghiêm ngặt hơn)
FACE_DETECTION_MODEL = "hog"      # "hog" (nhanh) hoặc "cnn" (chính xác)

# Trash Classification
TRASH_CONFIDENCE_THRESHOLD = 0.7   # Ngưỡng tin cậy tối thiểu
ACCEPT_HAZARDOUS = False           # Có chấp nhận rác nguy hại không

# Points System
POINTS_BASE_CHECKIN = 10
POINTS_RECYCLABLE = 5
POINTS_ORGANIC = 5
POINTS_NON_RECYCLABLE = 3
```

## 🔧 Troubleshooting

### Lỗi "dlib not found"

**Windows**: Download pre-built wheel và cài đặt thủ công
```bash
pip install dlib-19.24.0-cp39-cp39-win_amd64.whl
```

**Linux**: Cài đặt build tools
```bash
sudo apt-get install cmake build-essential
pip install dlib
```

### Lỗi "No face detected"

- Đảm bảo ánh sáng đủ sáng
- Khuôn mặt đối diện thẳng với camera
- Thử giảm `FACE_RECOGNITION_TOLERANCE` trong config

### Lỗi "Model not found"

Hệ thống sẽ tự động sử dụng dummy classification nếu chưa có model. Để thêm model thật:
1. Download model MobileNetV2 đã train
2. Đặt tại: `backend/ml_models/trash_classifier/model.h5`
3. Restart server

### ESP32 không kết nối được

- Kiểm tra ESP32 và server cùng mạng WiFi
- Cập nhật đúng IP address của server trong code Arduino
- Kiểm tra firewall không block port 8000

## 📈 Future Enhancements

- [ ] Migrate database từ CSV sang SQLite/PostgreSQL
- [ ] Mobile app cho giáo viên
- [ ] Email/SMS notifications
- [ ] Training interface cho trash classification model
- [ ] Multi-language support
- [ ] Cloud deployment (AWS/Azure)
- [ ] Badges và achievements system
- [ ] Weekly/monthly reports

## 👥 Contributors

Hệ thống được phát triển để hỗ trợ các trường học trong việc giáo dục môi trường và khuyến khích học sinh tham gia bảo vệ môi trường thông qua công nghệ.

## 📄 License

MIT License - Free to use for educational purposes

---

**Made with ❤️ for a greener future**
