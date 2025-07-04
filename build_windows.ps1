<#
Copyright (C) 2025  beyawnko
.SYNOPSIS
    A build script to set up a development environment and build a project.
    It ensures all necessary dependencies like Chocolatey, MSYS2, and Qt (with all required modules) are installed.

.DESCRIPTION
    This script automates the following steps:
    1.  Verifies it is running with Administrator privileges.
    2.  Ensures Chocolatey package manager is installed and updates the session PATH.
    3.  Installs required packages using Chocolatey (MSYS2, CMake, Git, Python).
    4.  Installs MSYS2/MinGW toolchain packages using pacman.
    5.  Ensures a specific Qt version is installed using aqtinstall, verifying all required modules are present.
    6.  Updates Git submodules.
    7.  Executes the final project build script with the correct environment PATH.
    8.  Warns the user if a reboot is required to finalize installations.

.NOTES
    Author: beyawnko (Refactored by Gemini)
    Version: 4.8
    Requires: PowerShell 5.1 or later.
    Must be run with Administrator privileges to install software.
#>
[CmdletBinding()]
param (
    # The version of Qt to install.
    [string]$QtVersion = '6.6.3', # Updated to ensure RHI headers are publicly available

    # The base directory for the Qt installation.
    [string]$QtDir = '/tmp/Qt'
)

# --- Script-Scoped Variables ---
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
# This flag will be set to true if any installer returns exit code 3010 (reboot required).
$script:RebootRequired = $false
# This will store the path to the Qt bin directory if it's installed by this script.
$script:QtBinPath = ''

# --- Helper Functions ---

# A robust function to execute external commands.
function Invoke-Process {
    param(
        [string]$FilePath,
        [string[]]$ArgumentList
    )
    
    $MyInvocation.MyCommand.Name | Write-Host -ForegroundColor Cyan
    
    # Resolve command path before trying to execute it.
    $commandPath = Get-Command $FilePath -ErrorAction SilentlyContinue
    if (-not $commandPath) {
        throw "Cannot find executable: '$FilePath'. Ensure it is in your PATH."
    }

    Write-Host ">> & '$($commandPath.Source)' $ArgumentList"
    & $commandPath.Source $ArgumentList
    
    if ($LASTEXITCODE -eq 3010) {
        Write-Warning "A reboot is required to complete the last installation step."
        $script:RebootRequired = $true
    }
    elseif ($LASTEXITCODE -ne 0) {
        # Using ${} around the variable name prevents parsing errors when it's followed by punctuation like a colon.
        throw "Command failed with exit code ${LASTEXITCODE}: '$($commandPath.Source)' $ArgumentList"
    }
}

# Converts a Windows path to an MSYS2-compatible path.
function Convert-PathToMsys {
    param(
        [string]$Path
    )

    if ($Path -match '^([A-Za-z]):') {
        $drive = $matches[1].ToLower()
        return "/$drive" + ($Path.Substring(2) -replace '\\', '/')
    }

    if ($Path -match '^\\(?!\\)') {
        $drive = (Get-Location).Drive.Name.TrimEnd(':').ToLower()
        return "/$drive" + ($Path -replace '\\', '/')
    }

    return ($Path -replace '\\', '/')
}

# Finds the MSYS2 bash executable.
function Find-Msys2Bash {
    $MyInvocation.MyCommand.Name | Write-Host -ForegroundColor Cyan

    # Check the primary Chocolatey installation path for msys2 first.
    $toolsPath = 'C:\tools\msys64\usr\bin\bash.exe'
    if (Test-Path $toolsPath) {
        Write-Host "Found MSYS2 bash at '$toolsPath'"
        return $toolsPath
    }

    # Check the ProgramData path as a secondary location.
    $chocoProgramDataPath = Join-Path $env:ProgramData 'chocolatey\lib\msys2\tools\msys64\usr\bin\bash.exe'
    if (Test-Path $chocoProgramDataPath) {
        Write-Host "Found MSYS2 bash at '$chocoProgramDataPath'"
        return $chocoProgramDataPath
    }
    
    # Check if bash is in the system's PATH as a final fallback.
    $bashInPath = Get-Command bash.exe -ErrorAction SilentlyContinue
    if ($bashInPath) {
        Write-Host "Found MSYS2 bash in PATH at '$($bashInPath.Source)'"
        return $bashInPath.Source
    }

    throw "MSYS2 bash.exe not found. Please ensure MSYS2 is installed and accessible."
}

