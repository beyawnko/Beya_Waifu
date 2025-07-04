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

project(realcugan-ncnn-vulkan)

cmake_minimum_required(VERSION 3.9)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE release CACHE STRING "Choose the type of build" FORCE)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native" CACHE STRING "Release flags" FORCE)

option(USE_SYSTEM_NCNN "build with system libncnn" OFF)
option(USE_SYSTEM_WEBP "build with system libwebp" ON)
option(USE_STATIC_MOLTENVK "link moltenvk static library" OFF)

find_package(Threads)
find_package(OpenMP)
find_package(Vulkan REQUIRED)

macro(realcugan_add_shader SHADER_SRC)
    get_filename_component(SHADER_SRC_NAME_WE ${SHADER_SRC} NAME_WE)
    set(SHADER_COMP_HEADER ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_SRC_NAME_WE}.comp.hex.h)

    add_custom_command(
        OUTPUT ${SHADER_COMP_HEADER}
        COMMAND ${CMAKE_COMMAND} -DSHADER_SRC=${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_SRC} -DSHADER_COMP_HEADER=${SHADER_COMP_HEADER} -P "${CMAKE_CURRENT_SOURCE_DIR}/generate_shader_comp_header.cmake"
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_SRC}
        COMMENT "Preprocessing shader source ${SHADER_SRC_NAME_WE}.comp"
        VERBATIM
    )
    set_source_files_properties(${SHADER_COMP_HEADER} PROPERTIES GENERATED TRUE)

    list(APPEND SHADER_SPV_HEX_FILES ${SHADER_COMP_HEADER})
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

# if(USE_SYSTEM_NCNN) # Section removed as USE_SYSTEM_NCNN is OFF by default
#     find_package(ncnn)
#     if(NOT TARGET ncnn)
#         message(WARNING "ncnn target not found! USE_SYSTEM_NCNN will be turned off.")
#         set(USE_SYSTEM_NCNN OFF)
#     endif()
# endif()

if(NOT USE_SYSTEM_NCNN)
    # build ncnn library
    # When USE_SYSTEM_NCNN is OFF (default), we expect to find ncnn at the root.
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../ncnn/CMakeLists.txt")
        message(FATAL_ERROR "NCNN root directory not found at ${CMAKE_CURRENT_SOURCE_DIR}/../../ncnn/CMakeLists.txt. This is required when USE_SYSTEM_NCNN is OFF.")
    endif()

    option(NCNN_INSTALL_SDK "" OFF)
    option(NCNN_PIXEL_ROTATE "" OFF)
    option(NCNN_PIXEL_AFFINE "" OFF)
    option(NCNN_PIXEL_DRAWING "" OFF)
    option(NCNN_VULKAN "" ON)
    option(NCNN_VULKAN_ONLINE_SPIRV "" ON)
    option(NCNN_BUILD_BENCHMARK "" OFF)
    option(NCNN_BUILD_TESTS "" OFF)
    option(NCNN_BUILD_TOOLS "" OFF)
    option(NCNN_BUILD_EXAMPLES "" OFF)
    option(NCNN_DISABLE_RTTI "" ON)
    option(NCNN_DISABLE_EXCEPTION "" ON)
    option(NCNN_INT8 "" OFF)

    option(WITH_LAYER_absval "" OFF)
    option(WITH_LAYER_argmax "" OFF)
    option(WITH_LAYER_batchnorm "" OFF)
    option(WITH_LAYER_bias "" OFF)
    option(WITH_LAYER_bnll "" OFF)
    option(WITH_LAYER_concat "" OFF)
    option(WITH_LAYER_convolution "" ON)
    option(WITH_LAYER_crop "" ON)
    option(WITH_LAYER_deconvolution "" ON)
    option(WITH_LAYER_dropout "" OFF)
    option(WITH_LAYER_eltwise "" ON)
    option(WITH_LAYER_elu "" OFF)
    option(WITH_LAYER_embed "" OFF)
    option(WITH_LAYER_exp "" OFF)
    option(WITH_LAYER_flatten "" ON)
    option(WITH_LAYER_innerproduct "" ON)
    option(WITH_LAYER_input "" ON)
    option(WITH_LAYER_log "" OFF)
    option(WITH_LAYER_lrn "" OFF)
    option(WITH_LAYER_memorydata "" OFF)
    option(WITH_LAYER_mvn "" OFF)
    option(WITH_LAYER_pooling "" ON)
    option(WITH_LAYER_power "" OFF)
    option(WITH_LAYER_prelu "" OFF)
    option(WITH_LAYER_proposal "" OFF)
    option(WITH_LAYER_reduction "" ON)
    option(WITH_LAYER_relu "" ON)
    option(WITH_LAYER_reshape "" OFF)
    option(WITH_LAYER_roipooling "" OFF)
    option(WITH_LAYER_scale "" ON)
    option(WITH_LAYER_sigmoid "" ON)
    option(WITH_LAYER_slice "" OFF)
    option(WITH_LAYER_softmax "" OFF)
    option(WITH_LAYER_split "" ON)
    option(WITH_LAYER_spp "" OFF)
    option(WITH_LAYER_tanh "" OFF)
    option(WITH_LAYER_threshold "" OFF)
    option(WITH_LAYER_tile "" OFF)
    option(WITH_LAYER_rnn "" OFF)
    option(WITH_LAYER_lstm "" OFF)
    option(WITH_LAYER_binaryop "" ON)
    option(WITH_LAYER_unaryop "" OFF)
    option(WITH_LAYER_convolutiondepthwise "" OFF)
    option(WITH_LAYER_padding "" ON)
    option(WITH_LAYER_squeeze "" OFF)
    option(WITH_LAYER_expanddims "" OFF)
    option(WITH_LAYER_normalize "" OFF)
    option(WITH_LAYER_permute "" OFF)
    option(WITH_LAYER_priorbox "" OFF)
    option(WITH_LAYER_detectionoutput "" OFF)
    option(WITH_LAYER_interp "" ON)
    option(WITH_LAYER_deconvolutiondepthwise "" OFF)
    option(WITH_LAYER_shufflechannel "" OFF)
    option(WITH_LAYER_instancenorm "" OFF)
    option(WITH_LAYER_clip "" OFF)
    option(WITH_LAYER_reorg "" OFF)
    option(WITH_LAYER_yolodetectionoutput "" OFF)
    option(WITH_LAYER_quantize "" OFF)
    option(WITH_LAYER_dequantize "" OFF)
    option(WITH_LAYER_yolov3detectionoutput "" OFF)
    option(WITH_LAYER_psroipooling "" OFF)
    option(WITH_LAYER_roialign "" OFF)
    option(WITH_LAYER_packing "" ON)
    option(WITH_LAYER_requantize "" OFF)
    option(WITH_LAYER_cast "" ON)
    option(WITH_LAYER_hardsigmoid "" OFF)
    option(WITH_LAYER_selu "" OFF)
    option(WITH_LAYER_hardswish "" OFF)
    option(WITH_LAYER_noop "" OFF)
    option(WITH_LAYER_pixelshuffle "" ON)
    option(WITH_LAYER_deepcopy "" OFF)
    option(WITH_LAYER_mish "" OFF)
    option(WITH_LAYER_statisticspooling "" OFF)
    option(WITH_LAYER_swish "" OFF)
    option(WITH_LAYER_gemm "" OFF)
    option(WITH_LAYER_groupnorm "" OFF)
    option(WITH_LAYER_layernorm "" OFF)
    option(WITH_LAYER_softplus "" OFF)
    option(WITH_LAYER_gru "" OFF)
    option(WITH_LAYER_multiheadattention "" OFF)
    option(WITH_LAYER_gelu "" OFF)
    option(WITH_LAYER_convolution1d "" OFF)
    option(WITH_LAYER_pooling1d "" OFF)
    option(WITH_LAYER_convolutiondepthwise1d "" OFF)
    option(WITH_LAYER_convolution3d "" OFF)
    option(WITH_LAYER_convolutiondepthwise3d "" OFF)
    option(WITH_LAYER_pooling3d "" OFF)
    option(WITH_LAYER_matmul "" OFF)
    option(WITH_LAYER_deconvolution1d "" OFF)
    option(WITH_LAYER_deconvolutiondepthwise1d "" OFF)
    option(WITH_LAYER_deconvolution3d "" OFF)
    option(WITH_LAYER_deconvolutiondepthwise3d "" OFF)
    option(WITH_LAYER_einsum "" OFF)
    option(WITH_LAYER_deformableconv2d "" OFF)

    add_subdirectory(../../ncnn ${CMAKE_BINARY_DIR}/ncnn_build_realcugan)
