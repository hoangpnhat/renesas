# Vấn Đề và Cải Tiến cho Phase 1 & Phase 2

## 📋 Tổng Quan Vấn Đề

### Vấn Đề 1: Phase 1 - Thiếu Sự Liên Kết Giữa Các Skills
### Vấn Đề 2: Phase 1 - Không Có Penalty Score cho Cross-Domain
### Vấn Đề 3: Phase 2 - Vector Có Quá Nhiều Giá Trị 0

---

## 🔴 VẤN ĐỀ 1: Thiếu Sự Liên Kết Giữa Các Skills

### Mô Tả Vấn Đề

```
Candidate A:
- Mention: FastAPI (score: 85)
- Không mention: Python

Kết quả:
- FastAPI: 85
- Python: 0 ← Sai! Nếu biết FastAPI thì PHẢI biết Python

Candidate B:
- Mention: React (score: 90)
- Không mention: JavaScript

Kết quả:
- React: 90
- JavaScript: 0 ← Sai! React requires JavaScript
```

### Root Cause

Evaluators chỉ đánh giá skills được **explicitly mentioned** trong interview, không infer dependencies.

### Impact

1. **Feature vectors không đầy đủ**: Missing critical foundational skills
2. **Clustering không chính xác**: 2 Python developers có thể xa nhau vì 1 người không mention "Python" explicitly
3. **Unfair comparison**: Candidates bị penalize vì không mention obvious dependencies

---

## 💡 GIẢI PHÁP 1: Skill Dependency Inference

### Approach 1: Rule-based Dependency Graph

Tạo một dependency graph cho skills:

```python
# Forward Dependencies: Specific → General (Strong inference)
SKILL_DEPENDENCIES = {
    # Framework → Language (STRONG: 必须会)
    "fastapi": {"python": 0.9},  # If FastAPI=85 → Python≥76.5 (85*0.9)
    "django": {"python": 0.9},
    "flask": {"python": 0.85},
    "react": {"javascript": 0.95},
    "vue": {"javascript": 0.9},
    "angular": {"typescript": 0.9, "javascript": 0.85},
    "next.js": {"react": 0.9, "javascript": 0.85},
    "spring": {"java": 0.9},
    "spring boot": {"java": 0.9, "spring": 0.85},
    
    # Library → Language
    "numpy": {"python": 0.85},
    "pandas": {"python": 0.85},
    "tensorflow": {"python": 0.8},
    "pytorch": {"python": 0.8},
    
    # Advanced → Foundational
    "kubernetes": {"docker": 0.8, "devops": 0.7},
    "terraform": {"devops": 0.75, "cloud": 0.7},
    "microservices": {"backend": 0.7, "api design": 0.6},
    
    # Database-specific → General DB
    "postgresql": {"sql": 0.9, "database": 0.85},
    "mysql": {"sql": 0.9, "database": 0.85},
    "mongodb": {"nosql": 0.85, "database": 0.8},
    
    # Cloud-specific → General Cloud
    "aws lambda": {"aws": 0.9, "cloud": 0.75},
    "ec2": {"aws": 0.85, "cloud": 0.7},
    "s3": {"aws": 0.8, "cloud": 0.65},
}

# Reverse Dependencies: General → Specific (Context-aware inference)
# Sử dụng domain context để infer với confidence phù hợp
SKILL_REVERSE_INFERENCE = {
    # Python → Domain-specific skills
    "python": {
        "domain_skills": {
            "backend": {
                # Backend Python developer GẦN NHƯ CHẮC CHẮN dùng một framework
                "frameworks": {
                    "django": {"score_ratio": 0.75, "confidence": 0.85},      # 75% of Python score
                    "fastapi": {"score_ratio": 0.70, "confidence": 0.80},
                    "flask": {"score_ratio": 0.65, "confidence": 0.75},
                },
                "min_python_score": 70,
                "infer_one_of": True,  # Chọn 1 framework nếu không có explicit mention
            },
            "data_science": {
                # Data Science CHẮC CHẮN dùng pandas/numpy
                "libraries": {
                    "pandas": {"score_ratio": 0.80, "confidence": 0.90},     # Gần như bắt buộc
                    "numpy": {"score_ratio": 0.75, "confidence": 0.90},
                    "matplotlib": {"score_ratio": 0.60, "confidence": 0.70},
                },
                "min_python_score": 75,
                "infer_all": True,  # Infer tất cả libraries
            },
            "machine_learning": {
                # ML Engineer CHẮC CHẮN dùng pandas + ML frameworks
                "libraries": {
                    "pandas": {"score_ratio": 0.85, "confidence": 0.95},
                    "numpy": {"score_ratio": 0.85, "confidence": 0.95},
                    "scikit-learn": {"score_ratio": 0.70, "confidence": 0.80},
                    "tensorflow": {"score_ratio": 0.60, "confidence": 0.60},  # Hoặc pytorch
                    "pytorch": {"score_ratio": 0.60, "confidence": 0.60},
                },
                "min_python_score": 80,
                "infer_all": ["pandas", "numpy"],  # Bắt buộc
                "infer_one_of": ["tensorflow", "pytorch", "scikit-learn"],  # Chọn 1
            },
            "full-stack": {
                # Full-stack có thể dùng framework nhưng confidence thấp hơn
                "frameworks": {
                    "django": {"score_ratio": 0.50, "confidence": 0.50},
                    "fastapi": {"score_ratio": 0.50, "confidence": 0.50},
                    "flask": {"score_ratio": 0.50, "confidence": 0.50},
                },
                "min_python_score": 60,
                "infer_one_of": False,  # Không bắt buộc
            }
        }
    },
    
    # JavaScript → Domain-specific skills
    "javascript": {
        "domain_skills": {
            "frontend": {
                # Frontend developer GẦN NHƯ CHẮC CHẮN dùng một framework modern
                "frameworks": {
                    "react": {"score_ratio": 0.75, "confidence": 0.80},
                    "vue": {"score_ratio": 0.70, "confidence": 0.75},
                    "angular": {"score_ratio": 0.70, "confidence": 0.75},
                },
                "min_javascript_score": 75,
                "infer_one_of": True,
            },
            "full-stack": {
                "frameworks": {
                    "react": {"score_ratio": 0.65, "confidence": 0.65},
                    "node.js": {"score_ratio": 0.70, "confidence": 0.70},
                    "express": {"score_ratio": 0.60, "confidence": 0.60},
                },
                "min_javascript_score": 70,
                "infer_one_of": False,
            },
            "backend": {
                # Backend JS → Node.js ecosystem
                "frameworks": {
                    "node.js": {"score_ratio": 0.80, "confidence": 0.85},
                    "express": {"score_ratio": 0.70, "confidence": 0.75},
                },
                "min_javascript_score": 75,
                "infer_all": ["node.js"],  # Backend JS = Node.js
            }
        }
    },
    
    # SQL knowledge → Database systems
    "sql": {
        "domain_skills": {
            "backend": {
                # Backend với SQL knowledge → có thể dùng PostgreSQL hoặc MySQL
                "databases": {
                    "postgresql": {"score_ratio": 0.70, "confidence": 0.65},
                    "mysql": {"score_ratio": 0.70, "confidence": 0.65},
                },
                "min_sql_score": 70,
                "infer_one_of": True,
            }
        }
    },
    
    # Machine Learning → Common tools
    "machine learning": {
        "domain_skills": {
            "machine_learning": {
                "libraries": {
                    "python": {"score_ratio": 0.90, "confidence": 0.95},       # ML ≈ Python
                    "pandas": {"score_ratio": 0.80, "confidence": 0.90},
                    "numpy": {"score_ratio": 0.80, "confidence": 0.90},
                    "scikit-learn": {"score_ratio": 0.70, "confidence": 0.75},
                },
                "min_machine_learning_score": 75,
                "infer_all": ["python", "pandas", "numpy"],
            }
        }
    },
    
    # DevOps → Container/Orchestration
    "devops": {
        "domain_skills": {
            "devops": {
                "tools": {
                    "docker": {"score_ratio": 0.80, "confidence": 0.85},
                    "kubernetes": {"score_ratio": 0.60, "confidence": 0.60},
                    "ci/cd": {"score_ratio": 0.70, "confidence": 0.70},
                },
                "min_devops_score": 75,
                "infer_all": ["docker"],  # Docker gần như bắt buộc
                "infer_one_of": ["kubernetes", "ci/cd"],
            }
        }
    }
}
```

