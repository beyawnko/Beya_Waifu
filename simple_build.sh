#!/bin/bash
set -ex # Exit on error, print commands

# Add common Qt bin locations to PATH
export PATH=/usr/lib/qt6/bin:$PATH
export PATH=/opt/qt6/bin:$PATH
export PATH=/usr/bin:$PATH # Ensure standard user bin is checked

# Navigate to the project directory
cd Waifu2x-Extension-QT
echo "Current directory: $(pwd)"

# Try to find qmake (prefer qmake6)
echo "Locating qmake..."
QMAKE_CMD=""
if command -v qmake6 &> /dev/null; then
    QMAKE_CMD="qmake6"
elif command -v qmake &> /dev/null; then
    QMAKE_CMD="qmake"
else
    echo "qmake (or qmake6) not found in PATH even after attempting to add common locations."
    if [ -f /usr/bin/qmake6 ]; then
        QMAKE_CMD="/usr/bin/qmake6"
        echo "Found qmake6 at /usr/bin/qmake6"
    elif [ -f /usr/lib/qt6/bin/qmake6 ]; then
        QMAKE_CMD="/usr/lib/qt6/bin/qmake6"
        echo "Found qmake6 at /usr/lib/qt6/bin/qmake6"
    else
        echo "Could not find qmake or qmake6."
        exit 1
    fi
fi
echo "Using QMAKE_CMD: $QMAKE_CMD"
$QMAKE_CMD --version || (echo "qmake version check failed"; exit 1)

echo "Locating make..."
which make || (echo "make not found in PATH"; exit 1)
make --version || (echo "make version check failed"; exit 1)

echo "Running qmake..."
$QMAKE_CMD Waifu2x-Extension-QT.pro CONFIG+=debug CONFIG+=qml_debug CONFIG+=qtquickcompiler || $QMAKE_CMD Waifu2x-Extension-QT.pro

echo "Compiling shaders..."
SHADER_DIR="./shaders" # Relative to Waifu2x-Extension-QT directory
QSB_CMD=""
if command -v qsb6 &> /dev/null; then
    QSB_CMD="qsb6"
elif command -v qsb &> /dev/null; then
    QSB_CMD="qsb"
else
    echo "qsb (or qsb6) not found in PATH."
    # Try to find it in a standard location from apt install
    if [ -f /usr/lib/qt6/bin/qsb ]; then # Common path for qsb after qt6-base-dev-tools
        QSB_CMD="/usr/lib/qt6/bin/qsb"
        echo "Found qsb at /usr/lib/qt6/bin/qsb"
    elif [ -f /usr/bin/qsb6 ]; then
        QSB_CMD="/usr/bin/qsb6"
        echo "Found qsb6 at /usr/bin/qsb6"
    elif [ -f /usr/lib/qt6/bin/qsb6 ]; then # Common path for qsb6
        QSB_CMD="/usr/lib/qt6/bin/qsb6"
        echo "Found qsb6 at /usr/lib/qt6/bin/qsb6"
    else
        echo "Could not find qsb or qsb6. Shader compilation might fail."
        # Allow to continue, make might fail with a clearer error if qsb files are truly needed and not generated.
    fi
fi

if [ -n "$QSB_CMD" ]; then
    echo "Using QSB_CMD: $QSB_CMD"
    $QSB_CMD --version || echo "qsb version check failed (non-critical)"
    for SHADER_FILE in $(find $SHADER_DIR -name '*.frag' -o -name '*.vert'); do
        OUTPUT_NAME="$SHADER_FILE.qsb"
        echo "Compiling $SHADER_FILE to $OUTPUT_NAME"
        $QSB_CMD "$SHADER_FILE" -o "$OUTPUT_NAME"
    done
else
    echo "Skipping shader compilation as qsb/qsb6 was not found."
fi
echo "Shader compilation finished."

echo "Running make clean..."
make clean

echo "Running make -j4..."
# Capture all output (stdout and stderr) to the log file
make -j4 > ../WinBuildErrorLog.md 2>&1

echo "Build attempt finished. Check WinBuildErrorLog.md"

# Check if the executable exists (example name, might need adjustment)
if [ -f ./release/Waifu2x-Extension-QT.exe ] || [ -f ./Waifu2x-Extension-QT ] || [ -f ./debug/Waifu2x-Extension-QT.exe ] || [ -f ./debug/Waifu2x-Extension-QT ]; then
    echo "Build successful, executable found."
else
    echo "Build might have failed, executable not found at expected locations."
fi
