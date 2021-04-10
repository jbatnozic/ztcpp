#ifndef ZTCPP_SOCKET_HPP
#define ZTCPP_SOCKET_HPP

#include <ZTCpp/Definitions.hpp>
#include <ZTCpp/Ip_address.hpp>
#include <ZTCpp/Result.hpp>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>

ZTCPP_NAMESPACE_BEGIN

enum class SocketDomain {
  InternetProtocol_IPv4,
  InternetProtocol_IPv6
};

enum class SocketType {
  Stream,   //! TCP
  Datagram, //! UDP
  Raw       //! Raw
};

struct PollEventBitmask {
  enum Enum {
    ReadyToReceive             = 1, //! Can call receive() or receiveFrom() without blocking
    ReadyToSend                = 2, //! Can call send() or sendTo() without blocking
    ReadyToReceivePriorityData = 4  //! Same as ReadyToReceive but for data deemed 'more important'
  };
};

class ZTCPP_API Socket {
public:
  Socket();
  ~Socket();

  EmptyResult init(SocketDomain aSocketDomain, SocketType aSocketType);

  EmptyResult bind(const IpAddress& aLocalIpAddress, uint16_t aLocalPortInHostOrder);

  EmptyResult connect(const IpAddress& aRemoteIpAddress,
                      uint16_t aRemotePortInHostOrder);

  EmptyResult listen(); // TODO

  EmptyResult accept(); // TODO

  // TODO: send

  Result<std::size_t> sendTo(const void* aData,
                             std::size_t aDataByteSize,
                             const IpAddress& aRemoteIpAddress,
                             uint16_t aRemotePortInHostOrder);

  // TODO: receive

  Result<std::size_t> receiveFrom(void* aDestinationBuffer,
                                  std::size_t aDestinationBufferByteSize,
                                  IpAddress& aSenderAddress,
                                  uint16_t& aSenderPort);

  // Select?

  //! On success, compare with PollEventBitmask::Enum to see which events have occurred
  //! If aMaxTimeToWait is 0, return immediately. If it is negative, waits indefinitely until an event occurs
  Result<int> pollEvents(std::chrono::milliseconds aMaxTimeToWait = std::chrono::milliseconds{0}) const;

  Result<IpAddress> getLocalIpAddress() const; // TODO
  Result<uint16_t> getLocalPort() const; // TODO
  // TODO remote IP/port

  EmptyResult close();

  // Control commands
  // TODO
  EmptyResult setNonBlocking(bool aNonBlocking);
  Result<bool> getNonBlocking() const;

private:
  class Impl;
  std::unique_ptr<Impl> _impl;
};

ZTCPP_NAMESPACE_END

#endif // !ZTCPP_SOCKET_HPP