### ⚠️ **Chiều Ngược Lại: CÓ hay KHÔNG?**

#### **Tại sao KHÔNG nên có chiều ngược lại đơn giản?**

```
❌ SAI: Python → FastAPI (one-to-many, ambiguous)
- Có Python không đồng nghĩa với có FastAPI
- Python developer có thể dùng Django, Flask, hoặc không dùng framework nào
- Infer sai sẽ tạo noise và false positives

✅ ĐÚNG: FastAPI → Python (many-to-one, deterministic)
- Có FastAPI thì CHẮC CHẮN có Python
- Relationship rõ ràng, không ambiguous
```

#### **Khi nào CÓ THỂ có chiều ngược lại (với điều kiện)?**

**Case 1: Context-based Weak Inference**

```python
# Candidate mention Python nhưng không mention framework cụ thể
Candidate:
- python: 85 (high score)
- domain: backend (detected from other skills)
- No FastAPI, Django, Flask mentioned

→ Có thể WEAK infer:
- django: 25 (0.3 confidence)
- flask: 25 (0.3 confidence)
- fastapi: 25 (0.3 confidence)

Lý do: Backend Python developer thường dùng một trong các frameworks này
Nhưng confidence thấp vì không chắc chắn
```

**Case 2: Ecosystem Completion**

```python
# Candidate mention nhiều Python skills nhưng thiếu common libraries
Candidate:
- python: 90
- machine learning: 85
- data analysis: 80
- (No pandas, numpy mentioned)

→ Có thể infer:
- pandas: 50 (0.6 confidence, vì ML + data analysis thường dùng pandas)
- numpy: 50 (0.6 confidence)

Lý do: Ecosystem-based inference - ML/Data không thể không dùng pandas/numpy
```

**Case 3: Negative Inference (Advanced)**

```python
# Candidate mention language nhưng explicitly mention competing frameworks
Candidate:
- python: 85
- django: 80 (explicitly mentioned)

→ Có thể NEGATIVE infer:
- fastapi: 10-20 (low, vì đã chọn Django)
- flask: 15 (low, vì đã chọn Django)

Lý do: Developer thường specialize vào 1 framework, ít khi master nhiều
```

### Implementation

