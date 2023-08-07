
#include <ZTCpp/Service.hpp>
#include <ZTCpp/Events.hpp>

#include <cstdlib>

#include <ZeroTierSockets.h>

ZTCPP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////
// CONFIGURATION                                                         //
///////////////////////////////////////////////////////////////////////////

EmptyResult Config::setIdentityFromStorage(const std::string& aPath) {
  const auto res = zts_init_from_storage(aPath.c_str());

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could not configure ZeroTier service)")};
  }
  else if (res == ZTS_ERR_ARG) {
    return {ZTCPP_ERROR_REPORT(ArgumentError,
                               "ZTS_ERR_ARG (Invalid path)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_init_from_storage returned " +
                             std::to_string(res) + ")")};
}

EmptyResult Config::setIdentityFromMemory(const char* aKey, std::size_t aKeyLength) {
  const auto res = zts_init_from_memory(aKey, aKeyLength);

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could not configure ZeroTier service)")};
  }
  else if (res == ZTS_ERR_ARG) {
    return {ZTCPP_ERROR_REPORT(ArgumentError,
                               "ZTS_ERR_ARG (Invalid key)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_init_from_memory returned " +
                             std::to_string(res) + ")")};
}

EmptyResult Config::setPort(uint16_t aPort) {
  const auto res = zts_init_set_port(aPort);

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could not configure ZeroTier service)")};
  }
  else if (res == ZTS_ERR_ARG) {
    return {ZTCPP_ERROR_REPORT(ArgumentError,
                               "ZTS_ERR_ARG (Port rejected)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_init_set_port returned " +
                             std::to_string(res) + ")")};
}

EmptyResult Config::setRandomPortRange(uint16_t aStartPort, uint16_t aEndPort) {
  const auto res = zts_init_set_random_port_range(aStartPort, aEndPort);

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could not configure ZeroTier service)")};
  }
  else if (res == ZTS_ERR_ARG) {
    return {ZTCPP_ERROR_REPORT(ArgumentError,
                               "ZTS_ERR_ARG (Port range rejected)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_init_set_random_port_range returned " +
                             std::to_string(res) + ")")};
}

EmptyResult Config::allowSecondaryPort(bool aAllowed) {
  const auto res = zts_init_allow_secondary_port(static_cast<unsigned>(aAllowed));

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could not configure ZeroTier service)")};
  }
  else if (res == ZTS_ERR_ARG) {
    return {ZTCPP_ERROR_REPORT(ArgumentError,
                               "ZTS_ERR_ARG (Invalid argument)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_init_allow_secondary_port returned " +
                             std::to_string(res) + ")")};
}

EmptyResult Config::allowPortMapping(bool aAllowed) {
  const auto res = zts_init_allow_port_mapping(static_cast<unsigned>(aAllowed));

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could not configure ZeroTier service)")};
  }
  else if (res == ZTS_ERR_ARG) {
    return {ZTCPP_ERROR_REPORT(ArgumentError,
                               "ZTS_ERR_ARG (Invalid argument)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_init_allow_port_mapping returned " +
                             std::to_string(res) + ")")};
}

EmptyResult Config::allowNetworkCaching(bool aAllowed) {
  const auto res = zts_init_allow_net_cache(static_cast<unsigned>(aAllowed));

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could not configure ZeroTier service)")};
  }
  else if (res == ZTS_ERR_ARG) {
    return {ZTCPP_ERROR_REPORT(ArgumentError,
                               "ZTS_ERR_ARG (Invalid argument)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_init_allow_net_cache returned " +
                             std::to_string(res) + ")")};
}

EmptyResult Config::allowPeerCaching(bool aAllowed) {
  const auto res = zts_init_allow_peer_cache(static_cast<unsigned>(aAllowed));

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could not configure ZeroTier service)")};
  }
  else if (res == ZTS_ERR_ARG) {
    return {ZTCPP_ERROR_REPORT(ArgumentError,
                               "ZTS_ERR_ARG (Invalid argument)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_init_allow_peer_cache returned " +
                             std::to_string(res) + ")")};
}

EmptyResult Config::allowRootCaching(bool aAllowed) {
  const auto res = zts_init_allow_roots_cache(static_cast<unsigned>(aAllowed));

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could not configure ZeroTier service)")};
  }
  else if (res == ZTS_ERR_ARG) {
    return {ZTCPP_ERROR_REPORT(ArgumentError,
                               "ZTS_ERR_ARG (Invalid argument)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_init_allow_roots_cache returned " +
                             std::to_string(res) + ")")};
}

EmptyResult Config::allowIdentityCaching(bool aAllowed) {
  const auto res = zts_init_allow_id_cache(static_cast<unsigned>(aAllowed));

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Could not configure ZeroTier service)")};
  }
  else if (res == ZTS_ERR_ARG) {
    return {ZTCPP_ERROR_REPORT(ArgumentError,
                               "ZTS_ERR_ARG (Invalid argument)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_init_allow_id_cache returned " +
                             std::to_string(res) + ")")};
}

