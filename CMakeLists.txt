# ===== PROJECT SETUP =====

cmake_minimum_required(VERSION 3.14)

project("zt-test" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ===== CONAN SETUP =====

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

message(STATUS " CONAN_LIBS=${CONAN_LIBS}")

# ===== CONFIG =====

set(ZTCPP_LIBRARY_NAME "ztcpp")
set(ZTCPP_TESTAPP_NAME "ztcpp-testapp")

# ===== LIBRARY =====

add_library(${ZTCPP_LIBRARY_NAME} SHARED
  "ztcpp/source/Ztcpp.cpp"
)

target_include_directories(${ZTCPP_LIBRARY_NAME}
PUBLIC
  "ztcpp/include/"
)

target_link_libraries(${ZTCPP_LIBRARY_NAME}
PRIVATE
  ${CONAN_LIBS}
)

# ===== TEST APP =====

add_executable(${ZTCPP_TESTAPP_NAME}
  "testapp/Main.cpp"
)

target_link_libraries(${ZTCPP_TESTAPP_NAME}
PUBLIC
  ${ZTCPP_LIBRARY_NAME}
)