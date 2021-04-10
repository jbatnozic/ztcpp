
#include <ZTCpp/Events.hpp>

#include "Sockaddr_util.hpp"

#include <cassert>
#include <sstream>
#include <string>
#include <type_traits>

#include <ZeroTierSockets.h>

#define ZTCPP_ASSERT(_expr_) assert(_expr_)

ZTCPP_NAMESPACE_BEGIN

namespace {

const zts_addr_details* DataToAddressDetails(const void* aData) {
  ZTCPP_ASSERT(aData);
  return static_cast<const zts_addr_details*>(aData);
}

const zts_network_details* DataToNetworkDetails(const void* aData) {
  ZTCPP_ASSERT(aData);
  return static_cast<const zts_network_details*>(aData);
}

const zts_netif_details* DataToNetworkInterfaceDetails(const void* aData) {
  ZTCPP_ASSERT(aData);
  return static_cast<const zts_netif_details*>(aData);
}

const zts_node_details* DataToNodeDetails(const void* aData) {
  ZTCPP_ASSERT(aData);
  return static_cast<const zts_node_details*>(aData);
}

const zts_peer_details* DataToPeerDetails(const void* aData) {
  ZTCPP_ASSERT(aData);
  return static_cast<const zts_peer_details*>(aData);
}

const zts_virtual_network_route* DataToRouteDetails(const void* aData) {
  ZTCPP_ASSERT(aData);
  return static_cast<const zts_virtual_network_route*>(aData);
}

} // namespace

//////////////////////////////////////////////////////////////////////////////
// Event detail classes                                                     //
//////////////////////////////////////////////////////////////////////////////

// *** AddressDetails ***

uint64_t AddressDetails::getNetworkID() const {
  return DataToAddressDetails(_data)->nwid;
}

IpAddress AddressDetails::getIpAddress() const {
  IpAddress result;
  uint16_t dummyPort;
  detail::ToIpAddressAndPort(&(DataToAddressDetails(_data)->addr), result, dummyPort);
  return result;
}

// *** NetworkDetails ***

uint64_t NetworkDetails::getNetworkID() const {
  return DataToNetworkDetails(_data)->nwid;
}

uint64_t NetworkDetails::getMACAddress() const {
  return DataToNetworkDetails(_data)->mac;
}

std::string NetworkDetails::getNetworkName() const {
  return std::string{DataToNetworkDetails(_data)->name};
}

VirtualNetworkStatus NetworkDetails::getNetworkStatus() const {
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
    return static_cast<VirtualNetworkStatus>(-1);
  }
}

VirtualNetworkType NetworkDetails::getNetworkType() const {
  switch (DataToNetworkDetails(_data)->type) {
  case ZTS_NETWORK_TYPE_PRIVATE: return VirtualNetworkType::Private;
  case ZTS_NETWORK_TYPE_PUBLIC:  return VirtualNetworkType::Public;
  default:
    return static_cast<VirtualNetworkType>(-1);
  }
}

uint32_t NetworkDetails::getMaximumTransissionUnit() const {
  return DataToNetworkDetails(_data)->mtu;
}

bool NetworkDetails::getDHCPAvailable() const {
  return (DataToNetworkDetails(_data)->dhcp != 0);
}

bool NetworkDetails::getBridgeEnabled() const {
  return (DataToNetworkDetails(_data)->bridge != 0);
}

bool NetworkDetails::getBroadcastEnabled() const {
  return (DataToNetworkDetails(_data)->broadcastEnabled != 0);
}

int NetworkDetails::getLastPortError() const {
  return DataToNetworkDetails(_data)->portError;
}

uint64_t NetworkDetails::getNetworkConfigurationRevision() const {
  return DataToNetworkDetails(_data)->netconfRevision;
}

uint32_t NetworkDetails::getAssignedAddressCount() const {
  return DataToNetworkDetails(_data)->assignedAddressCount;
}

uint32_t NetworkDetails::getRouteCount() const {
  return DataToNetworkDetails(_data)->routeCount;
}

uint32_t NetworkDetails::getMulticastSubscriptionCount() const {
  return DataToNetworkDetails(_data)->multicastSubscriptionCount;
}

// *** NetworkInterfaceDetails ***
// TODO

// *** NetworkStackDetails ***

// No methods for now

// *** NodeDetails ***

