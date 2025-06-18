# Memory: Windows screen off

Implemented `TurnOffScreen()` in `mainwindow.cpp` using the Windows API `SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2)` to power off monitors. Added conditional `windows.h` include. Non-Windows platforms now log that the feature isn't available.

Related memories:
- [Anime4K HDN passes enable fix](2025-06-18T195718Z-hdn-passes-fix.md)
- [Compatibility test progress bar resets](2025-06-18T213809Z-compat-test-progress-reset.md)
