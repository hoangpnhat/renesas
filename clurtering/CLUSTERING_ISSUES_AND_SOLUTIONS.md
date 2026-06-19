# Vấn Đề Clustering: Misalignment giữa Assignment và Labels

## 🚨 Vấn Đề Phát Hiện

### Case Study: Candidate với AWS/Full-stack Skills

```
CANDIDATE: 0262224e-1253-4624-830e-8448d9a7db2a

Top Skills:
- amazon web services (AWS): 85.0
- full-stack development: 82.8
- real-time communication: 81.0
- cross-device compatibility: 79.8
- microservices: 79.3

ASSIGNED TO: cluster_8
CLUSTER LABEL: "StrongPython_StrongFastapi_StrongKubernetes_WeakNodejs"

MATCHING STRENGTHS:
✗ python          | Cluster: 74.1 | Candidate: 0.0
✗ fastapi         | Cluster: 76.6 | Candidate: 0.0
✗ kubernetes      | Cluster: 74.9 | Candidate: 0.0
✗ deep learning   | Cluster: 72.8 | Candidate: 0.0
✓ postgresql      | Cluster: 72.4 | Candidate: 75.1  ← Only 1 match!
```

### ❌ Vấn đề

Candidate này:
- **KHÔNG CÓ** Python, FastAPI, Kubernetes (những skills chính của cluster)
- **CHỈ CÓ** AWS, Full-stack, Microservices
- Nhưng vẫn được assign vào cluster "Python/FastAPI/Kubernetes"

**Tại sao lại như vậy?**

---

## 🔍 Root Cause Analysis

### Nguyên nhân 1: Clustering trong Embedding Space vs Label từ Raw Skills

```
┌─────────────────────────────────────────────────────────────┐
│  CLUSTER ASSIGNMENT (dựa trên embeddings 25D)               │
│  ────────────────────────────────────────────────────────   │
│  1. Features: 1,902 dims (skill scores + domains + ...)    │
│  2. PCA: 1,902 → 100 dims                                   │
│  3. UMAP: 100 → 25 dims                                     │
│  4. Distance-based clustering trong 25D space               │
│                                                              │
│  → Candidate được assign dựa trên TỔNG THỂ features        │
│  → Không chỉ skills, mà cả domains, profile type, etc.     │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│  CLUSTER LABEL (dựa trên raw skills của members)            │
│  ────────────────────────────────────────────────────────   │
│  1. Lấy TẤT CẢ members của cluster                         │
│  2. Tính AVERAGE score của mỗi skill                       │
│  3. Pick top skills có avg score cao nhất                  │
│                                                              │
│  → Label dựa trên AVERAGE của cluster                      │
│  → Individual member có thể KHÔNG CÓ các skills này        │
└─────────────────────────────────────────────────────────────┘
```

### Ví dụ minh họa

```
Cluster 8 có 41 members:

Member 1: Python (90), FastAPI (85), Kubernetes (80)
Member 2: Python (88), FastAPI (82), PostgreSQL (75)
Member 3: Python (85), Kubernetes (78), Deep Learning (72)
...
Member 40: AWS (85), Microservices (79), PostgreSQL (75) ← Outlier!
Member 41: Full-stack (83), Real-time (81), Microservices (79) ← Outlier!

AVERAGE của cluster:
- Python: 74.1 (vì 39/41 members có Python)
- FastAPI: 76.6 (vì 35/41 members có FastAPI)
- Kubernetes: 74.9 (vì 37/41 members có Kubernetes)

→ Cluster label: "StrongPython_StrongFastapi_StrongKubernetes"

Nhưng Member 40 và 41 KHÔNG CÓ Python/FastAPI/Kubernetes!
```

### Nguyên nhân 2: Dimensionality Reduction Loss

```
1,902 features → 25 dimensions

Information Loss:
- PCA giữ lại ~85% variance
- UMAP giữ lại local/global structure
- Nhưng mất đi ~15% thông tin chi tiết

Hệ quả:
→ 2 candidates khác nhau về skills có thể gần nhau trong 25D space
→ Vì họ giống nhau về OTHER features (domains, profile type, etc.)
```

### Nguyên nhân 3: Distance-based Assignment không xét đến Skill Overlap

```
Cluster Assignment v4 (Distance-based):

1. Tính distance từ candidate đến cluster centroid trong 25D space
2. Assign nếu distance < threshold
3. KHÔNG XÉT skill overlap!

→ Candidate có thể gần cluster về "profile" nhưng khác về skills
```

