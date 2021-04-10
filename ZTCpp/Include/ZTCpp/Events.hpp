#ifndef ZTCPP_EVENTS_HPP
#define ZTCPP_EVENTS_HPP

#include <ZTCpp/Definitions.hpp>
#include <ZTCpp/Ip_address.hpp>

#include <chrono>
#include <string>

ZTCPP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
// Event codes                                                              //
//////////////////////////////////////////////////////////////////////////////

struct EventCode {
  //! Events about the local node's virtual IP address on ZeroTier networks
  enum class Address {
    AddedIPv4   = 260,
    RemovedIPv4 = 261,
    AddedIPv6   = 262,
    RemovedIPv6 = 263
  };

  //! TODO
  enum class Network {
    NotFound                = 210,
    ClientTooOld            = 211,
    RequestingConfiguration = 212,
    OK                      = 213,
    AccessDenied            = 214,
    ReadyIPv4               = 215,
    ReadyIPv6               = 216,
    ReadyIPv4_IPv6          = 217,
    Down                    = 218,
    Update                  = 219
  };

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
  enum class Node {
    Up                 = 200,
    Online             = 201,
    Offline            = 202,
    Down               = 203,
    IdentityCollision  = 204,
    UnrecoverableError = 205,
    NormalTermination  = 206
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

//////////////////////////////////////////////////////////////////////////////
// Other enums                                                              //
//////////////////////////////////////////////////////////////////////////////

//! Virtual network status codes
enum class VirtualNetworkStatus {
  RequestingConfiguration = 0, //! Waiting for network configuration (also means revision == 0)
  OK                      = 1, //! Configuration received and we are authorized
  AccessDenied            = 2, //! Netconf master told us 'nope'
  NotFound                = 3, //! Netconf master exists, but this virtual network does not
  PortError               = 4, //! Initialization of network failed or other internal error
  ClientTooOld            = 5  //! ZeroTier core version too old
};

//! Virtual network type codes
enum class VirtualNetworkType {
  Private = 0, //! Private networks are authorized via certificates of membership
  Public  = 1  //! Public networks have no access control -- they'll always be AUTHORIZED
};

//! What trust hierarchy role does this peer have?
enum class PeerRole {
  Leaf   = 0, //! Ordinary node
  Moon   = 1, //! Moon root
  Planet = 2  //! Planetary root
};

//////////////////////////////////////////////////////////////////////////////
// Event detail classes                                                     //
//////////////////////////////////////////////////////////////////////////////

class ZTCPP_API AddressDetails {
public:
  //! Network ID for which the event happened
  uint64_t getNetworkID() const;

  //! Virtual IP address pertaining to the event
  IpAddress getIpAddress() const;

private:
  const void* _data;
  template <class T> friend class PrivateDataSetter;
};

/**
 * Virtual network configuration
 */
class ZTCPP_API NetworkDetails {
public:
  //! 64-bit ZeroTier network ID
  uint64_t getNetworkID() const;

  //! Ethernet MAC (48 bits) that should be assigned to port
  uint64_t getMACAddress() const;

  //! Network name (from network configuration master)
  std::string getNetworkName() const;

  //! Network configuration request status
  VirtualNetworkStatus getNetworkStatus() const;

  //! Network type
  VirtualNetworkType getNetworkType() const;

  //! Maximum interface MTU
  uint32_t getMaximumTransissionUnit() const;

  //! If true, the network this port belongs to indicates DHCP availability
  //!
  //! This is a suggestion. The underlying implementation is free to ignore it
  //! for security or other reasons. This is simply a netconf parameter that
  //! means 'DHCP is available on this network.'
  //! (DHCP = Dynamic Host Configuration Protocol)
  bool getDHCPAvailable() const;

  //! If true, this port is allowed to bridge to other networks
  //!
  //! This is informational. If this is false, bridged packets will simply
  //! be dropped and bridging won't work.
  bool getBridgeEnabled() const;

  //! If true, this network supports and allows broadcast (ff:ff:ff:ff:ff:ff) traffic
  bool getBroadcastEnabled() const;

  //! If the network is in PORT_ERROR state, this is the (negative) error code most recently reported
  int getLastPortError() const;

  //! Revision number as reported by controller or 0 if still waiting for config
  uint64_t getNetworkConfigurationRevision() const;

  //! Number of assigned addresses
  uint32_t getAssignedAddressCount() const;

  //! ZeroTier-assigned addresses (in sockaddr_storage structures)
  //!
  //! For IP, the port number of the sockaddr_XX structure contains the number
  //! of bits in the address netmask. Only the IP address and port are used.
  //! Other fields like interface number can be ignored.
  //!
  //! This is only used for ZeroTier-managed address assignments sent by the
  //! virtual network's configuration master.
  // struct zts_sockaddr_storage assignedAddresses[ZTS_MAX_ZT_ASSIGNED_ADDRESSES];
  // TODO

  //! Number of ZT-pushed routes
  uint32_t getRouteCount() const;

  //! Routes (excluding those implied by assigned addresses and their masks)
  // ZTS_VirtualNetworkRoute routes[ZTS_MAX_NETWORK_ROUTES];
  // TODO

