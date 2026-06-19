"""Image processing utilities."""
import base64
import io
from typing import Tuple
import numpy as np
from PIL import Image
import cv2


def base64_to_numpy(base64_string: str) -> np.ndarray:
    """Convert base64 encoded image to numpy array."""
    # Remove data URL prefix if present
    if ',' in base64_string:
        base64_string = base64_string.split(',')[1]

    # Decode base64
    image_bytes = base64.b64decode(base64_string)

    # Use OpenCV to decode (more compatible with dlib)
    nparr = np.frombuffer(image_bytes, np.uint8)
    image = cv2.imdecode(nparr, cv2.IMREAD_COLOR)

    if image is None:
        # Fallback to PIL if cv2 fails
        pil_image = Image.open(io.BytesIO(image_bytes))
        if pil_image.mode != 'RGB':
            pil_image = pil_image.convert('RGB')
        image = np.array(pil_image, dtype=np.uint8)
        # Ensure contiguous array
        return np.ascontiguousarray(image)

    # Convert BGR to RGB (OpenCV loads as BGR)
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

    # Ensure contiguous array in memory (required by dlib)
    return np.ascontiguousarray(image)


def numpy_to_base64(image: np.ndarray) -> str:
    """Convert numpy array to base64 encoded string."""
    # Convert BGR to RGB if from OpenCV
    if len(image.shape) == 3 and image.shape[2] == 3:
        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

    # Convert to PIL Image
    pil_image = Image.fromarray(image.astype('uint8'))

    # Save to bytes
    buffer = io.BytesIO()
    pil_image.save(buffer, format='JPEG')
    buffer.seek(0)

    # Encode to base64
    return base64.b64encode(buffer.getvalue()).decode('utf-8')


def resize_image(image: np.ndarray, size: Tuple[int, int]) -> np.ndarray:
    """Resize image to specified dimensions."""
    return cv2.resize(image, size, interpolation=cv2.INTER_AREA)


def save_image(image: np.ndarray, filepath: str) -> bool:
    """Save numpy array as image file."""
    try:
        # Convert RGB to BGR for OpenCV
        if len(image.shape) == 3 and image.shape[2] == 3:
            image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)

        cv2.imwrite(filepath, image)
        return True
    except Exception:
        return False


def load_image(filepath: str) -> np.ndarray:
    """Load image file as numpy array (RGB)."""
    image = cv2.imread(filepath)
    if image is None:
        raise ValueError(f"Failed to load image: {filepath}")

    # Convert BGR to RGB
    return cv2.cvtColor(image, cv2.COLOR_BGR2RGB)


def preprocess_for_classification(
    image: np.ndarray,
    target_size: Tuple[int, int] = (224, 224)
) -> np.ndarray:
    """Preprocess image for trash classification model."""
    # Resize
    resized = resize_image(image, target_size)

    # Normalize to [0, 1]
    normalized = resized.astype(np.float32) / 255.0

    # Add batch dimension
    return np.expand_dims(normalized, axis=0)