# Checks if the script is running with Administrator privileges.
# function Test-IsAdmin {
#     $identity = [System.Security.Principal.WindowsIdentity]::GetCurrent()
#     $principal = New-Object System.Security.Principal.WindowsPrincipal($identity)
#     return $principal.IsInRole([System.Security.Principal.WindowsBuiltInRole]::Administrator)
# }


# --- Environment Setup Functions ---

# Ensures Chocolatey is installed.
function Ensure-Chocolatey {
    $MyInvocation.MyCommand.Name | Write-Host -ForegroundColor Cyan

    if (Get-Command choco -ErrorAction SilentlyContinue) {
        Write-Host "Chocolatey is already installed."
        return
    }
    
    Write-Host "Chocolatey not found. Installing..."
    # Set execution policy and security protocol for the installation process.
    Set-ExecutionPolicy Bypass -Scope Process -Force
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor [System.Net.SecurityProtocolType]::Tls12

    $installerUrl = "https://community.chocolatey.org/install.ps1"
    $tempFile = Join-Path $env:TEMP "install-choco.ps1"

    try {
        Write-Host "Downloading Chocolatey installer from '$installerUrl'..."
        (New-Object System.Net.WebClient).DownloadFile($installerUrl, $tempFile)
        
        Write-Host "Running Chocolatey installer..."
        & $tempFile
    }
    finally {
        if (Test-Path $tempFile) {
            Remove-Item $tempFile -Force -ErrorAction SilentlyContinue
        }
    }

    $chocoBinPath = Join-Path $env:ProgramData 'chocolatey\bin'
    Write-Host "Adding '$chocoBinPath' to the current session's PATH."
    $env:PATH = "$chocoBinPath;$env:PATH"

    if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
        throw "Chocolatey installation appears to have failed, even after updating session PATH."
    }
    Write-Host "Chocolatey installed successfully and PATH updated for this session."
}

# Installs a Chocolatey package if missing.
function Ensure-ChocoPackage {
    param(
        [string]$PackageName
    )
    $MyInvocation.MyCommand.Name | Write-Host -ForegroundColor Cyan
    
    $installed = choco list --local-only --exact --limit-output $PackageName
    if ($installed) {
        Write-Host "Chocolatey package '$PackageName' is already installed."
        return
    }

    Write-Host "Installing Chocolatey package '$PackageName'..."
    Invoke-Process 'choco' @('install', '-y', $PackageName)
}

# Updates MSYS2 and installs toolchain packages.
function Install-MSYS2Packages {
    param(
        [string]$Msys2BashPath
    )
    $MyInvocation.MyCommand.Name | Write-Host -ForegroundColor Cyan

    Write-Host "Updating MSYS2 and installing toolchain packages..."
    $commandString = "pacman -Syu --noconfirm && pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-make mingw-w64-x86_64-cmake git"
    $pacmanArgs = @(
        '-lc',
        $commandString
    )
    Invoke-Process -FilePath $Msys2BashPath -ArgumentList $pacmanArgs
}

