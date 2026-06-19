# System Design: Location Spoofing Detection

## High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────────────┐
│                          MOBILE DEVICE SDK                              │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │
│  │   GPS    │  │ Network  │  │ Sensors  │  │  Device  │  │ Security │   │
│  │  Module  │  │  Module  │  │  Module  │  │  Module  │  │  Module  │   │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘  └──────────┘   │
└────────────────────────────┬────────────────────────────────────────────┘
                             │ Event Data
                             ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                        DATA INGESTION LAYER                             │
│  • Collect telemetry from SDK                                           │
│  • Validate schema and data types                                       │
│  • Enrich with server-side signals (IP geolocation)                     │
└────────────────────────────┬────────────────────────────────────────────┘
                             │ Structured Event
                             ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                       FEATURE ENGINEERING                               │
│  ┌────────────────────────────────────────────────────────────────────┐ │
│  │ RAW FEATURES               DERIVED FEATURES                        │ │
│  │ • latitude, longitude      • speed_kmh (from m/s)                  │ │
│  │ • accuracy                 • is_stationary, is_fast_moving         │ │
│  │ • speed, bearing           • accuracy_perfect, accuracy_poor       │ │
│  │ • ip_address               • ip_region (west/east/central)         │ │
│  │ • accelerometer_variance   • low_accel_variance (< 0.01)           │ │
│  │ • gyroscope_variance       • low_gyro_variance (< 0.01)            │ │
│  │ • device_timezone_offset   • tz_expected, tz_mismatch              │ │
│  │ • mock_location_enabled    • sensor_mismatch (movement w/o sensor) │ │
│  └────────────────────────────────────────────────────────────────────┘ │
└────────────────────────────┬────────────────────────────────────────────┘
                             │ Feature Vector (33 dimensions)
                             ▼
