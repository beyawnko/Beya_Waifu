# Memory: LiquidGlass OpenGL initialization check

Implemented validation of OpenGL setup in `LiquidGlassWidget::initializeGL`.
The widget now checks that `initializeOpenGLFunctions()` succeeds and that a
3.3 context is available. If initialization or shader compilation/linking fails,
the widget logs a `qWarning()` and disables itself to avoid invalid OpenGL use.
All tests pass after installing dependencies and ensuring libEGL is present.

Related memories:
- [Initial Directories](2025-06-18-initial-directories.md)
- [Follow AGENTS Instructions](2025-06-18-run-tests-patch-agents.md)
- [Drop-file label references removed](2025-06-18T085248Z-drop-file-label-removal.md)
