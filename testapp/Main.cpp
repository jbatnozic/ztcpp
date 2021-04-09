
#include <functional>
#include <iostream>
#include <string>
#include <thread>

#include <Ztcpp.hpp>

std::uint64_t ZTNode_id = 0;
bool ZTNode_online = false;

namespace ztcpp = jbatnozic::ztcpp;

ztcpp::EventHandlers MakeEventHandlers() {
  using namespace ztcpp;
  EventHandlers handlers;

  handlers.nodeDetailsHandler = 
    [](EventCode::Node aEventCode, const NodeDetails* aNodeDetails) {
      if (aEventCode == EventCode::Node::Up) {
        printf("ZTS_EVENT_NODE_UP --- ???\n");
      }
      else if (aEventCode == EventCode::Node::Online) {
        printf("ZTS_EVENT_NODE_ONLINE --- This node's ID is %llx\n", aNodeDetails->getNodeID());
        ZTNode_id = aNodeDetails->getNodeID();
        ZTNode_online = true;
      }
      else if (aEventCode == EventCode::Node::Down) {
        printf("ZTS_EVENT_NODE_DOWN --- ???\n");
      }
      else if (aEventCode == EventCode::Node::Offline) {
        printf("ZTS_EVENT_NODE_OFFLINE --- Check your physical Internet connection, router, firewall, etc. What ports are you blocking?\n");
        ZTNode_online = false;
      }
      else {
        printf("ZTS_EVENT_NODE_* --- Unhandled Node event (%d)\n", static_cast<int>(aEventCode));
      }
    };

  handlers.networkDetailsHandler = 
    [](EventCode::Network aEventCode, const NetworkDetails* aNetworkDetails) {
      if (aEventCode == EventCode::Network::NotFound) {
        printf("ZTS_EVENT_NETWORK_NOT_FOUND --- Are you sure %llx is a valid network?\n",
               aNetworkDetails->getNetworkID());
      }
      else if (aEventCode == EventCode::Network::RequestingConfiguration) {
        printf("ZTS_EVENT_NETWORK_REQ_CONFIG --- Requesting config for network %llx, please wait a few seconds...\n", 
               aNetworkDetails->getNetworkID());
      }
      else if (aEventCode == EventCode::Network::AccessDenied) {
        printf("ZTS_EVENT_NETWORK_ACCESS_DENIED --- Access to virtual network %llx has been denied. Did you authorize the node yet?\n",
               aNetworkDetails->getNetworkID());
      }
      else if (aEventCode == EventCode::Network::ReadyIP6) {
        printf("ZTS_EVENT_NETWORK_READY_IP6 --- Network config received. IPv6 traffic can now be sent over network %llx\n",
               aNetworkDetails->getNetworkID());
        // myNode.joinedAtLeastOneNetwork = true;
      }
      else if (aEventCode == EventCode::Network::Down) {
        printf("ZTS_EVENT_NETWORK_DOWN --- %llx\n", aNetworkDetails->getNetworkID());
      }
      else {
        printf("ZTS_EVENT_NETWORK_* --- Unhandled Network event (%d)\n", static_cast<int>(aEventCode));
      }
    };

  handlers.networkStackDetailsHandler = 
    [](EventCode::NetworkStack aEventCode, const NetworkStackDetails* aNetworkStackDetails) {
      printf("ZTS_EVENT_STACK_* --- Unhandled NetworkStack event (%d)\n", static_cast<int>(aEventCode));
    };

  handlers.netIFDetailsHandler =
    [](EventCode::NetIF aEventCode, const NetIFDetails* aNodeDetails) {
      if (aEventCode == EventCode::NetIF::Up) {
        //printf("ZTS_EVENT_NETIF_UP --- network=%llx, mac=%llx, mtu=%d\n",
        //       msg->netif->nwid,
        //       msg->netif->mac,
        //       msg->netif->mtu);
        printf("ZTS_EVENT_NETIF_UP --- network=<unknown>, mac=<unknown>, mtu=<unknown>\n");
      }
      else if (aEventCode == EventCode::NetIF::Down) {
        //printf("ZTS_EVENT_NETIF_DOWN --- network=%llx, mac=%llx\n",
        //       msg->netif->nwid,
        //       msg->netif->mac);
        printf("ZTS_EVENT_NETIF_DOWN --- network=<unknown>, mac=<unknown>\n");
      }
    };

  handlers.virtualNetworkRouteHandler = 
    [](EventCode::Route aEventCode, const VirtualNetworkRoute* aVirtualNetworkRoute) {
      if (aEventCode == EventCode::Route::Added) {
        printf("ZTS_EVENT_ROUTE_ADDED --- ???\n");
      }
      else if (aEventCode == EventCode::Route::Removed) {
        printf("ZTS_EVENT_ROUTE_REMOVED --- ???\n");
      }
    };

  handlers.peerDetailsHandler = 
    [](EventCode::Peer aEventCode, const PeerDetails* aPeerDetails) {
      if (aPeerDetails) {
        if (aPeerDetails->getRole() == PeerRole::Planet) {
          /* Safe to ignore, these are our roots. They orchestrate the P2P connection.
          You might also see other unknown peers, these are our network controllers. */
          return;
        }
        if (aEventCode == EventCode::Peer::Direct) {
          printf("ZTS_EVENT_PEER_DIRECT --- A direct path is known for node=%llx\n",
                 aPeerDetails->getAddress());
        }
        if (aEventCode == EventCode::Peer::Relay) {
          printf("ZTS_EVENT_PEER_RELAY --- No direct path to node=%llx\n", 
                 aPeerDetails->getAddress());
        }
        if (aEventCode == EventCode::Peer::PathDiscovered) {
          printf("ZTS_EVENT_PEER_PATH_DISCOVERED --- A new direct path was discovered for node=%llx\n",
                 aPeerDetails->getAddress());
        }
        if (aEventCode == EventCode::Peer::PathDead) {
          printf("ZTS_EVENT_PEER_PATH_DEAD --- A direct path has died for node=%llx\n",
                 aPeerDetails->getAddress());
        }
      }
    };

  handlers.addressDetailsHandler = 
    [](EventCode::Address aEventCode, const AddressDetails* aNodeDetails) {
      if (aEventCode == EventCode::Address::AddedIP6) {
        //char ipstr[ZTS_INET6_ADDRSTRLEN];
        //struct zts_sockaddr_in6* in6 = (struct zts_sockaddr_in6*) & (msg->addr->addr);
        //zts_inet_ntop(ZTS_AF_INET6, &(in6->sin6_addr), ipstr, ZTS_INET6_ADDRSTRLEN);
        printf("ZTS_EVENT_ADDR_NEW_IP6 --- Join <unknown> and ping me at <unknown>\n");
               //msg->addr->nwid, ipstr);
      }
      else {
        printf("ZTS_EVENT_ADDRESS_* --- Unhandled Address event (%d)\n", static_cast<int>(aEventCode));
      }
    };

  return handlers;
}

