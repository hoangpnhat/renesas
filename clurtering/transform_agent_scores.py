"""
Transform agent_scores.csv to candidate-level format for clustering

Input: agent_scores.csv with columns:
  - meeting_id: candidate identifier
  - criteria_name: skill name (noisy!)
  - criteria_score: score for that skill

Output: candidates in format:
  {
    candidate_id: str,
    skills: [skill1, skill2, ...],
    skill_scores: {skill1: score1, skill2: score2, ...}
  }
"""

import pandas as pd
import json
import os

print("="*70)
print("TRANSFORMING agent_scores.csv TO CANDIDATE-LEVEL FORMAT")
print("="*70)

# Read the data
print("\n1. Reading agent_scores.csv...")
df = pd.read_csv('agent_scores.csv')

print(f"   - Total rows: {len(df)}")
print(f"   - Unique candidates: {df['meeting_id'].nunique()}")
print(f"   - Unique skills (NOISY!): {df['criteria_name'].nunique()}")

# Aggregate scores per candidate per skill (average across evaluators)
print("\n2. Aggregating scores per candidate per skill...")
print("   (Averaging scores when multiple evaluators rate the same skill)")

candidate_skills = df.groupby(['meeting_id', 'criteria_name']).agg({
    'criteria_score': 'mean'  # Average score across different evaluators
}).reset_index()

print(f"   - After aggregation: {len(candidate_skills)} candidate-skill pairs")

# Transform to candidate-level format
print("\n3. Creating candidate profiles...")

candidates = []
for candidate_id in candidate_skills['meeting_id'].unique():
    # Get all skills and scores for this candidate
    candidate_data = candidate_skills[candidate_skills['meeting_id'] == candidate_id]

    # Extract skills and scores
    skills = candidate_data['criteria_name'].tolist()
    skill_scores = dict(zip(
        candidate_data['criteria_name'],
        candidate_data['criteria_score']
    ))

    candidate = {
        'candidate_id': candidate_id,
        'skills': skills,
        'skill_scores': skill_scores
    }

    candidates.append(candidate)

print(f"   [OK] Created {len(candidates)} candidate profiles")

# Show sample candidates
print("\n4. Sample candidates (BEFORE normalization):")
for i, candidate in enumerate(candidates[:3], 1):
    print(f"\n   Candidate {i}: {candidate['candidate_id']}")
    print(f"     - Number of skills: {len(candidate['skills'])}")
    print(f"     - Sample skills (first 5): {candidate['skills'][:5]}")
    print(f"     - Sample scores: {dict(list(candidate['skill_scores'].items())[:3])}")
    print(f"     - Avg score: {sum(candidate['skill_scores'].values()) / len(candidate['skill_scores']):.2f}")

# Save to JSON
print("\n5. Saving data...")
os.makedirs('data/processed', exist_ok=True)

output_file = 'data/processed/candidates_raw.json'
with open(output_file, 'w', encoding='utf-8') as f:
    json.dump(candidates, f, indent=2, ensure_ascii=False)

print(f"   [OK] Saved to {output_file}")

# Create summary statistics
print("\n6. Generating statistics...")

stats = {
    'total_candidates': len(candidates),
    'total_raw_skills': len(df['criteria_name'].unique()),
    'total_evaluations': len(df),
    'avg_skills_per_candidate': sum(len(c['skills']) for c in candidates) / len(candidates),
    'avg_score_global': df['criteria_score'].mean(),
    'median_score_global': df['criteria_score'].median(),
    'score_range': {
        'min': float(df['criteria_score'].min()),
        'max': float(df['criteria_score'].max())
    },
    'top_10_skills_by_frequency': df['criteria_name'].value_counts().head(10).to_dict()
}

with open('data/processed/raw_data_stats.json', 'w', encoding='utf-8') as f:
    json.dump(stats, f, indent=2)

print(f"   [OK] Saved statistics to data/processed/raw_data_stats.json")

# Show statistics
print("\n" + "="*70)
print("DATA STATISTICS (RAW/BEFORE NORMALIZATION)")
print("="*70)
print(f"\nTotal Candidates: {stats['total_candidates']}")
print(f"Total Raw/Noisy Skills: {stats['total_raw_skills']}")
print(f"Total Evaluations: {stats['total_evaluations']}")
print(f"Avg Skills per Candidate: {stats['avg_skills_per_candidate']:.1f}")
print(f"\nScore Statistics:")
print(f"  - Global Mean: {stats['avg_score_global']:.2f}")
print(f"  - Global Median: {stats['median_score_global']:.2f}")
print(f"  - Min: {stats['score_range']['min']}")
print(f"  - Max: {stats['score_range']['max']}")

print(f"\nTop 10 Most Frequent Skills (RAW):")
for i, (skill, count) in enumerate(list(stats['top_10_skills_by_frequency'].items()), 1):
    print(f"  {i:2d}. {skill:50s} ({count} evaluations)")

print("\n" + "="*70)
print("COMPLETE!")
print("="*70)
print("\nFiles created:")
print("  1. data/processed/candidates_raw.json - Candidate data (BEFORE normalization)")
print("  2. data/processed/raw_data_stats.json - Statistics")
print("\nNext step: Apply Phase 1 normalization to clean up noisy skills!")
print("  - Currently: 1271 unique raw skills")
print("  - Expected after normalization: ~300-500 canonical skills (50-60% reduction)")
print("="*70)
