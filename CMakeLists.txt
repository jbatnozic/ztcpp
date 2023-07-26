# ===== PROJECT SETUP =====

cmake_minimum_required(VERSION 3.14)

project("ztcpp" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ===== ADD MODULES =====

add_subdirectory("ZTCpp")

if(NOT ZTCPP_SKIP_EXAMPLES)
  add_subdirectory("Examples")
endif()
