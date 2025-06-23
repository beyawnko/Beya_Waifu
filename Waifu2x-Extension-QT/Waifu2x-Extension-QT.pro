# Copyright (C) 2025 beyawnko
# (Full license header as before)

QT       += core gui concurrent multimedia opengl openglwidgets core5compat

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    topsupporterslist.cpp \
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
    progressBar.cpp \
    realcugan_ncnn_vulkan.cpp \
    RealCuganProcessor.cpp \
    realesrgan_ncnn_vulkan.cpp \
    realesrganprocessor.cpp \
    settings.cpp \
    table.cpp \
    textBrowser.cpp \
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
    realesrganprocessor.h \
    realcugan_settings.h \
    realesrgan_settings.h

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
