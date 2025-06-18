# Memory: Remove compatibility test progress bar init declaration

Deleted the unused `Init_progressBar_CompatibilityTest()` declaration from `mainwindow.h`. No definition or calls existed, so the removal cleans up dead code. All tests pass (`pytest -q`).

Related memories:
- [Compatibility test progress bar resets](2025-06-18T213809Z-compat-test-progress-reset.md)
- [Anime4K HDN passes enable fix](2025-06-18T195718Z-hdn-passes-fix.md)
