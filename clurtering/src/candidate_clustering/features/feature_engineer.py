"""
Feature Engineering Module - Phase 2

Extracts comprehensive features from normalized candidate data including:
- Core skill features
- Inferred skill scores
- Domain aggregations
- Weakness modeling
- Profile characteristics
"""

from typing import Dict, List, Tuple, Optional, Set
from collections import defaultdict
import numpy as np
import json
from pathlib import Path


class FeatureEngineer:
    """
    Feature engineering for candidate clustering.

    Generates ~600 dimensional feature vectors from normalized skills and scores,
    including weakness modeling and domain aggregations.
    """

    # Domain mapping: skill → domain(s)
    SKILL_TO_DOMAIN = {
        # Backend
        "python": ["Backend", "Data/ML"],
        "java": ["Backend"],
        "node.js": ["Backend"],
        "nodejs": ["Backend"],
        "go": ["Backend"],
        "golang": ["Backend"],
        "ruby": ["Backend"],
        "php": ["Backend"],
        "c#": ["Backend"],
        "csharp": ["Backend"],
        "scala": ["Backend"],
        "fastapi": ["Backend"],
        "django": ["Backend"],
        "flask": ["Backend"],
        "spring": ["Backend"],
        "express": ["Backend"],
        "express.js": ["Backend"],

        # Frontend
        "javascript": ["Frontend"],
        "typescript": ["Frontend"],
        "react": ["Frontend"],
        "react.js": ["Frontend"],
        "vue": ["Frontend"],
        "vue.js": ["Frontend"],
        "angular": ["Frontend"],
        "next.js": ["Frontend"],
        "nextjs": ["Frontend"],
        "svelte": ["Frontend"],
        "html": ["Frontend"],
        "css": ["Frontend"],
        "sass": ["Frontend"],
        "tailwind": ["Frontend"],
        "tailwindcss": ["Frontend"],
        "webpack": ["Frontend"],
        "redux": ["Frontend"],

        # Database
        "postgresql": ["Database"],
        "postgres": ["Database"],
        "mysql": ["Database"],
        "mongodb": ["Database"],
        "mongo": ["Database"],
        "redis": ["Database"],
        "elasticsearch": ["Database"],
        "cassandra": ["Database"],
        "dynamodb": ["Database"],
        "sql": ["Database"],
        "nosql": ["Database"],
        "data modeling": ["Database"],

        # DevOps
        "docker": ["DevOps"],
        "kubernetes": ["DevOps"],
        "k8s": ["DevOps"],
        "jenkins": ["DevOps"],
        "gitlab ci": ["DevOps"],
        "github actions": ["DevOps"],
        "terraform": ["DevOps"],
        "ansible": ["DevOps"],
        "ci/cd": ["DevOps"],
        "linux": ["DevOps"],
        "bash": ["DevOps"],
        "shell scripting": ["DevOps"],
        "monitoring": ["DevOps"],
        "prometheus": ["DevOps"],
        "grafana": ["DevOps"],

        # Cloud
        "aws": ["Cloud"],
        "amazon web services": ["Cloud"],
        "azure": ["Cloud"],
        "gcp": ["Cloud"],
        "google cloud": ["Cloud"],
        "lambda": ["Cloud"],
        "s3": ["Cloud"],
        "ec2": ["Cloud"],
        "cloudformation": ["Cloud"],

        # Data/ML
        "machine learning": ["Data/ML"],
        "deep learning": ["Data/ML"],
        "tensorflow": ["Data/ML"],
        "pytorch": ["Data/ML"],
        "pandas": ["Data/ML"],
        "numpy": ["Data/ML"],
        "scikit-learn": ["Data/ML"],
        "sklearn": ["Data/ML"],
        "data analysis": ["Data/ML"],
        "statistics": ["Data/ML"],
        "nlp": ["Data/ML"],
        "computer vision": ["Data/ML"],
        "jupyter": ["Data/ML"],

        # Testing
        "testing": ["Testing"],
        "unit testing": ["Testing"],
        "pytest": ["Testing"],
        "jest": ["Testing"],
        "selenium": ["Testing"],
        "test automation": ["Testing"],

        # Other
        "git": ["Tools"],
        "github": ["Tools"],
        "gitlab": ["Tools"],
        "agile": ["Soft Skills"],
        "scrum": ["Soft Skills"],
        "system design": ["Architecture"],
        "microservices": ["Architecture"],
        "rest api": ["Backend"],
        "restful": ["Backend"],
        "graphql": ["Backend"],
    }

    # Skill inference rules: skill → (inferred_skill, confidence)
    SKILL_INFERENCE_RULES = {
        # Framework implies language
        "react": [("javascript", 0.95)],
        "react.js": [("javascript", 0.95)],
        "vue": [("javascript", 0.95)],
        "vue.js": [("javascript", 0.95)],
        "angular": [("javascript", 0.95), ("typescript", 0.8)],
        "next.js": [("javascript", 0.95), ("react", 0.9)],
        "django": [("python", 0.95)],
        "flask": [("python", 0.95)],
        "fastapi": [("python", 0.95)],
        "spring": [("java", 0.95)],
        "express": [("node.js", 0.95), ("javascript", 0.9)],
        "express.js": [("node.js", 0.95), ("javascript", 0.9)],

        # DevOps implies base skills
        "docker": [("linux", 0.8)],
        "kubernetes": [("docker", 0.85), ("linux", 0.75), ("yaml", 0.8)],
        "k8s": [("docker", 0.85), ("linux", 0.75), ("yaml", 0.8)],
        "terraform": [("infrastructure as code", 0.9)],
        "ansible": [("linux", 0.8), ("yaml", 0.8)],

        # ML implies data skills
        "tensorflow": [("python", 0.95), ("machine learning", 0.9)],
        "pytorch": [("python", 0.95), ("machine learning", 0.9)],
        "scikit-learn": [("python", 0.95), ("machine learning", 0.85)],
        "pandas": [("python", 0.95), ("data analysis", 0.85)],
        "numpy": [("python", 0.95)],

        # Testing frameworks
        "pytest": [("python", 0.95), ("testing", 0.9)],
        "jest": [("javascript", 0.95), ("testing", 0.9)],
        "selenium": [("testing", 0.9), ("test automation", 0.85)],

        # Cloud implies base knowledge
        "aws": [("cloud computing", 0.9)],
        "azure": [("cloud computing", 0.9)],
        "gcp": [("cloud computing", 0.9)],

        # Architecture patterns
        "microservices": [("rest api", 0.8), ("system design", 0.85)],
        "graphql": [("rest api", 0.7)],
    }

    # Define all possible domains
    ALL_DOMAINS = [
        "Backend",
        "Frontend",
        "Database",
        "DevOps",
        "Cloud",
        "Data/ML",
        "Testing",
        "Architecture",
        "Tools",
        "Soft Skills"
    ]

    def __init__(self, config_path: Optional[str] = None):
        """
        Initialize Feature Engineer.

        Args:
            config_path: Optional path to custom configuration file
        """
        self.config = self._load_config(config_path) if config_path else {}

        # Statistics for weakness modeling (computed from dataset)
        self.skill_percentiles = {}  # skill → percentile thresholds
        self.domain_avg_scores = {}  # domain → average score
        self.global_avg_score = None

    def _load_config(self, config_path: str) -> dict:
        """Load configuration from file."""
        with open(config_path, 'r') as f:
            return json.load(f)

    def fit(self, candidates: List[Dict]):
        """
        Fit the feature engineer to a dataset of candidates.

        Computes dataset-level statistics needed for weakness modeling
        and relative feature calculation.

        Args:
            candidates: List of candidates with normalized_skills and normalized_scores
        """
        # Collect all skill scores across candidates
        skill_scores_list = defaultdict(list)
        domain_scores_list = defaultdict(list)
        all_scores = []

        for candidate in candidates:
            scores = candidate.get('normalized_scores', {})

            for skill, score in scores.items():
                skill_scores_list[skill].append(score)
                all_scores.append(score)

                # Add to domain scores
                domains = self.SKILL_TO_DOMAIN.get(skill, [])
                for domain in domains:
                    domain_scores_list[domain].append(score)

        # Compute percentiles for each skill
        for skill, scores in skill_scores_list.items():
            scores_array = np.array(scores)
            self.skill_percentiles[skill] = {
                'p25': np.percentile(scores_array, 25),
                'p40': np.percentile(scores_array, 40),
                'p50': np.percentile(scores_array, 50),
                'p75': np.percentile(scores_array, 75),
                'p90': np.percentile(scores_array, 90),
                'mean': np.mean(scores_array),
                'std': np.std(scores_array),
            }

        # Compute domain average scores
        for domain, scores in domain_scores_list.items():
            self.domain_avg_scores[domain] = np.mean(scores)

        # Global average
        if all_scores:
            self.global_avg_score = np.mean(all_scores)

    def extract_features(
        self,
        candidate_id: str,
        normalized_skills: List[str],
        normalized_scores: Dict[str, float],
        include_metadata: bool = False
    ) -> Dict:
        """
        Extract comprehensive features for a single candidate.

        Args:
            candidate_id: Unique candidate identifier
            normalized_skills: List of normalized skill names
            normalized_scores: Dict of skill → score
            include_metadata: If True, include feature explanations

        Returns:
            Dictionary with:
                - features: Dict of feature_name → value (~600 features)
                - metadata: Optional dict with feature explanations
        """
        features = {}
        metadata = {} if include_metadata else None

        # 1. Core skill features (one-hot + scores)
        skill_features = self._extract_skill_features(normalized_skills, normalized_scores)
        features.update(skill_features)

        # 2. Inferred skill scores
        inferred_features = self._extract_inferred_skills(normalized_skills, normalized_scores)
        features.update(inferred_features)

        # 3. Domain aggregations
        domain_features = self._extract_domain_features(normalized_skills, normalized_scores)
        features.update(domain_features)

        # 4. Weakness features (critical!)
        weakness_features, weakness_metadata = self._extract_weakness_features(
            normalized_skills, normalized_scores
        )
        features.update(weakness_features)
        if include_metadata:
            metadata['weaknesses'] = weakness_metadata

        # 5. Profile characteristics
        profile_features = self._extract_profile_features(normalized_skills, normalized_scores)
        features.update(profile_features)

        # 6. Statistical features
        stat_features = self._extract_statistical_features(normalized_scores)
        features.update(stat_features)

        result = {
            'candidate_id': candidate_id,
            'features': features,
            'feature_count': len(features)
        }

        if include_metadata:
            result['metadata'] = metadata

        return result

    def _extract_skill_features(
        self,
        skills: List[str],
        scores: Dict[str, float]
    ) -> Dict[str, float]:
        """
        Extract core skill features.

        Returns dict with:
            - skill_{name}: score (for each skill)
        """
        features = {}

        for skill in skills:
            if skill in scores:
                features[f"skill_{skill}"] = scores[skill]

        return features

    def _extract_inferred_skills(
        self,
        skills: List[str],
        scores: Dict[str, float]
    ) -> Dict[str, float]:
        """
        Infer scores for skills based on known skills.

        Example: If candidate knows Next.js at 90, infer JavaScript at ~85.

        Returns dict with:
            - inferred_{skill}: inferred_score
        """
        inferred = {}

        for skill in skills:
            if skill in self.SKILL_INFERENCE_RULES:
                skill_score = scores.get(skill, 0)

                for inferred_skill, confidence in self.SKILL_INFERENCE_RULES[skill]:
                    # Only infer if not explicitly present
                    if inferred_skill not in skills:
                        # Inferred score = original_score * confidence
                        inferred_score = skill_score * confidence
                        feature_name = f"inferred_{inferred_skill}"

                        # Take max if multiple skills infer the same skill
                        if feature_name in inferred:
                            inferred[feature_name] = max(inferred[feature_name], inferred_score)
                        else:
                            inferred[feature_name] = inferred_score

        return inferred

    def _extract_domain_features(
        self,
        skills: List[str],
        scores: Dict[str, float]
    ) -> Dict[str, float]:
        """
        Aggregate scores by domain.

        Returns dict with:
            - domain_{domain}_mean: average score in domain
            - domain_{domain}_max: max score in domain
            - domain_{domain}_min: min score in domain
            - domain_{domain}_std: std dev of scores in domain
            - domain_{domain}_count: number of skills in domain
        """
        features = {}

        # Collect scores by domain
        domain_scores = defaultdict(list)
        for skill in skills:
            if skill in scores:
                domains = self.SKILL_TO_DOMAIN.get(skill, [])
                for domain in domains:
                    domain_scores[domain].append(scores[skill])

        # Compute aggregations
        for domain in self.ALL_DOMAINS:
            domain_key = domain.lower().replace("/", "_").replace(" ", "_")

            if domain in domain_scores and domain_scores[domain]:
                scores_array = np.array(domain_scores[domain])
                features[f"domain_{domain_key}_mean"] = np.mean(scores_array)
                features[f"domain_{domain_key}_max"] = np.max(scores_array)
                features[f"domain_{domain_key}_min"] = np.min(scores_array)
                features[f"domain_{domain_key}_std"] = np.std(scores_array)
                features[f"domain_{domain_key}_count"] = len(scores_array)
            else:
                # Domain not present
                features[f"domain_{domain_key}_mean"] = 0.0
                features[f"domain_{domain_key}_max"] = 0.0
                features[f"domain_{domain_key}_min"] = 0.0
                features[f"domain_{domain_key}_std"] = 0.0
                features[f"domain_{domain_key}_count"] = 0

        return features

    def _extract_weakness_features(
        self,
        skills: List[str],
        scores: Dict[str, float]
    ) -> Tuple[Dict[str, float], Dict]:
        """
        Identify and quantify weaknesses.

        A weakness is defined as:
        - Score < 40th percentile (relative to other candidates)
        - Score < 60 (absolute threshold)
        - Score significantly below domain average

        Returns:
            - features: Dict with weakness indicators
            - metadata: Dict with weakness explanations
        """
        features = {}
        metadata = {
            'weak_skills': [],
            'weak_domains': [],
            'missing_expected_skills': []
        }

        # Identify weak skills
        weak_skills = []
        for skill, score in scores.items():
            is_weak = False
            weakness_type = []

            # Check absolute threshold
            if score < 60:
                is_weak = True
                weakness_type.append('absolute')

            # Check relative threshold (if we have dataset stats)
            if skill in self.skill_percentiles:
                p40 = self.skill_percentiles[skill]['p40']
                if score < p40:
                    is_weak = True
                    weakness_type.append('relative')

            if is_weak:
                weak_skills.append(skill)
                features[f"weak_at_{skill}"] = 1
                features[f"weakness_{skill}_score"] = score

                # Relative weakness (how much below average)
                if skill in self.skill_percentiles:
                    mean_score = self.skill_percentiles[skill]['mean']
                    relative_weakness = score / mean_score if mean_score > 0 else 0
                    features[f"relative_weakness_{skill}"] = relative_weakness

                metadata['weak_skills'].append({
                    'skill': skill,
                    'score': score,
                    'weakness_type': weakness_type
                })

        # Weakness count features
        features['weakness_count'] = len(weak_skills)
        features['weakness_ratio'] = len(weak_skills) / len(skills) if skills else 0

        # Identify weak domains (domain avg < 60 or < domain average)
        for domain in self.ALL_DOMAINS:
            domain_key = domain.lower().replace("/", "_").replace(" ", "_")
            domain_mean_key = f"domain_{domain_key}_mean"

            # Check if we computed domain mean
            if domain_mean_key in features:
                domain_mean = features[domain_mean_key]

                if domain_mean > 0 and domain_mean < 60:
                    features[f"domain_weak_{domain_key}"] = 1
                    metadata['weak_domains'].append({
                        'domain': domain,
                        'mean_score': domain_mean
                    })
                else:
                    features[f"domain_weak_{domain_key}"] = 0

        # Identify missing expected skills (based on what they know)
        # Example: If knows React but not JavaScript (even inferred)
        expected_missing = self._find_missing_expected_skills(skills, scores)
        features['missing_expected_skills_count'] = len(expected_missing)
        metadata['missing_expected_skills'] = expected_missing

        return features, metadata

    def _find_missing_expected_skills(
        self,
        skills: List[str],
        scores: Dict[str, float]
    ) -> List[str]:
        """
        Find skills that should be present based on other skills.

        Example: If candidate knows Docker + Kubernetes but not Linux → flag Linux
        """
        missing = []

        # Check all inference rules
        for skill in skills:
            if skill in self.SKILL_INFERENCE_RULES:
                for inferred_skill, _ in self.SKILL_INFERENCE_RULES[skill]:
                    # If inferred skill not present at all (not even inferred)
                    if inferred_skill not in skills and inferred_skill not in scores:
                        if inferred_skill not in missing:
                            missing.append(inferred_skill)

        return missing

    def _extract_profile_features(
        self,
        skills: List[str],
        scores: Dict[str, float]
    ) -> Dict[str, float]:
        """
        Extract profile characteristic features.

        Returns features like:
            - breadth: number of domains covered
            - depth: concentration in top domains
            - specialization_index: how specialized vs generalist
            - advanced_skill_ratio: ratio of high-scoring skills
        """
        features = {}

        # Breadth: number of unique domains covered
        domains_covered = set()
        for skill in skills:
            domains = self.SKILL_TO_DOMAIN.get(skill, [])
            domains_covered.update(domains)

        features['breadth_score'] = len(domains_covered)

        # Depth: max domain skill count
        domain_counts = defaultdict(int)
        for skill in skills:
            domains = self.SKILL_TO_DOMAIN.get(skill, [])
            for domain in domains:
                domain_counts[domain] += 1

        features['depth_score'] = max(domain_counts.values()) if domain_counts else 0

        # Specialization index (depth / breadth)
        if features['breadth_score'] > 0:
            features['specialization_index'] = features['depth_score'] / features['breadth_score']
        else:
            features['specialization_index'] = 0

        # Advanced skill ratio (skills with score > 80)
        if scores:
            advanced_skills = sum(1 for score in scores.values() if score >= 80)
            features['advanced_skill_ratio'] = advanced_skills / len(scores)
        else:
            features['advanced_skill_ratio'] = 0

        # Intermediate skill ratio (60-80)
        if scores:
            intermediate_skills = sum(1 for score in scores.values() if 60 <= score < 80)
            features['intermediate_skill_ratio'] = intermediate_skills / len(scores)
        else:
            features['intermediate_skill_ratio'] = 0

        # Beginner skill ratio (< 60)
        if scores:
            beginner_skills = sum(1 for score in scores.values() if score < 60)
            features['beginner_skill_ratio'] = beginner_skills / len(scores)
        else:
            features['beginner_skill_ratio'] = 0

        # T-shaped index (breadth * depth)
        features['t_shaped_index'] = features['breadth_score'] * features['depth_score']

        # Determine profile type (based on breadth and specialization)
        if features['breadth_score'] >= 5 and features['specialization_index'] < 2:
            features['profile_type_generalist'] = 1
            features['profile_type_specialist'] = 0
        elif features['specialization_index'] >= 3:
            features['profile_type_generalist'] = 0
            features['profile_type_specialist'] = 1
        else:
            features['profile_type_generalist'] = 0
            features['profile_type_specialist'] = 0

        return features

    def _extract_statistical_features(
        self,
        scores: Dict[str, float]
    ) -> Dict[str, float]:
        """
        Extract statistical features from scores.

        Returns:
            - score_mean: average score
            - score_median: median score
            - score_std: standard deviation
            - score_range: max - min
            - skill_count: number of skills
        """
        features = {}

        if scores:
            scores_array = np.array(list(scores.values()))

            features['score_mean'] = np.mean(scores_array)
            features['score_median'] = np.median(scores_array)
            features['score_std'] = np.std(scores_array)
            features['score_max'] = np.max(scores_array)
            features['score_min'] = np.min(scores_array)
            features['score_range'] = np.max(scores_array) - np.min(scores_array)
            features['skill_count'] = len(scores)
        else:
            features['score_mean'] = 0
            features['score_median'] = 0
            features['score_std'] = 0
            features['score_max'] = 0
            features['score_min'] = 0
            features['score_range'] = 0
            features['skill_count'] = 0

        return features

    def extract_features_batch(
        self,
        candidates: List[Dict],
        include_metadata: bool = False
    ) -> List[Dict]:
        """
        Extract features for multiple candidates efficiently.

        Args:
            candidates: List of dicts with candidate_id, normalized_skills, normalized_scores
            include_metadata: If True, include feature explanations

        Returns:
            List of feature dicts
        """
        results = []

        for candidate in candidates:
            features = self.extract_features(
                candidate_id=candidate.get('candidate_id', ''),
                normalized_skills=candidate.get('normalized_skills', []),
                normalized_scores=candidate.get('normalized_scores', {}),
                include_metadata=include_metadata
            )

            # Preserve original candidate data
            features['original_data'] = candidate
            results.append(features)

        return results

    def get_feature_vector(
        self,
        features: Dict[str, float],
        feature_names: Optional[List[str]] = None
    ) -> np.ndarray:
        """
        Convert feature dict to numpy array (for clustering).

        Args:
            features: Dictionary of feature_name → value
            feature_names: Optional list of feature names to use (for consistency)

        Returns:
            Numpy array of feature values
        """
        if feature_names is None:
            # Use all features in sorted order
            feature_names = sorted(features.keys())

        # Extract values in order
        vector = np.array([features.get(name, 0.0) for name in feature_names])

        return vector

    def get_all_feature_names(self, candidates: List[Dict]) -> List[str]:
        """
        Get all unique feature names across all candidates.

        Useful for creating consistent feature matrices.
        """
        all_features = set()

        for candidate in candidates:
            features = self.extract_features(
                candidate_id=candidate.get('candidate_id', ''),
                normalized_skills=candidate.get('normalized_skills', []),
                normalized_scores=candidate.get('normalized_scores', {})
            )
            all_features.update(features['features'].keys())

        return sorted(list(all_features))
