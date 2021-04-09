
#include "ztcpp.hpp"

#include <cstdlib>
#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <iostream> // TODO Temp.

#include <ZeroTierSockets.h>

#define ZTCPP_ASSERT(_expr_) assert(_expr_)

// TODO Temporary
#define ZTCPP_GENERIC_ERROR(_text_) \
  do{ std::cerr << _text_ << std::endl; std::abort(); }while(0)

namespace jbatnozic {
namespace ztcpp {

namespace {
const zts_node_details* DataToNodeDetails(const void* aData) {
  ZTCPP_ASSERT(aData);
  return static_cast<const zts_node_details*>(aData);
}
const zts_network_details* DataToNetworkDetails(const void* aData) {
  ZTCPP_ASSERT(aData);
  return static_cast<const zts_network_details*>(aData);
}
const zts_netif_details* DataToNetIFDetails(const void* aData) {
  ZTCPP_ASSERT(aData);
  return static_cast<const zts_netif_details*>(aData);
}
const zts_virtual_network_route* DataToVirtualNetworkRoute(const void* aData) {
  ZTCPP_ASSERT(aData);
  return static_cast<const zts_virtual_network_route*>(aData);
}
const zts_peer_details* DataToPeerDetails(const void* aData) {
  ZTCPP_ASSERT(aData);
  return static_cast<const zts_peer_details*>(aData);
}
const zts_addr_details* DataToAddrDetails(const void* aData) {
  ZTCPP_ASSERT(aData);
  return static_cast<const zts_addr_details*>(aData);
}

EventHandlers g_eventHandlers;
} // namespace

//////////////////////////////////////////////////////////////////////////////
// Dataclass methods                                                        //
//////////////////////////////////////////////////////////////////////////////

// *** NodeDetails ***

ZTCPP_API uint64_t ZTCPPCALL NodeDetails::getNodeID() const {
  return DataToNodeDetails(_data)->address;
}

ZTCPP_API uint16_t ZTCPPCALL NodeDetails::getPrimaryPort() const {
  return DataToNodeDetails(_data)->primaryPort;
}

ZTCPP_API uint16_t ZTCPPCALL NodeDetails::getSecondaryPort() const {
  return DataToNodeDetails(_data)->secondaryPort;
}

ZTCPP_API uint16_t ZTCPPCALL NodeDetails::getTertiaryPort() const {
  return DataToNodeDetails(_data)->tertiaryPort;
}

ZTCPP_API void ZTCPPCALL NodeDetails::getVersion(uint32_t& aMajor, uint32_t& aMinor, uint32_t& aRevision) const {
  const auto* nd = DataToNodeDetails(_data);
  aMajor = nd->versionMajor;
  aMinor = nd->versionMinor;
  aRevision = nd->versionRev;
}

// *** NetworkDetails ***

ZTCPP_API uint64_t ZTCPPCALL NetworkDetails::getNetworkID() const {
  return DataToNetworkDetails(_data)->nwid;
}

ZTCPP_API uint64_t ZTCPPCALL NetworkDetails::getMACAddress() const {
  return DataToNetworkDetails(_data)->mac;
}

ZTCPP_API std::string ZTCPPCALL NetworkDetails::getNetworkName() const {
  return std::string{DataToNetworkDetails(_data)->name};
}

ZTCPP_API VirtualNetworkStatus ZTCPPCALL NetworkDetails::getNetworkStatus() const {
  switch (DataToNetworkDetails(_data)->status) {
  case ZTS_NETWORK_STATUS_REQUESTING_CONFIGURATION:
    return VirtualNetworkStatus::RequestingConfiguration;

  case ZTS_NETWORK_STATUS_OK:
    return VirtualNetworkStatus::OK;

  case ZTS_NETWORK_STATUS_ACCESS_DENIED:
    return VirtualNetworkStatus::AccessDenied;

  case ZTS_NETWORK_STATUS_NOT_FOUND:
    return VirtualNetworkStatus::NotFound;

  case ZTS_NETWORK_STATUS_PORT_ERROR:
    return VirtualNetworkStatus::PortError;

  case ZTS_NETWORK_STATUS_CLIENT_TOO_OLD:
    return VirtualNetworkStatus::ClientTooOld;

  default:
    ZTCPP_GENERIC_ERROR("Invalid VirtualNetworkStatus value.");
  }
}

ZTCPP_API VirtualNetworkType ZTCPPCALL NetworkDetails::getNetworkType() const {
  switch (DataToNetworkDetails(_data)->type) {
  case ZTS_NETWORK_TYPE_PRIVATE: return VirtualNetworkType::Private;
  case ZTS_NETWORK_TYPE_PUBLIC:  return VirtualNetworkType::Public;
  default:
    ZTCPP_GENERIC_ERROR("Invalid VirtualNetworkType value.");
  }
}

ZTCPP_API uint32_t ZTCPPCALL NetworkDetails::getMaximumTransissionUnit() const {
  return DataToNetworkDetails(_data)->mtu;
}

ZTCPP_API bool ZTCPPCALL NetworkDetails::getDHCPAvailable() const {
  return (DataToNetworkDetails(_data)->dhcp != 0);
}

ZTCPP_API bool ZTCPPCALL NetworkDetails::getBridgeEnabled() const {
  return (DataToNetworkDetails(_data)->bridge != 0);
}

ZTCPP_API bool ZTCPPCALL NetworkDetails::getBroadcastEnabled() const {
  return (DataToNetworkDetails(_data)->broadcastEnabled != 0);
}

ZTCPP_API int ZTCPPCALL NetworkDetails::getLastPortError() const {
  return DataToNetworkDetails(_data)->portError;
}

ZTCPP_API uint64_t ZTCPPCALL NetworkDetails::getNetworkConfigurationRevision() const {
  return DataToNetworkDetails(_data)->netconfRevision;
}

ZTCPP_API uint32_t ZTCPPCALL NetworkDetails::getAssignedAddressCount() const {
  return DataToNetworkDetails(_data)->assignedAddressCount;
}

ZTCPP_API uint32_t ZTCPPCALL NetworkDetails::getRouteCount() const {
  return DataToNetworkDetails(_data)->routeCount;
}

ZTCPP_API uint32_t ZTCPPCALL NetworkDetails::getMulticastSubscriptionCount() const {
  return DataToNetworkDetails(_data)->multicastSubscriptionCount;
}

// *** NetworkStackDetails ***

// No methods for now

// *** NetIFDetails ***
// TODO

// *** VirtualNetworkRoute ***
// TODO

// *** PeerDetails ***

ZTCPP_API uint64_t ZTCPPCALL PeerDetails::getAddress() const {
  return DataToPeerDetails(_data)->address;
}

ZTCPP_API void ZTCPPCALL PeerDetails::getVersion(uint32_t& aMajor, uint32_t& aMinor, uint32_t& aRevision) const {
  const auto* pd = DataToPeerDetails(_data);
  aMajor = pd->versionMajor;
  aMinor = pd->versionMinor;
  aRevision = pd->versionRev;
}

ZTCPP_API std::chrono::milliseconds ZTCPPCALL PeerDetails::getLatency() const {
  return std::chrono::milliseconds{DataToPeerDetails(_data)->latency};
}

ZTCPP_API PeerRole ZTCPPCALL PeerDetails::getRole() const {
  switch (DataToPeerDetails(_data)->role) {
  case ZTS_PEER_ROLE_LEAF:   return PeerRole::Leaf;
  case ZTS_PEER_ROLE_MOON:   return PeerRole::Moon;
  case ZTS_PEER_ROLE_PLANET: return PeerRole::Planet;
  default:
    ZTCPP_GENERIC_ERROR("Invalid PeerRole value.");
  }
}

ZTCPP_API uint32_t ZTCPPCALL PeerDetails::getPathCount() const {
  return DataToPeerDetails(_data)->pathCount;
}

// *** AddressDetails ***
// TODO

//////////////////////////////////////////////////////////////////////////////
// Free functions                                                           //
//////////////////////////////////////////////////////////////////////////////

template <class taZTCppDataClass>
class PrivateDataSetter {
public:
  PrivateDataSetter(taZTCppDataClass& aDataHolder, const void* aDataPtr)
    : _dataHolder{aDataHolder}
    , _dataPtr{aDataPtr}
  {
  }

