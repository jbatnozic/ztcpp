
#include "Sockaddr_util.hpp"

#include <cassert>

#include <ZeroTierSockets.h>

#if defined(_WIN32)
  #include <winsock.h>
#else
  #include <arpa/inet.h>
#endif

ZTCPP_NAMESPACE_BEGIN
namespace detail {

//! Convert an IpAdress+port pair into a zts_sockaddr_storage object (can safely be cast into other
//! zts_sockaddr_* types).
//! Don't pass an invalid Ip address object!
struct zts_sockaddr_storage ToSockaddr(const IpAddress& aIpAddress, std::uint16_t aPortInHostOrder) {
  assert(aIpAddress.isValid());
  switch (aIpAddress.getAddressFamily()) {
  case AddressFamily::IPv4:
    {
      struct zts_sockaddr_in result;

      result.sin_family = ZTS_AF_INET;
      result.sin_port = htons(aPortInHostOrder);
    #ifdef _WIN32
      result.sin_addr.S_addr = aIpAddress.getIPv4AddressInNetworkOrder();
    #else
      result.sin_addr.s_addr = aIpAddress.getIPv4AddressInNetworkOrder();
    #endif
      result.sin_len = sizeof(result);
      std::memset(&(result.sin_zero), 0x00, sizeof(result.sin_zero) / sizeof(result.sin_zero[0]));

      return *reinterpret_cast<struct zts_sockaddr_storage*>(&result);
    }

  case AddressFamily::IPv6:
    {
      struct zts_sockaddr_in6 result;

      result.sin6_family = ZTS_AF_INET6;
      result.sin6_port = htons(aPortInHostOrder);
      auto rawIPv6Addr = aIpAddress.getIPv6AddressInNetworkOrder();
      std::memcpy(&(result.sin6_addr), &rawIPv6Addr, sizeof(result.sin6_addr));
      result.sin6_len = sizeof(result);
      result.sin6_flowinfo = 0;
      result.sin6_scope_id = 0;

      return *reinterpret_cast<struct zts_sockaddr_storage*>(&result);
    }

  default:
    assert(false && "ToSockaddr - Invalid AddressFamily");
  }

  assert(false && "ToSockaddr - This section should be unreachable!");
  return {};
}

//! Opposite of ToSockaddr
//! Don't pass a null pointer!
void ToIpAddressAndPort(const struct zts_sockaddr_storage* aSockaddr,
                        IpAddress& aIpAddress, 
                        std::uint16_t& aPortInHostOrder) {
  const zts_sa_family_t addressFamily = aSockaddr->ss_family;
  
  if (addressFamily == ZTS_AF_INET) {
    const struct zts_sockaddr_in* in4 = reinterpret_cast<const struct zts_sockaddr_in*>(aSockaddr);
    aIpAddress = IpAddress::ipv4FromBinaryRepresentationInNetworkOrder(&in4->sin_addr);
    aPortInHostOrder = ntohs(in4->sin_port);
  }
  else if (addressFamily == ZTS_AF_INET6) {
    const struct zts_sockaddr_in6* in6 = reinterpret_cast<const struct zts_sockaddr_in6*>(aSockaddr);
    aIpAddress = IpAddress::ipv6FromBinaryRepresentationInNetworkOrder(&in6->sin6_addr);
    aPortInHostOrder = ntohs(in6->sin6_port);
  }
  else {
    aIpAddress = IpAddress{};
    aPortInHostOrder = 0;
  }
}

} // namespace detail
ZTCPP_NAMESPACE_END