---

## 💡 Tại Sao Vấn Đề Này Xảy Ra?

### Trade-off giữa Rich Features và Interpretability

**Phase 2 tạo 1,902 features** bao gồm:
- Core skill features (~800)
- Domain aggregations (~10-20)
- Profile characteristics (~10-20)
- Weaknesses (~100-300)
- Statistical features (~20)
- Inferred skills (~200-400)

**→ Clustering dựa trên TẤT CẢ features này**

Ví dụ 2 candidates có thể gần nhau vì:
- Giống nhau về domain_backend_avg, domain_devops_avg
- Giống nhau về breadth_score, specialization_index
- Giống nhau về weakness patterns

**Mặc dù khác nhau về specific skills!**

---

## ✅ Giải Pháp

### Solution 1: Skill-Weighted Cluster Assignment (RECOMMENDED)

Thay vì chỉ dùng distance, kết hợp với skill overlap:

```python
def assign_cluster_with_skill_check(candidate, cluster):
    # 1. Distance score (như hiện tại)
    distance_score = 1 / (1 + distance_to_centroid)

    # 2. Skill overlap score (NEW!)
    cluster_top_skills = get_top_skills(cluster, top_k=10)
    candidate_skills = candidate['normalized_skills']

    overlap_count = len(set(cluster_top_skills) & set(candidate_skills))
    skill_overlap_score = overlap_count / len(cluster_top_skills)

    # 3. Combined score
    final_score = (0.5 * distance_score) + (0.5 * skill_overlap_score)

    return final_score
```

**Ưu điểm:**
- Đảm bảo candidate có ít nhất một số skills chính của cluster
- Vẫn giữ được benefit của rich features
- Balance giữa similarity và interpretability

### Solution 2: Post-Assignment Filtering

Sau khi assign, filter out candidates không match cluster label:

```python
def filter_misaligned_assignments(candidate, cluster):
    cluster_strengths = cluster['profile']['strengths'][:5]

    # Check if candidate has at least 2/5 cluster strengths
    match_count = 0
    for strength in cluster_strengths:
        if candidate['normalized_scores'].get(strength['skill'], 0) >= 65:
            match_count += 1

    if match_count < 2:
        # Reassign to different cluster or mark as outlier
        return False
    return True
```

**Ưu điểm:**
- Đảm bảo candidates trong cluster thực sự có skills của cluster
- Dễ implement

**Nhược điểm:**
- Có thể tạo ra nhiều outliers

### Solution 3: Cluster Labels dựa trên MAJORITY Skills (không phải AVERAGE)

Thay vì lấy average score, lấy skills mà MAJORITY members có:

```python
def generate_cluster_label_by_majority(cluster):
    members = cluster['members']

    # Count how many members have each skill
    skill_frequency = {}
    for member in members:
        for skill in member['normalized_skills']:
            skill_frequency[skill] = skill_frequency.get(skill, 0) + 1

    # Only include skills that >= 60% of members have
    threshold = len(members) * 0.6
    common_skills = {
        skill: freq
        for skill, freq in skill_frequency.items()
        if freq >= threshold
    }

    # Generate label from common skills
    return generate_label(common_skills)
```

**Ưu điểm:**
- Labels represent skills mà MOST members có
- Giảm outliers

**Nhược điểm:**
- Có thể tạo ra labels quá generic

### Solution 4: Hierarchical Labels (Multi-Level)

Tạo labels theo levels:

```
Cluster 8:
Level 1 (Core):     "Backend Developers"
Level 2 (Primary):  "StrongPython_StrongFastAPI" (>80% members)
Level 3 (Secondary): "WithKubernetes_WithPostgreSQL" (>50% members)
Level 4 (Diverse):  "VaryingAWS_VaryingMicroservices" (<50% members)
```

**Ưu điểm:**
- Cho phép hiểu được diversity trong cluster
- Outliers được explain ở level 4

### Solution 5: Separate Clustering cho Different Granularities

Tạo 2 clustering pipelines:

**Pipeline A: Rich-Feature Clustering** (hiện tại)
- Dùng 1,902 features
- Tốt cho finding similar candidates overall
- Nhưng labels có thể không chính xác cho specific skills

**Pipeline B: Skill-Only Clustering** (new)
- Chỉ dùng core skill features (~800 features)
- Distance based purely on skills
- Labels chính xác cho skills
- Nhưng có thể miss candidates similar về domains/profile

