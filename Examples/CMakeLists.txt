
add_executable("ztserver" "Source/ztserver.cpp")
target_link_libraries("ztserver" PUBLIC "CONAN_PKG::libzt")

add_executable("ztclient" "Source/ztclient.cpp")
target_link_libraries("ztclient" PUBLIC "ztcpp")

if(False)
  add_executable("testapp" "Source/Main.cpp")
  target_link_libraries("testapp" PUBLIC "ztcpp")
endif()