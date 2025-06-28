#!/bin/bash
# Exit immediately if a command exits with a non-zero status.
# set -e # Temporarily disable 'set -e' for debugging PATH and qmake
echo "--- Executing inside temporary bash wrapper (with debugging) ---"

echo "Initial PATH within wrapper:"
echo "-------------------------------------"
echo $PATH | tr ':' '
'
echo "-------------------------------------"

# Path to Qt, as passed from PowerShell, converted by Convert-PathToMsys
echo "Expected Qt bin directory (from PowerShell): /e/tmp/Qt/6.6.3/mingw_64/bin"
echo "Contents of expected Qt bin directory:"
ls -al "/e/tmp/Qt/6.6.3/mingw_64/bin" || echo "Could not list contents of /e/tmp/Qt/6.6.3/mingw_64/bin"
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

# Set the path for the build environment. The dollar sign in $PATH is escaped with a backtick ( )
# to prevent PowerShell from expanding it prematurely.
export PATH='/c/tools/msys64/mingw64/bin:/c/tools/msys64/usr/bin:/e/tmp/Qt/6.6.3/mingw_64/bin':$PATH

# Change to the project directory.
cd '/e/GitHub/Beya_Waifu'

# Execute the main build script.
././build_projects.sh

echo "--- Finished executing inside temporary bash wrapper ---"
