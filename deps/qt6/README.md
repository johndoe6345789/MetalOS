# QT6 Framework

This directory will contain a minimal QT6 build for MetalOS.

## Overview

QT6 provides the application framework for the single MetalOS application. We use a minimal, statically-linked build with only essential modules.

## Required Modules

- QtCore - Core non-GUI functionality
- QtGui - GUI components
- QtWidgets - Widget toolkit

## Source

QT6 is available from the [Qt Project](https://download.qt.io/official_releases/qt/).

## Setup Instructions

```bash
# Download QT6 source
wget https://download.qt.io/official_releases/qt/6.5/6.5.3/single/qt-everywhere-src-6.5.3.tar.xz
tar xf qt-everywhere-src-6.5.3.tar.xz
cd qt-everywhere-src-6.5.3

# Configure minimal build for MetalOS
./configure -static -release \
    -no-dbus -no-ssl -no-cups -no-fontconfig \
    -no-feature-network -no-feature-sql \
    -prefix $(pwd)/../deps/qt6 \
    -opensource -confirm-license

# Build (this will take a while)
make -j$(nproc)
make install
```

## MetalOS-Specific Configuration

We configure QT6 with:
- Static linking (no shared libraries)
- Minimal feature set
- No networking, database, or printing support
- Custom platform plugin for MetalOS framebuffer

## License

QT6 is available under:
- LGPL v3
- GPL v2
- Commercial License

For MetalOS, we use LGPL v3. See QT documentation for license details.

## Version Tracking

Create a `VERSION` file documenting:
- QT version (e.g., `6.5.3`)
- Build configuration options
- Date built
- Any local patches

## Status

⚠️ **Not yet populated** - QT6 will be added during Phase 7 (QT6 Port)
