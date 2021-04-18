
#include <ZTCpp/Service.hpp>
#include <ZTCpp/Events.hpp>

#include <cstdlib>

#include <ZeroTierSockets.h>

ZTCPP_NAMESPACE_BEGIN

// Declaration of function defined in Events.cpp
void ZeroTierEventCallback(void* aCallbackMessage);

///////////////////////////////////////////////////////////////////////////
// CONFIGURATION                                                         //
///////////////////////////////////////////////////////////////////////////

ZTCPP_API EmptyResult AllowNetworkLocalStorage(bool aAllow) {
  const auto res = zts_disable_local_storage(static_cast<std::uint8_t>(!aAllow));

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could not configure ZeroTier service)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_disable_local_storage returned " +
                             std::to_string(res) + ")")};
}

ZTCPP_API EmptyResult AllowNetworkCaching(bool aAllow) {
  const auto res = zts_allow_network_caching(static_cast<std::uint8_t>(aAllow));

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could not configure ZeroTier service)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_allow_network_caching returned " +
                             std::to_string(res) + ")")};
}

ZTCPP_API EmptyResult AllowPeerCaching(bool aAllow) {
  const auto res = zts_allow_peer_caching(static_cast<std::uint8_t>(aAllow));

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could not configure ZeroTier service)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_allow_peer_caching returned " +
                             std::to_string(res) + ")")};
}

ZTCPP_API EmptyResult AllowLocalConf(bool aAllow) {
  const auto res = zts_allow_local_conf(static_cast<std::uint8_t>(aAllow));

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could not configure ZeroTier service)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_allow_local_conf returned " +
                             std::to_string(res) + ")")};

}

///////////////////////////////////////////////////////////////////////////
// SERVICE CONTROL                                                       //
///////////////////////////////////////////////////////////////////////////

ZTCPP_API EmptyResult StartService(const std::string& aConfigFilePath, uint16_t aServicePort) {
  const auto res = zts_start(aConfigFilePath.c_str(), ZeroTierEventCallback, aServicePort);

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could not start ZeroTier service)")};
  }
  else if (res == ZTS_ERR_ARG) {
    return {ZTCPP_ERROR_REPORT(ArgumentError,
                               "ZTS_ERR_ARG (Invalid argument for zts_start)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_start returned " + std::to_string(res) + ")")};
}

ZTCPP_API EmptyResult RestartService() {
  const auto res = zts_restart();

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could not restart the ZeroTier service)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_restart returned " + std::to_string(res) + ")")};
}

ZTCPP_API EmptyResult StopService() {
    const auto res = zts_stop();

    if (res == ZTS_ERR_OK) {
      return EmptyResultOK();
    }
    else if (res == ZTS_ERR_SERVICE) {
      return {ZTCPP_ERROR_REPORT(ServiceError,
                                 "ZTS_ERR_SERVICE (Error stopping ZeroTier service)")};
    }

    return {ZTCPP_ERROR_REPORT(GenericError,
                               "Unknown error (zts_stop returned " + std::to_string(res) + ")")};
}

ZTCPP_API EmptyResult FreeServiceResources() {
    const auto res = zts_free();

    if (res == ZTS_ERR_OK) {
      return EmptyResultOK();
    }
    else if (res == ZTS_ERR_SERVICE) {
      return {ZTCPP_ERROR_REPORT(ServiceError,
                                 "ZTS_ERR_SERVICE (Error freeing ZeroTier service resources)")};
    }

    return {ZTCPP_ERROR_REPORT(GenericError,
                               "Unknown error (zts_free returned " + std::to_string(res) + ")")};
}

///////////////////////////////////////////////////////////////////////////
// NETWROK MANAGEMENT                                                    //
///////////////////////////////////////////////////////////////////////////

ZTCPP_API EmptyResult JoinNetwork(const uint64_t aNetworkId) {
    const auto res = zts_join(aNetworkId);

    if (res == ZTS_ERR_OK) {
      return EmptyResultOK();
    }
    else if (res == ZTS_ERR_SERVICE) {
      return {ZTCPP_ERROR_REPORT(ServiceError,
                                 "ZTS_ERR_SERVICE (Could join network because ZeroTier service is not up)")};
    }
    else if (res == ZTS_ERR_ARG) {
      return {ZTCPP_ERROR_REPORT(ArgumentError,
                                 "ZTS_ERR_ARG (Invalid argument for zts_join)")};
    }

    return {ZTCPP_ERROR_REPORT(GenericError,
                               "Unknown error (zts_join returned " + std::to_string(res) + ")")};
}

ZTCPP_API EmptyResult LeaveNetwork(const uint64_t aNetworkId) {
  const auto res = zts_leave(aNetworkId);

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could leave network because ZeroTier service is not up)")};
  }
  else if (res == ZTS_ERR_ARG) {
    return {ZTCPP_ERROR_REPORT(ArgumentError,
                               "ZTS_ERR_ARG (Invalid argument for zts_leave)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_leave returned " + std::to_string(res) + ")")};
}

ZTCPP_NAMESPACE_END