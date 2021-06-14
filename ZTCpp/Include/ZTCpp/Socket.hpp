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

//! Used for pollEvents() methods of the Socket class (see below).
struct PollEventBitmask {
  enum Enum {
    ReadyToReceive             = 1, //! Can call receive() or receiveFrom() without blocking
    ReadyToSend                = 2, //! Can call send() or sendTo() without blocking
    ReadyToReceivePriorityData = 4, //! Same as ReadyToReceive but for out-of-bound data

    ReadyToAccept              = ReadyToReceive, //! Can call accept() without blocking
    ReadyToReceiveAny          = ReadyToReceive | ReadyToReceivePriorityData,
    AnyEvent                   = ReadyToReceiveAny | ReadyToSend
  };
};

class ZTCPP_API Socket {
public:
  //! Creates an uninitialized socket.
  Socket();

  //! Regular destructor.
  ~Socket();

  //! TODO (doc)
  EmptyResult init(SocketDomain aSocketDomain, SocketType aSocketType);

  //! Bind the socket to a local address and port.
  //! Note: The socket will work even if bound to an unspecified address.
  EmptyResult bind(const IpAddress& aLocalIpAddress, uint16_t aLocalPortInHostOrder);

  //! Connects the socket to a remote host. Mandatory for TCP sockets before use.
  //! For UDP sockets, the only thing it does is enable calling() send and receive()
  //! instead of sendTo() and receiveFrom(), and it will fill out the missing
  //! information automatically.
  EmptyResult connect(const IpAddress& aRemoteIpAddress,
                      uint16_t aRemotePortInHostOrder);

  //! Set the socket in a listening state
  EmptyResult listen(std::size_t aMaxQueueSize); // TODO

  //! Accept a new connection and return a new socket to handle that new connection.
  //! Blocks until a new connection is available (you can pollEvent for 
  //! PollEventBitmask::ReadyToAccept if you want to avoid blocking).
  //! Only works for Stream (TCP) sockets and always fails on others.
  //! TODO (implementation)
  Result<Socket> accept(); 

  //! Sends data to a remote host.
  //! On success, return value = number of bytes sent
  Result<std::size_t> send(const void* aData,
                           std::size_t aDataByteSize);

  //! Sends data to a remote host.
  //! On success, return value = number of bytes sent
  Result<std::size_t> sendTo(const void* aData,
                             std::size_t aDataByteSize,
                             const IpAddress& aRemoteIpAddress,
                             uint16_t aRemotePortInHostOrder);

  //! Receive data from the a remote host.
  //! If the destination buffer is not large enough to hold the whole message that was
  //! received, it will be truncanted to fit and no error will be reported. Thus, unless
  //! you know in advance what kind of messages will be received, the safest approach 
  //! would be to provide as large a buffer as you can. Note that the theoretical limit
  //! for both TCP and UDP packet size is 64kB, so anything more that that is a certain 
  //! waste of memory.
  //! On successs, return value = number of bytes received (written to the buffer)
  Result<std::size_t> receive(void* aDestinationBuffer,
                              std::size_t aDestinationBufferByteSize);

  //! Same as receive() but also, on success, reports the sender's IP and port through 
  //! the last two arguments.
  Result<std::size_t> receiveFrom(void* aDestinationBuffer,
                                  std::size_t aDestinationBufferByteSize,
                                  IpAddress& aSenderAddress,
                                  uint16_t& aSenderPort);

  //! On success, compare with PollEventBitmask::Enum to see which events have occurred.
  //! Blocks until any event marked in aInterestedIn occurs, or until aMaxTimeToWait has
  //! passed. If aMaxTimeToWait is 0, return immediately. If it is negative, waits 
  //! indefinitely until an event occurs
  Result<int> pollEvents(
      PollEventBitmask::Enum aInterestedIn = PollEventBitmask::AnyEvent,
      std::chrono::milliseconds aMaxTimeToWait = std::chrono::milliseconds{0}) const;

  //! Return the address to which the socket was bound.
  //! Will return an all-zero address for an unbound socket.
  Result<IpAddress> getLocalIpAddress() const;

  //! Return the port number to which the socket was bound.
  //! Will return port 0 for an unbound socket.
  Result<uint16_t> getLocalPort() const;

  //! If the socket is connected to a remote, return the remote's IP address.
  //! Otherwise, returns an error.
  Result<IpAddress> getRemoteIpAddress() const;

  //! If the socket is connected to a remote, return the remote's port number.
  //! Otherwise, returns an error.
  Result<uint16_t> getRemotePort() const;

  //! Return true if the socket was initialized successfully and is ready to send
  //! and receive traffic. Once close() is called, isOpen() will return false again.
  bool isOpen() const;

  //! Close the socket. This method returns the socket into its initial state.
  //! The socket can become functional again if you call init().
  EmptyResult close();

  // Control commands
  // TODO - RDONLY, WRONLY, send/recv flags...

private:
  class Impl;
  std::unique_ptr<Impl> _impl;
};

ZTCPP_NAMESPACE_END

#endif // !ZTCPP_SOCKET_HPP