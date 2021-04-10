#ifndef ZTCPP_SOCKADDR_UTIL_HPP
#define ZTCPP_SOCKADDR_UTIL_HPP

#include <ZTCpp/Definitions.hpp>
#include <ZTCpp/Ip_address.hpp>

extern "C" {
struct zts_sockaddr_storage;
} // extern "C"

ZTCPP_NAMESPACE_BEGIN
namespace detail {

//! Convert an IpAdress+port pair into a zts_sockaddr_storage object (can safely be cast into other
//! zts_sockaddr_* types).
//! Don't pass an invalid Ip address object!
struct zts_sockaddr_storage ToSockaddr(const IpAddress& aIpAddress, std::uint16_t aPortInHostOrder);

//! Opposite of ToSockaddr
//! Don't pass a null pointer!
void ToIpAddressAndPort(const struct zts_sockaddr_storage* aSockaddr,
                        IpAddress& aIpAddress, 
                        std::uint16_t& aPortInHostOrder);

} // namespace detail
ZTCPP_NAMESPACE_END

#endif // !ZTCPP_SOCKADDR_UTIL_HPP