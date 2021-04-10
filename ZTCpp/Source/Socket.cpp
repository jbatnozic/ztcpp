
#include <ZTCpp/Socket.hpp>

#include "Sockaddr_util.hpp"

#include <ZeroTierSockets.h>

ZTCPP_NAMESPACE_BEGIN

class Socket::Impl {
public:
  Impl() = default;

  ~Impl() {
    close();
  }

  EmptyResult init(SocketDomain aSocketDomain, SocketType aSocketType) {
    if (aSocketDomain != SocketDomain::InternetProtocol_IPv4 &&
        aSocketDomain != SocketDomain::InternetProtocol_IPv6) {
      return {ZTCPP_ERROR_REPORT(ArgumentError,
                                 "aSocketDomain has invalid value")};
    }

    if (aSocketType != SocketType::Stream &&
        aSocketType != SocketType::Datagram &&
        aSocketType != SocketType::Raw) {
      return {ZTCPP_ERROR_REPORT(ArgumentError,
                                 "aSocketType has invalid value")};
    }

    _socketDomain = aSocketDomain;
    _socketType = aSocketType;
    _socketID = zts_socket(getZTAddressFamily(), getZTSocketType(), getZTProtocolFamily());

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

  EmptyResult bind(const IpAddress& aLocalIpAddress, uint16_t aLocalPortInHostOrder) {
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

  Result<std::size_t> sendTo(const void* aData,
                             std::size_t aDataByteSize,
                             const IpAddress& aRemoteIpAddress,
                             uint16_t aLocalPortInHostOrder) {
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

  Result<std::size_t> receiveFrom(void* aDestinationBuffer,
                                  std::size_t aDestinationBufferByteSize,
                                  IpAddress& aSenderAddress,
                                  uint16_t& aSenderPort) {
    // TODO (check valid)

    if (aDestinationBuffer == nullptr || aDestinationBufferByteSize == 0) {
      return {ZTCPP_ERROR_REPORT(ArgumentError,
                                 "aDestinationBuffer is null or aDestinationBufferByteSize == 0")};
    }

    struct zts_sockaddr_storage senderSockaddr;
    zts_socklen_t senderSockaddrLen = sizeof(senderSockaddr);

    const auto byteCount = zts_recvfrom(_socketID,
                                        aDestinationBuffer, aDestinationBufferByteSize,
                                        0, 
                                        reinterpret_cast<struct zts_sockaddr*>(&senderSockaddr), 
                                        &senderSockaddrLen);

    detail::ToIpAddressAndPort(reinterpret_cast<struct zts_sockaddr_storage*>(&senderSockaddr),
                               aSenderAddress, aSenderPort);

    if (byteCount > 0) {
      return {static_cast<std::size_t>(byteCount)};
    }
    if (byteCount == ZTS_ERR_SOCKET) {
      if (zts_errno == 140) {
        return {static_cast<std::size_t>(0)};
      }
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
      _socketID = ZTS_ERR_SOCKET;

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

  Result<int> pollEvents(std::chrono::milliseconds aMaxTimeToWait) const {
		struct zts_pollfd pollfd;
    pollfd.fd = _socketID;
    pollfd.events = ZTS_POLLIN | ZTS_POLLOUT | ZTS_POLLPRI;

    const int res = zts_poll(&pollfd, 1, static_cast<int>(aMaxTimeToWait.count()));

    if (res == ZTS_ERR_SOCKET) {
      return {ZTCPP_ERROR_REPORT(SocketError,
                                 "ZTS_ERR_SOCKET (errno=" + std::to_string(zts_errno) + ")")};
    }
    if (res == ZTS_ERR_SERVICE) {
      return {ZTCPP_ERROR_REPORT(ServiceError,
                                 "ZTS_ERR_SERVICE (errno=" + std::to_string(zts_errno) + ")")};
    }
    if (res != 1) {
      return {ZTCPP_ERROR_REPORT(GenericError,
                                 "Unspecified error from zts_poll (errno=" + std::to_string(zts_errno) + ")")};
    }
    if (pollfd.revents & ZTS_POLLNVAL) {
      return {ZTCPP_ERROR_REPORT(SocketError,
                                 "zts_poll returned ZTS_POLLNVAL (something wrong with socket descriptor)")};
    }
    if (pollfd.revents & ZTS_POLLHUP) {
      return {ZTCPP_ERROR_REPORT(SocketError,
                                 "zts_poll returned ZTS_POLLHUP (remote side of the connection hung up)")};
    }
    if (pollfd.revents & ZTS_POLLERR) {
      return {ZTCPP_ERROR_REPORT(SocketError,
                                 "zts_poll returned ZTS_POLLERR")};
    }

    int result = 0;
    if (pollfd.revents & ZTS_POLLIN)  result |= PollEventBitmask::ReadyToReceive;
    if (pollfd.revents & ZTS_POLLOUT) result |= PollEventBitmask::ReadyToSend;
    if (pollfd.revents & ZTS_POLLPRI) result |= PollEventBitmask::ReadyToReceivePriorityData;

    return {result};
  }

  EmptyResult setNonBlocking(bool aNonBlocking) {
    int flags = zts_fcntl(_socketID, ZTS_F_GETFL, 0);
    if (flags < 0) {
      return {ZTCPP_ERROR_REPORT(GenericError, 
                                 "Unspecified zts_fcntl() failure (" + std::to_string(flags) + ")")};
    }

    if (aNonBlocking) {
      flags |= ZTS_O_NONBLOCK;
    } 
    else {
      flags &= ~ZTS_O_NONBLOCK;
    }

    const int res = zts_fcntl(_socketID, ZTS_F_SETFL, flags);
    if (res < 0) {
      return {ZTCPP_ERROR_REPORT(GenericError, 
                                 "Unspecified zts_fcntl() failure (" + std::to_string(res) + ")")};
    }

    return EmptyResultOK();
  }

  Result<bool> getNonBlocking() const {
    const int res = zts_fcntl(_socketID, ZTS_F_GETFL, 0);
    if (res < 0) {
      return {ZTCPP_ERROR_REPORT(GenericError, 
                                 "Unspecified zts_fcntl() failure (" + std::to_string(res) + ")")};
    }
    return {(res & ZTS_O_NONBLOCK) != 0};
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

  int getZTSocketType() const {
    return (_socketType == SocketType::Stream)   ? ZTS_SOCK_STREAM :
           (_socketType == SocketType::Datagram) ? ZTS_SOCK_DGRAM  : ZTS_SOCK_RAW;
  }

  int getZTProtocolFamily() const {
    return (_socketDomain == SocketDomain::InternetProtocol_IPv4) ?
      ZTS_PF_INET : ZTS_PF_INET6;
  }

  SocketDomain _socketDomain = static_cast<SocketDomain>(-1);
  SocketType _socketType = static_cast<SocketType>(-1);
  int _socketID = ZTS_ERR_SOCKET;
};

Socket::Socket()
  : _impl{std::make_unique<Impl>()}
{
}

Socket::~Socket() = default;

EmptyResult Socket::init(SocketDomain aSocketDomain, SocketType aSocketType) {
  return _impl->init(aSocketDomain, aSocketType);
}

EmptyResult Socket::bind(const IpAddress & aLocalIpAddress, uint16_t aLocalPortInHostOrder) {
  return _impl->bind(aLocalIpAddress, aLocalPortInHostOrder);
}

Result<std::size_t> Socket::sendTo(const void* aData,
                                   std::size_t aDataByteSize,
                                   const IpAddress & aRemoteIpAddress,
                                   uint16_t aRemotePortInHostOrder) {
  return _impl->sendTo(aData, aDataByteSize, aRemoteIpAddress, aRemotePortInHostOrder);
}

Result<std::size_t> Socket::receiveFrom(void* aDestinationBuffer,
                                        std::size_t aDestinationBufferByteSize,
                                        IpAddress& aSenderAddress,
                                        uint16_t& aSenderPort) {
  return _impl->receiveFrom(aDestinationBuffer, aDestinationBufferByteSize, aSenderAddress, aSenderPort);
}

EmptyResult Socket::close() {
  return _impl->close();
}

Result<int> Socket::pollEvents(std::chrono::milliseconds aMaxTimeToWait) const {
  return _impl->pollEvents(aMaxTimeToWait);
}

EmptyResult Socket::setNonBlocking(bool aNonBlocking) {
  return _impl->setNonBlocking(aNonBlocking);
}

Result<bool> Socket::getNonBlocking() const {
  return _impl->getNonBlocking();
}

ZTCPP_NAMESPACE_END