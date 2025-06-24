# Memory: Update cppcheck workflow to Qt6

Adjusted `.github/workflows/cppcheck.yml` so the CI installs `qt6-base-dev` instead of the old `qtbase5-dev` package. This ensures Qt 6 headers are available for static analysis. All tests pass after the change.

Related memories:
- [Drop Qt5Compat support](2025-06-24T183813Z-drop-qt5compat.md)
- [Integrate clazy static analysis](2025-06-25T230000Z-clazy-checks.md)
