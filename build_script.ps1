# Copyright (C) 2025  beyawnko
$ErrorActionPreference = "Stop"
$OutputEncoding = [System.Text.Encoding]::UTF8

# --- Environment Setup (User may need to adjust QTDIR) ---
$env:QTDIR = "C:/tmp/Qt/6.5.2/mingw_64" # Ensure this points to your Qt MinGW installation
$env:PATH = "$env:QTDIR/bin;$env:PATH;C:/tools/msys64/mingw64/bin" # Ensure MinGW tools are in PATH

Write-Host "Starting Beya_Waifu Build Script for Windows..."

# --- 1. Git Submodule Update ---
Write-Host "Updating Git submodules..."
git submodule update --init --recursive
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to update Git submodules. Please check your Git installation and repository."
    exit 1
}
Write-Host "Git submodules updated successfully."

# --- 2. Python Dependencies ---
Write-Host "Installing Python dependencies (PySide6, Pillow)..."
pip install PySide6 Pillow
if ($LASTEXITCODE -ne 0) {
    Write-Warning "Failed to install Python dependencies using pip. Attempting with pip3..."
    pip3 install PySide6 Pillow
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to install Python dependencies even with pip3. Please ensure Python and pip are installed and in PATH."
        # Consider not exiting if these are only for optional tests, but for now, let's be strict.
        exit 1
    }
}
Write-Host "Python dependencies installed successfully."

# --- 3. glslangValidator Installation ---
Write-Host "Checking for glslangValidator..."
if (-not (Get-Command glslangValidator -ErrorAction SilentlyContinue)) {
    Write-Host "glslangValidator not found."
    if (Get-Command choco -ErrorAction SilentlyContinue) {
        Write-Host "Attempting to install glslangValidator using Chocolatey..."
        choco install glslang --source=https://community.chocolatey.org/api/v2/
        if (-not (Get-Command glslangValidator -ErrorAction SilentlyContinue)) {
            Write-Warning "Failed to install glslangValidator using Chocolatey. This might be an issue if shaders need recompilation."
            # Attempt to find Vulkan SDK as an alternative
            $vulkanSDKPath = (Get-ItemProperty -Path 'HKLM:\SOFTWARE\Khronos\VulkanSDK' -Name Path -ErrorAction SilentlyContinue).Path
            if ($vulkanSDKPath -and (Test-Path "$vulkanSDKPath\Bin\glslangValidator.exe")) {
                Write-Host "Found glslangValidator in Vulkan SDK: $vulkanSDKPath\Bin. Adding to PATH for this session."
                $env:PATH = "$vulkanSDKPath\Bin;$env:PATH"
                 if (-not (Get-Command glslangValidator -ErrorAction SilentlyContinue)) {
                     Write-Warning "Still unable to find glslangValidator after adding Vulkan SDK to PATH."
                 } else {
                     Write-Host "glslangValidator found after adding Vulkan SDK to PATH."
                 }
            } else {
                 Write-Warning "Vulkan SDK not found or glslangValidator not in its Bin directory. Please install glslangValidator manually or ensure the Vulkan SDK is installed and its Bin directory is in PATH."
            }
        } else {
            Write-Host "glslangValidator installed successfully via Chocolatey."
        }
    } else {
        Write-Warning "Chocolatey not found."
        $vulkanSDKPath = (Get-ItemProperty -Path 'HKLM:\SOFTWARE\Khronos\VulkanSDK' -Name Path -ErrorAction SilentlyContinue).Path
        if ($vulkanSDKPath -and (Test-Path "$vulkanSDKPath\Bin\glslangValidator.exe")) {
            Write-Host "Found glslangValidator in Vulkan SDK: $vulkanSDKPath\Bin. Adding to PATH for this session."
            $env:PATH = "$vulkanSDKPath\Bin;$env:PATH"
            if (-not (Get-Command glslangValidator -ErrorAction SilentlyContinue)) {
                Write-Warning "Still unable to find glslangValidator after adding Vulkan SDK to PATH."
            } else {
                Write-Host "glslangValidator found after adding Vulkan SDK to PATH."
            }
        } else {
            Write-Warning "Vulkan SDK not found or glslangValidator not in its Bin directory. Please install glslangValidator manually or ensure the Vulkan SDK is installed and its Bin directory is in PATH if shader compilation is needed."
        }
    }
} else {
    Write-Host "glslangValidator found."
}

# --- 4. Build/Copy Upscaler Submodules ---
$TARGET_APP_DIR = "Waifu2x-Extension-QT"
$TARGET_LAUNCHER_DIR = "Waifu2x-Extension-QT-Launcher" # For Windows, launcher also needs copies
mkdir -p $TARGET_APP_DIR # Ensure base directory exists
mkdir -p $TARGET_LAUNCHER_DIR # Ensure launcher directory exists

