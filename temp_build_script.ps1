$ErrorActionPreference = "Stop"
$OutputEncoding = [System.Text.Encoding]::UTF8

$env:QTDIR = "C:/tmp/Qt/6.5.2/mingw_64"
$env:PATH = "$($env:QTDIR)/bin;$($env:PATH);C:/tools/msys64/mingw64/bin"

# Log versions and paths for debugging
Write-Output "QTDIR: $env:QTDIR"
Write-Output "PATH: $env:PATH"
Get-Command qmake.exe | Select-Object -ExpandProperty Source | Write-Output
Get-Command mingw32-make.exe | Select-Object -ExpandProperty Source | Write-Output

Set-Location "./Waifu2x-Extension-QT" # Assuming the script is run from repo root
Write-Output "Current directory: $(Get-Location)"
Write-Output "Running qmake..."
qmake.exe Waifu2x-Extension-QT.pro -spec win32-g++ CONFIG+=debug CONFIG+=qml_debug CONFIG+=qtquickcompiler

Write-Output "Running mingw32-make clean..."
mingw32-make.exe clean

Write-Output "Running mingw32-make -j4..."
# Capture all streams (stdout and stderr) to the log file
$MakeOutput = mingw32-make.exe -j4 *>&1
$MakeOutput | Tee-Object -FilePath "../WinBuildErrorLog.md"

Write-Output "Build attempt finished. Check WinBuildErrorLog.md"
