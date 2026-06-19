"""
Simple test to demonstrate the solution to "too many zeros" problem.

Problem: Feature vectors have many zeros - ambiguous meaning:
  - 0 could mean "weak skill (scored 0)"
  - 0 could mean "missing skill (not mentioned)"

Solution: Add binary presence flags to distinguish these cases.
"""

import json
import numpy as np
from pathlib import Path
import sys

# Add src to path
sys.path.insert(0, str(Path(__file__).parent / 'src'))

from candidate_clustering.features.feature_engineer import FeatureEngineer
from candidate_clustering.features.enhanced_feature_engineer import EnhancedFeatureEngineer


def main():
    # Load normalized data
    print("Loading normalized data...")
    data_path = Path(__file__).parent / 'data' / 'processed' / 'candidates_normalized_tier2.json'
    
    with open(data_path, 'r', encoding='utf-8') as f:
        candidates = json.load(f)  # It's already a list, not a dict
    
    print(f"Loaded {len(candidates)} candidates\n")
    
    # Select two candidates for comparison
    # Candidate 1: Python specialist (few skills, deep)
    python_specialist = None
    for c in candidates:
        skills = c['normalized_skills']
        if 'python' in skills and len(skills) <= 5:
            python_specialist = c
            break
    
    # Candidate 2: Full-stack developer (many skills, broad)
    fullstack = None
    for c in candidates:
        skills = c['normalized_skills']
        if len(skills) >= 10:
            fullstack = c
            break
    
    if not python_specialist or not fullstack:
        print("❌ Could not find suitable candidates for comparison")
        return
    
    print("="*80)
    print("CANDIDATE COMPARISON")
    print("="*80)
    print(f"Candidate A ({python_specialist['candidate_id']}):")
    print(f"  Skills: {python_specialist['normalized_skills']}")
    print(f"  Total skills: {len(python_specialist['normalized_skills'])}")
    
    print(f"\nCandidate B ({fullstack['candidate_id']}):")
    print(f"  Skills: {fullstack['normalized_skills'][:10]}... (showing first 10)")
    print(f"  Total skills: {len(fullstack['normalized_skills'])}")
    
    # Test 1: OLD approach (standard FeatureEngineer)
    print("\n" + "="*80)
    print("⚠️  OLD APPROACH (Without Presence Flags)")
    print("="*80)
    
    old_engineer = FeatureEngineer()
    
    features_a_old = old_engineer.extract_features(
        candidate_id=python_specialist['candidate_id'],
        normalized_skills=python_specialist['normalized_skills'],
        normalized_scores=python_specialist['normalized_scores']
    )
    
    features_b_old = old_engineer.extract_features(
        candidate_id=fullstack['candidate_id'],
        normalized_skills=fullstack['normalized_skills'],
        normalized_scores=fullstack['normalized_scores']
    )
    
    print(f"\nCandidate A - Feature count: {features_a_old['feature_count']}")
    print(f"Candidate B - Feature count: {features_b_old['feature_count']}")
    
    # Show the problem with zeros
    print("\n💥 THE PROBLEM:")
    print("When we see a feature value of 0, we don't know if:")
    print("  1) The candidate HAS the skill but it's weak (scored 0)")
    print("  2) The candidate DOESN'T have the skill (missing/not mentioned)")
    print("\nThis makes clustering difficult!")
    
    # Test 2: NEW approach (Enhanced with presence flags)
    print("\n" + "="*80)
    print("✅ NEW APPROACH (With Presence Flags + Domain Features)")
    print("="*80)
    
    new_engineer = EnhancedFeatureEngineer(
        enable_inference=True,
        enable_reverse_inference=True,
        min_confidence=0.70,
        include_presence_features=True,
        include_domain_features=True
    )
    
    features_a_new = new_engineer.extract_features(
        candidate_id=python_specialist['candidate_id'],
        normalized_skills=python_specialist['normalized_skills'],
        normalized_scores=python_specialist['normalized_scores']
    )
    
    features_b_new = new_engineer.extract_features(
        candidate_id=fullstack['candidate_id'],
        normalized_skills=fullstack['normalized_skills'],
        normalized_scores=fullstack['normalized_scores']
    )
    
    print(f"\nCandidate A - Feature count: {features_a_new['feature_count']}")
    print(f"Candidate B - Feature count: {features_b_new['feature_count']}")
    
    added = features_a_new['feature_count'] - features_a_old['feature_count']
    print(f"\n✨ Added {added} new features!")
    
    # Show what's been added
    print("\n📊 NEW FEATURE TYPES:")
    
    # Count feature types in new features
    all_features = list(features_a_new['features'].keys())
    
    has_features = [f for f in all_features if f.startswith('has_')]
    domain_features = [f for f in all_features if f.startswith('domain_')]
    inference_features = [f for f in all_features if 'inference' in f]
    
    print(f"  1. Presence flags (has_X): {len(has_features)} features")
    print(f"     Example: has_python=1 means 'candidate mentioned Python'")
    print(f"              has_python=0 means 'candidate did NOT mention Python'")
    
    print(f"\n  2. Domain aggregations: {len(domain_features)} features")
    print(f"     Example: domain_backend_presence=0.8 means 'strong backend presence'")
    print(f"              domain_frontend_strength=0.3 means 'weak frontend skills'")
    
    print(f"\n  3. Inference statistics: {len(inference_features)} features")
    print(f"     Example: inference_avg_confidence=0.85 means 'high inference quality'")
    
    # Show examples
    print("\n" + "="*80)
    print("EXAMPLE FEATURES")
    print("="*80)
    
    print("\nPresence flags for Candidate A (Python specialist):")
    for feature in has_features[:5]:
        value = features_a_new['features'].get(feature, 0)
        status = "✓ MENTIONED" if value > 0 else "✗ NOT MENTIONED"
        print(f"  {feature}: {value:.0f} ({status})")
    
    print("\nDomain features for Candidate A:")
    for feature in domain_features[:6]:
        value = features_a_new['features'].get(feature, 0)
        print(f"  {feature}: {value:.2f}")
    
    print("\n" + "="*80)
    print("🎯 SOLUTION SUMMARY")
    print("="*80)
    print("\nThe enhanced approach solves 'too many zeros' by:")
    print("  1. Adding binary flags to distinguish 'weak' from 'missing'")
    print("  2. Adding domain features to capture specialist vs generalist")
    print("  3. Adding inference stats to measure inference quality")
    print("\nThis makes clustering more accurate and interpretable!")
    
    print("\n✅ Test completed successfully!")


if __name__ == '__main__':
    main()