endif()

if(USE_SYSTEM_WEBP)
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
    find_package(PkgConfig REQUIRED) # Ensure PkgConfig is found
    find_package(WebP)
    message(STATUS "WebP_FOUND: ${WebP_FOUND}")
    if(TARGET webp)
        message(STATUS "Target webp IS defined.")
    else()
        message(STATUS "Target webp IS NOT defined.")
    endif()
    if(NOT TARGET webp)
        message(WARNING "webp target not found! USE_SYSTEM_WEBP will be turned off.")
        set(USE_SYSTEM_WEBP OFF)
    endif()
endif()

if(NOT USE_SYSTEM_WEBP)
    # build libwebp library
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libwebp/CMakeLists.txt")
        message(FATAL_ERROR "The submodules were not downloaded! Please update submodules with \"git submodule update --init --recursive\" and try again.")
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

    add_subdirectory(libwebp)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libwebp/src)
endif()

realcugan_add_shader(realcugan_preproc.comp)
realcugan_add_shader(realcugan_postproc.comp)
realcugan_add_shader(realcugan_4x_postproc.comp)
realcugan_add_shader(realcugan_preproc_tta.comp)
realcugan_add_shader(realcugan_postproc_tta.comp)
realcugan_add_shader(realcugan_4x_postproc_tta.comp)

add_custom_target(generate-spirv DEPENDS ${SHADER_SPV_HEX_FILES})

add_executable(realcugan-ncnn-vulkan main.cpp realcugan.cpp)

set_property(TARGET realcugan-ncnn-vulkan PROPERTY CXX_STANDARD 17)

add_dependencies(realcugan-ncnn-vulkan generate-spirv)

set(REALCUGAN_LINK_LIBRARIES ncnn webp ${Vulkan_LIBRARY})

if(USE_STATIC_MOLTENVK)
    find_library(CoreFoundation NAMES CoreFoundation)
    find_library(Foundation NAMES Foundation)
    find_library(Metal NAMES Metal)
    find_library(QuartzCore NAMES QuartzCore)
    find_library(CoreGraphics NAMES CoreGraphics)
    find_library(Cocoa NAMES Cocoa)
    find_library(IOKit NAMES IOKit)
    find_library(IOSurface NAMES IOSurface)

    list(APPEND REALCUGAN_LINK_LIBRARIES
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
    list(APPEND REALCUGAN_LINK_LIBRARIES ${OpenMP_CXX_LIBRARIES})
endif()

target_link_libraries(realcugan-ncnn-vulkan ${REALCUGAN_LINK_LIBRARIES} -static-libstdc++)
