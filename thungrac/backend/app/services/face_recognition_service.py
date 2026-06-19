"""Face recognition service for student identification."""
import os
from typing import Optional, Dict
import numpy as np
from pathlib import Path

from app.config import settings
from app.utils.image_processing import base64_to_numpy

# Try to import face_recognition, gracefully handle if not installed
try:
    import face_recognition
    FACE_RECOGNITION_AVAILABLE = True
except ImportError:
    FACE_RECOGNITION_AVAILABLE = False
    print("WARNING: face-recognition library not installed. Face recognition features will be disabled.")
    print("Install with: pip install face-recognition")


class FaceRecognitionService:
    """Service for face recognition operations."""

    def __init__(self):
        """Initialize face recognition service."""
        self.available = FACE_RECOGNITION_AVAILABLE
        self.tolerance = settings.FACE_RECOGNITION_TOLERANCE
        self.model = settings.FACE_DETECTION_MODEL
        self.encodings_dir = settings.FACE_ENCODINGS_DIR

    async def register_face(self, image_base64: str, student_id: str) -> bool:
        """
        Register a student's face by generating and saving encoding.

        Args:
            image_base64: Base64 encoded image string
            student_id: Student ID

        Returns:
            True if successful, False otherwise
        """
        if not self.available:
            raise ValueError("Face recognition library not installed. Please install face-recognition package.")

        try:
            # Convert base64 to numpy array
            image = base64_to_numpy(image_base64)

            # Debug: Print image properties
            print(f"DEBUG register_face:")
            print(f"  Shape: {image.shape}")
            print(f"  Dtype: {image.dtype}")
            print(f"  Min/Max: {image.min()}/{image.max()}")
            print(f"  Contiguous: {image.flags['C_CONTIGUOUS']}")
            print(f"  Order: {image.flags['F_CONTIGUOUS']}")

            # Detect face locations
            face_locations = face_recognition.face_locations(
                image,
                model=self.model
            )

            if len(face_locations) == 0:
                raise ValueError("No face detected in image")

            if len(face_locations) > 1:
                raise ValueError("Multiple faces detected. Please ensure only one face is in the image")

            # Generate face encoding (128-d vector)
            face_encodings = face_recognition.face_encodings(
                image,
                face_locations
            )

            if len(face_encodings) == 0:
                raise ValueError("Failed to generate face encoding")

            encoding = face_encodings[0]

            # Save encoding to file
            encoding_path = self.encodings_dir / f"{student_id}.npy"
            np.save(encoding_path, encoding)

            return True

        except Exception as e:
            print(f"Error registering face: {str(e)}")
            return False

    async def recognize_face(self, image_base64: str) -> Optional[Dict]:
        """
        Recognize a face and return student information.

        Args:
            image_base64: Base64 encoded image string

        Returns:
            Dict with student_id and confidence_score if recognized, None otherwise
        """
        if not self.available:
            raise ValueError("Face recognition library not installed. Please install face-recognition package.")

        try:
            # Convert base64 to numpy array
            image = base64_to_numpy(image_base64)

            # Detect face
            face_locations = face_recognition.face_locations(
                image,
                model=self.model
            )

            if len(face_locations) == 0:
                raise ValueError("No face detected in image")

            # Use only the first face if multiple detected
            face_encodings = face_recognition.face_encodings(
                image,
                [face_locations[0]]
            )

            if len(face_encodings) == 0:
                raise ValueError("Failed to generate face encoding")

            unknown_encoding = face_encodings[0]

            # Load all known encodings
            known_encodings = []
            known_ids = []

            for encoding_file in self.encodings_dir.glob("*.npy"):
                student_id = encoding_file.stem
                encoding = np.load(encoding_file)
                known_encodings.append(encoding)
                known_ids.append(student_id)

            if len(known_encodings) == 0:
                return None

            # Compare faces
            matches = face_recognition.compare_faces(
                known_encodings,
                unknown_encoding,
                tolerance=self.tolerance
            )

            # Calculate face distances
            face_distances = face_recognition.face_distance(
                known_encodings,
                unknown_encoding
            )

            # Find best match
            best_match_index = np.argmin(face_distances)

            if matches[best_match_index]:
                student_id = known_ids[best_match_index]
                confidence_score = 1 - face_distances[best_match_index]

                return {
                    'student_id': student_id,
                    'confidence_score': float(confidence_score)
                }

            return None

        except Exception as e:
            print(f"Error recognizing face: {str(e)}")
            return None

    async def delete_face_encoding(self, student_id: str) -> bool:
        """Delete a student's face encoding."""
        try:
            encoding_path = self.encodings_dir / f"{student_id}.npy"
            if encoding_path.exists():
                encoding_path.unlink()
            return True
        except Exception:
            return False


# Global service instance
face_recognition_service = FaceRecognitionService()
