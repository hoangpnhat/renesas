"""Student data models."""
from datetime import datetime
from typing import Optional
from pydantic import BaseModel, Field


class StudentCreate(BaseModel):
    """Model for creating a new student."""

    name: str = Field(..., min_length=1, max_length=100)
    class_name: str = Field(..., alias="class", min_length=1, max_length=50)

    class Config:
        populate_by_name = True


class Student(BaseModel):
    """Student model."""

    student_id: str
    name: str
    class_name: str = Field(..., alias="class")
    registration_date: str
    total_points: int = 0
    face_encoding_path: str
    status: str = "active"

    class Config:
        populate_by_name = True


class StudentResponse(BaseModel):
    """Response model for student operations."""

    status: str
    student_id: Optional[str] = None
    message: str
    data: Optional[Student] = None
