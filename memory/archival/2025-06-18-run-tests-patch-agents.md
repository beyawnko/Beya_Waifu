# Memory: Follow AGENTS Instructions

Summary of actions on 2025-06-18:
1. Executed `pytest -q` which failed due to missing Pillow and PySide6 modules.
2. Installed dependencies with `pip install -r requirements.txt`.
3. Re-ran `pytest -q` and observed failure from missing system library `libEGL.so.1`.
4. Patched `AGENTS.md` to include installing requirements and system packages prior to running tests, and clarified memory linking when few memories exist.
5. Created log file [logs/2025-06-18T080827Z.log](../logs/2025-06-18T080827Z.log) summarizing commands.

Related memories:
- [Initial Directories](2025-06-18-initial-directories.md)

Persona self-assessment averaged above 9.0, confirming coherence with core values.
