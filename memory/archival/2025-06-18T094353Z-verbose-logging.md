# Memory: Verbose logging

Implemented verbose logging via QLoggingCategory and command line flag.
- Added `beyaLogCategory` with filter rules in `Logger.cpp`.
- `initLogger` now accepts a `verbose` flag and handles debug filtering.
- `main.cpp` creates a logs directory, parses `--verbose` option and passes it to the logger.
- Documented usage in README and added `tests/test_verbose_option.py` to verify the flag.
All tests pass (21 passed, 42 skipped).

Related memories:
- [LiquidGlass OpenGL initialization check](2025-06-18T091029Z-liquidglass-openGL.md)
- [Drop-file label references removed](2025-06-18T085248Z-drop-file-label-removal.md)