///////////////////////////////////////////////////////////////////////////
// LOCAL NODE                                                            //
///////////////////////////////////////////////////////////////////////////

void LocalNode::setEventHandler(EventHandlerInterface* aHandler) {
  detail::SetEventHandler(aHandler);
}

EventHandlerInterface* LocalNode::getEventHandler() {
  return detail::GetEventHandler();
  
}

EmptyResult LocalNode::start() {
  {
    const auto res = zts_init_set_event_handler(&detail::IntermediateEventHandler);

    if (res == ZTS_ERR_OK) {
      goto START_NODE;
    }
    else if (res == ZTS_ERR_ARG) {
      return {ZTCPP_ERROR_REPORT(ArgumentError,
                                 "ZTS_ERR_ARG (Could not set intermediate event handler)")};
    }
    else if (res == ZTS_ERR_SERVICE) {
      return {ZTCPP_ERROR_REPORT(ServiceError,
                                 "ZTS_ERR_SERVICE (Node encountered a problem while "
                                 "setting intermediate event handler)")};
    }

    return {ZTCPP_ERROR_REPORT(GenericError,
                               "Unknown error (zts_init_set_event_handler returned " +
                               std::to_string(res) + ")")};
  }
START_NODE:
  {
    const auto res = zts_node_start();

    if (res == ZTS_ERR_OK) {
      return EmptyResultOK();
    }
    else if (res == ZTS_ERR_SERVICE) {
      return {ZTCPP_ERROR_REPORT(ServiceError,
                                 "ZTS_ERR_SERVICE (Node encountered a problem while starting)")};
    }

    return {ZTCPP_ERROR_REPORT(GenericError,
                               "Unknown error (zts_node_start returned " +
                               std::to_string(res) + ")")};
  }
}

EmptyResult LocalNode::stop() {
  const auto res = zts_node_stop();

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Node encountered a problem while stopping)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_node_stop returned " +
                             std::to_string(res) + ")")};
}

EmptyResult LocalNode::freeResources() {
  const auto res = zts_node_free();

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Node encountered a problem while freeing resources)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_node_free returned " +
                             std::to_string(res) + ")")};
}

bool LocalNode::isOnline() {
  return static_cast<bool>(zts_node_is_online());
}

uint64_t LocalNode::getID() {
  return zts_node_get_id();
}

uint16_t LocalNode::getPort() {
  return zts_node_get_port();
}

///////////////////////////////////////////////////////////////////////////
// NETWORKS                                                              //
///////////////////////////////////////////////////////////////////////////

EmptyResult Network::join(uint64_t aNetworkId) {
  const auto res = zts_net_join(aNetworkId);

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Node encountered a problem or is not up)")};
  }
  else if (res == ZTS_ERR_ARG) {
    return {ZTCPP_ERROR_REPORT(ArgumentError,
                               "ZTS_ERR_ARG (Invalid argument for zts_net_join)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_net_join returned " + std::to_string(res) + ")")};
}

EmptyResult Network::leave(uint64_t aNetworkId) {
  const auto res = zts_net_leave(aNetworkId);

  if (res == ZTS_ERR_OK) {
    return EmptyResultOK();
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Node encountered a problem or is not up)")};
  }
  else if (res == ZTS_ERR_ARG) {
    return {ZTCPP_ERROR_REPORT(ArgumentError,
                               "ZTS_ERR_ARG (Invalid argument for zts_net_leave)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_net_leave returned " + std::to_string(res) + ")")};
}

bool Network::isTransportReady(uint64_t aNetworkId) {
  return static_cast<bool>(zts_net_transport_is_ready(aNetworkId));
}

bool Network::isBroadcastEnabled(uint64_t aNetworkId) {
  return static_cast<bool>(zts_net_get_broadcast(aNetworkId));
}

Result<std::string> Network::getName(uint64_t aNetworkId) {
  char charbuf[200];

  const auto res = zts_net_get_name(aNetworkId, charbuf, sizeof(charbuf) / sizeof(charbuf[0]));

  if (res == ZTS_ERR_OK) {
    return std::string{charbuf};
  }
  else if (res == ZTS_ERR_SERVICE) {
    return {ZTCPP_ERROR_REPORT(ServiceError,
                               "ZTS_ERR_SERVICE (Node encountered a problem or is not up)")};
  }
  else if (res == ZTS_ERR_ARG) {
    return {ZTCPP_ERROR_REPORT(ArgumentError,
                               "ZTS_ERR_ARG (Invalid argument for zts_net_get_name)")};
  }

  return {ZTCPP_ERROR_REPORT(GenericError,
                             "Unknown error (zts_net_get_name returned " + std::to_string(res) + ")")};
}

int Network::getStatus(uint64_t aNetworkId) {
  return zts_net_get_status(aNetworkId);
}

int Network::getType(uint64_t aNetworkId) {
  return zts_net_get_type(aNetworkId);
}

ZTCPP_NAMESPACE_END