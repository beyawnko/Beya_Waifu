#    Copyright (C) 2025  beyawnko

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Affero General Public License as
#    published by the Free Software Foundation, either version 3 of the
#    License, or (at your option) any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Affero General Public License for more details.

#    You should have received a copy of the GNU Affero General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#    My Github homepage: https://github.com/AaronFeng753

QT       += core gui
QT       += concurrent
QT       += multimedia opengl openglwidgets core5compat

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AnimatedPNG.cpp \
    CompatibilityTest.cpp \
    Current_File_Progress.cpp \
    CustomResolution.cpp \
    Donate.cpp \
    Finish_Action.cpp \
    Frame_Interpolation.cpp \
    Right-click_Menu.cpp \
    SystemTrayIcon.cpp \
    Web_Activities.cpp \
    checkupdate.cpp \
    files.cpp \
    gif.cpp \
    image.cpp \
    main.cpp \
    mainwindow.cpp \
    progressBar.cpp \
    RealCuganProcessor.cpp \
    waifu2xncnnvulkanprocessor.cpp \
    realesrgan_ncnn_vulkan.cpp \
    settings.cpp \
    table.cpp \
    textBrowser.cpp \
    topsupporterslist.cpp \
    video.cpp \
    VideoProcessor.cpp \
    RealcuganJobManager.cpp \
    RealesrganJobManager.cpp \
    FileManager.cpp \
    ProcessRunner.cpp \
    LiquidGlassWidget.cpp \
    GpuManager.cpp \
    UiController.cpp \
    Logger.cpp

HEADERS += \
    Logger.h \
    mainwindow.h \
    topsupporterslist.h \
    LiquidGlassWidget.h \
    utils/ffprobe_helpers.h \
    FileManager.h \
    ProcessRunner.h \
    GpuManager.h \
    UiController.h \
    VideoProcessor.h \
    RealcuganJobManager.h \
    RealesrganJobManager.h \
    RealCuganProcessor.h \
    waifu2xncnnvulkan_settings.h \
    waifu2xncnnvulkanprocessor.h \

FORMS += \
    mainwindow.ui \
    topsupporterslist.ui

TRANSLATIONS += language_English.ts \
               language_Chinese.ts \
               language_TraditionalChinese.ts

TARGET = Beya_Waifu

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    OtherPic.qrc \
    donate.qrc \
    icon.qrc \
    style.qrc \
    shaders.qrc

# compile GLSL shader to QSB
liquidglass_frag.input = SHADER
liquidglass_frag.files = $$PWD/shaders/liquidglass.frag
liquidglass_frag.output = $$PWD/shaders/liquidglass.frag.qsb
liquidglass_frag.commands = \
    mkdir -p $$PWD/shaders && \
    qsb "$$PWD/shaders/liquidglass.frag" -o "$$PWD/shaders/liquidglass.frag.qsb"
liquidglass_frag.CONFIG = no_link
QMAKE_EXTRA_TARGETS += liquidglass_frag
PRE_TARGETDEPS += $$liquidglass_frag.output

RC_ICONS =icon/icon.ico
