# Data Schema & Expected Output

## Input Data Structure

### Raw Candidate Data (từ LLM)

```python
[
    {
        "candidate_id": "C001",
        "question": "Tell me about your backend experience",  # Optional
        "interview_transcript": "I have 5 years of experience with Python...",  # Full text
        "skills": [
            "Python3.9",
            "Docker",
            "PostgreSQL",
            "K8s",
            "FastAPI"
        ],
        "skill_scores": {
            "Python3.9": 85,
            "Docker": 78,
            "PostgreSQL": 82,
            "K8s": 45,  # Low score - weakness!
            "FastAPI": 72
        }
    },
    {
        "candidate_id": "C002",
        "question": "Describe your full-stack projects",
        "interview_transcript": "I built several React applications...",
        "skills": [
            "JavaScript",
            "React.js",
            "Node.js",
            "MongoDB",
            "Docker"
        ],
        "skill_scores": {
            "JavaScript": 90,
            "React.js": 88,
            "Node.js": 85,
            "MongoDB": 70,
            "Docker": 50  # Weakness
        }
    },
    # ... thousands more candidates
]
```

### Field Descriptions

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `candidate_id` | string | ✅ Yes | Unique identifier |
| `question` | string | ❌ No | Interview question (optional) |
| `interview_transcript` | string | ⚠️ Optional | Full interview text (used in Phase 2 if available) |
| `skills` | list[string] | ✅ Yes | LLM-extracted skill names (noisy) |
| `skill_scores` | dict[string, float] | ✅ Yes | Score per skill (0-100 or 0-1) |

---

## Expected Output Structure

### Final Clusters with Multi-Label Profiles

```python
{
    "cluster_id": "cluster_1",
    "cluster_label": "StrongPython_StrongDocker_WeakKubernetes",  # Multi-label
    "cluster_description": "Backend specialists with strong Python and containerization skills, but weak in orchestration",
    "size": 145,  # Number of candidates

    # Detailed profile
    "profile": {
        # Top strengths (score > 75th percentile)
        "strengths": [
            {"skill": "python", "avg_score": 87.5, "percentile": 85},
            {"skill": "docker", "avg_score": 82.3, "percentile": 78},
            {"skill": "postgresql", "avg_score": 80.1, "percentile": 72}
        ],

        # Top weaknesses (score < 40th percentile)
        "weaknesses": [
            {"skill": "kubernetes", "avg_score": 45.2, "percentile": 25},
            {"skill": "system design", "avg_score": 48.5, "percentile": 30}
        ],

        # Domain averages
        "domain_scores": {
            "Backend": 82.5,
            "DevOps": 65.3,
            "Database": 78.9,
            "Cloud": 52.1
        },

        # Profile type
        "profile_type": "Specialist",  # or "Generalist", "Balanced"
        "specialization_domains": ["Backend", "Database"],
        "breadth": 4.2,  # Number of domains covered
        "depth": 8.5     # Max skill concentration
    },

    # Member candidates
    "members": [
        {
            "candidate_id": "C001",
            "confidence": 0.92,  # Cluster assignment confidence
            "distance_to_center": 0.15,
            "normalized_skills": ["python", "docker", "postgresql", "kubernetes", "fastapi"],
            "normalized_scores": {
                "python": 85,
                "docker": 78,
                "postgresql": 82,
                "kubernetes": 45,
                "fastapi": 72
            }
        },
        # ... more members
    ],

    # Metadata
    "metadata": {
        "creation_date": "2025-12-10",
        "algorithm": "HDBSCAN+GMM+Hierarchical (ensemble)",
        "silhouette_score": 0.35,
        "cohesion": 0.82
    }
}
```

---

## Multi-Label Format Examples

### Format: `Strong<Skill1>_Strong<Skill2>_Weak<Skill3>`

```python
# Example 1: Backend Specialist
"StrongPython_StrongDocker_WeakKubernetes"

# Example 2: Full-Stack Developer
"StrongJavaScript_StrongReact_StrongNodejs_WeakSystemDesign"

# Example 3: Data Scientist
"StrongPython_StrongMachineLearning_StrongPandas_WeakDeepLearning"

# Example 4: DevOps Engineer
"StrongDocker_StrongKubernetes_StrongAWS_WeakNetworking"

# Example 5: Generalist
"RoundedFullStack_ShallowDepth"

# Example 6: Database Specialist
"StrongPostgreSQL_StrongDataModeling_WeakNoSQL"
```

### Label Components

1. **Strength Prefix**
   - `Strong`: Score > 75th percentile
   - `Good`: Score 50-75th percentile
   - `Weak`: Score < 40th percentile
   - `Missing`: Expected skill but absent

