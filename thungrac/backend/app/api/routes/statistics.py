"""API routes for statistics and analytics."""
from fastapi import APIRouter, Query
from typing import Optional, List, Dict
from pydantic import BaseModel

from app.services.csv_service import csv_service
from app.services.points_service import points_service

router = APIRouter(prefix="/statistics", tags=["Statistics"])


class StudentRanking(BaseModel):
    """Student ranking model."""
    rank: int
    student_id: str
    name: str
    class_name: str = None
    total_points: int

    class Config:
        populate_by_name = True


class ClassRanking(BaseModel):
    """Class ranking model."""
    rank: int
    class_name: str
    total_points: int
    student_count: int
    avg_points_per_student: float


class TrashBreakdown(BaseModel):
    """Trash statistics breakdown model."""
    total_items: int
    breakdown: Dict[str, int]
    percentages: Dict[str, float]
    acceptance_rate: float


class RecentCheckin(BaseModel):
    """Recent check-in model."""
    checkin_id: str
    student_name: str
    class_name: str
    timestamp: str
    points_awarded: int
    confidence_score: float


class DashboardSummary(BaseModel):
    """Dashboard summary model."""
    summary: Dict
    trash_distribution: Dict[str, float]


@router.get("/student-rankings")
async def get_student_rankings(
    class_filter: Optional[str] = Query(None, alias="class"),
    limit: int = Query(10, ge=1, le=100)
) -> List[Dict]:
    """
    Get student rankings by points.

    Args:
        class_filter: Filter by class (optional)
        limit: Number of students to return

    Returns:
        List of top students
    """
    rankings = await points_service.get_leaderboard(
        class_filter=class_filter,
        limit=limit
    )

    return rankings


@router.get("/class-rankings")
async def get_class_rankings() -> List[Dict]:
    """
    Get class rankings by total points.

    Returns:
        List of classes ranked by points
    """
    rankings = await points_service.get_class_leaderboard()

    return rankings


@router.get("/trash-breakdown")
async def get_trash_breakdown(
    start_date: Optional[str] = None,
    end_date: Optional[str] = None
) -> Dict:
    """
    Get trash type statistics.

    Args:
        start_date: Start date filter (ISO format)
        end_date: End date filter (ISO format)

    Returns:
        Trash breakdown statistics
    """
    stats = await csv_service.get_trash_stats(
        start_date=start_date,
        end_date=end_date
    )

    return stats


@router.get("/recent-checkins")
async def get_recent_checkins(
    limit: int = Query(50, ge=1, le=200)
) -> List[Dict]:
    """
    Get recent check-in activity.

    Args:
        limit: Number of check-ins to return

    Returns:
        List of recent check-ins
    """
    checkins = await csv_service.get_recent_checkins(limit=limit)

    return checkins


@router.get("/dashboard")
async def get_dashboard() -> Dict:
    """
    Get dashboard summary statistics.

    Returns:
        Dashboard summary with key metrics
    """
    dashboard_data = await csv_service.get_dashboard_summary()

    return dashboard_data


@router.get("/recent-trash")
async def get_recent_trash(limit: int = Query(default=1, ge=1, le=10)) -> Dict:
    """
    Get recent trash classification logs with images.

    Args:
        limit: Number of trash logs to return (default 1)

    Returns:
        Recent trash logs with image paths
    """
    import pandas as pd
    from pathlib import Path

    trash_logs_file = csv_service.trash_logs_file

    if not trash_logs_file.exists():
        return {"trash_logs": []}

    # Read trash logs
    df = pd.read_csv(trash_logs_file)

    # Sort by timestamp descending and get recent logs
    df = df.sort_values('timestamp', ascending=False).head(limit)

    # Convert to list of dicts
    trash_logs = df.to_dict('records')

    # Add full image URL
    for log in trash_logs:
        if log.get('image_path'):
            log['image_url'] = f"/data/{log['image_path']}"
        else:
            log['image_url'] = None

    return {"trash_logs": trash_logs}
