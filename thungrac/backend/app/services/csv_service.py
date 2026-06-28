"""CSV database service for managing data persistence."""
import asyncio
import csv
from datetime import datetime
from pathlib import Path
from typing import List, Optional, Dict
import pandas as pd

from app.config import settings


def _sanitize_value(value):
    """Convert pandas NaN/None to JSON-safe values."""
    if value is None or (isinstance(value, float) and pd.isna(value)):
        return ""
    if isinstance(value, (pd.Timestamp, datetime)):
        return value.isoformat()
    return value


def _sanitize_record(record: Dict) -> Dict:
    """Sanitize a single CSV row dict for API/JSON use."""
    return {key: _sanitize_value(val) for key, val in record.items()}


class CSVService:
    """Service for managing CSV database operations."""

    def __init__(self):
        """Initialize CSV service with file locks."""
        self.students_file = settings.STUDENTS_CSV
        self.checkins_file = settings.CHECKINS_CSV
        self.trash_logs_file = settings.TRASH_LOGS_CSV
        self._students_lock = asyncio.Lock()
        self._checkins_lock = asyncio.Lock()
        self._trash_logs_lock = asyncio.Lock()

    # Student Operations
    async def create_student(self, student_data: Dict) -> str:
        """Create a new student entry."""
        async with self._students_lock:
            # Generate student ID
            df = pd.read_csv(self.students_file)
            student_id = f"STU{len(df) + 1:04d}"

            # Add student
            new_row = {
                'student_id': student_id,
                'name': student_data['name'],
                'class': student_data['class'],
                'registration_date': datetime.now().strftime('%Y-%m-%d'),
                'total_points': 0,
                'face_encoding_path': f"face_encodings/{student_id}.npy",
                'status': 'active'
            }

            df = pd.concat([df, pd.DataFrame([new_row])], ignore_index=True)
            df.to_csv(self.students_file, index=False)

            return student_id

    async def get_student(self, student_id: str) -> Optional[Dict]:
        """Get student by ID."""
        async with self._students_lock:
            df = pd.read_csv(self.students_file)
            student = df[df['student_id'] == student_id]

            if student.empty:
                return None

            return _sanitize_record(student.iloc[0].to_dict())

    async def get_all_students(self, class_filter: Optional[str] = None) -> List[Dict]:
        """Get all students, optionally filtered by class."""
        async with self._students_lock:
            df = pd.read_csv(self.students_file)

            if class_filter:
                df = df[df['class'] == class_filter]

            return [_sanitize_record(r) for r in df.to_dict('records')]

    async def update_student_points(self, student_id: str, points_to_add: int) -> bool:
        """Add points to a student's total."""
        async with self._students_lock:
            df = pd.read_csv(self.students_file)

            if student_id not in df['student_id'].values:
                return False

            df.loc[df['student_id'] == student_id, 'total_points'] += points_to_add
            df.to_csv(self.students_file, index=False)

            return True

    async def get_student_by_encoding_path(self, encoding_path: str) -> Optional[Dict]:
        """Get student by face encoding path."""
        async with self._students_lock:
            df = pd.read_csv(self.students_file)
            student = df[df['face_encoding_path'] == encoding_path]

            if student.empty:
                return None

            return _sanitize_record(student.iloc[0].to_dict())

    # Check-in Operations
    async def create_checkin(self, checkin_data: Dict) -> str:
        """Create a new check-in entry."""
        async with self._checkins_lock:
            df = pd.read_csv(self.checkins_file)
            checkin_id = f"CHK{len(df) + 1:04d}"

            new_row = {
                'checkin_id': checkin_id,
                'student_id': checkin_data['student_id'],
                'student_name': str(checkin_data.get('student_name') or '').strip(),
                'class': checkin_data['class'],
                'timestamp': checkin_data['timestamp'],
                'points_awarded': checkin_data['points_awarded'],
                'confidence_score': checkin_data['confidence_score']
            }

            df = pd.concat([df, pd.DataFrame([new_row])], ignore_index=True)
            df.to_csv(self.checkins_file, index=False)

            return checkin_id

    async def get_recent_checkins(self, limit: int = 50) -> List[Dict]:
        """Get recent check-ins."""
        async with self._checkins_lock:
            df = pd.read_csv(self.checkins_file)

            if df.empty:
                return []

            # Sort by timestamp descending and get last N
            df = df.sort_values('timestamp', ascending=False).head(limit)

            return [_sanitize_record(r) for r in df.to_dict('records')]

    async def get_checkins_by_student(self, student_id: str) -> List[Dict]:
        """Get all check-ins for a specific student."""
        async with self._checkins_lock:
            df = pd.read_csv(self.checkins_file)

            student_checkins = df[df['student_id'] == student_id]

            return [_sanitize_record(r) for r in student_checkins.to_dict('records')]

    async def get_checkin(self, checkin_id: str) -> Optional[Dict]:
        """Get a specific check-in by ID."""
        async with self._checkins_lock:
            df = pd.read_csv(self.checkins_file)

            checkin = df[df['checkin_id'] == checkin_id]

            if checkin.empty:
                return None

            return _sanitize_record(checkin.iloc[0].to_dict())

    async def update_checkin_points(self, checkin_id: str, points: int) -> bool:
        """Update points awarded in a check-in record."""
        async with self._checkins_lock:
            df = pd.read_csv(self.checkins_file)

            if checkin_id not in df['checkin_id'].values:
                return False

            df.loc[df['checkin_id'] == checkin_id, 'points_awarded'] = points
            df.to_csv(self.checkins_file, index=False)

            return True

    # Trash Log Operations
    async def create_trash_log(self, log_data: Dict) -> str:
        """Create a new trash log entry."""
        async with self._trash_logs_lock:
            df = pd.read_csv(self.trash_logs_file)
            if 'student_name' not in df.columns:
                df['student_name'] = ''
            log_id = f"TRH{len(df) + 1:04d}"

            new_row = {
                'log_id': log_id,
                'checkin_id': log_data.get('checkin_id', ''),
                'student_id': log_data.get('student_id', ''),
                'student_name': str(log_data.get('student_name') or '').strip(),
                'timestamp': log_data['timestamp'],
                'trash_type': log_data['trash_type'],
                'confidence_score': log_data['confidence_score'],
                'accepted': log_data['accepted'],
                'image_path': log_data['image_path']
            }

            df = pd.concat([df, pd.DataFrame([new_row])], ignore_index=True)
            df.to_csv(self.trash_logs_file, index=False)

            return log_id

    async def get_trash_stats(
        self,
        start_date: Optional[str] = None,
        end_date: Optional[str] = None
    ) -> Dict:
        """Get trash statistics."""
        async with self._trash_logs_lock:
            df = pd.read_csv(self.trash_logs_file)

            if df.empty:
                return {
                    'total_items': 0,
                    'breakdown': {},
                    'percentages': {},
                    'acceptance_rate': 0.0
                }

            # Filter by date if provided
            if start_date:
                df = df[df['timestamp'] >= start_date]
            if end_date:
                df = df[df['timestamp'] <= end_date]

            total = len(df)
            breakdown = df['trash_type'].value_counts().to_dict()
            percentages = {k: (v / total * 100) for k, v in breakdown.items()}
            acceptance_rate = (df['accepted'].sum() / total * 100) if total > 0 else 0.0

            return {
                'total_items': total,
                'breakdown': breakdown,
                'percentages': percentages,
                'acceptance_rate': round(acceptance_rate, 2)
            }

    # Statistics Operations
    async def get_student_rankings(
        self,
        class_filter: Optional[str] = None,
        limit: int = 10
    ) -> List[Dict]:
        """Get top students by points."""
        async with self._students_lock:
            df = pd.read_csv(self.students_file)

            if class_filter:
                df = df[df['class'] == class_filter]

            # Sort by points descending
            df = df.sort_values('total_points', ascending=False).head(limit)

            # Add rank
            rankings = df.to_dict('records')
            for i, student in enumerate(rankings, 1):
                student['rank'] = i

            return [_sanitize_record(r) for r in rankings]

    async def get_class_rankings(self) -> List[Dict]:
        """Get class rankings by total points."""
        async with self._students_lock:
            df = pd.read_csv(self.students_file)

            if df.empty:
                return []

            # Group by class
            class_stats = df.groupby('class').agg({
                'total_points': 'sum',
                'student_id': 'count'
            }).reset_index()

            class_stats.columns = ['class', 'total_points', 'student_count']
            class_stats['avg_points_per_student'] = (
                class_stats['total_points'] / class_stats['student_count']
            ).round(2)

            # Sort by total points
            class_stats = class_stats.sort_values('total_points', ascending=False)

            # Add rank
            rankings = class_stats.to_dict('records')
            for i, cls in enumerate(rankings, 1):
                cls['rank'] = i

            return [_sanitize_record(r) for r in rankings]

    async def get_dashboard_summary(self) -> Dict:
        """Get dashboard summary statistics."""
        today = datetime.now().strftime('%Y-%m-%d')

        # Get students count
        students_df = pd.read_csv(self.students_file)
        total_students = len(students_df)

        # Get check-ins
        checkins_df = pd.read_csv(self.checkins_file)
        checkins_today = len(checkins_df[checkins_df['timestamp'].str.startswith(today)])

        # Get trash stats
        trash_df = pd.read_csv(self.trash_logs_file)
        trash_today = len(trash_df[trash_df['timestamp'].str.startswith(today)])

        # Top student
        top_student = None
        if not students_df.empty:
            top = students_df.loc[students_df['total_points'].idxmax()]
            top_student = {
                'name': top['name'],
                'points': int(top['total_points'])
            }

        # Top class
        top_class = None
        if not students_df.empty:
            class_totals = students_df.groupby('class')['total_points'].sum()
            top_class_name = class_totals.idxmax()
            top_class = {
                'name': top_class_name,
                'points': int(class_totals[top_class_name])
            }

        # Trash distribution (6 TrashNet classes)
        trash_distribution = {}
        if not trash_df.empty:
            total = len(trash_df)
            for trash_type in ['cardboard', 'glass', 'metal', 'paper', 'plastic', 'trash']:
                count = len(trash_df[trash_df['trash_type'] == trash_type])
                trash_distribution[trash_type] = round(count / total * 100, 2)

        return {
            'summary': {
                'total_students': total_students,
                'total_checkins_today': checkins_today,
                'total_trash_items_today': trash_today,
                'top_student': top_student,
                'top_class': top_class
            },
            'trash_distribution': trash_distribution
        }


# Global CSV service instance
csv_service = CSVService()
