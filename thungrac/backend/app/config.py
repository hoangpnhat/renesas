"""
Configuration settings for the Smart Trash Bin System.
"""
import os
from pathlib import Path
from pydantic_settings import BaseSettings


class Settings(BaseSettings):
    """Application settings."""

    # Server
    HOST: str = "localhost"
    PORT: int = 8000
    DEBUG: bool = True

    # Demo Mode (bypass face recognition issues)
    DEMO_MODE: bool = True  # Set to False when face recognition is working

    # Paths
    BASE_DIR: Path = Path(__file__).parent.parent
    DATA_DIR: Path = BASE_DIR / "data"
    FACE_ENCODINGS_DIR: Path = DATA_DIR / "face_encodings"
    TRASH_IMAGES_DIR: Path = DATA_DIR / "trash_images"
    ML_MODELS_DIR: Path = BASE_DIR / "ml_models"

    # CSV Files
    STUDENTS_CSV: Path = DATA_DIR / "students.csv"
    CHECKINS_CSV: Path = DATA_DIR / "checkins.csv"
    TRASH_LOGS_CSV: Path = DATA_DIR / "trash_logs.csv"

    # Face Recognition
    FACE_RECOGNITION_TOLERANCE: float = 0.6
    FACE_DETECTION_MODEL: str = "hog"  # "hog" or "cnn"

    # Trash Classification
    TRASH_MODEL_WEIGHTS_PATH: Path = ML_MODELS_DIR / "trash_classifier" / "mobilenetv2_weights.h5"
    TRASH_CONFIDENCE_THRESHOLD: float = 0.7
    TRASH_IMAGE_SIZE: tuple = (224, 224)

    # Model predicts 6 classes from TrashNet dataset
    TRASH_MODEL_CLASSES: list = ["cardboard", "glass", "metal", "paper", "plastic", "trash"]

    # Points System - Based on 6 TrashNet classes
    POINTS_BASE_CHECKIN: int = 0  # No longer used - points only from trash
    POINTS_CARDBOARD: int = 5
    POINTS_GLASS: int = 5
    POINTS_METAL: int = 5
    POINTS_PAPER: int = 5
    POINTS_PLASTIC: int = 5
    POINTS_TRASH: int = 3  # General waste

    # Business Rules
    ACCEPT_HAZARDOUS: bool = False
    REQUIRE_MIN_CONFIDENCE: bool = True

    # Session Management
    SESSION_TIMEOUT_SECONDS: int = 60  # Timeout for trash deposit after check-in
    SESSION_CLEANUP_INTERVAL_SECONDS: int = 10  # How often to check for expired sessions

    # CORS
    ALLOW_ORIGINS: list = ["*"]
    ALLOW_CREDENTIALS: bool = True
    ALLOW_METHODS: list = ["*"]
    ALLOW_HEADERS: list = ["*"]

    class Config:
        env_file = ".env"
        case_sensitive = True


# Global settings instance
settings = Settings()


def ensure_directories():
    """Ensure all required directories exist."""
    settings.DATA_DIR.mkdir(parents=True, exist_ok=True)
    settings.FACE_ENCODINGS_DIR.mkdir(parents=True, exist_ok=True)
    settings.TRASH_IMAGES_DIR.mkdir(parents=True, exist_ok=True)
    settings.ML_MODELS_DIR.mkdir(parents=True, exist_ok=True)
