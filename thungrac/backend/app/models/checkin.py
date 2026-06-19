"""Check-in data models."""
from typing import Optional
from pydantic import BaseModel, Field


class CheckinCreate(BaseModel):
    """Model for creating a check-in."""

    student_id: str
    student_name: str
    class_name: str
    timestamp: str
    points_awarded: int
    confidence_score: float


class Checkin(BaseModel):
    """Check-in model."""

    checkin_id: str
    student_id: str
    student_name: str
    class_name: str
    timestamp: str
    points_awarded: int
    confidence_score: float


class CheckinResponse(BaseModel):
    """Response model for check-in operations."""

    status: str
    student_id: Optional[str] = None
    student_name: Optional[str] = None
    class_name: Optional[str] = Field(None, alias="class")
    points_awarded: Optional[int] = None
    total_points: Optional[int] = None
    message: str
    checkin_id: Optional[str] = None
    confidence_score: Optional[float] = None

    class Config:
        populate_by_name = True