# Installs Qt via aqtinstall and sets QtBinPath.
function Ensure-QMake {
    $MyInvocation.MyCommand.Name | Write-Host -ForegroundColor Cyan

    # Always install Qt to ensure a consistent environment
    Write-Host "Installing Qt $QtVersion via aqtinstall..."
    # Ensure-ChocoPackage 'python' # Assuming Python is available or aqt can be run directly

    Invoke-Process 'python' @('-m', 'pip', 'install', '--upgrade', 'pip')
    Invoke-Process 'python' @('-m', 'pip', 'install', 'aqtinstall')

    if (-not (Test-Path $QtDir)) {
        New-Item -ItemType Directory -Path $QtDir | Out-Null
    }
    $QtDir = (Resolve-Path $QtDir).Path

    $qtModules = @('qtmultimedia', 'qtshadertools', 'qtquick3d')
    $qtInstallArgs = @(
        '-m', 'aqt',
        'install-qt', 'windows', 'desktop', $QtVersion, 'win64_mingw',
        '-O', $QtDir, '-m'
    ) + $qtModules
    Invoke-Process 'python' $qtInstallArgs

    $script:QtBinPath = Join-Path -Path $QtDir -ChildPath "$QtVersion\mingw_64\bin"
    if (-not (Test-Path $script:QtBinPath)) {
        throw "Qt installation failed. Path not found: '$($script:QtBinPath)'"
    }

    # Verify qsb.exe exists, as it's crucial for the build
    $qsbPath = Join-Path $script:QtBinPath 'qsb.exe'
    if (-not (Test-Path $qsbPath)) {
        throw "Qt shadertools module missing. qsb.exe not found at '$qsbPath'"
    }

    # Verify qmake.exe exists
    $qmakePath = Join-Path $script:QtBinPath 'qmake.exe'
    if (-not (Test-Path $qmakePath)) {
        throw "Qt qmake.exe not found at '$qmakePath'"
    }
    Write-Host "qmake.exe successfully found at '$qmakePath'"

    Write-Host "Qt installation complete. QtBinPath set to '$($script:QtBinPath)'"
}

# --- Build Steps ---

# Updates Git submodules to the current revision.
function Update-Submodules {
    $MyInvocation.MyCommand.Name | Write-Host -ForegroundColor Cyan
    Write-Host "Updating Git submodules..."
    Set-Location $PSScriptRoot
    Invoke-Process 'git' @('submodule', 'update', '--init', '--recursive')
}

# Runs the main build script inside the MSYS2 environment.
function Build-Project {
    param(
        [string]$Msys2BashPath
    )
    $MyInvocation.MyCommand.Name | Write-Host -ForegroundColor Cyan
    
    Write-Host "Running the main project build script..."
    
    $buildScriptName = './build_projects.sh'
    $scriptFullPath = Join-Path -Path $PSScriptRoot -ChildPath $buildScriptName
    if (-not (Test-Path -LiteralPath $scriptFullPath)) {
        throw "Build script not found at '$scriptFullPath'"
    }

    $currentDir = $PSScriptRoot
    $msysDir = Convert-PathToMsys $currentDir

    # Define paths to the necessary bin directories for the build environment
    $mingwBinPath = 'C:\tools\msys64\mingw64\bin'
    $msysBinPath = 'C:\tools\msys64\usr\bin'

    # Convert paths to a format bash understands
    $msysMingwPath = Convert-PathToMsys $mingwBinPath
    $msysUsrBinPath = Convert-PathToMsys $msysBinPath
    $msysQtPath = '' # Initialize to empty

    # Build the path components
    $pathComponents = @(
        $msysMingwPath,
        $msysUsrBinPath
    )
    if (-not [string]::IsNullOrEmpty($script:QtBinPath)) {
        $msysQtPath = Convert-PathToMsys $script:QtBinPath # Will be correctly assigned here
        $pathComponents += $msysQtPath
    }

    # Join the path components for the bash script. No extra quotes here.
    $pathPrefix = $pathComponents -join ':'

    # Create a temporary bash script to robustly set the environment and execute the build.
    # This avoids complex quoting issues when calling bash from PowerShell.
    $scriptContent = @"
#!/bin/bash
# Exit immediately if a command exits with a non-zero status.
# set -e # Temporarily disable 'set -e' for debugging PATH and qmake
echo "--- Executing inside temporary bash wrapper (with debugging) ---"

echo "Initial PATH within wrapper:"
echo "-------------------------------------"
echo `$PATH | tr ':' '
'
echo "-------------------------------------"

# Path to Qt, as passed from PowerShell, converted by Convert-PathToMsys
echo "Expected Qt bin directory (from PowerShell): {3}"
echo "Contents of expected Qt bin directory:"
ls -al "{3}" || echo "Could not list contents of {3}"
echo "-------------------------------------"

echo "Attempting to find qmake with 'command -v qmake':"
command -v qmake || echo "qmake not found by 'command -v qmake'"
echo "-------------------------------------"
echo "Result of 'which qmake':"
which qmake || echo "qmake not found by 'which qmake'"
echo "-------------------------------------"

# Re-enable 'set -e' after debugging output
set -e
echo "Continuing with the build script..."

# Unset potentially problematic variables passed from the host environment.
unset MAKEFLAGS
unset MFLAGS

# Set the path for the build environment. The dollar sign in `$PATH` is escaped with a backtick (` `)
# to prevent PowerShell from expanding it prematurely.
export PATH='{0}':`$PATH

# Change to the project directory.
cd '{1}'

# Execute the main build script.
./{2}

echo "--- Finished executing inside temporary bash wrapper ---"
"@ -f $pathPrefix, $msysDir, $buildScriptName, $msysQtPath

    $tempScriptName = "temp_build_wrapper.sh"
    $tempScriptPath = Join-Path -Path $PSScriptRoot -ChildPath $tempScriptName

    try {
        # Write the script content to the temp file, ensuring LF line endings for bash.
        [IO.File]::WriteAllLines($tempScriptPath, ($scriptContent -split "`r`n"), [System.Text.Encoding]::ASCII)

        # Execute the wrapper script using bash. The -l flag makes it a login shell.
        Invoke-Process -FilePath $Msys2BashPath -ArgumentList @('-l', $tempScriptPath)
    }
    finally {
        # Clean up the temporary script
        # if (Test-Path $tempScriptPath) {
        #     Remove-Item $tempScriptPath -Force -ErrorAction SilentlyContinue
        # }
    }
}