**Use cases:**
- Pipeline A: "Find candidates similar to this person" (holistic)
- Pipeline B: "Find candidates with these specific skills" (targeted)

---

## 🎯 Recommendation

### Immediate Fix (Short-term): Solution 1 + Solution 2

1. **Implement skill-weighted assignment** (Solution 1)
   - Combine distance score với skill overlap score
   - Weight: 50% distance + 50% skill overlap

2. **Post-assignment filtering** (Solution 2)
   - Require at least 2/5 top cluster skills
   - Reassign outliers to better clusters

### Long-term Enhancement: Solution 5

Implement dual clustering pipelines:
- **Pipeline A (Holistic)**: Current approach với 1,902 features
- **Pipeline B (Skill-focused)**: Chỉ dùng skill features

Cho phép users chọn pipeline phù hợp với use case.

---

## 📝 Implementation Example

### Code để implement Solution 1:

```python
def compute_skill_overlap_score(candidate, cluster_profile, top_k=10):
    """
    Compute skill overlap score between candidate and cluster.
    """
    cluster_top_skills = [s['skill'] for s in cluster_profile['strengths'][:top_k]]
    candidate_skills = candidate['normalized_skills']

    # Count matches
    matches = 0
    for cluster_skill in cluster_top_skills:
        if cluster_skill in candidate_skills:
            # Check if score is reasonable (>= 65)
            if candidate['normalized_scores'].get(cluster_skill, 0) >= 65:
                matches += 1

    # Normalize
    overlap_score = matches / len(cluster_top_skills) if cluster_top_skills else 0
    return overlap_score


def assign_clusters_with_skill_check(candidate, clusters, embeddings,
                                     distance_weight=0.5, skill_weight=0.5):
    """
    Assign candidate to clusters using both distance and skill overlap.
    """
    candidate_idx = get_candidate_index(candidate)
    candidate_embedding = embeddings[candidate_idx]

    scores = []
    for cluster in clusters:
        # 1. Distance score
        centroid = cluster['centroid']
        distance = np.linalg.norm(candidate_embedding - centroid)
        distance_score = 1 / (1 + distance)

        # 2. Skill overlap score
        skill_score = compute_skill_overlap_score(candidate, cluster['profile'])

        # 3. Combined score
        final_score = (distance_weight * distance_score) + (skill_weight * skill_score)

        scores.append({
            'cluster_id': cluster['cluster_id'],
            'final_score': final_score,
            'distance_score': distance_score,
            'skill_score': skill_score,
            'distance': distance
        })

    # Sort by final score
    scores.sort(key=lambda x: x['final_score'], reverse=True)

    return scores
```

---

## 📊 Expected Impact

### Before Fix:
```
Candidate: AWS + Microservices + Full-stack
Assigned to: "StrongPython_StrongFastapi_StrongKubernetes"
Skill Match: 1/5 (20%) ❌
Explanation Quality: Poor
```

### After Fix (Solution 1 + 2):
```
Candidate: AWS + Microservices + Full-stack
Assigned to: "StrongAws_StrongMicroservices_StrongNode"
Skill Match: 4/5 (80%) ✅
Explanation Quality: Good
```

---

## 🔄 Migration Plan

### Phase 1: Add Skill Overlap Scoring
1. Implement `compute_skill_overlap_score()`
2. Test on current dataset
3. Compare results với current approach

### Phase 2: Modify Cluster Assignment
1. Update `assign_clusters()` để use combined score
2. Tune weights (distance_weight vs skill_weight)
3. Re-run Phase 3 clustering

### Phase 3: Post-Assignment Filtering
1. Implement filtering logic
2. Identify and reassign outliers
3. Regenerate cluster profiles

### Phase 4: Validate Results
1. Run explain_candidate_clustering() trên sample candidates
2. Verify skill matches improved
3. Document changes

---

## ✅ Summary

**Root Cause:**
- Clustering dựa trên 1,902 features (không chỉ skills)
- Labels dựa trên AVERAGE skills của members
- Distance-based assignment không check skill overlap

**Impact:**
- Candidates được assign vào clusters mà không có skills chính
- Giảm interpretability và trustworthiness

**Solution:**
- Combine distance score với skill overlap score
- Post-assignment filtering
- Có thể consider dual pipelines (holistic vs skill-focused)

**Next Steps:**
- Implement Solution 1 + 2
- Re-run Phase 3
- Validate improvements

---

**Status:** Issue identified, solutions proposed
**Priority:** HIGH - affects interpretability and user trust
**Effort:** ~2-3 days to implement and validate
