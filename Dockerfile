# MetalOS Docker Build Environment
# This container includes all dependencies needed to build MetalOS

FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Set working directory
WORKDIR /metalos

# Install base build tools and dependencies
RUN apt-get update && apt-get install -y \
    # Build essentials
    build-essential \
    gcc \
    g++ \
    nasm \
    make \
    cmake \
    ninja-build \
    meson \
    pkg-config \
    git \
    wget \
    curl \
    ca-certificates \
    # QEMU and UEFI firmware
    qemu-system-x86 \
    ovmf \
    # Image creation tools
    mtools \
    xorriso \
    dosfstools \
    # Python for build scripts
    python3 \
    python3-pip \
    # Additional utilities
    vim \
    less \
    file \
    && rm -rf /var/lib/apt/lists/*

# Create directory structure for dependencies
RUN mkdir -p /metalos/deps/firmware \
    /metalos/deps/ovmf \
    /metalos/deps/mesa-radv \
    /metalos/deps/qt6 \
    /metalos/scripts

# Set up OVMF firmware in deps directory
RUN cp /usr/share/OVMF/OVMF_CODE.fd /metalos/deps/ovmf/ 2>/dev/null || \
    cp /usr/share/ovmf/OVMF.fd /metalos/deps/ovmf/ 2>/dev/null || \
    echo "OVMF firmware will be installed by setup script"

# Set environment variables
ENV PATH="/metalos/tools:${PATH}"
ENV METALOS_ROOT="/metalos"

# Default command
CMD ["/bin/bash"]
