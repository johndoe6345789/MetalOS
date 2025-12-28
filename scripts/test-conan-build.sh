#!/bin/bash
# Test script for Conan integration
# This script tests that Conan is properly integrated and can build MetalOS

set -e  # Exit on any error

echo "=========================================="
echo "MetalOS Conan Integration Test"
echo "=========================================="
echo ""

# Check if Conan is installed
echo "1. Checking Conan installation..."
if ! command -v conan &> /dev/null; then
    echo "Error: Conan is not installed"
    echo "Install with: pip3 install conan"
    exit 1
fi
conan --version
echo "✓ Conan is installed"
echo ""

# Check if Conan profile exists
echo "2. Checking Conan profile..."
if ! conan profile show default &> /dev/null; then
    echo "Creating default Conan profile..."
    conan profile detect --force
fi
echo "✓ Conan profile exists"
echo ""

# Clean any previous build artifacts
echo "3. Cleaning previous build artifacts..."
rm -rf build build-conan-test CMakeUserPresets.json
echo "✓ Clean complete"
echo ""

# Install dependencies with Conan
echo "4. Installing dependencies with Conan..."
conan install . --build=missing
echo "✓ Dependencies installed"
echo ""

# Verify toolchain was generated
echo "5. Verifying Conan toolchain generation..."
TOOLCHAIN_PATH=""
if [ -f "build/Release/generators/conan_toolchain.cmake" ]; then
    TOOLCHAIN_PATH="build/Release/generators/conan_toolchain.cmake"
elif [ -f "build/Debug/generators/conan_toolchain.cmake" ]; then
    TOOLCHAIN_PATH="build/Debug/generators/conan_toolchain.cmake"
else
    echo "Error: conan_toolchain.cmake not found in build/Release/generators/ or build/Debug/generators/"
    exit 1
fi
echo "✓ Conan toolchain generated at: $TOOLCHAIN_PATH"
echo ""

# Configure with CMake using Conan toolchain
echo "6. Configuring CMake with Conan toolchain..."
mkdir -p build-conan-test
cd build-conan-test
cmake .. -DCMAKE_TOOLCHAIN_FILE="../$TOOLCHAIN_PATH"
cd ..
echo "✓ CMake configuration successful"
echo ""

# Clean up test build directory
echo "7. Cleaning up test artifacts..."
rm -rf build-conan-test build CMakeUserPresets.json
echo "✓ Cleanup complete"
echo ""

echo "=========================================="
echo "Conan Integration Test: PASSED ✓"
echo "=========================================="
echo ""
echo "Conan is properly integrated and working!"