```python
def infer_missing_skills(candidate_scores: Dict[str, float], 
                        candidate_domain: str = None,
                        enable_reverse_inference: bool = False) -> Dict[str, float]:
    """
    Infer missing skills based on dependencies.
    
    Args:
        candidate_scores: {skill: score} mapping
        candidate_domain: Primary domain (e.g., 'backend', 'frontend')
        enable_reverse_inference: Whether to use weak reverse inference
        
    Returns:
        Enhanced scores with inferred skills
    """
    inferred_scores = candidate_scores.copy()
    
    # Phase 1: Forward inference (Specific → General) - STRONG
    for skill, score in candidate_scores.items():
        if skill in SKILL_DEPENDENCIES:
            dependencies = SKILL_DEPENDENCIES[skill]
            
            for dep_skill, multiplier in dependencies.items():
                # Only infer if skill is NOT already present
                if dep_skill not in inferred_scores or inferred_scores[dep_skill] == 0:
                    inferred_score = score * multiplier
                    
                    # Tag as inferred for transparency
                    inferred_scores[dep_skill] = inferred_score
                    inferred_scores[f"{dep_skill}_inferred"] = True
                    inferred_scores[f"{dep_skill}_inference_type"] = "forward"
                    inferred_scores[f"{dep_skill}_confidence"] = multiplier
    
    # Phase 2: Reverse inference (General → Specific) - WEAK (optional)
    if enable_reverse_inference:
        inferred_scores = apply_reverse_inference(
            inferred_scores, 
            candidate_domain
        )
    
    return inferred_scores


def apply_reverse_inference(candidate_scores: Dict[str, float],
                           candidate_domain: str,
                           additional_context: Dict = None) -> Dict[str, float]:
    """
    Apply context-aware reverse inference (General → Specific).
    Uses domain information to infer with appropriate confidence levels.
    
    Args:
        candidate_scores: {skill: score} mapping
        candidate_domain: Primary domain ('backend', 'frontend', 'data_science', etc.)
        additional_context: Additional context like mentioned skills, role, etc.
    """
    enhanced_scores = candidate_scores.copy()
    additional_context = additional_context or {}
    
    for base_skill, config in SKILL_REVERSE_INFERENCE.items():
        # Check if base skill is present
        if base_skill not in candidate_scores:
            continue
        
        base_score = candidate_scores[base_skill]
        domain_skills = config.get("domain_skills", {})
        
        # Check if we have config for this domain
        if candidate_domain not in domain_skills:
            continue
        
        domain_config = domain_skills[candidate_domain]
        
        # Check minimum score threshold
        min_score_key = f"min_{base_skill.replace(' ', '_')}_score"
        min_score = domain_config.get(min_score_key, 70)
        
        if base_score < min_score:
            continue
        
        # Get skills to infer
        frameworks = domain_config.get("frameworks", {})
        libraries = domain_config.get("libraries", {})
        tools = domain_config.get("tools", {})
        databases = domain_config.get("databases", {})
        
        all_inferrable = {**frameworks, **libraries, **tools, **databases}
        
        # Check if candidate already mentioned any of these skills
        mentioned_skills = [
            skill for skill in all_inferrable.keys()
            if skill in candidate_scores and candidate_scores[skill] > 0
        ]
        
        # Strategy 1: infer_all - Always infer these skills (core dependencies)
        infer_all = domain_config.get("infer_all", [])
        if isinstance(infer_all, bool) and infer_all:
            infer_all = list(all_inferrable.keys())
        
        for skill in infer_all:
            if skill in mentioned_skills:
                continue  # Already has this skill
            
            if skill in all_inferrable:
                skill_config = all_inferrable[skill]
                inferred_score = base_score * skill_config["score_ratio"]
                confidence = skill_config["confidence"]
                
                enhanced_scores[skill] = inferred_score
                enhanced_scores[f"{skill}_inferred"] = True
                enhanced_scores[f"{skill}_inference_type"] = "domain_required"
                enhanced_scores[f"{skill}_confidence"] = confidence
                enhanced_scores[f"{skill}_source"] = base_skill
        
        # Strategy 2: infer_one_of - Infer ONE skill if none are mentioned
        infer_one_of = domain_config.get("infer_one_of", False)
        
        if infer_one_of:
            # Get candidate skills for this category
            if isinstance(infer_one_of, list):
                candidates = infer_one_of
            else:
                candidates = list(all_inferrable.keys())
            
            # Filter out skills in infer_all (already handled)
            candidates = [s for s in candidates if s not in infer_all]
            
            # Check if ANY of these skills are already mentioned
            has_any = any(skill in mentioned_skills for skill in candidates)
            
            if not has_any and candidates:
                # Pick the one with highest confidence
                best_skill = max(
                    candidates,
                    key=lambda s: all_inferrable.get(s, {}).get("confidence", 0)
                )
                
                skill_config = all_inferrable[best_skill]
                inferred_score = base_score * skill_config["score_ratio"]
                confidence = skill_config["confidence"]
                
                enhanced_scores[best_skill] = inferred_score
                enhanced_scores[f"{best_skill}_inferred"] = True
                enhanced_scores[f"{best_skill}_inference_type"] = "domain_likely"
                enhanced_scores[f"{best_skill}_confidence"] = confidence
                enhanced_scores[f"{best_skill}_source"] = base_skill
    
    return enhanced_scores


def detect_candidate_domain(candidate_scores: Dict[str, float]) -> str:
    """
    Detect primary domain from candidate scores with better granularity.
    """
    DOMAIN_GROUPS = {
        # Specific first (more precise)
        'machine_learning': [
            'machine learning', 'deep learning', 'neural network',
            'tensorflow', 'pytorch', 'model training', 'nlp', 'computer vision'
        ],
        'data_science': [
            'data science', 'data analysis', 'statistics', 'pandas', 'numpy',
            'data visualization', 'jupyter', 'r programming'
        ],
        'frontend': [
            'react', 'vue', 'angular', 'javascript', 'typescript', 
            'css', 'html', 'frontend', 'ui/ux'
        ],
        'backend': [
            'python', 'java', 'go', 'node.js', 'spring', 'django', 'fastapi',
            'backend', 'api', 'microservices', 'rest api', 'graphql'
        ],
        'devops': [
            'docker', 'kubernetes', 'jenkins', 'terraform', 'ansible',
            'ci/cd', 'devops', 'infrastructure'
        ],
        'mobile': [
            'react native', 'flutter', 'swift', 'kotlin', 'ios', 'android',
            'mobile development'
        ],
        'full-stack': [
            'full-stack', 'full stack development'
        ]
    }
    
    domain_scores = {}
    for domain, skills in DOMAIN_GROUPS.items():
        scores = [candidate_scores.get(skill, 0) for skill in skills]
        valid_scores = [s for s in scores if s > 0]
        
        if valid_scores:
            # Weighted: average score * number of skills
            domain_scores[domain] = np.mean(valid_scores) * len(valid_scores)
        else:
            domain_scores[domain] = 0
    
    # Get top domain
    if max(domain_scores.values()) > 0:
        primary_domain = max(domain_scores, key=domain_scores.get)
        
        # Special case: If has ML but also has data science, prefer ML
        if primary_domain == 'data_science' and domain_scores.get('machine_learning', 0) > domain_scores['data_science'] * 0.7:
            return 'machine_learning'
        
        return primary_domain
    
    return 'backend'  # Default fallback

```

