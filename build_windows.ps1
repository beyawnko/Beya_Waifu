# Requires PowerShell 5 or later
# Copyright (C) 2025  beyawnko

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Run-Command {
    param(
        [string]$Command
    )
    Write-Host ">> $Command"
    Invoke-Expression $Command
    if ($LASTEXITCODE -ne 0) {
        throw "Command failed with exit code $LASTEXITCODE: $Command"
    }
}

function Ensure-Chocolatey {
    if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
        Write-Host "Installing Chocolatey..."
        $install = 'Set-ExecutionPolicy Bypass -Scope Process -Force; '
        $install += '[System.Net.ServicePointManager]::SecurityProtocol = '
        $install += '[System.Net.ServicePointManager]::SecurityProtocol -bor 3072; '
        $install += 'iex ((New-Object System.Net.WebClient).DownloadString("https://community.chocolatey.org/install.ps1"))'
        Run-Command $install
    } else {
        Write-Host "Chocolatey already installed."
    }
}

function Ensure-ChocoPackage {
    param(
        [string]$PackageName
    )
    $installed = choco list --local-only --exact $PackageName | Select-String $PackageName
    if (-not $installed) {
        Run-Command "choco install -y $PackageName"
    } else {
        Write-Host "$PackageName already installed."
    }
}

function Install-MSYS2Packages {
    $bash = 'C:\\msys64\\usr\\bin\\bash.exe'
    if (-not (Test-Path $bash)) {
        throw "MSYS2 bash not found at $bash"
    }
    Run-Command "$bash -lc 'pacman -Syu --noconfirm'"
    Run-Command "$bash -lc 'pacman -Su --noconfirm'"
    Run-Command "$bash -lc 'pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-make mingw-w64-x86_64-cmake git'"
}

function Ensure-QMake {
    if (-not (Get-Command qmake -ErrorAction SilentlyContinue)) {
        Write-Host 'qmake not found. Installing Qt via aqtinstall...'
        Ensure-ChocoPackage 'python'
        Run-Command 'pip install --upgrade pip'
        Run-Command 'pip install aqtinstall'
        $qtDir = 'C:\\Qt'
        if (-not (Test-Path $qtDir)) {
            New-Item -ItemType Directory -Path $qtDir | Out-Null
        }
        Run-Command "aqt install-qt windows desktop 6.5.2 win64_mingw -O $qtDir"
        $env:PATH = "$qtDir\\6.5.2\\mingw_64\\bin;$env:PATH"
    } else {
        Write-Host 'qmake found.'
    }
}

function Update-Submodules {
    Run-Command 'git submodule update --init --recursive'
}

function Build-Project {
    $bash = 'C:\\msys64\\usr\\bin\\bash.exe'
    Run-Command "$bash -lc './build_projects.sh'"
}

try {
    Ensure-Chocolatey
    Ensure-ChocoPackage 'msys2'
    Ensure-ChocoPackage 'cmake'
    Ensure-ChocoPackage 'git'
    Install-MSYS2Packages
    Ensure-QMake
    Update-Submodules
    Build-Project
    Write-Host 'Build complete.'
} catch {
    Write-Error $_
    exit 1
}
