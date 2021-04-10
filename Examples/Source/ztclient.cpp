/**
 * ztcpp API example
 */

#include <string>
#include <stdexcept>
#include <thread>
#include <iostream>

#include <Ztcpp.hpp>

namespace zt = jbatnozic::ztcpp;

class NodeInfo {
public:
	NodeInfo() = default;

	bool online = false;
	int networksJoinedCount = 0;
	uint64_t id = 0;
};

/* Callback handler, you should return control from this function as quickly as you can
to ensure timely receipt of future events. You should not call libzt API functions from
this function unless it's something trivial like zts_inet_ntop() or similar that has
no state-change implications. */
#if 0
void on_zts_event(void* msgPtr)
{
	struct zts_callback_msg* msg = (struct zts_callback_msg*)msgPtr;
	// Peer events
	if (msg->peer) {
		if (msg->peer->role == ZTS_PEER_ROLE_PLANET) {
			/* Safe to ignore, these are our roots. They orchestrate the P2P connection.
			You might also see other unknown peers, these are our network controllers. */
			return;
		}
		if (msg->eventCode == ZTS_EVENT_PEER_DIRECT) {
			printf("ZTS_EVENT_PEER_DIRECT --- A direct path is known for node=%llx\n",
				   msg->peer->address);
		}
		if (msg->eventCode == ZTS_EVENT_PEER_RELAY) {
			printf("ZTS_EVENT_PEER_RELAY --- No direct path to node=%llx\n", msg->peer->address);
		}
		if (msg->eventCode == ZTS_EVENT_PEER_PATH_DISCOVERED) {
			printf("ZTS_EVENT_PEER_PATH_DISCOVERED --- A new direct path was discovered for node=%llx\n",
				   msg->peer->address);
		}
		if (msg->eventCode == ZTS_EVENT_PEER_PATH_DEAD) {
			printf("ZTS_EVENT_PEER_PATH_DEAD --- A direct path has died for node=%llx\n",
				   msg->peer->address);
		}
	}
}
#endif

class ZeroTierEventHandler : public zt::IEventHandler {
public:
	ZeroTierEventHandler(NodeInfo& aNodeInfo)
		: _nodeInfo{aNodeInfo}
	{
	}

	void onAddressEvent(zt::EventCode::Address aEventCode, const zt::AddressDetails* aDetails) noexcept override {
		std::cout << zt::EventDescription(aEventCode, aDetails) << std::endl;
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
	if (argc != 6) {
		printf("\nlibzt example client\n");
		printf("client <config_file_path> <nwid> <remoteAddr> <remotePort> <ztServicePort>\n");
		exit(0);
	}
	std::string identityPath = argv[1];
	uint64_t nwid = strtoull(argv[2], NULL, 16); // Network ID to join
	std::string remoteAddrStr = argv[3]; // Remote application's virtual ZT address
	int remotePort = atoi(argv[4]); // Port the application will try to connect to the server on
	int ztServicePort = atoi(argv[5]); // Port ZT uses to send encrypted UDP packets to peers (try something like 9994)
#else
	std::string identityPath = "ZTNodeData";
	uint64_t nwid = strtoull("a09acf0233ceff5c", NULL, 16); // Network ID to join
	std::string remoteAddrStr = "172.29.82.147"; // Remote application's virtual ZT address
	int remotePort = 8888; // Port the application will try to connect to the server on
	int ztServicePort = 9994; // Port ZT uses to send encrypted UDP packets to peers (try something like 9994)
#endif

	zt::SetEventHandler(&eventHandler);

	// Bring up ZeroTier service and join network

	int err = ZTS_ERR_OK;

	//zts_allow_network_caching(1);
	//zts_allow_peer_caching(1);
	//zts_allow_local_conf(1);

	printf("This node's identity is stored in %s\n", identityPath.c_str());

	printf("Starting ZeroTier service...\n");
	if ((err = zt::StartZeroTierService(identityPath, ztServicePort)) != ZTS_ERR_OK) {
		printf("Unable to start service, error = %d. Exiting.\n", err);
		exit(1);
	}

	printf("Waiting for node to come online...\n");
	while (!localNode.online) { 
		std::this_thread::sleep_for(std::chrono::milliseconds{50});
	}
	
	printf("Joining network %llx\n", nwid);
	printf("Don't forget to authorize this device in my.zerotier.com or the web API!\n");
	if ((err = zt::JoinNetwork(nwid)) != ZTS_ERR_OK) {
		printf("Unable to join network, error = %d. Exiting.\n", err);
		exit(1);
	}
	
	
	while (localNode.networksJoinedCount <= 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds{50});
	}

	// Socket-like API example

	char* msgStr = (char*)"Welcome to the machine";
	int bytes = 0;
	char recvBuf[128];
	memset(recvBuf, 0, sizeof(recvBuf));

	zt::Socket socket;
	{
		auto res = socket.init(zt::SocketDomain::InternetProtocol_IPv4, zt::SocketType::Datagram);
		ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
	}
	{
		auto res = socket.bind(zt::IpAddress::ipv4Unspecified(), 0);
		ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
	}

	printf("Remote IP = >>%s<<\n", remoteAddrStr.c_str());
	const auto remoteAddr = zt::IpAddress::ipv4FromString(remoteAddrStr);
	if (!remoteAddr.isValid()) {
		throw std::runtime_error("Invalid remote IP");
	}

	for (int i = 0; i < 10; i += 1) {
		auto res = socket.sendTo(msgStr, strlen(msgStr), remoteAddr, remotePort);
		ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
		printf("sent %d bytes\n", (int)*res);
		std::this_thread::sleep_for(std::chrono::milliseconds{500});
	}
	
	printf("Closing socket\n");
	{
		auto res = socket.close();
		ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
	}

	printf("Shutting down service\n");
	zt::StopZeroTierService();
	std::this_thread::sleep_for(std::chrono::milliseconds{1000});
	zt::SetEventHandler(nullptr);
	return EXIT_SUCCESS;
}
catch (std::exception& ex) {
	printf("Exception caught: %s\n", ex.what());
	printf("Shutting down service\n");
	zt::StopZeroTierService();
	std::this_thread::sleep_for(std::chrono::milliseconds{1000});
	zt::SetEventHandler(nullptr);
}
catch (...) {
	printf("Unknown exception caught\n");
	printf("Shutting down service\n");
	zt::StopZeroTierService();
	std::this_thread::sleep_for(std::chrono::milliseconds{1000});
	zt::SetEventHandler(nullptr);
}