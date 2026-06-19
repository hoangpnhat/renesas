"""
Tests for Feature Engineering Module - Phase 2
"""

import pytest
import numpy as np
from candidate_clustering.features.feature_engineer import FeatureEngineer


class TestFeatureEngineer:
    """Test suite for FeatureEngineer class."""

    @pytest.fixture
    def engineer(self):
        """Create a FeatureEngineer instance."""
        return FeatureEngineer()

    @pytest.fixture
    def sample_candidate(self):
        """Sample candidate with normalized skills and scores."""
        return {
            'candidate_id': 'C001',
            'normalized_skills': ['python', 'docker', 'postgresql', 'kubernetes'],
            'normalized_scores': {
                'python': 85,
                'docker': 78,
                'postgresql': 82,
                'kubernetes': 45
            }
        }

    @pytest.fixture
    def sample_dataset(self):
        """Sample dataset for fitting."""
        return [
            {
                'candidate_id': 'C001',
                'normalized_skills': ['python', 'docker', 'postgresql'],
                'normalized_scores': {'python': 85, 'docker': 78, 'postgresql': 82}
            },
            {
                'candidate_id': 'C002',
                'normalized_skills': ['javascript', 'react', 'node.js'],
                'normalized_scores': {'javascript': 90, 'react': 88, 'node.js': 85}
            },
            {
                'candidate_id': 'C003',
                'normalized_skills': ['python', 'tensorflow', 'pandas'],
                'normalized_scores': {'python': 92, 'tensorflow': 80, 'pandas': 90}
            },
            {
                'candidate_id': 'C004',
                'normalized_skills': ['docker', 'kubernetes', 'aws'],
                'normalized_scores': {'docker': 75, 'kubernetes': 70, 'aws': 85}
            }
        ]


class TestSkillFeatures(TestFeatureEngineer):
    """Test core skill feature extraction."""

    def test_extract_skill_features(self, engineer, sample_candidate):
        """Test basic skill feature extraction."""
        result = engineer.extract_features(
            candidate_id=sample_candidate['candidate_id'],
            normalized_skills=sample_candidate['normalized_skills'],
            normalized_scores=sample_candidate['normalized_scores']
        )

        features = result['features']

        # Check skill features are present
        assert 'skill_python' in features
        assert features['skill_python'] == 85
        assert 'skill_docker' in features
        assert features['skill_docker'] == 78

    def test_skill_features_with_missing_scores(self, engineer):
        """Test skill features when some scores are missing."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['python', 'docker', 'kubernetes'],
            normalized_scores={'python': 85, 'docker': 78}  # kubernetes missing
        )

        features = result['features']

        assert 'skill_python' in features
        assert 'skill_docker' in features
        # kubernetes should not be in features (no score)
        assert 'skill_kubernetes' not in features


class TestInferredSkills(TestFeatureEngineer):
    """Test inferred skill scoring."""

    def test_infer_javascript_from_react(self, engineer):
        """Test inferring JavaScript from React."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['react'],
            normalized_scores={'react': 90}
        )

        features = result['features']

        # Should infer JavaScript
        assert 'inferred_javascript' in features
        # Inferred score should be react_score * confidence (90 * 0.95 = 85.5)
        assert features['inferred_javascript'] == pytest.approx(85.5, rel=0.01)

    def test_infer_python_from_django(self, engineer):
        """Test inferring Python from Django."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['django'],
            normalized_scores={'django': 80}
        )

        features = result['features']

        assert 'inferred_python' in features
        assert features['inferred_python'] == pytest.approx(76.0, rel=0.01)  # 80 * 0.95

    def test_infer_multiple_skills_from_kubernetes(self, engineer):
        """Test inferring multiple skills from Kubernetes."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['kubernetes'],
            normalized_scores={'kubernetes': 70}
        )

        features = result['features']

        # Should infer docker, linux, yaml
        assert 'inferred_docker' in features
        assert 'inferred_linux' in features
        assert 'inferred_yaml' in features

    def test_no_inference_if_skill_present(self, engineer):
        """Test that inference doesn't happen if skill is explicitly present."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['react', 'javascript'],
            normalized_scores={'react': 90, 'javascript': 92}
        )

        features = result['features']

        # Should have explicit skill, not inferred
        assert 'skill_javascript' in features
        assert 'inferred_javascript' not in features

    def test_max_inferred_score(self, engineer):
        """Test that max inferred score is used when multiple sources."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['react', 'angular'],  # Both infer JavaScript
            normalized_scores={'react': 90, 'angular': 85}
        )

        features = result['features']

        # Should use max inference (react gives higher)
        assert 'inferred_javascript' in features
        # 90 * 0.95 = 85.5 (from react)
        assert features['inferred_javascript'] == pytest.approx(85.5, rel=0.01)


