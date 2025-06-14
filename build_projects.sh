#!/bin/bash
set -e

# Build Waifu2x-Extension-QT
pushd Waifu2x-Extension-QT >/dev/null
qmake Waifu2x-Extension-QT.pro
make liquidglass_frag
make
popd >/dev/null

# Build optional launcher
pushd Waifu2x-Extension-QT-Launcher >/dev/null
qmake Waifu2x-Extension-QT-Launcher.pro
make
popd >/dev/null

# Handle Upscaler Binaries (Prebuilt or CMake Build) for Windows
if [[ "$(uname -s)" == MSYS_* || "$(uname -s)" == CYGWIN_* ]]; then
    echo "Windows environment detected. Handling upscaler binaries..."

    RESRGAN_SRC_DIR="realesrgan-ncnn-vulkan"
    RCUGAN_SRC_DIR="realcugan-ncnn-vulkan"

    RESRGAN_PREBUILT_DIR="realesrgan-ncnn-vulkan/windows"
    RCUGAN_PREBUILT_DIR="realcugan-ncnn-vulkan/windows"

    TARGET_QT_DIR="Waifu2x-Extension-QT"
    TARGET_LAUNCHER_DIR="Waifu2x-Extension-QT-Launcher"

    # Ensure target directories exist
    mkdir -p "$TARGET_QT_DIR"
    mkdir -p "$TARGET_LAUNCHER_DIR"

    # --- Real-ESRGAN ---
    echo "Handling Real-ESRGAN..."
    if [ -d "$RESRGAN_PREBUILT_DIR" ]; then
        echo "Found prebuilt Real-ESRGAN directory: $RESRGAN_PREBUILT_DIR"
        echo "Copying .exe files from $RESRGAN_PREBUILT_DIR to $TARGET_QT_DIR/"
        find "$RESRGAN_PREBUILT_DIR" -name "*.exe" -type f -print -exec cp {} "$TARGET_QT_DIR/" \; 2>/dev/null || true
        echo "Copying .exe files from $RESRGAN_PREBUILT_DIR to $TARGET_LAUNCHER_DIR/"
        find "$RESRGAN_PREBUILT_DIR" -name "*.exe" -type f -print -exec cp {} "$TARGET_LAUNCHER_DIR/" \; 2>/dev/null || true
        echo "Copying .dll files from $RESRGAN_PREBUILT_DIR to $TARGET_QT_DIR/"
        find "$RESRGAN_PREBUILT_DIR" -name "*.dll" -type f -print -exec cp {} "$TARGET_QT_DIR/" \; 2>/dev/null || true
        echo "Copying .dll files from $RESRGAN_PREBUILT_DIR to $TARGET_LAUNCHER_DIR/"
        find "$RESRGAN_PREBUILT_DIR" -name "*.dll" -type f -print -exec cp {} "$TARGET_LAUNCHER_DIR/" \; 2>/dev/null || true
    else
        echo "Prebuilt Real-ESRGAN not found at $RESRGAN_PREBUILT_DIR. Attempting CMake build..."
        if [ -d "$RESRGAN_SRC_DIR" ]; then
            echo "Found Real-ESRGAN source directory: $RESRGAN_SRC_DIR"
            mkdir -p "$RESRGAN_SRC_DIR/build"
            pushd "$RESRGAN_SRC_DIR/build" >/dev/null
            echo "Running CMake for Real-ESRGAN..."
            cmake -G "MSYS Makefiles" .. || { echo "Real-ESRGAN CMake failed. Exiting."; popd >/dev/null; exit 1; }
            echo "Running make for Real-ESRGAN..."
            make || { echo "Real-ESRGAN make failed. Exiting."; popd >/dev/null; exit 1; }
            popd >/dev/null
            echo "Real-ESRGAN build successful."

            echo "Copying built Real-ESRGAN .exe files to $TARGET_QT_DIR/"
            find "$RESRGAN_SRC_DIR/build" -name "*.exe" -type f -print -exec cp {} "$TARGET_QT_DIR/" \; 2>/dev/null || true
            find "$RESRGAN_SRC_DIR/build/src" -name "*.exe" -type f -print -exec cp {} "$TARGET_QT_DIR/" \; 2>/dev/null || true # Common output location
            echo "Copying built Real-ESRGAN .exe files to $TARGET_LAUNCHER_DIR/"
            find "$RESRGAN_SRC_DIR/build" -name "*.exe" -type f -print -exec cp {} "$TARGET_LAUNCHER_DIR/" \; 2>/dev/null || true
            find "$RESRGAN_SRC_DIR/build/src" -name "*.exe" -type f -print -exec cp {} "$TARGET_LAUNCHER_DIR/" \; 2>/dev/null || true
            echo "Copying built Real-ESRGAN .dll files to $TARGET_QT_DIR/"
            find "$RESRGAN_SRC_DIR/build" -name "*.dll" -type f -print -exec cp {} "$TARGET_QT_DIR/" \; 2>/dev/null || true
            find "$RESRGAN_SRC_DIR/build/src" -name "*.dll" -type f -print -exec cp {} "$TARGET_QT_DIR/" \; 2>/dev/null || true
            echo "Copying built Real-ESRGAN .dll files to $TARGET_LAUNCHER_DIR/"
            find "$RESRGAN_SRC_DIR/build" -name "*.dll" -type f -print -exec cp {} "$TARGET_LAUNCHER_DIR/" \; 2>/dev/null || true
            find "$RESRGAN_SRC_DIR/build/src" -name "*.dll" -type f -print -exec cp {} "$TARGET_LAUNCHER_DIR/" \; 2>/dev/null || true
        else
            echo "Real-ESRGAN source directory $RESRGAN_SRC_DIR not found. Cannot build. Exiting."
            exit 1
        fi
    fi

    # --- Real-CUGAN ---
    echo "Handling Real-CUGAN..."
    if [ -d "$RCUGAN_PREBUILT_DIR" ]; then
        echo "Found prebuilt Real-CUGAN directory: $RCUGAN_PREBUILT_DIR"
        echo "Copying .exe files from $RCUGAN_PREBUILT_DIR to $TARGET_QT_DIR/"
        find "$RCUGAN_PREBUILT_DIR" -name "*.exe" -type f -print -exec cp {} "$TARGET_QT_DIR/" \; 2>/dev/null || true
        echo "Copying .exe files from $RCUGAN_PREBUILT_DIR to $TARGET_LAUNCHER_DIR/"
        find "$RCUGAN_PREBUILT_DIR" -name "*.exe" -type f -print -exec cp {} "$TARGET_LAUNCHER_DIR/" \; 2>/dev/null || true
        echo "Copying .dll files from $RCUGAN_PREBUILT_DIR to $TARGET_QT_DIR/"
        find "$RCUGAN_PREBUILT_DIR" -name "*.dll" -type f -print -exec cp {} "$TARGET_QT_DIR/" \; 2>/dev/null || true
        echo "Copying .dll files from $RCUGAN_PREBUILT_DIR to $TARGET_LAUNCHER_DIR/"
        find "$RCUGAN_PREBUILT_DIR" -name "*.dll" -type f -print -exec cp {} "$TARGET_LAUNCHER_DIR/" \; 2>/dev/null || true
    else
        echo "Prebuilt Real-CUGAN not found at $RCUGAN_PREBUILT_DIR. Attempting CMake build..."
        if [ -d "$RCUGAN_SRC_DIR" ]; then
            echo "Found Real-CUGAN source directory: $RCUGAN_SRC_DIR"
            mkdir -p "$RCUGAN_SRC_DIR/build"
            pushd "$RCUGAN_SRC_DIR/build" >/dev/null
            echo "Running CMake for Real-CUGAN..."
            cmake -G "MSYS Makefiles" .. || { echo "Real-CUGAN CMake failed. Exiting."; popd >/dev/null; exit 1; }
            echo "Running make for Real-CUGAN..."
            make || { echo "Real-CUGAN make failed. Exiting."; popd >/dev/null; exit 1; }
            popd >/dev/null
            echo "Real-CUGAN build successful."

            echo "Copying built Real-CUGAN .exe files to $TARGET_QT_DIR/"
            find "$RCUGAN_SRC_DIR/build" -name "*.exe" -type f -print -exec cp {} "$TARGET_QT_DIR/" \; 2>/dev/null || true
            find "$RCUGAN_SRC_DIR/build/src" -name "*.exe" -type f -print -exec cp {} "$TARGET_QT_DIR/" \; 2>/dev/null || true
            echo "Copying built Real-CUGAN .exe files to $TARGET_LAUNCHER_DIR/"
            find "$RCUGAN_SRC_DIR/build" -name "*.exe" -type f -print -exec cp {} "$TARGET_LAUNCHER_DIR/" \; 2>/dev/null || true
            find "$RCUGAN_SRC_DIR/build/src" -name "*.exe" -type f -print -exec cp {} "$TARGET_LAUNCHER_DIR/" \; 2>/dev/null || true
            echo "Copying built Real-CUGAN .dll files to $TARGET_QT_DIR/"
            find "$RCUGAN_SRC_DIR/build" -name "*.dll" -type f -print -exec cp {} "$TARGET_QT_DIR/" \; 2>/dev/null || true
            find "$RCUGAN_SRC_DIR/build/src" -name "*.dll" -type f -print -exec cp {} "$TARGET_QT_DIR/" \; 2>/dev/null || true
            echo "Copying built Real-CUGAN .dll files to $TARGET_LAUNCHER_DIR/"
            find "$RCUGAN_SRC_DIR/build" -name "*.dll" -type f -print -exec cp {} "$TARGET_LAUNCHER_DIR/" \; 2>/dev/null || true
            find "$RCUGAN_SRC_DIR/build/src" -name "*.dll" -type f -print -exec cp {} "$TARGET_LAUNCHER_DIR/" \; 2>/dev/null || true
        else
            echo "Real-CUGAN source directory $RCUGAN_SRC_DIR not found. Cannot build. Exiting."
            exit 1
        fi
    fi
fi

echo "Build complete."
