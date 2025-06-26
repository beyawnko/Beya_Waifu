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
PIP_CMD=""
if command -v pip3 >/dev/null 2>&1; then
    PIP_CMD="pip3"
elif command -v pip >/dev/null 2>&1; then
    PIP_CMD="pip"
fi

if [ -n "$PIP_CMD" ]; then
    echo "Using pip command: $PIP_CMD"
    $PIP_CMD install --user PySide6 Pillow aqtinstall
else
    echo "pip not found; skipping Python package installation (PySide6, Pillow, aqtinstall)."
fi

# Determine available make tool
MAKE=make
command -v $MAKE >/dev/null 2>&1 || MAKE=mingw32-make
command -v $MAKE >/dev/null 2>&1 || { echo "make tool not found"; exit 1; }

# --- OS-specific setup and upscaler builds ---

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
        echo "Linux environment detected."

        # Ensure essential Qt6 development packages are installed via apt-get for a stable build environment.
        # This is more reliable than aqtinstall if specific recent versions are not strictly needed for the build system test.
        echo "Ensuring Qt6 development packages are installed via apt-get..."
        if command -v apt-get >/dev/null 2>&1; then
            sudo apt-get update -qq
            sudo apt-get install -y --no-install-recommends \
                qt6-base-dev \
                qt6-base-dev-tools \
                qt6-multimedia-dev \
                qt6-shadertools-dev \
                qt6-declarative-dev \
                qt6-quick3d-dev \
                libvulkan-dev \
                glslang-tools \
                libwebp-dev || { echo "Failed to install Qt6/dev packages via apt-get."; exit 1; }
            echo "Qt6/dev packages installation attempt finished."
        else
            echo "apt-get not found. Cannot install Qt6/dev packages. Build may fail if they are not already present."
        fi

        # QT_BIN_PATH is typically not needed when using system-installed Qt that CMake can find.
        # Unset it to avoid potential conflicts if it was set by a previous run or caller.
        unset QT_BIN_PATH

        # Validate Qt tools (qmake, qsb) which should be available from apt packages.
        # The qmake check is primarily for the script's initial validation logic,
        # as CMake will use its own find_package mechanism.
        QMAKE_CMD=""
        if command -v qmake6 >/dev/null 2>&1; then
            QMAKE_CMD="qmake6"
        elif command -v qmake >/dev/null 2>&1; then
            QMAKE_CMD="qmake"
        fi

        if [ -z "$QMAKE_CMD" ]; then
            echo "qmake (or qmake6) not found in PATH. Ensure Qt dev tools (qt6-base-dev-tools) are installed correctly."
            exit 1
        else
            echo "Using qmake command: $($QMAKE_CMD -v)"
        fi

        QSB_CMD=""
        if command -v qsb6 >/dev/null 2>&1; then
            QSB_CMD="qsb6"
        elif command -v qsb >/dev/null 2>&1; then
            QSB_CMD="qsb"
        fi

        if [ -z "$QSB_CMD" ]; then
             echo "Warning: qsb tool (or qsb6) not found directly in PATH. Shader compilation relies on Qt6::ShaderTools being found by CMake."
        else
            echo "Using qsb command: $($QSB_CMD --version || true)" # qsb might not have --version, so allow failure
        fi

        # Upscaler builds (Real-ESRGAN, Real-CUGAN) remain the same, using CMake.
        # These are built first as the main application might depend on their output.
        echo "Building Real-ESRGAN for Linux..."
        pushd "$RESRGAN_SRC_DIR" >/dev/null
        git submodule update --init --recursive || { echo "Submodule update failed for $RESRGAN_SRC_DIR"; popd >/dev/null; exit 1; }
        rm -rf build_linux && mkdir -p build_linux && cd build_linux
        cmake ../src || { echo "Real-ESRGAN CMake configuration failed."; popd >/dev/null; exit 1; }
        $MAKE -j$(nproc) || { echo "Real-ESRGAN make failed."; popd >/dev/null; exit 1; }
        cp src/realesrgan-ncnn-vulkan "../../$TARGET_APP_DIR/" 2>/dev/null || cp realesrgan-ncnn-vulkan "../../$TARGET_APP_DIR/" 2>/dev/null || { echo "Failed to find/copy built realesrgan-ncnn-vulkan"; popd >/dev/null; exit 1; }
        popd >/dev/null

        echo "Copying Real-ESRGAN models for Linux..."
        mkdir -p "$TARGET_APP_DIR/models"
        cp -R "$RESRGAN_SRC_DIR/models/"* "$TARGET_APP_DIR/models/"

        echo "Building Real-CUGAN for Linux..."
        RCUGAN_MODEL_SUBDIRS="models-se models-pro models-nose"
        pushd "$RCUGAN_SRC_DIR" >/dev/null
        git submodule update --init --recursive || { echo "Submodule update failed for $RCUGAN_SRC_DIR"; popd >/dev/null; exit 1; }
        rm -rf build_linux && mkdir -p build_linux && cd build_linux
        cmake ../src || { echo "Real-CUGAN CMake configuration failed."; popd >/dev/null; exit 1; }
        $MAKE -j$(nproc) || { echo "Real-CUGAN make failed."; popd >/dev/null; exit 1; }
        cp src/realcugan-ncnn-vulkan "../../$TARGET_APP_DIR/" 2>/dev/null || cp realcugan-ncnn-vulkan "../../$TARGET_APP_DIR/" 2>/dev/null || { echo "Failed to find/copy built realcugan-ncnn-vulkan"; popd >/dev/null; exit 1; }
        popd >/dev/null

        echo "Copying Real-CUGAN models for Linux..."
        for model_subdir in $RCUGAN_MODEL_SUBDIRS; do
            mkdir -p "$TARGET_APP_DIR/$model_subdir"
            cp -R "$RCUGAN_SRC_DIR/models/$model_subdir/"* "$TARGET_APP_DIR/$model_subdir/"
        done

        # --- Qt Application Builds (Using CMake) ---
        echo "Building Qt applications using CMake for Linux..."
        TOP_LEVEL_BUILD_DIR="build_cmake_linux"
        rm -rf "$TOP_LEVEL_BUILD_DIR" && mkdir -p "$TOP_LEVEL_BUILD_DIR"
        pushd "$TOP_LEVEL_BUILD_DIR" >/dev/null

        CMAKE_PREFIX_PATH_ARG=""
        # Check if QT_BIN_PATH was set (e.g., by aqtinstall)
        if [ -n "$QT_BIN_PATH" ]; then
            # CMAKE_PREFIX_PATH should point to the directory containing lib/cmake/Qt6, etc.
            # For aqtinstall structure like ./Qt_linux/6.6.0/gcc_64, this is $QT_FULL_PATH_LINUX
             _qt_prefix_to_use="$QT_FULL_PATH_LINUX"
            if [ -d "$_qt_prefix_to_use" ]; then
                 CMAKE_PREFIX_PATH_ARG="-DCMAKE_PREFIX_PATH=$_qt_prefix_to_use"
                 echo "Using CMAKE_PREFIX_PATH: $_qt_prefix_to_use (derived from QT_BIN_PATH)"
            else
                echo "Warning: QT_BIN_PATH is set to $QT_BIN_PATH, but derived prefix $_qt_prefix_to_use does not exist. CMake might not find Qt correctly."
            fi
        else
            echo "QT_BIN_PATH not set, CMake will attempt to find system Qt."
        fi

        cmake .. $CMAKE_PREFIX_PATH_ARG -DCMAKE_BUILD_TYPE=Release || { echo "Top-level CMake configuration failed."; popd >/dev/null; exit 1; }
        $MAKE -j$(nproc) || { echo "Top-level CMake build failed."; popd >/dev/null; exit 1; }

        cp "Waifu2x-Extension-QT/Beya_Waifu" "../$TARGET_APP_DIR/" || { echo "Failed to copy Beya_Waifu executable."; popd >/dev/null; exit 1; }
        LAUNCHER_TARGET_DIR="../Waifu2x-Extension-QT-Launcher"
        mkdir -p "$LAUNCHER_TARGET_DIR"
        cp "Waifu2x-Extension-QT-Launcher/Beya_Waifu-Launcher" "$LAUNCHER_TARGET_DIR/" || { echo "Failed to copy Beya_Waifu-Launcher executable."; popd >/dev/null; exit 1; }

        popd >/dev/null

        # Restore original PATH if it was modified
        export PATH=$_ORIGINAL_PATH
        echo "Restored original PATH."
        ;;

    *)
        # Fallback for unsupported operating systems.
        echo "Unsupported OS: $(uname -s). Skipping upscaler and Qt builds."
        # Optionally exit 1 if upscalers are mandatory for the application to function.
        ;;
