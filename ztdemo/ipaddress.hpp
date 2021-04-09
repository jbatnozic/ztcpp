#ifndef ZTCPP_IPADDRESS_HPP
#define ZTCPP_IPADDRESS_HPP

#include <cstdint>
#include <string>
#include <ostream>

#define ZTCPP_API

namespace jbatnozic {
namespace ztcpp {

enum class AddressFamily {
    IPv4, IPv6
};

struct RawIPv6Address {
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

    bool isValid() const;
    AddressFamily getAddressFamily() const;
    std::string toString() const;

    std::uint32_t  getIPv4AddressInNetworkOrder() const;
    RawIPv6Address getIPv6AddressInNetworkOrder() const;

    friend bool operator==(const IpAddress& aLeft, const IpAddress& aRight);
    friend bool operator!=(const IpAddress& aLeft, const IpAddress& aRight);
    friend std::ostream& operator<<(std::ostream& aOstream, const IpAddress& aAddress);

private:
    std::uint32_t _addressBuf[4]; // 128 bits can hold both ipv4 and ipv6 addresses
                                  // The address is stored in network byte order
    AddressFamily _addressFamily;
    bool _isValid;
};

} // namespace ztcpp
} // namespace jbatnozic

#endif // !ZTCPP_IPADDRESS_HPP