#!/bin/bash
# Copyright (C) 2025  beyawnko
set -e

# Ensure submodules are present if git is available
if command -v git >/dev/null 2>&1; then
    git submodule update --init --recursive
else
    echo "git not found; skipping submodule update"
fi

# Install glslangValidator if missing
if ! command -v glslangValidator >/dev/null 2>&1; then
    if command -v apt-get >/dev/null 2>&1; then
        sudo apt-get update && sudo apt-get install -y glslang-tools
    else
        echo "glslangValidator not found and no package manager available"
    fi
fi

# Install required Python packages for tests if pip is available
if command -v pip >/dev/null 2>&1; then
    pip install --user PySide6 Pillow
else
    echo "pip not found; skipping Python package installation"
fi

# Determine available make tool
MAKE=make
command -v $MAKE >/dev/null 2>&1 || MAKE=mingw32-make
command -v $MAKE >/dev/null 2>&1 || { echo "make tool not found"; exit 1; }

# If the caller provided a Qt bin directory, prepend it to PATH.
QT_BIN_PATH="${QT_BIN_PATH:-${QtBinPath:-}}"
if [ -n "$QT_BIN_PATH" ]; then
    export PATH="$QT_BIN_PATH:$PATH"
fi

# Validate Qt tools are available after updating PATH.
if ! command -v qmake >/dev/null 2>&1; then
    echo "qmake not found in PATH. Ensure QT_BIN_PATH points to your Qt bin directory."
    exit 1
fi

if ! command -v qsb >/dev/null 2>&1; then
    echo "Warning: qsb tool not found in PATH. Shader compilation may fail."
fi

RESRGAN_SRC_DIR="realesrgan-ncnn-vulkan"
RCUGAN_SRC_DIR="realcugan-ncnn-vulkan"
# TARGET_APP_DIR specifies the directory where the main application (Beya_Waifu)
# and the upscaler executables will be placed. This is typically the Waifu2x-Extension-QT folder itself.
TARGET_APP_DIR="Waifu2x-Extension-QT" # This is where Beya_Waifu and upscalers go
# Ensure TARGET_APP_DIR exists. qmake might also create it later, but good practice to ensure.
mkdir -p "$TARGET_APP_DIR"

