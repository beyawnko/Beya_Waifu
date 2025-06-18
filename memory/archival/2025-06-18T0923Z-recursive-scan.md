# Recursive scanning refactor

## Summary
Implemented a new recursive file scanning approach using `QDirIterator` and removed the previous `QFileSystemWatcher` based logic. The add-file dialog now executes scanning in a background thread to keep the UI responsive. Added a minimal PySide6 test verifying `QDirIterator` recursion.

## Related
- [2025-06-18-initial-directories.md](../archival/2025-06-18-initial-directories.md)
- [2025-06-18-run-tests-patch-agents.md](../archival/2025-06-18-run-tests-patch-agents.md)
