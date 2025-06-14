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

# Copy prebuilt upscaler binaries if available
if [[ "$(uname -s)" == MSYS_* || "$(uname -s)" == CYGWIN_* ]]; then
    for d in "realcugan-ncnn-vulkan Win" "realesrgan-ncnn-vulkan-20220424-windows"; do
        if [ -d "$d" ]; then
            echo "Copying contents of $d into build directories"
            cp -r "$d"/* Waifu2x-Extension-QT/ 2>/dev/null || true
            cp -r "$d"/* Waifu2x-Extension-QT-Launcher/ 2>/dev/null || true
        fi
    done
fi

echo "Build complete."
