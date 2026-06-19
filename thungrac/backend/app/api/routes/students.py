"""API routes for student management."""
from fastapi import APIRouter, File, Form, UploadFile, HTTPException, status
from typing import List, Optional

from app.models.student import Student, StudentResponse
from app.services.face_recognition_service import face_recognition_service
from app.services.csv_service import csv_service
from app.utils.image_processing import numpy_to_base64
import base64

router = APIRouter(prefix="/students", tags=["Students"])


@router.post("/register", response_model=StudentResponse)
async def register_student(
    name: str = Form(...),
    class_name: str = Form(..., alias="class"),
    face_image: UploadFile = File(...)
):
    """
    Register a new student with face enrollment.

    Args:
        name: Student name
        class_name: Student class
        face_image: Face photo file

    Returns:
        StudentResponse with student_id and message
    """
    try:
        # Read image file
        image_bytes = await face_image.read()

        # Convert to base64
        image_base64 = base64.b64encode(image_bytes).decode('utf-8')

        # Create student record first (to get student_id)
        student_data = {
            'name': name,
            'class': class_name
        }

        student_id = await csv_service.create_student(student_data)

        # Register face encoding
        success = await face_recognition_service.register_face(
            image_base64,
            student_id
        )

        if not success:
            # Rollback: This is simplified - in production, use proper transaction handling
            raise ValueError("Failed to register face encoding")

        # Get created student
        student = await csv_service.get_student(student_id)

        return StudentResponse(
            status="success",
            student_id=student_id,
            message=f"Học sinh {name} đã được đăng ký thành công!",
            data=Student(**student)
        )

    except ValueError as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=str(e)
        )
    except Exception as e:
        print(f"Error registering student: {str(e)}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Internal server error during student registration"
        )


@router.get("/{student_id}", response_model=Student)
async def get_student(student_id: str):
    """
    Get student information by ID.

    Args:
        student_id: Student ID

    Returns:
        Student information
    """
    student = await csv_service.get_student(student_id)

    if not student:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail=f"Student {student_id} not found"
        )

    return Student(**student)


@router.get("", response_model=List[Student])
async def get_all_students(class_filter: Optional[str] = None):
    """
    Get all students, optionally filtered by class.

    Args:
        class_filter: Filter by class name (optional)

    Returns:
        List of students
    """
    students = await csv_service.get_all_students(class_filter=class_filter)

    return [Student(**s) for s in students]
