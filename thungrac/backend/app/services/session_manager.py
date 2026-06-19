"""Session management for tracking check-in to trash deposit flow."""
from datetime import datetime, timedelta
from typing import Optional, Dict
from enum import Enum
import asyncio

from app.config import settings


class SessionState(Enum):
    """Session states."""
    IDLE = "idle"
    CHECKED_IN = "checked_in"
    WAITING_TRASH = "waiting_trash"
    COMPLETED = "completed"
    EXPIRED = "expired"


class Session:
    """Represents a check-in session."""

    def __init__(
        self,
        checkin_id: str,
        student_id: str,
        student_name: str,
        class_name: str
    ):
        """
        Initialize a new session.

        Args:
            checkin_id: Unique check-in ID
            student_id: Student ID
            student_name: Student name
            class_name: Class name
        """
        self.checkin_id = checkin_id
        self.student_id = student_id
        self.student_name = student_name
        self.class_name = class_name
        self.state = SessionState.CHECKED_IN
        self.created_at = datetime.now()
        self.expires_at = self.created_at + timedelta(
            seconds=settings.SESSION_TIMEOUT_SECONDS
        )
        self.completed_at: Optional[datetime] = None

    def is_expired(self) -> bool:
        """Check if session has expired."""
        if self.state in [SessionState.COMPLETED, SessionState.EXPIRED]:
            return False
        return datetime.now() > self.expires_at

    def mark_waiting_trash(self):
        """Mark session as waiting for trash."""
        self.state = SessionState.WAITING_TRASH

    def mark_completed(self):
        """Mark session as completed."""
        self.state = SessionState.COMPLETED
        self.completed_at = datetime.now()

    def mark_expired(self):
        """Mark session as expired."""
        self.state = SessionState.EXPIRED
        self.completed_at = datetime.now()

    def to_dict(self) -> Dict:
        """Convert session to dictionary."""
        return {
            'checkin_id': self.checkin_id,
            'student_id': self.student_id,
            'student_name': self.student_name,
            'class_name': self.class_name,
            'state': self.state.value,
            'created_at': self.created_at.isoformat(),
            'expires_at': self.expires_at.isoformat(),
            'completed_at': self.completed_at.isoformat() if self.completed_at else None,
            'seconds_remaining': max(0, int((self.expires_at - datetime.now()).total_seconds()))
        }


class SessionManager:
    """
    Manages active check-in sessions.

    Ensures only one active session at a time and handles timeouts.
    """

    def __init__(self):
        """Initialize session manager."""
        self.active_session: Optional[Session] = None
        self._cleanup_task: Optional[asyncio.Task] = None
        self._lock = asyncio.Lock()

    async def create_session(
        self,
        checkin_id: str,
        student_id: str,
        student_name: str,
        class_name: str
    ) -> Session:
        """
        Create a new session.

        Args:
            checkin_id: Check-in ID
            student_id: Student ID
            student_name: Student name
            class_name: Class name

        Returns:
            Created session

        Raises:
            ValueError: If there's already an active session
        """
        async with self._lock:
            # Clean up expired session if exists
            if self.active_session and self.active_session.is_expired():
                self.active_session.mark_expired()
                print(f"[Session] Auto-expired session {self.active_session.checkin_id}")
                self.active_session = None

            # Check for active session
            if self.active_session:
                raise ValueError(
                    f"Session already active for {self.active_session.student_name}. "
                    f"Please wait {self.active_session.to_dict()['seconds_remaining']} seconds."
                )

            # Create new session
            session = Session(checkin_id, student_id, student_name, class_name)
            self.active_session = session

            print(f"[Session] Created for {student_name} (ID: {checkin_id})")
            print(f"[Session] Expires at {session.expires_at.strftime('%H:%M:%S')}")

            return session

    async def get_session(self, checkin_id: str) -> Optional[Session]:
        """
        Get session by check-in ID.

        Args:
            checkin_id: Check-in ID

        Returns:
            Session if found and valid, None otherwise
        """
        async with self._lock:
            if not self.active_session:
                return None

            if self.active_session.checkin_id != checkin_id:
                return None

            # Check if expired
            if self.active_session.is_expired():
                self.active_session.mark_expired()
                print(f"[Session] Session {checkin_id} has expired")
                self.active_session = None
                return None

            return self.active_session

    async def complete_session(self, checkin_id: str) -> bool:
        """
        Mark session as completed.

        Args:
            checkin_id: Check-in ID

        Returns:
            True if session was completed, False if not found
        """
        async with self._lock:
            if not self.active_session or self.active_session.checkin_id != checkin_id:
                return False

            self.active_session.mark_completed()
            print(f"[Session] Completed for {self.active_session.student_name}")

            # Clear active session
            self.active_session = None
            return True

    async def cancel_session(self, checkin_id: str) -> bool:
        """
        Cancel/expire session manually.

        Args:
            checkin_id: Check-in ID

        Returns:
            True if session was cancelled, False if not found
        """
        async with self._lock:
            if not self.active_session or self.active_session.checkin_id != checkin_id:
                return False

            self.active_session.mark_expired()
            print(f"[Session] Cancelled for {self.active_session.student_name}")

            # Clear active session
            self.active_session = None
            return True

    def get_active_session(self) -> Optional[Session]:
        """
        Get current active session (non-async, for read-only access).

        Returns:
            Active session if exists and not expired, None otherwise
        """
        if not self.active_session:
            return None

        if self.active_session.is_expired():
            return None

        return self.active_session

    async def cleanup_expired_sessions(self):
        """Background task to clean up expired sessions."""
        while True:
            try:
                await asyncio.sleep(settings.SESSION_CLEANUP_INTERVAL_SECONDS)

                async with self._lock:
                    if self.active_session and self.active_session.is_expired():
                        student_name = self.active_session.student_name
                        self.active_session.mark_expired()
                        self.active_session = None
                        print(f"[Session] Auto-cleaned expired session for {student_name}")

            except Exception as e:
                print(f"[Session] Error in cleanup task: {str(e)}")

    def start_cleanup_task(self):
        """Start background cleanup task."""
        if not self._cleanup_task or self._cleanup_task.done():
            self._cleanup_task = asyncio.create_task(self.cleanup_expired_sessions())
            print("[Session] Cleanup task started")

    def stop_cleanup_task(self):
        """Stop background cleanup task."""
        if self._cleanup_task and not self._cleanup_task.done():
            self._cleanup_task.cancel()
            print("[Session] Cleanup task stopped")


# Global session manager instance
session_manager = SessionManager()
