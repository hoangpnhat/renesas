# Quick Reference - Hệ Thống Phân Cụm Ứng Viên

## 🎯 Mục Tiêu
Phân cụm ứng viên thành các nhóm có ý nghĩa dựa trên skills và điểm số

---

## 📋 3 Phases Chính

### Phase 1: Skill Normalization (Chuẩn hóa kỹ năng)
**Input:** Skills nhiễu từ LLM
**Output:** Skills chuẩn hóa
**Tại sao:** Giải quyết duplicate skills ("Python3.9", "Python Programming" → "python")

**3 Tiers:**
1. **Tier 1**: Rule-based (typos, versions, abbreviations)
2. **Tier 2**: Embedding clustering (semantic similarity)
3. **Tier 3**: Co-occurrence graph (context disambiguation)

**Kết quả:** Giảm 40-60% số skills duy nhất, 95%+ accuracy

---

### Phase 2: Feature Engineering (Kỹ thuật đặc trưng)
**Input:** Skills chuẩn hóa
**Output:** Feature vectors (158 × 1,902)
**Tại sao:** Algorithms cần numerical features, không thể dùng skills thô

**6 loại features:**
1. **Core skills**: skill_python, skill_react, ...
2. **Inferred skills**: Suy diễn từ skills khác
3. **Domain aggregations**: domain_backend_avg, domain_frontend_avg, ...
4. **Weaknesses**: weakness_count, weakness_caching, ... ⭐ CRITICAL
5. **Profile characteristics**: breadth_score, specialization_index, ...
6. **Statistical**: score_mean, score_std, ...

**Kết quả:** 1,902 features ready for clustering

---

### Phase 3: Clustering (Phân cụm)
**Input:** Feature matrix
**Output:** Clusters với labels có ý nghĩa
**Tại sao:** Nhóm ứng viên giống nhau, gắn nhãn dễ hiểu cho HR

**3 Steps:**
1. **Dimensionality reduction**: 1,902 → 100 (PCA) → 25 (UMAP)
2. **Clustering**: HDBSCAN + GMM ensemble
3. **Label generation**: "StrongAws_StrongSpringboot"

---

## 🔄 Phase 3 Versions

| Version | Clusters | Multi-label | Khi nào dùng | Status |
|---------|----------|-------------|--------------|--------|
| **v1** | 7 | ❌ | Basic clustering | ✅ |
| **v2** ⭐ | 7 | ❌ | **Production** (RECOMMENDED) | ✅ |
| **v3** | 12-15 | ✅ | Multi-label (probability) | ✅ |
| **v4** | 12-18 | ✅ | Multi-label (distance) | ✅ |

### Khác biệt chính:

**v1 → v2:** Improved label quality
- Loại bỏ generic skills ("backend", "frontend")
- Focus vào technical skills cụ thể (Python, React, AWS)
- Nới lỏng weakness threshold (< 65 thay vì < 60)

**v2 → v3:** Multi-label + More clusters
- 1 candidate có thể thuộc nhiều clusters
- Dùng GMM probabilities
- 12-15 clusters thay vì 7

**v3 → v4:** Distance-based assignment
- Dùng distance to centroids thay vì probabilities
- Dễ giải thích hơn
- 12-18 clusters

---

## 📊 Kết Quả Thực Tế (v2)

```
Dataset: 158 candidates, 1,902 features

Cluster 1: StrongAws_StrongSpringboot (85 candidates) 🌟
  → Backend/Cloud generalists

Cluster 2: StrongReact_WeakCaching_WeakAlgorithms (13 candidates)
  → Frontend developers, cần training Caching

Cluster 3: StrongC++_WeakApidesign (17 candidates)
  → C++ specialists, cần training API Design

Cluster 0: StrongKafka_WeakDatamodelevolution (5 candidates)
  → Kafka specialists
```

---

## 🚀 Quick Start

### Chạy toàn bộ pipeline:

```bash
# Phase 1: Normalize skills
python apply_phase1_tier2.py

# Phase 2: Extract features
python apply_phase2_features.py

# Phase 3: Cluster (chọn version)
python apply_phase3_clustering_v2.py  # RECOMMENDED
```

### Query clusters:

```python
# Load results
with open('data/processed/clusters_final.json') as f:
    clusters = json.load(f)

# Find AWS specialists
aws_clusters = [c for c in clusters if 'aws' in c['label'].lower()]

# Find candidates needing caching training
weak_caching = [c for c in clusters if 'caching' in c['weaknesses']]
```

---

## ✅ Checklist

- ✅ Phase 1: COMPLETE (skill normalization)
- ✅ Phase 2: COMPLETE (feature engineering)
- ✅ Phase 3 v1: COMPLETE (basic)
- ✅ Phase 3 v2: COMPLETE (improved) ⭐ RECOMMENDED
- ✅ Phase 3 v3: COMPLETE (multi-label probability)
- ✅ Phase 3 v4: COMPLETE (multi-label distance)

---

## 🎯 Recommendation

**Cho production:** Dùng **Phase 3 v2**
- ✅ Single-label đủ cho hầu hết use cases
- ✅ Labels rõ ràng nhất
- ✅ Dễ maintain

**Nếu cần multi-label:** Dùng **Phase 3 v4**
- ✅ Distance-based dễ giải thích hơn v3

---

## 📚 Documents

- `PHASES_OVERVIEW.md` - Giải thích chi tiết tất cả phases
- `README.md` - Tổng quan dự án
- `PHASE1_SUMMARY.md` - Chi tiết Phase 1
- `clustering_summary.md` - Kết quả Phase 3
- `QUICK_REFERENCE.md` (file này) - Quick reference

---

## ❓ Tại Sao Cần 3 Phases?

```
❌ Không có Phase 1:
   → Duplicate skills → Kết quả sai

❌ Không có Phase 2:
   → Không có features → Không chạy clustering được

❌ Không có Phase 3:
   → Không có clusters → Không trả lời được "ứng viên thuộc nhóm nào?"

✅ Cả 3 Phases:
   → Clean data + Rich features + Meaningful clusters
   → Actionable insights cho HR!
```

---

**Status:** All phases COMPLETE ✅
**Next:** Deploy inference pipeline
