"""
Example: Using Skill Normalizer with Your Own Data

This script shows how to integrate the normalizer with real candidate data.
"""

import sys
from pathlib import Path
import pandas as pd

# Add src to path
sys.path.insert(0, str(Path(__file__).parent / "src"))

from candidate_clustering.skills.normalizer import SkillNormalizer


def example_1_simple_normalization():
    """Example 1: Simple skill normalization"""
    print("\n" + "="*70)
    print("Example 1: Simple Skill Normalization")
    print("="*70)

    # Initialize normalizer
    normalizer = SkillNormalizer()

    # Normalize individual skills
    skills_to_normalize = [
        "Python3.9",
        "JavaScript",
        "K8s",
        "Docker Container",
        "React.js",
        "pyton",  # typo
        "ML",
        "PostgreSQL"
    ]

    print("\nNormalizing skills:")
    print("-" * 70)
    for skill in skills_to_normalize:
        canonical, confidence = normalizer.normalize(skill, use_tier2=False)
        print(f"{skill:25} → {canonical:25} (confidence: {confidence:.2f})")


def example_2_batch_processing():
    """Example 2: Batch processing with embeddings"""
    print("\n" + "="*70)
    print("Example 2: Batch Processing with Semantic Clustering")
    print("="*70)

    normalizer = SkillNormalizer()

    # Simulate a list of skills from multiple candidates
    all_skills = [
        "Python3.9", "Python", "Python Programming",  # Should cluster together
        "JavaScript", "JS", "javascript",             # Should cluster together
        "React", "React.js", "ReactJS",               # Should cluster together
        "Docker", "Docker Container",                 # Should cluster together
        "K8s", "Kubernetes",                          # Should cluster together
        "SQL", "PostgreSQL", "Postgres"               # Related but different
    ]

    print("\nProcessing batch with Tier 2 (embeddings)...")
    results = normalizer.normalize_batch(all_skills, use_tier2=True)

    print("\nResults grouped by canonical skill:")
    print("-" * 70)

    # Group by canonical
    from collections import defaultdict
    grouped = defaultdict(list)
    for original, (canonical, conf) in results.items():
        grouped[canonical].append((original, conf))

    for canonical, skills in sorted(grouped.items()):
        print(f"\nCanonical: {canonical}")
        for skill, conf in skills:
            print(f"  ← {skill:30} (confidence: {conf:.3f})")


def example_3_real_data_format():
    """Example 3: Processing data in typical format"""
    print("\n" + "="*70)
    print("Example 3: Processing Candidate Data")
    print("="*70)

    # Sample data structure (replace with your actual data)
    candidates_data = [
        {
            "candidate_id": "C001",
            "skills": ["Python3.9", "Docker", "K8s", "PostgreSQL"],
            "skill_scores": {"Python3.9": 85, "Docker": 78, "K8s": 65, "PostgreSQL": 82}
        },
        {
            "candidate_id": "C002",
            "skills": ["JavaScript", "React.js", "Node.js", "MongoDB"],
            "skill_scores": {"JavaScript": 90, "React.js": 88, "Node.js": 75, "MongoDB": 70}
        },
        {
            "candidate_id": "C003",
            "skills": ["Python Programming", "Machine Learning", "TensorFlow", "pandas"],
            "skill_scores": {"Python Programming": 92, "Machine Learning": 85, "TensorFlow": 80, "pandas": 88}
        }
    ]

    df = pd.DataFrame(candidates_data)

    # Initialize normalizer
    normalizer = SkillNormalizer()

    # Collect all unique skills
    all_skills = []
    for skills_list in df['skills']:
        all_skills.extend(skills_list)
    unique_skills = list(set(all_skills))

    print(f"\nProcessing {len(unique_skills)} unique skills from {len(df)} candidates...")

    # Normalize all skills
    skill_mappings = normalizer.normalize_batch(unique_skills, use_tier2=True)

    # Apply normalization to dataset
    def normalize_candidate_skills(skills_list):
        normalized = []
        for skill in skills_list:
            canonical, _ = skill_mappings.get(skill, (skill, 1.0))
            normalized.append(canonical)
        return list(set(normalized))  # Remove duplicates

    df['normalized_skills'] = df['skills'].apply(normalize_candidate_skills)

    # Also normalize the skill scores dictionary
    def normalize_skill_scores(skills_list, scores_dict):
        normalized_scores = {}
        for skill in skills_list:
            canonical, _ = skill_mappings.get(skill, (skill, 1.0))
            if skill in scores_dict:
                # If multiple raw skills map to same canonical, take max score
                normalized_scores[canonical] = max(
                    normalized_scores.get(canonical, 0),
                    scores_dict[skill]
                )
        return normalized_scores

    df['normalized_scores'] = df.apply(
        lambda row: normalize_skill_scores(row['skills'], row['skill_scores']),
        axis=1
    )

    # Display results
    print("\n" + "-" * 70)
    for idx, row in df.iterrows():
        print(f"\nCandidate: {row['candidate_id']}")
        print(f"  Original skills: {row['skills']}")
        print(f"  Normalized: {row['normalized_skills']}")
        print(f"  Normalized scores: {row['normalized_scores']}")


