from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import copy
from os.path import join

class ZTCppRecipe(ConanFile):
    name = "ztcpp"
    version = "3.0.1"
    package_type = "library"

    # Metadata
    license = "MIT"
    author = "Jovan Batnožić <jovanbatnozic@hotmail.rs>"
    url = "https://github.com/jbatnozic/ztcpp"
    description = "User-friendly C++ wrapper over libzt's C interface"

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False]
    }
    default_options = {
        "shared": True,
        "fPIC": True
    }

    exports_sources = "CMakeLists.txt", "Include/*", "Source/*", "Examples/*"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        check_min_cppstd(self, "17")

        if self.options.shared:
            self.options.rm_safe("fPIC")

        self.options["libzt"].shared = False

    def requirements(self):
        self.requires("libzt/3.0.1@jbatnozic/stable")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        if not self.options.shared:
            tc.variables["ZTCPP_STATIC"] = 1
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

        copy(self, "*.dll",   self.build_folder, join(self.package_folder, "bin"), keep_path=False)
        copy(self, "*.a",     self.build_folder, join(self.package_folder, "lib"), keep_path=False)
        copy(self, "*.so",    self.build_folder, join(self.package_folder, "lib"), keep_path=False)
        copy(self, "*.lib",   self.build_folder, join(self.package_folder, "lib"), keep_path=False)
        copy(self, "*.dylib", self.build_folder, join(self.package_folder, "lib"), keep_path=False)

    def package_info(self):
        self.cpp_info.includedirs = ["Include"]
        self.cpp_info.libs = ["ztcpp"]
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.bindirs = ["bin"]
        if not self.options.shared:
            self.cpp_info.defines = ["ZTCPP_STATIC"]
