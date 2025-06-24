# Contributing to Beya_Waifu

Thank you for considering a contribution! Please follow these steps when preparing changes:

1. Clone the repository with submodules:
   ```bash
   git clone --recursive https://github.com/beyawnko/Beya_Waifu.git
   ```
2. Install the required Python packages before running the test suite:
   ```bash
   pip install -r requirements.txt
   ```
3. Generate the Liquid Glass shader if `shaders/liquidglass.frag.qsb` is missing:
   ```bash
   ./tools/build_liquidglass_shader.sh
   ```
4. Execute the tests from the repository root:
   ```bash
   pytest
   ```
   The suite relies on `PySide6` and other packages from `requirements.txt`. Tests that
   depend on PySide6 will be skipped automatically if it is not available.
5. Header files under `Waifu2x-Extension-QT` should use `#pragma once` for include guards.

See the `README.md` for build instructions and additional details.
