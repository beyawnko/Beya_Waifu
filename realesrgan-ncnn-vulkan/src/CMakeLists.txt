cmake_policy(SET CMP0091 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# -------------- Support C++17 for using filesystem  ------------------#
# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# expected behaviour
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++lastest")
# -------------- End  ------------------#

# set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

project(realesrgan-ncnn-vulkan)

cmake_minimum_required(VERSION 3.9)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native" CACHE STRING "Release flags" FORCE)

option(USE_SYSTEM_NCNN "build with system libncnn" OFF)
option(USE_SYSTEM_WEBP "build with system libwebp" ON) # Changed to ON
option(USE_STATIC_MOLTENVK "link moltenvk static library" OFF)

find_package(Threads)
find_package(OpenMP)
find_package(Vulkan REQUIRED)

find_program(GLSLANGVALIDATOR_EXECUTABLE NAMES glslangValidator PATHS $ENV{VULKAN_SDK}/bin NO_CMAKE_FIND_ROOT_PATH)
message(STATUS "Found glslangValidator: ${GLSLANGVALIDATOR_EXECUTABLE}")

macro(compile_shader SHADER_SRC)
    set(SHADER_SRC_FULLPATH ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_SRC})

    get_filename_component(SHADER_SRC_NAME_WE ${SHADER_SRC} NAME_WE)
    set(SHADER_SPV_HEX_FILE ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_SRC_NAME_WE}.spv.hex.h)
    add_custom_command(
        OUTPUT ${SHADER_SPV_HEX_FILE}
        COMMAND ${GLSLANGVALIDATOR_EXECUTABLE}
        ARGS -V -s -x -o ${SHADER_SPV_HEX_FILE} ${SHADER_SRC_FULLPATH}
        DEPENDS ${SHADER_SRC_FULLPATH}
        COMMENT "Building SPIR-V module ${SHADER_SRC_NAME_WE}.spv"
        VERBATIM
    )
    set_source_files_properties(${SHADER_SPV_HEX_FILE} PROPERTIES GENERATED TRUE)
    list(APPEND SHADER_SPV_HEX_FILES ${SHADER_SPV_HEX_FILE})

    # fp16 storage
    set(SHADER_fp16s_SRC_NAME_WE "${SHADER_SRC_NAME_WE}_fp16s")

    set(SHADER_fp16s_SPV_HEX_FILE ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_fp16s_SRC_NAME_WE}.spv.hex.h)
    add_custom_command(
        OUTPUT ${SHADER_fp16s_SPV_HEX_FILE}
        COMMAND ${GLSLANGVALIDATOR_EXECUTABLE}
        ARGS -DNCNN_fp16_storage=1 -V -s -x -o ${SHADER_fp16s_SPV_HEX_FILE} ${SHADER_SRC_FULLPATH}
        DEPENDS ${SHADER_SRC_FULLPATH}
        COMMENT "Building SPIR-V module ${SHADER_fp16s_SRC_NAME_WE}.spv"
        VERBATIM
    )
    set_source_files_properties(${SHADER_fp16s_SPV_HEX_FILE} PROPERTIES GENERATED TRUE)
    list(APPEND SHADER_SPV_HEX_FILES ${SHADER_fp16s_SPV_HEX_FILE})

    # int8 storage
    set(SHADER_int8s_SRC_NAME_WE "${SHADER_SRC_NAME_WE}_int8s")

    set(SHADER_int8s_SPV_HEX_FILE ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_int8s_SRC_NAME_WE}.spv.hex.h)
    add_custom_command(
        OUTPUT ${SHADER_int8s_SPV_HEX_FILE}
        COMMAND ${GLSLANGVALIDATOR_EXECUTABLE}
        ARGS -DNCNN_fp16_storage=1 -DNCNN_int8_storage=1 -V -s -x -o ${SHADER_int8s_SPV_HEX_FILE} ${SHADER_SRC_FULLPATH}
        DEPENDS ${SHADER_SRC_FULLPATH}
        COMMENT "Building SPIR-V module ${SHADER_int8s_SRC_NAME_WE}.spv"
        VERBATIM
    )
    set_source_files_properties(${SHADER_int8s_SPV_HEX_FILE} PROPERTIES GENERATED TRUE)
    list(APPEND SHADER_SPV_HEX_FILES ${SHADER_int8s_SPV_HEX_FILE})
