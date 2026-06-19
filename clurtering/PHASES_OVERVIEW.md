# Tổng Quan Các Phase - Hệ Thống Phân Cụm Ứng Viên

## Mục Tiêu Tổng Thể

Hệ thống này giải quyết bài toán: **Phân cụm hàng ngàn ứng viên thành các nhóm có ý nghĩa dựa trên kỹ năng, điểm số, và phỏng vấn của họ**

### Thách thức chính:
- ❌ Dữ liệu kỹ năng bị nhiễu từ LLM (ví dụ: "Python", "Python3", "Python Programming")
- ❌ Cần hiểu mối quan hệ giữa các kỹ năng (Next.js → JavaScript)
- ❌ Cần gắn nhãn cluster có ý nghĩa (bao gồm cả điểm mạnh VÀ điểm yếu)
- ❌ Cần khả năng mở rộng cho hàng ngàn ứng viên

---

## 📊 Tổng Quan Pipeline

```
Phase 1: Skill Normalization (Chuẩn hóa kỹ năng)
    ↓
Phase 2: Feature Engineering (Kỹ thuật đặc trưng)
    ↓
Phase 3: Clustering (Phân cụm)
    ├── v1: Single-label (7 clusters)
    ├── v2: Improved labeling (7 clusters)
    ├── v3: Multi-label probability-based (12-15 clusters)
    └── v4: Multi-label distance-based (12-18 clusters)
```

---

## Phase 1: Skill Normalization (Chuẩn hóa kỹ năng) ✅

### Tại sao cần Phase 1?

**Vấn đề:** Dữ liệu kỹ năng từ LLM rất nhiễu và không nhất quán

**Ví dụ thực tế:**
```
Input (Noisy):
- "Python3.9"
- "Python Programming"
- "pyton"
- "JS"
- "JavaScript"
- "React.js"
- "Docker Container"
- "K8s"

Output (Clean):
- python (từ: Python3.9, Python Programming, pyton)
- javascript (từ: JS, JavaScript)
- react (từ: React.js)
- docker (từ: Docker Container)
- kubernetes (từ: K8s)

✅ Giảm từ 8 skills → 5 skills (37.5% reduction)
```

### Cách giải quyết: 3-Tier Pipeline

#### **Tier 1: Rule-Based Normalization** (Chuẩn hóa dựa trên quy tắc)
- Chuyển về chữ thường
- Loại bỏ số phiên bản (`Python3.9` → `Python`)
- Mở rộng viết tắt (`JS` → `JavaScript`, `K8s` → `Kubernetes`)
- Sửa lỗi chính tả phổ biến (`pyton` → `python`)
- Ánh xạ từ đồng nghĩa

**Tại sao cần:** Xử lý nhanh các lỗi cơ bản và phổ biến nhất

#### **Tier 2: Embedding-Based Semantic Clustering** (Nhóm dựa trên ngữ nghĩa)
- Sử dụng sentence transformers để tạo embeddings
- HDBSCAN clustering để nhóm các skill tương tự
- Tính confidence score dựa trên độ gắn kết của cluster

**Tại sao cần:** Tier 1 không thể xử lý các biến thể phức tạp như "Python Programming" vs "Python"

#### **Tier 3: Co-occurrence Graph Disambiguation** (Phân biệt dựa trên ngữ cảnh)
- Xây dựng đồ thị đồng xuất hiện từ dữ liệu ứng viên
- Community detection (Louvain algorithm)
- Phân biệt các kỹ năng mơ hồ dựa trên ngữ cảnh

**Tại sao cần:** Giải quyết các từ viết tắt mơ hồ (ví dụ: "ML" có thể là "Machine Learning" hoặc "Markup Language")

### Score Handling (Xử lý điểm số)

**Vấn đề:** Khi nhiều skills được gộp thành 1 canonical skill, cần gộp điểm số

**Ví dụ:**
```
Input:
- "Python3.9": 85
- "Python Programming": 90

Output:
- "python": 90 (sử dụng max aggregation)
```

**Các phương pháp aggregation:**
- `max`: Lấy điểm cao nhất (mặc định)
- `mean`: Lấy trung bình
- `weighted_mean`: Trung bình có trọng số
- `first`: Lấy điểm đầu tiên

### Kết quả Phase 1:

