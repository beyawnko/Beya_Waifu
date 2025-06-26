Task: Add conditional checks in build_projects.sh to handle missing upscaler models. Updated README and tests accordingly.
- Added presence/emptiness checks before copying Real-ESRGAN and Real-CUGAN models on both Windows and Linux builds.
- Modified tests/test_build_projects_windows.py to include 'ls' and 'rm' utilities so script runs with new checks.
- Ensured build succeeds even when models directories are missing; tests pass.
Related memory: [2025-06-23-waifu2x-extension-qt-analysis.md](2025-06-23-waifu2x-extension-qt-analysis.md), [2025-06-24T223635Z-ci-ncnn-pythonpath.md](2025-06-24T223635Z-ci-ncnn-pythonpath.md).
