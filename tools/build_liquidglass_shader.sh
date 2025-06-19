#!/bin/bash
set -e

SCRIPT_DIR="$(dirname "$0")"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
SHADER="$REPO_ROOT/Waifu2x-Extension-QT/shaders/liquidglass.frag"
OUTPUT="$REPO_ROOT/Waifu2x-Extension-QT/shaders/liquidglass.frag.qsb"

if ! command -v qsb >/dev/null; then
    echo "Error: qsb not found in PATH. Install the Qt shader tools package." >&2
    exit 1
fi

qsb "$SHADER" -o "$OUTPUT"
echo "Generated $OUTPUT"