### Example

```python
# Case 1: Forward Inference Only (Default - Recommended)
candidate = {
    'fastapi': 85,
    'postgresql': 80,
    'docker': 75
}

enhanced = infer_missing_skills(candidate, enable_reverse_inference=False)

# Result:
{
    'fastapi': 85,
    'python': 76.5,          # Forward: 85 * 0.9 (HIGH confidence)
    'python_inferred': True,
    'python_inference_type': 'forward',
    'python_confidence': 0.9,
    
    'postgresql': 80,
    'sql': 72,               # Forward: 80 * 0.9
    'sql_inferred': True,
    'database': 68,          # Forward: 80 * 0.85
    'database_inferred': True,
    
    'docker': 75,
}


# Case 2: Forward + Reverse Inference with Domain Context
candidate = {
    'python': 90,            # High Python, no framework mentioned
    'postgresql': 75,
    'machine learning': 80,
}

domain = detect_candidate_domain(candidate)  # → "machine_learning"
enhanced = infer_missing_skills(candidate, candidate_domain=domain, 
                                enable_reverse_inference=True)

# Result with domain-aware reverse inference:
{
    'python': 90,
    'postgresql': 75,
    'machine learning': 80,
    
    # Forward inference from ML
    'data analysis': 56,     # Forward from ML: 80 * 0.7
    'data analysis_inferred': True,
    'data analysis_confidence': 0.7,
    
    # Domain-aware reverse inference: Python + ML domain → Core ML libraries
    'pandas': 76.5,          # Reverse: 90 * 0.85 (95% confidence - domain_required)
    'pandas_inferred': True,
    'pandas_inference_type': 'domain_required',
    'pandas_confidence': 0.95,
    'pandas_source': 'python',
    
    'numpy': 76.5,           # Reverse: 90 * 0.85 (95% confidence - domain_required)
    'numpy_inferred': True,
    'numpy_inference_type': 'domain_required',
    'numpy_confidence': 0.95,
    
    # One deep learning framework (highest confidence)
    'pytorch': 63.0,         # Reverse: 90 * 0.70 (90% confidence - domain_likely)
    'pytorch_inferred': True,
    'pytorch_inference_type': 'domain_likely',
    'pytorch_confidence': 0.90,
}


# Case 3: Backend Developer with Explicit Framework (Skip Reverse)
candidate = {
    'python': 85,
    'django': 82,           # Explicit framework → skip reverse inference
    'postgresql': 75,
}

domain = detect_candidate_domain(candidate)  # → "backend"
enhanced = infer_missing_skills(candidate, candidate_domain=domain,
                                enable_reverse_inference=True)

# Result:
{
    'python': 85,
    'django': 82,           # Explicit - no inference needed
    'postgresql': 75,
    'sql': 67.5,            # Forward from PostgreSQL
    'database': 63.75,      # Forward from PostgreSQL
    
    # NO reverse inference for Django/FastAPI - already has explicit framework!
}
```
    'django_inference_type': 'reverse_weak',
    'django_confidence': 0.3,
    
    'flask': 25.5,           # Reverse: 85 * 0.3 (LOW confidence)
    'flask_inferred': True,
    
}


# Case 4: Real-World Comparison - Backend Developer
# Before vs After Domain-Aware Inference

# === BEFORE (Old weak inference) ===
candidate_before = {
    'python': 90,
    'rest api': 85,
    'postgresql': 80
}

old_result = {
    'python': 90,
    'rest api': 85,
    'postgresql': 80,
    
    # Old reverse inference (30% confidence - too weak!)
    'django': 27.0,          # 90 * 0.3
    'fastapi': 27.0,         # 90 * 0.3
    'flask': 27.0,           # 90 * 0.3
}