# --- Real-ESRGAN ---
Write-Host "Handling Real-ESRGAN for Windows..."
$RESRGAN_SRC_DIR = "realesrgan-ncnn-vulkan"
$RESRGAN_PREBUILT_DIR = "$RESRGAN_SRC_DIR/windows"
$RESRGAN_EXE_NAME = "realesrgan-ncnn-vulkan.exe"
$RESRGAN_LEGACY_EXE_NAME = "realesrgan.exe"
$FINAL_RESRGAN_EXE_NAME = "realesrgan.exe" # Name to use in TARGET_APP_DIR

if (Test-Path "$RESRGAN_PREBUILT_DIR/$RESRGAN_EXE_NAME") {
    Write-Host "Found prebuilt Real-ESRGAN executable ($RESRGAN_EXE_NAME). Copying..."
    Copy-Item "$RESRGAN_PREBUILT_DIR/$RESRGAN_EXE_NAME" "$TARGET_APP_DIR/$FINAL_RESRGAN_EXE_NAME"
    Copy-Item "$RESRGAN_PREBUILT_DIR/$RESRGAN_EXE_NAME" "$TARGET_LAUNCHER_DIR/$FINAL_RESRGAN_EXE_NAME"
    # Copy DLLs if any - be more specific if possible
    Get-ChildItem -Path $RESRGAN_PREBUILT_DIR -Filter "*.dll" | Copy-Item -Destination "$TARGET_APP_DIR/"
    Get-ChildItem -Path $RESRGAN_PREBUILT_DIR -Filter "*.dll" | Copy-Item -Destination "$TARGET_LAUNCHER_DIR/"
} elseif (Test-Path "$RESRGAN_PREBUILT_DIR/$RESRGAN_LEGACY_EXE_NAME") {
    Write-Host "Found prebuilt Real-ESRGAN executable ($RESRGAN_LEGACY_EXE_NAME). Copying..."
    Copy-Item "$RESRGAN_PREBUILT_DIR/$RESRGAN_LEGACY_EXE_NAME" "$TARGET_APP_DIR/$FINAL_RESRGAN_EXE_NAME"
    Copy-Item "$RESRGAN_PREBUILT_DIR/$RESRGAN_LEGACY_EXE_NAME" "$TARGET_LAUNCHER_DIR/$FINAL_RESRGAN_EXE_NAME"
    Get-ChildItem -Path $RESRGAN_PREBUILT_DIR -Filter "*.dll" | Copy-Item -Destination "$TARGET_APP_DIR/"
    Get-ChildItem -Path $RESRGAN_PREBUILT_DIR -Filter "*.dll" | Copy-Item -Destination "$TARGET_LAUNCHER_DIR/"
} else {
    Write-Host "Prebuilt Real-ESRGAN not found. Attempting CMake + MinGW build..."
    Push-Location $RESRGAN_SRC_DIR
    git submodule update --init --recursive # Ensure nested submodules
    mkdir -p build_windows
    Set-Location build_windows
    cmake -G "MinGW Makefiles" ..\src # Adjust path to src if CMakeLists.txt is in src
    if ($LASTEXITCODE -ne 0) { Write-Error "Real-ESRGAN CMake configuration failed."; Pop-Location; exit 1 }
    mingw32-make.exe -j$(Get-CimInstance Win32_Processor | Select-Object -ExpandProperty NumberOfLogicalProcessors)
    if ($LASTEXITCODE -ne 0) { Write-Error "Real-ESRGAN make failed."; Pop-Location; exit 1 }

    if (Test-Path "src/$RESRGAN_EXE_NAME") {
        Copy-Item "src/$RESRGAN_EXE_NAME" "../../$TARGET_APP_DIR/$FINAL_RESRGAN_EXE_NAME"
        Copy-Item "src/$RESRGAN_EXE_NAME" "../../$TARGET_LAUNCHER_DIR/$FINAL_RESRGAN_EXE_NAME"
    } elseif (Test-Path $RESRGAN_EXE_NAME) {
         Copy-Item $RESRGAN_EXE_NAME "../../$TARGET_APP_DIR/$FINAL_RESRGAN_EXE_NAME"
         Copy-Item $RESRGAN_EXE_NAME "../../$TARGET_LAUNCHER_DIR/$FINAL_RESRGAN_EXE_NAME"
    } else {
        Write-Error "Failed to find built $RESRGAN_EXE_NAME after build attempt."
        Pop-Location; exit 1
    }
    # Copy DLLs built (if any)
    Get-ChildItem -Path . -Filter "*.dll" | Copy-Item -Destination "../../$TARGET_APP_DIR/"
    Get-ChildItem -Path . -Filter "*.dll" | Copy-Item -Destination "../../$TARGET_LAUNCHER_DIR/"
    Pop-Location
    Pop-Location # Back to root
}
# Copy Real-ESRGAN models
Write-Host "Copying Real-ESRGAN models..."
mkdir -p "$TARGET_APP_DIR/models"
if (Test-Path "$RESRGAN_PREBUILT_DIR/models") {
    Copy-Item -Path "$RESRGAN_PREBUILT_DIR/models/*" -Destination "$TARGET_APP_DIR/models/" -Recurse -Force
} elseif (Test-Path "$RESRGAN_SRC_DIR/models") {
    Copy-Item -Path "$RESRGAN_SRC_DIR/models/*" -Destination "$TARGET_APP_DIR/models/" -Recurse -Force
} else {
    Write-Warning "No Real-ESRGAN models found in prebuilt or source directory."
}