uint64_t NodeDetails::getNodeID() const {
  return DataToNodeDetails(_data)->address;
}

uint16_t NodeDetails::getPrimaryPort() const {
  return DataToNodeDetails(_data)->primaryPort;
}

uint16_t NodeDetails::getSecondaryPort() const {
  return DataToNodeDetails(_data)->secondaryPort;
}

uint16_t NodeDetails::getTertiaryPort() const {
  return DataToNodeDetails(_data)->tertiaryPort;
}

void NodeDetails::getVersion(uint32_t& aMajor, uint32_t& aMinor, uint32_t& aRevision) const {
  const auto* nd = DataToNodeDetails(_data);
  aMajor = nd->versionMajor;
  aMinor = nd->versionMinor;
  aRevision = nd->versionRev;
}

// *** PeerDetails ***

uint64_t PeerDetails::getAddress() const {
  return DataToPeerDetails(_data)->address;
}

void PeerDetails::getVersion(uint32_t& aMajor, uint32_t& aMinor, uint32_t& aRevision) const {
  const auto* pd = DataToPeerDetails(_data);
  aMajor = pd->versionMajor;
  aMinor = pd->versionMinor;
  aRevision = pd->versionRev;
}

std::chrono::milliseconds PeerDetails::getLatency() const {
  return std::chrono::milliseconds{DataToPeerDetails(_data)->latency};
}

PeerRole PeerDetails::getRole() const {
  switch (DataToPeerDetails(_data)->role) {
  case ZTS_PEER_ROLE_LEAF:   return PeerRole::Leaf;
  case ZTS_PEER_ROLE_MOON:   return PeerRole::Moon;
  case ZTS_PEER_ROLE_PLANET: return PeerRole::Planet;
  default:
    return static_cast<PeerRole>(-1);
  }
}

uint32_t PeerDetails::getPathCount() const {
  return DataToPeerDetails(_data)->pathCount;
}

// *** RouteDetails ***
// TODO

//////////////////////////////////////////////////////////////////////////////
// Event dispatching                                                        //
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

template <class taZTCppDetailClass, class taEventCodeType, class taZTDataStruct>
void HandleEvent(IEventHandler& aHandler, int16_t aRawEventCode, const taZTDataStruct* aTZDataStruct) {
  taZTCppDetailClass detailClass;
  PrivateDataSetter<taZTCppDetailClass>{detailClass, aTZDataStruct}.set();

  // Address event
  if constexpr (std::is_same<taZTCppDetailClass, AddressDetails>::value) {
    if (aTZDataStruct) {
      aHandler.onAddressEvent(static_cast<taEventCodeType>(aRawEventCode), &detailClass);
    }
    else {
      aHandler.onAddressEvent(static_cast<taEventCodeType>(aRawEventCode), nullptr);
    }
  }

  // Network event
  if constexpr (std::is_same<taZTCppDetailClass, NetworkDetails>::value) {
    if (aTZDataStruct) {
      aHandler.onNetworkEvent(static_cast<taEventCodeType>(aRawEventCode), &detailClass);
    }
    else {
      aHandler.onNetworkEvent(static_cast<taEventCodeType>(aRawEventCode), nullptr);
    }
  }

  // NetworkInterface event
  if constexpr (std::is_same<taZTCppDetailClass, NetworkInterfaceDetails>::value) {
    if (aTZDataStruct) {
      aHandler.onNetworkInterfaceEvent(static_cast<taEventCodeType>(aRawEventCode), &detailClass);
    }
    else {
      aHandler.onNetworkInterfaceEvent(static_cast<taEventCodeType>(aRawEventCode), nullptr);
    }
  }

  // NetworkStack event
  if constexpr (std::is_same<taZTCppDetailClass, NetworkStackDetails>::value) {
    if (aTZDataStruct) {
     aHandler.onNetworkStackEvent(static_cast<taEventCodeType>(aRawEventCode), &detailClass);
    }
    else {
      aHandler.onNetworkStackEvent(static_cast<taEventCodeType>(aRawEventCode), nullptr);
    }
  }

  // Node event
  if constexpr (std::is_same<taZTCppDetailClass, NodeDetails>::value) {
    if (aTZDataStruct) {
      aHandler.onNodeEvent(static_cast<taEventCodeType>(aRawEventCode), &detailClass);
    }
    else {
      aHandler.onNodeEvent(static_cast<taEventCodeType>(aRawEventCode), nullptr);
    }
  }

  // Peer event
  if constexpr (std::is_same<taZTCppDetailClass, PeerDetails>::value) {
    if (aTZDataStruct) {
      aHandler.onPeerEvent(static_cast<taEventCodeType>(aRawEventCode), &detailClass);
    }
    else {
      aHandler.onPeerEvent(static_cast<taEventCodeType>(aRawEventCode), nullptr);
    }
  }

  // Route event
  if constexpr (std::is_same<taZTCppDetailClass, RouteDetails>::value) {
    if (aTZDataStruct) {
      aHandler.onRouteEvent(static_cast<taEventCodeType>(aRawEventCode), &detailClass);
    }
    else {
      aHandler.onRouteEvent(static_cast<taEventCodeType>(aRawEventCode), nullptr);
    }
  }
}

