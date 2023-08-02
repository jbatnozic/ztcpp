from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain
from conan.tools.files import copy


class ZtcppConan(ConanFile):
    name = "ztcpp"
    version = "3.0.0"
    package_type = "library"

    # Optional metadata
    license = "MIT"
    author = "Jovan Batnožić <jovanbatnozic@hotmail.rs>"
    url = "https://github.com/jbatnozic/ztcpp"
    description = "User-friendly C++ wrapper over libzt's C interface"

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True],
        "fPIC": [True, False]
    }
    default_options = {
        "shared": True,
        "fPIC": True
    }

    exports_sources = ['CMakeLists.txt', 'ZTCpp/*']

    requires = "libzt/3.0.0"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        check_min_cppstd(self, "17")

        # Configure options
        if self.options.shared:
            self.options.rm_safe("fPIC")

        self.options["libzt"].shared = False

    def generate(self):
        cmake_deps = CMakeDeps(self)
        cmake_deps.generate()

        tc = CMakeToolchain(self)
        tc.variables["ZTCPP_SKIP_EXAMPLES"] = 1
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()     
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["ztcpp"]
        self.cpp_info.libdirs = ['lib']
        self.cpp_info.bindirs = ['bin']
        if not self.options.shared:
            self.cpp_info.defines = ["ZTCPP_STATIC"]
