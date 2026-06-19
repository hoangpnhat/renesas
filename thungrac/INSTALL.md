# Installation Guide - Smart Trash Bin System

## Critical Dependency Requirements

### ⚠️ IMPORTANT: Version Compatibility

The face recognition features require **specific version combinations**:

- **numpy**: Must be `<2.0` (use 1.26.x)
- **pandas**: Must be `<3.0` (use 2.x)
- **opencv-python-headless**: Must be `<4.10` (use 4.9.x)
- **dlib**: Must use pre-built wheel (19.24.99) from [z-mahmud22/Dlib_Windows_Python3.x](https://github.com/z-mahmud22/Dlib_Windows_Python3.x)
- **setuptools**: Must be `<70` (for pkg_resources support)

### Why These Versions?

1. **dlib pre-built wheel** is compiled against numpy 1.x
2. **opencv 4.10+** requires numpy 2.x
3. **pandas 3.x** requires numpy 2.x
4. **face_recognition_models** needs pkg_resources from setuptools <70

## Installation Steps (Windows)

### 1. Clone/Navigate to Project
```bash
cd c:\Users\a5153207\Documents\code\thungrac
```

### 2. Create Virtual Environment
```bash
python -m venv .venv
# Or use uv (recommended)
uv venv
```

### 3. Install dlib Pre-built Wheel
```bash
uv pip install https://github.com/z-mahmud22/Dlib_Windows_Python3.x/raw/main/dlib-19.24.99-cp312-cp312-win_amd64.whl
```

### 4. Install face_recognition_models
```bash
uv pip install git+https://github.com/ageitgey/face_recognition_models
```

### 5. Downgrade setuptools
```bash
uv pip install "setuptools<70"
```

### 6. Install Other Dependencies
```bash
cd backend
uv pip install -r requirements.txt
```

### 7. Verify Installation
```bash
uv run python -c "
import face_recognition
import numpy as np
print('✓ numpy:', np.__version__)
print('✓ face_recognition:', face_recognition.__version__)

# Test with dummy image
img = np.zeros((100, 100, 3), dtype=np.uint8)
locations = face_recognition.face_locations(img, model='hog')
print('✓ face_locations works! Found', len(locations), 'faces')
print('')
print('SUCCESS: All components working!')
"
```

## Running the Server

```bash
cd backend
uv run run.py
```

Server will start at: **http://localhost:8000**

## Testing

### 1. Test Registration
- Open: http://localhost:8000/register.html
- Fill in name and class
- Click "Bật Camera" and capture photo
- Click "Đăng Ký"

### 2. Test Auto Check-in
- Open: http://localhost:8000/ (homepage is now auto check-in)
- Stand in front of camera
- System will auto-detect and check you in

### 3. View Statistics
- Open: http://localhost:8000/statistics.html
- View student rankings, class rankings, and trash stats

## Troubleshooting

### Error: "Unsupported image type"
**Cause**: numpy 2.x is installed
**Fix**:
```bash
uv pip install "numpy<2.0"
uv pip install --force-reinstall "pandas<3.0" "opencv-python-headless<4.10"
```

### Error: "No module named 'pkg_resources'"
**Cause**: setuptools 70+ doesn't include pkg_resources
**Fix**:
```bash
uv pip install "setuptools<70"
```

### Error: "DLL load failed while importing _dlib_pybind11"
**Cause**: Using PyPI dlib instead of pre-built wheel
**Fix**: Reinstall dlib from pre-built wheel (see step 3 above)

### Error: "No module named 'cv2'"
**Cause**: opencv-python-headless not installed
**Fix**:
```bash
uv pip install "opencv-python-headless<4.10"
```

## Verified Working Configuration

```
Python: 3.12.12
dlib: 19.24.99 (from wheel)
face-recognition: 1.3.0
face-recognition-models: 0.3.0 (from git)
numpy: 1.26.4
pandas: 2.3.3
opencv-python-headless: 4.9.0.80
setuptools: 69.5.1
fastapi: 0.135.3
uvicorn: 0.44.0
```

## Architecture Overview

```
Frontend (Auto Check-in) → FastAPI Backend → Face Recognition Service → dlib
                                          ↓
                                    CSV Database
                                          ↓
                                    Statistics API
```

## Pages

- **/** - Auto check-in (camera always on)
- **/register.html** - Student registration
- **/statistics.html** - Rankings and stats
- **/guide.html** - User guide

## API Endpoints

- `POST /api/esp32/face-checkin` - Face recognition check-in
- `POST /api/esp32/classify-trash` - Trash classification
- `POST /api/students/register` - Register new student
- `GET /api/statistics/dashboard` - Dashboard data
- `GET /api/statistics/student-rankings` - Top students
- `GET /api/statistics/class-rankings` - Class comparison

## Notes

- Face recognition uses HOG model by default (faster, less accurate)
- To use CNN model (slower, more accurate), change `FACE_DETECTION_MODEL` in config.py
- Trash classification uses dummy/random mode by default (no model loaded)
- To add real trash classification model, place model.h5 in `backend/ml_models/trash_classifier/`
