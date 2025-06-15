$ErrorActionPreference = "Stop"
$OutputEncoding = [System.Text.Encoding]::UTF8

$env:QTDIR = "C:/tmp/Qt/6.5.2/mingw_64"
$env:PATH = "$env:QTDIR/bin;$env:PATH;C:/tools/msys64/mingw64/bin"

Set-Location "Waifu2x-Extension-QT"

# Run qmake to ensure the Makefile is up-to-date with the .pro file and recent .h changes
qmake.exe Waifu2x-Extension-QT.pro -spec win32-g++ CONFIG+=debug CONFIG+=qml_debug CONFIG+=qtquickcompiler

# Clean previous build artifacts
mingw32-make.exe clean

# Build the project and redirect all output (stdout and stderr) to WinBuildErrorLog.md
# The Tee-Object will also allow the subtask runner to see the output.
mingw32-make.exe -j4 *>&1 | Tee-Object -FilePath "../WinBuildErrorLog.md"

"Build attempt finished. Check WinBuildErrorLog.md for results."
