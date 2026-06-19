"""
Skill Normalization Module - 3-Tier Pipeline

Handles noisy LLM-generated skill names and normalizes them to canonical forms.

Tier 1: Rule-based normalization (lowercase, versions, abbreviations, typos)
Tier 2: Embedding-based semantic clustering
Tier 3: Co-occurrence graph disambiguation
"""

import json
import logging
import re
from collections import Counter, defaultdict
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple

import numpy as np
import pandas as pd
from sentence_transformers import SentenceTransformer
from sklearn.metrics.pairwise import cosine_similarity
import hdbscan
import networkx as nx
from community import community_louvain

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class SkillNormalizer:
    """
    3-Tier skill normalization pipeline for handling noisy LLM-generated skills.
    """

    def __init__(
        self,
        rules_path: Optional[str] = None,
        embedding_model: str = "sentence-transformers/all-mpnet-base-v2",
        min_cluster_size: int = 3,
        similarity_threshold: float = 0.85,
    ):
        """
        Initialize the skill normalizer.

        Args:
            rules_path: Path to normalization rules JSON file
            embedding_model: Name of sentence transformer model
            min_cluster_size: Minimum cluster size for HDBSCAN
            similarity_threshold: Threshold for semantic similarity
        """
        self.rules_path = rules_path or str(
            Path(__file__).parent.parent.parent.parent / "config" / "normalization_rules.json"
        )
        self.embedding_model_name = embedding_model
        self.min_cluster_size = min_cluster_size
        self.similarity_threshold = similarity_threshold

        # Load normalization rules
        self._load_rules()

        # Initialize embedding model (lazy loading)
        self._embedding_model = None

        # Caches
        self.tier1_cache: Dict[str, str] = {}
        self.tier2_cache: Dict[str, Tuple[str, float]] = {}
        self.tier3_cache: Dict[str, str] = {}

        # Skill co-occurrence graph
        self.cooccurrence_graph: Optional[nx.Graph] = None

        logger.info("SkillNormalizer initialized")

    def _load_rules(self):
        """Load normalization rules from JSON file."""
        try:
            with open(self.rules_path, "r") as f:
                rules = json.load(f)

            self.abbreviations = {k.lower(): v.lower() for k, v in rules["abbreviations"].items()}
            self.version_patterns = [re.compile(pattern) for pattern in rules["version_patterns"]]
            self.common_typos = {k.lower(): v.lower() for k, v in rules["common_typos"].items()}
            self.skill_synonyms = {k.lower(): v.lower() for k, v in rules["skill_synonyms"].items()}

            logger.info(f"Loaded normalization rules from {self.rules_path}")
        except Exception as e:
            logger.warning(f"Could not load rules from {self.rules_path}: {e}")
            self.abbreviations = {}
            self.version_patterns = []
            self.common_typos = {}
            self.skill_synonyms = {}

    @property
    def embedding_model(self):
        """Lazy load embedding model."""
        if self._embedding_model is None:
            logger.info(f"Loading embedding model: {self.embedding_model_name}")
            self._embedding_model = SentenceTransformer(self.embedding_model_name)
        return self._embedding_model

    # ============================================================
    # TIER 1: Rule-Based Normalization
    # ============================================================

    def normalize_tier1(self, skill: str) -> str:
        """
        Tier 1: Rule-based normalization.

        Steps:
        1. Lowercase and strip whitespace
        2. Remove version numbers
        3. Expand abbreviations
        4. Fix common typos
        5. Apply synonym mapping

        Args:
            skill: Raw skill string

        Returns:
            Normalized skill string
        """
        if not skill or not isinstance(skill, str):
            return ""

        # Check cache
        if skill in self.tier1_cache:
            return self.tier1_cache[skill]

        original = skill
        normalized = skill.strip().lower()

        # Remove version numbers
        for pattern in self.version_patterns:
            normalized = pattern.sub("", normalized).strip()

        # Remove trailing/leading dots, commas, dashes
        normalized = normalized.strip(".,- ")

        # Expand abbreviations (full word match)
        words = normalized.split()
        expanded_words = [self.abbreviations.get(word, word) for word in words]
        normalized = " ".join(expanded_words)

        # Fix common typos
        if normalized in self.common_typos:
            normalized = self.common_typos[normalized]

        # Apply synonym mapping
        if normalized in self.skill_synonyms:
            normalized = self.skill_synonyms[normalized]

        # Clean up multiple spaces
        normalized = " ".join(normalized.split())

        # Cache result
        self.tier1_cache[original] = normalized

        return normalized

    # ============================================================
    # TIER 2: Embedding-Based Semantic Clustering
    # ============================================================

    def normalize_tier2(
        self, skills: List[str], force_recalculate: bool = False
    ) -> Dict[str, Tuple[str, float]]:
        """
        Tier 2: Embedding-based semantic clustering.

        Groups semantically similar skills using sentence embeddings and HDBSCAN.

        Args:
            skills: List of unique skills (already normalized by Tier 1)
            force_recalculate: Force recalculation even if cached

        Returns:
            Dictionary mapping skill -> (canonical_skill, confidence)
        """
        if not skills:
            return {}

        # Filter out cached skills
        if not force_recalculate:
            uncached_skills = [s for s in skills if s not in self.tier2_cache]
        else:
            uncached_skills = skills

        if not uncached_skills:
            return {s: self.tier2_cache[s] for s in skills if s in self.tier2_cache}

        logger.info(f"Tier 2: Processing {len(uncached_skills)} skills with embeddings")

        # Generate embeddings
        embeddings = self.embedding_model.encode(uncached_skills, show_progress_bar=True)

        # Compute similarity matrix
        similarity_matrix = cosine_similarity(embeddings)

        # Apply HDBSCAN clustering
        clusterer = hdbscan.HDBSCAN(
            min_cluster_size=self.min_cluster_size,
            metric="euclidean",
            cluster_selection_method="eom",
        )

        cluster_labels = clusterer.fit_predict(embeddings)

        # Map skills to canonical forms
        skill_to_cluster = {}
        cluster_to_skills = defaultdict(list)

        for skill, cluster_id in zip(uncached_skills, cluster_labels):
            if cluster_id == -1:  # Noise point
                skill_to_cluster[skill] = (skill, 1.0)  # Keep as-is with full confidence
            else:
                cluster_to_skills[cluster_id].append(skill)

        # For each cluster, choose canonical skill (most common or shortest)
        for cluster_id, cluster_skills in cluster_to_skills.items():
            # Choose shortest skill as canonical (usually most concise)
            canonical = min(cluster_skills, key=len)

            # Assign confidence based on cluster cohesion
            cluster_indices = [uncached_skills.index(s) for s in cluster_skills]
            cluster_embeddings = embeddings[cluster_indices]
            cluster_similarities = cosine_similarity(cluster_embeddings)
            avg_similarity = cluster_similarities.mean()

            confidence = float(min(avg_similarity, 1.0))

            for skill in cluster_skills:
                skill_to_cluster[skill] = (canonical, confidence)

        # Update cache
        self.tier2_cache.update(skill_to_cluster)

        # Return results for all requested skills
        return {s: self.tier2_cache.get(s, (s, 1.0)) for s in skills}

    # ============================================================
    # TIER 3: Co-occurrence Graph Disambiguation
    # ============================================================

    def build_cooccurrence_graph(
        self, candidate_skills: List[List[str]], min_cooccurrence: int = 5
    ):
        """
        Build skill co-occurrence graph from candidate data.

        Args:
            candidate_skills: List of skill lists, one per candidate
            min_cooccurrence: Minimum co-occurrence count to create edge
        """
        logger.info(f"Building co-occurrence graph from {len(candidate_skills)} candidates")

        # Count co-occurrences
        cooccurrence_counts = defaultdict(int)

        for skills in candidate_skills:
            # Normalize to tier1 first
            normalized_skills = [self.normalize_tier1(s) for s in skills]
            unique_skills = list(set(normalized_skills))

            # Count all pairs
            for i, skill1 in enumerate(unique_skills):
                for skill2 in unique_skills[i + 1 :]:
                    pair = tuple(sorted([skill1, skill2]))
                    cooccurrence_counts[pair] += 1

        # Build graph
        G = nx.Graph()

        for (skill1, skill2), count in cooccurrence_counts.items():
            if count >= min_cooccurrence:
                G.add_edge(skill1, skill2, weight=count)

        self.cooccurrence_graph = G
        logger.info(
            f"Built graph with {G.number_of_nodes()} nodes and {G.number_of_edges()} edges"
        )

    def normalize_tier3(self, ambiguous_skill: str, context_skills: List[str]) -> str:
        """
        Tier 3: Co-occurrence graph disambiguation.

        Disambiguate ambiguous skills based on context (other skills in profile).

        Example: "ML" could be "Machine Learning" or "Markup Language"
        If context includes ["Python", "TensorFlow"], choose "Machine Learning"

        Args:
            ambiguous_skill: Skill to disambiguate
            context_skills: Other skills in the same candidate profile

        Returns:
            Disambiguated skill
        """
        if self.cooccurrence_graph is None:
            logger.warning("Co-occurrence graph not built. Call build_cooccurrence_graph() first")
            return ambiguous_skill

        # Normalize context skills
        normalized_context = [self.normalize_tier1(s) for s in context_skills]

        # Check if skill is in graph
        if ambiguous_skill not in self.cooccurrence_graph:
            return ambiguous_skill

        # Find connected components (potential disambiguations)
        # Use community detection to find skill groups
        communities = community_louvain.best_partition(self.cooccurrence_graph)

        skill_community = communities.get(ambiguous_skill)
        if skill_community is None:
            return ambiguous_skill

        # Find which community context skills belong to
        context_communities = [
            communities.get(s) for s in normalized_context if s in communities
        ]

        if not context_communities:
            return ambiguous_skill

        # Choose most common community in context
        most_common_community = Counter(context_communities).most_common(1)[0][0]

        # If skill's community matches context, keep it
        # Otherwise, find alternative interpretation
        if skill_community == most_common_community:
            return ambiguous_skill
        else:
            # Find neighbors in the target community
            neighbors = list(self.cooccurrence_graph.neighbors(ambiguous_skill))
            target_neighbors = [
                n for n in neighbors if communities.get(n) == most_common_community
            ]

            if target_neighbors:
                # Return closest neighbor in target community
                return target_neighbors[0]
            else:
                return ambiguous_skill

    # ============================================================
    # END-TO-END NORMALIZATION
    # ============================================================

    def normalize(
        self,
        skill: str,
        context_skills: Optional[List[str]] = None,
        use_tier2: bool = False,
        use_tier3: bool = False,
    ) -> Tuple[str, float]:
        """
        End-to-end skill normalization.

        Args:
            skill: Raw skill string
            context_skills: Other skills in same profile (for Tier 3)
            use_tier2: Apply Tier 2 embedding-based normalization
            use_tier3: Apply Tier 3 graph-based disambiguation

        Returns:
            Tuple of (canonical_skill, confidence)
        """
        # Tier 1: Always apply
        normalized = self.normalize_tier1(skill)

        if not normalized:
            return "", 0.0

        confidence = 0.98  # High confidence for rule-based

        # Tier 2: Semantic clustering (optional, requires batch processing)
        if use_tier2 and normalized in self.tier2_cache:
            normalized, confidence = self.tier2_cache[normalized]

        # Tier 3: Context-based disambiguation
        if use_tier3 and context_skills and self.cooccurrence_graph:
            normalized = self.normalize_tier3(normalized, context_skills)

        return normalized, confidence

    def normalize_batch(
        self, skills: List[str], use_tier2: bool = True
    ) -> Dict[str, Tuple[str, float]]:
        """
        Normalize a batch of skills efficiently.

        Args:
            skills: List of raw skill strings
            use_tier2: Apply Tier 2 embedding-based normalization

        Returns:
            Dictionary mapping original_skill -> (canonical_skill, confidence)
        """
        if not skills:
            return {}

        # Tier 1: Normalize all skills
        tier1_results = {skill: self.normalize_tier1(skill) for skill in skills}

        # Tier 2: Get unique normalized skills and apply embedding clustering
        if use_tier2:
            unique_normalized = list(set(tier1_results.values()))
            tier2_results = self.normalize_tier2(unique_normalized)

            # Map back to original skills
            final_results = {}
            for original, tier1_norm in tier1_results.items():
                canonical, confidence = tier2_results.get(tier1_norm, (tier1_norm, 0.98))
                final_results[original] = (canonical, confidence)

            return final_results
        else:
            # Return Tier 1 results only
            return {skill: (norm, 0.98) for skill, norm in tier1_results.items()}

    # ============================================================
    # SCORE HANDLING
    # ============================================================

    def normalize_skill_scores(
        self,
        skills: List[str],
        scores: Dict[str, float],
        aggregation: str = "max",
        use_tier2: bool = True,
    ) -> Dict[str, float]:
        """
        Normalize skill scores by mapping to canonical skills.

        When multiple raw skills map to the same canonical skill, aggregates their scores.

        Args:
            skills: List of raw skill names
            scores: Dictionary of raw_skill -> score (0-100 or 0-1)
            aggregation: How to combine scores when multiple map to same canonical:
                - 'max': Take maximum score (default, assumes best evidence)
                - 'mean': Average all scores
                - 'weighted_mean': Average weighted by mapping confidence
                - 'first': Take first occurrence
            use_tier2: Apply Tier 2 embedding-based normalization

        Returns:
            Dictionary of canonical_skill -> aggregated_score

        Example:
            >>> normalizer.normalize_skill_scores(
            ...     skills=["Python3.9", "Python Programming", "JS"],
            ...     scores={"Python3.9": 85, "Python Programming": 90, "JS": 88},
            ...     aggregation="max"
            ... )
            {'python': 90, 'javascript': 88}
        """
        if not skills or not scores:
            return {}

        # Normalize skill names
        skill_mappings = self.normalize_batch(skills, use_tier2=use_tier2)

        # Aggregate scores for canonical skills
        canonical_scores = defaultdict(list)
        canonical_confidences = defaultdict(list)

        for skill in skills:
            if skill not in scores:
                continue

            canonical, confidence = skill_mappings.get(skill, (skill, 1.0))
            canonical_scores[canonical].append(scores[skill])
            canonical_confidences[canonical].append(confidence)

        # Aggregate based on method
        result = {}
        for canonical, score_list in canonical_scores.items():
            if aggregation == "max":
                result[canonical] = max(score_list)
            elif aggregation == "mean":
                result[canonical] = sum(score_list) / len(score_list)
            elif aggregation == "weighted_mean":
                # Weight by mapping confidence
                weights = canonical_confidences[canonical]
                weighted_sum = sum(s * w for s, w in zip(score_list, weights))
                weight_sum = sum(weights)
                result[canonical] = weighted_sum / weight_sum if weight_sum > 0 else 0
            elif aggregation == "first":
                result[canonical] = score_list[0]
            else:
                # Default to max
                result[canonical] = max(score_list)

        return result

    def normalize_candidate(
        self,
        candidate_skills: List[str],
        candidate_scores: Dict[str, float],
        aggregation: str = "max",
        use_tier2: bool = True,
        include_metadata: bool = False,
    ) -> Dict:
        """
        Normalize both skills and scores for a single candidate.

        Args:
            candidate_skills: List of raw skill names
            candidate_scores: Dictionary of raw_skill -> score
            aggregation: Score aggregation method ('max', 'mean', 'weighted_mean', 'first')
            use_tier2: Apply Tier 2 normalization
            include_metadata: Include mapping metadata (confidence, aggregation info)

        Returns:
            Dictionary with:
                - 'normalized_skills': List of unique canonical skills
                - 'normalized_scores': Dict of canonical_skill -> score
                - 'metadata': Optional dict with mapping details

        Example:
            >>> result = normalizer.normalize_candidate(
            ...     candidate_skills=["Python3.9", "JS", "React.js"],
            ...     candidate_scores={"Python3.9": 85, "JS": 88, "React.js": 92}
            ... )
            >>> print(result['normalized_skills'])
            ['python', 'javascript', 'react']
            >>> print(result['normalized_scores'])
            {'python': 85, 'javascript': 88, 'react': 92}
        """
        # Normalize skill names
        skill_mappings = self.normalize_batch(candidate_skills, use_tier2=use_tier2)

        # Get unique canonical skills
        canonical_skills = list(set([canonical for canonical, _ in skill_mappings.values()]))

        # Normalize scores
        normalized_scores = self.normalize_skill_scores(
            skills=candidate_skills,
            scores=candidate_scores,
            aggregation=aggregation,
            use_tier2=use_tier2,
        )

        result = {
            "normalized_skills": sorted(canonical_skills),
            "normalized_scores": normalized_scores,
        }

        if include_metadata:
            # Track which raw skills mapped to which canonical
            skill_mapping_details = defaultdict(list)
            for raw_skill, (canonical, confidence) in skill_mappings.items():
                skill_mapping_details[canonical].append(
                    {
                        "raw_skill": raw_skill,
                        "confidence": confidence,
                        "raw_score": candidate_scores.get(raw_skill),
                    }
                )

            result["metadata"] = {
                "skill_mappings": dict(skill_mapping_details),
                "aggregation_method": aggregation,
                "original_skill_count": len(candidate_skills),
                "normalized_skill_count": len(canonical_skills),
                "reduction": len(candidate_skills) - len(canonical_skills),
            }

        return result

    def normalize_dataset(
        self,
        candidates: List[Dict],
        skill_field: str = "skills",
        score_field: str = "skill_scores",
        aggregation: str = "max",
        use_tier2: bool = True,
    ) -> List[Dict]:
        """
        Normalize skills and scores for an entire dataset of candidates.

        Args:
            candidates: List of candidate dictionaries
            skill_field: Field name containing skill list
            score_field: Field name containing score dictionary
            aggregation: Score aggregation method
            use_tier2: Apply Tier 2 normalization

        Returns:
            List of candidate dictionaries with added fields:
                - 'normalized_skills'
                - 'normalized_scores'

        Example:
            >>> candidates = [
            ...     {
            ...         "candidate_id": "C001",
            ...         "skills": ["Python3.9", "Docker"],
            ...         "skill_scores": {"Python3.9": 85, "Docker": 78}
            ...     }
            ... ]
            >>> normalized = normalizer.normalize_dataset(candidates)
        """
        if not candidates:
            return []

        logger.info(f"Normalizing dataset with {len(candidates)} candidates...")

        # Collect all unique skills for batch processing
        all_skills = []
        for candidate in candidates:
            if skill_field in candidate:
                all_skills.extend(candidate[skill_field])

        unique_skills = list(set(all_skills))
        logger.info(f"Found {len(unique_skills)} unique skills across dataset")

        # Pre-compute all skill mappings (more efficient)
        skill_mappings = self.normalize_batch(unique_skills, use_tier2=use_tier2)

        # Process each candidate
        normalized_candidates = []
        for candidate in candidates:
            normalized = candidate.copy()

            if skill_field in candidate and score_field in candidate:
                result = self.normalize_candidate(
                    candidate_skills=candidate[skill_field],
                    candidate_scores=candidate[score_field],
                    aggregation=aggregation,
                    use_tier2=False,  # Already computed above
                )

                normalized["normalized_skills"] = result["normalized_skills"]
                normalized["normalized_scores"] = result["normalized_scores"]

            normalized_candidates.append(normalized)

        logger.info("Dataset normalization complete")
        return normalized_candidates

    # ============================================================
    # UTILITIES
    # ============================================================

    def get_mapping_dict(self, confidence_threshold: float = 0.8) -> Dict[str, str]:
        """
        Get a simple dictionary of skill mappings above confidence threshold.

        Args:
            confidence_threshold: Minimum confidence to include mapping

        Returns:
            Dictionary of skill -> canonical_skill
        """
        mapping = {}

        # From Tier 1 cache
        mapping.update(self.tier1_cache)

        # From Tier 2 cache (filter by confidence)
        for skill, (canonical, confidence) in self.tier2_cache.items():
            if confidence >= confidence_threshold:
                mapping[skill] = canonical

        return mapping

    def save_mappings(self, output_path: str):
        """Save all mappings to JSON file."""
        mappings = {
            "tier1_cache": self.tier1_cache,
            "tier2_cache": {
                k: {"canonical": v[0], "confidence": v[1]}
                for k, v in self.tier2_cache.items()
            },
        }

        with open(output_path, "w") as f:
            json.dump(mappings, f, indent=2)

        logger.info(f"Saved mappings to {output_path}")

    def load_mappings(self, input_path: str):
        """Load mappings from JSON file."""
        with open(input_path, "r") as f:
            mappings = json.load(f)

        self.tier1_cache = mappings.get("tier1_cache", {})

        tier2_data = mappings.get("tier2_cache", {})
        self.tier2_cache = {
            k: (v["canonical"], v["confidence"]) for k, v in tier2_data.items()
        }

        logger.info(f"Loaded mappings from {input_path}")

    def get_statistics(self) -> Dict:
        """Get normalization statistics."""
        return {
            "tier1_mappings": len(self.tier1_cache),
            "tier2_mappings": len(self.tier2_cache),
            "unique_canonical_skills": len(set(self.tier1_cache.values())),
            "graph_nodes": (
                self.cooccurrence_graph.number_of_nodes()
                if self.cooccurrence_graph
                else 0
            ),
            "graph_edges": (
                self.cooccurrence_graph.number_of_edges()
                if self.cooccurrence_graph
                else 0
            ),
        }
