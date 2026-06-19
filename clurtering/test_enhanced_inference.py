"""
Test script for Enhanced Feature Engineer with smart skill inference.

This script demonstrates:
1. Forward inference (FastAPI → Python)
2. Domain-aware reverse inference (Python + ML → pandas/numpy)
3. Confidence filtering
4. Before/After comparison
"""

import json
from pathlib import Path
from src.candidate_clustering.features.enhanced_feature_engineer import EnhancedFeatureEngineer


def test_forward_inference():
    """Test forward inference: FastAPI → Python"""
    print("\n" + "="*80)
    print("TEST 1: Forward Inference (FastAPI → Python)")
    print("="*80)
    
    engineer = EnhancedFeatureEngineer(
        enable_inference=True,
        enable_reverse_inference=False,
        min_confidence=0.70
    )
    
    # Candidate mentions FastAPI but not Python
    candidate = {
        'candidate_id': 'test_001',
        'normalized_skills': ['fastapi', 'postgresql', 'docker'],
        'normalized_scores': {
            'fastapi': 85,
            'postgresql': 80,
            'docker': 75
        }
    }
    
    print("\n📥 INPUT:")
    print(f"Skills: {candidate['normalized_skills']}")
    print(f"Scores: {json.dumps(candidate['normalized_scores'], indent=2)}")
    
    # Apply inference
    enhanced_scores, metadata = engineer._apply_skill_inference(
        candidate['normalized_skills'],
        candidate['normalized_scores']
    )
    
    print("\n📤 OUTPUT (After Forward Inference):")
    print(f"Enhanced scores ({len(enhanced_scores)} skills):")
    for skill, score in sorted(enhanced_scores.items(), key=lambda x: -x[1]):
        if skill not in candidate['normalized_scores']:
            print(f"  ✨ {skill}: {score:.1f} (INFERRED)")
        else:
            print(f"  ✓ {skill}: {score:.1f}")
    
    print("\n📊 Inference Details:")
    for inf in metadata['forward_inferences']:
        print(f"  {inf['skill']}: {inf['score']:.1f} from {inf['source']} "
              f"(confidence: {inf['confidence']:.0%}, type: {inf['type']})")


def test_reverse_inference_ml():
    """Test reverse inference: Python + ML domain → pandas/numpy/pytorch"""
    print("\n" + "="*80)
    print("TEST 2: Domain-Aware Reverse Inference (ML Engineer)")
    print("="*80)
    
    engineer = EnhancedFeatureEngineer(
        enable_inference=True,
        enable_reverse_inference=True,
        min_confidence=0.70
    )
    
    # ML engineer with Python but no libraries
    candidate = {
        'candidate_id': 'test_002',
        'normalized_skills': ['python', 'machine learning', 'deep learning'],
        'normalized_scores': {
            'python': 92,
            'machine learning': 88,
            'deep learning': 80
        }
    }
    
    print("\n📥 INPUT:")
    print(f"Skills: {candidate['normalized_skills']}")
    print(f"Scores: {json.dumps(candidate['normalized_scores'], indent=2)}")
    
    # Apply inference
    enhanced_scores, metadata = engineer._apply_skill_inference(
        candidate['normalized_skills'],
        candidate['normalized_scores']
    )
    
    print("\n📤 OUTPUT (After Forward + Reverse Inference):")
    print(f"Domain detected: {metadata['detected_domain']}")
    print(f"\nEnhanced scores ({len(enhanced_scores)} skills):")
    for skill, score in sorted(enhanced_scores.items(), key=lambda x: -x[1]):
        if skill not in candidate['normalized_scores']:
            print(f"  ✨ {skill}: {score:.1f} (INFERRED)")
        else:
            print(f"  ✓ {skill}: {score:.1f}")
    
    print("\n📊 Reverse Inference Details:")
    for inf in metadata['reverse_inferences']:
        print(f"  {inf['skill']}: {inf['score']:.1f} from {inf['source']} "
              f"(confidence: {inf['confidence']:.0%}, type: {inf['type']})")