# OS detection logic: Determines the operating system to tailor build and file handling.
# $(uname -s) gets the kernel name, tr converts to lowercase for case-insensitive matching.
case $(uname -s | tr '[:upper:]' '[:lower:]') in
    msys*|cygwin*|mingw*)
        # Windows specific environment (MSYS, Cygwin, MinGW)
        echo "Windows environment detected. Handling upscaler binaries..."

        # Attempt to add Vulkan SDK to PATH for glslc
        VULKAN_SDK_BIN_PATH=""
        for d in /c/VulkanSDK/*; do
            if [ -d "$d/Bin" ]; then
                VULKAN_SDK_BIN_PATH="$d/Bin"
                break
            fi
        done
        if [ -d "$VULKAN_SDK_BIN_PATH" ] && [ -f "$VULKAN_SDK_BIN_PATH/glslc.exe" ]; then
            echo "Found glslc in Vulkan SDK at $VULKAN_SDK_BIN_PATH. Adding to PATH."
            export PATH="$VULKAN_SDK_BIN_PATH:$PATH"
        else
            echo "Vulkan SDK glslc.exe not found at $VULKAN_SDK_BIN_PATH. Assuming glslc is in existing PATH or will be found by CMake."
            # As an alternative, try to find glslc using 'which' or 'command -v' if the above path fails
            GLSLC_PATH=$(command -v glslc.exe || true)
            if [ -n "$GLSLC_PATH" ]; then
                echo "Found glslc.exe in PATH at $GLSLC_PATH."
                GLSLC_DIR=$(dirname "$GLSLC_PATH")
                # Check if already in PATH to avoid duplicates, though usually not harmful
                if [[ ":$PATH:" != *":$GLSLC_DIR:"* ]]; then
                    echo "Adding $GLSLC_DIR to PATH."
                    export PATH="$GLSLC_DIR:$PATH"
                fi
            else
                echo "glslc.exe still not found in common locations or PATH. Build might fail if it's required."
            fi
        fi

        RESRGAN_PREBUILT_DIR="realesrgan-ncnn-vulkan/windows"
        RCUGAN_PREBUILT_DIR="realcugan-ncnn-vulkan/windows"
        TARGET_LAUNCHER_DIR="Waifu2x-Extension-QT-Launcher" # Launcher directory, kept for Windows builds
        mkdir -p "$TARGET_LAUNCHER_DIR"

        # --- Real-ESRGAN (Windows) ---
        # For Windows, the script prefers using prebuilt upscaler binaries if available.
        # If prebuilt binaries are not found, it falls back to building from source using CMake.
        echo "Handling Real-ESRGAN for Windows..."
        if [ -f "$RESRGAN_PREBUILT_DIR/realesrgan-ncnn-vulkan.exe" ]; then
            echo "Found prebuilt Real-ESRGAN executable. Copying..."
            cp "$RESRGAN_PREBUILT_DIR/realesrgan-ncnn-vulkan.exe" "$TARGET_APP_DIR/realesrgan.exe"
            cp "$RESRGAN_PREBUILT_DIR/realesrgan-ncnn-vulkan.exe" "$TARGET_LAUNCHER_DIR/realesrgan.exe"
        elif [ -f "$RESRGAN_PREBUILT_DIR/realesrgan.exe" ]; then
            echo "Found prebuilt Real-ESRGAN executable. Copying..."
            cp "$RESRGAN_PREBUILT_DIR/realesrgan.exe" "$TARGET_APP_DIR/realesrgan.exe"
            cp "$RESRGAN_PREBUILT_DIR/realesrgan.exe" "$TARGET_LAUNCHER_DIR/realesrgan.exe"
            # Copy known DLLs, be specific if possible, otherwise copy all from prebuilt dir
            find "$RESRGAN_PREBUILT_DIR" -maxdepth 1 -name "*.dll" -exec cp {} "$TARGET_APP_DIR/" \;
            find "$RESRGAN_PREBUILT_DIR" -maxdepth 1 -name "*.dll" -exec cp {} "$TARGET_LAUNCHER_DIR/" \;

            # Models are copied from the prebuilt directory if available.
            echo "Copying Real-ESRGAN models from Windows prebuilt..."
            mkdir -p "$TARGET_APP_DIR/models"
            if [ -d "$RESRGAN_PREBUILT_DIR/models" ]; then
                cp -R "$RESRGAN_PREBUILT_DIR/models/"* "$TARGET_APP_DIR/models/"
            elif [ -d "$RESRGAN_SRC_DIR/models" ]; then
                echo "Warning: Prebuilt Real-ESRGAN models directory not found at $RESRGAN_PREBUILT_DIR/models. Trying source models."
                cp -R "$RESRGAN_SRC_DIR/models/"* "$TARGET_APP_DIR/models/"
            else
                echo "Warning: No Real-ESRGAN models found. Skipping model copy."
            fi
        else
            # Fallback: Build Real-ESRGAN from source if prebuilt not found.
            echo "Prebuilt Real-ESRGAN not found. Attempting CMake build..."
            echo "Ensuring nested submodules for $RESRGAN_SRC_DIR are initialized..."
            if [ -d "$RESRGAN_SRC_DIR" ]; then
                echo "Updating submodules in $RESRGAN_SRC_DIR..."
                (cd "$RESRGAN_SRC_DIR" && git submodule update --init --recursive)
            else
                echo "Warning: Directory $RESRGAN_SRC_DIR not found. Cannot initialize its submodules."
            fi
            pushd "$RESRGAN_SRC_DIR" >/dev/null
            mkdir -p build_windows && cd build_windows
            cmake -G "MSYS Makefiles" .. || { echo "Real-ESRGAN CMake failed."; popd >/dev/null; exit 1; }
            $MAKE -j$(nproc) || { echo "Real-ESRGAN make failed."; popd >/dev/null; exit 1; }

            # Copy built executable (CMake should place it in build_windows/src or build_windows)
            cp src/realesrgan-ncnn-vulkan.exe "$TARGET_APP_DIR/realesrgan.exe" 2>/dev/null || cp realesrgan-ncnn-vulkan.exe "$TARGET_APP_DIR/realesrgan.exe" 2>/dev/null || { echo "Failed to find/copy built realesrgan.exe"; popd >/dev/null; exit 1; }
            cp src/realesrgan-ncnn-vulkan.exe "$TARGET_LAUNCHER_DIR/realesrgan.exe" 2>/dev/null || cp realesrgan-ncnn-vulkan.exe "$TARGET_LAUNCHER_DIR/realesrgan.exe" 2>/dev/null # Also to launcher

            # Copy any DLLs found in the build directory (e.g., if statically linked CRT is not used)
            find . -maxdepth 1 -name "*.dll" -exec cp {} "$TARGET_APP_DIR/" \;
            find . -maxdepth 1 -name "*.dll" -exec cp {} "$TARGET_LAUNCHER_DIR/" \;
            popd >/dev/null

            # Models are copied from the source submodule directory after a source build.
            echo "Copying Real-ESRGAN models from source for Windows build..."
            mkdir -p "$TARGET_APP_DIR/models"
            cp -R "$RESRGAN_SRC_DIR/models/"* "$TARGET_APP_DIR/models/"
        fi

        # --- Real-CUGAN (Windows) ---
        # Similar logic for Real-CUGAN: Prefer prebuilt, fallback to source build.
        echo "Handling Real-CUGAN for Windows..."
        RCUGAN_MODEL_SUBDIRS="models-se models-pro models-nose"
        if [ -f "$RCUGAN_PREBUILT_DIR/realcugan-ncnn-vulkan.exe" ]; then
            echo "Found prebuilt Real-CUGAN executable. Copying..."
            cp "$RCUGAN_PREBUILT_DIR/realcugan-ncnn-vulkan.exe" "$TARGET_APP_DIR/realcugan.exe"
            cp "$RCUGAN_PREBUILT_DIR/realcugan-ncnn-vulkan.exe" "$TARGET_LAUNCHER_DIR/realcugan.exe"
        elif [ -f "$RCUGAN_PREBUILT_DIR/realcugan.exe" ]; then
            echo "Found prebuilt Real-CUGAN executable. Copying..."
            cp "$RCUGAN_PREBUILT_DIR/realcugan.exe" "$TARGET_APP_DIR/realcugan.exe"
            cp "$RCUGAN_PREBUILT_DIR/realcugan.exe" "$TARGET_LAUNCHER_DIR/realcugan.exe"
        find "$RCUGAN_PREBUILT_DIR" -maxdepth 1 -name "*.dll" -exec cp {} "$TARGET_APP_DIR/" \;
        find "$RCUGAN_PREBUILT_DIR" -maxdepth 1 -name "*.dll" -exec cp {} "$TARGET_LAUNCHER_DIR/" \;

            # Copy Real-CUGAN models from prebuilt directory.
            echo "Copying Real-CUGAN models from Windows prebuilt..."
            for model_subdir in $RCUGAN_MODEL_SUBDIRS; do
                if [ -d "$RCUGAN_PREBUILT_DIR/$model_subdir" ]; then
                    mkdir -p "$TARGET_APP_DIR/$model_subdir"
                    cp -R "$RCUGAN_PREBUILT_DIR/$model_subdir/"* "$TARGET_APP_DIR/$model_subdir/"
                else
                    echo "Warning: Prebuilt Real-CUGAN model directory $RCUGAN_PREBUILT_DIR/$model_subdir not found. Trying source."
                    mkdir -p "$TARGET_APP_DIR/$model_subdir"
                    if [ -d "$RCUGAN_SRC_DIR/models/$model_subdir" ]; then
                        cp -R "$RCUGAN_SRC_DIR/models/$model_subdir/"* "$TARGET_APP_DIR/$model_subdir/"
                    else
                        echo "Warning: No source models for Real-CUGAN subdir $model_subdir found."
                    fi
                fi
            done
        else
            # Fallback: Build Real-CUGAN from source.
            echo "Prebuilt Real-CUGAN not found. Attempting CMake build..."
            echo "Ensuring nested submodules for $RCUGAN_SRC_DIR are initialized..."
            if [ -d "$RCUGAN_SRC_DIR" ]; then
                echo "Updating submodules in $RCUGAN_SRC_DIR..."
                (cd "$RCUGAN_SRC_DIR" && git submodule update --init --recursive)
            else
                echo "Warning: Directory $RCUGAN_SRC_DIR not found. Cannot initialize its submodules."
            fi
            pushd "$RCUGAN_SRC_DIR" >/dev/null
            mkdir -p build_windows && cd build_windows
            cmake -G "MSYS Makefiles" .. || { echo "Real-CUGAN CMake failed."; popd >/dev/null; exit 1; }
            $MAKE -j$(nproc) || { echo "Real-CUGAN make failed."; popd >/dev/null; exit 1; }

            cp src/realcugan-ncnn-vulkan.exe "$TARGET_APP_DIR/realcugan.exe" 2>/dev/null || cp realcugan-ncnn-vulkan.exe "$TARGET_APP_DIR/realcugan.exe" 2>/dev/null || { echo "Failed to find/copy built realcugan.exe"; popd >/dev/null; exit 1; }
            cp src/realcugan-ncnn-vulkan.exe "$TARGET_LAUNCHER_DIR/realcugan.exe" 2>/dev/null || cp realcugan-ncnn-vulkan.exe "$TARGET_LAUNCHER_DIR/realcugan.exe" 2>/dev/null

            find . -maxdepth 1 -name "*.dll" -exec cp {} "$TARGET_APP_DIR/" \;
            find . -maxdepth 1 -name "*.dll" -exec cp {} "$TARGET_LAUNCHER_DIR/" \;
            popd >/dev/null

            # Copy Real-CUGAN models from source submodule after building.
            echo "Copying Real-CUGAN models from source for Windows build..."
            for model_subdir in $RCUGAN_MODEL_SUBDIRS; do
                mkdir -p "$TARGET_APP_DIR/$model_subdir"
                if [ -d "$RCUGAN_SRC_DIR/models/$model_subdir" ]; then
                    cp -R "$RCUGAN_SRC_DIR/models/$model_subdir/"* "$TARGET_APP_DIR/$model_subdir/"
                else
                    echo "Warning: No source models for Real-CUGAN subdir $model_subdir found during build."
                fi
            done
        fi
        ;;

    linux*)
        # Linux specific environment
        echo "Linux environment detected. Building upscalers from source..."

        # --- Real-ESRGAN (Linux) ---
        # On Linux, upscalers are built from their source submodules using CMake.
        echo "Building Real-ESRGAN for Linux..."
        pushd "$RESRGAN_SRC_DIR" >/dev/null
        echo "Updating submodules in $RESRGAN_SRC_DIR..."
        git submodule update --init --recursive || { echo "Submodule update failed for $RESRGAN_SRC_DIR"; popd >/dev/null; exit 1; }
        rm -rf build_linux # Clean previous build
        mkdir -p build_linux && cd build_linux
        cmake ../src || { echo "Real-ESRGAN CMake failed."; popd >/dev/null; exit 1; }
        $MAKE -j$(nproc) || { echo "Real-ESRGAN make failed."; popd >/dev/null; exit 1; }
        # CMake on Linux usually places executables in build_linux/src or build_linux directly
        cp src/realesrgan-ncnn-vulkan "$TARGET_APP_DIR/" 2>/dev/null || cp realesrgan-ncnn-vulkan "$TARGET_APP_DIR/" 2>/dev/null || { echo "Failed to find/copy built realesrgan-ncnn-vulkan"; popd >/dev/null; exit 1; }
        popd >/dev/null

        # Models for Linux are copied from the source submodule.
        echo "Copying Real-ESRGAN models for Linux..."
        mkdir -p "$TARGET_APP_DIR/models"
        cp -R "$RESRGAN_SRC_DIR/models/"* "$TARGET_APP_DIR/models/"

        # --- Real-CUGAN (Linux) ---
        # Real-CUGAN is also built from source on Linux.
        echo "Building Real-CUGAN for Linux..."
        RCUGAN_MODEL_SUBDIRS="models-se models-pro models-nose"
        pushd "$RCUGAN_SRC_DIR" >/dev/null
        echo "Updating submodules in $RCUGAN_SRC_DIR..."
        git submodule update --init --recursive || { echo "Submodule update failed for $RCUGAN_SRC_DIR"; popd >/dev/null; exit 1; }
        rm -rf build_linux # Clean previous build
        mkdir -p build_linux && cd build_linux
        cmake ../src || { echo "Real-CUGAN CMake failed."; popd >/dev/null; exit 1; }
        $MAKE -j$(nproc) || { echo "Real-CUGAN make failed."; popd >/dev/null; exit 1; }
        cp src/realcugan-ncnn-vulkan "$TARGET_APP_DIR/" 2>/dev/null || cp realcugan-ncnn-vulkan "$TARGET_APP_DIR/" 2>/dev/null || { echo "Failed to find/copy built realcugan-ncnn-vulkan"; popd >/dev/null; exit 1; }
        popd >/dev/null

        # Models for Real-CUGAN on Linux are copied from the source submodule.
        echo "Copying Real-CUGAN models for Linux..."
        for model_subdir in $RCUGAN_MODEL_SUBDIRS; do
            mkdir -p "$TARGET_APP_DIR/$model_subdir"
            cp -R "$RCUGAN_SRC_DIR/models/$model_subdir/"* "$TARGET_APP_DIR/$model_subdir/"
        done
        ;;

    *)
        # Fallback for unsupported operating systems.
        echo "Unsupported OS: $(uname -s). Skipping upscaler build."
        # Optionally exit 1 if upscalers are mandatory for the application to function.
        ;;
esac

# --- Qt Application Builds ---
# After preparing the upscaler engines, the main Qt application and the launcher are built.

# Build the main Waifu2x-Extension-QT application.
echo "Building Waifu2x-Extension-QT..."
pushd "$TARGET_APP_DIR" >/dev/null # Change to the application directory where the .pro file is located.
qmake Waifu2x-Extension-QT.pro || { echo "qmake for Waifu2x-Extension-QT failed."; popd >/dev/null; exit 1; }
$MAKE liquidglass_frag || { echo "make liquidglass_frag failed."; popd >/dev/null; exit 1; }
# The shader step should produce shaders/liquidglass.frag.qsb via qsb.
# General make command for the application, using multiple cores.
$MAKE -j$(nproc) || { echo "make for Waifu2x-Extension-QT failed."; popd >/dev/null; exit 1; }
    echo "Deploying Qt DLLs for Waifu2x-Extension-QT..."
    if command -v windeployqt.exe >/dev/null 2>&1; then
        windeployqt.exe "release/Beya_Waifu.exe" --no-opengl-sw --no-translations --no-system-d3d-compiler --dir "release"
        echo "Copying MinGW runtime DLLs for Waifu2x-Extension-QT..."
        # Attempt to locate MinGW bin directory, common path is /mingw64/bin
        MINGW_BIN_DIR="/mingw64/bin" # This could also be made more robust, e.g. by deriving from 'which gcc'
        if [ -d "$MINGW_BIN_DIR" ]; then
            cp "$MINGW_BIN_DIR/libgcc_s_seh-1.dll" "release/" 2>/dev/null || echo "Note: libgcc_s_seh-1.dll not found in $MINGW_BIN_DIR"
            cp "$MINGW_BIN_DIR/libstdc++-6.dll" "release/" 2>/dev/null || echo "Note: libstdc++-6.dll not found in $MINGW_BIN_DIR"
            cp "$MINGW_BIN_DIR/libwinpthread-1.dll" "release/" 2>/dev/null || echo "Note: libwinpthread-1.dll not found in $MINGW_BIN_DIR"
        else
            echo "Warning: MinGW bin directory not found at $MINGW_BIN_DIR. Runtime DLLs may be missing."
        fi
    else
        echo "Warning: windeployqt.exe not found in PATH. Skipping Qt deployment for Waifu2x-Extension-QT."
    fi
popd >/dev/null

# Build the Waifu2x-Extension-QT-Launcher application.
echo "Building Waifu2x-Extension-QT-Launcher..."
pushd Waifu2x-Extension-QT-Launcher >/dev/null # Change to the launcher's directory.
qmake Waifu2x-Extension-QT-Launcher.pro || { echo "qmake for Launcher failed."; popd >/dev/null; exit 1; }
# General make command for the launcher, using multiple cores.
$MAKE -j$(nproc) || { echo "make for Launcher failed."; popd >/dev/null; exit 1; }
    echo "Deploying Qt DLLs for Waifu2x-Extension-QT-Launcher..."
    if command -v windeployqt.exe >/dev/null 2>&1; then
        windeployqt.exe "release/Beya_Waifu-Launcher.exe" --no-opengl-sw --no-translations --no-system-d3d-compiler --dir "release"
        echo "Copying MinGW runtime DLLs for Waifu2x-Extension-QT-Launcher..."
        MINGW_BIN_DIR="/mingw64/bin" # This could also be made more robust
        if [ -d "$MINGW_BIN_DIR" ]; then
            cp "$MINGW_BIN_DIR/libgcc_s_seh-1.dll" "release/" 2>/dev/null || echo "Note: libgcc_s_seh-1.dll not found in $MINGW_BIN_DIR for Launcher"
            cp "$MINGW_BIN_DIR/libstdc++-6.dll" "release/" 2>/dev/null || echo "Note: libstdc++-6.dll not found in $MINGW_BIN_DIR for Launcher"
            cp "$MINGW_BIN_DIR/libwinpthread-1.dll" "release/" 2>/dev/null || echo "Note: libwinpthread-1.dll not found in $MINGW_BIN_DIR for Launcher"
        else
            echo "Warning: MinGW bin directory not found at $MINGW_BIN_DIR. Runtime DLLs may be missing for Launcher."
        fi
    else
        echo "Warning: windeployqt.exe not found in PATH. Skipping Qt deployment for Waifu2x-Extension-QT-Launcher."
    fi
popd >/dev/null

echo "Build complete."
