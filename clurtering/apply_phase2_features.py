"""
Apply Phase 2: Feature Engineering to Normalized Data

This will extract comprehensive features from normalized skills including:
- Core skill features
- Inferred skill scores
- Domain aggregations
- Weakness modeling (critical for clustering!)
- Profile characteristics

Output: Feature vectors ready for clustering (Phase 3)
"""

import sys
from pathlib import Path
import json
import numpy as np
import pandas as pd

# Add src to path
sys.path.insert(0, str(Path(__file__).parent / "src"))

from candidate_clustering.features.feature_engineer import FeatureEngineer

print("="*70)
print("PHASE 2: FEATURE ENGINEERING")
print("="*70)

# Load normalized candidates
print("\n1. Loading normalized candidates (Tier 2)...")
with open('data/processed/candidates_normalized_tier2.json', 'r', encoding='utf-8') as f:
    candidates = json.load(f)

print(f"   - Loaded {len(candidates)} candidates")
print(f"   - Total normalized skills: {len(set(s for c in candidates for s in c['normalized_skills']))}")

# Initialize feature engineer
print("\n2. Initializing Feature Engineer...")
engineer = FeatureEngineer()
print("   [OK] Feature Engineer initialized")

# Fit on dataset to compute statistics
print("\n3. Fitting on dataset to compute skill statistics...")
print("   (This computes percentiles, domain averages for weakness modeling)")
engineer.fit(candidates)

print(f"   [OK] Computed statistics for {len(engineer.skill_percentiles)} skills")
print(f"   [OK] Computed averages for {len(engineer.domain_avg_scores)} domains")
print(f"   [OK] Global average score: {engineer.global_avg_score:.2f}")

# Extract features for all candidates
print("\n4. Extracting features for all candidates...")
print("   This will generate ~600 features per candidate including:")
print("   - Core skill features")
print("   - Inferred skill scores")
print("   - Domain aggregations")
print("   - Weakness features (CRITICAL!)")
print("   - Profile characteristics")

features_list = engineer.extract_features_batch(candidates, include_metadata=True)

print(f"\n   [OK] Extracted features for {len(features_list)} candidates")

# Show sample features
sample = features_list[0]
print(f"\n   Sample candidate: {sample['candidate_id']}")
print(f"   - Feature count: {sample['feature_count']}")
print(f"   - Sample features: {list(sample['features'].keys())[:5]}...")

# Analyze features
print("\n5. Analyzing feature quality...")

# Count feature types
feature_counts = {
    'skill_features': 0,
    'inferred_features': 0,
    'domain_features': 0,
    'weakness_features': 0,
    'profile_features': 0,
    'statistical_features': 0
}

for feature_name in sample['features'].keys():
    if feature_name.startswith('skill_'):
        feature_counts['skill_features'] += 1
    elif feature_name.startswith('inferred_'):
        feature_counts['inferred_features'] += 1
    elif feature_name.startswith('domain_'):
        feature_counts['domain_features'] += 1
    elif 'weak' in feature_name or 'weakness' in feature_name:
        feature_counts['weakness_features'] += 1
    elif feature_name in ['breadth_score', 'depth_score', 'specialization_index',
                          'advanced_skill_ratio', 'profile_type_specialist', 'profile_type_generalist']:
        feature_counts['profile_features'] += 1
    elif feature_name.startswith('score_'):
        feature_counts['statistical_features'] += 1

print("\n   Feature breakdown:")
for feature_type, count in feature_counts.items():
    print(f"   - {feature_type:30}: {count:4d} features")

# Get all feature names (for consistent feature vectors)
print("\n6. Creating consistent feature matrix...")
all_feature_names = engineer.get_all_feature_names(candidates)
print(f"   - Total unique features across all candidates: {len(all_feature_names)}")

# Convert to feature matrix
feature_matrix = []
for features_dict in features_list:
    vector = engineer.get_feature_vector(features_dict['features'], all_feature_names)
    feature_matrix.append(vector)

feature_matrix = np.array(feature_matrix)
print(f"   - Feature matrix shape: {feature_matrix.shape}")
print(f"     ({feature_matrix.shape[0]} candidates × {feature_matrix.shape[1]} features)")

# Check for NaN/Inf
nan_count = np.isnan(feature_matrix).sum()
inf_count = np.isinf(feature_matrix).sum()
print(f"   - NaN values: {nan_count}")
print(f"   - Inf values: {inf_count}")

if nan_count > 0 or inf_count > 0:
    print("   [WARNING] Cleaning NaN/Inf values...")
    feature_matrix = np.nan_to_num(feature_matrix, nan=0.0, posinf=100.0, neginf=0.0)
    print("   [OK] Cleaned")