class TestDomainFeatures(TestFeatureEngineer):
    """Test domain aggregation features."""

    def test_domain_aggregations(self, engineer, sample_candidate):
        """Test domain aggregation features."""
        result = engineer.extract_features(
            candidate_id=sample_candidate['candidate_id'],
            normalized_skills=sample_candidate['normalized_skills'],
            normalized_scores=sample_candidate['normalized_scores']
        )

        features = result['features']

        # Backend domain (python)
        assert 'domain_backend_mean' in features
        assert features['domain_backend_mean'] == 85  # Only python

        # Database domain (postgresql)
        assert 'domain_database_mean' in features
        assert features['domain_database_mean'] == 82

        # DevOps domain (docker, kubernetes)
        assert 'domain_devops_mean' in features
        assert features['domain_devops_mean'] == pytest.approx((78 + 45) / 2, rel=0.01)
        assert features['domain_devops_max'] == 78
        assert features['domain_devops_min'] == 45

    def test_domain_counts(self, engineer):
        """Test domain skill counts."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['python', 'java', 'go'],
            normalized_scores={'python': 85, 'java': 80, 'go': 75}
        )

        features = result['features']

        # All backend skills
        assert features['domain_backend_count'] == 3

    def test_empty_domain(self, engineer):
        """Test domains with no skills."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['python'],
            normalized_scores={'python': 85}
        )

        features = result['features']

        # Frontend domain should be empty
        assert features['domain_frontend_mean'] == 0
        assert features['domain_frontend_count'] == 0

    def test_multi_domain_skill(self, engineer):
        """Test skill that belongs to multiple domains."""
        # Python belongs to both Backend and Data/ML
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['python'],
            normalized_scores={'python': 90}
        )

        features = result['features']

        # Should appear in both domains
        assert features['domain_backend_mean'] == 90
        assert features['domain_data_ml_mean'] == 90


