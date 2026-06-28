"""API routes for ESP32 hardware communication."""
from datetime import datetime
from fastapi import APIRouter, HTTPException, status
from pydantic import BaseModel

from app.models.checkin import CheckinResponse
from app.models.trash_log import TrashClassificationResponse
from app.services.face_recognition_service import face_recognition_service
from app.services.trash_classification_service import trash_classification_service
from app.services.points_service import points_service
from app.services.csv_service import csv_service
from app.services.session_manager import session_manager
from app.utils.image_processing import save_image, base64_to_numpy
from app.config import settings

router = APIRouter(prefix="/esp32", tags=["ESP32"])

# Global variable to store latest bin control signal
# In production, use Redis or a database
latest_bin_control = {
    "action": "",
    "trash_type": "",
    "accepted": False,
    "timestamp": None
}


class FaceCheckinRequest(BaseModel):
    """Request model for face check-in."""
    image: str  # Base64 encoded image
    bin_id: str
    timestamp: str = None


class TrashClassifyRequest(BaseModel):
    """Request model for trash classification."""
    image: str  # Base64 encoded image
    checkin_id: str = None
    bin_id: str
    timestamp: str = None


class BinControlRequest(BaseModel):
    """Request model for bin control signal."""
    action: str  # "open" or "keep_closed"
    bin_id: str
    trash_type: str = None
    accepted: bool = False


@router.post("/face-checkin", response_model=CheckinResponse)
async def face_checkin(request: FaceCheckinRequest):
    """
    Face recognition check-in endpoint for ESP32.

    Recognizes student face and awards points.
    """
    try:
        # Set timestamp if not provided
        if not request.timestamp:
            request.timestamp = datetime.now().isoformat()

        # Recognize face
        recognition_result = await face_recognition_service.recognize_face(
            request.image
        )

        if not recognition_result:
            return CheckinResponse(
                status="not_registered",
                message="Khuôn mặt không được nhận diện. Vui lòng đăng ký trước!",
            )

        student_id = recognition_result['student_id']
        confidence_score = recognition_result['confidence_score']

        # Get student information
        student = await csv_service.get_student(student_id)

        if not student:
            return CheckinResponse(
                status="error",
                message="Student data not found"
            )

        # Get current total points (NO points awarded at check-in)
        total_points = await points_service.get_student_total_points(student_id)

        # If a session is already active, resume or wait — don't create duplicate check-ins
        active_session = await session_manager.get_active_session()
        if active_session:
            seconds_remaining = max(
                0,
                int((active_session.expires_at - datetime.now()).total_seconds())
            )
            if active_session.student_id == student_id:
                return CheckinResponse(
                    status="success",
                    student_id=student_id,
                    student_name=student['name'],
                    class_name=student['class'],
                    points_awarded=0,
                    total_points=total_points,
                    message=(
                        f"Tiếp tục phiên check-in của bạn. "
                        f"Còn {seconds_remaining}s để bỏ rác và nhận điểm."
                    ),
                    checkin_id=active_session.checkin_id,
                    confidence_score=confidence_score
                )

            return CheckinResponse(
                status="session_busy",
                student_id=student_id,
                student_name=student['name'],
                class_name=student['class'],
                message=(
                    f"Thùng rác đang được {active_session.student_name} sử dụng. "
                    f"Vui lòng đợi thêm {seconds_remaining} giây."
                ),
            )

        # Create check-in record with 0 points awarded
        checkin_data = {
            'student_id': student_id,
            'student_name': student['name'],
            'class': student['class'],
            'timestamp': request.timestamp,
            'points_awarded': 0,  # No points at check-in, only when trash is deposited
            'confidence_score': confidence_score
        }

        checkin_id = await csv_service.create_checkin(checkin_data)

        # Create session for trash deposit (60s timeout)
        try:
            session = await session_manager.create_session(
                checkin_id=checkin_id,
                student_id=student_id,
                student_name=student['name'],
                class_name=student['class']
            )
            session_info = session.to_dict()
            message = f"Chào mừng, {student['name']}! Bạn có {session_info['seconds_remaining']}s để bỏ rác và nhận điểm."
        except ValueError as e:
            return CheckinResponse(
                status="session_busy",
                student_id=student_id,
                student_name=student['name'],
                class_name=student['class'],
                checkin_id=checkin_id,
                message=f"Không thể bắt đầu phiên mới. {str(e)}"
            )

        return CheckinResponse(
            status="success",
            student_id=student_id,
            student_name=student['name'],
            class_name=student['class'],
            points_awarded=0,  # No points yet
            total_points=total_points,
            message=message,
            checkin_id=checkin_id,
            confidence_score=confidence_score
        )

    except ValueError as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=str(e)
        )
    except Exception as e:
        print(f"Error in face check-in: {str(e)}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Internal server error during face recognition"
        )