def test_reverse_inference_backend():
    """Test reverse inference: Python + backend → Django (one framework)"""
    print("\n" + "="*80)
    print("TEST 3: Domain-Aware Reverse Inference (Backend Developer)")
    print("="*80)
    
    engineer = EnhancedFeatureEngineer(
        enable_inference=True,
        enable_reverse_inference=True,
        min_confidence=0.70
    )
    
    # Backend developer with Python but no framework
    candidate = {
        'candidate_id': 'test_003',
        'normalized_skills': ['python', 'rest api', 'postgresql'],
        'normalized_scores': {
            'python': 90,
            'rest api': 85,
            'postgresql': 80
        }
    }
    
    print("\n📥 INPUT:")
    print(f"Skills: {candidate['normalized_skills']}")
    print(f"Scores: {json.dumps(candidate['normalized_scores'], indent=2)}")
    
    # Apply inference
    enhanced_scores, metadata = engineer._apply_skill_inference(
        candidate['normalized_skills'],
        candidate['normalized_scores']
    )
    
    print("\n📤 OUTPUT (After Forward + Reverse Inference):")
    print(f"Domain detected: {metadata['detected_domain']}")
    print(f"\nEnhanced scores ({len(enhanced_scores)} skills):")
    for skill, score in sorted(enhanced_scores.items(), key=lambda x: -x[1]):
        if skill not in candidate['normalized_scores']:
            print(f"  ✨ {skill}: {score:.1f} (INFERRED)")
        else:
            print(f"  ✓ {skill}: {score:.1f}")
    
    print("\n📊 Forward Inference Details:")
    for inf in metadata.get('forward_inferences', []):
        print(f"  {inf['skill']}: {inf['score']:.1f} from {inf['source']} "
              f"(confidence: {inf['confidence']:.0%})")
    
    print("\n📊 Reverse Inference Details:")
    for inf in metadata['reverse_inferences']:
        print(f"  {inf['skill']}: {inf['score']:.1f} from {inf['source']} "
              f"(confidence: {inf['confidence']:.0%}, type: {inf['type']})")


def test_skip_reverse_with_explicit_framework():
    """Test that reverse inference is skipped when framework is already mentioned"""
    print("\n" + "="*80)
    print("TEST 4: Skip Reverse Inference (Has Explicit Framework)")
    print("="*80)
    
    engineer = EnhancedFeatureEngineer(
        enable_inference=True,
        enable_reverse_inference=True,
        min_confidence=0.70
    )
    
    # Backend developer with Django already mentioned
    candidate = {
        'candidate_id': 'test_004',
        'normalized_skills': ['python', 'django', 'postgresql'],
        'normalized_scores': {
            'python': 85,
            'django': 82,
            'postgresql': 75
        }
    }
    
    print("\n📥 INPUT:")
    print(f"Skills: {candidate['normalized_skills']}")
    print(f"Scores: {json.dumps(candidate['normalized_scores'], indent=2)}")
    
    # Apply inference
    enhanced_scores, metadata = engineer._apply_skill_inference(
        candidate['normalized_skills'],
        candidate['normalized_scores']
    )
    
    print("\n📤 OUTPUT (After Inference):")
    print(f"Domain detected: {metadata['detected_domain']}")
    print(f"\nEnhanced scores ({len(enhanced_scores)} skills):")
    for skill, score in sorted(enhanced_scores.items(), key=lambda x: -x[1]):
        if skill not in candidate['normalized_scores']:
            print(f"  ✨ {skill}: {score:.1f} (INFERRED)")
        else:
            print(f"  ✓ {skill}: {score:.1f}")
    
    print("\n📊 Forward Inference Details:")
    for inf in metadata.get('forward_inferences', []):
        print(f"  {inf['skill']}: {inf['score']:.1f} from {inf['source']} "
              f"(confidence: {inf['confidence']:.0%})")
    
    reverse_count = len(metadata.get('reverse_inferences', []))
    print(f"\n📊 Reverse Inference: {reverse_count} inferences")
    if reverse_count > 0:
        for inf in metadata['reverse_inferences']:
            print(f"  {inf['skill']}: {inf['score']:.1f} from {inf['source']}")
    else:
        print("  ✓ No frameworks inferred (already has Django)")