void ZeroTierEventCallback(void* aCallbackMessage) {
  const auto* data = static_cast<struct zts_callback_msg*>(aCallbackMessage);
  if (!data) {
    return;
  }

  auto* eventHandler = GetEventHandler();
  if (!eventHandler) {
    return;
  }

  switch (data->eventCode) {
  // Node events
  case ZTS_EVENT_NODE_UP:
  case ZTS_EVENT_NODE_ONLINE:
  case ZTS_EVENT_NODE_OFFLINE:
  case ZTS_EVENT_NODE_DOWN:
  case ZTS_EVENT_NODE_IDENTITY_COLLISION:
  case ZTS_EVENT_NODE_UNRECOVERABLE_ERROR:
  case ZTS_EVENT_NODE_NORMAL_TERMINATION:
    HandleEvent<NodeDetails, EventCode::Node>(*eventHandler, data->eventCode, data->node);
    break;

  // Network events
  case ZTS_EVENT_NETWORK_NOT_FOUND:
  case ZTS_EVENT_NETWORK_CLIENT_TOO_OLD:
  case ZTS_EVENT_NETWORK_REQ_CONFIG:
  case ZTS_EVENT_NETWORK_OK:
  case ZTS_EVENT_NETWORK_ACCESS_DENIED:
  case ZTS_EVENT_NETWORK_READY_IP4:
  case ZTS_EVENT_NETWORK_READY_IP6:
  case ZTS_EVENT_NETWORK_READY_IP4_IP6:
  case ZTS_EVENT_NETWORK_DOWN:
  case ZTS_EVENT_NETWORK_UPDATE:
    HandleEvent<NetworkDetails, EventCode::Network>(*eventHandler, data->eventCode, data->network);
    break;

  // Network Stack events
  case ZTS_EVENT_STACK_UP:
  case ZTS_EVENT_STACK_DOWN:
    HandleEvent<NetworkStackDetails, EventCode::NetworkStack>(*eventHandler, data->eventCode, data->netif);
    break;

  // lwIP netif events
  case ZTS_EVENT_NETIF_UP:
  case ZTS_EVENT_NETIF_DOWN:
  case ZTS_EVENT_NETIF_REMOVED:
  case ZTS_EVENT_NETIF_LINK_UP:
  case ZTS_EVENT_NETIF_LINK_DOWN:
    HandleEvent<NetworkInterfaceDetails, EventCode::NetworkInterface>(*eventHandler, data->eventCode, data->netif);
    break;

  // Peer events
  case ZTS_EVENT_PEER_DIRECT:
  case ZTS_EVENT_PEER_RELAY:
  case ZTS_EVENT_PEER_UNREACHABLE:
  case ZTS_EVENT_PEER_PATH_DISCOVERED:
  case ZTS_EVENT_PEER_PATH_DEAD:
    HandleEvent<PeerDetails, EventCode::Peer>(*eventHandler, data->eventCode, data->peer);
    break;

  // Route events
  case ZTS_EVENT_ROUTE_ADDED:
  case ZTS_EVENT_ROUTE_REMOVED:
    HandleEvent<RouteDetails, EventCode::Route>(*eventHandler, data->eventCode, data->route);
    break;

  // Address events
  case ZTS_EVENT_ADDR_ADDED_IP4:
  case ZTS_EVENT_ADDR_REMOVED_IP4:
  case ZTS_EVENT_ADDR_ADDED_IP6:
  case ZTS_EVENT_ADDR_REMOVED_IP6:
    HandleEvent<AddressDetails, EventCode::Address>(*eventHandler, data->eventCode, data->addr);
    break;

  default:
    eventHandler->onUnknownEvent(data->eventCode);
  }
}

