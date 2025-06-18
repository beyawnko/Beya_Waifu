# Memory: Drop-file label references removed

Today I removed the dynamic relayout block in `mainwindow.cpp` and commented out
all remaining uses of `label_DropFile` in `table.cpp`. Build attempts via
`simple_build.sh` failed due to missing Qt packages, and `build_windows.ps1`
could not run because PowerShell was unavailable. After installing the Python
requirements, all tests passed (19 passed, 42 skipped).

Related memories:
- [Initial Directories](2025-06-18-initial-directories.md)
- [Follow AGENTS Instructions](2025-06-18-run-tests-patch-agents.md)
