#!/bin/bash
set -e

if ! command -v pkg-config >/dev/null; then
    echo "Error: pkg-config not found. Install pkg-config to run static analysis." >&2
    exit 1
fi

QT_PKG="Qt6Core"
if ! pkg-config --exists "$QT_PKG"; then
    echo "Error: Qt 6 development packages not found." >&2
    exit 1
fi

QT_INCLUDE_DIR=$(pkg-config --variable=includedir "$QT_PKG")

cppcheck --enable=all --library=qt --template=gcc --force --std=c++11 \
    @tools/cppcheck.cfg \
    -I "$QT_INCLUDE_DIR" \
    -I "$QT_INCLUDE_DIR/QtCore" \
    -I "$QT_INCLUDE_DIR/QtGui" \
    -I "$QT_INCLUDE_DIR/QtWidgets" \
    -I "/usr/include" \
    Waifu2x-Extension-QT 2> cppcheck.log
