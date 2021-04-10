# ===== LIBRARY SETUP =====

set(ZTCPP_LIBRARY_NAME "ztcpp")

add_compile_definitions("ZTCPP_EXPORT")

# ===== ADD TARGET =====

add_library(${ZTCPP_LIBRARY_NAME}
SHARED
  "Source/Events.cpp"
  "Source/Ip_address.cpp"
  "Source/Service.cpp"
  "Source/Sockaddr_util.cpp"
  "Source/Socket.cpp"
)

target_include_directories(${ZTCPP_LIBRARY_NAME}
PUBLIC
  "Include/"
)

target_link_libraries(${ZTCPP_LIBRARY_NAME}
PRIVATE
  "CONAN_PKG::libzt"
)