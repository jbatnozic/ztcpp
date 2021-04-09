/**
 * libzt API example
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdexcept>
#include <thread>

#include <winsock.h>
#include "ZeroTierSockets.h"

#include "udpsocket.hpp"

namespace ztcpp = jbatnozic::ztcpp;

struct Node
{
	Node() : online(false), joinedAtLeastOneNetwork(false), id(0) {}
	bool online;
	bool joinedAtLeastOneNetwork;
	uint64_t id;
	// etc
} myNode;

/* Callback handler, you should return control from this function as quickly as you can
to ensure timely receipt of future events. You should not call libzt API functions from
this function unless it's something trivial like zts_inet_ntop() or similar that has
no state-change implications. */
void on_zts_event(void* msgPtr)
{
	struct zts_callback_msg* msg = (struct zts_callback_msg*)msgPtr;

	// Node events
	if (msg->eventCode == ZTS_EVENT_NODE_ONLINE) {
		printf("ZTS_EVENT_NODE_ONLINE --- This node's ID is %llx\n", msg->node->address);
		myNode.id = msg->node->address;
		myNode.online = true;
	}
	if (msg->eventCode == ZTS_EVENT_NODE_OFFLINE) {
		printf("ZTS_EVENT_NODE_OFFLINE --- Check your physical Internet connection, router, firewall, etc. What ports are you blocking?\n");
		myNode.online = false;
	}
	if (msg->eventCode == ZTS_EVENT_NODE_NORMAL_TERMINATION) {
		printf("ZTS_EVENT_NODE_NORMAL_TERMINATION\n");
	}

	// Virtual network events
	if (msg->eventCode == ZTS_EVENT_NETWORK_NOT_FOUND) {
		printf("ZTS_EVENT_NETWORK_NOT_FOUND --- Are you sure %llx is a valid network?\n",
			   msg->network->nwid);
	}
	if (msg->eventCode == ZTS_EVENT_NETWORK_REQ_CONFIG) {
		printf("ZTS_EVENT_NETWORK_REQ_CONFIG --- Requesting config for network %llx, please wait a few seconds...\n", msg->network->nwid);
	}
	if (msg->eventCode == ZTS_EVENT_NETWORK_ACCESS_DENIED) {
		printf("ZTS_EVENT_NETWORK_ACCESS_DENIED --- Access to virtual network %llx has been denied. Did you authorize the node yet?\n",
			   msg->network->nwid);
	}
	if (msg->eventCode == ZTS_EVENT_NETWORK_READY_IP4) {
		printf("ZTS_EVENT_NETWORK_READY_IP4 --- Network config received. IPv4 traffic can now be sent over network %llx\n",
			   msg->network->nwid);
		myNode.joinedAtLeastOneNetwork = true;
	}
	if (msg->eventCode == ZTS_EVENT_NETWORK_READY_IP6) {
		printf("ZTS_EVENT_NETWORK_READY_IP6 --- Network config received. IPv6 traffic can now be sent over network %llx\n",
			   msg->network->nwid);
		myNode.joinedAtLeastOneNetwork = true;
	}
	if (msg->eventCode == ZTS_EVENT_NETWORK_DOWN) {
		printf("ZTS_EVENT_NETWORK_DOWN --- %llx\n", msg->network->nwid);
	}

	// Address events
	if (msg->eventCode == ZTS_EVENT_ADDR_ADDED_IP4) {
		char ipstr[ZTS_INET_ADDRSTRLEN];
		struct zts_sockaddr_in* in4 = (struct zts_sockaddr_in*) & (msg->addr->addr);
		zts_inet_ntop(ZTS_AF_INET, &(in4->sin_addr), ipstr, ZTS_INET_ADDRSTRLEN);
		printf("ZTS_EVENT_ADDR_NEW_IP4 --- This node's virtual address on network %llx is %s\n",
			   msg->addr->nwid, ipstr);
	}
	if (msg->eventCode == ZTS_EVENT_ADDR_ADDED_IP6) {
		char ipstr[ZTS_INET6_ADDRSTRLEN];
		struct zts_sockaddr_in6* in6 = (struct zts_sockaddr_in6*) & (msg->addr->addr);
		zts_inet_ntop(ZTS_AF_INET6, &(in6->sin6_addr), ipstr, ZTS_INET6_ADDRSTRLEN);
		printf("ZTS_EVENT_ADDR_NEW_IP6 --- This node's virtual address on network %llx is %s\n",
			   msg->addr->nwid, ipstr);
	}
	if (msg->eventCode == ZTS_EVENT_ADDR_REMOVED_IP4) {
		char ipstr[ZTS_INET_ADDRSTRLEN];
		struct zts_sockaddr_in* in4 = (struct zts_sockaddr_in*) & (msg->addr->addr);
		zts_inet_ntop(ZTS_AF_INET, &(in4->sin_addr), ipstr, ZTS_INET_ADDRSTRLEN);
		printf("ZTS_EVENT_ADDR_REMOVED_IP4 --- The virtual address %s for this node on network %llx has been removed.\n",
			   ipstr, msg->addr->nwid);
	}
	if (msg->eventCode == ZTS_EVENT_ADDR_REMOVED_IP6) {
		char ipstr[ZTS_INET6_ADDRSTRLEN];
		struct zts_sockaddr_in6* in6 = (struct zts_sockaddr_in6*) & (msg->addr->addr);
		zts_inet_ntop(ZTS_AF_INET6, &(in6->sin6_addr), ipstr, ZTS_INET6_ADDRSTRLEN);
		printf("ZTS_EVENT_ADDR_REMOVED_IP6 --- The virtual address %s for this node on network %llx has been removed.\n",
			   ipstr, msg->addr->nwid);
	}
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

int main(int argc, char** argv)
try {
#if 1
	if (argc != 6) {
		printf("\nlibzt example client\n");
		printf("client <config_file_path> <nwid> <remoteAddr> <remotePort> <ztServicePort>\n");
		exit(0);
	}
	uint64_t nwid = strtoull(argv[2], NULL, 16); // Network ID to join
	std::string remoteAddrStr = argv[3]; // Remote application's virtual ZT address
	int remotePort = atoi(argv[4]); // Port the application will try to connect to the server on
	int ztServicePort = atoi(argv[5]); // Port ZT uses to send encrypted UDP packets to peers (try something like 9994)
#else
	uint64_t nwid = strtoull("a09acf0233ceff5c", NULL, 16); // Network ID to join
	std::string remoteAddrStr = "172.29.11.27"; // Remote application's virtual ZT address
	int remotePort = 8888; // Port the application will try to connect to the server on
	int ztServicePort = 9994; // Port ZT uses to send encrypted UDP packets to peers (try something like 9994)
#endif

	// Bring up ZeroTier service and join network

	int err = ZTS_ERR_OK;

	zts_allow_network_caching(1);
	zts_allow_peer_caching(1);
	zts_allow_local_conf(1);

	if ((err = zts_start("ZTNodeData", &on_zts_event, ztServicePort)) != ZTS_ERR_OK) {
		printf("Unable to start service, error = %d. Exiting.\n", err);
		exit(1);
	}
	printf("Waiting for node to come online...\n");
	while (!myNode.online) { zts_delay_ms(50); }
	printf("This node's identity is stored in %s\n", argv[1]);

	if ((err = zts_join(nwid)) != ZTS_ERR_OK) {
		printf("Unable to join network, error = %d. Exiting.\n", err);
		exit(1);
	}
	printf("Joining network %llx\n", nwid);
	printf("Don't forget to authorize this device in my.zerotier.com or the web API!\n");
	while (!myNode.joinedAtLeastOneNetwork) { zts_delay_ms(50); }

	// Socket-like API example

	char* msgStr = (char*)"Welcome to the machine";
	int bytes = 0, fd;
	char recvBuf[128];
	memset(recvBuf, 0, sizeof(recvBuf));

	ztcpp::UdpSocket socket;
	{
		auto res = socket.init(ztcpp::SocketDomain::InternetProtocol_IPv4);
		ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
	}
	{
		auto res = socket.bind(ztcpp::IpAddress::ipv4Unspecified(), 0);
		ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
	}

	printf("Remote IP = >>%s<<\n", remoteAddrStr.c_str());
	const auto remoteAddr = ztcpp::IpAddress::ipv4FromString(remoteAddrStr);
	if (!remoteAddr.isValid()) {
		throw std::runtime_error("Invalid remote IP");
	}

	for (int i = 0; i < 100; i += 1) {
		auto res = socket.send(msgStr, strlen(msgStr), remoteAddr, remotePort);
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
	zts_stop();
	return EXIT_SUCCESS;
}
catch (std::exception& ex) {
	printf("Exception caught: %s\n", ex.what());
	printf("Shutting down service\n");
	zts_stop();
}
catch (...) {
	printf("Unknown exception caught\n");
	printf("Shutting down service\n");
	zts_stop();
}