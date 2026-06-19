"""Data models for the application."""
from .student import Student, StudentCreate
from .checkin import Checkin, CheckinCreate
from .trash_log import TrashLog, TrashLogCreate

__all__ = [
    "Student",
    "StudentCreate",
    "Checkin",
    "CheckinCreate",
    "TrashLog",
    "TrashLogCreate",
]
