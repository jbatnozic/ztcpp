#ifndef ZTCPP_SERVICE_HPP
#define ZTCPP_SERVICE_HPP

#include <ZTCpp/Definitions.hpp>
#include <ZTCpp/Result.hpp>

#include <string>

ZTCPP_NAMESPACE_BEGIN

// TODO: make RAII wrappers for this stuff

///////////////////////////////////////////////////////////////////////////
// CONFIGURATION                                                         //
///////////////////////////////////////////////////////////////////////////

//! Enable or disable whether the service will read or write config data
//! to local storage.
//! Should be called before StartService() if you intend on changing its state.
//! On failure, can result in: ServiceError, GenericError
ZTCPP_API EmptyResult AllowNetworkLocalStorage(bool aAllow);

//! Enable or disable whether the service will cache network details 
//! (enabled by default). This can potentially shorten (startup) times.
//! This allows the service to nearly instantly inform the network stack
//! of an address to use for this peer so that it can create an interface.
//! This can be disabled for cases where one may not want network config
//! details to be written to storage. This is especially useful for
//! situations where address assignments do not change often.
//! Should be called before StartService() if you intend on changing its state.
//! On failure, can result in: ServiceError, GenericError
ZTCPP_API EmptyResult AllowNetworkCaching(bool aAllow);

//! Enable or disable whether the service will cache peer details
//! (enabled by default). This can potentially shorten (connection) times.
//! This allows the service to re-use previously discovered paths to a peer,
//! this prevents the service from having to go through the entire
//! transport-triggered link provisioning process. This is especially
//! useful for situations where paths to peers do not change often. This is
//! enabled by default and can be disabled for cases where one may not want
//! peer details to be written to storage.
//! Should be called before StartService() if you intend on changing its state.
//! On failure, can result in: ServiceError, GenericError
ZTCPP_API EmptyResult AllowPeerCaching(bool aAllow);

//! Enable or disable whether the service will read node configuration
//! settings from a local.conf.
//! Should be called before StartService() if you intend on changing its state.
//! On failure, can result in: ServiceError, GenericError
ZTCPP_API EmptyResult AllowLocalConf(bool aAllow);

///////////////////////////////////////////////////////////////////////////
// SERVICE CONTROL                                                       //
///////////////////////////////////////////////////////////////////////////

//! Start the ZeroTier service on the given port. The files containing
//! configuration parameters and information about the local node will
//! be stored in the given path (relative to the current directory unless
//! absolute).
//! On failure, can result in: ServiceError, ArgumentError, GenericError
ZTCPP_API EmptyResult StartService(const std::string& aConfigFilePath, uint16_t aServicePort);

//! Restart the ZeroTier service.
//! This call will block until the service has been brought offline. Then
//! it will return and the user application can then watch for the appropriate
//! startup callback events.
//! On failure, can result in: ServiceError, GenericError
ZTCPP_API EmptyResult RestartService();

//! Stops the ZeroTier service and brings down all virtual network interfaces.
//! While the ZeroTier service will stop, the stack driver (with associated 
//! timers) will remain active in case future traffic processing is required.
//! To stop all activity and free all resources use FreeServiceResources() 
//! instead.
//! On failure, can result in: ServiceError, GenericError
ZTCPP_API EmptyResult StopService();

//! Stop all background services, bring down all interfaces, free all resources.
//! After calling this function an application restart will be required before
//! the library can be used again.
//! This should be called at the end of your program or when you do not anticipate
//! any more communication over ZeroTier.
//! On failure, can result in: ServiceError, GenericError
ZTCPP_API EmptyResult FreeServiceResources();

///////////////////////////////////////////////////////////////////////////
// NETWROK MANAGEMENT                                                    //
///////////////////////////////////////////////////////////////////////////

//! Attempt to join a ZeroTier network.
//! On failure, can result in: ServiceError, ArgumentError, GenericError
ZTCPP_API EmptyResult JoinNetwork(uint64_t aNetworkId);

//! Leave a ZeroTier network.
//! On failure, can result in: ServiceError, ArgumentError, GenericError
ZTCPP_API EmptyResult LeaveNetwork(uint64_t aNetworkId);

ZTCPP_NAMESPACE_END

#endif // !ZTCPP_SERVICE_HPP