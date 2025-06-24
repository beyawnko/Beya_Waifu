# Memory: Ensure ncnn Python modules importable in CI

Updated `.github/workflows/ci.yml` so the Linux job installs `numpy` and `pybind11` after requirements, then exports `PYTHONPATH` to include the repository's local `ncnn/python` and `ncnn/tools/pnnx/python` directories. The workflow also runs `pytest -q tests` to limit test discovery.

Related memories:
- [Follow AGENTS Instructions](2025-06-18-run-tests-patch-agents.md)
- [Update cppcheck workflow to Qt6](2025-06-24T222821Z-qt6-cppcheck-workflow.md)
