/**
 * ztcpp API example
 */

#include <string>
#include <stdexcept>
#include <thread>
#include <iostream>

#include <ZTCpp.hpp>

namespace zt = jbatnozic::ztcpp;

class NodeInfo {
public:
  NodeInfo() = default;

  bool online = false;
  int networksJoinedCount = 0;
  uint64_t id = 0;
  zt::IpAddress ip4 = zt::IpAddress::ipv4Unspecified();
  zt::IpAddress ip6 = zt::IpAddress::ipv6Unspecified();
};

class ZeroTierEventHandler : public zt::IEventHandler {
public:
  ZeroTierEventHandler(NodeInfo& aNodeInfo)
    : _nodeInfo{aNodeInfo}
  {
  }

  void onAddressEvent(zt::EventCode::Address aEventCode, const zt::AddressDetails* aDetails) noexcept override {
    std::cout << zt::EventDescription(aEventCode, aDetails) << std::endl;

    if (aDetails && aEventCode == zt::EventCode::Address::AddedIPv4) {
      _nodeInfo.ip4 = aDetails->getIpAddress();
    }
  }

  void onNetworkEvent(zt::EventCode::Network aEventCode, const zt::NetworkDetails* aDetails) noexcept override {
    std::cout << zt::EventDescription(aEventCode, aDetails) << std::endl;

    if (aEventCode == zt::EventCode::Network::ReadyIPv4 || 
        aEventCode == zt::EventCode::Network::ReadyIPv6 ||
        aEventCode == zt::EventCode::Network::ReadyIPv4_IPv6) {
      _nodeInfo.networksJoinedCount += 1;
    }
    else if (aEventCode == zt::EventCode::Network::Down) {
      _nodeInfo.networksJoinedCount -= 1;
    }
  }

  void onNetworkInterfaceEvent(zt::EventCode::NetworkInterface aEventCode,
                               const zt::NetworkInterfaceDetails* aDetails) noexcept override{
    std::cout << zt::EventDescription(aEventCode, aDetails) << std::endl;
  }

  void onNetworkStackEvent(zt::EventCode::NetworkStack aEventCode,
                           const zt::NetworkStackDetails* aDetails) noexcept override {
    std::cout << zt::EventDescription(aEventCode, aDetails) << std::endl;
  }

  void onNodeEvent(zt::EventCode::Node aEventCode, const zt::NodeDetails* aDetails) noexcept override {
    std::cout << zt::EventDescription(aEventCode, aDetails) << std::endl;

    if (aEventCode == zt::EventCode::Node::Online) {
      _nodeInfo.online = true;
    }
    else if (aEventCode == zt::EventCode::Node::Online) {
      _nodeInfo.online = false;
    }
  }

  void onPeerEvent(zt::EventCode::Peer aEventCode, const zt::PeerDetails* aDetails) noexcept override {
    std::cout << zt::EventDescription(aEventCode, aDetails) << std::endl;
  }

  void onRouteEvent(zt::EventCode::Route aEventCode, const zt::RouteDetails* aDetails) noexcept override {
    std::cout << zt::EventDescription(aEventCode, aDetails) << std::endl;
  }

  void onUnknownEvent(int16_t aRawZeroTierEventCode) noexcept override {
    std::cout << "An unknown Zero Tier event was dispatched (" << aRawZeroTierEventCode << ")" << std::endl;
  }

private:
  NodeInfo& _nodeInfo;
};

NodeInfo localNode;
ZeroTierEventHandler eventHandler{localNode};

