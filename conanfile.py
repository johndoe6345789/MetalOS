from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout

class MetalOSConan(ConanFile):
    name = "metalos"
    version = "0.1.0"
    
    # Project metadata
    license = "MIT"
    author = "MetalOS Contributors"
    url = "https://github.com/johndoe6345789/MetalOS"
    description = "Minimal OS for QT6 on AMD64 + RX 6600"
    topics = ("os", "kernel", "uefi", "qt6", "minimal")
    
    # Build settings
    settings = "os", "compiler", "build_type", "arch"
    
    # Build options
    options = {
        "with_tests": [True, False],
        "qemu_display": ["none", "gtk", "sdl"]
    }
    
    default_options = {
        "with_tests": True,
        "qemu_display": "none"
    }
    
    # Sources are in the same repo
    exports_sources = (
        "CMakeLists.txt",
        "bootloader/*",
        "kernel/*",
        "userspace/*",
        "tests/*",
        "scripts/*",
        "docs/*"
    )
    
    def layout(self):
        cmake_layout(self)
    
    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_TESTING"] = self.options.with_tests
        tc.variables["QEMU_DISPLAY"] = str(self.options.qemu_display)
        tc.generate()
    
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        
        if self.options.with_tests:
            cmake.test()
    
    def package(self):
        cmake = CMake(self)
        cmake.install()
    
    def package_info(self):
        self.cpp_info.libs = ["metalos"]
