# MainWindow UI Load Test

Implemented a new pytest `test_mainwindow_ui_load` that uses `QUiLoader` to open
`mainwindow.ui` and verify core widgets like `tabWidget` and `pushButton_Start`
exist. The test initializes `QApplication` in offscreen mode and skips when
PySide6 is unavailable.

Related memories:
- [Documentation and Test Skip Improvements](2025-06-19T011114Z-contrib-tests.md)
- [Qt Style Fallback Logic](2025-06-19T015431Z-style-fallback.md)