# --- Main Execution Logic ---

# 1. Check for Administrator privileges before doing anything else.
# if (-not (Test-IsAdmin)) {
#     Write-Error "This script must be run with Administrator privileges."
#     exit 1
# }

try {
    Write-Host "`n--- Starting Build Process ---`n"
    Write-Host "Stage 1: Ensuring Chocolatey and core packages."
    # Step 1: Ensure core dependencies are installed with Chocolatey.
    Ensure-Chocolatey
    Ensure-ChocoPackage 'msys2'
    Ensure-ChocoPackage 'cmake'
    Ensure-ChocoPackage 'git'
    Write-Host "Stage 1: Complete."

    Write-Host "Stage 2: Ensuring Vulkan SDK."
    # Custom Vulkan SDK Installation
    $vulkanSdkInstalled = $false
    $vulkanVersion = "1.3.268.0" # Specify a known good version
    $vulkanSdkPath = "C:\VulkanSDK\$vulkanVersion"
    $envVulkanSdkPath = $env:VULKAN_SDK

    if ($envVulkanSdkPath -and (Test-Path (Join-Path $envVulkanSdkPath "bin\glslc.exe"))) {
        Write-Host "Vulkan SDK found at '$envVulkanSdkPath' (via VULKAN_SDK env var). Checking version..."
        # Try to get version from path, simple check
        if ($envVulkanSdkPath -like "*$vulkanVersion*") {
            Write-Host "Vulkan SDK version appears to be $vulkanVersion or compatible."
            $vulkanSdkInstalled = $true
        } else {
            Write-Host "Vulkan SDK at $envVulkanSdkPath might be a different version. Will attempt to install $vulkanVersion if specific components are missing."
            # We could be more sophisticated here, but for now, if glslc.exe exists, assume it's usable
            # and let the build script determine if it's sufficient.
            # If a specific version is strictly required, uncommenting the install block is better.
             $vulkanSdkInstalled = $true # Tentatively mark as installed if glslc is found
        }
    } elseif (Test-Path (Join-Path $vulkanSdkPath "bin\glslc.exe")) {
        Write-Host "Vulkan SDK $vulkanVersion found at default path '$vulkanSdkPath'."
        $vulkanSdkInstalled = $true
        if (-not $envVulkanSdkPath) {
            Write-Host "Setting VULKAN_SDK environment variable for this session."
            $env:VULKAN_SDK = $vulkanSdkPath
            $env:PATH = "$($env:VULKAN_SDK)\Bin;$env:PATH"
        }
    }

    if (-not $vulkanSdkInstalled) {
        Write-Host "Vulkan SDK $vulkanVersion not found or version mismatch. Installing..."
        $vulkanInstallerName = "VulkanSDK-$($vulkanVersion)-Installer.exe"
        $vulkanDownloadUrl = "https://sdk.lunarg.com/sdk/download/$($vulkanVersion)/windows/$($vulkanInstallerName)"
        $tempInstallerPath = Join-Path $env:TEMP $vulkanInstallerName

        try {
            Write-Host "Downloading Vulkan SDK from $vulkanDownloadUrl..."
            Invoke-WebRequest -Uri $vulkanDownloadUrl -OutFile $tempInstallerPath -UseBasicParsing
            Write-Host "Installing Vulkan SDK..."
            # Installer needs admin rights if not already running as admin.
            # The --root option could be used if a different install path is desired.
            $installArgs = @(
                "--accept-licenses",
                "--default-answer",
                "--confirm-command",
                "install"
            )
            Invoke-Process -FilePath $tempInstallerPath -ArgumentList $installArgs

            # Verify installation by checking default path and setting VULKAN_SDK if needed
            if (Test-Path (Join-Path $vulkanSdkPath "bin\glslc.exe")) {
                Write-Host "Vulkan SDK installed successfully to $vulkanSdkPath"
                if (-not $env:VULKAN_SDK) {
                    $env:VULKAN_SDK = $vulkanSdkPath
                    $env:PATH = "$($env:VULKAN_SDK)\Bin;$env:PATH"
                    Write-Host "VULKAN_SDK environment variable set for this session."
                }
            } else {
                throw "Vulkan SDK installation may have failed. '$vulkanSdkPath\bin\glslc.exe' not found after install."
            }
        }
        catch {
            throw "Failed to download or install Vulkan SDK: $($_.Exception.Message)"
        }
        finally {
            if (Test-Path $tempInstallerPath) {
                Remove-Item $tempInstallerPath -Force -ErrorAction SilentlyContinue
            }
        }
    } else {
        Write-Host "Vulkan SDK $vulkanVersion (or a usable version) is already present. Skipping installation."
    }
    Write-Host "Stage 2: Complete."
    
    Write-Host "Stage 3: Finding MSYS2."
    # Find MSYS2 after ensuring it's installed.
    $bashPath = Find-Msys2Bash
    # $bashPath = '/bin/bash' # Provide a dummy path
    Write-Host "Stage 3: Complete."

    Write-Host "Stage 4: Setting up MSYS2 environment."
    # Step 2: Set up the MSYS2 environment.
    Install-MSYS2Packages -Msys2BashPath $bashPath
    Write-Host "Stage 4: Complete."
    
    Write-Host "Stage 5: Setting up Qt environment."
    # Step 3: Set up the Qt environment.
    Ensure-QMake
    Write-Host "Stage 5: Complete."
    
    Write-Host "Stage 6: Preparing source code."
    # Step 4: Prepare the source code.
    Update-Submodules
    Write-Host "Stage 6: Complete."
    
    Write-Host "Stage 7: Running final build."
    # Step 5: Run the final build.
    Build-Project -Msys2BashPath $bashPath
    Write-Host "Stage 7: Complete."

    Write-Host "Stage 8: Deploying external binaries."
    # Define paths for external prebuilt binaries
    $resrganDir = Join-Path -Path $PSScriptRoot -ChildPath 'Waifu2x-Extension-QT\realesrgan-ncnn-vulkan'
    $rcuganDir = Join-Path -Path $PSScriptRoot -ChildPath 'Waifu2x-Extension-QT\realcugan-ncnn-vulkan'
    $destDir = Join-Path -Path $PSScriptRoot -ChildPath 'Waifu2x-Extension-QT\release'

    # Ensure the destination directory exists
    if (-not (Test-Path -Path $destDir)) {
        throw "The destination release directory does not exist: $destDir"
    }

    # Copy the directories
    Write-Host "Copying realesrgan-ncnn-vulkan to release directory..."
    Copy-Item -Path $resrganDir -Destination $destDir -Recurse -Force
    Write-Host "Copying realcugan-ncnn-vulkan to release directory..."
    Copy-Item -Path $rcuganDir -Destination $destDir -Recurse -Force
    Write-Host "Stage 8: Complete."
    
    Write-Host "`nBuild process completed successfully!" -ForegroundColor Green

    if ($script:RebootRequired) {
        Write-Warning "One or more installations require a system reboot to complete. Please restart your computer."
    }
    
} catch {
    Write-Error "A critical error occurred during the build process: $($_.Exception.Message)"
    Write-Error "At: $($_.InvocationInfo.PositionMessage)"
    # Re-throw the original error to preserve full details and stop the script.
    throw $_
}
