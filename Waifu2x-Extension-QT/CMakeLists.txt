cmake_minimum_required(VERSION 3.16)
project(Beya_Waifu LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Concurrent Multimedia OpenGL OpenGLWidgets Widgets ShaderTools LinguistTools Quick REQUIRED)

set(CMAKE_PREFIX_PATH ${Qt6_PREFIX_PATH})

# Define sources, headers, and UI forms
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    topsupporterslist.cpp
    AnimatedPNG.cpp
    CompatibilityTest.cpp
    Current_File_Progress.cpp
    CustomResolution.cpp
    Donate.cpp
    Finish_Action.cpp
    Frame_Interpolation.cpp
    Right-click_Menu.cpp
    SystemTrayIcon.cpp
    Web_Activities.cpp
    checkupdate.cpp
    files.cpp
    gif.cpp
    image.cpp
    progressBar.cpp
    RealCuganProcessor.cpp
    realesrganprocessor.cpp
    settings.cpp
    table.cpp
    textBrowser.cpp
    video.cpp
    VideoProcessor.cpp
    RealcuganJobManager.cpp
    RealesrganJobManager.cpp
    FileManager.cpp
    ProcessRunner.cpp
    LiquidGlassWidget.cpp
    GpuManager.cpp
    UiController.cpp
    Logger.cpp
    anime4kprocessor.cpp
    # RhiLiquidGlassItem.cpp # Temporarily commented out due to RHI issues
)

set(PROJECT_HEADERS
    Logger.h
    mainwindow.h
    topsupporterslist.h
    LiquidGlassWidget.h
    LiquidGlassParams.h
    utils/ffprobe_helpers.h
    FileManager.h
    ProcessRunner.h
    GpuManager.h
    UiController.h
    VideoProcessor.h
    RealcuganJobManager.h
    RealesrganJobManager.h
    RealCuganProcessor.h
    realesrganprocessor.h
    realcugan_settings.h
    realesrgan_settings.h
    anime4kprocessor.h
    anime4k_settings.h
)

set(PROJECT_FORMS
    mainwindow.ui
    topsupporterslist.ui
)

set(PROJECT_RESOURCES
    OtherPic.qrc
    donate.qrc
    icon.qrc
    style.qrc
    shaders.qrc
)

set(PROJECT_SHADERS
    shaders/liquidglass.frag
)

# Translation handling
find_program(LRELEASE_EXECUTABLE NAMES lrelease-qt6 lrelease
    PATHS /usr/lib/qt6/bin /usr/bin NO_DEFAULT_PATH DOC "Qt lrelease executable")
# Try also default paths if not found in specific Qt path
if(NOT LRELEASE_EXECUTABLE)
    find_program(LRELEASE_EXECUTABLE NAMES lrelease-qt6 lrelease DOC "Qt lrelease executable")
endif()

if(NOT LRELEASE_EXECUTABLE)
    message(FATAL_ERROR "lrelease (or lrelease-qt6) not found. Please install Qt Linguist tools.")
endif()

set(PROJECT_TRANSLATIONS_TS
    language_English.ts
#    language_Chinese.ts
#    language_TraditionalChinese.ts
)

# The simplified explicit add_custom_command calls, ensuring correct argument order
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/language_English.qm"
    COMMAND ${LRELEASE_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/language_English.ts" -qm "${CMAKE_CURRENT_BINARY_DIR}/language_English.qm"
    MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/language_English.ts"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/language_English.ts"
    COMMENT "Compiling language_English.ts"
    VERBATIM
)
# add_custom_command(
#    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/language_Chinese.qm"
#    COMMAND ${LRELEASE_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/language_Chinese.ts" -qm "${CMAKE_CURRENT_BINARY_DIR}/language_Chinese.qm"
#    MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/language_Chinese.ts"
#    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/language_Chinese.ts"
#    COMMENT "Compiling language_Chinese.ts"
#    VERBATIM
# )
# add_custom_command(
#    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/language_TraditionalChinese.qm"
#    COMMAND ${LRELEASE_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/language_TraditionalChinese.ts" -qm "${CMAKE_CURRENT_BINARY_DIR}/language_TraditionalChinese.qm"
#    MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/language_TraditionalChinese.ts"
#    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/language_TraditionalChinese.ts"
#    COMMENT "Compiling language_TraditionalChinese.ts"
#    VERBATIM
# )

set(TRANSLATIONS_QRC_FILE_CONTENT_STATIC
"<!DOCTYPE RCC><RCC version=\"1.0\">
<qresource prefix=\"/translations\">
    <file>language_English.qm</file>
</qresource>
</RCC>"
)
set(TRANSLATIONS_QRC_FILE_STATIC "${CMAKE_CURRENT_BINARY_DIR}/translations_static.qrc")
file(WRITE ${TRANSLATIONS_QRC_FILE_STATIC} "${TRANSLATIONS_QRC_FILE_CONTENT_STATIC}")

# Add the generated QRC file to resources.
list(APPEND PROJECT_RESOURCES ${TRANSLATIONS_QRC_FILE_STATIC})
# End Translation handling

# Shader handling with qt_add_shaders
qt_add_shaders(${PROJECT_NAME} ShadersGenerated
    FILES ${PROJECT_SHADERS}
    PREFIX "/com.waifu2x.effects/shaders"
)
# The qt_add_shaders function should handle adding the compiled shaders to the target.
# So, PROJECT_SHADERS should not be listed in qt_add_executable directly.

qt_add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${PROJECT_FORMS}
    ${PROJECT_RESOURCES}
    # PROJECT_SHADERS # Removed from here, handled by qt_add_shaders
)

# Also list TS files as sources for the target to help with dependency tracking (REMOVED - caused issues)
# foreach(TS_FILE ${PROJECT_TRANSLATIONS_TS})
#    target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/${TS_FILE}")
# endforeach()

# qt_add_translations(${PROJECT_NAME} TS_FILES ${PROJECT_TRANSLATIONS}) # Qt5
# qt_add_l10n(${PROJECT_NAME} TS_FILES ${PROJECT_TRANSLATIONS}) # Qt6 - Command not found, will handle manually

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Concurrent
    Qt6::Multimedia
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Widgets
    Qt6::ShaderTools
    Qt6::Quick
)

# Add defines
target_compile_definitions(${PROJECT_NAME} PRIVATE QT_DEPRECATED_WARNINGS)

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Windows specific icon
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    target_sources(${PROJECT_NAME} PRIVATE icon/icon.ico)
endif()
