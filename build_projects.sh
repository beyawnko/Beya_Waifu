#!/bin/bash
set -e

# Build Waifu2x-Extension-QT
pushd Waifu2x-Extension-QT >/dev/null
qmake Waifu2x-Extension-QT.pro
make liquidglass_frag
make
popd >/dev/null

# Build optional launcher
pushd Waifu2x-Extension-QT-Launcher >/dev/null
qmake Waifu2x-Extension-QT-Launcher.pro
make
popd >/dev/null

echo "Build complete."