@router.post("/classify-trash", response_model=TrashClassificationResponse)
async def classify_trash(request: TrashClassifyRequest):
    """
    Trash classification endpoint for ESP32.

    Classifies trash type and returns accept/reject signal.
    """
    try:
        # Set timestamp if not provided
        if not request.timestamp:
            request.timestamp = datetime.now().isoformat()

        # Classify trash
        classification = await trash_classification_service.classify_trash(
            request.image
        )

        trash_type = classification['trash_type']
        confidence = classification['confidence']

        # Determine acceptance based on business rules
        accepted, message = trash_classification_service.should_accept(
            trash_type,
            confidence
        )

        # Get action for ESP32
        action = trash_classification_service.get_action(accepted)

        # Save trash image
        try:
            image_array = base64_to_numpy(request.image)
            timestamp_str = datetime.now().strftime('%Y%m%d_%H%M%S')
            image_filename = f"trash_{timestamp_str}.jpg"
            image_path = settings.TRASH_IMAGES_DIR / image_filename
            save_image(image_array, str(image_path))
            image_path_str = f"trash_images/{image_filename}"
        except Exception as e:
            print(f"Error saving trash image: {str(e)}")
            image_path_str = ""

        # Validate session and award points
        student_id = None
        student_name = None
        points_awarded = 0
        total_points = None
        session_valid = False

        if request.checkin_id:
            # Validate session exists and not expired
            session = await session_manager.get_session(request.checkin_id)

            if not session:
                # Session expired or not found
                return TrashClassificationResponse(
                    status="session_expired",
                    trash_type=trash_type,
                    confidence=confidence,
                    accepted=False,
                    action="keep_closed",
                    message="Phiên làm việc đã hết hạn! Vui lòng check-in lại.",
                    log_id="",
                    points_awarded=0,
                    total_points=None
                )

            session_valid = True
            student_id = session.student_id
            student_name = session.student_name

            # Award points if trash is accepted
            if accepted:
                points_info = await points_service.calculate_trash_points(
                    student_id,
                    trash_type
                )
                points_awarded = points_info['points_awarded']
                total_points = points_info['total_points']

                # Update checkin record with points awarded
                await csv_service.update_checkin_points(request.checkin_id, points_awarded)

                # Update message with points info
                if points_awarded > 0:
                    message = f"{message} Bạn nhận được +{points_awarded} điểm!"

            # Complete session (whether accepted or rejected)
            await session_manager.complete_session(request.checkin_id)

        # Create trash log
        log_data = {
            'checkin_id': request.checkin_id if request.checkin_id else '',
            'student_id': student_id if student_id else '',
            'student_name': student_name if student_name else '',
            'timestamp': request.timestamp,
            'trash_type': trash_type,
            'confidence_score': confidence,
            'accepted': accepted,
            'image_path': image_path_str
        }

        log_id = await csv_service.create_trash_log(log_data)

        return TrashClassificationResponse(
            status="success",
            trash_type=trash_type,
            confidence=confidence,
            accepted=accepted,
            action=action,
            message=message,
            log_id=log_id,
            points_awarded=points_awarded,
            total_points=total_points
        )

    except ValueError as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=str(e)
        )
    except Exception as e:
        print(f"Error in trash classification: {str(e)}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Internal server error during trash classification"
        )


@router.post("/bin-control")
async def bin_control(request: BinControlRequest):
    """
    Bin control endpoint to send open/close signal to ESP32.

    This endpoint receives control signals from the web interface
    and stores them for ESP32 to poll.
    """
    try:
        global latest_bin_control

        # Store the control signal
        latest_bin_control = {
            "action": request.action,
            "trash_type": request.trash_type,
            "accepted": request.accepted,
            "timestamp": datetime.now().isoformat()
        }

        print(f"[BIN CONTROL] Signal stored: {request.action} for bin {request.bin_id}, "
              f"Trash Type: {request.trash_type}, Accepted: {request.accepted}")

        return {
            "status": "success",
            "action": request.action,
            "bin_id": request.bin_id,
            "message": f"Signal '{request.action}' stored for bin {request.bin_id}"
        }

    except Exception as e:
        print(f"Error in bin control: {str(e)}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Internal server error during bin control"
        )


@router.get("/bin-status")
async def get_bin_status(bin_id: str):
    """
    Bin status polling endpoint for ESP32.

    ESP32 calls this endpoint periodically to check for control signals.
    Returns the latest signal and clears it after reading.
    """
    try:
        global latest_bin_control

        # Get the current signal
        signal = latest_bin_control.copy()

        # Clear the signal after reading (one-time delivery)
        if signal["action"] != "":
            print(f"[BIN STATUS] ESP32 {bin_id} retrieved signal: {signal['action']}")
            latest_bin_control["action"] = ""  # Clear after delivery

        return signal

    except Exception as e:
        print(f"Error getting bin status: {str(e)}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Internal server error getting bin status"
        )
