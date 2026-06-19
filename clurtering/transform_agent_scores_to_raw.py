"""
Transform agent_scores.csv to candidates_raw.json

Input: agent_scores.csv (30,422 rows, multiple evaluators per candidate-skill pair)
Output: candidates_raw.json (158 candidates, candidate-level format)

Process:
1. Load agent_scores.csv
2. Aggregate scores per candidate per skill (average across evaluators)
3. Transform to candidate-level format with skills and skill_scores
"""

import pandas as pd
import json
from pathlib import Path

print("="*70)
print("TRANSFORM: agent_scores.csv -> candidates_raw.json")
print("="*70)

# Create output directory if needed
Path('data/processed').mkdir(parents=True, exist_ok=True)

# Load agent_scores.csv
print("\n1. Loading agent_scores.csv...")
df = pd.read_csv('agent_scores.csv')

print(f"   - Total rows: {len(df)}")
print(f"   - Columns: {list(df.columns)}")
print(f"   - Unique candidates (meeting_id): {df['meeting_id'].nunique()}")
print(f"   - Unique skills (criteria_name): {df['criteria_name'].nunique()}")

# Aggregate scores per candidate per skill (average across evaluators)
print("\n2. Aggregating scores per candidate per skill...")
print("   (Taking average across multiple evaluators)")

candidate_skills = df.groupby(['meeting_id', 'criteria_name']).agg({
    'criteria_score': 'mean'
}).reset_index()

print(f"   [OK] After aggregation: {len(candidate_skills)} candidate-skill pairs")

# Transform to candidate-level format
print("\n3. Transforming to candidate-level format...")

candidates_raw = []

for candidate_id in candidate_skills['meeting_id'].unique():
    # Get all skills for this candidate
    candidate_data = candidate_skills[candidate_skills['meeting_id'] == candidate_id]

    # Extract skills and scores
    skills = candidate_data['criteria_name'].tolist()
    skill_scores = dict(zip(
        candidate_data['criteria_name'],
        candidate_data['criteria_score']
    ))

    candidates_raw.append({
        'candidate_id': candidate_id,
        'skills': skills,
        'skill_scores': skill_scores
    })

print(f"   [OK] Created {len(candidates_raw)} candidate profiles")

# Calculate statistics
total_skills = sum(len(c['skills']) for c in candidates_raw)
avg_skills = total_skills / len(candidates_raw)
unique_skills = len(set(skill for c in candidates_raw for skill in c['skills']))

print(f"\n   Statistics:")
print(f"   - Total candidates: {len(candidates_raw)}")
print(f"   - Unique skills: {unique_skills}")
print(f"   - Avg skills per candidate: {avg_skills:.1f}")

# Show sample
print(f"\n   Sample candidate:")
sample = candidates_raw[0]
print(f"   - Candidate ID: {sample['candidate_id']}")
print(f"   - Number of skills: {len(sample['skills'])}")
print(f"   - Sample skills: {sample['skills'][:5]}")
print(f"   - Sample scores: {dict(list(sample['skill_scores'].items())[:3])}")

# Save to JSON
print("\n4. Saving to candidates_raw.json...")

with open('data/processed/candidates_raw.json', 'w', encoding='utf-8') as f:
    json.dump(candidates_raw, f, indent=2, ensure_ascii=False)

print(f"   [OK] Saved to data/processed/candidates_raw.json")

# Summary
print("\n" + "="*70)
print("TRANSFORMATION COMPLETE")
print("="*70)

print(f"\nInput:  agent_scores.csv ({len(df)} rows)")
print(f"Output: data/processed/candidates_raw.json ({len(candidates_raw)} candidates)")
print(f"\nReady for Phase 1: Skill Normalization")
print("="*70)