def example_4_save_and_reuse():
    """Example 4: Save mappings and reuse"""
    print("\n" + "="*70)
    print("Example 4: Save and Reuse Mappings")
    print("="*70)

    # First session: Build mappings
    print("\n[Session 1] Building and saving mappings...")
    normalizer1 = SkillNormalizer()

    skills = ["Python3.9", "JS", "K8s", "React.js", "Docker Container"]
    mappings = normalizer1.normalize_batch(skills, use_tier2=True)

    # Save to file
    output_dir = Path("data/processed")
    output_dir.mkdir(parents=True, exist_ok=True)
    mapping_file = output_dir / "my_skill_mappings.json"

    normalizer1.save_mappings(str(mapping_file))
    print(f"✓ Saved mappings to {mapping_file}")

    # Second session: Load and reuse
    print("\n[Session 2] Loading and reusing mappings...")
    normalizer2 = SkillNormalizer()
    normalizer2.load_mappings(str(mapping_file))
    print(f"✓ Loaded mappings from {mapping_file}")

    # Use loaded mappings
    new_skills = ["Python", "JavaScript", "Kubernetes"]
    for skill in new_skills:
        canonical, conf = normalizer2.normalize(skill, use_tier2=True)
        print(f"  {skill:20} → {canonical:20} ({conf:.2f})")


def example_5_with_cooccurrence():
    """Example 5: Using co-occurrence graph for disambiguation"""
    print("\n" + "="*70)
    print("Example 5: Context-Aware Disambiguation")
    print("="*70)

    normalizer = SkillNormalizer()

    # Simulate candidate data with skill co-occurrences
    candidates_skills = [
        ["python", "tensorflow", "machine learning", "pandas", "numpy"],
        ["python", "pytorch", "deep learning", "scikit-learn"],
        ["python", "flask", "docker", "postgresql", "redis"],
        ["javascript", "react", "node.js", "html", "css"],
        ["javascript", "vue", "typescript", "webpack"],
    ]

    print("\nBuilding co-occurrence graph...")
    normalizer.build_cooccurrence_graph(candidates_skills, min_cooccurrence=2)

    stats = normalizer.get_statistics()
    print(f"✓ Graph built with {stats['graph_nodes']} nodes and {stats['graph_edges']} edges")

    # Now disambiguate ambiguous skills based on context
    print("\nDisambiguating 'ml' with different contexts:")

    context1 = ["python", "tensorflow", "pandas"]  # ML/AI context
    result1 = normalizer.normalize_tier3("ml", context1)
    print(f"  'ml' with {context1} → {result1}")

    context2 = ["html", "css", "xml"]  # Web context
    result2 = normalizer.normalize_tier3("ml", context2)
    print(f"  'ml' with {context2} → {result2}")


def main():
    """Run all examples"""
    print("\n" + "="*70)
    print("CANDIDATE CLUSTERING - SKILL NORMALIZATION EXAMPLES")
    print("="*70)

    # Run examples
    example_1_simple_normalization()
    example_2_batch_processing()
    example_3_real_data_format()
    example_4_save_and_reuse()
    example_5_with_cooccurrence()

    print("\n" + "="*70)
    print("All examples complete!")
    print("="*70)
    print("\nNext steps:")
    print("  • Adapt these examples to your own data format")
    print("  • Customize normalization rules in config/normalization_rules.json")
    print("  • Run jupyter notebook notebooks/01_skill_normalization_demo.ipynb")
    print("="*70 + "\n")


if __name__ == "__main__":
    main()
