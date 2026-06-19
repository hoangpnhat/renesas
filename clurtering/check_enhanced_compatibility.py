"""
Check if candidates_normalized data is compatible with EnhancedFeatureEngineer
"""

import json
from pathlib import Path

# Load normalized candidates
print("="*80)
print("ENHANCED FEATURE ENGINEER - INPUT COMPATIBILITY CHECK")
print("="*80)

data_path = Path('data/processed/candidates_normalized_tier2.json')

if not data_path.exists():
    print(f"\n❌ ERROR: {data_path} not found!")
    print("   Run Phase 1 in complete_pipeline.ipynb first")
    exit(1)

with open(data_path, 'r', encoding='utf-8') as f:
    candidates = json.load(f)

print(f"\n✓ Loaded {len(candidates)} candidates")

# Check required fields for EnhancedFeatureEngineer
print("\n" + "="*80)
print("CHECKING REQUIRED FIELDS")
print("="*80)

required_fields = {
    'candidate_id': str,
    'normalized_skills': list,
    'normalized_scores': dict,
}

issues = []
compatible_count = 0

for i, candidate in enumerate(candidates[:5], 1):  # Check first 5
    print(f"\n--- Candidate {i}: {candidate.get('candidate_id', 'UNKNOWN')} ---")
    
    candidate_issues = []
    
    for field, expected_type in required_fields.items():
        if field not in candidate:
            candidate_issues.append(f"  ❌ Missing field: '{field}'")
        else:
            value = candidate[field]
            if not isinstance(value, expected_type):
                candidate_issues.append(
                    f"  ❌ Wrong type for '{field}': "
                    f"expected {expected_type.__name__}, got {type(value).__name__}"
                )
            else:
                print(f"  ✓ {field}: {expected_type.__name__}")
    
    if candidate_issues:
        issues.extend(candidate_issues)
        for issue in candidate_issues:
            print(issue)
    else:
        compatible_count += 1
        
        # Show sample data
        print(f"\n  Sample data:")
        print(f"    - ID: {candidate['candidate_id']}")
        print(f"    - Skills count: {len(candidate['normalized_skills'])}")
        print(f"    - Top 3 skills: {candidate['normalized_skills'][:3]}")
        
        # Check normalized_scores structure
        sample_skills = list(candidate['normalized_scores'].items())[:3]
        print(f"    - Sample scores:")
        for skill, score in sample_skills:
            print(f"      • {skill}: {score}")

print("\n" + "="*80)
print("VALIDATION SUMMARY")
print("="*80)

if issues:
    print(f"\n❌ Found {len(issues)} issues:")
    for issue in issues:
        print(issue)
else:
    print(f"\n✅ ALL CHECKS PASSED!")
    print(f"   {len(candidates)} candidates are compatible with EnhancedFeatureEngineer")

print("\n" + "="*80)
print("ENHANCED FEATURE ENGINEER USAGE")
print("="*80)

print("""
To use EnhancedFeatureEngineer in your pipeline:

```python
from src.candidate_clustering.features.enhanced_feature_engineer import EnhancedFeatureEngineer

# Initialize (with default settings)
engineer = EnhancedFeatureEngineer(
    enable_inference=True,           # Enable smart inference
    enable_reverse_inference=True,   # Enable domain-aware reverse
    min_confidence=0.70              # Minimum confidence threshold
)

# Extract features for each candidate
features_list = []
for candidate in candidates_normalized:
    features = engineer.extract_features(
        candidate_id=candidate['candidate_id'],
        normalized_skills=candidate['normalized_skills'],
        normalized_scores=candidate['normalized_scores'],
        include_metadata=True  # To see inference details
    )
    features_list.append(features)
```

Example output with metadata:
- features['vector']: numpy array of feature values
- features['feature_names']: list of feature names
- features['metadata']['skill_inference']:
  - forward_inferences: [{'skill': 'python', 'score': 76.5, 'confidence': 0.90, ...}]
  - reverse_inferences: [{'skill': 'pandas', 'score': 78.2, 'confidence': 0.95, ...}]
  - detected_domain: 'machine_learning'
""")

print("\n" + "="*80)
print("COMPATIBILITY CHECK COMPLETE")
print("="*80)
