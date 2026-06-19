"""
Quick Start Script for Candidate Clustering System
Demonstrates Phase 1: Skill Normalization
"""

import sys
from pathlib import Path

# Add src to path
sys.path.insert(0, str(Path(__file__).parent / "src"))

from candidate_clustering.skills.normalizer import SkillNormalizer
from candidate_clustering.data.sample_generator import SampleDataGenerator
from collections import Counter

def main():
    print("=" * 70)
    print("Candidate Clustering System - Phase 1 Demo")
    print("Skill Normalization Pipeline")
    print("=" * 70)

    # Step 1: Generate sample data
    print("\n[Step 1] Generating sample candidate data...")
    df = SampleDataGenerator.generate_candidates(n_candidates=100, seed=42)
    print(f"✓ Generated {len(df)} candidates")
    print(f"  Profile types: {dict(df['profile_type'].value_counts())}")

    # Step 2: Initialize normalizer
    print("\n[Step 2] Initializing skill normalizer...")
    normalizer = SkillNormalizer()
    print("✓ Normalizer initialized with 3-tier pipeline")

    # Step 3: Collect all unique skills
    print("\n[Step 3] Analyzing skills...")
    all_skills = [skill for skills_list in df['skills'] for skill in skills_list]
    unique_skills = list(set(all_skills))
    print(f"✓ Found {len(unique_skills)} unique raw skills")

    # Step 4: Apply Tier 1 normalization
    print("\n[Step 4] Applying Tier 1: Rule-based normalization...")
    print("  Examples:")
    test_skills = ["Python3.9", "JS", "K8s", "Docker Container", "pyton", "React.js"]
    for skill in test_skills:
        normalized = normalizer.normalize_tier1(skill)
        print(f"    {skill:25} → {normalized}")

    # Step 5: Apply full batch normalization
    print("\n[Step 5] Applying full normalization to all skills...")
    print("  (This includes Tier 2: Embedding-based clustering)")
    print("  This may take 30-60 seconds...")

    results = normalizer.normalize_batch(unique_skills, use_tier2=True)

    canonical_skills = set([canonical for canonical, conf in results.values()])
    print(f"\n✓ Normalization complete!")
    print(f"  Original skills: {len(unique_skills)}")
    print(f"  Canonical skills: {len(canonical_skills)}")
    print(f"  Reduction: {len(unique_skills) - len(canonical_skills)} skills")
    print(f"  Compression ratio: {len(canonical_skills) / len(unique_skills):.1%}")

    # Step 6: Show some mappings
    print("\n[Step 6] Sample skill mappings:")
    print("  " + "-" * 66)
    sample_mappings = [(raw, canonical, conf)
                       for raw, (canonical, conf) in list(results.items())[:15]
                       if raw.lower() != canonical.lower()]
    for raw, canonical, conf in sample_mappings:
        print(f"  {raw:25} → {canonical:25} ({conf:.2f})")

    # Step 7: Build co-occurrence graph
    print("\n[Step 7] Building skill co-occurrence graph...")
    candidate_skills = df['skills'].tolist()
    normalizer.build_cooccurrence_graph(candidate_skills, min_cooccurrence=3)
    stats = normalizer.get_statistics()
    print(f"✓ Graph built with {stats['graph_nodes']} nodes and {stats['graph_edges']} edges")

    # Step 8: Apply to dataset
    print("\n[Step 8] Applying normalization to candidate dataset...")
    def normalize_skills(skills_list):
        normalized = []
        for skill in skills_list:
            canonical, _ = results.get(skill, (skill, 1.0))
            normalized.append(canonical)
        return list(set(normalized))

    df['normalized_skills'] = df['skills'].apply(normalize_skills)
    print("✓ Dataset updated with normalized skills")

    # Step 9: Show most common canonical skills
    print("\n[Step 9] Top 10 most common canonical skills:")
    all_normalized = [skill for skills_list in df['normalized_skills'] for skill in skills_list]
    print("  " + "-" * 66)
    for skill, count in Counter(all_normalized).most_common(10):
        print(f"  {skill:40} : {count:3d} occurrences")

    # Step 10: Save results
    print("\n[Step 10] Saving results...")
    output_dir = Path("data/processed")
    output_dir.mkdir(parents=True, exist_ok=True)

    mapping_path = output_dir / "skill_mappings.json"
    normalizer.save_mappings(str(mapping_path))
    print(f"✓ Skill mappings saved to: {mapping_path}")

    data_path = output_dir / "candidates_normalized.pkl"
    df.to_pickle(str(data_path))
    print(f"✓ Normalized data saved to: {data_path}")

    # Summary
    print("\n" + "=" * 70)
    print("Phase 1: Skill Normalization - COMPLETE ✓")
    print("=" * 70)
    print("\nKey Results:")
    print(f"  • Processed {len(df)} candidates")
    print(f"  • Normalized {len(unique_skills)} → {len(canonical_skills)} skills")
    print(f"  • Built co-occurrence graph with {stats['graph_nodes']} nodes")
    print(f"  • Average confidence: {sum(c for _, c in results.values()) / len(results):.2f}")
    print("\nNext Steps:")
    print("  • Run: jupyter notebook notebooks/01_skill_normalization_demo.ipynb")
    print("  • Or proceed to Phase 2: Feature Engineering")
    print("=" * 70)


if __name__ == "__main__":
    main()