# Analyze weakness features
print("\n7. Analyzing weakness modeling...")
weakness_stats = []
for features_dict in features_list:
    candidate_id = features_dict['candidate_id']
    features = features_dict['features']

    weakness_count = features.get('weakness_count', 0)
    weakness_ratio = features.get('weakness_ratio', 0)

    if 'metadata' in features_dict and 'weaknesses' in features_dict['metadata']:
        weak_skills = features_dict['metadata']['weaknesses'].get('weak_skills', [])
    else:
        weak_skills = []

    weakness_stats.append({
        'candidate_id': candidate_id,
        'weakness_count': weakness_count,
        'weakness_ratio': weakness_ratio,
        'weak_skills_sample': [w['skill'] for w in weak_skills[:3]] if weak_skills else []
    })

print(f"   - Candidates with weaknesses: {sum(1 for w in weakness_stats if w['weakness_count'] > 0)}/{len(weakness_stats)}")
print(f"   - Avg weakness count: {np.mean([w['weakness_count'] for w in weakness_stats]):.2f}")
print(f"   - Avg weakness ratio: {np.mean([w['weakness_ratio'] for w in weakness_stats]):.2f}")

# Show sample weaknesses
print("\n   Sample candidates with weaknesses:")
for i, stat in enumerate(weakness_stats[:3], 1):
    if stat['weakness_count'] > 0:
        print(f"   {i}. {stat['candidate_id']}")
        print(f"      - Weakness count: {stat['weakness_count']}")
        print(f"      - Weak skills: {stat['weak_skills_sample']}")

# Save results
print("\n8. Saving Phase 2 results...")

# Save feature data
output = {
    'candidates': [
        {
            'candidate_id': f['candidate_id'],
            'feature_count': f['feature_count'],
            'features': f['features'],
            'metadata': f.get('metadata', {})
        }
        for f in features_list
    ],
    'feature_names': all_feature_names,
    'feature_matrix_shape': feature_matrix.shape
}

with open('data/processed/candidates_features.json', 'w', encoding='utf-8') as f:
    json.dump(output, f, indent=2)

print(f"   [OK] Saved to data/processed/candidates_features.json")

# Save feature matrix as numpy
np.save('data/processed/feature_matrix.npy', feature_matrix)
print(f"   [OK] Saved feature matrix to data/processed/feature_matrix.npy")

# Save feature names
with open('data/processed/feature_names.json', 'w', encoding='utf-8') as f:
    json.dump(all_feature_names, f, indent=2)
print(f"   [OK] Saved feature names to data/processed/feature_names.json")

# Create statistics
stats = {
    'total_candidates': len(candidates),
    'feature_matrix_shape': list(feature_matrix.shape),
    'total_features': len(all_feature_names),
    'feature_breakdown': feature_counts,
    'weakness_statistics': {
        'candidates_with_weaknesses': sum(1 for w in weakness_stats if w['weakness_count'] > 0),
        'avg_weakness_count': float(np.mean([w['weakness_count'] for w in weakness_stats])),
        'avg_weakness_ratio': float(np.mean([w['weakness_ratio'] for w in weakness_stats])),
    },
    'feature_statistics': {
        'mean': feature_matrix.mean(axis=0).tolist()[:10],  # First 10 features
        'std': feature_matrix.std(axis=0).tolist()[:10],
        'min': feature_matrix.min(axis=0).tolist()[:10],
        'max': feature_matrix.max(axis=0).tolist()[:10]
    }
}

with open('data/processed/phase2_stats.json', 'w', encoding='utf-8') as f:
    json.dump(stats, f, indent=2)

print(f"   [OK] Saved statistics to data/processed/phase2_stats.json")

# Create summary report
print("\n" + "="*70)
print("PHASE 2 COMPLETE - FEATURE ENGINEERING SUMMARY")
print("="*70)

print(f"\nDataset:")
print(f"  - Candidates: {stats['total_candidates']}")
print(f"  - Feature matrix: {stats['feature_matrix_shape'][0]} × {stats['feature_matrix_shape'][1]}")

print(f"\nFeature Breakdown:")
for feature_type, count in feature_counts.items():
    print(f"  - {feature_type:30}: {count:4d} features")

print(f"\nWeakness Modeling:")
print(f"  - Candidates with weaknesses: {stats['weakness_statistics']['candidates_with_weaknesses']}/{stats['total_candidates']}")
print(f"  - Avg weakness count: {stats['weakness_statistics']['avg_weakness_count']:.2f}")
print(f"  - Avg weakness ratio: {stats['weakness_statistics']['avg_weakness_ratio']:.2%}")

print(f"\nFiles Created:")
print(f"  1. data/processed/candidates_features.json - Full feature data")
print(f"  2. data/processed/feature_matrix.npy - NumPy feature matrix")
print(f"  3. data/processed/feature_names.json - Feature name list")
print(f"  4. data/processed/phase2_stats.json - Statistics")

print("\n" + "="*70)
print("READY FOR PHASE 3: CLUSTERING!")
print("="*70)
print("\nNext steps:")
print("  1. Dimensionality reduction (PCA + UMAP): 600+ dims → 20-30 dims")
print("  2. Ensemble clustering (HDBSCAN + GMM + Hierarchical)")
print("  3. Generate multi-label cluster profiles")
print("="*70)
