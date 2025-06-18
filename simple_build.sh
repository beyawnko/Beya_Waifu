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

# Verify required Qt development packages are installed
echo "Checking Qt dependencies..."
MISSING_PKGS=()
for pkg in qt6-base-dev qt6-base-dev-tools qt6-multimedia-dev qt6-5compat-dev qt6-shadertools-dev; do
    if ! dpkg -s "$pkg" >/dev/null 2>&1; then
        MISSING_PKGS+=("$pkg")
    fi
done
if [ ${#MISSING_PKGS[@]} -ne 0 ]; then
    echo "Missing Qt packages: ${MISSING_PKGS[*]}"
    echo "Install them using:"
    echo "sudo apt install ${MISSING_PKGS[*]}"
    exit 2
fi

echo "Locating make..."
which make || (echo "make not found in PATH"; exit 1)
make --version || (echo "make version check failed"; exit 1)

echo "Running qmake..."
set +e
QMAKE_OUTPUT=$($QMAKE_CMD Waifu2x-Extension-QT.pro CONFIG+=debug CONFIG+=qml_debug CONFIG+=qtquickcompiler 2>&1)
QMAKE_STATUS=$?
set -e
if [ $QMAKE_STATUS -ne 0 ]; then
    echo "$QMAKE_OUTPUT"
    if echo "$QMAKE_OUTPUT" | grep -q "Unknown module(s) in QT"; then
        echo "Unknown Qt modules detected. Install the Qt 6 development packages as described in README, e.g.:"
        echo "sudo apt install qt6-base-dev qt6-base-dev-tools qt6-multimedia-dev qt6-5compat-dev qt6-shadertools-dev"
        exit 2
    fi
    echo "Retrying qmake without additional CONFIG options..."
    set +e
    QMAKE_OUTPUT=$($QMAKE_CMD Waifu2x-Extension-QT.pro 2>&1)
    QMAKE_STATUS=$?
    set -e
    if [ $QMAKE_STATUS -ne 0 ]; then
        echo "$QMAKE_OUTPUT"
        if echo "$QMAKE_OUTPUT" | grep -q "Unknown module(s) in QT"; then
            echo "Unknown Qt modules detected. Install the Qt 6 development packages as described in README, e.g.:"
            echo "sudo apt install qt6-base-dev qt6-base-dev-tools qt6-multimedia-dev qt6-5compat-dev qt6-shadertools-dev"
            exit 2
        fi
        exit $QMAKE_STATUS
    fi
fi

echo "Compiling shaders..."
SHADER_DIR="./shaders" # Relative to Waifu2x-Extension-QT directory
QSB_CMD=""
if command -v qsb6 &> /dev/null; then
    QSB_CMD="qsb6"
elif command -v qsb &> /dev/null; then
    QSB_CMD="qsb"
elif [ -f /usr/lib/qt6/bin/qsb ]; then
    QSB_CMD="/usr/lib/qt6/bin/qsb"
elif [ -f /usr/bin/qsb6 ]; then
    QSB_CMD="/usr/bin/qsb6"
elif [ -f /usr/lib/qt6/bin/qsb6 ]; then
    QSB_CMD="/usr/lib/qt6/bin/qsb6"
else
    echo "Could not find qsb or qsb6."
    echo "Install the Qt 6 shader tools package, e.g.:"
    echo "sudo apt install qt6-shadertools-dev"
    exit 3
fi

if [ -n "$QSB_CMD" ]; then
    echo "Using QSB_CMD: $QSB_CMD"
    $QSB_CMD --version || echo "qsb version check failed (non-critical)"
    for SHADER_FILE in $(find $SHADER_DIR -name '*.frag' -o -name '*.vert'); do
        OUTPUT_NAME="$SHADER_FILE.qsb"
        echo "Compiling $SHADER_FILE to $OUTPUT_NAME"
        $QSB_CMD "$SHADER_FILE" -o "$OUTPUT_NAME"
    done
fi
echo "Shader compilation finished."

echo "Running make clean..."
make clean

echo "Running make -j4..."
# Capture all output (stdout and stderr) to the log file
make -j4 > ../WinBuildErrorLog.md 2>&1

echo "Build attempt finished. Check WinBuildErrorLog.md"

# Check if the executable exists (example name, might need adjustment)
if [ -f ./release/Beya_Waifu.exe ] || [ -f ./Beya_Waifu ] || [ -f ./debug/Beya_Waifu.exe ] || [ -f ./debug/Beya_Waifu ]; then
    echo "Build successful, executable found."
else
    echo "Build might have failed, executable not found at expected locations."
fi