def test_before_after_comparison():
    """Compare feature vectors before/after inference"""
    print("\n" + "="*80)
    print("TEST 5: Before/After Feature Vector Comparison")
    print("="*80)
    
    # WITHOUT inference
    engineer_no_inference = EnhancedFeatureEngineer(
        enable_inference=False,
        enable_reverse_inference=False
    )
    
    # WITH inference
    engineer_with_inference = EnhancedFeatureEngineer(
        enable_inference=True,
        enable_reverse_inference=True,
        min_confidence=0.70
    )
    
    candidate = {
        'candidate_id': 'test_005',
        'normalized_skills': ['python', 'machine learning'],
        'normalized_scores': {
            'python': 90,
            'machine learning': 85
        }
    }
    
    print("\n📥 INPUT:")
    print(f"Skills: {candidate['normalized_skills']}")
    print(f"Scores: {json.dumps(candidate['normalized_scores'], indent=2)}")
    
    # Without inference
    scores_before, _ = engineer_no_inference._apply_skill_inference(
        candidate['normalized_skills'],
        candidate['normalized_scores']
    )
    
    # With inference
    scores_after, metadata = engineer_with_inference._apply_skill_inference(
        candidate['normalized_skills'],
        candidate['normalized_scores']
    )
    
    print("\n📊 COMPARISON:")
    print(f"Before inference: {len(scores_before)} skills")
    print(f"After inference:  {len(scores_after)} skills")
    print(f"Skills added:     {len(scores_after) - len(scores_before)}")
    
    print("\n📈 Skills Added:")
    for skill in scores_after:
        if skill not in scores_before:
            confidence = None
            for inf in metadata.get('reverse_inferences', []):
                if inf['skill'] == skill:
                    confidence = inf['confidence']
                    break
            
            conf_str = f"(confidence: {confidence:.0%})" if confidence else ""
            print(f"  ✨ {skill}: {scores_after[skill]:.1f} {conf_str}")


def test_real_dataset():
    """Test on real candidates from dataset"""
    print("\n" + "="*80)
    print("TEST 6: Real Dataset Statistics")
    print("="*80)
    
    # Load real data
    data_path = Path("data/processed/candidates_normalized.json")
    
    if not data_path.exists():
        print(f"❌ Data file not found: {data_path}")
        print("   Run the normalization pipeline first.")
        return
    
    with open(data_path, 'r', encoding='utf-8') as f:
        candidates = json.load(f)
    
    print(f"\n📊 Loaded {len(candidates)} candidates")
    
    # Create engineer with inference
    engineer = EnhancedFeatureEngineer(
        enable_inference=True,
        enable_reverse_inference=True,
        min_confidence=0.70
    )
    
    # Get statistics
    stats = engineer.get_inference_stats(candidates)
    
    print("\n📈 INFERENCE STATISTICS:")
    print(f"Total candidates: {stats['total_candidates']}")
    print(f"With forward inference: {stats['candidates_with_forward_inference']} "
          f"({stats['candidates_with_forward_inference']/stats['total_candidates']*100:.1f}%)")
    print(f"With reverse inference: {stats['candidates_with_reverse_inference']} "
          f"({stats['candidates_with_reverse_inference']/stats['total_candidates']*100:.1f}%)")
    print(f"\nTotal forward inferences: {stats['total_forward_inferences']}")
    print(f"Total reverse inferences: {stats['total_reverse_inferences']}")
    print(f"\nAvg skills before: {stats['avg_skills_before']:.1f}")
    print(f"Avg skills after:  {stats['avg_skills_after']:.1f}")
    print(f"Avg skills added:  {stats['avg_skills_added']:.1f} (+{stats['skill_increase_pct']:.1f}%)")
    
    print("\n🎯 Domain Distribution:")
    for domain, count in sorted(stats['domain_distribution'].items(), key=lambda x: -x[1]):
        pct = count / stats['total_candidates'] * 100
        print(f"  {domain}: {count} ({pct:.1f}%)")
    
    print("\n⭐ Most Inferred Skills (Top 10):")
    for skill, count in list(stats['most_inferred_skills'].items())[:10]:
        pct = count / stats['total_candidates'] * 100
        print(f"  {skill}: {count} times ({pct:.1f}%)")


if __name__ == "__main__":
    print("\n" + "="*80)
    print("🚀 Enhanced Feature Engineer - Skill Inference Tests")
    print("="*80)
    
    # Run all tests
    test_forward_inference()
    test_reverse_inference_ml()
    test_reverse_inference_backend()
    test_skip_reverse_with_explicit_framework()
    test_before_after_comparison()
    test_real_dataset()
    
    print("\n" + "="*80)
    print("✅ All tests completed!")
    print("="*80)
