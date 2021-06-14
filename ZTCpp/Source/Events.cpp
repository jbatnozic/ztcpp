
#include <ZTCpp/Events.hpp>

#include "Sockaddr_util.hpp"

#include <cassert>
#include <mutex>
#include <sstream>
#include <string>
#include <type_traits>

#include <ZeroTierSockets.h>

#define ZTCPP_ASSERT(_expr_) assert(_expr_)

ZTCPP_NAMESPACE_BEGIN

namespace {

using zts_addr_details    = zts_addr_info_t;
using zts_network_details = zts_net_info_t;
using zts_netif_details   = zts_netif_info_t;
using zts_node_details    = zts_node_info_t;
using zts_peer_details    = zts_peer_info_t;
using zts_route_details   = zts_route_info_t;

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

const zts_route_details* DataToRouteDetails(const void* aData) {
  ZTCPP_ASSERT(aData);
  return static_cast<const zts_route_details*>(aData);
}

} // namespace

//////////////////////////////////////////////////////////////////////////////
// Event detail classes                                                     //
//////////////////////////////////////////////////////////////////////////////

// *** AddressDetails ***

uint64_t AddressDetails::getNetworkID() const {
  return DataToAddressDetails(_data)->net_id;
}

IpAddress AddressDetails::getIpAddress() const {
  IpAddress result;
  uint16_t dummyPort;
  detail::ToIpAddressAndPort(&(DataToAddressDetails(_data)->addr), result, dummyPort);
  return result;
}

// *** NetworkDetails ***

uint64_t NetworkDetails::getNetworkID() const {
  return DataToNetworkDetails(_data)->net_id;
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
  return (DataToNetworkDetails(_data)->broadcast_enabled != 0);
}

int NetworkDetails::getLastPortError() const {
  return DataToNetworkDetails(_data)->port_error;
}

uint64_t NetworkDetails::getNetworkConfigurationRevision() const {
  return DataToNetworkDetails(_data)->netconf_rev;
}

uint32_t NetworkDetails::getAssignedAddressCount() const {
  return DataToNetworkDetails(_data)->assigned_addr_count;
}

uint32_t NetworkDetails::getRouteCount() const {
  return DataToNetworkDetails(_data)->route_count;
}

uint32_t NetworkDetails::getMulticastSubscriptionCount() const {
  return DataToNetworkDetails(_data)->multicast_sub_count;
}

// *** NetworkInterfaceDetails ***
// TODO

// *** NetworkStackDetails ***

// (No methods for now)

// *** NodeDetails ***

uint64_t NodeDetails::getNodeID() const {
  return DataToNodeDetails(_data)->node_id;
}

uint16_t NodeDetails::getPrimaryPort() const {
  return DataToNodeDetails(_data)->port_primary;
}

uint16_t NodeDetails::getSecondaryPort() const {
  return DataToNodeDetails(_data)->port_secondary;
}

uint16_t NodeDetails::getTertiaryPort() const {
  return DataToNodeDetails(_data)->port_tertiary;
}

void NodeDetails::getVersion(uint32_t& aMajor, uint32_t& aMinor, uint32_t& aRevision) const {
  const auto* nd = DataToNodeDetails(_data);
  aMajor = nd->ver_major;
  aMinor = nd->ver_minor;
  aRevision = nd->ver_rev;
}

// *** PeerDetails ***

uint64_t PeerDetails::getAddress() const {
  return DataToPeerDetails(_data)->peer_id;
}

void PeerDetails::getVersion(uint32_t& aMajor, uint32_t& aMinor, uint32_t& aRevision) const {
  const auto* pd = DataToPeerDetails(_data);
  aMajor = pd->ver_major;
  aMinor = pd->ver_minor;
  aRevision = pd->ver_rev;
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
  return DataToPeerDetails(_data)->path_count;
}

// *** RouteDetails ***
// TODO

//////////////////////////////////////////////////////////////////////////////
// Event handling                                                           //
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
template <class taZTCppDetailClass, class taEventCodeType, class taZTDataStruct>
void HandleEvent(EventHandlerInterface& aHandler, int16_t aRawEventCode, const taZTDataStruct* aTZDataStruct) {
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

std::recursive_mutex   g_eventHandlerMutex;
EventHandlerInterface* g_eventHandler = nullptr;
} // namespace