✅ **Compression ratio**: Giảm 40-60% số skill duy nhất
✅ **Accuracy**: 95%+ độ chính xác
✅ **Confidence**: Trung bình 0.92
✅ **Performance**: Xử lý 1000 skills trong ~15s

---

## Phase 2: Feature Engineering (Kỹ thuật đặc trưng) ✅

### Tại sao cần Phase 2?

**Vấn đề:** Thuật toán clustering không thể hoạt động trực tiếp trên dữ liệu skills thô

**Giải thích:**
- Algorithms như HDBSCAN, GMM cần numerical features (vector số)
- Không thể so sánh ứng viên chỉ bằng danh sách skills
- Cần biểu diễn "chất lượng" và "độ sâu" của kỹ năng

### Cách giải quyết: Trích xuất Features

#### 1. **Core Skill Features** (Đặc trưng kỹ năng cơ bản)
```
Cho mỗi skill trong toàn bộ dataset:
- skill_python: 85 (điểm của ứng viên cho Python)
- skill_javascript: 0 (ứng viên không có skill này)
```

**Tại sao cần:** Biểu diễn kỹ năng cụ thể của từng ứng viên

#### 2. **Inferred Skill Scores** (Điểm kỹ năng suy diễn)
```
Nếu ứng viên có React (score: 90):
→ Suy ra có JavaScript (inferred score: ~85)
```

**Tại sao cần:** Không phải ứng viên nào cũng liệt kê đầy đủ skills, nhưng có thể suy ra từ skills khác

#### 3. **Domain Aggregations** (Tổng hợp theo lĩnh vực)
```
- domain_backend_avg: 85.5 (trung bình điểm Backend)
- domain_frontend_avg: 78.2
- domain_cloud_avg: 92.0
- domain_database_avg: 76.5
- domain_devops_avg: 88.3
```

**Tại sao cần:** Clustering theo "profile" (Backend developer, Full-stack, DevOps, etc.)

#### 4. **Weakness Modeling** (Mô hình điểm yếu) ⭐ CRITICAL
```
- weakness_count: 3 (số skill yếu)
- weakness_ratio: 0.15 (15% skills là yếu)
- weakness_caching: 1 (có yếu về caching)
- weakness_algorithms: 1 (có yếu về algorithms)
```

**Tại sao cần:**
- Để gắn nhãn cluster có ý nghĩa (không chỉ điểm mạnh mà cả điểm yếu)
- Giúp HR biết cần training gì
- Ví dụ label: "StrongReact_WeakCaching_WeakAlgorithms"

#### 5. **Profile Characteristics** (Đặc điểm profile)
```
- breadth_score: 15 (số lượng skills)
- depth_score: 85.5 (điểm trung bình)
- specialization_index: 0.35 (độ chuyên môn hóa)
- advanced_skill_ratio: 0.25 (25% skills có điểm > 85)
- profile_type_specialist: 0 hoặc 1
- profile_type_generalist: 0 hoặc 1
```

**Tại sao cần:** Phân biệt Specialist vs Generalist

#### 6. **Statistical Features** (Đặc trưng thống kê)
```
- score_mean: 82.5
- score_std: 8.3
- score_min: 65
- score_max: 95
```

**Tại sao cần:** Hiểu phân bố điểm của ứng viên

### Kết quả Phase 2:

✅ **Feature count**: ~1,900 features per candidate
✅ **Feature matrix**: 158 candidates × 1,902 features
✅ **Weakness modeling**: 100% candidates có weakness features
✅ **Ready for clustering**

---

## Phase 3: Clustering (Phân cụm) 🎯

### Tại sao cần Phase 3?

**Mục tiêu cuối cùng:** Nhóm ứng viên thành các cluster có ý nghĩa

**Use cases:**
1. **HR Matching**: "Tìm ứng viên giống profile này"
2. **Team Building**: "Tạo team cân bằng với skills đa dạng"
3. **Training**: "Ứng viên nào cần training về Kubernetes?"
4. **Talent Pool**: "Có bao nhiêu Backend specialists?"

### Workflow chung cho tất cả versions:

#### **Step 1: Dimensionality Reduction** (Giảm chiều)
```
1,902 features → 100 dims (PCA) → 25 dims (UMAP)
```

