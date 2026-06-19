"""
Tests for Score Handling in Skill Normalizer
"""

import pytest
import sys
from pathlib import Path

# Add src to path
sys.path.insert(0, str(Path(__file__).parent.parent / "src"))

from candidate_clustering.skills.normalizer import SkillNormalizer


class TestScoreNormalization:
    """Test score normalization and aggregation"""

    @pytest.fixture
    def normalizer(self):
        return SkillNormalizer()

    def test_normalize_skill_scores_max(self, normalizer):
        """Test score normalization with max aggregation"""
        skills = ["Python3.9", "Python Programming", "JS"]
        scores = {"Python3.9": 85, "Python Programming": 90, "JS": 88}

        result = normalizer.normalize_skill_scores(
            skills=skills, scores=scores, aggregation="max", use_tier2=False
        )

        # Python3.9 and Python Programming should map to 'python' with max score
        assert "python" in result
        assert result["python"] == 90  # max(85, 90)
        assert "javascript" in result
        assert result["javascript"] == 88

    def test_normalize_skill_scores_mean(self, normalizer):
        """Test score normalization with mean aggregation"""
        skills = ["Python3.9", "Python Programming"]
        scores = {"Python3.9": 80, "Python Programming": 90}

        result = normalizer.normalize_skill_scores(
            skills=skills, scores=scores, aggregation="mean", use_tier2=False
        )

        assert "python" in result
        assert result["python"] == 85  # mean(80, 90)

    def test_normalize_skill_scores_weighted_mean(self, normalizer):
        """Test score normalization with weighted mean"""
        skills = ["Python", "pyton"]  # pyton is typo, lower confidence
        scores = {"Python": 90, "pyton": 70}

        # Normalize to get mappings first
        normalizer.normalize_batch(skills, use_tier2=False)

        result = normalizer.normalize_skill_scores(
            skills=skills, scores=scores, aggregation="weighted_mean", use_tier2=False
        )

        # Should weight correct spelling higher
        assert "python" in result
        # Weighted mean should be closer to 90 than simple mean (80)
        assert 80 < result["python"] <= 90

    def test_normalize_skill_scores_empty(self, normalizer):
        """Test handling of empty inputs"""
        result = normalizer.normalize_skill_scores(skills=[], scores={})
        assert result == {}

        result = normalizer.normalize_skill_scores(
            skills=["Python"], scores={}, use_tier2=False
        )
        assert result == {}

    def test_normalize_skill_scores_missing_scores(self, normalizer):
        """Test handling of skills without scores"""
        skills = ["Python", "JavaScript", "React"]
        scores = {"Python": 85, "JavaScript": 88}  # React missing

        result = normalizer.normalize_skill_scores(
            skills=skills, scores=scores, use_tier2=False
        )

        assert "python" in result
        assert "javascript" in result
        assert "react" not in result  # Missing score should be excluded


class TestCandidateNormalization:
    """Test normalizing individual candidates"""

    @pytest.fixture
    def normalizer(self):
        return SkillNormalizer()

    def test_normalize_candidate_basic(self, normalizer):
        """Test basic candidate normalization"""
        skills = ["Python3.9", "Docker", "K8s"]
        scores = {"Python3.9": 85, "Docker": 78, "K8s": 65}

        result = normalizer.normalize_candidate(
            candidate_skills=skills, candidate_scores=scores, use_tier2=False
        )

        assert "normalized_skills" in result
        assert "normalized_scores" in result

        # Check normalized skills
        assert "python" in result["normalized_skills"]
        assert "docker" in result["normalized_skills"]
        assert "kubernetes" in result["normalized_skills"]

        # Check scores
        assert result["normalized_scores"]["python"] == 85
        assert result["normalized_scores"]["docker"] == 78
        assert result["normalized_scores"]["kubernetes"] == 65

    def test_normalize_candidate_with_duplicates(self, normalizer):
        """Test candidate with duplicate skills (different names)"""
        skills = ["Python3.9", "Python Programming", "JS", "JavaScript"]
        scores = {"Python3.9": 85, "Python Programming": 90, "JS": 88, "JavaScript": 92}

        result = normalizer.normalize_candidate(
            candidate_skills=skills,
            candidate_scores=scores,
            aggregation="max",
            use_tier2=False,
        )

        # Should have only 2 unique skills
        assert len(result["normalized_skills"]) == 2
        assert "python" in result["normalized_skills"]
        assert "javascript" in result["normalized_skills"]

        # Scores should be aggregated (max)
        assert result["normalized_scores"]["python"] == 90
        assert result["normalized_scores"]["javascript"] == 92

    def test_normalize_candidate_with_metadata(self, normalizer):
        """Test candidate normalization with metadata"""
        skills = ["Python3.9", "Python Programming"]
        scores = {"Python3.9": 85, "Python Programming": 90}

        result = normalizer.normalize_candidate(
            candidate_skills=skills,
            candidate_scores=scores,
            include_metadata=True,
            use_tier2=False,
        )

        assert "metadata" in result
        metadata = result["metadata"]

        assert "skill_mappings" in metadata
        assert "aggregation_method" in metadata
        assert "original_skill_count" in metadata
        assert "normalized_skill_count" in metadata
        assert "reduction" in metadata

        # Check metadata values
        assert metadata["original_skill_count"] == 2
        assert metadata["normalized_skill_count"] == 1
        assert metadata["reduction"] == 1

        # Check skill mapping details
        assert "python" in metadata["skill_mappings"]
        python_mappings = metadata["skill_mappings"]["python"]
        assert len(python_mappings) == 2  # Two raw skills mapped


