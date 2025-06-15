<#
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
    Version: 4.4
    Requires: PowerShell 5.1 or later.
    Must be run with Administrator privileges to install software.
#>
[CmdletBinding()]
param (
    # The version of Qt to install.
    [string]$QtVersion = '6.5.2',

    # The base directory for the Qt installation.
    [string]$QtDir = 'C:\Qt'
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
function Test-IsAdmin {
    $identity = [System.Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object System.Security.Principal.WindowsPrincipal($identity)
    return $principal.IsInRole([System.Security.Principal.WindowsBuiltInRole]::Administrator)
}


# --- Environment Setup Functions ---

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

function Ensure-QMake {
    $MyInvocation.MyCommand.Name | Write-Host -ForegroundColor Cyan

    $qmakeCmd = Get-Command qmake -ErrorAction SilentlyContinue
    $isInstallComplete = $false
    $qtBinDir = ''

    if ($qmakeCmd) {
        $qtBinDir = Split-Path $qmakeCmd.Path
        $qtMultimediaDllPath = Join-Path $qtBinDir "Qt6Multimedia.dll"
        if (Test-Path $qtMultimediaDllPath) {
            Write-Host "qmake and required Qt modules appear to be installed."
            $isInstallComplete = $true
            # Store path for the build function
            $script:QtBinPath = $qtBinDir
        } else {
            Write-Warning "qmake found, but Qt Multimedia module is missing. Re-running Qt installation to add missing modules."
        }
    }

    if (-not $isInstallComplete) {
        Write-Host "qmake not found or installation is incomplete. Installing Qt $QtVersion via aqtinstall..."
        Ensure-ChocoPackage 'python'
        
        Invoke-Process 'python' @('-m', 'pip', 'install', '--upgrade', 'pip')
        Invoke-Process 'python' @('-m', 'pip', 'install', 'aqtinstall')
        
        if (-not (Test-Path $QtDir)) {
            New-Item -ItemType Directory -Path $QtDir | Out-Null
        }
        
        # Only specify truly optional modules. The others are included by default.
        $qtModules = @('qtmultimedia')
        $qtInstallArgs = @('install-qt', 'windows', 'desktop', $QtVersion, 'win64_mingw', '-O', $QtDir, '-m') + $qtModules
        Invoke-Process 'aqt' $qtInstallArgs
        
        $qtBinPath = Join-Path -Path $QtDir -ChildPath "$QtVersion\mingw_64\bin"
        if (-not (Test-Path $qtBinPath)) {
            throw "Qt installation failed. Path not found: '$qtBinPath'"
        }
        
        Write-Host "Adding '$qtBinPath' to PATH for this session."
        $env:PATH = "$qtBinPath;$env:PATH"
        # Store the path for the build function to use inside bash.
        $script:QtBinPath = $qtBinPath

        if (-not (Get-Command qmake -ErrorAction SilentlyContinue)) {
            throw "Failed to find qmake even after installation and adding to PATH."
        }
    }
}

# --- Build Steps ---

function Update-Submodules {
    $MyInvocation.MyCommand.Name | Write-Host -ForegroundColor Cyan
    Write-Host "Updating Git submodules..."
    Invoke-Process 'git' @('submodule', 'update', '--init', '--recursive')
}

function Build-Project {
    param(
        [string]$Msys2BashPath
    )
    $MyInvocation.MyCommand.Name | Write-Host -ForegroundColor Cyan
    
    Write-Host "Running the main project build script..."
    
    $buildScriptName = './build_projects.sh'
    $scriptFullPath = Join-Path -Path (Get-Location).Path -ChildPath $buildScriptName
    if (-not (Test-Path -LiteralPath $scriptFullPath)) {
        throw "Build script not found at '$scriptFullPath'"
    }

    $currentDir = (Get-Location).Path
    $msysDir = ($currentDir -replace '\\', '/')
    
    # Define paths to the necessary bin directories for the build environment
    $mingwBinPath = 'C:\tools\msys64\mingw64\bin'
    $msysBinPath = 'C:\tools\msys64\usr\bin'

    # Convert paths to a format bash understands
    $msysMingwPath = (($mingwBinPath -replace '^([A-Za-z]):', '/$1') -replace '\\', '/')
    $msysUsrBinPath = (($msysBinPath -replace '^([A-Za-z]):', '/$1') -replace '\\', '/')

    # Build the path components
    $pathComponents = @(
        $msysMingwPath,
        $msysUsrBinPath
    )
    if (-not [string]::IsNullOrEmpty($script:QtBinPath)) {
        $msysQtPath = (($script:QtBinPath -replace '^([A-Za-z]):', '/$1') -replace '\\', '/')
        $pathComponents += $msysQtPath
    }

    # Quote each path component for safety in the bash command
    $pathPrefix = ($pathComponents | ForEach-Object { "`"$_`"" }) -join ':'

    # Construct the final command string using the -f format operator.
    # This is the most robust way to prevent PowerShell from expanding '$PATH' prematurely.
    $finalCommandString = 'export PATH={0}:$PATH && cd "{1}" && {2}' -f $pathPrefix, $msysDir, $buildScriptName
    
    Invoke-Process -FilePath $Msys2BashPath -ArgumentList @('-lc', $finalCommandString)
}

# --- Main Execution Logic ---

# 1. Check for Administrator privileges before doing anything else.
if (-not (Test-IsAdmin)) {
    Write-Error "This script must be run with Administrator privileges."
    exit 1
}

try {
    Write-Host "`n--- Starting Build Process ---`n"

    # Step 1: Ensure core dependencies are installed with Chocolatey.
    Ensure-Chocolatey
    Ensure-ChocoPackage 'msys2'
    Ensure-ChocoPackage 'cmake'
    Ensure-ChocoPackage 'git'
    
    # Find MSYS2 after ensuring it's installed.
    $bashPath = Find-Msys2Bash

    # Step 2: Set up the MSYS2 environment.
    Install-MSYS2Packages -Msys2BashPath $bashPath
    
    # Step 3: Set up the Qt environment.
    Ensure-QMake
    
    # Step 4: Prepare the source code.
    Update-Submodules
    
    # Step 5: Run the final build.
    Build-Project -Msys2BashPath $bashPath
    
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
