#!/bin/bash
set -e

# Determine the repository root using git
REPO_ROOT=$(git rev-parse --show-toplevel)

if [ -z "$REPO_ROOT" ]; then
    echo "Error: Could not determine repository root. Make sure you are in a git repository." >&2
    exit 1
fi

SHADER_PATH="$REPO_ROOT/Waifu2x-Extension-QT/shaders/liquidglass.frag"
OUTPUT_PATH="$REPO_ROOT/Waifu2x-Extension-QT/shaders/liquidglass.frag.qsb"

# Try to find qsb
QSB_BIN=$(which qsb 2>/dev/null)

# If not found by which, check common locations
if [ -z "$QSB_BIN" ]; then
    COMMON_PATHS=(
        "/usr/lib/x86_64-linux-gnu/qt6/bin/qsb" # Ubuntu Noble default
        "/usr/lib/qt6/bin/qsb"
        "/usr/bin/qsb"
        "/opt/qt6/bin/qsb" # Common for some installations
    )
    for path_option in "${COMMON_PATHS[@]}"; do
        if [ -f "$path_option" ]; then
            QSB_BIN="$path_option"
            break
        fi
    done
fi

if [ -z "$QSB_BIN" ] || [ ! -f "$QSB_BIN" ]; then
    echo "Error: qsb not found. Please ensure qt6-shader-baker is installed and qsb is in your PATH or a standard location." >&2
    exit 1
fi

echo "Using qsb at: $QSB_BIN"
"$QSB_BIN" "$SHADER_PATH" -o "$OUTPUT_PATH"
echo "Generated $OUTPUT_PATH"
