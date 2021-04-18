
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
                                 "ZTS_ERR_SOCKET (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (_socketID == ZTS_ERR_SERVICE) {
      return {ZTCPP_ERROR_REPORT(ServiceError,
                                 "ZTS_ERR_SERVICE (zts_errno=" + std::to_string(zts_errno) + ")")};
    }

    return EmptyResultOK();
  }

  EmptyResult bind(const IpAddress& aLocalIpAddress, uint16_t aLocalPortInHostOrder) {
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
                                 "ZTS_ERR_SOCKET (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (res == ZTS_ERR_SERVICE) {
      return {ZTCPP_ERROR_REPORT(ServiceError,
                                 "ZTS_ERR_SERVICE (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (res == ZTS_ERR_ARG) {
      return {ZTCPP_ERROR_REPORT(ArgumentError,
                                 "ZTS_ERR_ARG (zts_errno=" + std::to_string(zts_errno) + ")")};
    }

    return {ZTCPP_ERROR_REPORT(GenericError,
                               "Unknown error (zts_sendto returned " + std::to_string(res) +
                               ", zts_errno= " + std::to_string(zts_errno) + ")")};
  }

  Result<std::size_t> sendTo(const void* aData,
                             std::size_t aDataByteSize,
                             const IpAddress& aRemoteIpAddress,
                             uint16_t aLocalPortInHostOrder) {
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
                                 "ZTS_ERR_SOCKET (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (byteCount == ZTS_ERR_SERVICE) {
      return {ZTCPP_ERROR_REPORT(ServiceError,
                                 "ZTS_ERR_SERVICE (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (byteCount == ZTS_ERR_ARG) {
      return {ZTCPP_ERROR_REPORT(ArgumentError,
                                 "ZTS_ERR_ARG (zts_errno=" + std::to_string(zts_errno) + ")")};
    }

    return {ZTCPP_ERROR_REPORT(GenericError,
                               "Unknown error (zts_sendto returned " + std::to_string(byteCount) +
                               ", zts_errno= " + std::to_string(zts_errno) + ")")};
  }

  Result<std::size_t> receiveFrom(void* aDestinationBuffer,
                                  std::size_t aDestinationBufferByteSize,
                                  IpAddress& aSenderAddress,
                                  uint16_t& aSenderPort) {
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
      return {ZTCPP_ERROR_REPORT(SocketError,
                                 "ZTS_ERR_SOCKET (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (byteCount == ZTS_ERR_SERVICE) {
      return {ZTCPP_ERROR_REPORT(ServiceError,
                                 "ZTS_ERR_SERVICE (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (byteCount == ZTS_ERR_ARG) {
      return {ZTCPP_ERROR_REPORT(ArgumentError,
                                 "ZTS_ERR_ARG (zts_errno=" + std::to_string(zts_errno) + ")")};
    }

    return {ZTCPP_ERROR_REPORT(GenericError,
                               "Unknown error (zts_recvfrom returned " + std::to_string(byteCount) +
                               ", zts_errno= " + std::to_string(zts_errno) + ")")};
  }

  bool isOpen() const {
    return (_socketID >= 0);
  }

  EmptyResult close() {
    if (isOpen()) {
      const auto res = zts_close(_socketID);
      _socketID = ZTS_ERR_SOCKET;

      if (res == ZTS_ERR_SOCKET) {
        return {ZTCPP_ERROR_REPORT(SocketError,
                                   "ZTS_ERR_SOCKET (zts_errno=" + std::to_string(zts_errno) + ")")};
      }
      if (res == ZTS_ERR_SERVICE) {
        return {ZTCPP_ERROR_REPORT(ServiceError,
                                   "ZTS_ERR_SERVICE (zts_errno=" + std::to_string(zts_errno) + ")")};
      }
    }

    return EmptyResultOK();
  }

  Result<int> pollEvents(PollEventBitmask::Enum aInterestedIn,
                         std::chrono::milliseconds aMaxTimeToWait) const {
    if (aInterestedIn == 0) {
      return {ZTCPP_ERROR_REPORT(ArgumentError,
                                 "aInterestedIn was 0")};
    }

		struct zts_pollfd pollfd;
    pollfd.fd = _socketID;
    pollfd.events = 0;
    pollfd.events |= ((aInterestedIn & PollEventBitmask::ReadyToReceive) != 0)             ? ZTS_POLLIN  : 0;
    pollfd.events |= ((aInterestedIn & PollEventBitmask::ReadyToSend) != 0)                ? ZTS_POLLOUT : 0;
    pollfd.events |= ((aInterestedIn & PollEventBitmask::ReadyToReceivePriorityData) != 0) ? ZTS_POLLPRI : 0;

    const int pollres = zts_poll(&pollfd, 1, static_cast<int>(aMaxTimeToWait.count()));

    if (pollres == ZTS_ERR_SOCKET) {
      return {ZTCPP_ERROR_REPORT(SocketError,
                                 "ZTS_ERR_SOCKET (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (pollres == ZTS_ERR_SERVICE) {
      return {ZTCPP_ERROR_REPORT(ServiceError,
                                 "ZTS_ERR_SERVICE (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (pollres != 1 && pollres != 0) {
      return {ZTCPP_ERROR_REPORT(GenericError,
                                 "Unspecified error from zts_poll (zts_errno=" + std::to_string(zts_errno) + ")")};
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
    // If zts_poll returned 0, it means the socket wasn't 
    // ready for any of the events we were interested in
    if (pollres != 0) {
      if (pollfd.revents & ZTS_POLLIN)  result |= PollEventBitmask::ReadyToReceive;
      if (pollfd.revents & ZTS_POLLOUT) result |= PollEventBitmask::ReadyToSend;
      if (pollfd.revents & ZTS_POLLPRI) result |= PollEventBitmask::ReadyToReceivePriorityData;
    }

    return {result};
  }

  Result<IpAddress> getLocalIpAddress() const {
    struct zts_sockaddr_storage localAddress;
    zts_socklen_t localAddressLen = sizeof(localAddress);
    const int res = zts_getsockname(_socketID,
                                    reinterpret_cast<struct zts_sockaddr*>(&localAddress),
                                    &localAddressLen);

    if (res == ZTS_ERR_OK) {
      IpAddress result;
      uint16_t dummyPort;
      detail::ToIpAddressAndPort(&localAddress, result, dummyPort);
      return {result};
    }
    if (res == ZTS_ERR_SOCKET) {
      return {ZTCPP_ERROR_REPORT(SocketError,
                                 "ZTS_ERR_SOCKET (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (res == ZTS_ERR_SERVICE) {
      return {ZTCPP_ERROR_REPORT(ServiceError,
                                 "ZTS_ERR_SERVICE (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (res == ZTS_ERR_ARG) {
      return {ZTCPP_ERROR_REPORT(ArgumentError,
                                 "ZTS_ERR_ARG (zts_errno=" + std::to_string(zts_errno) + ")")};
    }

    return {ZTCPP_ERROR_REPORT(GenericError,
                               "Unknown error (zts_getsockname returned " + std::to_string(res) +
                               ", zts_errno= " + std::to_string(zts_errno) + ")")};
  }

  Result<uint16_t> getLocalPort() const {
    struct zts_sockaddr_storage localAddress;
    zts_socklen_t localAddressLen = sizeof(localAddress);
    const int res = zts_getsockname(_socketID,
                                    reinterpret_cast<struct zts_sockaddr*>(&localAddress),
                                    &localAddressLen);

    if (res == ZTS_ERR_OK) {
      IpAddress dummyAddress;
      uint16_t result;
      detail::ToIpAddressAndPort(&localAddress, dummyAddress, result);
      return {result};
    }
    if (res == ZTS_ERR_SOCKET) {
      return {ZTCPP_ERROR_REPORT(SocketError,
                                 "ZTS_ERR_SOCKET (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (res == ZTS_ERR_SERVICE) {
      return {ZTCPP_ERROR_REPORT(ServiceError,
                                 "ZTS_ERR_SERVICE (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (res == ZTS_ERR_ARG) {
      return {ZTCPP_ERROR_REPORT(ArgumentError,
                                 "ZTS_ERR_ARG (zts_errno=" + std::to_string(zts_errno) + ")")};
    }

    return {ZTCPP_ERROR_REPORT(GenericError,
                               "Unknown error (zts_getsockname returned " + std::to_string(res) +
                               ", zts_errno= " + std::to_string(zts_errno) + ")")};
  }

  Result<IpAddress> getRemoteIpAddress() const {
    struct zts_sockaddr_storage localAddress;
    zts_socklen_t localAddressLen = sizeof(localAddress);
    const int res = zts_getpeername(_socketID,
                                    reinterpret_cast<struct zts_sockaddr*>(&localAddress),
                                    &localAddressLen);

    if (res == ZTS_ERR_OK) {
      IpAddress result;
      uint16_t dummyPort;
      detail::ToIpAddressAndPort(&localAddress, result, dummyPort);
      return {result};
    }
    if (res == ZTS_ERR_SOCKET) {
      return {ZTCPP_ERROR_REPORT(SocketError,
                                 "ZTS_ERR_SOCKET (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (res == ZTS_ERR_SERVICE) {
      return {ZTCPP_ERROR_REPORT(ServiceError,
                                 "ZTS_ERR_SERVICE (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (res == ZTS_ERR_ARG) {
      return {ZTCPP_ERROR_REPORT(ArgumentError,
                                 "ZTS_ERR_ARG (zts_errno=" + std::to_string(zts_errno) + ")")};
    }

    return {ZTCPP_ERROR_REPORT(GenericError,
                               "Unknown error (zts_getsockname returned " + std::to_string(res) +
                               ", zts_errno= " + std::to_string(zts_errno) + ")")};
  }

  Result<uint16_t> getRemotePort() const {
    struct zts_sockaddr_storage localAddress;
    zts_socklen_t localAddressLen = sizeof(localAddress);
    const int res = zts_getpeername(_socketID,
                                    reinterpret_cast<struct zts_sockaddr*>(&localAddress),
                                    &localAddressLen);

    if (res == ZTS_ERR_OK) {
      IpAddress dummyAddress;
      uint16_t result;
      detail::ToIpAddressAndPort(&localAddress, dummyAddress, result);
      return {result};
    }
    if (res == ZTS_ERR_SOCKET) {
      return {ZTCPP_ERROR_REPORT(SocketError,
                                 "ZTS_ERR_SOCKET (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (res == ZTS_ERR_SERVICE) {
      return {ZTCPP_ERROR_REPORT(ServiceError,
                                 "ZTS_ERR_SERVICE (zts_errno=" + std::to_string(zts_errno) + ")")};
    }
    if (res == ZTS_ERR_ARG) {
      return {ZTCPP_ERROR_REPORT(ArgumentError,
                                 "ZTS_ERR_ARG (zts_errno=" + std::to_string(zts_errno) + ")")};
    }

    return {ZTCPP_ERROR_REPORT(GenericError,
                               "Unknown error (zts_getpeername returned " + std::to_string(res) +
                               ", zts_errno= " + std::to_string(zts_errno) + ")")};
  }

#if 0
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
#endif

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

bool Socket::isOpen() const {
  return _impl->isOpen();
}

EmptyResult Socket::close() {
  return _impl->close();
}

Result<int> Socket::pollEvents(PollEventBitmask::Enum aInterestedIn,
                               std::chrono::milliseconds aMaxTimeToWait) const {
  return _impl->pollEvents(aInterestedIn, aMaxTimeToWait);
}

Result<IpAddress> Socket::getLocalIpAddress() const {
  return _impl->getLocalIpAddress();
}

Result<uint16_t> Socket::getLocalPort() const {
  return _impl->getLocalPort();
}

Result<IpAddress> Socket::getRemoteIpAddress() const {
  return _impl->getRemoteIpAddress();
}

Result<uint16_t> Socket::getRemotePort() const {
  return _impl->getRemotePort();
}

ZTCPP_NAMESPACE_END