# globalrule-ui

## Type checking (mypy)

Static type checking is configured in `pyproject.toml` (`[tool.mypy]`) and
scoped to `app/`. Run from the repo root:

```bash
.venv/bin/mypy app/
```

Notes:
- `mypy_path = "app"` mirrors `pytest.ini`'s `pythonpath = ./app/` so bare
  imports inside the source tree (`from core.loguru import logger`) resolve
  the same way they do at runtime.
- The pragmatic baseline is intentionally narrower than `--strict`. It does
  **not** enable `disallow_untyped_defs`, so untyped functions are tolerated
  for now. Tighten incrementally once the existing error backlog is drained.
- Third-party libraries without stubs are silenced per-module under
  `[[tool.mypy.overrides]]`. Add new libraries there only after confirming
  no stub package exists on PyPI (`pip install types-<lib>`).
- Baseline at the time mypy was introduced: **827 errors in 117 files** —
  not blocking; subsequent changes should drive that number down.
