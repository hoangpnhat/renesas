"""
Explore agent_scores.csv for clustering
Focus on: id, criteria_name, criteria_score
"""

import pandas as pd
import json

print("="*70)
print("EXPLORING agent_scores.csv")
print("="*70)

# Read the CSV file
df = pd.read_csv('agent_scores.csv')

print(f"\nShape: {df.shape} (rows: {df.shape[0]}, columns: {df.shape[1]})")
print(f"\nColumns: {list(df.columns)}")

print(f"\nFirst 10 rows:")
print(df.head(10))

print(f"\nData types:")
print(df.dtypes)

print(f"\nMissing values:")
print(df.isnull().sum())

print("\n" + "="*70)
print("DATA ANALYSIS")
print("="*70)

# Basic statistics
print(f"\nTotal rows: {len(df)}")
print(f"Unique meeting IDs (candidates): {df['meeting_id'].nunique()}")
print(f"Unique criteria/skills: {df['criteria_name'].nunique()}")
print(f"Unique roles: {df['role_name'].nunique()}")

print(f"\nScore statistics:")
print(df['criteria_score'].describe())

print(f"\nAll unique criteria/skills ({df['criteria_name'].nunique()}):")
for i, skill in enumerate(sorted(df['criteria_name'].unique()), 1):
    count = len(df[df['criteria_name'] == skill])
    print(f"  {i:2d}. {skill:50s} ({count} evaluations)")

# Analyze candidate-level data
print("\n" + "="*70)
print("CANDIDATE-LEVEL ANALYSIS")
print("="*70)

# Group by candidate to see distribution
candidate_counts = df.groupby('meeting_id').size()
print(f"\nSkills per candidate:")
print(f"  - Mean: {candidate_counts.mean():.1f}")
print(f"  - Median: {candidate_counts.median():.1f}")
print(f"  - Min: {candidate_counts.min()}")
print(f"  - Max: {candidate_counts.max()}")

# Sample candidates
print(f"\nSample candidates:")
for i, candidate_id in enumerate(df['meeting_id'].unique()[:5], 1):
    candidate_data = df[df['meeting_id'] == candidate_id]
    print(f"\n  {i}. Candidate: {candidate_id}")
    print(f"     - Skills: {len(candidate_data)}")
    print(f"     - Avg score: {candidate_data['criteria_score'].mean():.2f}")
    print(f"     - Skills: {candidate_data['criteria_name'].tolist()[:5]}...")
    print(f"     - Scores: {dict(zip(candidate_data['criteria_name'].tolist()[:3], candidate_data['criteria_score'].tolist()[:3]))}")

print("\n" + "="*70)
print("COMPLETE!")
print("="*70)
