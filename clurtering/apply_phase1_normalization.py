"""
Apply Phase 1 normalization to the actual candidate data

This will clean up the 1271 noisy skills into canonical skills
"""

import sys
from pathlib import Path
import json

# Add src to path
sys.path.insert(0, str(Path(__file__).parent / "src"))

from candidate_clustering.skills.normalizer import SkillNormalizer

print("="*70)
print("PHASE 1: SKILL NORMALIZATION ON ACTUAL DATA")
print("="*70)

# Load raw candidate data
print("\n1. Loading raw candidate data...")
with open('data/processed/candidates_raw.json', 'r', encoding='utf-8') as f:
    candidates = json.load(f)

print(f"   - Loaded {len(candidates)} candidates")
print(f"   - Total evaluations before normalization: {sum(len(c['skills']) for c in candidates)}")

# Get all unique raw skills
all_raw_skills = set()
for candidate in candidates:
    all_raw_skills.update(candidate['skills'])

print(f"   - Unique raw skills: {len(all_raw_skills)}")

# Initialize normalizer
print("\n2. Initializing normalizer...")
normalizer = SkillNormalizer()
print("   [OK] Normalizer initialized")

# Normalize all candidates using the dataset method
print("\n3. Normalizing skills and scores (this may take a while)...")
print("   Using Tier 1 (rule-based) normalization only for speed...")

normalized_candidates = normalizer.normalize_dataset(
    candidates,
    skill_field='skills',
    score_field='skill_scores',
    aggregation='max',  # Use max score when duplicates map to same canonical
    use_tier2=False  # Skip embedding-based clustering for now (too slow for 1271 skills)
)

print(f"   [OK] Normalized {len(normalized_candidates)} candidates")

# Analyze results
print("\n4. Analyzing normalization results...")

# Count unique normalized skills
all_normalized_skills = set()
for candidate in normalized_candidates:
    all_normalized_skills.update(candidate['normalized_skills'])

reduction = len(all_raw_skills) - len(all_normalized_skills)
reduction_pct = (reduction / len(all_raw_skills)) * 100

print(f"\n   BEFORE: {len(all_raw_skills)} unique raw skills")
print(f"   AFTER:  {len(all_normalized_skills)} unique canonical skills")
print(f"   REDUCTION: {reduction} skills ({reduction_pct:.1f}%)")

# Sample normalization results
print("\n5. Sample normalization mappings:")
sample_raw_skills = list(all_raw_skills)[:10]
for raw_skill in sample_raw_skills:
    canonical, conf = normalizer.normalize(raw_skill, use_tier2=False)
    if canonical != raw_skill.lower().replace(" ", "_"):  # Show interesting mappings
        print(f"   '{raw_skill}' -> '{canonical}' (confidence: {conf:.2f})")

# Save normalized data
print("\n6. Saving normalized data...")
output_file = 'data/processed/candidates_normalized.json'
with open(output_file, 'w', encoding='utf-8') as f:
    json.dump(normalized_candidates, f, indent=2, ensure_ascii=False)

print(f"   [OK] Saved to {output_file}")

# Save skill mappings for reuse
mappings_file = 'data/processed/skill_mappings.json'
normalizer.save_mappings(mappings_file)
print(f"   [OK] Saved skill mappings to {mappings_file}")

# Create normalization statistics
stats = {
    'total_candidates': len(normalized_candidates),
    'raw_skills_count': len(all_raw_skills),
    'normalized_skills_count': len(all_normalized_skills),
    'reduction_count': reduction,
    'reduction_percentage': reduction_pct,
    'avg_skills_per_candidate_before': sum(len(c['skills']) for c in candidates) / len(candidates),
    'avg_skills_per_candidate_after': sum(len(c['normalized_skills']) for c in normalized_candidates) / len(normalized_candidates),
    'top_20_normalized_skills': {}
}

# Count frequency of normalized skills
from collections import Counter
skill_counter = Counter()
for candidate in normalized_candidates:
    skill_counter.update(candidate['normalized_skills'])

stats['top_20_normalized_skills'] = dict(skill_counter.most_common(20))

with open('data/processed/normalization_stats.json', 'w', encoding='utf-8') as f:
    json.dump(stats, f, indent=2)

print(f"   [OK] Saved statistics to data/processed/normalization_stats.json")

# Show sample normalized candidates
print("\n7. Sample normalized candidates:")
for i, candidate in enumerate(normalized_candidates[:3], 1):
    orig = next(c for c in candidates if c['candidate_id'] == candidate['candidate_id'])
    print(f"\n   Candidate {i}: {candidate['candidate_id']}")
    print(f"     - Original skills: {len(orig['skills'])}")
    print(f"     - Normalized skills: {len(candidate['normalized_skills'])}")
    print(f"     - Reduction: {len(orig['skills']) - len(candidate['normalized_skills'])} skills")
    print(f"     - Sample normalized skills: {candidate['normalized_skills'][:5]}")
    print(f"     - Sample scores: {dict(list(candidate['normalized_scores'].items())[:3])}")

# Show statistics
print("\n" + "="*70)
print("NORMALIZATION STATISTICS")
print("="*70)
print(f"\nTotal Candidates: {stats['total_candidates']}")
print(f"\nSkill Reduction:")
print(f"  - Before: {stats['raw_skills_count']} unique skills")
print(f"  - After:  {stats['normalized_skills_count']} unique skills")
print(f"  - Reduced by: {stats['reduction_count']} skills ({stats['reduction_percentage']:.1f}%)")
print(f"\nAvg Skills per Candidate:")
print(f"  - Before: {stats['avg_skills_per_candidate_before']:.1f}")
print(f"  - After:  {stats['avg_skills_per_candidate_after']:.1f}")

print(f"\nTop 20 Most Common Skills (After Normalization):")
for i, (skill, count) in enumerate(list(stats['top_20_normalized_skills'].items()), 1):
    print(f"  {i:2d}. {skill:50s} ({count} candidates)")

print("\n" + "="*70)
print("PHASE 1 COMPLETE!")
print("="*70)
print("\nFiles created:")
print("  1. data/processed/candidates_normalized.json - Normalized candidate data")
print("  2. data/processed/skill_mappings.json - Skill normalization mappings")
print("  3. data/processed/normalization_stats.json - Normalization statistics")
print("\nNext steps:")
print("  - Review normalization results")
print("  - Refine normalization rules if needed")
print("  - Proceed to Phase 2: Feature Engineering")
print("="*70)