2. **Skill Grouping**
   - Individual skills: `StrongPython`
   - Skill families: `StrongJavaScript_StrongReact` (related)
   - Domains: `StrongBackend_WeakFrontend`

3. **Profile Patterns**
   - `Specialist`: Deep in 1-2 domains
   - `Generalist`: Broad across many domains
   - `Rounded`: Balanced strengths
   - `Shallow`: Many skills but low depth

---

## Complete Pipeline: Input → Output

### Phase 1: Skill Normalization (Current ✅)

**Input:**
```python
{
    "skills": ["Python3.9", "Docker Container", "K8s"],
    "skill_scores": {"Python3.9": 85, "Docker Container": 78, "K8s": 45}
}
```

**Output:**
```python
{
    "normalized_skills": ["python", "docker", "kubernetes"],
    "normalized_scores": {"python": 85, "docker": 78, "kubernetes": 45}
}
```

---

### Phase 2: Feature Engineering (Next)

**Input:** Normalized skills + scores

**Output:** Feature vector per candidate

```python
{
    "candidate_id": "C001",
    "features": {
        # Core skill features (300 dims)
        "skill_python": 85,
        "skill_docker": 78,
        "skill_kubernetes": 45,

        # Inferred skills (100 dims)
        "skill_linux": 75,  # Inferred from Docker
        "skill_yaml": 70,   # Inferred from K8s

        # Domain aggregations (40 dims)
        "domain_backend_mean": 82.5,
        "domain_backend_max": 85,
        "domain_devops_mean": 61.5,
        "domain_devops_std": 23.3,

        # Weakness features (60 dims)
        "weak_at_kubernetes": 1,  # Binary flag
        "weakness_kubernetes_score": 45,
        "relative_weakness_kubernetes": 0.65,  # Compared to domain avg

        # Profile characteristics (30 dims)
        "breadth_score": 4.2,
        "depth_score": 8.5,
        "specialization_index": 0.72,
        "advanced_skill_ratio": 0.4,

        # ... total ~600 features
    }
}
```

---

### Phase 3: Dimensionality Reduction

**Input:** 600-dim feature vectors

**Output:** 20-30 dim embeddings

```python
# PCA: 600 → 100 dims
# UMAP: 100 → 25 dims
candidate_embeddings = [
    [0.23, -0.45, 0.67, ...],  # 25 dimensions
    [0.31, -0.52, 0.71, ...],
    ...
]
```

---

### Phase 4: Clustering

**Input:** 25-dim embeddings

**Output:** Cluster assignments

```python
{
    "C001": {"cluster_id": 1, "confidence": 0.92},
    "C002": {"cluster_id": 5, "confidence": 0.88},
    ...
}
```

---

### Phase 5: Cluster Interpretation & Multi-Label Generation

**Input:** Clusters + original features

**Output:** Multi-label profiles

```python
{
    "cluster_1": {
        "label": "StrongPython_StrongDocker_WeakKubernetes",
        "description": "Backend specialists with containerization but weak orchestration",

        # How label was generated
        "label_generation": {
            "top_3_strengths": [
                {"skill": "python", "avg_score": 87.5, "threshold": 75},
                {"skill": "docker", "avg_score": 82.3, "threshold": 75},
                {"skill": "postgresql", "avg_score": 80.1, "threshold": 75}
            ],
            "top_2_weaknesses": [
                {"skill": "kubernetes", "avg_score": 45.2, "threshold": 60},
                {"skill": "system design", "avg_score": 48.5, "threshold": 60}
            ],
            "profile_type": "Specialist"
        },

        "members": [...]
    }
}
```

---

## Example: Complete Data Flow

### Input Candidate

```python
{
    "candidate_id": "C001",
    "interview_transcript": "I have 5 years of Python backend development...",
    "skills": [
        "Python3.9", "Python Programming",  # Duplicates
        "Docker Container", "Containerization",  # Duplicates
        "K8s", "PostgreSQL", "FastAPI"
    ],
    "skill_scores": {
        "Python3.9": 85,
        "Python Programming": 88,
        "Docker Container": 78,
        "Containerization": 75,
        "K8s": 45,
        "PostgreSQL": 82,
        "FastAPI": 72
    }
}
```

### After Phase 1: Normalization

```python
{
    "candidate_id": "C001",
    "normalized_skills": ["python", "docker", "kubernetes", "postgresql", "fastapi"],
    "normalized_scores": {
        "python": 88,      # max(85, 88)
        "docker": 78,      # max(78, 75)
        "kubernetes": 45,
        "postgresql": 82,
        "fastapi": 72
    }
}
```

