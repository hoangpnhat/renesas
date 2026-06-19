"""
Demo: Score Handling in Phase 1
Shows how to normalize skills AND scores together
"""

import sys
from pathlib import Path
import json

# Add src to path
sys.path.insert(0, str(Path(__file__).parent / "src"))

from candidate_clustering.skills.normalizer import SkillNormalizer


def demo_1_basic_score_normalization():
    """Demo 1: Basic score normalization"""
    print("\n" + "=" * 70)
    print("Demo 1: Basic Score Normalization")
    print("=" * 70)

    normalizer = SkillNormalizer()

    # Example: Candidate with duplicate skills (different names, different scores)
    skills = ["Python3.9", "Python Programming", "JS", "React.js"]
    scores = {
        "Python3.9": 85,
        "Python Programming": 90,  # Higher Python score
        "JS": 88,
        "React.js": 92,
    }

    print("\nInput:")
    print(f"  Skills: {skills}")
    print(f"  Scores: {json.dumps(scores, indent=10)}")

    # Normalize with different aggregation methods
    print("\n" + "-" * 70)
    print("Different Aggregation Methods:")
    print("-" * 70)

    for method in ["max", "mean", "weighted_mean"]:
        result = normalizer.normalize_skill_scores(
            skills=skills, scores=scores, aggregation=method, use_tier2=False
        )
        print(f"\n{method.upper()} aggregation:")
        for skill, score in sorted(result.items()):
            print(f"  {skill:20} : {score:.2f}")


def demo_2_normalize_single_candidate():
    """Demo 2: Normalize a complete candidate"""
    print("\n" + "=" * 70)
    print("Demo 2: Normalize Complete Candidate")
    print("=" * 70)

    normalizer = SkillNormalizer()

    # Realistic candidate with noisy skills
    candidate = {
        "candidate_id": "C001",
        "name": "John Doe",
        "skills": [
            "Python3.9",
            "Python Programming",  # Duplicate
            "JS",
            "JavaScript",  # Duplicate
            "React.js",
            "Docker Container",
            "K8s",
            "Postgre",  # Typo
        ],
        "skill_scores": {
            "Python3.9": 85,
            "Python Programming": 90,
            "JS": 88,
            "JavaScript": 92,
            "React.js": 95,
            "Docker Container": 78,
            "K8s": 65,
            "Postgre": 82,
        },
    }

    print(f"\nCandidate: {candidate['candidate_id']}")
    print(f"Raw skills ({len(candidate['skills'])}): {candidate['skills']}")
    print(f"Raw scores: {json.dumps(candidate['skill_scores'], indent=2)}")

    # Normalize
    result = normalizer.normalize_candidate(
        candidate_skills=candidate["skills"],
        candidate_scores=candidate["skill_scores"],
        aggregation="max",
        use_tier2=False,
        include_metadata=True,
    )

    print("\n" + "-" * 70)
    print("After Normalization:")
    print("-" * 70)
    print(f"Normalized skills ({len(result['normalized_skills'])}): {result['normalized_skills']}")
    print("\nNormalized scores:")
    for skill, score in sorted(result["normalized_scores"].items()):
        print(f"  {skill:20} : {score:.1f}")

    # Show metadata
    print("\n" + "-" * 70)
    print("Metadata:")
    print("-" * 70)
    metadata = result["metadata"]
    print(f"  Original skill count: {metadata['original_skill_count']}")
    print(f"  Normalized skill count: {metadata['normalized_skill_count']}")
    print(f"  Reduction: {metadata['reduction']} skills")

    print("\nSkill Mappings:")
    for canonical, mappings in metadata["skill_mappings"].items():
        print(f"\n  {canonical}:")
        for mapping in mappings:
            print(
                f"    ← {mapping['raw_skill']:25} "
                f"(score: {mapping['raw_score']:5.1f}, "
                f"confidence: {mapping['confidence']:.2f})"
            )


