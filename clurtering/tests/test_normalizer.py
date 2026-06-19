"""
Tests for Skill Normalizer
"""

import pytest
import sys
from pathlib import Path

# Add src to path
sys.path.insert(0, str(Path(__file__).parent.parent / "src"))

from candidate_clustering.skills.normalizer import SkillNormalizer


class TestTier1Normalization:
    """Test Tier 1: Rule-based normalization"""

    @pytest.fixture
    def normalizer(self):
        return SkillNormalizer()

    def test_lowercase_normalization(self, normalizer):
        """Test that skills are converted to lowercase"""
        assert normalizer.normalize_tier1("Python") == "python"
        assert normalizer.normalize_tier1("JAVASCRIPT") == "javascript"
        assert normalizer.normalize_tier1("React") == "react"

    def test_version_removal(self, normalizer):
        """Test that version numbers are removed"""
        assert normalizer.normalize_tier1("Python3.9") == "python"
        assert normalizer.normalize_tier1("Python 3.10.2") == "python"
        assert normalizer.normalize_tier1("Node.js v18") == "node.js"

    def test_abbreviation_expansion(self, normalizer):
        """Test that abbreviations are expanded"""
        assert normalizer.normalize_tier1("JS") == "javascript"
        assert normalizer.normalize_tier1("K8s") == "kubernetes"
        assert normalizer.normalize_tier1("ML") == "machine learning"

    def test_typo_correction(self, normalizer):
        """Test that common typos are fixed"""
        assert normalizer.normalize_tier1("pyton") == "python"
        assert normalizer.normalize_tier1("javascirpt") == "javascript"
        assert normalizer.normalize_tier1("postgre") == "postgresql"

    def test_synonym_mapping(self, normalizer):
        """Test that synonyms are mapped to canonical forms"""
        assert normalizer.normalize_tier1("Python Programming") == "python"
        assert normalizer.normalize_tier1("React.js") == "react"
        assert normalizer.normalize_tier1("Docker Container") == "docker"

    def test_whitespace_handling(self, normalizer):
        """Test that extra whitespace is handled"""
        assert normalizer.normalize_tier1("  Python  ") == "python"
        assert normalizer.normalize_tier1("Machine  Learning") == "machine learning"

    def test_empty_input(self, normalizer):
        """Test handling of empty/invalid input"""
        assert normalizer.normalize_tier1("") == ""
        assert normalizer.normalize_tier1(None) == ""
        assert normalizer.normalize_tier1("   ") == ""

    def test_caching(self, normalizer):
        """Test that normalization results are cached"""
        skill = "Python3.9"
        result1 = normalizer.normalize_tier1(skill)
        result2 = normalizer.normalize_tier1(skill)

        assert result1 == result2
        assert skill in normalizer.tier1_cache


class TestTier2Normalization:
    """Test Tier 2: Embedding-based semantic clustering"""

    @pytest.fixture
    def normalizer(self):
        return SkillNormalizer()

    def test_batch_normalization(self, normalizer):
        """Test that batch normalization works"""
        skills = ["python", "python programming", "java", "javascript"]
        results = normalizer.normalize_tier2(skills)

        assert len(results) == len(skills)
        for skill in skills:
            assert skill in results
            canonical, confidence = results[skill]
            assert isinstance(canonical, str)
            assert 0 <= confidence <= 1

    def test_semantic_grouping(self, normalizer):
        """Test that semantically similar skills are grouped"""
        skills = [
            "python",
            "python programming",
            "python development",
            "java",
            "javascript",
        ]
        results = normalizer.normalize_tier2(skills)

        # Python variations should map to same canonical
        python_canonicals = [
            results[s][0]
            for s in ["python", "python programming", "python development"]
        ]
        assert len(set(python_canonicals)) <= 2  # Should be mostly unified

    def test_confidence_scores(self, normalizer):
        """Test that confidence scores are reasonable"""
        skills = ["python", "java", "javascript"]
        results = normalizer.normalize_tier2(skills)

        for skill in skills:
            canonical, confidence = results[skill]
            assert 0.5 <= confidence <= 1.0  # Should have decent confidence


class TestEndToEndNormalization:
    """Test end-to-end normalization"""

    @pytest.fixture
    def normalizer(self):
        return SkillNormalizer()

    def test_normalize_single_skill(self, normalizer):
        """Test normalizing a single skill"""
        canonical, confidence = normalizer.normalize("Python3.9", use_tier2=False)

        assert canonical == "python"
        assert confidence > 0.9

    def test_normalize_batch(self, normalizer):
        """Test batch normalization"""
        skills = ["Python3.9", "JS", "K8s", "Docker Container", "pyton"]
        results = normalizer.normalize_batch(skills, use_tier2=False)

        assert len(results) == len(skills)
        assert results["Python3.9"][0] == "python"
        assert results["JS"][0] == "javascript"
        assert results["K8s"][0] == "kubernetes"

    def test_mapping_dict_export(self, normalizer):
        """Test exporting mapping dictionary"""
        skills = ["Python3.9", "React.js", "K8s"]
        normalizer.normalize_batch(skills, use_tier2=False)

        mapping_dict = normalizer.get_mapping_dict()

        assert isinstance(mapping_dict, dict)
        assert "python3.9" in mapping_dict or "Python3.9" in mapping_dict


class TestCooccurrenceGraph:
    """Test Tier 3: Co-occurrence graph"""

    @pytest.fixture
    def normalizer(self):
        return SkillNormalizer()

    @pytest.fixture
    def sample_candidates(self):
        """Sample candidate skill lists"""
        return [
            ["python", "tensorflow", "machine learning", "pandas"],
            ["python", "pytorch", "deep learning", "numpy"],
            ["python", "scikit-learn", "machine learning", "pandas"],
            ["javascript", "react", "node.js", "html"],
            ["javascript", "vue", "node.js", "css"],
        ]

    def test_build_graph(self, normalizer, sample_candidates):
        """Test building co-occurrence graph"""
        normalizer.build_cooccurrence_graph(sample_candidates, min_cooccurrence=2)

        assert normalizer.cooccurrence_graph is not None
        assert normalizer.cooccurrence_graph.number_of_nodes() > 0
        assert normalizer.cooccurrence_graph.number_of_edges() > 0

    def test_graph_statistics(self, normalizer, sample_candidates):
        """Test graph statistics"""
        normalizer.build_cooccurrence_graph(sample_candidates, min_cooccurrence=2)
        stats = normalizer.get_statistics()

        assert "graph_nodes" in stats
        assert "graph_edges" in stats
        assert stats["graph_nodes"] > 0


class TestSaveLoad:
    """Test saving and loading mappings"""

    @pytest.fixture
    def normalizer(self):
        return SkillNormalizer()

    def test_save_and_load_mappings(self, normalizer, tmp_path):
        """Test saving and loading mappings"""
        # Normalize some skills
        skills = ["Python3.9", "JS", "K8s"]
        normalizer.normalize_batch(skills, use_tier2=False)

        # Save mappings
        output_path = tmp_path / "mappings.json"
        normalizer.save_mappings(str(output_path))

        assert output_path.exists()

        # Create new normalizer and load mappings
        new_normalizer = SkillNormalizer()
        new_normalizer.load_mappings(str(output_path))

        # Check that mappings were loaded
        assert len(new_normalizer.tier1_cache) > 0


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