# === AFTER (Domain-aware inference) ===
candidate_after = {
    'python': 90,
    'rest api': 85,
    'postgresql': 80
}

domain = detect_candidate_domain(candidate_after)  # → 'backend'
new_result = infer_missing_skills(
    candidate_after, 
    candidate_domain=domain,
    enable_reverse_inference=True
)

new_result = {
    'python': 90,
    'rest api': 85,
    'postgresql': 80,
    'sql': 72.0,            # Forward from PostgreSQL
    'database': 68.0,       # Forward from PostgreSQL
    
    # Domain-aware reverse: ONE framework with high confidence
    'django': 67.5,         # 90 * 0.75 (85% confidence - domain_likely)
    'django_inferred': True,
    'django_inference_type': 'domain_likely',
    'django_confidence': 0.85,
    'django_source': 'python',
    
    # Core backend tools (infer_all)
    'rest api': 85,
    'pytest': 49.5,         # 90 * 0.55 (70% confidence - domain_required)
    'pytest_inferred': True,
    'pytest_confidence': 0.70,
}

# Impact:
# - Before: 3 weak frameworks @ 27 points (30% confidence)
# - After: 1 strong framework @ 67.5 points (85% confidence)
# - Reduction: From ambiguous to decisive
# - Quality: Higher confidence = better clustering
```

### 🎯 **Recommendation: Khi nào dùng Reverse Inference?**

#### **KHÔNG dùng reverse inference (Default - Safe):**
```
✅ Ưu điểm:
- Chỉ infer khi CHẮC CHẮN (high precision)
- Tránh false positives
- Results dễ explain

❌ Nhược điểm:
- Có thể miss một số skills candidate thực sự có
- Less complete feature vectors (more zeros)
```

#### **CÓ dùng reverse inference với domain context (Recommended for completeness):**
```
✅ Ưu điểm:
- More complete feature vectors (fewer zeros)
- Better similarity matching trong clustering
- Domain-aware: 60-95% confidence (không còn weak 30% nữa!)
- Chọn 1 framework thay vì infer tất cả (less ambiguity)
- ML engineers tự động có pandas/numpy (almost certain)
- Backend devs có pytest/sqlalchemy (domain_required)

❌ Nhược điểm:
- Có thể infer sai (nhưng có confidence score để filter)
- Cần maintain domain configs
- Slightly more complex

📊 Use case:
- Dataset có nhiều CVs thiếu thông tin chi tiết
- Muốn tăng similarity giữa candidates cùng domain
- Có thể accept một chút noise để đổi lấy completeness
- Clustering quality > Individual accuracy
```

#### **Recommended Approach: Hybrid with Confidence Filtering**

```python
# Step 1: Forward inference (always safe, always apply)
enhanced = infer_missing_skills(candidate, enable_reverse_inference=False)

# Step 2: Domain detection
domain = detect_candidate_domain(enhanced)

# Step 3: Reverse inference with domain context
enhanced = apply_reverse_inference(enhanced, domain)

# Step 4: Filter by confidence (keep only 70%+ confidence)
def filter_by_confidence(scores, min_confidence=0.70):
    """Only keep high-confidence inferences."""
    filtered = {}
    for skill, score in scores.items():
        # Skip metadata keys
        if '_confidence' in skill or '_inferred' in skill or '_source' in skill or '_type' in skill:
            continue
        
        confidence_key = f"{skill}_confidence"
        if confidence_key in scores:
            # Inferred skill: check confidence
            if scores[confidence_key] >= min_confidence:
                filtered[skill] = score
        else:
            # Original skill: always keep
            filtered[skill] = score
    
    return filtered

final_scores = filter_by_confidence(enhanced, min_confidence=0.70)

# Result: 
# ✅ Keep: pandas (95%), pytorch (90%), django (85%)
# ❌ Filter: jupyter (60%), matplotlib (65%)
# 🎯 Balance: Completeness + High precision
```

### 📊 Confidence Levels Guide

```python
CONFIDENCE_TIERS = {
    'almost_certain': 0.90,   # pandas for ML engineers, Python for Django devs
    'highly_likely': 0.80,     # FastAPI for Python backend, pytest for developers
    'very_likely': 0.70,       # sqlalchemy for backend, rest api for web devs
    'likely': 0.60,            # jupyter for data scientists (but not certain)
    'possibly': 0.50,          # Could be, could not be
    'weak': 0.30,              # OLD approach - too risky
}

# Recommended thresholds:
# - For clustering: 0.60+ (balance completeness & precision)
# - For display: 0.80+ (show only highly confident)
# - For critical decisions: 0.90+ (almost certain only)
```
            continue
        if skill.endswith('_inference_type'):
            continue
        
        # Check confidence
        confidence_key = f"{skill}_confidence"
        if confidence_key in scores:
            if scores[confidence_key] >= min_confidence:
                filtered[skill] = score
        else:
            filtered[skill] = score  # Original skill, always include
    
    return filtered
```

### Approach 2: Co-occurrence Based Inference

Học dependencies từ data:

```python
def learn_skill_dependencies(all_candidates):
    """
    Learn which skills frequently co-occur.
    """
    cooccurrence = defaultdict(lambda: defaultdict(int))
    
    for candidate in all_candidates:
        skills = candidate['skills']
        for skill1 in skills:
            for skill2 in skills:
                if skill1 != skill2:
                    cooccurrence[skill1][skill2] += 1
    
    # Convert to conditional probabilities
    dependencies = {}
    for skill1, co_skills in cooccurrence.items():
        total = sum(co_skills.values())
        dependencies[skill1] = {
            skill2: count / total
            for skill2, count in co_skills.items()
            if count / total > 0.7  # Strong dependency threshold
        }
    
    return dependencies
```

