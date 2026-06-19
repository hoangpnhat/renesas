import face_recognition
import numpy as np
import cv2

print("Testing dlib face_recognition...")

# Test 1: Load real image file if exists
try:
    # Create a simple test image with opencv
    img = np.zeros((480, 640, 3), dtype=np.uint8)
    
    # Draw a white circle (fake face)
    cv2.circle(img, (320, 240), 100, (255, 255, 255), -1)
    
    print("\n1. Testing with OpenCV-created image:")
    print(f"   Shape: {img.shape}")
    print(f"   Dtype: {img.dtype}")
    print(f"   Contiguous: {img.flags['C_CONTIGUOUS']}")
    
    locations = face_recognition.face_locations(img, model='hog')
    print(f"   Result: Found {len(locations)} faces")
    
except Exception as e:
    print(f"   ERROR: {type(e).__name__}: {e}")

# Test 2: Try with different array orders
try:
    print("\n2. Testing with explicit copy:")
    img2 = np.array(img, order='C', copy=True)
    locations = face_recognition.face_locations(img2, model='hog')
    print(f"   Result: Found {len(locations)} faces")
except Exception as e:
    print(f"   ERROR: {type(e).__name__}: {e}")

# Test 3: Try converting to grayscale
try:
    print("\n3. Testing with grayscale:")
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    print(f"   Shape: {gray.shape}")
    locations = face_recognition.face_locations(gray, model='hog')
    print(f"   Result: Found {len(locations)} faces")
except Exception as e:
    print(f"   ERROR: {type(e).__name__}: {e}")

print("\nDone!")
