"""
Apply Phase 1 normalization WITH Tier 2 (semantic clustering)

This will use sentence embeddings to cluster semantically similar skills
Expected: 40-50% reduction (1271 -> ~500-750 skills)
"""

import sys
from pathlib import Path
import json

# Add src to path
sys.path.insert(0, str(Path(__file__).parent / "src"))

from candidate_clustering.skills.normalizer import SkillNormalizer

print("="*70)
print("PHASE 1: TIER 2 SEMANTIC NORMALIZATION")
print("="*70)
print("\nThis will take 10-15 minutes to:")
print("  1. Load sentence embedding model (~400MB)")
print("  2. Generate embeddings for 1,271 skills")
print("  3. Cluster semantically similar skills")
print("  4. Apply normalization to all candidates")

# Load raw candidate data
print("\n1. Loading raw candidate data...")
with open('data/processed/candidates_raw.json', 'r', encoding='utf-8') as f:
    candidates = json.load(f)

print(f"   - Loaded {len(candidates)} candidates")

# Get all unique raw skills
all_raw_skills = set()
for candidate in candidates:
    all_raw_skills.update(candidate['skills'])

print(f"   - Unique raw skills: {len(all_raw_skills)}")

# Initialize normalizer with local model
print("\n2. Initializing normalizer with local model...")
# Use local model path instead of downloading from HuggingFace
local_model_path = "all-mpnet-base-v2"  # Local folder with the model
normalizer = SkillNormalizer(embedding_model=local_model_path)
print("   [OK] Normalizer initialized with local model")

# Normalize with Tier 2 enabled
print("\n3. Normalizing with Tier 1 + Tier 2 (semantic clustering)...")
print("   This may take 10-15 minutes - please wait...")
print("   Progress will be logged as it processes...")

normalized_candidates = normalizer.normalize_dataset(
    candidates,
    skill_field='skills',
    score_field='skill_scores',
    aggregation='max',
    use_tier2=True  # ENABLE TIER 2!
)

print(f"\n   [OK] Normalized {len(normalized_candidates)} candidates")

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

# Compare with Tier 1 only results
print("\n5. Comparing with Tier 1 only results...")
print(f"   Tier 1 only: 1271 -> 1232 (3.1% reduction)")
print(f"   Tier 1 + 2:  {len(all_raw_skills)} -> {len(all_normalized_skills)} ({reduction_pct:.1f}% reduction)")
print(f"   Improvement: {reduction_pct - 3.1:.1f}% additional reduction from Tier 2")

# Show interesting mappings
print("\n6. Sample semantic mappings (from Tier 2):")
sample_skills = [
    "API Design",
    "API Design Skills",
    "API Design Competence",
    "Microservices",
    "Microservices Architecture",
    "Backend Development",
    "Backend Engineering",
    "Performance Optimization",
    "Performance Tuning",
    "Database Management",
    "Database Management Skills"
]

for skill in sample_skills:
    if skill in all_raw_skills:
        canonical, conf = normalizer.normalize(skill, use_tier2=True)
        print(f"   '{skill}' -> '{canonical}' (confidence: {conf:.2f})")

# Save normalized data
print("\n7. Saving Tier 2 normalized data...")
output_file = 'data/processed/candidates_normalized_tier2.json'
with open(output_file, 'w', encoding='utf-8') as f:
    json.dump(normalized_candidates, f, indent=2, ensure_ascii=False)

print(f"   [OK] Saved to {output_file}")

# Save Tier 2 skill mappings
mappings_file = 'data/processed/skill_mappings_tier2.json'
normalizer.save_mappings(mappings_file)
print(f"   [OK] Saved skill mappings to {mappings_file}")

# Create detailed statistics
from collections import Counter

skill_counter = Counter()
for candidate in normalized_candidates:
    skill_counter.update(candidate['normalized_skills'])

stats = {
    'normalization_level': 'Tier 1 + Tier 2 (semantic clustering)',
    'total_candidates': len(normalized_candidates),
    'raw_skills_count': len(all_raw_skills),
    'normalized_skills_count': len(all_normalized_skills),
    'reduction_count': reduction,
    'reduction_percentage': reduction_pct,
    'avg_skills_per_candidate_before': sum(len(c['skills']) for c in candidates) / len(candidates),
    'avg_skills_per_candidate_after': sum(len(c['normalized_skills']) for c in normalized_candidates) / len(normalized_candidates),
    'top_30_normalized_skills': dict(skill_counter.most_common(30))
}

with open('data/processed/normalization_stats_tier2.json', 'w', encoding='utf-8') as f:
    json.dump(stats, f, indent=2)

print(f"   [OK] Saved statistics to data/processed/normalization_stats_tier2.json")

# Show sample normalized candidates
print("\n8. Sample normalized candidates (Tier 2):")
for i, candidate in enumerate(normalized_candidates[:3], 1):
    orig = next(c for c in candidates if c['candidate_id'] == candidate['candidate_id'])
    print(f"\n   Candidate {i}: {candidate['candidate_id']}")
    print(f"     - Original skills: {len(orig['skills'])}")
    print(f"     - Normalized skills: {len(candidate['normalized_skills'])}")
    print(f"     - Reduction: {len(orig['skills']) - len(candidate['normalized_skills'])} skills")
    print(f"     - Sample normalized: {candidate['normalized_skills'][:5]}")

# Show statistics
print("\n" + "="*70)
print("TIER 2 NORMALIZATION STATISTICS")
print("="*70)
print(f"\nTotal Candidates: {stats['total_candidates']}")
print(f"\nSkill Reduction:")
print(f"  - Before: {stats['raw_skills_count']} unique skills")
print(f"  - After:  {stats['normalized_skills_count']} unique skills")
print(f"  - Reduced by: {stats['reduction_count']} skills ({stats['reduction_percentage']:.1f}%)")
print(f"\nAvg Skills per Candidate:")
print(f"  - Before: {stats['avg_skills_per_candidate_before']:.1f}")
print(f"  - After:  {stats['avg_skills_per_candidate_after']:.1f}")

print(f"\nTop 30 Most Common Skills (After Tier 2 Normalization):")
for i, (skill, count) in enumerate(list(stats['top_30_normalized_skills'].items()), 1):
    print(f"  {i:2d}. {skill:50s} ({count} candidates)")

print("\n" + "="*70)
print("TIER 2 NORMALIZATION COMPLETE!")
print("="*70)
print("\nFiles created:")
print("  1. data/processed/candidates_normalized_tier2.json - Tier 2 normalized data")
print("  2. data/processed/skill_mappings_tier2.json - Tier 2 mappings")
print("  3. data/processed/normalization_stats_tier2.json - Tier 2 statistics")
print("\nNext step: Proceed to Phase 2 (Feature Engineering) with clean data!")
print("="*70)
