# Run CMake build tests

Executed full pytest suite after installing `PySide6` and dependencies via `pip`. Missing system libraries `libEGL.so.1` caused import errors for PySide6; resolved by installing `libegl1` and `qt6-*` development packages with `apt-get`. All 23 tests passed with 47 skipped. Related to previous QA review and clazy integration.

Related memories:
- [Qt6 + C++17 QA Review](2025-06-24T224736Z-qt6-qa-review.md)
- [Integrate clazy static analysis](2025-06-25T230000Z-clazy-checks.md)