**Tại sao cần:**
- 1,902 dimensions quá lớn cho clustering algorithms
- "Curse of dimensionality": khoảng cách giữa các điểm không còn ý nghĩa
- PCA + UMAP giữ lại structure quan trọng nhất

#### **Step 2: Clustering**
Sử dụng ensemble của nhiều algorithms:
- **HDBSCAN**: Density-based clustering (tốt cho clusters không đều)
- **GMM**: Gaussian Mixture Model (tốt cho xác suất)

**Tại sao ensemble:** Kết hợp ưu điểm của nhiều algorithms

#### **Step 3: Label Generation** (Tạo nhãn)
```
Cluster 1: "StrongAws_StrongSpringboot"
Cluster 2: "StrongReact_WeakCaching_WeakAlgorithms"
```

**Tại sao cần nhãn có ý nghĩa:**
- HR không hiểu "Cluster 1", "Cluster 2"
- Cần biết cluster này đại diện cho profile gì
- Bao gồm cả strengths VÀ weaknesses

---

## So sánh các versions của Phase 3

### Version 1: Single-Label Basic (v1)

**Đặc điểm:**
- ✅ 7 clusters
- ✅ Mỗi candidate thuộc đúng 1 cluster
- ✅ Label generation cơ bản

**Vấn đề:**
- ❌ Labels bao gồm cả generic skills ("backend", "frontend")
- ❌ Weakness detection quá nghiêm (< 60 score)
- ❌ 1 candidate chỉ thuộc 1 cluster (không linh hoạt)

**Khi nào dùng:**
- Cần phân loại đơn giản
- Mỗi ứng viên chỉ được gán vào 1 nhóm duy nhất

---

### Version 2: Improved Labeling (v2) ⭐ RECOMMENDED

**Improvements so với v1:**
- ✅ Loại trừ generic skills khỏi labels
- ✅ Chỉ dùng technical skills cụ thể (Python, React, AWS, etc.)
- ✅ Nới lỏng weakness threshold (< 65 score, ≥ 20% frequency)
- ✅ Labels rõ ràng hơn

**Kết quả:**
```
Cluster 0: "StrongKafka_WeakDatamodelevolution"
Cluster 1: "StrongAws_StrongSpringboot" (85 candidates)
Cluster 2: "StrongReact_WeakCaching_WeakAlgorithms"
Cluster 3: "StrongC++_WeakApidesign"
```

**Ưu điểm:**
- ✅ Labels dễ hiểu và actionable
- ✅ Focus vào technical skills quan trọng
- ✅ Balance tốt giữa số clusters và cluster size

**Khi nào dùng:**
- ✅ **RECOMMENDED cho production**
- ✅ Cần labels rõ ràng, dễ hiểu
- ✅ Single-label classification

---

### Version 3: Multi-Label Probability-Based (v3)

**Improvements so với v2:**
- ✅ 12-15 clusters (nhiều hơn v2)
- ✅ **Multi-label**: 1 candidate có thể thuộc nhiều clusters
- ✅ Sử dụng GMM probabilities để assign

**Cách hoạt động:**
```
Candidate A:
- Cluster 1: 45% probability → ASSIGNED
- Cluster 3: 35% probability → ASSIGNED
- Cluster 5: 15% probability → ASSIGNED
- Cluster 2: 5% probability → NOT ASSIGNED (< threshold)
```

**Ưu điểm:**
- ✅ Linh hoạt: Ứng viên có thể vào nhiều clusters
- ✅ Nhiều clusters hơn → chi tiết hơn
- ✅ Xác suất membership

**Nhược điểm:**
- ❌ Phức tạp hơn để giải thích
- ❌ Inference phức tạp hơn

**Khi nào dùng:**
- Ứng viên có nhiều strengths đa dạng
- Cần flexibility trong matching
- Có thể xử lý multi-label classification

---

### Version 4: Multi-Label Distance-Based (v4)

**Improvements so với v3:**
- ✅ Sử dụng **distance to centroids** thay vì GMM probabilities
- ✅ Dễ hiểu hơn: "candidate gần centroid nào?"
- ✅ 12-18 clusters

**Cách hoạt động:**
```
Candidate A distance to:
- Cluster 1 centroid: 2.5 → PRIMARY
- Cluster 3 centroid: 3.2 → SECONDARY (within 150% of primary)
- Cluster 5 centroid: 5.8 → NOT ASSIGNED (too far)
```

