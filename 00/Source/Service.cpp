
#include <ZTCpp/Service.hpp>

#include <ZTCpp/Events.hpp>

#include <ZeroTierSockets.h>

ZTCPP_NAMESPACE_BEGIN

// Declaration of function defined in Events.cpp
void ZeroTierEventCallback(void* aCallbackMessage);

ZTCPP_API int StartZeroTierService(const std::string& aConfigFilePath, uint16_t aServicePort) {
  return zts_start(aConfigFilePath.c_str(), ZeroTierEventCallback, aServicePort);
}

ZTCPP_API int StopZeroTierService() {
  return zts_stop();
}

ZTCPP_API int FreeZeroTierServiceResources() {
  return zts_free();
}

ZTCPP_API int JoinNetwork(const uint64_t aNetworkId) {
  return zts_join(aNetworkId);
}

ZTCPP_NAMESPACE_END