---

## 🔴 VẤN ĐỀ 2: Không Có Penalty Score

### Mô Tả Vấn Đề

```
Frontend Developer thực sự:
- React: 95
- JavaScript: 90
- CSS: 85
- Backend: 70 ← Sai! Không nên cao
- Python: 65  ← Sai! Không nên có
- AWS: 68    ← Sai! Không phải chuyên môn

→ Khi clustering, candidate này có thể gần Backend developers
  vì có scores cao ở Backend skills (mặc dù không thực sự giỏi)
```

### Root Cause

1. **Evaluators đánh giá không chính xác**: Cho điểm cao cho skills ngoài chuyên môn
2. **Không có mutual exclusivity**: Frontend và Backend skills không được model như competing domains
3. **Lack of specialization penalty**: Candidate "jack of all trades" được treat như candidates chuyên sâu

### Impact

1. **Domain confusion**: Frontend developers cluster với Backend developers
2. **Diluted clusters**: Clusters chứa candidates với diverse backgrounds
3. **Poor interpretability**: Cluster labels không reflect actual specializations

---

## 💡 GIẢI PHÁP 2: Domain-Based Penalty Scores

### Approach 1: Mutual Exclusivity Penalty

```python
DOMAIN_GROUPS = {
    'frontend': ['react', 'vue', 'angular', 'javascript', 'typescript', 'css', 'html'],
    'backend': ['python', 'java', 'go', 'node.js', 'spring', 'django', 'fastapi'],
    'data': ['pandas', 'numpy', 'tensorflow', 'pytorch', 'machine learning', 'data analysis'],
    'devops': ['docker', 'kubernetes', 'jenkins', 'terraform', 'ansible'],
    'mobile': ['react native', 'flutter', 'swift', 'kotlin', 'ios', 'android'],
}

def apply_domain_penalty(candidate_scores: Dict[str, float]) -> Dict[str, float]:
    """
    Apply penalty to cross-domain skills based on primary specialization.
    """
    # 1. Identify primary domain
    domain_strengths = {}
    for domain, skills in DOMAIN_GROUPS.items():
        domain_scores = [candidate_scores.get(skill, 0) for skill in skills]
        domain_strengths[domain] = np.mean([s for s in domain_scores if s > 0])
    
    primary_domain = max(domain_strengths, key=domain_strengths.get)
    primary_strength = domain_strengths[primary_domain]
    
    # 2. Apply penalty to other domains
    penalized_scores = candidate_scores.copy()
    
    for domain, skills in DOMAIN_GROUPS.items():
        if domain != primary_domain:
            # Calculate penalty factor
            domain_strength = domain_strengths[domain]
            
            # If secondary domain strength is too high, apply penalty
            if domain_strength > primary_strength * 0.6:
                penalty_factor = 0.7  # Reduce by 30%
                
                for skill in skills:
                    if skill in penalized_scores:
                        penalized_scores[skill] *= penalty_factor
                        penalized_scores[f"{skill}_penalized"] = True
    
    return penalized_scores
```

### Example

```python
# Before
candidate = {
    'react': 95,
    'javascript': 90,
    'css': 85,
    'python': 70,    # Too high for frontend specialist
    'django': 68,    # Too high for frontend specialist
}

# After penalty
candidate_penalized = {
    'react': 95,
    'javascript': 90,
    'css': 85,
    'python': 49,           # 70 * 0.7 (penalized)
    'python_penalized': True,
    'django': 47.6,         # 68 * 0.7 (penalized)
    'django_penalized': True,
}
```

### Approach 2: Specialization Score Weighting

```python
def compute_specialization_penalty(candidate_scores: Dict[str, float]) -> float:
    """
    Compute a specialization score (0-1).
    Higher = more specialized
    Lower = more generalist
    """
    # Count how many domains the candidate has skills in
    domains_present = 0
    for domain, skills in DOMAIN_GROUPS.items():
        domain_scores = [candidate_scores.get(skill, 0) for skill in skills if skill in candidate_scores]
        if any(s > 60 for s in domain_scores):  # Strong skill in domain
            domains_present += 1
    
    # Specialization score
    # 1 domain = 1.0 (specialist)
    # 2 domains = 0.8 (T-shaped)
    # 3 domains = 0.6 (versatile)
    # 4+ domains = 0.4 (generalist)
    
    if domains_present == 1:
        return 1.0
    elif domains_present == 2:
        return 0.8
    elif domains_present == 3:
        return 0.6
    else:
        return 0.4

def apply_specialization_weighting(candidate_scores, specialization_score):
    """
    Weight skills based on specialization.
    Specialists get boost, generalists get penalty.
    """
    weighted_scores = {}
    for skill, score in candidate_scores.items():
        # Boost primary skills, penalty secondary skills
        if specialization_score > 0.8:  # Specialist
            weighted_scores[skill] = score * 1.1  # 10% boost
        elif specialization_score < 0.5:  # Generalist
            weighted_scores[skill] = score * 0.9  # 10% penalty
        else:
            weighted_scores[skill] = score
    
    return weighted_scores
```

---

