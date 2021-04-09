#ifndef ZTCPP_ZTCPP_HPP_
#define ZTCPP_ZTCPP_HPP_

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>

// TODO Temporary
#define ZTS_ERR_OK            0 // No error
#define ZTS_ERR_SOCKET       -1 // Socket error, see zts_errno
#define ZTS_ERR_SERVICE      -2 // You probably did something at the wrong time
#define ZTS_ERR_ARG          -3 // Invalid argument
#define ZTS_ERR_NO_RESULT    -4 // No result (not necessarily an error)
#define ZTS_ERR_GENERAL      -5 // Consider filing a bug report

namespace jbatnozic {
namespace ztcpp {

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

#if defined(_WIN32)
  // TODO This config doesn't support shared build
  #ifndef ZTCPP_IMPORT
    #define ZTCPP_API __declspec(dllexport)
  #else
    #define ZTCPP_API __declspec(dllimport)
  #endif
  #define ZTCPPCALL __cdecl
#else
  #define ZTCPP_API
  #define ZTCPPCALL
#endif

struct EventCode {
  enum class Node {
    Up                 = 200,
    Online             = 201,
    Offline            = 202,
    Down               = 203,
    IdentityCollision  = 204,
    UnrecoverableError = 205,
    NormalTermination  = 206
  };

  enum class Network {
    NotFound                = 210,
    ClientTooOld            = 211,
    RequestingConfiguration = 212,
    OK                      = 213,
    AccessDenied            = 214,
    ReadyIP4                = 215,
    ReadyIP6                = 216,
    ReadyIP4_IP6            = 217,
    Down                    = 218,
    Update                  = 219
  };

  enum class NetworkStack {
    Up   = 220,
    Down = 221
  };

  enum class NetIF {
    Up       = 230,
    Down     = 231,
    Removed  = 232,
    LinkUp   = 233,
    LinkDown = 234
  };

  enum class Peer {
    Direct         = 240,
    Relay          = 241,
    Unreachable    = 242,
    PathDiscovered = 243,
    PathDead       = 244
  };

  enum class Route {
    Added   = 250,
    Removed = 251
  };

  enum class Address {
    AddedIP4   = 260,
    RemovedIP4 = 261,
    AddedIP6   = 262,
    RemovedIP6 = 263
  };
};

/**
 * Virtual network status codes
 */
enum class VirtualNetworkStatus {
  /**
   * Waiting for network configuration (also means revision == 0)
   */
  RequestingConfiguration = 0,

  /**
   * Configuration received and we are authorized
   */
  OK = 1,

  /**
   * Netconf master told us 'nope'
   */
  AccessDenied = 2,

  /**
   * Netconf master exists, but this virtual network does not
   */
  NotFound = 3,

  /**
   * Initialization of network failed or other internal error
   */
  PortError = 4,

  /**
   * ZeroTier core version too old
   */
  ClientTooOld = 5
};

/**
 * Virtual network type codes
 */
enum class VirtualNetworkType {
  /**
   * Private networks are authorized via certificates of membership
   */
  Private = 0,

  /**
   * Public networks have no access control -- they'll always be AUTHORIZED
   */
  Public = 1
};

/**
 * What trust hierarchy role does this peer have?
 */
enum class PeerRole {
  Leaf   = 0, // ordinary node
  Moon   = 1, // moon root
  Planet = 2  // planetary root
};

/**
 * A structure used to convey details about the current node
 * to the user application
 */
class NodeDetails {
public:
  /**
   * The node ID
   */
  ZTCPP_API uint64_t ZTCPPCALL getNodeID() const;

  /**
   * The ports used by the service to send and receive
   * all encapsulated traffic
   */
  ZTCPP_API uint16_t ZTCPPCALL getPrimaryPort() const;
  ZTCPP_API uint16_t ZTCPPCALL getSecondaryPort() const;
  ZTCPP_API uint16_t ZTCPPCALL getTertiaryPort() const;

  /**
   * Retrieve ZeroTier version
   */
  ZTCPP_API void ZTCPPCALL getVersion(uint32_t& aMajor, uint32_t& aMinor, uint32_t& aRevision) const;

private:
  const void* _data;
  template <class T> friend class PrivateDataSetter;
};

/**
 * Virtual network configuration
 */
class NetworkDetails {
public:
  /**
   * 64-bit ZeroTier network ID
   */
  ZTCPP_API uint64_t ZTCPPCALL getNetworkID() const;

  /**
   * Ethernet MAC (48 bits) that should be assigned to port
   */
  ZTCPP_API uint64_t ZTCPPCALL getMACAddress() const;

  /**
   * Network name (from network configuration master)
   */
  ZTCPP_API std::string ZTCPPCALL getNetworkName() const;

  /**
   * Network configuration request status
   */
  ZTCPP_API VirtualNetworkStatus ZTCPPCALL getNetworkStatus() const;

  /**
   * Network type
   */
  ZTCPP_API VirtualNetworkType ZTCPPCALL getNetworkType() const;

  /**
   * Maximum interface MTU
   */
  ZTCPP_API uint32_t ZTCPPCALL getMaximumTransissionUnit() const;

  /**
   * If true, the network this port belongs to indicates DHCP availability
   *
   * This is a suggestion. The underlying implementation is free to ignore it
   * for security or other reasons. This is simply a netconf parameter that
   * means 'DHCP is available on this network.'
   * (DHCP = Dynamic Host Configuration Protocol)
   */
  ZTCPP_API bool ZTCPPCALL getDHCPAvailable() const;

