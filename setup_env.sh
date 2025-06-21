#!/bin/bash
set -eux

# Navigate to the application directory if mounted at /app
if [ -d "/app" ]; then
  cd /app
else
  echo "Warning: /app directory not found. Assuming current directory is the project root."
fi

# Allow pip to install packages globally
export PIP_BREAK_SYSTEM_PACKAGES=1

# Install Python dependencies
if [ -f "requirements.txt" ]; then
  python3 -m pip install -r requirements.txt
else
  echo "Error: requirements.txt not found."
  exit 1
fi

# Install system dependencies for Debian/Ubuntu
# Update package lists
apt-get update -y

# Install build tools and C++ compiler
apt-get install -y --no-install-recommends \
  build-essential \
  g++ \
  make \
  cmake

# Install Qt6 development libraries
apt-get install -y --no-install-recommends \
  qt6-base-dev \
  qt6-base-dev-tools \
  qt6-multimedia-dev \
  qt6-5compat-dev \
  qt6-shadertools-dev

# Install FFmpeg (optional, but listed as a dependency)
apt-get install -y --no-install-recommends ffmpeg

# Clean up apt cache
apt-get clean
rm -rf /var/lib/apt/lists/*

echo "Environment setup complete."