class TestDatasetNormalization:
    """Test normalizing entire datasets"""

    @pytest.fixture
    def normalizer(self):
        return SkillNormalizer()

    @pytest.fixture
    def sample_dataset(self):
        return [
            {
                "candidate_id": "C001",
                "skills": ["Python3.9", "Docker"],
                "skill_scores": {"Python3.9": 85, "Docker": 78},
            },
            {
                "candidate_id": "C002",
                "skills": ["JavaScript", "React.js"],
                "skill_scores": {"JavaScript": 90, "React.js": 88},
            },
            {
                "candidate_id": "C003",
                "skills": ["Python Programming", "JS"],
                "skill_scores": {"Python Programming": 92, "JS": 85},
            },
        ]

    def test_normalize_dataset_basic(self, normalizer, sample_dataset):
        """Test basic dataset normalization"""
        result = normalizer.normalize_dataset(sample_dataset, use_tier2=False)

        assert len(result) == 3

        # Check first candidate
        c1 = result[0]
        assert "normalized_skills" in c1
        assert "normalized_scores" in c1
        assert "python" in c1["normalized_skills"]
        assert "docker" in c1["normalized_skills"]
        assert c1["normalized_scores"]["python"] == 85

        # Check second candidate
        c2 = result[1]
        assert "javascript" in c2["normalized_skills"]
        assert "react" in c2["normalized_skills"]

    def test_normalize_dataset_preserves_original(self, normalizer, sample_dataset):
        """Test that original data is preserved"""
        result = normalizer.normalize_dataset(sample_dataset, use_tier2=False)

        # Original fields should still exist
        for candidate in result:
            assert "candidate_id" in candidate
            assert "skills" in candidate
            assert "skill_scores" in candidate

    def test_normalize_dataset_empty(self, normalizer):
        """Test handling of empty dataset"""
        result = normalizer.normalize_dataset([])
        assert result == []


class TestScoreAggregationMethods:
    """Test different aggregation methods"""

    @pytest.fixture
    def normalizer(self):
        return SkillNormalizer()

    @pytest.fixture
    def duplicate_skills(self):
        return {
            "skills": ["Python", "Python3", "Python3.9"],
            "scores": {"Python": 70, "Python3": 80, "Python3.9": 90},
        }

    def test_max_aggregation(self, normalizer, duplicate_skills):
        """Test max aggregation"""
        result = normalizer.normalize_skill_scores(
            skills=duplicate_skills["skills"],
            scores=duplicate_skills["scores"],
            aggregation="max",
            use_tier2=False,
        )

        assert result["python"] == 90  # max(70, 80, 90)

    def test_mean_aggregation(self, normalizer, duplicate_skills):
        """Test mean aggregation"""
        result = normalizer.normalize_skill_scores(
            skills=duplicate_skills["skills"],
            scores=duplicate_skills["scores"],
            aggregation="mean",
            use_tier2=False,
        )

        assert result["python"] == 80  # mean(70, 80, 90)

    def test_first_aggregation(self, normalizer, duplicate_skills):
        """Test first aggregation"""
        result = normalizer.normalize_skill_scores(
            skills=duplicate_skills["skills"],
            scores=duplicate_skills["scores"],
            aggregation="first",
            use_tier2=False,
        )

        assert result["python"] == 70  # first occurrence


class TestRealWorldScenarios:
    """Test real-world scenarios"""

    @pytest.fixture
    def normalizer(self):
        return SkillNormalizer()

    def test_complex_candidate(self, normalizer):
        """Test complex candidate with many skills and variations"""
        candidate = {
            "candidate_id": "C999",
            "skills": [
                "Python3.9",
                "Python Programming",
                "JavaScript",
                "JS",
                "React.js",
                "ReactJS",
                "Docker",
                "Docker Container",
                "K8s",
                "Kubernetes",
            ],
            "skill_scores": {
                "Python3.9": 85,
                "Python Programming": 90,
                "JavaScript": 88,
                "JS": 92,
                "React.js": 95,
                "ReactJS": 93,
                "Docker": 75,
                "Docker Container": 78,
                "K8s": 65,
                "Kubernetes": 70,
            },
        }

        result = normalizer.normalize_candidate(
            candidate_skills=candidate["skills"],
            candidate_scores=candidate["skill_scores"],
            aggregation="max",
            use_tier2=False,
        )

        # Should have 5 unique skills
        assert len(result["normalized_skills"]) == 5

        # Check max scores
        assert result["normalized_scores"]["python"] == 90
        assert result["normalized_scores"]["javascript"] == 92
        assert result["normalized_scores"]["react"] == 95
        assert result["normalized_scores"]["docker"] == 78
        assert result["normalized_scores"]["kubernetes"] == 70

    def test_score_range_validation(self, normalizer):
        """Test handling of different score ranges"""
        # Test 0-100 range
        result1 = normalizer.normalize_skill_scores(
            skills=["Python"], scores={"Python": 85}, use_tier2=False
        )
        assert 0 <= result1["python"] <= 100

        # Test 0-1 range
        result2 = normalizer.normalize_skill_scores(
            skills=["Python"], scores={"Python": 0.85}, use_tier2=False
        )
        assert 0 <= result2["python"] <= 1


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
