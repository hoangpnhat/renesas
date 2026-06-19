"""Trash log data models."""
from typing import Optional
from pydantic import BaseModel


class TrashLogCreate(BaseModel):
    """Model for creating a trash log."""

    checkin_id: Optional[str] = None
    student_id: Optional[str] = None
    timestamp: str
    trash_type: str
    confidence_score: float
    accepted: bool
    image_path: str


class TrashLog(BaseModel):
    """Trash log model."""

    log_id: str
    checkin_id: Optional[str] = None
    student_id: Optional[str] = None
    timestamp: str
    trash_type: str
    confidence_score: float
    accepted: bool
    image_path: str


class TrashClassificationResponse(BaseModel):
    """Response model for trash classification."""

    status: str
    trash_type: str
    confidence: float
    accepted: bool
    action: str  # "open_lid" or "keep_closed"
    message: str
    log_id: Optional[str] = None
    points_awarded: Optional[int] = 0  # Points awarded for this trash
    total_points: Optional[int] = None  # Student's new total points