namespace detail {
void SetEventHandler(EventHandlerInterface* aHandler) {
  std::scoped_lock<std::recursive_mutex> lock{g_eventHandlerMutex};
  g_eventHandler = aHandler;
}

EventHandlerInterface* GetEventHandler() {
  std::scoped_lock<std::recursive_mutex> lock{g_eventHandlerMutex};
  auto p = g_eventHandler;
  return p;
}

void IntermediateEventHandler(void* aEventMessage) {
  auto* data = static_cast<zts_event_msg_t*>(aEventMessage);
  if (!data) {
    return;
  }

  std::scoped_lock<std::recursive_mutex> lock{g_eventHandlerMutex};
  auto* eventHandler = GetEventHandler();
  if (!eventHandler) {
    return;
  }

  switch (data->event_code) {
    // Node events
  case ZTS_EVENT_NODE_UP:
  case ZTS_EVENT_NODE_ONLINE:
  case ZTS_EVENT_NODE_OFFLINE:
  case ZTS_EVENT_NODE_DOWN:
    //case ZTS_EVENT_NODE_IDENTITY_COLLISION:
    //case ZTS_EVENT_NODE_UNRECOVERABLE_ERROR:
    //case ZTS_EVENT_NODE_NORMAL_TERMINATION:
    HandleEvent<NodeDetails, EventCode::Node>(*eventHandler, data->event_code, data->node);
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
    HandleEvent<NetworkDetails, EventCode::Network>(*eventHandler, data->event_code, data->network);
    break;

    // Network Stack events
  case ZTS_EVENT_STACK_UP:
  case ZTS_EVENT_STACK_DOWN:
    HandleEvent<NetworkStackDetails, EventCode::NetworkStack>(*eventHandler, data->event_code, data->netif);
    break;

    // lwIP netif events
  case ZTS_EVENT_NETIF_UP:
  case ZTS_EVENT_NETIF_DOWN:
  case ZTS_EVENT_NETIF_REMOVED:
  case ZTS_EVENT_NETIF_LINK_UP:
  case ZTS_EVENT_NETIF_LINK_DOWN:
    HandleEvent<NetworkInterfaceDetails, EventCode::NetworkInterface>(*eventHandler, data->event_code, data->netif);
    break;

    // Peer events
  case ZTS_EVENT_PEER_DIRECT:
  case ZTS_EVENT_PEER_RELAY:
  case ZTS_EVENT_PEER_UNREACHABLE:
  case ZTS_EVENT_PEER_PATH_DISCOVERED:
  case ZTS_EVENT_PEER_PATH_DEAD:
    HandleEvent<PeerDetails, EventCode::Peer>(*eventHandler, data->event_code, data->peer);
    break;

    // Route events
  case ZTS_EVENT_ROUTE_ADDED:
  case ZTS_EVENT_ROUTE_REMOVED:
    HandleEvent<RouteDetails, EventCode::Route>(*eventHandler, data->event_code, data->route);
    break;

    // Address events
  case ZTS_EVENT_ADDR_ADDED_IP4:
  case ZTS_EVENT_ADDR_REMOVED_IP4:
    data->addr->addr.ss_family = ZTS_AF_INET; // ZT doesn't fill in this info for us
    HandleEvent<AddressDetails, EventCode::Address>(*eventHandler, data->event_code, data->addr);
    break;

  case ZTS_EVENT_ADDR_ADDED_IP6:
  case ZTS_EVENT_ADDR_REMOVED_IP6:
    data->addr->addr.ss_family = ZTS_AF_INET6; // ZT doesn't fill in this info for us
    HandleEvent<AddressDetails, EventCode::Address>(*eventHandler, data->event_code, data->addr);
    break;

  default:
    eventHandler->onUnknownEvent(data->event_code);
  }
}
} // namespace detail

//////////////////////////////////////////////////////////////////////////////
// Utility                                                                  //
//////////////////////////////////////////////////////////////////////////////