//////////////////////////////////////////////////////////////////////////////
// User-implemented event handler interface                                 //
//////////////////////////////////////////////////////////////////////////////

namespace {
IEventHandler* g_eventHandler = nullptr;
}

ZTCPP_API void SetEventHandler(IEventHandler* aHandler) {
  g_eventHandler = aHandler;
}

ZTCPP_API IEventHandler* GetEventHandler() {
  return g_eventHandler;
}

//////////////////////////////////////////////////////////////////////////////
// Utility                                                                  //
//////////////////////////////////////////////////////////////////////////////

#if 0
struct EventCode {
  //! TODO
  enum class NetworkInterface {
    Up       = 230,
    Down     = 231,
    Removed  = 232,
    LinkUp   = 233,
    LinkDown = 234
  };

  //! TODO
  enum class NetworkStack {
    Up   = 220,
    Down = 221
  };

  //! TODO
  enum class Peer {
    Direct         = 240,
    Relay          = 241,
    Unreachable    = 242,
    PathDiscovered = 243,
    PathDead       = 244
  };

  //! TODO
  enum class Route {
    Added   = 250,
    Removed = 251
  };
};
#endif

ZTCPP_API std::string EventDescription(EventCode::Address aEventCode, const AddressDetails* aDetails) {
  std::stringstream ss;

  switch (aEventCode) {
  case EventCode::Address::AddedIPv4:
    ss << "Address event: AddedIPv4 (ZTS_EVENT_ADDR_ADDED_IP4)";
    if (aDetails) {
      ss << "\n    IP address: " << aDetails->getIpAddress();
      ss << "\n    Node ID: " << std::uppercase << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: This is the node's virtual IP address on the specified network.";
    break;

  case EventCode::Address::RemovedIPv4:
    ss << "Address event: RemovedIPv4 (ZTS_EVENT_ADDR_REMOVED_IP4)";
    if (aDetails) {
      ss << "\n    IP address: " << aDetails->getIpAddress();
      ss << "\n    Node ID: " << std::uppercase << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: This IP address of this node has been removed from the specified network.";
    break;

  case EventCode::Address::AddedIPv6:  
    ss << "Address event: AddedIPv6 (ZTS_EVENT_ADDR_ADDED_IP6)";
    if (aDetails) {
      ss << "\n    IP address: " << aDetails->getIpAddress();
      ss << "\n    Node ID: " << std::uppercase << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: This is the node's virtual IP address on the specified network.";
    break;

  case EventCode::Address::RemovedIPv6:
    ss << "Address event: RemovedIPv6 (ZTS_EVENT_ADDR_REMOVED_IP6)";
    ss << "Address event: RemovedIPv4 (ZTS_EVENT_ADDR_REMOVED_IP4)";
    if (aDetails) {
      ss << "\n    IP address: " << aDetails->getIpAddress();
      ss << "\n    Node ID: " << std::uppercase << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: This IP address of this node has been removed from the specified network.";
    break;

  default:
    ss << "Address event: Unknown (?)";
  }

  return ss.str();
}

ZTCPP_API std::string EventDescription(EventCode::Network aEventCode, const NetworkDetails* aDetails) {
  std::stringstream ss;

  switch (aEventCode) {
  case EventCode::Network::NotFound:
    ss << "Network event: Up (ZTS_EVENT_NETWORK_NOT_FOUND)";
    if (aDetails) {
      ss << "\n    Node ID: " << std::uppercase << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: Are you sure the network ID is correct?";
    break;

  case EventCode::Network::ClientTooOld:
    ss << "Network event: ClientTooOld (ZTS_EVENT_NETWORK_CLIENT_TOO_OLD)";
      if (aDetails) {
      ss << "\n    Node ID: " << std::uppercase << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    break;

  case EventCode::Network::RequestingConfiguration:
    ss << "Network event: RequestingConfiguration (ZTS_EVENT_NETWORK_REQ_CONFIG)";
    if (aDetails) {
      ss << "\n    Node ID: " << std::uppercase << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: Requiesting configuration, please wait a few seconds.";
    break;

  case EventCode::Network::OK:
    ss << "Network event: OK (ZTS_EVENT_NETWORK_OK)";
    if (aDetails) {
      ss << "\n    Node ID: " << std::uppercase << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    break;

  case EventCode::Network::AccessDenied:
    ss << "Network event: AccessDenied (ZTS_EVENT_NETWORK_ACCESS_DENIED)";
    if (aDetails) {
      ss << "\n    Node ID: " << std::uppercase << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: Access to virtual network denied. Was the node authorized?";
    break;

  case EventCode::Network::ReadyIPv4:
    ss << "Network event: ReadyIPv4 (ZTS_EVENT_NETWORK_READY_IP4)";
    if (aDetails) {
      ss << "\n    Node ID: " << std::uppercase << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: Configuration received. IPv4 traffic can now be sent over the network.";
    break;

  case EventCode::Network::ReadyIPv6:
    ss << "Network event: ReadyIPv6 (ZTS_EVENT_NETWORK_READY_IP6)";
    if (aDetails) {
      ss << "\n    Node ID: " << std::uppercase << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: Configuration received. IPv6 traffic can now be sent over the network.";
    break;

  case EventCode::Network::ReadyIPv4_IPv6:
    ss << "Network event: ReadyIPv4_IPv6 (ZTS_EVENT_NETWORK_READY_IP4_IP6)";
    if (aDetails) {
      ss << "\n    Node ID: " << std::uppercase << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: Configuration received. IPv4 and IPv6 traffic can now be sent over the network.";
    break;

  case EventCode::Network::Down:
    ss << "Network event: Down (ZTS_EVENT_NETWORK_DOWN)";
    if (aDetails) {
      ss << "\n    Node ID: " << std::uppercase << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    break;

  case EventCode::Network::Update:
    ss << "Network event: Update (ZTS_EVENT_NETWORK_UPDATE)";
    if (aDetails) {
      ss << "\n    Node ID: " << std::uppercase << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: Network state has changed.";
    break;

  default:
    ss << "Network event: Unknown (?)";
  }

  return ss.str();
}

ZTCPP_API std::string EventDescription(EventCode::NetworkInterface aEventCode,
                                       const NetworkInterfaceDetails* aDetails) {
  return "Network interface event: <description not implemented>";
}

ZTCPP_API std::string EventDescription(EventCode::NetworkStack aEventCode,
                                       const NetworkStackDetails* aDetails) {
  return "Network stack event: <description not implemented>";
}

ZTCPP_API std::string EventDescription(EventCode::Node aEventCode, const NodeDetails* aDetails) {
  std::stringstream ss;

  switch (aEventCode) {
  case EventCode::Node::Up:
    ss << "Node event: Up (ZTS_EVENT_NODE_UP)";
    break;

  case EventCode::Node::Online:
    ss << "Node event: Online (ZTS_EVENT_NODE_ONLINE)";
    if (aDetails) {
      ss << "\n    Node ID: " << std::uppercase << std::hex << aDetails->getNodeID();
    }
    else {
      ss << "\n    [No data available]";
    }
    break;

  case EventCode::Node::Offline:
    ss << "Node event: Offline (ZTS_EVENT_NODE_OFFLINE)\n";
    ss << "    Comment: Check your physical Internet connection, router, firewall, etc. What ports are you blocking?";
    break;

  case EventCode::Node::Down:
    ss << "Node event: Down (ZTS_EVENT_NODE_DOWN)";
    break;

  case EventCode::Node::IdentityCollision:
    ss << "Node event: IdentityCollision (ZTS_EVENT_NODE_IDENTITY_COLLISION)"; 
    break;

  case EventCode::Node::UnrecoverableError:
    ss << "Node event: UnrecoverableError (ZTS_EVENT_NODE_UNRECOVERABLE_ERROR)";
    break;

  case EventCode::Node::NormalTermination:
    ss << "Node event: NormalTermination (ZTS_EVENT_NODE_NORMAL_TERMINATION)";
    break;

  default:
    ss << "Node event: Unknown (?)";
  }

  return ss.str();
}

ZTCPP_API std::string EventDescription(EventCode::Peer aEventCode, const PeerDetails* aDetails) {
  return "Peer event: <description not implemented>";
}

ZTCPP_API std::string EventDescription(EventCode::Route aEventCode, const RouteDetails* aDetails) {
  return "Route event: <description not implemented>";
}

ZTCPP_NAMESPACE_END