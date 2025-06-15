#!/bin/bash
set -e
QT_INCLUDE_DIR=$(pkg-config --variable=includedir Qt5Core)
 cppcheck --enable=all --library=qt --template=gcc --force --std=c++11 \
         @tools/cppcheck.cfg \
         -I "$QT_INCLUDE_DIR" \
         -I "$QT_INCLUDE_DIR/QtCore" \
         -I "$QT_INCLUDE_DIR/QtGui" \
         -I "$QT_INCLUDE_DIR/QtWidgets" \
          -I "/usr/include" \
         Waifu2x-Extension-QT 2> cppcheck.log
