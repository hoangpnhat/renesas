"""
Data loading script - loads CSV data into Django database
"""
import os
import sys
import django
from pathlib import Path
import pandas as pd
from datetime import datetime

# Setup Django environment
BASE_DIR = Path(__file__).resolve().parent.parent
sys.path.append(str(BASE_DIR))
os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'config.settings')
django.setup()

from core.models import Project


def parse_date(date_str):
    """Parse date string from CSV, return None if invalid"""
    if pd.isna(date_str) or not date_str:
        return None

    try:
        # Try different date formats
        for fmt in ['%d-%m-%Y', '%Y-%m-%d', '%m/%d/%Y']:
            try:
                return datetime.strptime(date_str, fmt).date()
            except ValueError:
                continue
        return None
    except Exception:
        return None


def load_projects_from_csv(csv_path):
    """Load property projects from CSV into database"""

    print("=" * 80)
    print("LOADING PROPERTY DATA INTO DATABASE")
    print("=" * 80)

    # Read CSV
    print(f"\nReading CSV from: {csv_path}")
    df = pd.read_csv(csv_path)
    print(f"Found {len(df)} records in CSV")

    # Clean and prepare data
    print("\nCleaning data...")

    # Replace NaN with None
    df = df.where(pd.notna(df), None)

    # Filter out records with missing critical fields
    original_count = len(df)
    df = df[df['city'].notna() & df['Price (USD)'].notna()]
    filtered_count = original_count - len(df)
    if filtered_count > 0:
        print(f"Filtered out {filtered_count} records with missing critical fields (city, price)")

    print(f"Proceeding with {len(df)} valid records")

    # Clear existing data
    print("\nClearing existing projects...")
    Project.objects.all().delete()
    print("Cleared existing data")

    # Bulk create projects
    print("\nInserting projects into database...")
    projects = []
    skipped = 0

    for idx, row in df.iterrows():
        try:
            # Parse date
            completion_date = parse_date(row.get('completion_date'))

            # Handle numeric fields
            no_of_bedrooms = int(row['No of bedrooms']) if pd.notna(row.get('No of bedrooms')) else None
            bathrooms = int(row['bathrooms']) if pd.notna(row.get('bathrooms')) else None
            price_usd = int(row['Price (USD)'])
            area_sq_mtrs = int(row['Area (sq mtrs)'])

            # Parse JSON fields (features, facilities)
            # These are stored as strings in CSV, need to handle them
            features = []
            facilities = []

            # Handle project_description - ensure it's never None
            description = row.get('Project description')
            if pd.isna(description) or description is None:
                description = ''

            project = Project(
                project_name=row.get('Project name'),
                no_of_bedrooms=no_of_bedrooms,
                completion_status=row.get('Completion status (off plan/available)'),
                bathrooms=bathrooms,
                unit_type=row.get('unit type'),
                developer_name=row.get('developer name'),
                price_usd=price_usd,
                area_sq_mtrs=area_sq_mtrs,
                property_type=row.get('Property type (apartment/villa)'),
                city=row['city'],
                country=row.get('country'),
                completion_date=completion_date,
                features=features,
                facilities=facilities,
                project_description=description,
            )
            projects.append(project)

            if len(projects) % 100 == 0:
                print(f"Prepared {len(projects)} projects...")

        except Exception as e:
            skipped += 1
            print(f"Skipped row {idx}: {str(e)}")

    # Bulk insert
    print(f"\nInserting {len(projects)} projects...")
    Project.objects.bulk_create(projects, batch_size=100)

    print(f"\n{'='*80}")
    print(f"DATA LOADING COMPLETE")
    print(f"{'='*80}")
    print(f"Successfully loaded: {len(projects)} projects")
    print(f"Skipped: {skipped} projects")
    from django.db.models import Min, Max

    print(f"\nDatabase statistics:")
    print(f"  Total projects: {Project.objects.count()}")
    print(f"  Cities: {Project.objects.values('city').distinct().count()}")
    print(f"  Countries: {Project.objects.values('country').distinct().count()}")

    price_range = Project.objects.aggregate(min_price=Min('price_usd'), max_price=Max('price_usd'))
    print(f"  Price range: ${price_range['min_price']:,} - ${price_range['max_price']:,}")

    print(f"\nSample projects:")
    for project in Project.objects.all()[:5]:
        print(f"  - {project}")


if __name__ == '__main__':
    csv_path = BASE_DIR / 'Property sales agent - Challenge.csv'

    if not csv_path.exists():
        print(f"ERROR: CSV file not found at {csv_path}")
        sys.exit(1)

    load_projects_from_csv(csv_path)