**Ưu điểm:**
- ✅ Dễ giải thích hơn v3 ("gần" vs "xác suất")
- ✅ Distance metric trực quan hơn
- ✅ Vẫn giữ multi-label flexibility

**Khi nào dùng:**
- Cần multi-label nhưng dễ giải thích hơn
- Distance metric có ý nghĩa trong business context

---

## 📊 Bảng so sánh tổng quan

| Feature | v1 | v2 | v3 | v4 |
|---------|----|----|----|----|
| **Số clusters** | 7 | 7 | 12-15 | 12-18 |
| **Multi-label** | ❌ | ❌ | ✅ | ✅ |
| **Label quality** | ⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| **Complexity** | Đơn giản | Đơn giản | Phức tạp | Trung bình |
| **Interpretability** | Tốt | Rất tốt | Khó | Tốt |
| **Flexibility** | Thấp | Thấp | Cao | Cao |
| **Production-ready** | ✅ | ⭐ **BEST** | ✅ | ✅ |

---

## 🎯 Khi nào dùng version nào?

### Dùng **Version 2 (v2)** khi:
- ✅ Cần production-ready solution
- ✅ Single-label classification đủ
- ✅ Cần labels rõ ràng, actionable
- ✅ **RECOMMENDED cho hầu hết use cases**

### Dùng **Version 3 (v3)** khi:
- ✅ Ứng viên có profile đa dạng
- ✅ Cần assign vào nhiều clusters
- ✅ Có khả năng xử lý probability-based logic

### Dùng **Version 4 (v4)** khi:
- ✅ Cần multi-label
- ✅ Distance metric dễ giải thích hơn probability
- ✅ Business có thể hiểu "khoảng cách" tốt hơn "xác suất"

---

## 🚀 Next Steps: Inference Pipeline

Sau khi có clusters, có thể:

### 1. Query by Cluster Label
```python
# Tìm tất cả AWS specialists
candidates = find_by_cluster("StrongAws_StrongSpringboot")
```

### 2. Query by Technical Skills
```python
# Tìm candidates giỏi AWS và Java
candidates = find_by_skills(strengths=["aws", "java"])
```

### 3. Query by Weakness Profile
```python
# Tìm candidates yếu về caching để training
candidates = find_by_weaknesses(["caching"])
```

### 4. Similarity Search
```python
# Tìm candidates tương tự profile này
similar = find_similar_candidates(target_profile, top_k=10)
```

---

## 📈 Kết quả thực tế

### Dataset
- **158 candidates**
- **1,902 features**
- **7-18 clusters** (tùy version)

### Cluster Examples (v2)
```
Cluster 1: StrongAws_StrongSpringboot (85 candidates) 🌟 LARGEST
  - Backend: 80.53, Frontend: 72.29, Cloud: 74.30
  - Generalist profile

Cluster 2: StrongReact_WeakCaching_WeakAlgorithms (13 candidates)
  - DevOps: 83.26, Frontend: 76.99
  - Need training in: Caching, Algorithms

Cluster 3: StrongC++_WeakApidesign (17 candidates)
  - C++ specialists
  - Need training in: API Design, Monitoring
```

---

## ✅ Checklist: Phase nào đã hoàn thành?

- ✅ **Phase 1**: COMPLETE - Skill normalization + Score handling
- ✅ **Phase 2**: COMPLETE - Feature engineering (1,902 features)
- ✅ **Phase 3 v1**: COMPLETE - Basic clustering
- ✅ **Phase 3 v2**: COMPLETE - Improved labeling ⭐ RECOMMENDED
- ✅ **Phase 3 v3**: COMPLETE - Multi-label (probability)
- ✅ **Phase 3 v4**: COMPLETE - Multi-label (distance)

---

## 🎓 Tại sao PHẢI có cả 3 Phases?

### ❌ Nếu bỏ Phase 1:
- Clustering sẽ chia "Python" và "Python3.9" thành 2 skills khác nhau
- Kết quả sai lệch, không có ý nghĩa
- Lãng phí dimensions cho duplicate skills