## 🔴 VẤN ĐỀ 3: Vector Có Quá Nhiều Giá Trị 0

### Mô Tả Vấn Đề

```
Candidate A (Python specialist):
- python: 95
- fastapi: 90
- postgresql: 85
- react: 0       ← Không mention
- javascript: 0  ← Không mention
- aws: 0         ← Không mention
... (1000+ zeros)

Candidate B (Full-stack):
- python: 80
- fastapi: 75
- react: 20      ← Mention nhưng yếu
- javascript: 25 ← Mention nhưng yếu
- aws: 30        ← Mention nhưng yếu

Vấn đề:
- A có react=0 (không biết)
- B có react=20 (biết nhưng yếu)
→ Khi tính distance, cả 2 đều "low scores" nhưng ý nghĩa khác nhau
→ Clustering có thể nhầm lẫn
```

### Root Cause

**Giá trị 0 có 2 nghĩa khác nhau:**
1. **True Zero**: Skill được đánh giá và score = 0 (rất yếu)
2. **Missing Value**: Skill không được mention/đánh giá (unknown)

**Nhưng trong feature vector, cả 2 đều là 0!**

### Impact

1. **Ambiguous distances**: Distance metrics không phân biệt "unknown" vs "weak"
2. **Clustering artifacts**: Candidates có nhiều 0s (specialists) cluster riêng
3. **Weak vs Missing confusion**: Cannot distinguish between weak skills and missing data

---

## 💡 GIẢI PHÁP 3: Encoding Strategy cho Missing Values

### Approach 1: Three-Value Encoding

Thay vì 0-100, dùng 3 giá trị:

```python
def encode_skills_three_value(candidate_scores: Dict[str, float], all_skills: List[str]):
    """
    Encode skills with 3 states:
    - -1: Not mentioned (unknown)
    -  0-100: Mentioned with score
    """
    encoded = {}
    for skill in all_skills:
        if skill in candidate_scores:
            encoded[skill] = candidate_scores[skill]  # 0-100
        else:
            encoded[skill] = -1  # Unknown
    
    return encoded
```

**Benefit:**
- `-1`: Unknown (not comparable)
- `0-100`: Known (comparable)
- Distance metrics can handle -1 specially

**Drawback:**
- Requires custom distance metrics
- More complex implementation

### Approach 2: Dual-Vector Representation

Tạo 2 vectors riêng:

```python
def create_dual_vectors(candidate_scores: Dict[str, float], all_skills: List[str]):
    """
    Create 2 vectors:
    1. Score vector (0-100 for mentioned skills, 0 for not mentioned)
    2. Mask vector (1 for mentioned, 0 for not mentioned)
    """
    score_vector = []
    mask_vector = []
    
    for skill in all_skills:
        if skill in candidate_scores:
            score_vector.append(candidate_scores[skill])
            mask_vector.append(1)  # Mentioned
        else:
            score_vector.append(0)
            mask_vector.append(0)  # Not mentioned
    
    return {
        'scores': np.array(score_vector),
        'mask': np.array(mask_vector)
    }

def compute_masked_distance(candidate1, candidate2):
    """
    Compute distance only on commonly mentioned skills.
    """
    # Find common skills (both mentioned)
    common_mask = candidate1['mask'] * candidate2['mask']
    
    # Compute distance only on common skills
    diff = candidate1['scores'] - candidate2['scores']
    masked_diff = diff * common_mask
    
    # Normalize by number of common skills
    num_common = common_mask.sum()
    if num_common == 0:
        return float('inf')  # No common skills
    
    distance = np.linalg.norm(masked_diff) / np.sqrt(num_common)
    return distance
```

**Benefit:**
- Clear separation between "score" and "presence"
- Can compute distance only on common skills
- More interpretable

**Drawback:**
- Doubles feature space
- More complex distance metrics

### Approach 3: Imputation Strategy

Fill 0s với giá trị khác:

```python
def impute_missing_skills(candidate_scores: Dict[str, float], 
                          all_skills: List[str],
                          strategy: str = 'domain_mean'):
    """
    Impute missing skills with reasonable values.
    
    Strategies:
    - 'domain_mean': Fill with mean of skills in same domain
    - 'global_mean': Fill with global mean (50)
    - 'conservative': Fill with low value (30)
    - 'negative': Fill with -1 (unknown)
    """
    imputed = candidate_scores.copy()
    
    if strategy == 'domain_mean':
        for skill in all_skills:
            if skill not in imputed:
                # Find domain
                domain = get_skill_domain(skill)
                # Compute mean of mentioned skills in same domain
                domain_skills = [s for s in imputed if get_skill_domain(s) == domain]
                if domain_skills:
                    imputed[skill] = np.mean([imputed[s] for s in domain_skills]) * 0.5
                else:
                    imputed[skill] = 30  # Conservative default
    
    elif strategy == 'global_mean':
        for skill in all_skills:
            if skill not in imputed:
                imputed[skill] = 50
    
    elif strategy == 'conservative':
        for skill in all_skills:
            if skill not in imputed:
                imputed[skill] = 30
    
    elif strategy == 'negative':
        for skill in all_skills:
            if skill not in imputed:
                imputed[skill] = -1
    
    return imputed
```

### Approach 4: Feature Engineering với Binary Flags

Thêm binary features:

