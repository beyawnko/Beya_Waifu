#!/bin/bash
set -e

if ! command -v pkg-config >/dev/null; then
    echo "Error: pkg-config not found. Install pkg-config to run static analysis." >&2
    exit 1
fi

QT_PKG=""
if pkg-config --exists Qt5Core; then
    QT_PKG=Qt5Core
elif pkg-config --exists Qt6Core; then
    QT_PKG=Qt6Core
else
    echo "Error: Qt development packages not found. Install qtbase5-dev or qtbase6-dev." >&2
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
