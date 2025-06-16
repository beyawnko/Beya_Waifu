#!/bin/bash
set -e # Exit immediately if a command exits with a non-zero status.

# Test PySide6 import
echo "Testing PySide6 import..."
python3 -c "import PySide6"
if [ $? -ne 0 ]; then
    echo "Error: PySide6 import failed. Please ensure PySide6 is installed and accessible."
    exit 1
fi
echo "PySide6 import test successful."

# Ensure Qt tools are in PATH (usually are if installed via apt)
# Configure Qt toolchain location
QTDIR=/usr/lib/qt6
export PATH="$QTDIR/bin:$PATH"

git submodule update --init --recursive
cd Waifu2x-Extension-QT

# Run qmake to generate the Makefile
# Try qmake6 first, then qmake
if command -v qmake6 &> /dev/null
then
    QMAKE_EXECUTABLE=qmake6
elif command -v qmake &> /dev/null
then
    QMAKE_EXECUTABLE=qmake
else
    echo "qmake or qmake6 not found. Please install Qt development tools."
    exit 1
fi

$QMAKE_EXECUTABLE Waifu2x-Extension-QT.pro -spec linux-g++ CONFIG+=debug

# Clean specific previous build artifacts to ensure shader is rebuilt
echo "Cleaning specific build artifacts..."
rm -f shaders/liquidglass.frag.qsb Beya_Waifu

# Build the project and redirect all output (stdout and stderr) to WinBuildErrorLog.md
# Using script to tee output to file and stdout
(make -j$(nproc) 2>&1) | tee ../WinBuildErrorLog.md

echo "Build attempt finished. Check WinBuildErrorLog.md for results."
