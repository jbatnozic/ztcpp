
#include "ipaddress.hpp"

#include <cstring>

#include <ZeroTierSockets.h>

namespace jbatnozic {
namespace ztcpp {

IpAddress::IpAddress()
    : _isValid{false}
{
}

IpAddress IpAddress::ipv4Unspecified() {
    IpAddress result;
    result._addressFamily = AddressFamily::IPv4;
    result._addressBuf[0] = ZTS_INADDR_ANY;
    result._isValid = true;
    return result;
}

IpAddress IpAddress::ipv6Unspecified() {
    IpAddress result;
    result._addressFamily = AddressFamily::IPv4;
    std::memset(&(result._addressBuf[0]), 0x00, sizeof(std::uint32_t) * 4);
    result._isValid = true;
    return result;
}

IpAddress IpAddress::ipv4Loopback() {
    IpAddress result;
    result._addressFamily = AddressFamily::IPv4;
    result._addressBuf[0] = ZTS_INADDR_LOOPBACK;
    result._isValid = true;
    return result;
}

IpAddress IpAddress::ipv6Loopback() {
    IpAddress result;
    result._addressFamily = AddressFamily::IPv4;
    std::memset(&(result._addressBuf[0]), 0x00, sizeof(std::uint32_t) * 4);
    reinterpret_cast<char*>(&(result._addressBuf[0]))[15] = 1;
    result._isValid = true;
    return result;
}

IpAddress IpAddress::ipv4FromString(const char* aAddress) {
    IpAddress result;
    result._addressFamily = AddressFamily::IPv4;
    if (zts_inet_pton(ZTS_AF_INET, aAddress, result._addressBuf) == 1) {
        result._isValid = true;
    }
    return result;
}

IpAddress IpAddress::ipv4FromString(const std::string& aAddress) {
    return ipv4FromString(aAddress.c_str());
}

IpAddress IpAddress::ipv6FromString(const char* aAddress) {
    IpAddress result;
    result._addressFamily = AddressFamily::IPv6;
    if (zts_inet_pton(ZTS_AF_INET6, aAddress, result._addressBuf) == 1) {
        result._isValid = true;
    }
    return result;
}

IpAddress IpAddress::ipv6FromString(const std::string& aAddress) {
    return ipv6FromString(aAddress.c_str());
}

bool IpAddress::isValid() const {
    return _isValid;
}

AddressFamily IpAddress::getAddressFamily() const {
    return _addressFamily;
}

std::string IpAddress::toString() const {
    char buf[50];
    const char* res = (!_isValid) ? nullptr :
        zts_inet_ntop((_addressFamily == AddressFamily::IPv4) ? ZTS_AF_INET : ZTS_AF_INET6,
                      _addressBuf, buf, sizeof(buf) / sizeof(buf[0]));

    return res ? std::string{res} : std::string{"<Invalid IP address>"};
}

std::uint32_t IpAddress::getIPv4AddressInNetworkOrder() const {
    return _addressBuf[0];
}

RawIPv6Address IpAddress::getIPv6AddressInNetworkOrder() const {
    RawIPv6Address result;
    std::memcpy(&(result.bytes[0]), _addressBuf, sizeof(std::uint32_t) * 4);
    return result;
}

bool operator==(const IpAddress& aLeft, const IpAddress& aRight) {
    return (aLeft._addressFamily == aRight._addressFamily) &&
           (std::memcmp(aLeft._addressBuf, aRight._addressBuf, sizeof(std::uint32_t) * 4) == 0);
}

bool operator!=(const IpAddress& aLeft, const IpAddress& aRight) {
    return !(aLeft == aRight);
}

std::ostream& operator<<(std::ostream& aStream, const IpAddress& aAddress) {
    return (aStream << aAddress.toString());
}

} // namespace ztcpp
} // namespace jbatnozic