  //! Number of multicast groups subscribed
  uint32_t getMulticastSubscriptionCount() const;

  //! Multicast groups to which this network's device is subscribed
  // struct {
  //   uint64_t mac; /* MAC in lower 48 bits */
  //   uint32_t adi; /* Additional distinguishing information, usually zero except for IPv4 ARP groups */
  // } multicastSubscriptions[ZTS_MAX_MULTICAST_SUBSCRIPTIONS];
  // TODO

private:
  const void* _data;
  template <class T> friend class PrivateDataSetter;
};

class NetworkInterfaceDetails {
public:
  // TODO
protected:
  const void* _data;
  template <class T> friend class PrivateDataSetter;
};

//! Information about the networking stack
class NetworkStackDetails : public NetworkInterfaceDetails {
  template <class T> friend class PrivateDataSetter;
};

/**
 * A structure used to convey details about the current node
 * to the user application
 */
class NodeDetails {
public:
  //! The node ID
  ZTCPP_API uint64_t getNodeID() const;

  /**
   * The ports used by the service to send and receive
   * all encapsulated traffic
   */
  ZTCPP_API uint16_t getPrimaryPort() const;
  ZTCPP_API uint16_t getSecondaryPort() const;
  ZTCPP_API uint16_t getTertiaryPort() const;

  //! Retrieve ZeroTier version
  ZTCPP_API void getVersion(uint32_t& aMajor, uint32_t& aMinor, uint32_t& aRevision) const;

private:
  const void* _data;
  template <class T> friend class PrivateDataSetter;
};

class PeerDetails {
public:
  /**
   * ZeroTier address (40 bits)
   */
  ZTCPP_API uint64_t getAddress() const;

  /**
   * Remote version (Major, Minor, Revicion); -1 if not known
   */
  ZTCPP_API void getVersion(uint32_t& aMajor, uint32_t& aMinor, uint32_t& aRevision) const;

  /**
   * Last measured latency; -1 if unknown
   */
  ZTCPP_API std::chrono::milliseconds getLatency() const;

  /**
   * What trust hierarchy role does this device have?
   */
  ZTCPP_API PeerRole getRole() const;

  /**
   * Number of paths (size of paths[])
   */
  ZTCPP_API uint32_t getPathCount() const;

  /**
   * Known network paths to peer
   */
  // struct zts_physical_path paths[ZTS_MAX_PEER_NETWORK_PATHS];
  // TODO

private:
  const void* _data;
  template <class T> friend class PrivateDataSetter;
};

//! Details about a virtual network route
class RouteDetails {
public:
  // TODO
private:
  const void* _data;
  template <class T> friend class PrivateDataSetter;
};

//////////////////////////////////////////////////////////////////////////////
// User-implemented event handler interface                                 //
//////////////////////////////////////////////////////////////////////////////

class ZTCPP_API IEventHandler {
public:
  virtual void onAddressEvent(EventCode::Address aEventCode, const AddressDetails* aDetails) noexcept = 0;
  virtual void onNetworkEvent(EventCode::Network aEventCode, const NetworkDetails* aDetails) noexcept = 0;
  virtual void onNetworkInterfaceEvent(EventCode::NetworkInterface aEventCode, 
                                       const NetworkInterfaceDetails* aDetails) noexcept = 0;
  virtual void onNetworkStackEvent(EventCode::NetworkStack aEventCode, 
                                   const NetworkStackDetails* aDetails) noexcept = 0;
  virtual void onNodeEvent(EventCode::Node aEventCode, const NodeDetails* aDetails) noexcept = 0;
  virtual void onPeerEvent(EventCode::Peer aEventCode, const PeerDetails* aDetails) noexcept = 0;
  virtual void onRouteEvent(EventCode::Route aEventCode, const RouteDetails* aDetails) noexcept = 0;
  virtual void onUnknownEvent(int16_t aRawZeroTierEventCode) noexcept = 0;
};

ZTCPP_API void SetEventHandler(IEventHandler* aHandler);
ZTCPP_API IEventHandler* GetEventHandler();

//////////////////////////////////////////////////////////////////////////////
// Utility                                                                  //
//////////////////////////////////////////////////////////////////////////////

ZTCPP_API std::string EventDescription(EventCode::Address aEventCode, const AddressDetails* aDetails);
ZTCPP_API std::string EventDescription(EventCode::Network aEventCode, const NetworkDetails* aDetails);
ZTCPP_API std::string EventDescription(EventCode::NetworkInterface aEventCode, 
                                       const NetworkInterfaceDetails* aDetails);
ZTCPP_API std::string EventDescription(EventCode::NetworkStack aEventCode, 
                                       const NetworkStackDetails* aDetails);
ZTCPP_API std::string EventDescription(EventCode::Node aEventCode, const NodeDetails* aDetails);
ZTCPP_API std::string EventDescription(EventCode::Peer aEventCode, const PeerDetails* aDetails);
ZTCPP_API std::string EventDescription(EventCode::Route aEventCode, const RouteDetails* aDetails);

ZTCPP_NAMESPACE_END

#endif // !ZTCPP_EVENTS_HPP