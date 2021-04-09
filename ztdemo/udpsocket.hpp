#ifndef ZTCPP_UDPSOCKET_HPP
#define ZTCPP_UDPSOCKET_HPP

#include "ipaddress.hpp"
#include "result.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>

#define ZTCPP_API

namespace jbatnozic {
namespace ztcpp {

enum class SocketDomain {
    InternetProtocol_IPv4 = 0,
    InternetProtocol_IPv6,
    ElementCount // Keep last
};

class ZTCPP_API UdpSocket {
public:
    UdpSocket();
    ~UdpSocket();

    EmptyResult init(SocketDomain aSocketDomain);

    EmptyResult bind(const IpAddress& aLocalIpAddress, std::uint16_t aLocalPortInHostOrder);

    Result<std::size_t> send(const void* aData,
                             std::size_t aDataByteSize,
                             const IpAddress& aRemoteIpAddress,
                             std::uint16_t aRemotePortInHostOrder);

    Result<std::size_t> receive(void* aDestinationBuffer,
                                std::size_t aDestinationBufferByteSize,
                                IpAddress& aSenderAddress, 
                                std::uint16_t& aSenderPort);

    EmptyResult close();

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};

} // namespace ztcpp
} // namespace jbatnozic

#endif // !ZTCPP_UDPSOCKET_HPP