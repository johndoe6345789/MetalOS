# Docker Build Environment for MetalOS

This document explains how to use Docker to build MetalOS with all required dependencies pre-configured.

## Why Docker?

Building MetalOS natively requires:
- Cross-compiler toolchain setup
- QEMU and UEFI firmware installation
- Dependency management (firmware blobs, Mesa RADV, QT6)
- Platform-specific configurations

Docker simplifies this by providing a pre-configured build environment that works consistently across:
- Linux (various distributions)
- macOS (with Docker Desktop)
- Windows (with Docker Desktop + WSL2)

## Prerequisites

Install Docker:
- **Linux**: [Docker Engine](https://docs.docker.com/engine/install/)
- **macOS/Windows**: [Docker Desktop](https://docs.docker.com/desktop/)

Verify installation:
```bash
docker --version
# Should output: Docker version 20.10.x or later
```

## Quick Start

### 1. Build the Docker Image

From the MetalOS repository root:

```bash
./scripts/docker-build.sh
```

This creates a `metalos-builder:latest` image with:
- Ubuntu 22.04 base
- Build tools (gcc, nasm, make, cmake, meson)
- QEMU with UEFI support
- OVMF firmware
- Python3 for build scripts

Build time: ~5-10 minutes (one-time setup)

### 2. Setup Dependencies

Download required firmware and dependencies:

```bash
# Setup all dependencies
./scripts/docker-run.sh scripts/setup-deps.sh all

# Or setup individually
./scripts/docker-run.sh scripts/setup-deps.sh firmware  # AMD GPU firmware
./scripts/docker-run.sh scripts/setup-deps.sh ovmf      # UEFI firmware
```

This downloads:
- **AMD GPU firmware blobs** (dimgrey_cavefish_*.bin) from linux-firmware repository
- **OVMF UEFI firmware** for QEMU testing

### 3. Build MetalOS

Build the bootloader and kernel:

```bash
./scripts/docker-run.sh make all
```

### 4. Test in QEMU

Run MetalOS in QEMU with UEFI firmware:

```bash
# Headless mode (serial console only)
./scripts/docker-run.sh make qemu

# With debug output
./scripts/docker-run.sh make qemu-debug

# Test UEFI firmware setup
./scripts/docker-run.sh make qemu-uefi-test
```

## Common Tasks

### Interactive Shell

Get a bash shell inside the container:

```bash
./scripts/docker-run.sh /bin/bash
```

From the shell, you can run any commands:
```bash
# Inside container
make all
make test
make qemu
scripts/setup-deps.sh firmware
```

Exit with `exit` or Ctrl+D.

### Clean Build

```bash
./scripts/docker-run.sh make clean
```

### Run Unit Tests

```bash
./scripts/docker-run.sh make test
```

### View Dependency Status

```bash
./scripts/docker-run.sh ls -la deps/firmware/
./scripts/docker-run.sh cat deps/firmware/VERSION
```

## Advanced Usage

### Custom Docker Image Name

Edit `scripts/docker-build.sh` and `scripts/docker-run.sh` to change:
```bash
IMAGE_NAME="my-custom-name"
IMAGE_TAG="v1.0"
```

### Rebuild Docker Image

After updating Dockerfile:

```bash
./scripts/docker-build.sh
```

Docker will cache layers and only rebuild changed parts.

### Running Specific Commands

The `docker-run.sh` script accepts any command:

```bash
# Run specific make target
./scripts/docker-run.sh make kernel

# Check QEMU version
./scripts/docker-run.sh qemu-system-x86_64 --version

# List files
./scripts/docker-run.sh ls -la build/

# Run a script
./scripts/docker-run.sh ./scripts/create_image.sh
```

### Volume Mounts

The source directory is mounted at `/metalos` inside the container:
- Changes made in the container are reflected on the host
- Build artifacts created in the container are accessible on the host
- No need to copy files in/out of the container

### GPU Access (Future)

For testing with host GPU (when GPU driver is implemented):

```bash
docker run --rm -it \
    --device=/dev/dri \
    -v "$(pwd):/metalos" \
    metalos-builder:latest \
    /bin/bash
```

## Troubleshooting

### Docker Permission Denied

On Linux, add your user to the docker group:
```bash
sudo usermod -aG docker $USER
# Log out and back in
```

### Cannot Connect to Docker Daemon

Ensure Docker daemon is running:
```bash
sudo systemctl start docker    # Linux
# Or start Docker Desktop       # macOS/Windows
```

### Build Fails with "No Space Left"

Docker has run out of disk space. Clean up:
```bash
docker system prune -a
```

### Container Files Owned by Root

This is normal. To change ownership:
```bash
sudo chown -R $USER:$USER build/
```

Or use Docker with user mapping:
```bash
docker run --rm -it \
    --user $(id -u):$(id -g) \
    -v "$(pwd):/metalos" \
    metalos-builder:latest \
    /bin/bash
```

### QEMU Display Issues

Docker containers run headless by default. For graphical QEMU:

1. On Linux with X11:
```bash
docker run --rm -it \
    -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v "$(pwd):/metalos" \
    metalos-builder:latest \
    make qemu QEMU_DISPLAY=gtk
```

2. On macOS/Windows: Use VNC or serial console instead

3. For now, use headless mode:
```bash
./scripts/docker-run.sh make qemu QEMU_DISPLAY=none
```

## CI/CD Integration

### GitHub Actions

Example workflow:

```yaml
name: Docker Build

on: [push, pull_request]

jobs:
  docker-build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      
      - name: Build Docker image
        run: ./scripts/docker-build.sh
      
      - name: Setup dependencies
        run: ./scripts/docker-run.sh scripts/setup-deps.sh all
      
      - name: Build MetalOS
        run: ./scripts/docker-run.sh make all
      
      - name: Run tests
        run: ./scripts/docker-run.sh make test
```

### GitLab CI

```yaml
build:
  image: docker:latest
  services:
    - docker:dind
  script:
    - ./scripts/docker-build.sh
    - ./scripts/docker-run.sh make all
    - ./scripts/docker-run.sh make test
```

## Docker Image Contents

The `metalos-builder` image includes:

### Build Tools
- gcc, g++, make, cmake, ninja, meson
- nasm (assembler)
- binutils (ld, as, objcopy, etc.)
- pkg-config

### Testing Tools
- qemu-system-x86_64
- OVMF UEFI firmware
- mtools, xorriso (image creation)

### Utilities
- git, wget, curl
- Python3
- vim, less, file

### Directory Structure
```
/metalos/
├── deps/           # Dependencies (mounted from host)
│   ├── firmware/   # AMD GPU firmware blobs
│   ├── ovmf/       # UEFI firmware
│   ├── mesa-radv/  # Mesa RADV driver
│   └── qt6/        # QT6 framework
├── bootloader/     # UEFI bootloader source
├── kernel/         # Kernel source
├── scripts/        # Build scripts
└── build/          # Build artifacts
```

## Next Steps

After setting up the Docker environment:

1. Review [docs/BUILD.md](BUILD.md) for detailed build instructions
2. Check [docs/DEVELOPMENT.md](DEVELOPMENT.md) for development workflow
3. See [docs/TESTING.md](TESTING.md) for testing procedures
4. Read [deps/README.md](../deps/README.md) for dependency details

## Comparison: Docker vs Native Build

| Aspect | Docker Build | Native Build |
|--------|-------------|--------------|
| Setup Time | 5-10 minutes (one-time) | 1-2 hours (toolchain setup) |
| Platform Support | Linux, macOS, Windows | Linux (primary), macOS (limited) |
| Consistency | Identical across platforms | Platform-dependent |
| Isolation | Complete isolation | Uses system tools |
| Disk Space | ~2GB for image | ~500MB for tools |
| Performance | Slight overhead | Native speed |
| CI/CD | Easy integration | Platform-specific config |

## Getting Help

- **Documentation**: Check [docs/](.) directory
- **Issues**: [GitHub Issues](https://github.com/johndoe6345789/MetalOS/issues)
- **Discussions**: Use GitHub Discussions for questions
