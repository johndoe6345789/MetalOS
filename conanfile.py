"""
MetalOS Conan Package Configuration

This file defines the dependencies and build configuration for MetalOS.
Currently, MetalOS is a freestanding OS with no external dependencies,
but this file is prepared for future use when we integrate:
- Mesa RADV (GPU driver)
- QT6 (application framework)
- Other system libraries
"""

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout


class MetalOSConan(ConanFile):
    name = "metalos"
    version = "0.1.0"
    license = "MIT"
    author = "MetalOS Contributors"
    url = "https://github.com/johndoe6345789/MetalOS"
    description = "Minimal OS for QT6 applications on AMD64 + Radeon RX 6600"
    topics = ("os", "uefi", "minimal", "qt6", "gpu")
    
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "build_bootloader": [True, False],
        "build_kernel": [True, False],
        "build_tests": [True, False],
    }
    default_options = {
        "build_bootloader": True,
        "build_kernel": True,
        "build_tests": True,
    }
    
    # Specify which generator to use (cmake, make, ninja, etc.)
    generators = "CMakeDeps"
    
    # No external dependencies yet (freestanding OS)
    # Future dependencies will be added here:
    # requires = (
    #     "qt/6.5.3",  # When we port QT6 (from KernelCenter)
    #     "mesa-radv/24.0.0",  # When we integrate Mesa RADV (from KernelCenter)
    # )
    
    def layout(self):
        cmake_layout(self)
    
    def generate(self):
        tc = CMakeToolchain(self)
        # Pass options to CMake
        tc.variables["BUILD_BOOTLOADER"] = self.options.build_bootloader
        tc.variables["BUILD_KERNEL"] = self.options.build_kernel
        tc.variables["BUILD_TESTS"] = self.options.build_tests
        tc.generate()
    
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
    
    def package(self):
        cmake = CMake(self)
        cmake.install()
    
    def package_info(self):
        self.cpp_info.libs = []  # MetalOS doesn't provide libraries
        self.cpp_info.bindirs = ["boot", "boot/efi/EFI/BOOT"]
        
        # Set environment variables for tools that need to find our binaries
        self.buildenv_info.append_path("PATH", self.package_folder)
