"""
Sample Data Generator for Testing

Generates synthetic candidate data with noisy skill names.
"""

import random
from typing import List, Dict
import pandas as pd


class SampleDataGenerator:
    """Generate synthetic candidate data with noisy skills."""

    # Base canonical skills
    CANONICAL_SKILLS = {
        "Backend": [
            "python",
            "java",
            "node.js",
            "go",
            "rust",
            "ruby",
            "php",
            "c++",
            "sql",
            "postgresql",
            "mongodb",
            "redis",
            "docker",
            "kubernetes",
        ],
        "Frontend": [
            "javascript",
            "typescript",
            "react",
            "vue",
            "angular",
            "html",
            "css",
            "webpack",
            "next.js",
            "svelte",
        ],
        "Data/ML": [
            "python",
            "machine learning",
            "deep learning",
            "tensorflow",
            "pytorch",
            "scikit-learn",
            "pandas",
            "numpy",
            "data analysis",
            "sql",
        ],
        "DevOps": [
            "docker",
            "kubernetes",
            "aws",
            "google cloud platform",
            "jenkins",
            "git",
            "terraform",
            "ansible",
            "linux",
            "bash",
        ],
    }

    # Noisy variations for each skill
    SKILL_VARIATIONS = {
        "python": ["Python", "python", "Python3", "Python 3.9", "Python Programming", "py", "pyton"],
        "javascript": ["JavaScript", "javascript", "JS", "js", "JavaScirpt", "Java Script"],
        "typescript": ["TypeScript", "typescript", "TS", "ts", "Type Script"],
        "react": ["React", "react", "ReactJS", "React.js", "react.js"],
        "node.js": ["Node.js", "node.js", "NodeJS", "Node", "nodejs"],
        "docker": ["Docker", "docker", "Docker Container", "Containerization", "Docker Engine"],
        "kubernetes": ["Kubernetes", "kubernetes", "K8s", "k8s", "Kubenetes"],
        "postgresql": ["PostgreSQL", "postgresql", "Postgres", "postgres", "Postgre"],
        "mongodb": ["MongoDB", "mongodb", "Mongo", "mongo", "Mongo DB"],
        "machine learning": [
            "Machine Learning",
            "machine learning",
            "ML",
            "ml",
            "Machine Learning Modeling",
            "ML Modeling",
        ],
        "tensorflow": ["TensorFlow", "tensorflow", "TF", "tf", "Tensor Flow"],
        "aws": ["AWS", "aws", "Amazon Web Services", "Amazon AWS"],
        "google cloud platform": ["Google Cloud Platform", "GCP", "gcp", "Google Cloud"],
        "next.js": ["Next.js", "next.js", "NextJS", "Next"],
        "sql": ["SQL", "sql", "Structured Query Language"],
        "git": ["Git", "git", "GIT"],
    }

    @classmethod
    def generate_candidates(cls, n_candidates: int = 100, seed: int = 42) -> pd.DataFrame:
        """
        Generate synthetic candidate data.

        Args:
            n_candidates: Number of candidates to generate
            seed: Random seed for reproducibility

        Returns:
            DataFrame with candidate_id, skills, skill_scores
        """
        random.seed(seed)

        candidates = []

        for i in range(n_candidates):
            # Randomly select a profile type
            profile_type = random.choice(list(cls.CANONICAL_SKILLS.keys()))

            # Select 5-12 skills from that domain
            domain_skills = cls.CANONICAL_SKILLS[profile_type]
            n_skills = random.randint(5, min(12, len(domain_skills)))
            selected_canonical = random.sample(domain_skills, n_skills)

            # Add some cross-domain skills
            if random.random() > 0.5:
                other_domains = [d for d in cls.CANONICAL_SKILLS.keys() if d != profile_type]
                other_domain = random.choice(other_domains)
                selected_canonical += random.sample(cls.CANONICAL_SKILLS[other_domain], 2)

            # Apply noisy variations
            noisy_skills = []
            for canonical_skill in selected_canonical:
                if canonical_skill in cls.SKILL_VARIATIONS:
                    variations = cls.SKILL_VARIATIONS[canonical_skill]
                    noisy_skill = random.choice(variations)
                else:
                    # Keep as-is if no variations defined
                    noisy_skill = canonical_skill

                noisy_skills.append(noisy_skill)

            # Generate skill scores (0-100)
            scores = {
                skill: round(random.uniform(30, 95), 2) for skill in noisy_skills
            }

            candidates.append(
                {
                    "candidate_id": f"C{i:04d}",
                    "profile_type": profile_type,
                    "skills": noisy_skills,
                    "skill_scores": scores,
                }
            )

        return pd.DataFrame(candidates)

    @classmethod
    def save_sample_data(cls, output_path: str, n_candidates: int = 100):
        """Generate and save sample data to CSV."""
        df = cls.generate_candidates(n_candidates)

        # Convert lists/dicts to strings for CSV storage
        df["skills_str"] = df["skills"].apply(lambda x: "|".join(x))
        df["scores_str"] = df["skill_scores"].apply(
            lambda x: "|".join([f"{k}:{v}" for k, v in x.items()])
        )

        df[["candidate_id", "profile_type", "skills_str", "scores_str"]].to_csv(
            output_path, index=False
        )

        print(f"Saved {len(df)} candidates to {output_path}")

    @classmethod
    def load_sample_data(cls, input_path: str) -> pd.DataFrame:
        """Load sample data from CSV."""
        df = pd.read_csv(input_path)

        # Convert strings back to lists/dicts
        df["skills"] = df["skills_str"].apply(lambda x: x.split("|"))
        df["skill_scores"] = df["scores_str"].apply(
            lambda x: dict(
                [pair.split(":")[0], float(pair.split(":")[1])]
                for pair in x.split("|")
            )
        )

        df = df.drop(columns=["skills_str", "scores_str"])

        return df
