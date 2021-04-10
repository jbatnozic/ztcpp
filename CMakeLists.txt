# ===== PROJECT SETUP =====

cmake_minimum_required(VERSION 3.14)

project("ztcpp" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ===== CONAN SETUP =====

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# ===== ADD MODULES =====

add_subdirectory("ZTCpp")
add_subdirectory("Examples")