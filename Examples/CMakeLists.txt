# Server example
add_executable("ztserver" "Source/ztserver.cpp")
target_link_libraries("ztserver" PUBLIC "ztcpp")

# Client example
add_executable("ztclient" "Source/ztclient.cpp")
target_link_libraries("ztclient" PUBLIC "ztcpp")
