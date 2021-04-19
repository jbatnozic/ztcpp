from conans import ConanFile, CMake, tools


class ZtcppConan(ConanFile):
    name = "ztcpp"
    version = "1.1.1"

    # Information for humans
    license = "MIT"
    author = "Jovan Batnožić <jovanbatnozic@hotmail.rs>"
    url = "https://github.com/jbatnozic/ztcpp"
    description = "User-friendly C++ wrapper over libzt's C interface"

    # Information for computers
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True],
        "fPIC": [True, False]
    }
    default_options = {
        "shared": True,
        "fPIC": True
    }
    generators = "cmake"

    exports_sources = ['CMakeLists.txt', 'ZTCpp/*']

    requires = "libzt/1.1.0@jbatnozic/stable"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        tools.check_min_cppstd(self, "17")
        self.options["libzt"].shared = False

    def build(self):
        cmake = CMake(self)
        cmake.definitions["ZTCPP_SKIP_EXAMPLES"] = 1
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*.hpp", dst="include", src="ZTCpp/Include")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["ztcpp"]
        if not self.options.shared:
            self.cpp_info.defines = ["ZTCPP_STATIC"]
