# LiquidGlass Shader Build Instructions

## Summary
- Documented that `shaders/liquidglass.frag.qsb` must be generated with `qsb` before building or running GUI tests.
- Added `tools/build_liquidglass_shader.sh` to compile the shader.
- Tests now skip the Liquid Glass QML test when the `.qsb` file is absent.

## Related
- [2025-06-19T011114Z-contrib-tests.md](2025-06-19T011114Z-contrib-tests.md)
- [2025-06-19T015431Z-style-fallback.md](2025-06-19T015431Z-style-fallback.md)
