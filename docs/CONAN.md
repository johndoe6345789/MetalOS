# Conan Package Manager Integration

MetalOS uses [Conan](https://conan.io/) as a package manager to manage C/C++ dependencies and build configurations.

> **Note**: This documentation uses Conan 2.x syntax and conventions. Conan 2.x is the modern version with improved performance and simplified package references.

## Overview

Conan provides:
- **Dependency management** - Handle third-party libraries and tools
- **Build configuration** - Generate CMake toolchains for reproducible builds
- **Package distribution** - Share MetalOS components as Conan packages
- **Cross-platform builds** - Consistent builds across different environments

## Custom Conan Repository

MetalOS has a **custom Conan repository (KernelCenter)** that hosts MetalOS-specific packages and configurations:

**Repository URL**: https://johndoe6345789.github.io/kernelcenter/

This custom repository provides:
- Pre-built MetalOS components and dependencies
- Custom package recipes optimized for minimal OS development
- Freestanding environment configurations
- GPU firmware packages and utilities

### Adding the Custom Repository

To use the custom Conan repository with your MetalOS development:

```bash
# Add the KernelCenter remote
conan remote add kernelcenter https://johndoe6345789.github.io/kernelcenter/

# List your configured remotes to verify
conan remote list

# Set KernelCenter as the primary remote (optional)
conan remote enable kernelcenter
```

### Using Packages from KernelCenter

Once the remote is added, you can install packages from it:

```bash
# Search for available packages
conan search "*" -r=kernelcenter

# Install specific packages (Conan 2.x syntax)
conan install --requires=<package>/<version> -r=kernelcenter
```

### Repository Information

You can view detailed information about the KernelCenter repository:

```bash
# Fetch repository information
curl https://johndoe6345789.github.io/kernelcenter/
# or
wget -q -O - https://johndoe6345789.github.io/kernelcenter/
```

The repository page contains:
- Available package list
- Package versions and compatibility
- Installation instructions
- Repository metadata and configuration

## Standard Conan Workflow

### Installation

```bash
# Install Conan via pip
pip3 install conan

# Verify installation
conan --version

# Create default profile
conan profile detect --force
```

### Profile Configuration

MetalOS includes a custom Conan profile at `conan_profile`:

```bash
# View the MetalOS profile
cat conan_profile

# Use the custom profile
conan install . --profile=conan_profile --build=missing
```

The profile configures:
- Target architecture (x86_64)
- Compiler settings (GCC/Clang)
- Build type (Release/Debug)
- OS settings (Linux freestanding)

### Installing Dependencies

```bash
# Install all MetalOS dependencies
conan install . --build=missing

# Install for specific build type
conan install . --build=missing -s build_type=Debug
conan install . --build=missing -s build_type=Release
```

This generates:
- `build/Release/generators/conan_toolchain.cmake` - CMake toolchain file
- `build/Release/generators/CMakeDeps.cmake` - CMake dependency files
- Build configuration files

### Building with Conan

```bash
# 1. Install dependencies
conan install . --build=missing

# 2. Configure CMake with Conan toolchain
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../build/Release/generators/conan_toolchain.cmake

# 3. Build
cmake --build .
```

Or with Ninja:

```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=../build/Release/generators/conan_toolchain.cmake -G Ninja
ninja
```

## Conan Package Definition

The `conanfile.py` defines MetalOS as a Conan package:

```python
class MetalOSConan(ConanFile):
    name = "metalos"
    version = "0.1.0"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "build_bootloader": [True, False],
        "build_kernel": [True, False],
        "build_tests": [True, False],
    }
```

### Package Options

Configure MetalOS components:

```bash
# Build only the kernel
conan install . -o build_bootloader=False -o build_tests=False

# Build everything
conan install . -o build_bootloader=True -o build_kernel=True -o build_tests=True
```

## Testing Conan Integration

Verify that Conan is properly configured:

```bash
# Run the integration test
./scripts/test-conan-build.sh
```

This script:
1. Checks Conan installation
2. Verifies profile configuration
3. Installs dependencies
4. Generates toolchain
5. Tests CMake configuration
6. Validates the integration

## Future Dependencies

MetalOS currently has no external dependencies (freestanding OS), but future phases will integrate:

### Phase 4 - GPU Support
```python
requires = (
    "mesa-radv/24.0.0",  # Available from KernelCenter
    "amd-gpu-firmware/[>=navi23]",  # GPU firmware package
)
```

### Phase 7 - QT6 Integration
```python
requires = (
    "qt/6.5.3",  # Minimal static build from KernelCenter
)
```

These packages will be available from the KernelCenter repository.

## Conan Commands Reference

### Remote Management

```bash
# Add remote
conan remote add <name> <url>

# List remotes
conan remote list

# Remove remote
conan remote remove <name>

# Enable/disable remote
conan remote enable <name>
conan remote disable <name>
```

### Profile Management

```bash
# Detect system profile
conan profile detect --force

# Show profile
conan profile show <name>

# List profiles
conan profile list

# Create/edit custom profile
conan profile new <name> --detect
```

### Package Management

```bash
# Search packages
conan search <pattern>
conan search <pattern> -r=<remote>

# Get package info
conan inspect <package>

# Install package
conan install <package>

# Remove package from cache
conan remove <package>
```

### Build Commands

```bash
# Install dependencies
conan install <path> --build=missing

# Create package
conan create <path>

# Export recipe
conan export <path> <user>/<channel>
```

## Advanced Configuration

### Custom Build Types

```bash
# Create custom build type profile
conan profile new metalos-optimized --detect

# Edit profile to add custom settings
conan profile update settings.compiler.optimization=aggressive metalos-optimized
```

### Multiple Configurations

```bash
# Generate configurations for multiple build types
conan install . -s build_type=Debug --build=missing
conan install . -s build_type=Release --build=missing
conan install . -s build_type=RelWithDebInfo --build=missing
```

### Offline Development

```bash
# Download all dependencies once
conan install . --build=missing

# Later builds use cached packages (no network needed)
conan install .
```

## Troubleshooting

### Conan Not Found

```bash
# Reinstall Conan
pip3 install --upgrade conan

# Add to PATH if needed
export PATH="$HOME/.local/bin:$PATH"
```

### Profile Issues

```bash
# Reset profile
rm -rf ~/.conan2/profiles/default
conan profile detect --force

# Or manually create profile
conan profile new default --detect
```

### Cache Issues

```bash
# Clear Conan cache
conan remove "*" -c

# Or remove specific package
conan remove "<package>/*" -c
```

### Remote Connection Issues

```bash
# Test remote connectivity
conan search "*" -r=kernelcenter

# Check remote configuration
conan remote list

# Re-add remote if needed
conan remote remove kernelcenter
conan remote add kernelcenter https://johndoe6345789.github.io/kernelcenter/
```

### Build Generation Issues

```bash
# Clean and regenerate
rm -rf build CMakeUserPresets.json
conan install . --build=missing

# Verify toolchain exists
ls -la build/Release/generators/conan_toolchain.cmake
```

## Integration with CI/CD

MetalOS Docker environment includes Conan for consistent CI builds:

```dockerfile
# Install Conan in Docker
RUN pip3 install conan && conan profile detect --force

# Add custom remote
RUN conan remote add kernelcenter https://johndoe6345789.github.io/kernelcenter/
```

This ensures all builds (local and CI) use identical dependency versions.

## Resources

- **Conan Documentation**: https://docs.conan.io/
- **KernelCenter Repository**: https://johndoe6345789.github.io/kernelcenter/
- **MetalOS Conan Profile**: `conan_profile` in repository root
- **MetalOS Package Definition**: `conanfile.py` in repository root
- **Integration Test**: `scripts/test-conan-build.sh`

## See Also

- [BUILD.md](BUILD.md) - Building MetalOS
- [DEVELOPMENT.md](DEVELOPMENT.md) - Development workflow
- [deps/README.md](../deps/README.md) - Dependency management
- [DOCKER.md](DOCKER.md) - Docker build environment