def demo_3_normalize_dataset():
    """Demo 3: Normalize entire dataset"""
    print("\n" + "=" * 70)
    print("Demo 3: Normalize Entire Dataset")
    print("=" * 70)

    normalizer = SkillNormalizer()

    # Sample dataset
    candidates = [
        {
            "candidate_id": "C001",
            "profile": "Backend Developer",
            "skills": ["Python3.9", "Docker", "PostgreSQL"],
            "skill_scores": {"Python3.9": 85, "Docker": 78, "PostgreSQL": 82},
        },
        {
            "candidate_id": "C002",
            "profile": "Frontend Developer",
            "skills": ["JavaScript", "React.js", "HTML", "CSS"],
            "skill_scores": {"JavaScript": 90, "React.js": 88, "HTML": 95, "CSS": 92},
        },
        {
            "candidate_id": "C003",
            "profile": "Full Stack Developer",
            "skills": ["Python Programming", "JS", "Docker Container", "Postgre"],
            "skill_scores": {
                "Python Programming": 92,
                "JS": 85,
                "Docker Container": 75,
                "Postgre": 70,
            },
        },
        {
            "candidate_id": "C004",
            "profile": "ML Engineer",
            "skills": ["Python3", "Machine Learning", "TensorFlow", "pandas"],
            "skill_scores": {"Python3": 88, "Machine Learning": 85, "TensorFlow": 80, "pandas": 90},
        },
    ]

    print(f"\nDataset: {len(candidates)} candidates")

    # Normalize
    normalized = normalizer.normalize_dataset(
        candidates, aggregation="max", use_tier2=False
    )

    print("\n" + "-" * 70)
    print("Normalized Results:")
    print("-" * 70)

    for candidate in normalized:
        print(f"\n{candidate['candidate_id']} ({candidate['profile']}):")
        print(f"  Raw skills: {candidate['skills']}")
        print(f"  Normalized: {candidate['normalized_skills']}")
        print(f"  Scores: {candidate['normalized_scores']}")


def demo_4_aggregation_comparison():
    """Demo 4: Compare different aggregation methods"""
    print("\n" + "=" * 70)
    print("Demo 4: Aggregation Method Comparison")
    print("=" * 70)

    normalizer = SkillNormalizer()

    # Candidate with skill variations and different scores
    skills = ["Python", "Python3", "Python3.9", "Python Programming"]
    scores = {"Python": 70, "Python3": 80, "Python3.9": 90, "Python Programming": 85}

    print("\nSkills with different scores:")
    for skill, score in scores.items():
        print(f"  {skill:25} : {score}")

    print("\n" + "-" * 70)
    print("Comparison of Aggregation Methods:")
    print("-" * 70)

    methods = ["max", "mean", "weighted_mean", "first"]

    results = {}
    for method in methods:
        result = normalizer.normalize_skill_scores(
            skills=skills, scores=scores, aggregation=method, use_tier2=False
        )
        results[method] = result.get("python", 0)

    print(f"\n{'Method':<20} {'Python Score':<15} {'Explanation'}")
    print("-" * 70)
    print(f"{'max':<20} {results['max']:<15.1f} Takes highest score (best evidence)")
    print(f"{'mean':<20} {results['mean']:<15.1f} Average of all scores")
    print(
        f"{'weighted_mean':<20} {results['weighted_mean']:<15.1f} Weighted by mapping confidence"
    )
    print(f"{'first':<20} {results['first']:<15.1f} First occurrence only")

    print("\nRecommendation:")
    print("  • Use 'max' for most cases (assumes best evidence)")
    print("  • Use 'mean' if you want conservative estimates")
    print("  • Use 'weighted_mean' to account for confidence in mappings")


