#ifndef ZTCPP_IP_ADDRESS_HPP
#define ZTCPP_IP_ADDRESS_HPP

#include <ZTCpp/Definitions.hpp>

#include <cstdint>
#include <string>
#include <ostream>

ZTCPP_NAMESPACE_BEGIN

enum class AddressFamily {
  IPv4, IPv6
};

struct ZTCPP_API RawIPv6Address {
  std::uint8_t bytes[16];
};

class ZTCPP_API IpAddress {
public:
  IpAddress();

  static IpAddress ipv4Unspecified();
  static IpAddress ipv6Unspecified();
  static IpAddress ipv4Loopback();
  static IpAddress ipv6Loopback();
  static IpAddress ipv4FromString(const char* aAddress);
  static IpAddress ipv4FromString(const std::string& aAddress);
  static IpAddress ipv6FromString(const char* aAddress);
  static IpAddress ipv6FromString(const std::string& aAddress);
  static IpAddress ipv4FromBinaryRepresentationInNetworkOrder(const void* aData);
  static IpAddress ipv6FromBinaryRepresentationInNetworkOrder(const void* aData);

  bool isValid() const;
  AddressFamily getAddressFamily() const;
  std::string toString() const;

  std::uint32_t  getIPv4AddressInNetworkOrder() const;
  RawIPv6Address getIPv6AddressInNetworkOrder() const;

  ZTCPP_API friend bool operator==(const IpAddress& aLeft, const IpAddress& aRight);
  ZTCPP_API friend bool operator!=(const IpAddress& aLeft, const IpAddress& aRight);
  ZTCPP_API friend std::ostream& operator<<(std::ostream& aOstream, const IpAddress& aAddress);

private:
  std::uint32_t _addressBuf[4]; // 128 bits can hold both ipv4 and ipv6 addresses
                                // The address is stored in network byte order
  AddressFamily _addressFamily;
  bool _isValid;
};

ZTCPP_NAMESPACE_END

#endif // !ZTCPP_IP_ADDRESS_HPP