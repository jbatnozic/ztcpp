# ===== LIBRARY SETUP =====

find_package(libzt CONFIG REQUIRED)

set(ZTCPP_LIBRARY_NAME "ztcpp")

add_compile_definitions("ZTCPP_EXPORT")

# ===== ADD TARGET =====

add_library(${ZTCPP_LIBRARY_NAME} SHARED)

target_sources(${ZTCPP_LIBRARY_NAME}
PRIVATE
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
    "libzt::libzt"
)

target_sources(${ZTCPP_LIBRARY_NAME}
PUBLIC FILE_SET HEADERS
BASE_DIRS
    "Include/"
FILES
    "Include/ZTCpp.hpp"
    "Include/ZTCpp/Definitions.hpp"
    "Include/ZTCpp/Events.hpp"
    "Include/ZTCpp/Ip_address.hpp"
    "Include/ZTCpp/Result.hpp"
    "Include/ZTCpp/Service.hpp"
    "Include/ZTCpp/Socket.hpp"
)

install(
    TARGETS ${ZTCPP_LIBRARY_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    FILE_SET HEADERS
)
