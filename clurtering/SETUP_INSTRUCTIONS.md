# Setup Instructions - Phase 1 Complete

## Quick Setup (5 minutes)

### 1. Verify Python Installation

```bash
python --version
# Should be 3.10 or higher
```

### 2. Create Virtual Environment

```bash
# Navigate to project directory
cd C:\Users\a5153207\Documents\code\clurtering

# Create virtual environment
python -m venv venv

# Activate it (Windows)
venv\Scripts\activate

# Your prompt should now show (venv)
```

### 3. Install Dependencies

```bash
pip install --upgrade pip
pip install -r requirements.txt
```

This will install:
- Core ML libraries (scikit-learn, numpy, pandas)
- NLP tools (sentence-transformers, transformers)
- Clustering algorithms (hdbscan, umap-learn)
- Graph libraries (networkx, python-louvain)
- Testing tools (pytest)
- Jupyter for notebooks

**Note**: First install may take 5-10 minutes as it downloads embedding models.

### 4. Verify Installation

```bash
# Run tests
pytest tests/ -v

# Should see: "15 passed in XX seconds"
```

### 5. Run Quick Demo

```bash
python quick_start.py
```

Expected output:
```
======================================================================
Candidate Clustering System - Phase 1 Demo
Skill Normalization Pipeline
======================================================================

[Step 1] Generating sample candidate data...
✓ Generated 100 candidates
...
Phase 1: Skill Normalization - COMPLETE ✓
```

## Detailed Setup Options

### Option A: Minimal Install (Testing Only)

```bash
pip install numpy pandas scikit-learn pytest
```

This installs only core dependencies for running tests without embedding models.

### Option B: Full Install with Development Tools

```bash
pip install -r requirements.txt
pip install jupyter black isort pytest-cov
```

This includes:
- Jupyter notebooks for interactive development
- Code formatting tools (black, isort)
- Test coverage reporting

### Option C: Install as Package

```bash
pip install -e .
```

This makes the package importable from anywhere:

```python
from candidate_clustering.skills import SkillNormalizer
```

## Directory Structure After Setup

```
clurtering/
├── venv/                          # Virtual environment (created)
├── data/
│   └── processed/                 # Created by quick_start.py
│       ├── skill_mappings.json
│       └── candidates_normalized.pkl
├── src/candidate_clustering/       # Source code
├── tests/                          # Tests
├── notebooks/                      # Jupyter notebooks
├── config/                         # Configuration
└── quick_start.py                 # Demo script
```

## Running Components

### 1. Quick Start Script (Recommended for First Run)

```bash
python quick_start.py
```

Demonstrates:
- Sample data generation
- 3-tier normalization
- Results and statistics

### 2. Interactive Notebook

```bash
jupyter notebook notebooks/01_skill_normalization_demo.ipynb
```

Provides step-by-step walkthrough with visualizations.

### 3. Tests

```bash
# All tests
pytest tests/ -v

# Specific test file
pytest tests/test_normalizer.py -v

# With coverage report
pytest tests/ --cov=src/candidate_clustering --cov-report=html
open htmlcov/index.html  # View coverage in browser
```

### 4. Python REPL

```python
from candidate_clustering.skills.normalizer import SkillNormalizer

normalizer = SkillNormalizer()
canonical, conf = normalizer.normalize("Python3.9")
print(f"{canonical} ({conf})")
# Output: python (0.98)
```

## Troubleshooting

### Issue 1: "No module named 'sentence_transformers'"

**Solution**: Install transformers
```bash
pip install sentence-transformers
```

### Issue 2: "Failed building wheel for hdbscan"

**Solution**: Install build tools
```bash
# Windows: Install Microsoft C++ Build Tools
# https://visualstudio.microsoft.com/visual-cpp-build-tools/

# Or use pre-built wheel
pip install hdbscan --prefer-binary
```

### Issue 3: Jupyter kernel not found

**Solution**: Install ipykernel
```bash
pip install ipykernel
python -m ipykernel install --user --name=venv
```

### Issue 4: Out of memory during embedding generation

**Solution**: Use smaller batch size or CPU-only mode
```bash
# Set environment variable to limit memory
set OMP_NUM_THREADS=1
python quick_start.py
```

### Issue 5: Slow embedding generation

**Solution**: Use GPU if available

Check GPU availability:
```python
import torch
print(f"CUDA available: {torch.cuda.is_available()}")
print(f"Device: {torch.cuda.get_device_name(0) if torch.cuda.is_available() else 'CPU'}")
```

If you have NVIDIA GPU, install CUDA-enabled PyTorch:
```bash
pip install torch --index-url https://download.pytorch.org/whl/cu118
```

## Configuration

### Edit Normalization Rules

File: `config/normalization_rules.json`

Add custom abbreviations, typos, or synonyms:

```json
{
  "abbreviations": {
    "py": "python",
    "your_abbrev": "full_name"
  },
  "common_typos": {
    "your_typo": "correct_spelling"
  }
}
```

### Edit Pipeline Settings

File: `config/config.yaml`

Adjust clustering parameters:

```yaml
normalization:
  tier2:
    min_cluster_size: 3      # Increase for stricter clustering
    similarity_threshold: 0.85  # Increase for higher confidence
```

## Next Steps After Setup

1. **Run the demo**:
   ```bash
   python quick_start.py
   ```

2. **Explore the notebook**:
   ```bash
   jupyter notebook notebooks/01_skill_normalization_demo.ipynb
   ```

3. **Review results**:
   - Check `data/processed/skill_mappings.json`
   - Load `data/processed/candidates_normalized.pkl`

4. **Customize for your data**:
   - Replace sample data with real candidate data
   - Adjust normalization rules in config
   - Re-run pipeline

## System Requirements

### Minimum
- Python 3.10+
- 4GB RAM
- 2GB disk space

### Recommended
- Python 3.11+
- 16GB RAM
- 5GB disk space
- NVIDIA GPU with 4GB+ VRAM (optional)

## Getting Help

- **Documentation**: See README.md
- **Phase 1 Summary**: See PHASE1_SUMMARY.md
- **Tests**: Run `pytest tests/ -v` to verify functionality
- **Issues**: Check error messages and logs

## Success Checklist

- ✅ Python 3.10+ installed
- ✅ Virtual environment activated
- ✅ Dependencies installed
- ✅ Tests passing (`pytest tests/ -v`)
- ✅ Quick start script runs successfully
- ✅ Notebook opens without errors

**Phase 1 Setup Complete!** You're ready to proceed.