ZTCPP_API std::string EventDescription(EventCode::Address aEventCode, const AddressDetails* aDetails) {
  std::stringstream ss;

  switch (aEventCode) {
  case EventCode::Address::AddedIPv4:
    ss << "Address event: AddedIPv4 (ZTS_EVENT_ADDR_ADDED_IP4)";
    if (aDetails) {
      ss << "\n    IP address: " << aDetails->getIpAddress();
      ss << "\n    Node ID: " << std::hex << aDetails->getNetworkID();
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
      ss << "\n    Node ID: " << std::hex << aDetails->getNetworkID();
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
      ss << "\n    Node ID: " << std::hex << aDetails->getNetworkID();
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
      ss << "\n    Node ID: " << std::hex << aDetails->getNetworkID();
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
      ss << "\n    Network ID: " << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: Are you sure the network ID is correct?";
    break;

  case EventCode::Network::ClientTooOld:
    ss << "Network event: ClientTooOld (ZTS_EVENT_NETWORK_CLIENT_TOO_OLD)";
      if (aDetails) {
      ss << "\n    Network ID: " << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    break;

  case EventCode::Network::RequestingConfiguration:
    ss << "Network event: RequestingConfiguration (ZTS_EVENT_NETWORK_REQ_CONFIG)";
    if (aDetails) {
      ss << "\n    Network ID: " << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: Requiesting configuration, please wait a few seconds.";
    break;

  case EventCode::Network::OK:
    ss << "Network event: OK (ZTS_EVENT_NETWORK_OK)";
    if (aDetails) {
      ss << "\n    Network ID: " << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    break;

  case EventCode::Network::AccessDenied:
    ss << "Network event: AccessDenied (ZTS_EVENT_NETWORK_ACCESS_DENIED)";
    if (aDetails) {
      ss << "\n    Network ID: " << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: Access to virtual network denied. Was the node authorized?";
    break;

  case EventCode::Network::ReadyIPv4:
    ss << "Network event: ReadyIPv4 (ZTS_EVENT_NETWORK_READY_IP4)";
    if (aDetails) {
      ss << "\n    Network ID: " << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: Configuration received. IPv4 traffic can now be sent over the network.";
    break;

  case EventCode::Network::ReadyIPv6:
    ss << "Network event: ReadyIPv6 (ZTS_EVENT_NETWORK_READY_IP6)";
    if (aDetails) {
      ss << "\n    Network ID: " << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: Configuration received. IPv6 traffic can now be sent over the network.";
    break;

  case EventCode::Network::ReadyIPv4_IPv6:
    ss << "Network event: ReadyIPv4_IPv6 (ZTS_EVENT_NETWORK_READY_IP4_IP6)";
    if (aDetails) {
      ss << "\n    Network ID: " << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    ss << "\n    Comment: Configuration received. IPv4 and IPv6 traffic can now be sent over the network.";
    break;

  case EventCode::Network::Down:
    ss << "Network event: Down (ZTS_EVENT_NETWORK_DOWN)";
    if (aDetails) {
      ss << "\n    Network ID: " << std::hex << aDetails->getNetworkID();
    }
    else {
      ss << "\n    [No data available]";
    }
    break;

  case EventCode::Network::Update:
    ss << "Network event: Update (ZTS_EVENT_NETWORK_UPDATE)";
    if (aDetails) {
      ss << "\n    Network ID: " << std::hex << aDetails->getNetworkID();
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
  std::stringstream ss;

  switch (aEventCode) {
  case EventCode::NetworkInterface::Up:
    ss << "Network interface event: Up (ZTS_EVENT_NETIF_UP)";
    break;

  case EventCode::NetworkInterface::Down:
    ss << "Network interface event: Down (ZTS_EVENT_NETIF_DOWN)";
    break;

  case EventCode::NetworkInterface::Removed:
    ss << "Network interface event: Removed (ZTS_EVENT_NETIF_REMOVED)";
    break;

  case EventCode::NetworkInterface::LinkUp:
    ss << "Network interface event: LinkUp (ZTS_EVENT_NETIF_LINK_UP)";
    break;

  case EventCode::NetworkInterface::LinkDown:
    ss << "Network interface event: LinkDown (ZTS_EVENT_NETIF_LINK_DOWN)";
    break;

  default:
    ss << "Network interface event: Unknown (?)";
    break;
  }

  return ss.str();
}

ZTCPP_API std::string EventDescription(EventCode::NetworkStack aEventCode,
                                       const NetworkStackDetails* aDetails) {
  std::stringstream ss;

  switch (aEventCode) {
  case EventCode::NetworkStack::Up:
    ss << "Networking stack event: Up (ZTS_EVENT_STACK_UP)";
    break;

  case EventCode::NetworkStack::Down:
    ss << "Networking stack event: Down (ZTS_EVENT_STACK_DOWN)";
    break;

  default:
    ss << "Networking stack event: Unknown (?)";
    break;
  }

  return ss.str();
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
      ss << "\n    Node ID: " << std::hex << aDetails->getNodeID();
    }
    else {
      ss << "\n    [No data available]";
    }
    break;

  case EventCode::Node::Offline:
    ss << "Node event: Offline (ZTS_EVENT_NODE_OFFLINE)";
    ss << "\n    Comment: Check your physical Internet connection, "
          "router, firewall, etc. What ports are you blocking?";
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
  std::stringstream ss;

#define PEER_ROLE_STR(_role_) \
  (((_role_) == PeerRole::Leaf)   ? "Leaf (Normal node)" : \
   ((_role_) == PeerRole::Moon)   ? "Moon (Root server / P2P connection orchestrator)" : \
   ((_role_) == PeerRole::Planet) ? "Planet (Root server / P2P connection orchestrator)" : "<Unknown>")

  switch (aEventCode) {
  case EventCode::Peer::Direct:
    ss << "Peer event: Direct (ZTS_EVENT_PEER_DIRECT)";
    if (aDetails) {
      ss << "\n    Node info: ID=" << std::hex << aDetails->getAddress()
         << ", Role=" << PEER_ROLE_STR(aDetails->getRole());
    }
    else {
      ss << "\n    [Node details unavailable]";
    }
    ss << "\n    Comment: A direct path is known to this node.";
    break;

  case EventCode::Peer::Relay:
    ss << "Peer event: Relay (ZTS_EVENT_PEER_RELAY)";
    if (aDetails) {
      ss << "\n    Node info: ID=" << std::hex << aDetails->getAddress()
        << ", Role=" << PEER_ROLE_STR(aDetails->getRole());
    }
    else {
      ss << "\n    [Node details unavailable]";
    }
    ss << "\n    Comment: No direct path is known to this node.";
    break;

  case EventCode::Peer::Unreachable:
    ss << "Peer event: Unreachable (ZTS_EVENT_PEER_UNREACHABLE)";
    if (aDetails) {
      ss << "\n    Node info: ID=" << std::hex << aDetails->getAddress()
        << ", Role=" << PEER_ROLE_STR(aDetails->getRole());
    }
    else {
      ss << "\n    [Node details unavailable]";
    }
    ss << "\n    Comment: This node has become unreachable.";
    break;

  case EventCode::Peer::PathDiscovered:
    ss << "Peer event: PathDiscovered (ZTS_EVENT_PEER_PATH_DISCOVERED)";
    if (aDetails) {
      ss << "\n    Node info: ID=" << std::hex << aDetails->getAddress()
        << ", Role=" << PEER_ROLE_STR(aDetails->getRole());
    }
    else {
      ss << "\n    [Node details unavailable]";
    }
    ss << "\n    Comment: A new direct path to this node was discovered.";
    break;

  case EventCode::Peer::PathDead:
    ss << "Peer event: PathDead (ZTS_EVENT_PEER_PATH_DEAD)";
    if (aDetails) {
      ss << "\n    Node info: ID=" << std::hex << aDetails->getAddress()
        << ", Role=" << PEER_ROLE_STR(aDetails->getRole());
    }
    else {
      ss << "\n    [Node details unavailable]";
    }
    ss << "\n    Comment: A direct path to this node has died.";
    break;

  default:
    ss << "Peer event: Unknown (?)";
    break;
  }

#undef PEER_ROLE_STR

  return ss.str();
}

ZTCPP_API std::string EventDescription(EventCode::Route aEventCode, const RouteDetails* aDetails) {
  std::stringstream ss;

  switch (aEventCode) {
  case EventCode::Route::Added:
    ss << "Route event: Added (ZTS_EVENT_ROUTE_ADDED)";
    break;

  case EventCode::Route::Removed:
    ss << "Route event: Removed (ZTS_EVENT_ROUTE_REMOVED)";
    break;

  default:
    ss << "Route event: Unknown (?)";
    break;
  }

  return ss.str();
}

ZTCPP_NAMESPACE_END