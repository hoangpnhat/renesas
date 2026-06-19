"""Trash classification service using ML model."""
import os
import random
from typing import Dict, Tuple
import numpy as np

from app.config import settings
from app.utils.image_processing import base64_to_numpy, preprocess_for_classification


class TrashClassificationService:
    """Service for trash classification using ML model."""

    def __init__(self):
        """Initialize trash classification service."""
        self.model = None
        self.model_classes = settings.TRASH_MODEL_CLASSES  # 6 classes from TrashNet
        self.confidence_threshold = settings.TRASH_CONFIDENCE_THRESHOLD
        self.weights_path = settings.TRASH_MODEL_WEIGHTS_PATH
        self.use_dummy = False

        # Try to load model
        self._load_model()

    def _load_model(self):
        """Load the trash classification model."""
        try:
            if self.weights_path.exists():
                import tensorflow as tf
                from tensorflow.keras.applications import MobileNetV2
                from tensorflow.keras.models import Model
                from tensorflow.keras.layers import GlobalAveragePooling2D, Dense, Dropout

                # Recreate model architecture (same as training)
                base_model = MobileNetV2(
                    weights='imagenet',
                    include_top=False,
                    input_shape=(224, 224, 3)
                )

                # Freeze first 100 layers
                for layer in base_model.layers[:100]:
                    layer.trainable = False

                # Unfreeze layers 100-153
                for layer in base_model.layers[100:]:
                    layer.trainable = True

                # Add custom layers
                x = base_model.output
                x = GlobalAveragePooling2D()(x)
                x = Dropout(0.5)(x)
                predictions = Dense(6, activation='softmax')(x)  # 6 classes

                # Create model
                self.model = Model(inputs=base_model.input, outputs=predictions)

                # Load trained weights
                self.model.load_weights(str(self.weights_path))

                print(f"[OK] Loaded trash classification model from {self.weights_path}")
                print(f"[OK] Model predicts {len(self.model_classes)} classes: {self.model_classes}")
            else:
                print(f"Model weights not found at {self.weights_path}. Using dummy classification.")
                self.use_dummy = True
        except Exception as e:
            print(f"Error loading model: {str(e)}. Using dummy classification.")
            import traceback
            traceback.print_exc()
            self.use_dummy = True

    async def classify_trash(self, image_base64: str) -> Dict[str, any]:
        """
        Classify trash type from image.

        Args:
            image_base64: Base64 encoded image string

        Returns:
            Dict with trash_type and confidence
        """
        try:
            # Convert base64 to numpy array
            image = base64_to_numpy(image_base64)

            if self.use_dummy:
                # Use dummy classification for development/testing
                return self._dummy_classify()

            # Preprocess image for model
            preprocessed = preprocess_for_classification(
                image,
                target_size=settings.TRASH_IMAGE_SIZE
            )

            # Run inference
            predictions = self.model.predict(preprocessed, verbose=0)[0]

            # Get predicted class from model (6 classes)
            predicted_index = np.argmax(predictions)
            trash_type = self.model_classes[predicted_index]
            confidence = float(predictions[predicted_index])

            return {
                'trash_type': trash_type,  # TrashNet class: cardboard, glass, metal, paper, plastic, trash
                'confidence': confidence
            }

        except Exception as e:
            print(f"Error classifying trash: {str(e)}")
            # Fallback to dummy classification
            return self._dummy_classify()

    def _dummy_classify(self) -> Dict[str, any]:
        """
        Dummy classification for development/testing.
        Randomly assigns a trash type with reasonable confidence.
        """
        # Weighted random selection from 6 TrashNet classes
        weights = [0.15, 0.15, 0.15, 0.15, 0.30, 0.10]  # cardboard, glass, metal, paper, plastic, trash
        trash_type = random.choices(self.model_classes, weights=weights)[0]

        # Random confidence between 0.7 and 0.95
        confidence = round(random.uniform(0.7, 0.95), 2)

        return {
            'trash_type': trash_type,
            'confidence': confidence
        }

    def should_accept(self, trash_type: str, confidence: float) -> Tuple[bool, str]:
        """
        Determine if trash should be accepted based on business rules.

        Args:
            trash_type: Type of trash (one of 6 TrashNet classes)
            confidence: Classification confidence

        Returns:
            Tuple of (accepted: bool, message: str)
        """
        # Reject if confidence too low
        if settings.REQUIRE_MIN_CONFIDENCE and confidence < self.confidence_threshold:
            return False, f"Không chắc chắn về loại rác (confidence: {confidence:.2%}). Vui lòng thử lại."

        # Accept with appropriate message for each of the 6 classes
        messages = {
            "cardboard": "Thùng carton! Cảm ơn bạn đã bảo vệ môi trường!",
            "glass": "Thủy tinh! Cảm ơn bạn đã bảo vệ môi trường!",
            "metal": "Kim loại! Cảm ơn bạn đã bảo vệ môi trường!",
            "paper": "Giấy! Cảm ơn bạn đã bảo vệ môi trường!",
            "plastic": "Nhựa! Cảm ơn bạn đã bảo vệ môi trường!",
            "trash": "Rác thông thường. Cảm ơn bạn đã sử dụng thùng rác!"
        }

        return True, messages.get(trash_type, "Rác đã được phân loại!")

    def get_action(self, accepted: bool) -> str:
        """
        Get hardware action based on acceptance.

        Args:
            accepted: Whether trash is accepted

        Returns:
            Action string for ESP32: "open_lid" or "keep_closed"
        """
        return "open_lid" if accepted else "keep_closed"


# Global service instance
trash_classification_service = TrashClassificationService()