  /**
   * If true, this port is allowed to bridge to other networks
   *
   * This is informational. If this is false, bridged packets will simply
   * be dropped and bridging won't work.
   */
  ZTCPP_API bool ZTCPPCALL getBridgeEnabled() const;

  /**
   * If true, this network supports and allows broadcast (ff:ff:ff:ff:ff:ff) traffic
   */
  ZTCPP_API bool ZTCPPCALL getBroadcastEnabled() const;

  /**
   * If the network is in PORT_ERROR state, this is the (negative) error code most recently reported
   */
  ZTCPP_API int ZTCPPCALL getLastPortError() const;

  /**
   * Revision number as reported by controller or 0 if still waiting for config
   */
  ZTCPP_API uint64_t ZTCPPCALL getNetworkConfigurationRevision() const;

  /**
   * Number of assigned addresses
   */
  ZTCPP_API uint32_t ZTCPPCALL getAssignedAddressCount() const;

  /**
   * ZeroTier-assigned addresses (in sockaddr_storage structures)
   *
   * For IP, the port number of the sockaddr_XX structure contains the number
   * of bits in the address netmask. Only the IP address and port are used.
   * Other fields like interface number can be ignored.
   *
   * This is only used for ZeroTier-managed address assignments sent by the
   * virtual network's configuration master.
   */
  // struct zts_sockaddr_storage assignedAddresses[ZTS_MAX_ZT_ASSIGNED_ADDRESSES];
  // TODO

  /**
   * Number of ZT-pushed routes
   */
  ZTCPP_API uint32_t ZTCPPCALL getRouteCount() const;

  /**
   * Routes (excluding those implied by assigned addresses and their masks)
   */
  // ZTS_VirtualNetworkRoute routes[ZTS_MAX_NETWORK_ROUTES];
  // TODO

  /**
   * Number of multicast groups subscribed
   */
  ZTCPP_API uint32_t ZTCPPCALL getMulticastSubscriptionCount() const;

  /**
   * Multicast groups to which this network's device is subscribed
   */
  // struct {
  //   uint64_t mac; /* MAC in lower 48 bits */
  //   uint32_t adi; /* Additional distinguishing information, usually zero except for IPv4 ARP groups */
  // } multicastSubscriptions[ZTS_MAX_MULTICAST_SUBSCRIPTIONS];
  // TODO

private:
  const void* _data;
  template <class T> friend class PrivateDataSetter;
};

class NetIFDetails {
public:
  // TODO
protected:
  const void* _data;
  template <class T> friend class PrivateDataSetter;
};

/*
 * Information about the networking stack
 */
class NetworkStackDetails : public NetIFDetails {
  template <class T> friend class PrivateDataSetter;
};

class VirtualNetworkRoute {
public:
  // TODO
private:
  const void* _data;
  template <class T> friend class PrivateDataSetter;
};

class PeerDetails {
public:
  /**
   * ZeroTier address (40 bits)
   */
  ZTCPP_API uint64_t ZTCPPCALL getAddress() const;

  /**
   * Remote version (Major, Minor, Revicion); -1 if not known
   */
  ZTCPP_API void ZTCPPCALL getVersion(uint32_t& aMajor, uint32_t& aMinor, uint32_t& aRevision) const;

  /**
   * Last measured latency; -1 if unknown
   */
  ZTCPP_API std::chrono::milliseconds ZTCPPCALL getLatency() const;

  /**
   * What trust hierarchy role does this device have?
   */
  ZTCPP_API PeerRole ZTCPPCALL getRole() const;

  /**
   * Number of paths (size of paths[])
   */
  ZTCPP_API uint32_t ZTCPPCALL getPathCount() const;

  /**
   * Known network paths to peer
   */
  // struct zts_physical_path paths[ZTS_MAX_PEER_NETWORK_PATHS];
  // TODO

private:
  const void* _data;
  template <class T> friend class PrivateDataSetter;
};

class AddressDetails {
public:
  // TODO
private:
  const void* _data;
  template <class T> friend class PrivateDataSetter;
};

struct EventHandlers {
  std::function<void(EventCode::Node,         const NodeDetails*)>         nodeDetailsHandler;
  std::function<void(EventCode::Network,      const NetworkDetails*)>      networkDetailsHandler;
  std::function<void(EventCode::NetworkStack, const NetworkStackDetails*)> networkStackDetailsHandler;
  std::function<void(EventCode::NetIF,        const NetIFDetails*)>        netIFDetailsHandler;
  std::function<void(EventCode::Route,        const VirtualNetworkRoute*)> virtualNetworkRouteHandler;
  std::function<void(EventCode::Peer,         const PeerDetails*)>         peerDetailsHandler;
  std::function<void(EventCode::Address,      const AddressDetails*)>      addressDetailsHandler;
};

//////////////////////////////////////////////////////////////////////////////
// Free functions                                                           //
//////////////////////////////////////////////////////////////////////////////

ZTCPP_API int ZTCPPCALL StartZT(const std::string& aPath, EventHandlers aEventHandlers, uint16_t aPort);
ZTCPP_API int ZTCPPCALL StopZT();
ZTCPP_API int ZTCPPCALL FreeZT();

ZTCPP_API int ZTCPPCALL JoinNetwork(const uint64_t aNetworkId);

ZTCPP_API void ZTCPPCALL ztcpp_central_cleanup();

} // namespace ztcpp
} // namespace jbatnozic

#endif // !ZTCPP_ZTCPP_HPP_