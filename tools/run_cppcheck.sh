#!/bin/bash
set -e
QT_INCLUDE_DIR=$(pkg-config --variable=includedir Qt5Core)
cppcheck --enable=all --library=qt --template=gcc \
         @tools/cppcheck.cfg \
         -I "$QT_INCLUDE_DIR" \
         -I "$QT_INCLUDE_DIR/QtCore" \
         -I "$QT_INCLUDE_DIR/QtGui" \
         -I "$QT_INCLUDE_DIR/QtWidgets" \
         . 2> cppcheck.log