# --- Real-CUGAN ---
Write-Host "Handling Real-CUGAN for Windows..."
$RCUGAN_SRC_DIR = "realcugan-ncnn-vulkan"
$RCUGAN_PREBUILT_DIR = "$RCUGAN_SRC_DIR/windows"
$RCUGAN_EXE_NAME = "realcugan-ncnn-vulkan.exe"
$RCUGAN_LEGACY_EXE_NAME = "realcugan.exe"
$FINAL_RCUGAN_EXE_NAME = "realcugan.exe"

if (Test-Path "$RCUGAN_PREBUILT_DIR/$RCUGAN_EXE_NAME") {
    Write-Host "Found prebuilt Real-CUGAN executable ($RCUGAN_EXE_NAME). Copying..."
    Copy-Item "$RCUGAN_PREBUILT_DIR/$RCUGAN_EXE_NAME" "$TARGET_APP_DIR/$FINAL_RCUGAN_EXE_NAME"
    Copy-Item "$RCUGAN_PREBUILT_DIR/$RCUGAN_EXE_NAME" "$TARGET_LAUNCHER_DIR/$FINAL_RCUGAN_EXE_NAME"
    Get-ChildItem -Path $RCUGAN_PREBUILT_DIR -Filter "*.dll" | Copy-Item -Destination "$TARGET_APP_DIR/"
    Get-ChildItem -Path $RCUGAN_PREBUILT_DIR -Filter "*.dll" | Copy-Item -Destination "$TARGET_LAUNCHER_DIR/"
} elseif (Test-Path "$RCUGAN_PREBUILT_DIR/$RCUGAN_LEGACY_EXE_NAME") {
    Write-Host "Found prebuilt Real-CUGAN executable ($RCUGAN_LEGACY_EXE_NAME). Copying..."
    Copy-Item "$RCUGAN_PREBUILT_DIR/$RCUGAN_LEGACY_EXE_NAME" "$TARGET_APP_DIR/$FINAL_RCUGAN_EXE_NAME"
    Copy-Item "$RCUGAN_PREBUILT_DIR/$RCUGAN_LEGACY_EXE_NAME" "$TARGET_LAUNCHER_DIR/$FINAL_RCUGAN_EXE_NAME"
    Get-ChildItem -Path $RCUGAN_PREBUILT_DIR -Filter "*.dll" | Copy-Item -Destination "$TARGET_APP_DIR/"
    Get-ChildItem -Path $RCUGAN_PREBUILT_DIR -Filter "*.dll" | Copy-Item -Destination "$TARGET_LAUNCHER_DIR/"
} else {
    Write-Host "Prebuilt Real-CUGAN not found. Attempting CMake + MinGW build..."
    Push-Location $RCUGAN_SRC_DIR
    git submodule update --init --recursive
    mkdir -p build_windows
    Set-Location build_windows
    cmake -G "MinGW Makefiles" ..\src # Adjust path to src if CMakeLists.txt is in src
    if ($LASTEXITCODE -ne 0) { Write-Error "Real-CUGAN CMake configuration failed."; Pop-Location; exit 1 }
    mingw32-make.exe -j$(Get-CimInstance Win32_Processor | Select-Object -ExpandProperty NumberOfLogicalProcessors)
    if ($LASTEXITCODE -ne 0) { Write-Error "Real-CUGAN make failed."; Pop-Location; exit 1 }

    if (Test-Path "src/$RCUGAN_EXE_NAME") {
        Copy-Item "src/$RCUGAN_EXE_NAME" "../../$TARGET_APP_DIR/$FINAL_RCUGAN_EXE_NAME"
        Copy-Item "src/$RCUGAN_EXE_NAME" "../../$TARGET_LAUNCHER_DIR/$FINAL_RCUGAN_EXE_NAME"
    } elseif (Test-Path $RCUGAN_EXE_NAME) {
        Copy-Item $RCUGAN_EXE_NAME "../../$TARGET_APP_DIR/$FINAL_RCUGAN_EXE_NAME"
        Copy-Item $RCUGAN_EXE_NAME "../../$TARGET_LAUNCHER_DIR/$FINAL_RCUGAN_EXE_NAME"
    } else {
        Write-Error "Failed to find built $RCUGAN_EXE_NAME after build attempt."
        Pop-Location; exit 1
    }
    Get-ChildItem -Path . -Filter "*.dll" | Copy-Item -Destination "../../$TARGET_APP_DIR/"
    Get-ChildItem -Path . -Filter "*.dll" | Copy-Item -Destination "../../$TARGET_LAUNCHER_DIR/"
    Pop-Location
    Pop-Location # Back to root
}
# Copy Real-CUGAN models
Write-Host "Copying Real-CUGAN models..."
$RCUGAN_MODEL_SUBDIRS = "models-se", "models-pro", "models-nose" # For RealCUGAN
foreach ($model_subdir in $RCUGAN_MODEL_SUBDIRS) {
    mkdir -p "$TARGET_APP_DIR/$model_subdir"
    if (Test-Path "$RCUGAN_PREBUILT_DIR/$model_subdir") {
        Copy-Item -Path "$RCUGAN_PREBUILT_DIR/$model_subdir/*" -Destination "$TARGET_APP_DIR/$model_subdir/" -Recurse -Force
    } elseif (Test-Path "$RCUGAN_SRC_DIR/models/$model_subdir") {
        Copy-Item -Path "$RCUGAN_SRC_DIR/models/$model_subdir/*" -Destination "$TARGET_APP_DIR/$model_subdir/" -Recurse -Force
    } else {
        Write-Warning "No Real-CUGAN models found for $model_subdir in prebuilt or source directory."
    }
}

