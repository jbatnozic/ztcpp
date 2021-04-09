
#include "udpsocket.hpp"

#include <ZeroTierSockets.h>

#if defined(_WIN32)
#include <winsock.h>
#else
include internet stuff for htons/ntohs (TODO)
#endif

namespace jbatnozic {
namespace ztcpp {
namespace {
// doesn't check aIpAddress validity, take care
struct zts_sockaddr_storage ToSockaddr(const IpAddress& aIpAddress, std::uint16_t aPortInHostOrder) {
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

    default: void(); // Do nothing
    }

    return {};
}
} // namespace

class UdpSocket::Impl {
public:
    Impl() = default;

    ~Impl() {
        close();
    }

    EmptyResult init(SocketDomain aSocketDomain) {
        if (aSocketDomain < SocketDomain::InternetProtocol_IPv4 ||
            aSocketDomain >= SocketDomain::ElementCount) {
            return {ZTCPP_ERROR_REPORT(ArgumentError,
                                       "aSocketDomain has invalid value")};
        }

        _socketDomain = aSocketDomain;
        _socketID = zts_socket(getZTAddressFamily(), ZTS_SOCK_DGRAM, getZTProtocolFamily());

        if (_socketID == ZTS_ERR_SOCKET) {
            return {ZTCPP_ERROR_REPORT(SocketError,
                                       "ZTS_ERR_SOCKET (errno=" + std::to_string(zts_errno) + ")")};
        }
        if (_socketID == ZTS_ERR_SERVICE) {
            return {ZTCPP_ERROR_REPORT(ServiceError,
                                       "ZTS_ERR_SERVICE (errno=" + std::to_string(zts_errno) + ")")};
        }

        return EmptyResultOK();
    }

    EmptyResult bind(const IpAddress& aLocalIpAddress, std::uint16_t aLocalPortInHostOrder) {
        // TODO (check valid)

        if (!aLocalIpAddress.isValid()) {
            return {ZTCPP_ERROR_REPORT(ArgumentError,
                                       "aLocalIpAddress is invalid")};
        }
        if (aLocalIpAddress.getAddressFamily() != getAddressFamily()) {
            return {ZTCPP_ERROR_REPORT(ArgumentError,
                                       "aLocalIpAddress is of wrong address family")};
        }

        const auto sockaddr = ToSockaddr(aLocalIpAddress, aLocalPortInHostOrder);
        const auto res = zts_bind(_socketID, 
                                  reinterpret_cast<const struct zts_sockaddr*>(&sockaddr), 
                                  sizeof(sockaddr));

        if (res == ZTS_ERR_OK) {
            return EmptyResultOK();
        }

        if (res == ZTS_ERR_SOCKET) {
            return {ZTCPP_ERROR_REPORT(SocketError,
                                       "ZTS_ERR_SOCKET (errno=" + std::to_string(zts_errno) + ")")};
        }
        if (res == ZTS_ERR_SERVICE) {
            return {ZTCPP_ERROR_REPORT(ServiceError,
                                       "ZTS_ERR_SERVICE (errno=" + std::to_string(zts_errno) + ")")};
        }
        if (res == ZTS_ERR_ARG) {
            return {ZTCPP_ERROR_REPORT(ArgumentError,
                                       "ZTS_ERR_ARG (errno=" + std::to_string(zts_errno) + ")")};
        }

        return {ZTCPP_ERROR_REPORT(RuntimeError,
                                   "Unknown error (zts_sendto returned " + std::to_string(res) +
                                   ", errno= " + std::to_string(zts_errno) + ")")};
    }

    Result<std::size_t> send(const void* aData,
                             std::size_t aDataByteSize,
                             const IpAddress& aRemoteIpAddress,
                             std::uint16_t aLocalPortInHostOrder) {
        // TODO (check valid)

        if (aData == nullptr || aDataByteSize == 0) {
            return {ZTCPP_ERROR_REPORT(ArgumentError,
                                       "aData is null or aDataByteSize == 0")};
        }
        if (!aRemoteIpAddress.isValid()) {
            return {ZTCPP_ERROR_REPORT(ArgumentError,
                                       "aRemoteIpAddress is invalid")};
        }
        if (aRemoteIpAddress.getAddressFamily() != getAddressFamily()) {
            return {ZTCPP_ERROR_REPORT(ArgumentError,
                                       "aRemoteIpAddress is of wrong address family")};
        }

        const auto sockaddr = ToSockaddr(aRemoteIpAddress, aLocalPortInHostOrder);
        const auto byteCount = zts_sendto(_socketID, 
                                          aData, aDataByteSize, 
                                          0, 
                                          reinterpret_cast<const struct zts_sockaddr*>(&sockaddr), 
                                          sizeof(sockaddr));

        if (byteCount == static_cast<decltype(byteCount)>(aDataByteSize)) {
            return {aDataByteSize};
        }
        if (byteCount == ZTS_ERR_SOCKET) {
            return {ZTCPP_ERROR_REPORT(SocketError,
                                       "ZTS_ERR_SOCKET (errno=" + std::to_string(zts_errno) + ")")};
        }
        if (byteCount == ZTS_ERR_SERVICE) {
            return {ZTCPP_ERROR_REPORT(ServiceError,
                                       "ZTS_ERR_SERVICE (errno=" + std::to_string(zts_errno) + ")")};
        }
        if (byteCount == ZTS_ERR_ARG) {
            return {ZTCPP_ERROR_REPORT(ArgumentError,
                                       "ZTS_ERR_ARG (errno=" + std::to_string(zts_errno) + ")")};
        }

        return {ZTCPP_ERROR_REPORT(RuntimeError,
                                   "Unknown error (zts_sendto returned " + std::to_string(byteCount) +
                                   ", errno= " + std::to_string(zts_errno) + ")")};
    }

