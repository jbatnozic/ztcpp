cmake_minimum_required(VERSION 3.15)
project("ztcpp-conan-test-package" CXX)

find_package(ztcpp CONFIG REQUIRED)

add_executable(${PROJECT_NAME} test.cpp)
target_link_libraries(${PROJECT_NAME} ztcpp::ztcpp)