endmacro()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(OpenMP_CXX_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# enable global link time optimization
cmake_policy(SET CMP0069 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_supported_output)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(WARNING "IPO is not supported: ${ipo_supported_output}")
endif()

if(USE_SYSTEM_NCNN)
    set(GLSLANG_TARGET_DIR "GLSLANG-NOTFOUND" CACHE PATH "Absolute path to glslangTargets.cmake directory")
    if(NOT GLSLANG_TARGET_DIR AND NOT DEFINED ENV{GLSLANG_TARGET_DIR})
        message(WARNING "GLSLANG_TARGET_DIR must be defined! USE_SYSTEM_NCNN will be turned off.")
        set(USE_SYSTEM_NCNN OFF)
    else()
        message(STATUS "Using glslang install located at ${GLSLANG_TARGET_DIR}")

        find_package(Threads)

        include("${GLSLANG_TARGET_DIR}/OSDependentTargets.cmake")
        include("${GLSLANG_TARGET_DIR}/OGLCompilerTargets.cmake")
        if(EXISTS "${GLSLANG_TARGET_DIR}/HLSLTargets.cmake")
            # hlsl support can be optional
            include("${GLSLANG_TARGET_DIR}/HLSLTargets.cmake")
        endif()
        include("${GLSLANG_TARGET_DIR}/glslangTargets.cmake")
        include("${GLSLANG_TARGET_DIR}/SPIRVTargets.cmake")

        if (NOT TARGET glslang OR NOT TARGET SPIRV)
            message(WARNING "glslang or SPIRV target not found! USE_SYSTEM_NCNN will be turned off.")
            set(USE_SYSTEM_NCNN OFF)
        endif()
    endif()
endif()

# if(USE_SYSTEM_NCNN) # Section removed as USE_SYSTEM_NCNN is now OFF by default for this path
#     find_package(ncnn)
#     if(NOT TARGET ncnn)
#         message(WARNING "ncnn target not found! USE_SYSTEM_NCNN will be turned off.")
#         set(USE_SYSTEM_NCNN OFF)
#     endif()
# endif()

if(NOT USE_SYSTEM_NCNN)
    # build ncnn library
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../ncnn/CMakeLists.txt")
        message(FATAL_ERROR "NCNN root directory not found at ${CMAKE_CURRENT_SOURCE_DIR}/../../ncnn/CMakeLists.txt. This is required when USE_SYSTEM_NCNN is OFF.")
    endif()

    option(NCNN_INSTALL_SDK "" OFF)
    option(NCNN_PIXEL_ROTATE "" OFF)
    option(NCNN_VULKAN "" ON)
    option(NCNN_VULKAN_ONLINE_SPIRV "" ON)
    option(NCNN_BUILD_BENCHMARK "" OFF)
    option(NCNN_BUILD_TESTS "" OFF)
    option(NCNN_BUILD_TOOLS "" OFF)
    option(NCNN_BUILD_EXAMPLES "" OFF)
    # NCNN options are no longer set here as we are using find_package for a pre-built NCNN.
    # The add_subdirectory call is also removed. # This comment is no longer accurate.

    # Set CMAKE_PREFIX_PATH to the directory where NCNN was installed.
    # The NCNN install prefix from previous step: /app/ncnn/install_dir/app/ncnn/build-for-realesrgan/install
    # CMake's find_package will look in <prefix>/lib/cmake/ncnn or <prefix>/ncnnConfig.cmake
    # So, CMAKE_PREFIX_PATH should be /app/ncnn/install_dir/app/ncnn/build-for-realesrgan/install
    # set(NCNN_INSTALL_DIR "/app/ncnn/install_dir/app/ncnn/build-for-realesrgan/install") # Commented out
    # list(APPEND CMAKE_PREFIX_PATH "${NCNN_INSTALL_DIR}") # Commented out
    # message(STATUS "Using CMAKE_PREFIX_PATH for NCNN: ${CMAKE_PREFIX_PATH}") # Commented out

    # find_package(ncnn REQUIRED CONFIG) # Commented out - we will build from submodule
    add_subdirectory(ncnn) # Added to build ncnn from submodule
    message(STATUS "Building NCNN from submodule.") # Adjusted message
    if(TARGET ncnn::ncnn)
        message(STATUS "NCNN target ncnn::ncnn found after building submodule.")
    else()
        message(WARNING "NCNN target ncnn::ncnn NOT found. Will try linking against 'ncnn' if available.")
    endif()
endif()

if(USE_SYSTEM_WEBP)
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
    find_package(WebP)
    if(NOT TARGET webp)
        message(WARNING "webp target not found! USE_SYSTEM_WEBP will be turned off.")
        set(USE_SYSTEM_WEBP OFF)
    endif()
endif()

if(NOT USE_SYSTEM_WEBP)
    # build libwebp library
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libwebp/CMakeLists.txt")
        message(WARNING "libwebp submodule not found at ${CMAKE_CURRENT_SOURCE_DIR}/libwebp/CMakeLists.txt. Continuing, but build may fail if system libwebp is not found or not used.")
    endif()

    option(WEBP_ENABLE_SIMD "" ON)
    option(WEBP_BUILD_ANIM_UTILS "" OFF)
    option(WEBP_BUILD_CWEBP "" OFF)
    option(WEBP_BUILD_DWEBP "" OFF)
    option(WEBP_BUILD_GIF2WEBP "" OFF)
    option(WEBP_BUILD_IMG2WEBP "" OFF)
    option(WEBP_BUILD_VWEBP "" OFF)
    option(WEBP_BUILD_WEBPINFO "" OFF)
    option(WEBP_BUILD_WEBPMUX "" OFF)
    option(WEBP_BUILD_EXTRAS "" OFF)
    option(WEBP_BUILD_WEBP_JS "" OFF)
    option(WEBP_NEAR_LOSSLESS "" OFF)
    option(WEBP_ENABLE_SWAP_16BIT_CSP "" OFF)

    # add_subdirectory(libwebp) # No longer build libwebp as a submodule

    # include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libwebp/src) # Not needed if using system libwebp
endif()

# look for vulkan compute shader and compile
set(SHADER_SPV_HEX_FILES)

compile_shader(realesrgan_preproc.comp)
compile_shader(realesrgan_postproc.comp)
compile_shader(realesrgan_preproc_tta.comp)
compile_shader(realesrgan_postproc_tta.comp)

add_custom_target(generate-spirv DEPENDS ${SHADER_SPV_HEX_FILES})

add_executable(realesrgan-ncnn-vulkan main.cpp realesrgan.cpp)

# Ensure target_include_directories for NCNN paths are removed if they existed from previous attempts
target_include_directories(realesrgan-ncnn-vulkan PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR} # For generated shader headers
)

