#ifndef ZTCPP_SERVICE_HPP
#define ZTCPP_SERVICE_HPP

#include <ZTCpp/Definitions.hpp>
#include <ZTCpp/Result.hpp>

#include <string>

// TODO Temporary
#define ZTS_ERR_OK            0 // No error
#define ZTS_ERR_SOCKET       -1 // Socket error, see zts_errno
#define ZTS_ERR_SERVICE      -2 // You probably did something at the wrong time
#define ZTS_ERR_ARG          -3 // Invalid argument
#define ZTS_ERR_NO_RESULT    -4 // No result (not necessarily an error)
#define ZTS_ERR_GENERAL      -5 // Consider filing a bug report

ZTCPP_NAMESPACE_BEGIN

// TODO: make RAII wrappers for this stuff

ZTCPP_API int StartZeroTierService(const std::string& aConfigFilePath, uint16_t aServicePort);
ZTCPP_API int StopZeroTierService();
ZTCPP_API int FreeZeroTierServiceResources();

ZTCPP_API int JoinNetwork(const uint64_t aNetworkId);

ZTCPP_NAMESPACE_END

#endif // !ZTCPP_SERVICE_HPP