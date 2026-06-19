"""Points calculation service."""
from typing import Dict
from datetime import datetime

from app.config import settings
from app.services.csv_service import csv_service


class PointsService:
    """Service for managing points calculations."""

    def __init__(self):
        """Initialize points service."""
        self.points_config = {
            'base_checkin': settings.POINTS_BASE_CHECKIN,
            'cardboard': settings.POINTS_CARDBOARD,
            'glass': settings.POINTS_GLASS,
            'metal': settings.POINTS_METAL,
            'paper': settings.POINTS_PAPER,
            'plastic': settings.POINTS_PLASTIC,
            'trash': settings.POINTS_TRASH,
        }

    async def calculate_trash_points(
        self,
        student_id: str,
        trash_type: str
    ) -> Dict[str, int]:
        """
        Calculate points for trash disposal (NEW LOGIC: only award points for trash, not check-in).

        Args:
            student_id: Student ID
            trash_type: Type of trash

        Returns:
            Dict with points_awarded and new total_points
        """
        # Get points based on trash type only (no base_checkin)
        points = self.points_config.get(trash_type, 0)

        # Update student's total points
        if points > 0:
            await csv_service.update_student_points(student_id, points)

        # Get updated student info
        student = await csv_service.get_student(student_id)
        total_points = student['total_points'] if student else 0

        return {
            'points_awarded': points,
            'total_points': int(total_points)
        }

    async def get_student_total_points(self, student_id: str) -> int:
        """
        Get student's current total points without awarding new points.

        Args:
            student_id: Student ID

        Returns:
            Current total points
        """
        student = await csv_service.get_student(student_id)
        return int(student['total_points']) if student else 0

    async def get_leaderboard(
        self,
        class_filter: str = None,
        limit: int = 10
    ) -> list:
        """
        Get leaderboard of top students.

        Args:
            class_filter: Filter by class (optional)
            limit: Number of top students to return

        Returns:
            List of top students with rankings
        """
        return await csv_service.get_student_rankings(
            class_filter=class_filter,
            limit=limit
        )

    async def get_class_leaderboard(self) -> list:
        """
        Get leaderboard of classes.

        Returns:
            List of classes ranked by total points
        """
        return await csv_service.get_class_rankings()

    def get_points_breakdown(self) -> Dict:
        """
        Get the current points configuration.

        Returns:
            Dict with points values for each action
        """
        return self.points_config.copy()


# Global service instance
points_service = PointsService()