### After Phase 2: Feature Engineering

```python
{
    "candidate_id": "C001",
    "feature_vector": [88, 78, 45, 82, 72, ...],  # 600 dimensions
    "domain_profile": {
        "Backend": 80.7,  # Strong
        "DevOps": 61.5,   # Moderate
        "Database": 82.0  # Strong
    }
}
```

### After Phase 4: Clustering

```python
{
    "candidate_id": "C001",
    "cluster_id": 3,
    "confidence": 0.92
}
```

### Final Output: Cluster Profile

```python
{
    "cluster_id": 3,
    "label": "StrongPython_StrongPostgreSQL_WeakKubernetes",
    "natural_language": "Backend Database Specialists with strong Python and SQL skills, but lacking Kubernetes/orchestration experience",

    "profile": {
        "strengths": ["python", "postgresql", "docker"],
        "weaknesses": ["kubernetes", "system design"],
        "best_fit_roles": ["Backend Engineer", "API Developer", "Database Engineer"],
        "growth_areas": ["Kubernetes", "Distributed Systems", "System Design"]
    },

    "size": 145,
    "members": ["C001", "C023", "C045", ...]
}
```

---

## All Possible Cluster Labels (Examples)

### Backend-Focused Clusters

```
1. StrongPython_StrongDocker_WeakKubernetes
2. StrongJava_StrongSpring_WeakMicroservices
3. StrongNodejs_StrongExpress_WeakDatabase
4. StrongGo_StrongKubernetes_WeakFrontend
```

### Frontend-Focused Clusters

```
5. StrongReact_StrongJavaScript_WeakBackend
6. StrongVue_StrongTypeScript_WeakTesting
7. StrongAngular_StrongRxJS_WeakPerformance
8. StrongReact_StrongNextjs_StrongTypeScript_RoundedSkills
```

### Full-Stack Clusters

```
9. RoundedFullStack_ShallowDepth
10. StrongReact_StrongNodejs_WeakDevOps
11. StrongJavaScript_StrongPython_BalancedProfile
```

### Data/ML Clusters

```
12. StrongPython_StrongMachineLearning_WeakDeepLearning
13. StrongPandas_StrongSQLAnalysis_WeakVisualization
14. StrongTensorFlow_StrongPyTorch_SpecializedML
```

### DevOps Clusters

```
15. StrongKubernetes_StrongAWS_WeakProgramming
16. StrongDocker_StrongTerraform_WeakMonitoring
17. StrongJenkins_StrongGit_WeakCloud
```

### Generalist Clusters

```
18. WellRounded_ModerateDepth
19. BroadSkillSet_ShallowExpertise
20. MultiDomain_BalancedProfile
```

---

## Summary Table

| Phase | Input | Output | Purpose |
|-------|-------|--------|---------|
| **Phase 1** | Raw skills + scores | Normalized skills + scores | Clean data |
| **Phase 2** | Normalized data | Feature vectors (600 dims) | Extract patterns |
| **Phase 3** | Feature vectors | Embeddings (25 dims) | Reduce dimensions |
| **Phase 4** | Embeddings | Cluster assignments | Group candidates |
| **Phase 5** | Clusters + features | Multi-label profiles | Interpret clusters |

---

## Data Format Validation

### Minimum Required Fields

```python
# Minimal valid input
{
    "candidate_id": "C001",  # Required
    "skills": ["Python", "Docker"],  # Required
    "skill_scores": {"Python": 85, "Docker": 78}  # Required
}

# Full input (recommended)
{
    "candidate_id": "C001",
    "name": "John Doe",  # Optional
    "email": "john@example.com",  # Optional
    "experience_years": 5,  # Optional
    "interview_transcript": "...",  # Optional but recommended
    "question": "...",  # Optional
    "skills": [...],  # Required
    "skill_scores": {...}  # Required
}
```

### Validation Rules

✅ `candidate_id` must be unique
✅ `skills` must be non-empty list
✅ `skill_scores` must have scores for all skills in `skills` list
✅ Scores should be numeric (0-100 or 0-1, but consistent)
⚠️ Missing `interview_transcript` → NLP features won't be extracted
⚠️ Duplicate skills → Will be normalized and aggregated

---

## Next Steps

1. **Phase 1 (Complete)** ✅
   - Input: Raw skills + scores
   - Output: Normalized skills + scores

2. **Phase 2 (Next)** 🚧
   - Input: Normalized data
   - Output: Feature vectors with weakness modeling

3. **Phase 3-5 (Upcoming)** 📋
   - Clustering and multi-label generation

Ready to implement Phase 2? 🚀