  void set() const {
    _dataHolder._data = _dataPtr;
  }

private:
  taZTCppDataClass& _dataHolder;
  const void* _dataPtr;
};

namespace {
template <class taZTCppDataClass, class taEventCodeType, class taZTDataStruct>
void InvokeEventHandler(int16_t aRawEventCode, const taZTDataStruct* aTZDataStruct) {
  taZTCppDataClass dataClass;
  PrivateDataSetter<taZTCppDataClass>{dataClass, aTZDataStruct}.set();

  auto& handlers = g_eventHandlers;

  if constexpr (std::is_same<taZTCppDataClass, NodeDetails>::value) {
    if (handlers.nodeDetailsHandler) {
      if (aTZDataStruct) {
        handlers.nodeDetailsHandler(static_cast<taEventCodeType>(aRawEventCode), &dataClass);
      }
      else {
        handlers.nodeDetailsHandler(static_cast<taEventCodeType>(aRawEventCode), nullptr);
      }
    }
  }
  if constexpr (std::is_same<taZTCppDataClass, NetworkDetails>::value) {
    if (handlers.networkDetailsHandler) {
      if (aTZDataStruct) {
        handlers.networkDetailsHandler(static_cast<taEventCodeType>(aRawEventCode), &dataClass);
      }
      else {
        handlers.networkDetailsHandler(static_cast<taEventCodeType>(aRawEventCode), nullptr);
      }
    }
  }
  if constexpr (std::is_same<taZTCppDataClass, NetworkStackDetails>::value) {
    if (handlers.networkStackDetailsHandler) {
      if (aTZDataStruct) {
        handlers.networkStackDetailsHandler(static_cast<taEventCodeType>(aRawEventCode), &dataClass);
      }
      else {
        handlers.networkStackDetailsHandler(static_cast<taEventCodeType>(aRawEventCode), nullptr);
      }
    }
  }
  if constexpr (std::is_same<taZTCppDataClass, NetIFDetails>::value) {
    if (handlers.netIFDetailsHandler) {
      if (aTZDataStruct) {
        handlers.netIFDetailsHandler(static_cast<taEventCodeType>(aRawEventCode), &dataClass);
      }
      else {
        handlers.netIFDetailsHandler(static_cast<taEventCodeType>(aRawEventCode), nullptr);
      }
    }
  }
  if constexpr (std::is_same<taZTCppDataClass, VirtualNetworkRoute>::value) {
    if (handlers.virtualNetworkRouteHandler) {
      if (aTZDataStruct) {
        handlers.virtualNetworkRouteHandler(static_cast<taEventCodeType>(aRawEventCode), &dataClass);
      }
      else {
        handlers.virtualNetworkRouteHandler(static_cast<taEventCodeType>(aRawEventCode), nullptr);
      }
    }
  }
  if constexpr (std::is_same<taZTCppDataClass, PeerDetails>::value) {
    if (handlers.peerDetailsHandler) {
      if (aTZDataStruct) {
        handlers.peerDetailsHandler(static_cast<taEventCodeType>(aRawEventCode), &dataClass);
      }
      else {
        handlers.peerDetailsHandler(static_cast<taEventCodeType>(aRawEventCode), nullptr);
      }
    }
  }
  if constexpr (std::is_same<taZTCppDataClass, AddressDetails>::value) {
    if (handlers.addressDetailsHandler) {
      if (aTZDataStruct) {
        handlers.addressDetailsHandler(static_cast<taEventCodeType>(aRawEventCode), &dataClass);
      }
      else {
        handlers.addressDetailsHandler(static_cast<taEventCodeType>(aRawEventCode), nullptr);
      }
    }
  }
}

void ZeroTierEventCallback(void* aData) {
  struct zts_stack_details_dummy {};

  const auto* data = static_cast<struct zts_callback_msg*>(aData);
  ZTCPP_ASSERT(data);

  switch (data->eventCode) {
#define INVOKE(_ztcpp_dataclass_type, _event_code_type_, _raw_event_code_, _pdatastruct_) \
  InvokeEventHandler<_ztcpp_dataclass_type, _event_code_type_>( \
    _raw_event_code_, _pdatastruct_); \
  break

  // Node events
  case ZTS_EVENT_NODE_UP:
    INVOKE(NodeDetails, EventCode::Node, data->eventCode, data->node);
  case ZTS_EVENT_NODE_ONLINE:
    INVOKE(NodeDetails, EventCode::Node, data->eventCode, data->node);
  case ZTS_EVENT_NODE_OFFLINE:
    INVOKE(NodeDetails, EventCode::Node, data->eventCode, data->node);
  case ZTS_EVENT_NODE_DOWN:
    INVOKE(NodeDetails, EventCode::Node, data->eventCode, data->node);
  case ZTS_EVENT_NODE_IDENTITY_COLLISION:
    INVOKE(NodeDetails, EventCode::Node, data->eventCode, data->node);
  case ZTS_EVENT_NODE_UNRECOVERABLE_ERROR:
    INVOKE(NodeDetails, EventCode::Node, data->eventCode, data->node);
  case ZTS_EVENT_NODE_NORMAL_TERMINATION:
    INVOKE(NodeDetails, EventCode::Node, data->eventCode, data->node);

  // Network events
  case ZTS_EVENT_NETWORK_NOT_FOUND:
    INVOKE(NetworkDetails, EventCode::Network, data->eventCode, data->network);
  case ZTS_EVENT_NETWORK_CLIENT_TOO_OLD:
    INVOKE(NetworkDetails, EventCode::Network, data->eventCode, data->network);
  case ZTS_EVENT_NETWORK_REQ_CONFIG:
    INVOKE(NetworkDetails, EventCode::Network, data->eventCode, data->network);
  case ZTS_EVENT_NETWORK_OK:
    INVOKE(NetworkDetails, EventCode::Network, data->eventCode, data->network);
  case ZTS_EVENT_NETWORK_ACCESS_DENIED:
    INVOKE(NetworkDetails, EventCode::Network, data->eventCode, data->network);
  case ZTS_EVENT_NETWORK_READY_IP4:
    INVOKE(NetworkDetails, EventCode::Network, data->eventCode, data->network);
  case ZTS_EVENT_NETWORK_READY_IP6:
    INVOKE(NetworkDetails, EventCode::Network, data->eventCode, data->network);
  case ZTS_EVENT_NETWORK_READY_IP4_IP6:
    INVOKE(NetworkDetails, EventCode::Network, data->eventCode, data->network);
  case ZTS_EVENT_NETWORK_DOWN:
    INVOKE(NetworkDetails, EventCode::Network, data->eventCode, data->network);
  case ZTS_EVENT_NETWORK_UPDATE:
    INVOKE(NetworkDetails, EventCode::Network, data->eventCode, data->network);

  // Network Stack events
  case ZTS_EVENT_STACK_UP:
    INVOKE(NetworkStackDetails, EventCode::NetworkStack, data->eventCode, (zts_stack_details_dummy*)nullptr);
  case ZTS_EVENT_STACK_DOWN:
    INVOKE(NetworkStackDetails, EventCode::NetworkStack, data->eventCode, (zts_stack_details_dummy*)nullptr);

  // lwIP netif events
  case ZTS_EVENT_NETIF_UP:
    INVOKE(NetIFDetails, EventCode::NetIF, data->eventCode, data->netif);
  case ZTS_EVENT_NETIF_DOWN:
    INVOKE(NetIFDetails, EventCode::NetIF, data->eventCode, data->netif);
  case ZTS_EVENT_NETIF_REMOVED:
    INVOKE(NetIFDetails, EventCode::NetIF, data->eventCode, data->netif);
  case ZTS_EVENT_NETIF_LINK_UP:
    INVOKE(NetIFDetails, EventCode::NetIF, data->eventCode, data->netif);
  case ZTS_EVENT_NETIF_LINK_DOWN:
    INVOKE(NetIFDetails, EventCode::NetIF, data->eventCode, data->netif);

  // Peer events
  case ZTS_EVENT_PEER_DIRECT:
    INVOKE(PeerDetails, EventCode::Peer, data->eventCode, data->peer);
  case ZTS_EVENT_PEER_RELAY:
    INVOKE(PeerDetails, EventCode::Peer, data->eventCode, data->peer);
  case ZTS_EVENT_PEER_UNREACHABLE:
    INVOKE(PeerDetails, EventCode::Peer, data->eventCode, data->peer);
  case ZTS_EVENT_PEER_PATH_DISCOVERED:
    INVOKE(PeerDetails, EventCode::Peer, data->eventCode, data->peer);
  case ZTS_EVENT_PEER_PATH_DEAD:
    INVOKE(PeerDetails, EventCode::Peer, data->eventCode, data->peer);

  // Route events
  case ZTS_EVENT_ROUTE_ADDED:
    INVOKE(VirtualNetworkRoute, EventCode::Route, data->eventCode, data->route);
  case ZTS_EVENT_ROUTE_REMOVED:
    INVOKE(VirtualNetworkRoute, EventCode::Route, data->eventCode, data->route);

  // Address events
  case ZTS_EVENT_ADDR_ADDED_IP4:
    INVOKE(AddressDetails, EventCode::Address, data->eventCode, data->addr);
  case ZTS_EVENT_ADDR_REMOVED_IP4:
    INVOKE(AddressDetails, EventCode::Address, data->eventCode, data->addr);
  case ZTS_EVENT_ADDR_ADDED_IP6:
    INVOKE(AddressDetails, EventCode::Address, data->eventCode, data->addr);
  case ZTS_EVENT_ADDR_REMOVED_IP6:
    INVOKE(AddressDetails, EventCode::Address, data->eventCode, data->addr);

  default:
    ZTCPP_GENERIC_ERROR("Unknown event code received");
  }
}
} // namespace

ZTCPP_API int ZTCPPCALL StartZT(const std::string& aPath, EventHandlers aEventHandlers, std::uint16_t aPort) {
  g_eventHandlers = std::move(aEventHandlers);
  return zts_start(aPath.c_str(), ZeroTierEventCallback, aPort);
}

ZTCPP_API int ZTCPPCALL StopZT() {
  return zts_stop();
}

ZTCPP_API int ZTCPPCALL FreeZT() {
  return zts_free();
}

ZTCPP_API int ZTCPPCALL JoinNetwork(const uint64_t aNetworkId) {
  return zts_join(aNetworkId);
}

ZTCPP_API void ZTCPPCALL ztcpp_central_cleanup() {
  zts_central_cleanup();
}

} // namespace ztcpp
} // namespace jbatnozic