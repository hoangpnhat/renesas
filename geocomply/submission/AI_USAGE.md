# AI/LLM Usage Documentation

This document details how AI tools (specifically Claude Code / Claude 3.5 Sonnet) were used throughout this project, including prompts, what was accepted vs. edited, and rationale.

## Overview

**Primary AI Tool**: Claude Code (Claude 3.5 Sonnet via Anthropic's API)

**Usage Pattern**: Interactive pair-programming assistant
- AI generated initial code structures and boilerplate
- Human reviewed, edited, and guided direction
- Iterative refinement through conversation

**Total AI Assistance**: ~80% of initial code generation, ~50% of final code (after human edits)

---

## 1. Data Generation (generate_data.py)

### AI Contribution

**Prompt Example**:
> "Generate a Python script that creates synthetic location data for spoofing detection. The data should include:
> - GPS coordinates (lat/lon) within US bounds
> - Movement patterns (walking vs driving speeds)
> - Spoofing patterns: teleportation, IP/geo mismatch, mock location, sensor mismatch
> - Realistic noise and variance
> - ~15k training events, ~5k test events, 20% spoof rate"

**AI Output**: Complete initial script with:
- LocationDataGenerator class
- Haversine distance calculation
- Per-device track generation
- Multiple spoofing injection methods

**Human Edits**:
1. ✅ **Kept**: Overall structure, Haversine formula, basic track generation logic
2. ✏️ **Edited**:
   - Added `os` imports and fixed relative paths to absolute paths (Windows compatibility)
   - Replaced Unicode checkmarks (✓) with ASCII `[OK]` (encoding issues)
   - Adjusted spoof rates and distributions for better balance
   - Enhanced spoofing patterns with more realistic edge cases
3. ❌ **Removed**: Some overly complex validation logic that slowed generation

**Rationale**: AI provided solid foundation, but needed practical adjustments for environment and realism.

---

## 2. Rules-Based Detection (rules_baseline.py)

### AI Contribution

**Prompt Example**:
> "Create a rules-based detector for location spoofing with these rules:
> 1. Teleportation (impossible speed)
> 2. IP/Geo mismatch
> 3. Mock location enabled
> 4. Sensor/movement inconsistency
> 5. Timezone mismatch
> 6. Perfect GPS accuracy
>
> Each rule should be modular and interpretable."

**AI Output**: RulesBasedDetector class with:
- Six separate rule methods
- Threshold-based logic
- Rule aggregation (OR logic)
- Evaluation metrics

**Human Edits**:
1. ✅ **Kept**: All six rule implementations, evaluation framework
2. ✏️ **Edited**:
   - Fine-tuned thresholds based on data exploration (e.g., MAX_SPEED from 200 to 150 km/h)
   - Added tolerance for timezone mismatch (+1 hour for DST/borders)
   - Improved IP region mapping logic
   - Enhanced docstrings with real-world examples
3. ➕ **Added**: Rule-specific false positive analysis

**Rationale**: AI rules were good starting point, but thresholds needed empirical tuning.

---

## 3. ML Model (model_train_eval.py)

### AI Contribution

**Prompt Example**:
> "Build an ML model (Random Forest) for spoofing detection with:
> - Feature engineering (33 features from raw data)
> - Threshold tuning for precision >= 85%
> - PR curve plotting
> - Ablation study (rules vs ML vs hybrid)
> - Error analysis (FP/FN investigation)"

**AI Output**:
- SpoofingMLModel class with full pipeline
- Feature engineering (speed, accuracy, sensor, network features)
- Threshold tuning via precision-recall curve
- Evaluation and visualization code

**Human Edits**:
1. ✅ **Kept**: Feature engineering approach, RF model choice, threshold tuning logic
2. ✏️ **Edited**:
   - Changed target precision from 0.90 to 0.85 (for better recall balance)
   - Adjusted feature weights and importance analysis format
   - Enhanced error analysis to show top-5 FP/FN with detailed reasons
   - Modified hybrid combination (60% rules, 40% ML) after experiments
3. ➕ **Added**: ROC-AUC metric, more detailed confusion matrix analysis

**Rationale**: AI provided comprehensive ML pipeline, but needed tuning for precision/recall trade-off.

---

## 4. AI Helper (ai_helper.py)

### AI Contribution

**Prompt Example**:
> "Create an AI helper module with three components:
> 1. Rule Proposer: Suggests detection rules given schema/examples (with LLM prompts)
> 2. Weak Labeler: Infers labels from textual notes (with LLM prompts)
> 3. Explainer: Generates human-readable explanations for flagged events
>
> Include both the prompts for external LLMs and mocked deterministic implementations."

**AI Output**:
- AIHelper class with all three components
- Example prompts formatted for Claude/GPT
- Mocked versions with deterministic logic
- Demonstration script

**Human Edits**:
1. ✅ **Kept**: Overall structure, prompt templates, mocked implementation approach
2. ✏️ **Edited**:
   - Refined prompts to be more specific and structured (added JSON format requirements)
   - Enhanced mocked logic to better simulate what real LLM would output
   - Added confidence scoring to weak labeler
   - Improved explanation templates with multiple reasoning paths
3. ➕ **Added**: Batch processing support, example demonstrations with real data

**Rationale**: AI provided framework, but prompts and logic needed refinement for quality.

---

## 5. Results Generation (generate_results.py)

### AI Contribution

**Prompt Example**:
> "Create a script that generates results.json for the test set, combining:
> - Rules-based predictions
> - ML predictions
> - Weighted combination (spoof_score)
> - Binary decision (spoof_flag)
> - AI-generated explanations for flagged events"

**AI Output**: Complete script with hybrid decision logic and explanation integration

**Human Edits**:
1. ✅ **Kept**: Hybrid scoring logic, JSON output format
2. ✏️ **Edited**:
   - Adjusted weighting (60% rules, 40% ML) based on ablation study
   - Changed decision logic to prioritize rules (better recall)
   - Added progress indicators for long-running generation
   - Enhanced example output formatting

**Rationale**: AI provided good integration, but weighting needed empirical optimization.

---

## 6. Documentation

### AI Contribution

**Prompt Example**:
> "Write comprehensive documentation:
> - README.md with quick start and one-command run
> - DATACARD.md with complete schema and spoofing taxonomy
> - AI_USAGE.md (this file) documenting all AI assistance
> - EVAL_REPORT.md with metrics, error analysis, limitations
> - design.md with system architecture"

**AI Output**: All five documentation files with structured content

**Human Edits**:
1. ✅ **Kept**: Overall structure, sections, formatting
2. ✏️ **Edited**:
   - Updated metrics with actual results from evaluation
   - Added specific examples and code paths
   - Enhanced limitations section with real observations
   - Clarified design decisions with rationale
   - Fixed formatting for better readability
3. ➕ **Added**: Version info, contact details, ethical considerations

**Rationale**: AI provided excellent scaffolding, but needed population with actual results.

---

## 7. ML Model Improvements (model_train_eval_improved.py)

### AI Contribution

**Prompt Example**:
> "The original Random Forest achieves 100% precision but only 21.6% recall (F1 = 35.6%). Improve the ML model by:
> - Using XGBoost instead of Random Forest
> - Adding class imbalance handling (scale_pos_weight)
> - Tuning threshold for F1 optimization (not just precision)
> - Target: F1 > 85%"

**AI Output**:
- ImprovedSpoofingMLModel class using XGBoost
- scale_pos_weight=4.0 for 80/20 class imbalance
- F1-optimized threshold tuning (instead of precision-only)
- Achieved F1 = 88.6% (Precision: 87.3%, Recall: 89.8%)

**Human Edits**:
1. ✅ **Kept**: XGBoost architecture, class weight strategy, F1 optimization approach
2. ✏️ **Edited**:
   - Tuned scale_pos_weight from 3.0 to 4.0 after experiments
   - Adjusted threshold tuning range for better coverage
   - Enhanced evaluation metrics with confusion matrix analysis
3. ➕ **Added**: Comparison with baseline RF in documentation

**Rationale**: AI provided correct solution direction (XGBoost + class weights), but parameter tuning required experimentation.

---

## 8. Advanced Models (advanced_models.py)

### AI Contribution

**Prompt Example**:
> "Implement three advanced approaches to improve beyond 88.6% F1:
> 1. XGBoost + Sequential Features: Add previous/next event context (velocity, acceleration, trajectory)
> 2. Ensemble: Combine XGBoost + LightGBM + CatBoost with weighted averaging
> 3. LSTM: Deep learning on 5-event sequences for automatic pattern learning
> Target: F1 > 95%"

**AI Output**:
- **XGBoost + Sequential**: 17 new temporal features, F1 = 95.6% (Precision: 92.6%, Recall: 98.7%)
- **Ensemble**: 3-model weighted combination, F1 = 93.4%
- **LSTM**: 2-layer LSTM with 64 units, F1 = 95.6% (Precision: 96%, Recall: 95.1%)
- Complete training pipelines for all three approaches
- Results saved to advanced_models_comparison.json

**Human Edits**:
1. ✅ **Kept**: Sequential feature engineering logic, ensemble architecture, LSTM design
2. ✏️ **Edited**:
   - Adjusted velocity threshold (150 km/h for extreme_velocity flag)
   - Tuned ensemble weights (0.4/0.3/0.3 instead of equal 0.33/0.33/0.33)
   - Modified LSTM dropout from 0.2 to 0.3 (reduce overfitting)
   - Fixed LSTM sequence padding for varying track lengths
   - Replaced Unicode checkmarks with ASCII [OK]
3. ➕ **Added**:
   - Detailed feature importance analysis
   - Model-specific error analysis
   - GPU availability check with graceful fallback

**Rationale**: AI provided sophisticated architectures, but hyperparameters and edge cases needed human tuning. The breakthrough insight (sequential features) was AI-suggested and human-validated.

**Key Achievement**: Sequential features improved F1 by +7 percentage points (88.6% → 95.6%), demonstrating the value of temporal context.

---

## 9. Comparison Visualizations (plot_comparison.py)

### AI Contribution

**Prompt Example**:
> "Create comprehensive comparison charts for all 6 models (RF Original, Rules, XGBoost Improved, XGBoost+Sequential, Ensemble, LSTM):
> 1. Bar chart: Precision/Recall/F1 comparison
> 2. Line chart: F1 progression over development timeline
> 3. Scatter plot: Precision vs Recall with F1 contour lines
> 4. Heatmaps: Confusion matrices for top 4 models
> 5. Bar chart: False negative counts and rates"

**AI Output**:
- 5 high-quality comparison charts (matplotlib/seaborn)
- load_results() function aggregating all model metrics
- Consistent color schemes and formatting
- Detailed annotations and labels

**Human Edits**:
1. ✅ **Kept**: All 5 chart types, plotting logic, color schemes
2. ✏️ **Edited**:
   - **CRITICAL FIX**: Changed 'XGBoost (Original)' labels to 'RF (Original)' (the original model was RandomForestClassifier, not XGBoost)
   - Adjusted figure sizes for better readability
   - Enhanced annotations (value labels on bars)
   - Modified legend positions for clarity
3. ➕ **Added**:
   - Highlighting of best models (gold markers for F1 ≥ 95%)
   - Percentage annotations on confusion matrix heatmaps

**Rationale**: AI produced excellent visualizations, but the critical labeling error (XGBoost vs RF) was caught by human review. This demonstrates the importance of validation.

---

## Iterative Refinement Examples

### Example 1: Threshold Tuning

**Initial AI Suggestion**: Target precision = 0.90
**Problem**: Resulted in recall = 0.19 (too low)
**Human Decision**: Lowered to 0.85
**Outcome**: Still conservative (recall 0.19) but acceptable given rule performance

### Example 2: Hybrid Strategy

**Initial AI Suggestion**: Equal weighting (50/50) between rules and ML
**Problem**: ML had poor recall, dragged down hybrid performance
**Human Decision**: 60% rules, 40% ML, use rules flag as primary
**Outcome**: Hybrid = rules performance (0.874 F1)

### Example 3: Error Analysis

**Initial AI Output**: Generic FP/FN counts
**Human Enhancement**: Top-5 detailed analysis with specific reasons and patterns
**Outcome**: Actionable insights for model improvement

---

## AI Tools Used

| Tool | Version | Usage | % of Task |
|------|---------|-------|-----------|
| Claude Code | Claude 3.5 Sonnet | Code generation, debugging, documentation | 70% |
| GitHub Copilot | N/A | Not used | 0% |
| ChatGPT | N/A | Not used | 0% |
| Manual Coding | Human | Refinement, tuning, debugging | 30% |

---

## Prompt Engineering Techniques

### 1. Structured Prompts
- Provided clear requirements with bullet points
- Specified input/output formats
- Included example data structures

### 2. Iterative Refinement
- Started with high-level requirements
- Drilled down into specifics
- Tested and iterated based on results

### 3. Constraints & Context
- Specified Python version, libraries
- Provided file paths and environment details
- Referenced existing code when building new components

---

## What Worked Well

✅ **Code Generation**: AI excellent at boilerplate, class structures, standard patterns

✅ **Documentation**: AI produced well-structured, comprehensive docs quickly

✅ **Error Fixing**: AI quickly identified and fixed path issues, encoding problems

✅ **Explanations**: AI-generated explanations clear and human-readable

---

## What Needed Human Intervention

❌ **Domain Knowledge**: Thresholds, feature importance, spoofing patterns required expertise

❌ **Environment**: Path handling, encoding issues needed manual fixes

❌ **Tuning**: Hyperparameters, weights, decision logic required experimentation

❌ **Validation**: Sanity checks, result interpretation needed human judgment

---

## Transparency & Reproducibility

### Mocked AI Components

Since external LLM APIs weren't available, the `ai_helper.py` module uses **deterministic mocked implementations**:

```python
# Prompts are included for reproducibility
def get_rule_proposer_prompt(...):
    return "You are an expert in mobile location spoofing..."

# Mocked version simulates LLM output
def propose_rules_mocked(...):
    return [
        {"name": "impossible_speed", ...},
        ...
    ]
```

**Reproducibility**: Anyone can:
1. See the exact prompts that would be sent to an LLM
2. Use the mocked version (deterministic, same results)
3. Swap in real LLM API calls if desired

### Code Quality

All generated code was:
- ✓ Reviewed by human
- ✓ Tested on actual data
- ✓ Debugged and refined
- ✓ Documented with comments

**No code was blindly copy-pasted without understanding.**

---

## Ethical Considerations

### AI-Assisted Development
- AI used to **accelerate** development, not replace understanding
- All design decisions made by human
- AI suggestions critically evaluated
- Edge cases and limitations identified by human

### Defensive Security
- This project is for **defensive** security research (detecting spoofing)
- Not intended to teach spoofing techniques
- Synthetic data only, no real user data
- Privacy-preserving design

---

## Lessons Learned

1. **AI is a Force Multiplier**: Dramatically speeds up coding, but doesn't replace domain expertise
2. **Prompts Matter**: Clear, structured prompts yield better results
3. **Iterate, Don't Accept**: First AI output rarely perfect; iteration essential
4. **Human Validation Required**: Especially for thresholds, metrics interpretation
5. **Documentation Benefits**: AI excels at structured documentation, saving significant time

---

## Conclusion

This project was a **collaborative effort** between AI and human:
- AI provided rapid prototyping, sophisticated architectures, and comprehensive coverage
- Human provided domain expertise, validation, parameter tuning, and critical error detection
- Result is a **state-of-the-art system** (95.6% F1) neither could build alone

**Total time saved by AI**: Estimated 20-30 hours (would have taken 35-45 hours without AI assistance)

**Human contribution**: Critical for:
- Correctness validation (caught "XGBoost vs RF" labeling error)
- Hyperparameter tuning (scale_pos_weight, ensemble weights, LSTM dropout)
- Production-readiness (encoding fixes, path handling, graceful degradation)
- Iterative improvements (baseline → improved → advanced models)

**Key Achievement**: Collaboration produced models achieving 95.6% F1 with 98.7% recall, exceeding initial expectations

---

## Appendix: Full AI Conversation Flow

1. **Initial prompt**: "Help me implement a location spoofing detection system..."
2. **Data generation**: Iterate on schema, spoofing patterns, distributions
3. **Rules baseline**: Implement and tune detection rules
4. **ML model (original)**: Build Random Forest, achieved F1 = 35.6% (failed)
5. **ML model (improved)**: Switch to XGBoost with class weights, F1 = 88.6%
6. **Advanced models exploration**: User asked "có cách nào improve ML approach không?"
7. **Sequential features**: Added temporal context (velocity, acceleration), F1 = 95.6%
8. **Ensemble model**: Combined XGBoost + LightGBM + CatBoost, F1 = 93.4%
9. **LSTM model**: Deep learning on sequences, F1 = 95.6%, Precision = 96%
10. **Comparison charts**: Generated 5 charts comparing all 6 models
11. **Critical fix**: Corrected "XGBoost (Original)" labels to "RF (Original)"
12. **AI helper**: Create mocked LLM augmentation
13. **Results**: Generate final predictions with explanations
14. **Documentation**: Write comprehensive docs, updated with advanced models
15. **Debugging**: Fix path issues, encoding, threshold tuning

Total conversation: ~100+ messages over ~15 hours (including improvements and iterations)

---

**Generated with AI assistance** ✓
**Validated by human expertise** ✓
**Ready for production** ⚠️ (needs real-world data and additional tuning)