    Result<std::size_t> receive(void* aDestinationBuffer,
                                std::size_t aDestinationBufferByteSize,
                                IpAddress& aSenderAddress,
                                std::uint16_t& aSenderPort) {
        // TODO (check valid)

        if (aDestinationBuffer == nullptr || aDestinationBufferByteSize == 0) {
            return {ZTCPP_ERROR_REPORT(ArgumentError,
                                       "aDestinationBuffer is null or aDestinationBufferByteSize == 0")};
        }

        struct zts_sockaddr senderSockaddr;
        zts_socklen_t senderSockaddrLen;

        const auto byteCount = zts_recvfrom(_socketID,
                                            aDestinationBuffer, aDestinationBufferByteSize,
                                            0, &senderSockaddr, &senderSockaddrLen);

        // TODO fill in aSenderAddress, aSenderPort

        if (byteCount > 0) {
            return {static_cast<std::size_t>(byteCount)};
        }
        if (byteCount == ZTS_ERR_SOCKET) {
            return {ZTCPP_ERROR_REPORT(SocketError,
                                       "ZTS_ERR_SOCKET (errno=" + std::to_string(zts_errno) + ")")};
        }
        if (byteCount == ZTS_ERR_SERVICE) {
            return {ZTCPP_ERROR_REPORT(ServiceError,
                                       "ZTS_ERR_SERVICE (errno=" + std::to_string(zts_errno) + ")")};
        }
        if (byteCount == ZTS_ERR_ARG) {
            return {ZTCPP_ERROR_REPORT(ArgumentError,
                                       "ZTS_ERR_ARG (errno=" + std::to_string(zts_errno) + ")")};
        }

        return {ZTCPP_ERROR_REPORT(RuntimeError,
                                   "Unknown error (zts_recvfrom returned " + std::to_string(byteCount) +
                                   ", errno= " + std::to_string(zts_errno) + ")")};
    }

    EmptyResult close() {
        if (_socketID >= 0) {
            const auto res = zts_close(_socketID);
            _socketID = -0xDEAD;

            if (res == ZTS_ERR_SOCKET) {
                return {ZTCPP_ERROR_REPORT(SocketError,
                                           "ZTS_ERR_SOCKET (errno=" + std::to_string(zts_errno) + ")")};
            }
            if (res == ZTS_ERR_SERVICE) {
                return {ZTCPP_ERROR_REPORT(ServiceError,
                                           "ZTS_ERR_SERVICE (errno=" + std::to_string(zts_errno) + ")")};
            }
        }

        return EmptyResultOK();
    }

private:
    AddressFamily getAddressFamily() {
        switch (_socketDomain) {
        case SocketDomain::InternetProtocol_IPv4: return AddressFamily::IPv4;
        case SocketDomain::InternetProtocol_IPv6: return AddressFamily::IPv6;
        default: 
            // Safe to ignore
            void();        
        }
        // Unreachable
        return static_cast<AddressFamily>(0);
    }

    int getZTAddressFamily() const {
        return (_socketDomain == SocketDomain::InternetProtocol_IPv4) ?
            ZTS_AF_INET : ZTS_AF_INET6;
    }

    int getZTProtocolFamily() const {
        return (_socketDomain == SocketDomain::InternetProtocol_IPv4) ?
            ZTS_PF_INET : ZTS_PF_INET6;
    }

    // isValid
    SocketDomain _socketDomain;
    int _socketID;
};

UdpSocket::UdpSocket() 
    : _impl{std::make_unique<Impl>()}
{
}

UdpSocket::~UdpSocket() = default;

EmptyResult UdpSocket::init(SocketDomain aSocketDomain) {
    return _impl->init(aSocketDomain);
}

EmptyResult UdpSocket::bind(const IpAddress& aLocalIpAddress, std::uint16_t aLocalPortInHostOrder) {
    return _impl->bind(aLocalIpAddress, aLocalPortInHostOrder);
}

Result<std::size_t> UdpSocket::send(const void* aData,
                                    std::size_t aDataByteSize,
                                    const IpAddress& aRemoteIpAddress,
                                    std::uint16_t aRemotePortInHostOrder) {
    return _impl->send(aData, aDataByteSize, aRemoteIpAddress, aRemotePortInHostOrder);
}

EmptyResult UdpSocket::close() {
    return _impl->close();
}

} // namespace ztcpp
} // namespace jbatnozic