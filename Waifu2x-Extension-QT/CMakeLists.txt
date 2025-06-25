cmake_minimum_required(VERSION 3.16)
project(Beya_Waifu LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find base Qt6 components required by the project
find_package(Qt6 COMPONENTS Core Gui Concurrent Multimedia OpenGL OpenGLWidgets Widgets ShaderTools LinguistTools Quick REQUIRED)

# CMAKE_PREFIX_PATH will be set on the command line if using a non-system Qt.
# The line below is commented out as per previous decisions.
# set(CMAKE_PREFIX_PATH ${Qt6_PREFIX_PATH})

# Determine if RHI should be enabled (Qt >= 6.6.0)
if (Qt6_VERSION VERSION_GREATER_EQUAL "6.6.0")
  set(ENABLE_RHI TRUE)
  message(STATUS "Qt6 >= 6.6 detected: enabling RHI integration via Quick3D (and linking GuiPrivate for headers)")
  # Find Quick3D component if Qt version is sufficient, as it provides RHI
  # GuiPrivate is not found this way, but the Qt6::GuiPrivate target should be available if Gui is found.
  find_package(Qt6 COMPONENTS Quick3D REQUIRED)
else()
  set(ENABLE_RHI FALSE)
  message(STATUS "Qt6 < 6.6 detected: building without RHI support (RHI available publicly since Qt 6.6). Will attempt to use private API if available and explicitly enabled.")
  # Note: The previous attempt to use Qt6::GuiPrivate for < 6.6 failed due to missing headers in system packages.
  # For < 6.6, RHI features will effectively be disabled if RhiLiquidGlassItem.cpp is conditionally compiled out.
endif()

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
    # RhiLiquidGlassItem.cpp will be added conditionally
)

if (ENABLE_RHI)
  list(APPEND PROJECT_SOURCES RhiLiquidGlassItem.cpp)
  # Add any other RHI-specific sources here if needed
endif()

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

# Define the executable target first
qt_add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${PROJECT_FORMS}
    ${PROJECT_RESOURCES}
    # PROJECT_SHADERS # Will be handled by qt_add_shaders linking to this target
)

# Shader handling with qt_add_shaders
# This should come AFTER qt_add_executable so that ${PROJECT_NAME} target exists
qt_add_shaders(${PROJECT_NAME} ShadersGenerated
    FILES ${PROJECT_SHADERS}
    PREFIX "/com.waifu2x.effects/shaders"
)
# The qt_add_shaders function should handle adding the compiled shaders to the target.

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
    # Conditionally link Quick3D (which includes RHI) and GuiPrivate (for rhi headers) using generator expression
    "$<$<BOOL:${ENABLE_RHI}>:Qt6::Quick3D>"
    "$<$<BOOL:${ENABLE_RHI}>:Qt6::GuiPrivate>"
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
