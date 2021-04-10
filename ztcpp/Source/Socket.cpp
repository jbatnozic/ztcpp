
#include <ZTCpp/Socket.hpp>

#include "Sockaddr_util.hpp"

#include <ZeroTierSockets.h>

ZTCPP_NAMESPACE_BEGIN

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

        const auto sockaddr = detail::ToSockaddr(aLocalIpAddress, aLocalPortInHostOrder);
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

        const auto sockaddr = detail::ToSockaddr(aRemoteIpAddress, aLocalPortInHostOrder);
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

ZTCPP_NAMESPACE_END