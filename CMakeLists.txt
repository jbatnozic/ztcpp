cmake_minimum_required(VERSION 3.23)

project("ztcpp" LANGUAGES CXX)

find_package(libzt CONFIG REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

add_library(${PROJECT_NAME}
    "Source/Events.cpp"
    "Source/Ip_address.cpp"
    "Source/Service.cpp"
    "Source/Sockaddr_util.cpp"
    "Source/Socket.cpp"
)

target_compile_definitions(${PROJECT_NAME}
PRIVATE
    "ZTCPP_EXPORT"
)

target_include_directories(${PROJECT_NAME}
PUBLIC 
    "Include/"
)

target_link_libraries(${PROJECT_NAME}
PRIVATE 
    "libzt::libzt"
)

install(DIRECTORY "Include" DESTINATION .)

add_subdirectory("Examples")