esac

# --- Qt Application Builds (Windows) ---
case $(uname -s | tr '[:upper:]' '[:lower:]') in
    msys*|cygwin*|mingw*)
        echo "Building Qt applications using CMake for Windows..."
        TOP_LEVEL_BUILD_DIR_WIN="build_cmake_windows"
        rm -rf "$TOP_LEVEL_BUILD_DIR_WIN" # Clean previous CMake build
        mkdir -p "$TOP_LEVEL_BUILD_DIR_WIN"
        pushd "$TOP_LEVEL_BUILD_DIR_WIN" >/dev/null

        # Configure the entire project using the root CMakeLists.txt
        # Pass CMAKE_PREFIX_PATH if QT_BIN_PATH is set
        CMAKE_PREFIX_PATH_ARG_WIN=""
        if [ -n "$QT_BIN_PATH" ]; then
            QT_PREFIX_PATH_WIN=$(dirname "$QT_BIN_PATH")
            # Convert to MSYS-style path if necessary, though CMake might handle Windows paths.
            # For MinGW Makefiles, MSYS-style paths are generally safer.
            QT_PREFIX_PATH_WIN_MSYS=$(echo "$QT_PREFIX_PATH_WIN" | sed 's|\\|/|g; s|C:|/c|')
            CMAKE_PREFIX_PATH_ARG_WIN="-DCMAKE_PREFIX_PATH=$QT_PREFIX_PATH_WIN_MSYS"
        fi

        # Use "MinGW Makefiles" generator for consistency with upscaler builds on Windows if they were built from source.
        # This assumes we are in an MSYS2 MinGW environment.
        cmake .. -G "MinGW Makefiles" $CMAKE_PREFIX_PATH_ARG_WIN -DCMAKE_BUILD_TYPE=Release || { echo "Top-level CMake configuration for Windows failed."; popd >/dev/null; exit 1; }

        # Build the entire project
        $MAKE -j$(nproc) || { echo "Top-level CMake build for Windows failed."; popd >/dev/null; exit 1; }

        # --- Deployment for Windows (after CMake build) ---

        # Main Application Deployment
        echo "Deploying Qt DLLs for Waifu2x-Extension-QT (CMake build)..."
        # Output path from CMake: <build_dir>/Waifu2x-Extension-QT/Beya_Waifu.exe
        BUILT_APP_EXE_PATH="Waifu2x-Extension-QT/Beya_Waifu.exe"
        # Target deployment directory: ../Waifu2x-Extension-QT/release/
        # windeployqt needs to run from a directory that contains the .exe or be given full path to .exe
        # Create release directory if it doesn't exist inside TARGET_APP_DIR
        mkdir -p "../$TARGET_APP_DIR/release"
        if [ -f "$BUILT_APP_EXE_PATH" ]; then
            if command -v windeployqt.exe >/dev/null 2>&1; then
                # Copy the exe to the release dir first, then run windeployqt
                cp "$BUILT_APP_EXE_PATH" "../$TARGET_APP_DIR/release/"
                pushd "../$TARGET_APP_DIR/release" >/dev/null
                windeployqt.exe "Beya_Waifu.exe" --no-opengl-sw --no-translations --no-system-d3d-compiler --dir "."
                echo "Copying MinGW runtime DLLs for Waifu2x-Extension-QT..."
                MINGW_BIN_DIR_DETECTED=$(dirname "$(which gcc)") # More robust way to find MinGW bin
                if [ -d "$MINGW_BIN_DIR_DETECTED" ]; then
                    cp "$MINGW_BIN_DIR_DETECTED/libgcc_s_seh-1.dll" "." 2>/dev/null || echo "Note: libgcc_s_seh-1.dll not found in $MINGW_BIN_DIR_DETECTED"
                    cp "$MINGW_BIN_DIR_DETECTED/libstdc++-6.dll" "." 2>/dev/null || echo "Note: libstdc++-6.dll not found in $MINGW_BIN_DIR_DETECTED"
                    cp "$MINGW_BIN_DIR_DETECTED/libwinpthread-1.dll" "." 2>/dev/null || echo "Note: libwinpthread-1.dll not found in $MINGW_BIN_DIR_DETECTED"
                else
                    echo "Warning: MinGW bin directory not found via 'which gcc'. Runtime DLLs may be missing."
                fi
                popd >/dev/null
            else
                echo "Warning: windeployqt.exe not found in PATH. Skipping Qt deployment for Waifu2x-Extension-QT."
                # As a fallback, copy the executable without full deployment
                 cp "$BUILT_APP_EXE_PATH" "../$TARGET_APP_DIR/"
            fi
        else
            echo "Error: Built Beya_Waifu.exe not found at $BUILT_APP_EXE_PATH"
        fi

        # Launcher Application Deployment
        echo "Deploying Qt DLLs for Waifu2x-Extension-QT-Launcher (CMake build)..."
        BUILT_LAUNCHER_EXE_PATH="Waifu2x-Extension-QT-Launcher/Beya_Waifu-Launcher.exe"
        mkdir -p "../$TARGET_LAUNCHER_DIR/release"
        if [ -f "$BUILT_LAUNCHER_EXE_PATH" ]; then
            if command -v windeployqt.exe >/dev/null 2>&1; then
                cp "$BUILT_LAUNCHER_EXE_PATH" "../$TARGET_LAUNCHER_DIR/release/"
                pushd "../$TARGET_LAUNCHER_DIR/release" >/dev/null
                windeployqt.exe "Beya_Waifu-Launcher.exe" --no-opengl-sw --no-translations --no-system-d3d-compiler --dir "."
                echo "Copying MinGW runtime DLLs for Waifu2x-Extension-QT-Launcher..."
                MINGW_BIN_DIR_DETECTED=$(dirname "$(which gcc)")
                if [ -d "$MINGW_BIN_DIR_DETECTED" ]; then
                    cp "$MINGW_BIN_DIR_DETECTED/libgcc_s_seh-1.dll" "." 2>/dev/null || echo "Note: libgcc_s_seh-1.dll not found in $MINGW_BIN_DIR_DETECTED for Launcher"
                    cp "$MINGW_BIN_DIR_DETECTED/libstdc++-6.dll" "." 2>/dev/null || echo "Note: libstdc++-6.dll not found in $MINGW_BIN_DIR_DETECTED for Launcher"
                    cp "$MINGW_BIN_DIR_DETECTED/libwinpthread-1.dll" "." 2>/dev/null || echo "Note: libwinpthread-1.dll not found in $MINGW_BIN_DIR_DETECTED for Launcher"
                else
                    echo "Warning: MinGW bin directory not found via 'which gcc'. Runtime DLLs may be missing for Launcher."
                fi
                popd >/dev/null
            else
                echo "Warning: windeployqt.exe not found in PATH. Skipping Qt deployment for Waifu2x-Extension-QT-Launcher."
                cp "$BUILT_LAUNCHER_EXE_PATH" "../$TARGET_LAUNCHER_DIR/"
            fi
        else
            echo "Error: Built Beya_Waifu-Launcher.exe not found at $BUILT_LAUNCHER_EXE_PATH"
        fi
        popd >/dev/null # Exit TOP_LEVEL_BUILD_DIR_WIN
        ;;
esac

echo "Build complete."
