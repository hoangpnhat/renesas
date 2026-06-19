"""
Combine and transform the 3 sheets into the expected candidate clustering format
"""

import pandas as pd
import json

print("="*70)
print("COMBINING AND TRANSFORMING DATA")
print("="*70)

# Read all sheets
print("\n1. Reading Excel file...")
xls = pd.ExcelFile('Clustering Project.xlsx')

sheet1 = pd.read_excel(xls, sheet_name='Sheet1')  # Transcripts
sheet3 = pd.read_excel(xls, sheet_name='Sheet3')  # Main candidate data
sheet4 = pd.read_excel(xls, sheet_name='Sheet4')  # Meeting statistics

print(f"   - Sheet1 (transcripts): {sheet1.shape[0]} rows")
print(f"   - Sheet3 (candidate data): {sheet3.shape[0]} rows")
print(f"   - Sheet4 (meeting stats): {sheet4.shape[0]} rows")

# Analyze Sheet3 structure
print("\n2. Analyzing Sheet3 (main data)...")
print(f"   - Unique candidates: {sheet3['candidate_id'].nunique()}")
print(f"   - Unique criteria/skills: {sheet3['criteria_name'].nunique()}")
print(f"   - Unique roles: {sheet3['role_name'].nunique()}")

print(f"\n   Sample criteria/skills:")
for i, skill in enumerate(sheet3['criteria_name'].unique()[:10], 1):
    print(f"     {i}. {skill}")

print(f"\n   Sample roles:")
for i, role in enumerate(sheet3['role_name'].unique(), 1):
    print(f"     {i}. {role}")

# Transform Sheet3 to candidate-level format
print("\n3. Transforming data to candidate-level format...")

# Group by candidate_id and criteria_name, aggregate scores
# Each candidate has multiple rows (different roles evaluating same criteria)
candidate_skills = sheet3.groupby(['candidate_id', 'criteria_name']).agg({
    'criteria_score': 'mean',  # Average score across different evaluators
    'criteria_weight': 'mean'
}).reset_index()

print(f"   - After aggregation: {len(candidate_skills)} candidate-skill pairs")

# Pivot to get skills and scores per candidate
print("\n4. Creating candidate profiles...")

candidates = []
for candidate_id in candidate_skills['candidate_id'].unique():
    # Get all skills and scores for this candidate
    candidate_data = candidate_skills[candidate_skills['candidate_id'] == candidate_id]

    # Extract skills and scores
    skills = candidate_data['criteria_name'].tolist()
    skill_scores = dict(zip(
        candidate_data['criteria_name'],
        candidate_data['criteria_score']
    ))

    # Get transcript if available
    transcript = None
    if candidate_id in sheet1['id'].values:
        transcript = sheet1[sheet1['id'] == candidate_id]['transcript'].iloc[0]

    candidate = {
        'candidate_id': candidate_id,
        'skills': skills,
        'skill_scores': skill_scores,
        'interview_transcript': transcript
    }

    candidates.append(candidate)

print(f"   - Total candidates: {len(candidates)}")

# Show sample candidates
print("\n5. Sample candidates:")
for i, candidate in enumerate(candidates[:3], 1):
    print(f"\n   Candidate {i}: {candidate['candidate_id']}")
    print(f"     - Skills: {len(candidate['skills'])}")
    print(f"     - Skills list: {candidate['skills'][:5]}...")
    print(f"     - Sample scores: {dict(list(candidate['skill_scores'].items())[:3])}")
    print(f"     - Has transcript: {'Yes' if candidate['interview_transcript'] else 'No'}")

# Save combined data
print("\n6. Saving combined data...")

# Save as JSON
output_file = 'data/processed/candidates_combined.json'
import os
os.makedirs('data/processed', exist_ok=True)

with open(output_file, 'w', encoding='utf-8') as f:
    json.dump(candidates, f, indent=2, ensure_ascii=False)

print(f"   ✓ Saved to {output_file}")

# Save summary statistics
stats = {
    'total_candidates': len(candidates),
    'total_skills': len(sheet3['criteria_name'].unique()),
    'avg_skills_per_candidate': sum(len(c['skills']) for c in candidates) / len(candidates),
    'unique_skills': sorted(sheet3['criteria_name'].unique().tolist()),
    'score_range': {
        'min': float(sheet3['criteria_score'].min()),
        'max': float(sheet3['criteria_score'].max()),
        'mean': float(sheet3['criteria_score'].mean()),
        'median': float(sheet3['criteria_score'].median())
    }
}

with open('data/processed/data_summary.json', 'w', encoding='utf-8') as f:
    json.dump(stats, f, indent=2)

print(f"   ✓ Saved summary to data/processed/data_summary.json")

# Create DataFrame for analysis
df_candidates = pd.DataFrame([
    {
        'candidate_id': c['candidate_id'],
        'num_skills': len(c['skills']),
        'avg_score': sum(c['skill_scores'].values()) / len(c['skill_scores']) if c['skill_scores'] else 0,
        'max_score': max(c['skill_scores'].values()) if c['skill_scores'] else 0,
        'min_score': min(c['skill_scores'].values()) if c['skill_scores'] else 0,
        'has_transcript': c['interview_transcript'] is not None
    }
    for c in candidates
])

df_candidates.to_csv('data/processed/candidates_summary.csv', index=False)
print(f"   ✓ Saved candidate summary to data/processed/candidates_summary.csv")

print("\n" + "="*70)
print("DATA STATISTICS")
print("="*70)
print(f"\nTotal Candidates: {stats['total_candidates']}")
print(f"Total Unique Skills: {stats['total_skills']}")
print(f"Avg Skills per Candidate: {stats['avg_skills_per_candidate']:.1f}")
print(f"\nScore Range:")
print(f"  - Min: {stats['score_range']['min']}")
print(f"  - Max: {stats['score_range']['max']}")
print(f"  - Mean: {stats['score_range']['mean']:.2f}")
print(f"  - Median: {stats['score_range']['median']:.2f}")

print(f"\nCandidates with Transcripts: {df_candidates['has_transcript'].sum()} / {len(df_candidates)}")

print("\n" + "="*70)
print("COMPLETE!")
print("="*70)
print("\nFiles created:")
print("  1. data/processed/candidates_combined.json - Main candidate data")
print("  2. data/processed/data_summary.json - Statistics")
print("  3. data/processed/candidates_summary.csv - Candidate summary")
print("\nNext step: Run Phase 1 normalization on this data")
print("="*70)