```python
def create_enhanced_features(candidate_scores: Dict[str, float], all_skills: List[str]):
    """
    Create enhanced feature set:
    - Original scores (0-100 or 0 if not mentioned)
    - Binary "has_skill" flags
    - Domain presence flags
    """
    features = {}
    
    # 1. Original scores
    for skill in all_skills:
        features[f"score_{skill}"] = candidate_scores.get(skill, 0)
    
    # 2. Binary flags
    for skill in all_skills:
        features[f"has_{skill}"] = 1 if skill in candidate_scores else 0
    
    # 3. Domain presence
    for domain, skills in DOMAIN_GROUPS.items():
        domain_count = sum(1 for s in skills if s in candidate_scores)
        features[f"domain_{domain}_count"] = domain_count
        features[f"domain_{domain}_presence"] = 1 if domain_count > 0 else 0
    
    return features
```

**Benefit:**
- Explicit separation of "score" vs "presence"
- Clustering can use both aspects
- Easy to implement

**Drawback:**
- Doubles feature space again
- May introduce redundancy

---

## 🎯 RECOMMENDED SOLUTION

### Integrated Approach: Combine All 3 Solutions

```python
class EnhancedFeatureEngineer:
    """
    Enhanced feature engineering with:
    1. Skill dependency inference
    2. Domain-based penalty
    3. Missing value handling
    """
    
    def __init__(self):
        self.skill_dependencies = SKILL_DEPENDENCIES
        self.domain_groups = DOMAIN_GROUPS
    
    def extract_features(self, candidate):
        """
        Extract enhanced features.
        """
        # Step 1: Get raw scores
        raw_scores = candidate['normalized_scores']
        
        # Step 2: Infer missing skills (Solution 1)
        inferred_scores = self.infer_missing_skills(raw_scores)
        
        # Step 3: Apply domain penalty (Solution 2)
        penalized_scores = self.apply_domain_penalty(inferred_scores)
        
        # Step 4: Create dual vectors (Solution 3)
        features = self.create_dual_vectors(penalized_scores)
        
        return features
    
    def infer_missing_skills(self, scores):
        """Solution 1: Infer dependencies"""
        # ... (implementation from above)
        pass
    
    def apply_domain_penalty(self, scores):
        """Solution 2: Apply penalties"""
        # ... (implementation from above)
        pass
    
    def create_dual_vectors(self, scores):
        """Solution 3: Dual vectors + binary flags"""
        # ... (implementation from above)
        pass
```

---

## 📊 Expected Impact

### Before Improvements

```
Candidate A (FastAPI developer):
- fastapi: 85
- python: 0 ← Missing!
- postgresql: 80
- react: 70 ← Too high (penalty needed)

Feature Vector: [85, 0, 80, 70, ...]
→ Looks weak in Python
→ Looks strong in React (wrong)
```

### After Improvements

```
Candidate A:
Step 1 - Inference:
- fastapi: 85
- python: 76.5 (inferred from FastAPI)
- postgresql: 80
- sql: 72 (inferred from PostgreSQL)
- react: 70

Step 2 - Penalty:
- fastapi: 85
- python: 76.5
- postgresql: 80
- sql: 72
- react: 49 (penalized - not primary domain)

Step 3 - Dual Vectors:
- score_vector: [85, 76.5, 80, 72, 49, ...]
- mask_vector: [1, 0, 1, 0, 1, ...] (0 = inferred)
- has_fastapi: 1
- has_python: 0 (inferred, not explicitly mentioned)
- domain_backend_strength: 0.9
- domain_frontend_strength: 0.3 (penalized)

→ Clear representation: Strong backend, weak frontend
→ Better clustering with similar candidates
```

---

## 🚀 Implementation Plan

### Phase 1: Skill Dependency Inference (Week 1)
1. Create `SKILL_DEPENDENCIES` mapping
2. Implement `infer_missing_skills()`
3. Test on sample candidates
4. Validate inference quality

### Phase 2: Domain Penalty (Week 1)
1. Define `DOMAIN_GROUPS`
2. Implement `apply_domain_penalty()`
3. Test penalty factors
4. Tune thresholds

### Phase 3: Missing Value Handling (Week 2)
1. Implement dual-vector representation
2. Update distance metrics
3. Add binary flags
4. Test clustering improvements

### Phase 4: Integration (Week 2)
1. Integrate all 3 solutions into `FeatureEngineer`
2. Re-run Phase 2 with new features
3. Re-run Phase 3 clustering
4. Compare results with baseline

### Phase 5: Validation (Week 3)
1. Validate cluster quality
2. Check interpretability improvements
3. Document findings
4. Create production config

---

## ✅ Success Metrics

1. **Inference Quality**
   - % of inferred skills that make sense
   - Accuracy of inferred score levels
   - Target: >90% valid inferences

2. **Penalty Effectiveness**
   - Reduction in cross-domain confusion
   - Improved domain separation in clusters
   - Target: >80% reduction in cross-domain mixing

3. **Missing Value Impact**
   - Improved cluster homogeneity
   - Better separation between specialists and generalists
   - Target: 20% improvement in silhouette score

4. **Overall Clustering Quality**
   - Davies-Bouldin Index (lower is better)
   - Silhouette Score (higher is better)
   - Manual inspection of cluster interpretability

---

## 📝 Next Steps

1. **Review and approve** this design document
2. **Prioritize** which solution to implement first
3. **Create** implementation tasks
4. **Start** with Skill Dependency Inference (quickest win)
5. **Iterate** based on results

**Estimated Total Effort:** 3-4 weeks for all 3 solutions
**Recommended Start:** Skill Dependency Inference (highest impact, lowest effort)