int main(int argc, char** argv)
try {
#if 1
  if (argc != 5) {
    printf("\ztcpp example server\n");
    printf("server <config_file_path> <nwid> <serverBindPort> <ztServicePort>\n");
    exit(0);
  }
  std::string identityPath = argv[1];
  uint64_t nwid = strtoull(argv[2], NULL, 16); // Network ID to join
  int serverBindPort = atoi(argv[3]); // Port the application should bind to
  int ztServicePort = atoi(argv[4]); // Port ZT uses to send encrypted UDP packets to peers (try something like 9994)
#else
  std::string identityPath = "ZTNodeData";
  uint64_t nwid = strtoull("a09acf0233ceff5c", NULL, 16); // Network ID to join
  int serverBindPort = 8890; // Port the application should bind to
  int ztServicePort = 9994; // Port ZT uses to send encrypted UDP packets to peers (try something like 9994)
#endif

  zt::SetEventHandler(&eventHandler);

  // Bring up ZeroTier service and join network

  printf("Configuring ZeroTier service...\n");
  {
    const auto res = zt::AllowNetworkCaching(true);
    ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
  }
  {
    const auto res = zt::AllowPeerCaching(true);
    ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
  }
  {
    const auto res = zt::AllowLocalConf(true);
    ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
  }

  printf("This node's identity is stored in %s\n", identityPath.c_str());

  printf("Starting ZeroTier service...\n");
  {
    const auto res = zt::StartService(identityPath, ztServicePort);
    ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
  }

  printf("Waiting for node to come online...\n");
  while (!localNode.online) { 
    std::this_thread::sleep_for(std::chrono::milliseconds{50});
  }
  
  printf("Joining network %llx\n", nwid);
  printf("Don't forget to authorize this device in my.zerotier.com or the web API!\n");
  {
    const auto res = zt::JoinNetwork(nwid);
    ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
  }
  
  while (localNode.networksJoinedCount <= 0) {
    printf("Waiting to join network (networksJoinedCount = %d)...\n", localNode.networksJoinedCount);
    std::this_thread::sleep_for(std::chrono::milliseconds{1000});
  }

  // Socket-like API example

  const char* msg = "Hello, me!";
  int bytes = 0;
  char recvBuf[128];
  memset(recvBuf, 0, sizeof(recvBuf));

  printf("Initializing socket...\n");
  zt::Socket socket;
  zt::Socket sock2;

  {
    auto res  = socket.init(zt::SocketDomain::InternetProtocol_IPv4, zt::SocketType::Datagram);
    auto res2 = sock2.init(zt::SocketDomain::InternetProtocol_IPv4, zt::SocketType::Datagram);
    ZTCPP_THROW_ON_ERROR(res,  std::runtime_error);
    ZTCPP_THROW_ON_ERROR(res2, std::runtime_error);
  }

  printf("Binding socket to port %d...\n", serverBindPort);
  {
    auto res = socket.bind(localNode.ip4, serverBindPort);
    ZTCPP_THROW_ON_ERROR(res, std::runtime_error);

    auto res2 = sock2.bind(localNode.ip4, 0);
    ZTCPP_THROW_ON_ERROR(res2, std::runtime_error);
  }

  {
    const auto ip   = socket.getLocalIpAddress();
    ZTCPP_THROW_ON_ERROR(ip, std::runtime_error);
    const auto port = socket.getLocalPort();
    ZTCPP_THROW_ON_ERROR(port, std::runtime_error);
    std::cout << "Local socket: " << *ip << ":" << *port << '\n';
  }

  printf("Receiving data...\n");
  for (int i = 0; i < 10; i += 1) {
    const auto pollres = socket.pollEvents(zt::PollEventBitmask::ReadyToReceiveAny);
    ZTCPP_THROW_ON_ERROR(pollres, std::runtime_error);
    const bool readyToReceive = 
      (*pollres & zt::PollEventBitmask::ReadyToReceiveAny) != 0;

    if (readyToReceive) {
      zt::IpAddress remoteIp;
      std::uint16_t remotePort;
      auto res = socket.receiveFrom(recvBuf, 128, remoteIp, remotePort);
      ZTCPP_THROW_ON_ERROR(res, std::runtime_error);

      std::cout << "Received " << *res << " bytes from " << remoteIp << ":" << remotePort << ";\n"
                << "    Message: " << recvBuf << "\n";
    }
    else {
      std::cout << "Server skipping recvfrom call - no data ready\n";
      auto res = sock2.sendTo(msg, std::strlen(msg), 
                              zt::IpAddress::ipv4FromString("127.0.0.1"), 
                              socket.getLocalPort());
      if (res) {
        printf("Sent %d bytes\n", static_cast<int>(*res));
      }
      else {
        printf("Send error: %s\n", res.getError().message.c_str());
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds{500});
  }
  
  printf("Closing socket\n");
  {
    auto res = socket.close();
    ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
  }

  printf("Shutting down service\n");
  zt::StopService();
  std::this_thread::sleep_for(std::chrono::milliseconds{1000});
  zt::SetEventHandler(nullptr);
  return EXIT_SUCCESS;
}
catch (std::exception& ex) {
  printf("Exception caught: %s\n", ex.what());
  printf("Shutting down service\n");
  zt::StopService();
  std::this_thread::sleep_for(std::chrono::milliseconds{1000});
  zt::SetEventHandler(nullptr);
}
catch (...) {
  printf("Unknown exception caught\n");
  printf("Shutting down service\n");
  zt::StopService();
  std::this_thread::sleep_for(std::chrono::milliseconds{1000});
  zt::SetEventHandler(nullptr);
}