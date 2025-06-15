#!/bin/bash
set -e # Exit immediately if a command exits with a non-zero status.

# Ensure Qt tools are in PATH (usually are if installed via apt)
# export QTDIR=/usr/lib/qt6 # Or /opt/qt6 - this might not be needed if qmake6 is in path
# export PATH=$QTDIR/bin:$PATH # Again, might not be needed

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

# Clean previous build artifacts
make clean

# Build the project and redirect all output (stdout and stderr) to WinBuildErrorLog.md
# Using script to tee output to file and stdout
(make -j$(nproc) 2>&1) | tee ../WinBuildErrorLog.md

echo "Build attempt finished. Check WinBuildErrorLog.md for results."