┌──────────────────────────────────────────────────────────────────────────┐
│                        DETECTION PIPELINE                                │
│  ┌──────────────────────┐              ┌──────────────────────┐          │
│  │  RULES-BASED ENGINE  │              │     ML MODEL         │          │
│  │  ┌─────────────────┐ │              │ ┌──────────────────┐ │          │
│  │  │ Rule 1:         │ │              │ │  Random Forest   │ │          │
│  │  │ Teleportation   │ │              │ │  (100 trees)     │ │          │
│  │  │ (speed > 150km/h│ │              │ │                  │ │          │
│  │  └─────────────────┘ │              │ │  33 features     │ │          │
│  │  ┌─────────────────┐ │              │ │  Binary output   │ │          │
│  │  │ Rule 2:         │ │              │ └──────────────────┘ │          │
│  │  │ IP/Geo Mismatch │ │              │                      │          │
│  │  └─────────────────┘ │              │ Threshold: 1.0       │          │
│  │  ┌─────────────────┐ │              │ Precision: 100%      │          │
│  │  │ Rule 3:         │ │              │ Recall: 21.6%        │          │
│  │  │ Mock Location   │ │              └──────────────────────┘          │
│  │  └─────────────────┘ │                                                │
│  │  ┌─────────────────┐ │                                                │
│  │  │ Rule 4:         │ │                                                │
│  │  │ Sensor Mismatch │ │                                                │
│  │  └─────────────────┘ │                                                │
│  │  ┌─────────────────┐ │                                                │
│  │  │ Rule 5:         │ │                                                │
│  │  │ Timezone        │ │                                                │
│  │  └─────────────────┘ │                                                │
│  │  ┌─────────────────┐ │                                                │
│  │  │ Rule 6:         │ │                                                │
│  │  │ Perfect Accuracy│ │                                                │
│  │  └─────────────────┘ │                                                │
│  │                      │                                                │
│  │  OR Aggregation      │                                                │
│  │  Precision: 91.7%    │                                                │
│  │  Recall: 83.4%       │                                                │
│  └──────┬───────────────┘              └───────────┬────────────────────┘
│         │                                          │                     │
│         │ spoof_score_rules                        │ spoof_score_ml      │
│         │ spoof_flag_rules                         │ spoof_flag_ml       │
│         │ triggered_rules[]                        │                     │
│         └────────────┬─────────────────────────────┘                     │
│                      ▼                                                   │
│           ┌──────────────────────┐                                       │
│           │   HYBRID COMBINER    │                                       │
│           │  ┌────────────────┐  │                                       │
│           │  │ Weighted Avg:  │  │                                       │
│           │  │ 60% rules      │  │                                       │
│           │  │ 40% ML         │  │                                       │
│           │  └────────────────┘  │                                       │
│           │                      │                                       │
│           │  spoof_score (0-1)   │                                       │
│           │  spoof_flag (bool)   │                                       │
│           └──────────┬───────────┘                                       │
└──────────────────────┼───────────────────────────────────────────────────┘
                       │ Decision + Score
                       ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                      AI AUGMENTATION LAYER                              │
│  ┌──────────────────────┐  ┌──────────────────────┐  ┌────────────────┐ │
│  │   RULE PROPOSER      │  │   WEAK LABELER       │  │   EXPLAINER    │ │
│  │  ┌────────────────┐  │  │  ┌────────────────┐  │  │  ┌───────────┐ │ │
│  │  │ Input: Schema  │  │  │  │ Input: Notes   │  │  │  │ Input:    │ │ │
│  │  │ + Examples     │  │  │  │ + Features     │  │  │  │ Event +   │ │ │
│  │  │                │  │  │  │                │  │  │  │ Rules +   │ │ │
│  │  │ Output:        │  │  │  │ Output:        │  │  │  │ ML Score  │ │ │
│  │  │ Candidate      │  │  │  │ Weak Label     │  │  │  │           │ │ │
│  │  │ Detection      │  │  │  │ + Confidence   │  │  │  │ Output:   │ │ │
│  │  │ Rules          │  │  │  │                │  │  │  │ Human-    │ │ │
│  │  │                │  │  │  │                │  │  │  │ readable  │ │ │
│  │  │ (Brainstorming)│  │  │  │ (Data labeling)│  │  │  │ Reasoning │ │ │
│  │  └────────────────┘  │  │  └────────────────┘  │  │  └───────────┘ │ │
│  │   [LLM-powered or   │  │   [LLM-powered or   │  │  [LLM-powered  │ │
│  │    mocked]          │  │    mocked]          │  │   or mocked]   │ │
│  └──────────────────────┘  └──────────────────────┘  └────────────────┘ │
└───────────────────────────────────────┬─────────────────────────────────┘
                                        │ Explanation
                                        ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                          DECISION OUTPUT                                 │
│  {                                                                       │
│    "event_id": "evt_001234",                                            │
│    "spoof_score": 0.87,          // Combined confidence                 │
│    "spoof_flag": true,           // Binary decision                     │
│    "explanation": "This event was flagged because mock location         │
│                   software is enabled and GPS accuracy is               │
│                   suspiciously perfect. Confidence: high."              │
│  }                                                                       │
└────────────────────────────┬────────────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                       ACTION / RESPONSE                                  │
│  • Geo-compliance: Block transaction                                    │
│  • Anti-cheat: Flag for review                                          │
│  • Fraud prevention: Require additional verification                    │
│  • Logging: Record decision for audit                                   │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## Component Details

### 1. Mobile SDK Data Collection

**Purpose**: Capture comprehensive device telemetry for spoofing detection

**Data Sources**:

| Module | iOS API | Android API | Signals Collected |
|--------|---------|-------------|-------------------|
| GPS | CLLocationManager | FusedLocationProvider | lat, lon, accuracy, altitude, speed, bearing |
| Network | CTTelephonyNetworkInfo | ConnectivityManager | IP, network type, WiFi/cell counts |
| Sensors | CMMotionManager | SensorManager | Accelerometer, gyroscope variance |
| Device | UIDevice, NSTimeZone | BatteryManager, TimeZone | Battery, timezone, device state |
| Security | CLLocation.isMock | Settings.Secure | Mock location, dev options |

**Collection Frequency**: Event-driven (location update every 30s-5min)

**Privacy**: Anonymized device IDs, no PII collected

---

### 2. Data Ingestion & Validation

**Purpose**: Ensure data quality and enrich with server-side signals

**Steps**:
1. **Schema Validation**: Check all required fields present and typed correctly
2. **Bounds Checking**: Lat/lon within valid ranges, accuracy > 0, etc.
3. **IP Geolocation**: Enrich with IP-based location estimate (compare with GPS)
4. **Timestamp Ordering**: Ensure chronological order per device

**Output**: Validated, enriched event ready for processing

---

### 3. Feature Engineering

**Purpose**: Transform raw signals into ML-friendly features

**Categories**:

1. **Location Features**:
   - `lat_rounded`, `lon_rounded`: Coarse-grained location
   - `speed_kmh`: Convert m/s to km/h for interpretability

2. **Accuracy Indicators**:
   - `accuracy_perfect`: Binary flag for < 2m (suspicious)
   - `accuracy_poor`: Binary flag for > 50m (GPS issues)

3. **Movement Features**:
   - `is_stationary`: Speed < 0.5 m/s
   - `is_fast_moving`: Speed > 20 m/s (72 km/h)

4. **Sensor Features**:
   - `low_accel_variance`, `low_gyro_variance`: Suspiciously quiet sensors
   - `sensor_mismatch`: Movement without sensor activity

5. **Network Features**:
   - `ip_is_west`, `ip_is_east`, `ip_is_central`: IP region encoding
   - `has_wifi`, `network_is_wifi`: Connectivity context

6. **Timezone Features**:
   - `tz_expected`: Calculate from longitude (lon / 15)
   - `tz_mismatch`: Difference from expected
   - `tz_mismatch_significant`: > 2 hours off

**Total Features**: 33 (mix of raw and derived)

---

### 4. Rules-Based Detection Engine

**Purpose**: Apply interpretable heuristics for explainable decisions

**Rule Implementations**:

```python
Rule 1: Teleportation
- Logic: Speed between consecutive GPS points > 150 km/h
- Threshold: 150 km/h (even generous for aircraft)
- Output: Binary flag + confidence

Rule 2: IP/Geo Mismatch
- Logic: IP region doesn't match GPS coordinates (> 5° apart)
- Threshold: 5 degrees latitude/longitude
- Output: Binary flag + confidence

Rule 3: Mock Location Enabled
- Logic: Direct check of mock_location_enabled flag
- Threshold: Boolean (true = flagged)
- Output: Binary flag + high confidence

Rule 4: Sensor/Movement Mismatch
- Logic: Speed > 1 m/s but accel/gyro variance < 0.01
- Threshold: Speed > 1 m/s AND variance < 0.01
- Output: Binary flag + confidence

Rule 5: Timezone Mismatch
- Logic: Device timezone != expected (lon/15) by > 2 hours
- Threshold: 2 hour tolerance (for borders, DST)
- Output: Binary flag + confidence

Rule 6: Perfect Accuracy Anomaly
- Logic: GPS accuracy too perfect (< 2 meters)
- Threshold: 2 meters
- Output: Binary flag + low confidence
```

**Aggregation**:
- Flag event if **ANY** rule triggers (OR logic)
- Count triggered rules for confidence
- Record which rules triggered for explanation

**Performance**: Precision 91.7%, Recall 83.4%, F1 87.4%

**Advantages**:
- ✅ Fully interpretable (can explain every decision)
- ✅ Fast inference (<1ms per event)
- ✅ No model training required
- ✅ Easy to tune and update

**Disadvantages**:
- ❌ Hard-coded thresholds (not adaptive)
- ❌ Vulnerable to adversarial evasion (if thresholds known)
- ❌ Can't learn subtle patterns from data

---

### 5. ML Model (Random Forest)

**Purpose**: Learn complex patterns from data for validation

**Architecture**:
- **Algorithm**: Random Forest Classifier (scikit-learn)
- **Trees**: 100 estimators
- **Depth**: 10 (prevent overfitting)
- **Features**: 33 (engineered features)
- **Training**: 14,889 events (17.2% spoofed)

**Training Process**:
1. Load training data with labels
2. Engineer 33 features
3. Train Random Forest
4. Tune decision threshold (precision >= 85%)
5. Evaluate on held-out test set

**Threshold Tuning**:
- Target: Precision >= 85%
- Method: Precision-recall curve analysis
- Result: Threshold = 1.0 (ultra-conservative)
- Trade-off: Perfect precision (100%) but low recall (21.6%)

**Feature Importance** (top 5):
1. `accuracy` (12.5%) - GPS accuracy in meters
2. `accelerometer_variance` (11.5%) - Motion sensor activity
3. `mock_location` (11.2%) - Mock location flag
4. `gyroscope_variance` (10.6%) - Rotation sensor activity
5. `low_gyro_variance` (7.6%) - Binary sensor flag

**Performance**: Precision 100%, Recall 21.6%, F1 35.6%, ROC-AUC 98.1%

**Advantages**:
- ✅ Learns from data (can discover new patterns)
- ✅ Perfect precision (no false positives)
- ✅ Good for high-stakes decisions

**Disadvantages**:
- ❌ Low recall (misses 78% of spoofs)
- ❌ Requires labeled training data
- ❌ Less interpretable than rules
- ❌ Slower inference (~10ms per event)

---

### 6. Hybrid Combiner

**Purpose**: Combine rules and ML for best of both worlds

**Strategy**:
```python
# Weighted combination
spoof_score_combined = 0.6 * spoof_score_rules + 0.4 * spoof_score_ml

# Decision logic
spoof_flag = spoof_flag_rules  # Primary: Rules (better recall)
# OR spoof_flag_ml if ML has high confidence (future enhancement)
```

**Rationale for Weighting**:
- Rules have better recall (83.4% vs 21.6%)
- Rules have good precision (91.7% vs 100%)
- ML validates high-confidence cases
- 60/40 split favors proven rules

**Current Performance**: Matches rules-only (87.4% F1)

**Future Improvements**:
1. Lower ML threshold (increase recall)
2. Use ML for edge case validation
3. Train separate models for each spoof type
4. Ensemble multiple ML models

---

### 7. AI Augmentation Layer

**Purpose**: Enhance system with LLM-powered capabilities

#### 7a. Rule Proposer

**Function**: Suggest new detection rules based on schema and examples

**Workflow**:
```
Input: Data schema + example spoofed/legitimate events
↓
LLM Prompt: "Propose 5 interpretable rules for detecting location spoofing"
↓
LLM Output: [
  {"name": "impossible_speed", "logic": "...", "threshold": "150 km/h", ...},
  ...
]
↓
Human Review: Evaluate rules, select promising candidates
↓
Implementation: Code new rules into detection engine
```

**Use Cases**:
- Brainstorming new detection approaches
- Discovering patterns humans might miss
- Accelerating rule development

**Implementation**: Mocked (prompts provided, deterministic logic simulates LLM)

#### 7b. Weak Labeler

**Function**: Create labels from textual notes for semi-supervised learning

**Workflow**:
```
Input: Event note (e.g., "teleported to random location") + features
↓
LLM Prompt: "Based on this note, is the event spoofed or legitimate?"
↓
LLM Output: {
  "prediction": "spoofed",
  "confidence": 0.85,
  "reasoning": "Note explicitly mentions teleportation"
}
↓
Use as weak label for training (with confidence weighting)
```

**Use Cases**:
- Labeling data when ground truth unavailable
- Augmenting training data
- Active learning (prioritize manual labeling)

**Implementation**: Mocked (keyword matching simulates LLM reasoning)

#### 7c. Explainer

**Function**: Generate human-readable explanations for flagged events

**Workflow**:
```
Input: Event features + triggered rules + ML score
↓
LLM Prompt: "Explain why this event was flagged as spoofed"
↓
LLM Output: "This event was flagged because mock location software
             is enabled and GPS accuracy is suspiciously perfect.
             Confidence: high."
↓
Display to reviewer/user for transparency
```

**Use Cases**:
- Customer support (explain why user was flagged)
- Manual review (help reviewers understand flagging)
- Audit trail (document decision reasoning)

**Implementation**: Mocked (template-based generation with rule names)

**Benefits of AI Augmentation**:
- ✅ Accelerates rule development
- ✅ Enables semi-supervised learning
- ✅ Improves explainability and trust
- ✅ Reduces manual labeling effort

---

## Design Decisions & Trade-offs

### 1. Hybrid Architecture (Rules + ML)

**Decision**: Use rules as primary, ML as secondary validation

**Rationale**:
- Rules have better recall and are interpretable
- ML provides additional validation with perfect precision
- Hybrid allows best of both worlds

**Trade-offs**:
- ➕ Better recall than ML-only
- ➕ More interpretable than ML-only
- ➖ More complex than single approach
- ➖ Requires tuning both components

---

### 2. Feature Engineering (33 features)

**Decision**: Extensive feature engineering before ML

**Rationale**:
- Domain knowledge encoded in features
- Helps shallow models (RF) perform well
- Makes features interpretable

**Trade-offs**:
- ➕ Better performance with limited data
- ➕ More interpretable features
- ➖ Requires domain expertise
- ➖ May not generalize to new spoof types

---

### 3. Threshold Tuning (Precision ≥ 85%)

**Decision**: Tune ML model for high precision, accept lower recall

**Rationale**:
- False positives costly (user friction)
- Rules provide recall, ML adds precision
- Geo-compliance context favors precision

**Trade-offs**:
- ➕ Zero false accusations (100% precision)
- ➕ Suitable for high-stakes decisions
- ➖ Misses majority of spoofs (21.6% recall)
- ➖ Requires rules to compensate

---

### 4. Synthetic Data Only

**Decision**: Generate synthetic data, no real-world data

**Rationale**:
- Assignment requirement
- Privacy-preserving (no PII)
- Controllable distributions

**Trade-offs**:
- ➕ No privacy concerns
- ➕ Perfect ground truth
- ➕ Controllable patterns
- ➖ May not capture real-world complexity
- ➖ Needs validation on real data

---

### 5. Mocked AI Augmentation

**Decision**: Implement mocked LLM responses, not real API calls

**Rationale**:
- No external API access in assignment
- Deterministic and reproducible
- Shows prompts and expected behavior

**Trade-offs**:
- ➕ Reproducible results
- ➕ No API costs
- ➕ Clear prompts for future use
- ➖ Not as sophisticated as real LLM
- ➖ Can't discover truly novel insights

---

### 6. Advanced Models Evolution

**Decision**: Implement iterative improvements from baseline to state-of-the-art

**Evolution Path**:
1. **Random Forest (Baseline)**: F1 = 35.6% (failed due to class imbalance)
2. **XGBoost (Improved)**: F1 = 88.6% (added class weights + F1 tuning)
3. **XGBoost + Sequential Features**: F1 = 95.6% (added temporal context)
4. **Ensemble (XGB+LGBM+CatBoost)**: F1 = 93.4% (model diversity)
5. **LSTM**: F1 = 95.6% (deep learning on sequences)

**Key Breakthrough**: Sequential features (velocity, acceleration, trajectory)
- Added 17 temporal features from previous/next events
- Enabled detection of impossible movements and trajectory anomalies
- Improved F1 by +7 percentage points (88.6% → 95.6%)

**Rationale**:
- Single-event features insufficient for trajectory-based spoofs
- Temporal context critical for detecting teleportation, sudden changes
- Sequential features make spoofing patterns obvious to models

**Architecture Choices**:

**XGBoost + Sequential** (Recommended):
```
Input: [33 base features + 17 sequential features]
Model: XGBoost (100 trees, max_depth=6, scale_pos_weight=4.0)
Output: spoof_probability
Performance: Precision 92.6%, Recall 98.7%, F1 95.6%
Inference: <10ms per batch
```

**LSTM** (High-Precision Alternative):
```
Input: Sequence of 5 events × 25 features
Model: 2-layer LSTM (64 units, dropout=0.3)
       → Dense(32) → Dense(1)
Parameters: 58,689
Output: spoof_probability
Performance: Precision 96.0%, Recall 95.1%, F1 95.6%
Inference: GPU-accelerated, ~20ms per batch
```

**Ensemble** (Most Robust):
```
Models: XGBoost (0.4) + LightGBM (0.3) + CatBoost (0.3)
Combination: Weighted average of probabilities
Output: ensemble_probability
Performance: Precision 93.7%, Recall 93.1%, F1 93.4%
Advantage: Graceful degradation if one model fails
```

**Trade-offs**:

| Model | Pros | Cons | Best For |
|-------|------|------|----------|
| **XGBoost + Sequential** | Highest recall (98.7%), fast inference, explainable | Requires historical data | Production (catch all spoofs) |
| **LSTM** | Highest precision (96%), automatic pattern learning | Requires GPU, black-box | High-value transactions |
| **Ensemble** | Most robust, model diversity | Slower (3× models), higher complexity | Critical systems |

---

## Scalability Considerations

### Performance Characteristics

| Component | Latency | Throughput | Notes |
|-----------|---------|------------|-------|
| Feature Engineering | <1ms | 10k+ events/sec | Pure Python, vectorized |
| Rules Engine | <1ms | 10k+ events/sec | Simple comparisons |
| ML Inference | ~10ms | 100-1k events/sec | RF prediction (33 features) |
| AI Explainer | <1ms | 10k+ events/sec | Template-based (mocked) |
| **Total Pipeline** | ~11ms | 100-1k events/sec | Bottleneck: ML inference |

### Scalability Strategies

1. **Batching**: Process events in batches of 100-1000
2. **Caching**: Cache model in memory, avoid reload
3. **Parallel Processing**: Multi-process pool for ML inference
4. **Async**: Non-blocking inference for low-latency responses
5. **Hardware**: GPU inference for deep learning (future)

### Deployment Architectures

**Option 1: Real-time API**
```
Mobile SDK → REST API → Detection Pipeline → Response
Latency: ~50ms (including network)
Throughput: 100-1k req/sec per instance
```

**Option 2: Batch Processing**
```
Mobile SDK → Event Queue → Batch Processor → Database
Latency: Minutes (asynchronous)
Throughput: 10k-100k events/sec
```

**Option 3: Hybrid (Recommended)**
```
Mobile SDK → {
  High-priority: Real-time API (rules-only, <1ms)
  Low-priority: Batch processing (rules + ML, minutes)
}
```

---

## Monitoring & Operations

### Key Metrics to Track

1. **Performance**:
   - Precision, Recall, F1 (daily)
   - Latency (p50, p95, p99)
   - Throughput (events/sec)

2. **Business**:
   - Spoof detection rate (%)
   - False positive rate (user complaints)
   - Manual review queue depth

3. **System Health**:
   - API uptime
   - Model inference errors
   - Data pipeline failures

### Alerting

- ⚠️ Precision drops below 85%
- ⚠️ Recall drops below 75%
- ⚠️ Latency p95 > 100ms
- ⚠️ API error rate > 1%

---

## Security & Privacy

### Privacy-Preserving Design

- ✅ No PII collected (no names, emails, phone numbers)
- ✅ Device IDs hashed/anonymized
- ✅ Locations aggregated (rounded to ~10km)
- ✅ Data retention policy (delete after 90 days)

### Security Considerations

- 🔒 Encrypt data in transit (TLS)
- 🔒 Encrypt data at rest (AES-256)
- 🔒 Rate limiting (prevent API abuse)
- 🔒 Access control (RBAC for manual review)

### Adversarial Robustness

- ⚔️ Randomize thresholds per user (prevent evasion)
- ⚔️ Adversarial training (generate evasive examples)
- ⚔️ Monitor for distribution shifts (detect attacks)
- ⚔️ Red team testing (simulate attackers)

---

## Conclusion

This detection system evolved from interpretable rules to **state-of-the-art ML models** achieving 95.6% F1 through iterative improvements. The final architecture prioritizes:

1. **Performance**: **95.6% F1** with 98.7% recall (only 11 missed spoofs out of 874)
2. **Explainability**: Sequential features (velocity, acceleration) are interpretable
3. **Scalability**: <10ms inference (XGBoost), GPU-accelerated (LSTM)
4. **Robustness**: Ensemble provides graceful degradation

**Evolution Summary**:
- Baseline (Random Forest): 35.6% F1 ❌
- Improved (XGBoost): 88.6% F1 ✅
- **Advanced (XGBoost + Sequential)**: **95.6% F1** ⭐

**Key Breakthrough**: Temporal features (velocity, acceleration, trajectory) improved F1 by +7 percentage points

**Production-ready**: Advanced models ready for pilot deployment with real-world validation and monitoring.

---

**System Version**: 2.0 (Advanced Models)
**Last Updated**: January 2025
**Architecture**: Hybrid (Rules + Advanced ML + AI Augmentation)
**Best Model**: XGBoost + Sequential Features (F1 = 95.6%, Recall = 98.7%)
