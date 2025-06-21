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

QSB_BIN="/usr/lib/qt6/bin/qsb"

if [ ! -f "$QSB_BIN" ]; then
    echo "Error: qsb not found at $QSB_BIN. Please ensure qt6-shader-baker is installed correctly." >&2
    exit 1
fi

"$QSB_BIN" "$SHADER_PATH" -o "$OUTPUT_PATH"
echo "Generated $OUTPUT_PATH"