# --- 5. Main Application Build ---
Write-Host "Building Waifu2x-Extension-QT (Main Application)..."
Set-Location $TARGET_APP_DIR # Change to the application directory

# Run qmake to ensure the Makefile is up-to-date with the .pro file and recent .h changes
Write-Host "Running qmake for $TARGET_APP_DIR..."
qmake.exe Waifu2x-Extension-QT.pro -spec win32-g++ CONFIG+=release # Using release for final build
if ($LASTEXITCODE -ne 0) { Write-Error "qmake for $TARGET_APP_DIR failed."; exit 1 }

# Clean previous build artifacts (optional, but good for a clean build)
# mingw32-make.exe clean
# if ($LASTEXITCODE -ne 0) { Write-Warning "mingw32-make clean for $TARGET_APP_DIR failed. Continuing..." }

# Build the project
Write-Host "Running mingw32-make for $TARGET_APP_DIR..."
mingw32-make.exe -j$(Get-CimInstance Win32_Processor | Select-Object -ExpandProperty NumberOfLogicalProcessors)
if ($LASTEXITCODE -ne 0) {
    Write-Error "mingw32-make for $TARGET_APP_DIR failed. Check logs."
    # Consider redirecting build output to a log file here if not done by default by the calling environment
    exit 1
}
Write-Host "$TARGET_APP_DIR build completed."
Set-Location ".." # Back to root

# --- 6. Launcher Application Build (Optional, if it's still used/needed) ---
Write-Host "Building Waifu2x-Extension-QT-Launcher..."
Set-Location $TARGET_LAUNCHER_DIR

Write-Host "Running qmake for $TARGET_LAUNCHER_DIR..."
qmake.exe Waifu2x-Extension-QT-Launcher.pro -spec win32-g++ CONFIG+=release
if ($LASTEXITCODE -ne 0) { Write-Error "qmake for $TARGET_LAUNCHER_DIR failed."; exit 1 }

# mingw32-make.exe clean
# if ($LASTEXITCODE -ne 0) { Write-Warning "mingw32-make clean for $TARGET_LAUNCHER_DIR failed. Continuing..." }

Write-Host "Running mingw32-make for $TARGET_LAUNCHER_DIR..."
mingw32-make.exe -j$(Get-CimInstance Win32_Processor | Select-Object -ExpandProperty NumberOfLogicalProcessors)
if ($LASTEXITCODE -ne 0) {
    Write-Error "mingw32-make for $TARGET_LAUNCHER_DIR failed. Check logs."
    exit 1
}
Write-Host "$TARGET_LAUNCHER_DIR build completed."
Set-Location ".." # Back to root

Write-Host "Build script finished successfully."
"Build script finished. Check for any warnings or errors above."
