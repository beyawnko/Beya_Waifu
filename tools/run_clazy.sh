#!/bin/bash
set -e

if ! command -v clazy-standalone >/dev/null 2>&1; then
    echo "Error: clazy-standalone not found. Install clazy to run static analysis." >&2
    exit 1
fi

QMAKE_CMD=""
if command -v qmake6 >/dev/null 2>&1; then
    QMAKE_CMD="qmake6"
elif command -v qmake >/dev/null 2>&1; then
    QMAKE_CMD="qmake"
else
    echo "Error: qmake or qmake6 not found. Ensure Qt development tools are installed." >&2
    exit 1
fi

BUILD_DIR=clazy-build
rm -rf "$BUILD_DIR"
mkdir "$BUILD_DIR"

pushd "$BUILD_DIR" >/dev/null
$QMAKE_CMD ../Waifu2x-Extension-QT/Waifu2x-Extension-QT.pro -spec linux-clang CONFIG+=c++17
if command -v bear >/dev/null 2>&1; then
    bear -- make -j"$(nproc)"
else
    make -j"$(nproc)"
fi
popd >/dev/null

clazy-standalone -p "$BUILD_DIR" --checks=level1 --header-filter='Waifu2x-Extension-QT/.*' > clazy.log

echo "clazy analysis finished, results stored in clazy.log"
