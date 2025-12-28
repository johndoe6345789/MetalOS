# QT6 Hello World Application

QT += core gui widgets

CONFIG += c++17

SOURCES += hello_world.cpp

TARGET = hello_world
TEMPLATE = app

# MetalOS specific configuration
# This would need significant work to actually run on MetalOS

QMAKE_CXXFLAGS += -ffreestanding
QMAKE_LFLAGS += -nostdlib

# Note: This is a placeholder .pro file
# Actual QT6 port to MetalOS would require:
# 1. QT Platform Abstraction (QPA) plugin for MetalOS
# 2. Custom event loop integration
# 3. Graphics backend using MetalOS framebuffer/GPU
# 4. Input device integration