class TestWeaknessFeatures(TestFeatureEngineer):
    """Test weakness modeling features."""

    def test_absolute_weakness_threshold(self, engineer):
        """Test weakness detection by absolute threshold (< 60)."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['kubernetes', 'python'],
            normalized_scores={'kubernetes': 45, 'python': 85}
        )

        features = result['features']
        metadata = result.get('metadata', {})

        # Kubernetes should be flagged as weak (score < 60)
        assert features['weakness_count'] == 1
        assert 'weak_at_kubernetes' in features
        assert features['weak_at_kubernetes'] == 1

    def test_relative_weakness_with_dataset(self, engineer, sample_dataset):
        """Test weakness detection relative to dataset."""
        # Fit on dataset first
        engineer.fit(sample_dataset)

        # Create candidate with low docker score
        result = engineer.extract_features(
            candidate_id='C005',
            normalized_skills=['docker'],
            normalized_scores={'docker': 50}  # Well below dataset average
        )

        features = result['features']

        # Should be flagged as weak (relative to dataset)
        assert 'weak_at_docker' in features
        assert 'relative_weakness_docker' in features

    def test_weakness_ratio(self, engineer):
        """Test weakness ratio calculation."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['python', 'kubernetes', 'docker'],
            normalized_scores={'python': 85, 'kubernetes': 45, 'docker': 50}
        )

        features = result['features']

        # 2 out of 3 skills are weak
        assert features['weakness_count'] == 2
        assert features['weakness_ratio'] == pytest.approx(2/3, rel=0.01)

    def test_weak_domain_detection(self, engineer):
        """Test weak domain detection."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['docker', 'kubernetes'],
            normalized_scores={'docker': 50, 'kubernetes': 45}
        )

        features = result['features']

        # DevOps domain average is (50 + 45) / 2 = 47.5 < 60
        assert 'domain_weak_devops' in features
        assert features['domain_weak_devops'] == 1

    def test_missing_expected_skills(self, engineer):
        """Test detection of missing expected skills."""
        # Knows React but not JavaScript
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['react'],
            normalized_scores={'react': 90},
            include_metadata=True
        )

        features = result['features']
        metadata = result['metadata']

        # Should flag missing JavaScript
        assert features['missing_expected_skills_count'] > 0
        assert 'javascript' in metadata['weaknesses']['missing_expected_skills']


class TestProfileFeatures(TestFeatureEngineer):
    """Test profile characteristic features."""

    def test_breadth_score(self, engineer):
        """Test breadth score (number of domains)."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['python', 'react', 'docker', 'postgresql'],
            normalized_scores={'python': 85, 'react': 80, 'docker': 75, 'postgresql': 82}
        )

        features = result['features']

        # Python (Backend + Data/ML), React (Frontend), Docker (DevOps), PostgreSQL (Database)
        # Unique domains: Backend, Data/ML, Frontend, DevOps, Database = 5
        assert features['breadth_score'] >= 4

    def test_depth_score(self, engineer):
        """Test depth score (max skills in one domain)."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['python', 'java', 'go'],  # All backend
            normalized_scores={'python': 85, 'java': 80, 'go': 75}
        )

        features = result['features']

        # All 3 skills are backend
        assert features['depth_score'] == 3

    def test_specialization_index(self, engineer):
        """Test specialization index (depth / breadth)."""
        # Specialist: deep in one domain
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['python', 'java', 'go'],
            normalized_scores={'python': 85, 'java': 80, 'go': 75}
        )

        features = result['features']

        # High specialization (depth ~3, breadth ~1-2)
        assert features['specialization_index'] >= 1

    def test_advanced_skill_ratio(self, engineer):
        """Test advanced skill ratio (score >= 80)."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['python', 'docker', 'kubernetes'],
            normalized_scores={'python': 90, 'docker': 85, 'kubernetes': 50}
        )

        features = result['features']

        # 2 out of 3 are advanced (>= 80)
        assert features['advanced_skill_ratio'] == pytest.approx(2/3, rel=0.01)

    def test_profile_type_specialist(self, engineer):
        """Test specialist profile type."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['python', 'django', 'flask', 'fastapi'],
            normalized_scores={'python': 90, 'django': 85, 'flask': 82, 'fastapi': 88}
        )

        features = result['features']

        # High specialization index → specialist
        assert features['profile_type_specialist'] == 1 or features['specialization_index'] >= 3

    def test_profile_type_generalist(self, engineer):
        """Test generalist profile type."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['python', 'react', 'docker', 'postgresql', 'aws', 'kubernetes'],
            normalized_scores={
                'python': 75, 'react': 70, 'docker': 72,
                'postgresql': 68, 'aws': 74, 'kubernetes': 71
            }
        )

        features = result['features']

        # High breadth, low specialization → generalist
        assert features['breadth_score'] >= 5


class TestStatisticalFeatures(TestFeatureEngineer):
    """Test statistical features."""

    def test_score_statistics(self, engineer):
        """Test basic score statistics."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['python', 'docker', 'kubernetes'],
            normalized_scores={'python': 90, 'docker': 70, 'kubernetes': 50}
        )

        features = result['features']

        assert 'score_mean' in features
        assert features['score_mean'] == pytest.approx(70.0, rel=0.01)

        assert 'score_median' in features
        assert features['score_median'] == 70

        assert 'score_max' in features
        assert features['score_max'] == 90

        assert 'score_min' in features
        assert features['score_min'] == 50

        assert 'score_range' in features
        assert features['score_range'] == 40  # 90 - 50

    def test_skill_count(self, engineer):
        """Test skill count feature."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['python', 'docker', 'kubernetes'],
            normalized_scores={'python': 90, 'docker': 70, 'kubernetes': 50}
        )

        features = result['features']

        assert features['skill_count'] == 3


