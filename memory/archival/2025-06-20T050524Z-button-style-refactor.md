# Centralized QPushButton Styles

## Summary
- Extracted common QPushButton styles from `mainwindow.ui` into new stylesheet `styles/button.qss`.
- Styles are now applied via dynamic `class` properties rather than inline blocks.
- Updated `UiController` to load `button.qss` alongside existing theme.

## Related
- [[2025-06-19T022900Z-mainwindow-ui-test.md]]
- [[2025-06-19T015431Z-style-fallback.md]]