def demo_5_real_world_scenario():
    """Demo 5: Real-world scenario with complex data"""
    print("\n" + "=" * 70)
    print("Demo 5: Real-World Scenario")
    print("=" * 70)

    normalizer = SkillNormalizer()

    # Realistic scenario: Resume parser extracted these skills with LLM
    candidate = {
        "candidate_id": "C999",
        "name": "Alice Johnson",
        "experience_years": 5,
        "skills": [
            # Programming Languages (with variations)
            "Python3.9",
            "Python Programming",
            "JavaScript",
            "JS",
            "TypeScript",
            # Frameworks
            "React.js",
            "ReactJS",
            "Next.js",
            "Node.js",
            # Databases
            "PostgreSQL",
            "Postgre",
            "MongoDB",
            "Mongo",
            # DevOps
            "Docker",
            "Docker Container",
            "K8s",
            "Kubernetes",
            # Cloud
            "AWS",
            "Amazon Web Services",
        ],
        "skill_scores": {
            "Python3.9": 85,
            "Python Programming": 90,
            "JavaScript": 88,
            "JS": 92,
            "TypeScript": 85,
            "React.js": 95,
            "ReactJS": 93,
            "Next.js": 88,
            "Node.js": 86,
            "PostgreSQL": 82,
            "Postgre": 80,
            "MongoDB": 75,
            "Mongo": 78,
            "Docker": 80,
            "Docker Container": 85,
            "K8s": 65,
            "Kubernetes": 70,
            "AWS": 88,
            "Amazon Web Services": 90,
        },
    }

    print(f"\nCandidate: {candidate['name']} ({candidate['experience_years']} years exp)")
    print(f"Raw skills extracted by LLM: {len(candidate['skills'])} skills")

    # Normalize
    result = normalizer.normalize_candidate(
        candidate_skills=candidate["skills"],
        candidate_scores=candidate["skill_scores"],
        aggregation="max",
        use_tier2=False,
        include_metadata=True,
    )

    print("\n" + "-" * 70)
    print("After Normalization:")
    print("-" * 70)
    print(f"Unique skills: {len(result['normalized_skills'])} skills")
    print(
        f"Reduction: {result['metadata']['reduction']} duplicate/variant skills removed"
    )

    # Group by domain for better visualization
    domains = {
        "Programming": ["python", "javascript", "typescript"],
        "Frontend": ["react", "next.js"],
        "Backend": ["node.js"],
        "Database": ["postgresql", "mongodb"],
        "DevOps": ["docker", "kubernetes"],
        "Cloud": ["aws", "amazon web services"],
    }

    print("\nSkills by Domain:")
    for domain, domain_skills in domains.items():
        print(f"\n  {domain}:")
        for skill in domain_skills:
            if skill in result["normalized_scores"]:
                score = result["normalized_scores"][skill]
                bar = "█" * int(score / 10)
                print(f"    {skill:20} : {score:5.1f}  {bar}")

    # Calculate domain averages
    print("\n" + "-" * 70)
    print("Domain Averages:")
    print("-" * 70)
    for domain, domain_skills in domains.items():
        scores = [
            result["normalized_scores"][s]
            for s in domain_skills
            if s in result["normalized_scores"]
        ]
        if scores:
            avg = sum(scores) / len(scores)
            print(f"  {domain:20} : {avg:5.1f}")

    print("\n" + "-" * 70)
    print("Summary:")
    print("-" * 70)
    print(f"  • Original: {len(candidate['skills'])} skills")
    print(f"  • Normalized: {len(result['normalized_skills'])} unique skills")
    print(f"  • Compression: {len(result['normalized_skills']) / len(candidate['skills']):.1%}")
    print(f"  • Top skills: {sorted(result['normalized_scores'].items(), key=lambda x: x[1], reverse=True)[:3]}")


def main():
    """Run all demos"""
    print("\n" + "=" * 70)
    print("SCORE HANDLING DEMOS - Phase 1")
    print("=" * 70)

    demo_1_basic_score_normalization()
    demo_2_normalize_single_candidate()
    demo_3_normalize_dataset()
    demo_4_aggregation_comparison()
    demo_5_real_world_scenario()

    print("\n" + "=" * 70)
    print("All demos complete!")
    print("=" * 70)
    print("\nKey Takeaways:")
    print("  ✓ Skills and scores are normalized together")
    print("  ✓ Duplicate skills are aggregated (max, mean, weighted_mean)")
    print("  ✓ Metadata tracks all mappings and transformations")
    print("  ✓ Dataset-level normalization is efficient (batch processing)")
    print("\nNext: Run tests with 'pytest tests/test_score_handling.py -v'")
    print("=" * 70 + "\n")


if __name__ == "__main__":
    main()