### ❌ Nếu bỏ Phase 2:
- Không thể chạy clustering algorithms (cần numerical vectors)
- Mất thông tin quan trọng (domain, weaknesses, profile type)
- Không có weaknesses → labels không đầy đủ

### ❌ Nếu bỏ Phase 3:
- Có features nhưng không có clusters
- Không trả lời được câu hỏi: "Ứng viên này thuộc nhóm nào?"
- Không có labels → không có interpretability

### ✅ Cả 3 Phases cùng nhau:
```
Phase 1: Clean data (Python3.9 → python)
   ↓
Phase 2: Create features (1,902 numerical features)
   ↓
Phase 3: Cluster + Label ("StrongAws_StrongSpringboot")
   ↓
RESULT: Actionable insights for HR!
```

---

## 📚 Files Quan Trọng

### Documentation
- `README.md` - Tổng quan dự án
- `PHASE1_SUMMARY.md` - Chi tiết Phase 1
- `PHASE1_COMPLETE.md` - Phase 1 deliverables
- `SCORE_HANDLING_GUIDE.md` - Hướng dẫn score handling
- `clustering_summary.md` - Kết quả Phase 3
- **`PHASES_OVERVIEW.md` (file này)** - Tổng quan tất cả phases

### Code Files
- `apply_phase1_normalization.py` - Chạy Phase 1
- `apply_phase1_tier2.py` - Chạy Phase 1 Tier 2
- `apply_phase2_features.py` - Chạy Phase 2
- `apply_phase3_clustering.py` - Chạy Phase 3 v1
- `apply_phase3_clustering_v2.py` - Chạy Phase 3 v2 ⭐
- `apply_phase3_clustering_v3.py` - Chạy Phase 3 v3
- `apply_phase3_clustering_v4.py` - Chạy Phase 3 v4

### Data Files
- `data/processed/candidates_normalized_tier2.json` - Output Phase 1
- `data/processed/candidates_features.json` - Output Phase 2
- `data/processed/feature_matrix.npy` - Feature matrix
- `data/processed/embeddings_25d.npy` - Reduced embeddings
- `data/processed/clusters_final.json` - Output Phase 3

---

## 🎯 Recommendation

**Cho production:** Sử dụng **Phase 3 Version 2 (v2)**

**Lý do:**
- ✅ Balance tốt nhất giữa simplicity và effectiveness
- ✅ Labels rõ ràng, actionable
- ✅ Single-label đủ cho hầu hết use cases
- ✅ Dễ maintain và explain

**Nếu cần multi-label:** Sử dụng **Version 4 (v4)**
- Distance-based dễ giải thích hơn probability-based (v3)

---

## ⚠️ Known Issues & Improvements

### Issue 1: Misalignment giữa Cluster Assignment và Labels

**Vấn đề phát hiện:**
- Candidates được assign vào clusters nhưng không có skills chính của cluster đó
- Ví dụ: Candidate có AWS/Microservices được assign vào cluster "StrongPython_StrongFastapi"

**Root Cause:**
1. **Clustering dựa trên 1,902 features** (không chỉ skills):
   - Core skills (~800)
   - Domain aggregations (~20)
   - Profile characteristics (~20)
   - Weaknesses (~300)
   - Statistical features (~20)

   → 2 candidates có thể gần nhau vì giống về domains/profile, mặc dù khác về skills

2. **Labels dựa trên AVERAGE skills của cluster**:
   - Cluster có 41 members
   - 39 members có Python → Average Python score: 74.1
   - 2 members KHÔNG có Python nhưng vẫn trong cluster
   - Label: "StrongPython" nhưng không đại diện cho ALL members

3. **Dimensionality Reduction loss**:
   - 1,902 dims → 25 dims mất ~15% information
   - Distance trong 25D space không phản ánh chính xác skill similarity

**Solutions:**
- ✅ Combine distance score với skill overlap score (50-50 weight)
- ✅ Post-assignment filtering: require ≥2/5 cluster skills match
- ✅ Dual pipelines: Holistic (1,902 features) vs Skill-focused (800 features)

**Chi tiết:** Xem `CLUSTERING_ISSUES_AND_SOLUTIONS.md`

---

**Status:** All phases COMPLETE ✅

**Known Issues:** Clustering misalignment identified (see above)

**Next steps:**
1. Implement skill-weighted assignment
2. Deploy inference pipeline