class TestBatchProcessing(TestFeatureEngineer):
    """Test batch feature extraction."""

    def test_extract_features_batch(self, engineer, sample_dataset):
        """Test batch feature extraction."""
        results = engineer.extract_features_batch(sample_dataset)

        assert len(results) == 4
        assert all('features' in r for r in results)
        assert all('candidate_id' in r for r in results)

    def test_batch_preserves_original_data(self, engineer, sample_dataset):
        """Test that batch processing preserves original candidate data."""
        results = engineer.extract_features_batch(sample_dataset)

        assert all('original_data' in r for r in results)
        assert results[0]['original_data']['candidate_id'] == 'C001'


class TestDatasetFitting(TestFeatureEngineer):
    """Test fitting on dataset for relative features."""

    def test_fit_computes_percentiles(self, engineer, sample_dataset):
        """Test that fit computes skill percentiles."""
        engineer.fit(sample_dataset)

        # Python appears in C001 (85), C003 (92)
        assert 'python' in engineer.skill_percentiles
        assert 'mean' in engineer.skill_percentiles['python']
        assert engineer.skill_percentiles['python']['mean'] == pytest.approx(88.5, rel=0.01)

    def test_fit_computes_domain_averages(self, engineer, sample_dataset):
        """Test that fit computes domain averages."""
        engineer.fit(sample_dataset)

        # Backend domain should have average
        assert 'Backend' in engineer.domain_avg_scores

    def test_fit_computes_global_average(self, engineer, sample_dataset):
        """Test that fit computes global average."""
        engineer.fit(sample_dataset)

        assert engineer.global_avg_score is not None
        assert engineer.global_avg_score > 0


class TestFeatureVector(TestFeatureEngineer):
    """Test feature vector generation."""

    def test_get_feature_vector(self, engineer):
        """Test converting features to numpy array."""
        features = {
            'skill_python': 85,
            'skill_docker': 78,
            'domain_backend_mean': 85
        }

        vector = engineer.get_feature_vector(features)

        assert isinstance(vector, np.ndarray)
        assert len(vector) == 3

    def test_get_feature_vector_with_names(self, engineer):
        """Test feature vector with specific feature names."""
        features = {
            'skill_python': 85,
            'skill_docker': 78,
            'skill_java': 70
        }

        feature_names = ['skill_python', 'skill_docker']
        vector = engineer.get_feature_vector(features, feature_names)

        assert len(vector) == 2
        assert vector[0] == 85
        assert vector[1] == 78

    def test_get_all_feature_names(self, engineer, sample_dataset):
        """Test getting all unique feature names."""
        feature_names = engineer.get_all_feature_names(sample_dataset)

        assert isinstance(feature_names, list)
        assert len(feature_names) > 0
        assert all(isinstance(name, str) for name in feature_names)


class TestEdgeCases(TestFeatureEngineer):
    """Test edge cases and error handling."""

    def test_empty_skills(self, engineer):
        """Test with empty skills list."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=[],
            normalized_scores={}
        )

        features = result['features']

        assert features['skill_count'] == 0
        assert features['breadth_score'] == 0
        assert features['weakness_count'] == 0

    def test_single_skill(self, engineer):
        """Test with single skill."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['python'],
            normalized_scores={'python': 85}
        )

        features = result['features']

        assert 'skill_python' in features
        assert features['skill_count'] == 1

    def test_unknown_skill(self, engineer):
        """Test with skill not in domain mapping."""
        result = engineer.extract_features(
            candidate_id='C001',
            normalized_skills=['unknown_skill'],
            normalized_scores={'unknown_skill': 75}
        )

        features = result['features']

        # Should still create skill feature
        assert 'skill_unknown_skill' in features
        assert features['skill_unknown_skill'] == 75

    def test_metadata_inclusion(self, engineer, sample_candidate):
        """Test metadata inclusion."""
        result = engineer.extract_features(
            candidate_id=sample_candidate['candidate_id'],
            normalized_skills=sample_candidate['normalized_skills'],
            normalized_scores=sample_candidate['normalized_scores'],
            include_metadata=True
        )

        assert 'metadata' in result
        assert 'weaknesses' in result['metadata']
