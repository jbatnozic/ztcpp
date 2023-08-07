#ifndef ZTCPP_SERVICE_HPP
#define ZTCPP_SERVICE_HPP

#include <ZTCpp/Definitions.hpp>
#include <ZTCpp/Events.hpp>
#include <ZTCpp/Result.hpp>

#include <string>

ZTCPP_NAMESPACE_BEGIN

// TODO (make RAII wrappers for this stuff)

///////////////////////////////////////////////////////////////////////////
// CONFIGURATION                                                         //
///////////////////////////////////////////////////////////////////////////

//! Configuration methods for the local node. 
//! IMPORTANT: They must be called BEFORE the local node is started!
class ZTCPP_API Config {
public:
  //! TODO (add doc)
  static EmptyResult setIdentityFromStorage(const std::string& aPath);

  //! TODO (add doc)
  static EmptyResult setIdentityFromMemory(const char* aKey, std::size_t aKeyLength);

  //! Set the port to which the node should bind.
  //! On failure, can result in: ArgumentError, ServiceError.
  static EmptyResult setPort(uint16_t aPort);

  //! Set range that random ports will be selected from.
  //! On failure, can result in: ArgumentError, ServiceError.
  static EmptyResult setRandomPortRange(uint16_t aStartPort, uint16_t aEndPort);

  //! Allow or disallow ZeroTier from automatically selecting a backup port 
  //! to help get through buggy NAT. This is enabled by default. This port is
  //! randomly chosen and should be disabled if you want to control exactly
  //! which ports ZeroTier talks on and (iff) you know with absolute certainty
  //! that traffic on your chosen primary port is allowed.
  //! On failure, can result in: ArgumentError (not likely) or ServiceError.
  static EmptyResult allowSecondaryPort(bool aAllowed);

  //! Allow or disallow the use of port-mapping. This is enabled by default.
  //! On failure, can result in: ArgumentError (not likely) or ServiceError.
  static EmptyResult allowPortMapping(bool aAllowed);

  //! Enable or disable whether the node will cache network details (enabled
  //! by default when `setIdentityFromStorage()` is used).
  //!
  //! This can potentially shorten (startup) times between node restarts. This
  //! allows the service to nearly instantly inform the network stack of an 
  //! address to use for this peer so that it can create a transport service.
  //! This can be disabled for cases where one may not want network config
  //! details to be written to storage. This is especially useful for
  //! situations where address assignments do not change often.
  //!
  //! On failure, can result in: ArgumentError (not likely) or ServiceError.
  static EmptyResult allowNetworkCaching(bool aAllowed);

  //! Enable or disable whether the node will cache peer details (enabled
  //! by default when `setIdentityFromStorage()` is used).
  //!
  //! This can potentially shorten (connection) times between node restarts.
  //! This allows the service to re-use previously discovered paths to a peer,
  //! this prevents the service from having to go through the entire
  //! transport-triggered link provisioning process. This is especially useful
  //! for situations where paths to peers do not change often. This is enabled
  //! by default and can be disabled for cases where one may not want peer 
  //! details to be written to storage.
  //!
  //! On failure, can result in: ArgumentError (not likely) or ServiceError.
  static EmptyResult allowPeerCaching(bool aAllowed);

  //! Enable or disable whether the node will cache root definitions (enabled
  //! by default when `setIdentityFromStorage()` is used).
  //! On failure, can result in: ArgumentError (not likely) or ServiceError.
  static EmptyResult allowRootCaching(bool aAllowed);

  //! Enable or disable whether the node will cache identities (enabled
  //! by default when `setIdentityFromStorage()` is used).
  //! On failure, can result in: ArgumentError (not likely) or ServiceError.
  static EmptyResult allowIdentityCaching(bool aAllowed);
};

///////////////////////////////////////////////////////////////////////////
// LOCAL NODE                                                            //
///////////////////////////////////////////////////////////////////////////

class ZTCPP_API LocalNode {
public:
  //! TODO (add doc)
  static void setEventHandler(EventHandlerInterface* aHandler);

  //! TODO (add doc)
  static EventHandlerInterface* getEventHandler();

  //! Start the local ZeroTier node. Should be called after calling the
  //! relevant `Config::*` functions for your application.
  //!
  //! Note: If neither `Config::setIdentityFromStorage()` nor 
  //! `Config::setIdentityFromMemory()` are called, a new identity will
  //! be generated and will be retrievable via `zts_node_get_id_pair()`
  //! *after* the node has started.
  //!
  //! //! On failure, can result in: ServiceError.
  static EmptyResult start();

  //! Stop the local ZeroTier node and bring down all virtual network
  //! transport services. Callable only after the node has been started.
  //!
  //! While the ZeroTier node will stop, the stack driver (with associated
  //! timers) will remain active in case future traffic processing is
  //! required. To stop all activity and free all resources use 
  //! `freeResources()` instead.
  //!
  //! On failure, can result in: ServiceError.
  static EmptyResult stop();

  //! Stop all background threads, bring down all transport services,
  //! free all resources. After calling this function an application
  //! restart will be required before the library can be used again. 
  //! Callable only after the node has been started.
  //!
  //! This should be called at the end of your program or when you do not
  //! anticipate communicating over ZeroTier again.
  //!
  //! On failure, can result in: ServiceError.
  static EmptyResult freeResources();

  //! Returns true if the node is online (Can reach the Internet).
  static bool isOnline();

  //! TODO (add doc)
  static uint64_t getID();

  //! Get the primary port to which the node is bound. Callable only after
  //! the node has been started.
  static uint16_t getPort();
};

///////////////////////////////////////////////////////////////////////////
// NETWORKS                                                              //
///////////////////////////////////////////////////////////////////////////

class ZTCPP_API Network {
public:
  //! Attempt to join a ZeroTier network (with the given network ID).
  //! On failure, can result in: ArgumentError, ServiceError, GenericError.
  static EmptyResult join(uint64_t aNetworkId);

  //! Leave a ZeroTier network.
  //! On failure, can result in: ArgumentError, ServiceError, GenericError.
  static EmptyResult leave(uint64_t aNetworkId);

  //! TODO (add doc)
  static bool isTransportReady(uint64_t aNetworkId);

  //! TODO (add doc)
  static bool isBroadcastEnabled(uint64_t aNetworkId);

  //! TODO (add doc)
  static Result<std::string> getName(uint64_t aNetworkId);

  //! TODO (add doc)
  //! Note: returns one of zts_network_status_t
  static int getStatus(uint64_t aNetworkId);

  //! TODO (add doc)
  //! Note: returns one of zts_net_info_type_t
  static int getType(uint64_t aNetworkId);
};

ZTCPP_NAMESPACE_END

#endif // !ZTCPP_SERVICE_HPP