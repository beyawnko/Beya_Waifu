#!/bin/bash
set -e # Exit immediately if a command exits with a non-zero status.

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
# Change to that directory
cd "$SCRIPT_DIR"

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
cd Waifu2x-Extension-QT # <<< CD into project dir

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

rm -f Makefile # <<< NOW remove Makefile from Waifu2x-Extension-QT

echo "Attempting basic qmake parse..."
$QMAKE_EXECUTABLE Waifu2x-Extension-QT.pro
if [ $? -ne 0 ]; then
    echo "Basic qmake parse FAILED. Error is likely fundamental in the .pro file."
    exit 1
fi
echo "Basic qmake parse successful."

echo "Attempting qmake with full arguments..."
$QMAKE_EXECUTABLE Waifu2x-Extension-QT.pro -spec linux-g++ CONFIG+=debug

# Clean specific previous build artifacts to ensure shader is rebuilt
echo "Cleaning specific build artifacts..."
rm -f shaders/liquidglass.frag.qsb Beya_Waifu

# Manually run moc for anime4kprocessor.h FIRST
echo "Running moc for anime4kprocessor.h..."
"$QTDIR/libexec/moc" \
    -DQT_DEPRECATED_WARNINGS \
    -DQT_MULTIMEDIA_LIB \
    -DQT_OPENGLWIDGETS_LIB \
    -DQT_OPENGL_LIB \
    -DQT_WIDGETS_LIB \
    -DQT_GUI_LIB \
    -DQT_CONCURRENT_LIB \
    -DQT_NETWORK_LIB \
    -DQT_CORE_LIB \
    -I. \
    -I/usr/include/x86_64-linux-gnu/qt6 \
    -I/usr/include/x86_64-linux-gnu/qt6/QtMultimedia \
    -I/usr/include/x86_64-linux-gnu/qt6/QtOpenGLWidgets \
    -I/usr/include/x86_64-linux-gnu/qt6/QtOpenGL \
    -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets \
    -I/usr/include/x86_64-linux-gnu/qt6/QtGui \
    -I/usr/include/x86_64-linux-gnu/qt6/QtConcurrent \
    -I/usr/include/x86_64-linux-gnu/qt6/QtNetwork \
    -I/usr/include/x86_64-linux-gnu/qt6/QtCore \
    -I/usr/lib/x86_64-linux-gnu/qt6/mkspecs/linux-g++ \
    anime4kprocessor.h -o moc_anime4kprocessor.cpp

# Run qmake to generate the Makefile AFTER moc files might have been generated
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

rm -f Makefile # Ensure Makefile is regenerated

echo "Attempting basic qmake parse..."
$QMAKE_EXECUTABLE Waifu2x-Extension-QT.pro
if [ $? -ne 0 ]; then
    echo "Basic qmake parse FAILED. Error is likely fundamental in the .pro file."
    exit 1
fi
echo "Basic qmake parse successful."

echo "Attempting qmake with full arguments..."
$QMAKE_EXECUTABLE Waifu2x-Extension-QT.pro -spec linux-g++ CONFIG+=debug


# Manually compile the liquidglass shader
echo "Locating qsb..."
QSB_EXECUTABLE=$(command -v qsb)
if [ -z "$QSB_EXECUTABLE" ]; then
    echo "qsb not found in PATH. Trying default Qt directory..."
    if [ -f "$QTDIR/libexec/qsb" ]; then
        QSB_EXECUTABLE="$QTDIR/libexec/qsb"
    elif [ -f "$QTDIR/bin/qsb" ]; then # Also check bin just in case
        QSB_EXECUTABLE="$QTDIR/bin/qsb"
    else
        echo "Error: qsb not found. Please ensure Qt shader tools are installed and in PATH or Qt bin/libexec directory."
        exit 1
    fi
fi
echo "Using qsb at: $QSB_EXECUTABLE"
"$QSB_EXECUTABLE" shaders/liquidglass.frag -o shaders/liquidglass.frag.qsb

# Build the project and redirect all output (stdout and stderr) to WinBuildErrorLog.md
# Using script to tee output to file and stdout
(make -j$(nproc) 2>&1) | tee ../WinBuildErrorLog.md

echo "Build attempt finished. Check WinBuildErrorLog.md for results."