void StopZeroTier() {
  int status;
  if ((status = ztcpp::StopZT()) != ZTS_ERR_OK) {
    printf("Tried stopping ZeroTier but an error occurred (%d).\n", status);
  }
  if ((status = ztcpp::FreeZT()) != ZTS_ERR_OK) {
    printf("Tried freeing ZeroTier but an error occurred (%d).\n", status);
  }
}

int main(int argc, char* argv[])
try {
  auto eventHandlers = MakeEventHandlers();

  int status;

  puts(">>> Starting ZeroTier...");
  status = ztcpp::StartZT("zt-test", std::move(eventHandlers), 9994); // ZTS_ERR_SERVICE, ZTS_ERR_ARG, ZTS_ERR_OK
  if (status != ZTS_ERR_OK) {
    puts("Tried to start ZeroTier but an error occurred.");
    goto END;
  }

  puts(">>> Waiting for node to come online...");
  while (ZTNode_online == false) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  const std::uint64_t networkID =
    (argc > 1) ? std::stoull(std::string(argv[1])) : 0xabfd31bd47bcccd9; // Default = Test Group

  puts(">>> Trying to join network 0xabfd31bd47bcccd9...");
  //status = ztcpp::JoinNetwork(0xe4da7455b26d776e); // Galactica
  status = ztcpp::JoinNetwork(networkID);
  if (status != ZTS_ERR_OK) {
    puts("Tried to join a network but an error occurred.");
    goto END;
  }

  //puts(">>> Waiting 2 minutes...");
  //for (int i = 0; i < 120; i += 1) {
  //  std::this_thread::sleep_for(std::chrono::seconds(1));
  //}

  puts(">>> Waiting for user input...");
  int dummy;
  std::cin >> dummy;

END:
  puts(">>> Exiting...");
  StopZeroTier();
  return 0;
}
catch (...) {
  StopZeroTier();
}