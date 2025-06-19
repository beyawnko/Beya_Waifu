# Qt Style Fallback Logic

## Summary
- Replaced unconditional style initialization with fallback logic in both main executables.
- Added a PySide6 test ensuring QApplication::style is not null when applying this logic.
- All existing tests including the new one pass.

## Related
- [2025-06-18T215543Z-turn-off-screen.md](2025-06-18T215543Z-turn-off-screen.md)
- [2025-06-19T011114Z-contrib-tests.md](2025-06-19T011114Z-contrib-tests.md)