add_dependencies(realesrgan-ncnn-vulkan generate-spirv)

# Use ncnn::ncnn if available (standard for NCNN CMake config), otherwise fallback to plain 'ncnn'
if(TARGET ncnn::ncnn)
    set(REALESRGAN_LINK_LIBRARIES ncnn::ncnn webp ${Vulkan_LIBRARY})
    message(STATUS "Linking realesrgan-ncnn-vulkan with ncnn::ncnn")
else()
    set(REALESRGAN_LINK_LIBRARIES ncnn webp ${Vulkan_LIBRARY})
    message(STATUS "Linking realesrgan-ncnn-vulkan with ncnn (fallback/legacy name)")
endif()

if(USE_STATIC_MOLTENVK)
    find_library(CoreFoundation NAMES CoreFoundation)
    find_library(Foundation NAMES Foundation)
    find_library(Metal NAMES Metal)
    find_library(QuartzCore NAMES QuartzCore)
    find_library(CoreGraphics NAMES CoreGraphics)
    find_library(Cocoa NAMES Cocoa)
    find_library(IOKit NAMES IOKit)
    find_library(IOSurface NAMES IOSurface)

    list(APPEND REALESRGAN_LINK_LIBRARIES
        ${Metal}
        ${QuartzCore}
        ${CoreGraphics}
        ${Cocoa}
        ${IOKit}
        ${IOSurface}
        ${Foundation}
        ${CoreFoundation}
    )
endif()

if(OpenMP_CXX_FOUND)
    list(APPEND REALESRGAN_LINK_LIBRARIES ${OpenMP_CXX_LIBRARIES})
endif()

target_link_libraries(realesrgan-ncnn-vulkan ${REALESRGAN_LINK_LIBRARIES} -static-libstdc++)

