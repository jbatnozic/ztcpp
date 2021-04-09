#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>

#include <ZeroTierSockets.h>
#include <winsock.h>

struct Node
{
    Node() : online(false), joinedAtLeastOneNetwork(false), id(0) {}
    bool online;
    bool joinedAtLeastOneNetwork;
    uint64_t id;
    struct zts_sockaddr_in in4;
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

        myNode.in4 = *in4;
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

/*
typedef uint32_t zts_socklen_t;
typedef uint32_t zts_in_addr_t;
typedef uint16_t zts_in_port_t;
typedef uint8_t zts_sa_family_t;

// Socket protocol types
#define ZTS_SOCK_STREAM     0x0001
#define ZTS_SOCK_DGRAM      0x0002
#define ZTS_SOCK_RAW        0x0003

// Socket family types
#define ZTS_AF_UNSPEC       0x0000
#define ZTS_AF_INET         0x0002
#define ZTS_AF_INET6        0x000a
#define ZTS_PF_INET         ZTS_AF_INET
#define ZTS_PF_INET6        ZTS_AF_INET6
#define ZTS_PF_UNSPEC       ZTS_AF_UNSPEC

// IP address holders
struct zts_in_addr
struct zts_in6_addr

// Complete address data holders
struct zts_sockaddr_in
struct zts_sockaddr_in6

struct zts_sockaddr
*/

int bind_to_port_in_range(int sockfd, 
                          zts_sa_family_t socket_family,
                          zts_in_addr_t inaddr,
                          zts_in_port_t lowport, 
                          zts_in_port_t highport,
                          zts_in_port_t* chosen_port) {
    for (zts_in_port_t port = lowport; port <= highport; port += 1) {
        struct zts_sockaddr_in sockaddr;
        sockaddr.sin_port = port;
#if defined(_WIN32)
        sockaddr.sin_addr.S_addr = inaddr;
#else
        sockaddr.sin_addr.s_addr = inaddr;
#endif
        sockaddr.sin_family = socket_family;

        const int status = zts_bind(sockfd, 
                                    reinterpret_cast<struct zts_sockaddr*>(&sockaddr), 
                                    sizeof(sockaddr));

        if (status == ZTS_ERR_ARG || status == ZTS_ERR_SERVICE) {
            return status;
        }

        if (status == ZTS_ERR_OK || port == highport) {
            *chosen_port = port;
            return status;
        }
    }
    // Actually unreachable
    return ZTS_ERR_OK;
}

#define DATABUFFER_LEN 1024
char databuffer[DATABUFFER_LEN];

int main(int argc, char* argv[]) {
    zts_allow_network_caching(1);
    zts_allow_peer_caching(1);
    zts_allow_local_conf(1);

    int err;

    printf(">>> Starting ZeroTier...\n");
    if ((err = zts_start("ZTNodeData", &on_zts_event, 9994)) != ZTS_ERR_OK) {
        printf("Unable to start service, error = %d. Exiting.\n", err);
        return EXIT_FAILURE;
    }

    printf(">>> Waiting for node to come online...\n");
    while (!myNode.online) {
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
    }

    const std::uint64_t networkID = 0xabfd31bd47bcccd9;
    if ((err = zts_join(networkID)) != ZTS_ERR_OK) {
        printf("Unable to join network, error = %d. Exiting.\n", err);
        return EXIT_FAILURE;
    }

    printf(">>> Waiting to join network %llu...\n", networkID);
    while (!myNode.joinedAtLeastOneNetwork) {
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
    }

    // ========================================================================

#if 0
    int fd1, fd2;

    fd1 = zts_socket(ZTS_AF_INET, ZTS_SOCK_DGRAM, 0);
    assert(fd1 >= 0);

    fd2 = zts_socket(ZTS_AF_INET, ZTS_SOCK_DGRAM, 0);
    assert(fd2 >= 0);

    myNode.in4.sin_port = 2000;

    err = zts_bind(fd1,
                   reinterpret_cast<struct zts_sockaddr*>(&myNode.in4),
                   sizeof(myNode.in4));
    assert(fd1 == ZTS_ERR_OK);

    err = zts_bind(fd2,
                   reinterpret_cast<struct zts_sockaddr*>(&myNode.in4),
                   sizeof(myNode.in4));
    assert(fd1 == ZTS_ERR_OK);

    zts_close(fd1);
    zts_close(fd2);
#endif

#if 1
    int sockfd;

    printf(">>> Creating socket...\n");
    if ((sockfd = zts_socket(ZTS_AF_INET, ZTS_SOCK_DGRAM, ZTS_PF_INET)) < 0) {
        printf("Error creating ZeroTier socket (fd=%d, ret=%d, zts_errno=%d). Exiting.\n", sockfd, err, zts_errno);
        return EXIT_FAILURE;
    } 

    enum class Mode : int { SERVER = 1, CLIENT = 2 };
    printf(":/> Input mode (%d=Server, %d=Client, [other]=Exit): ", (int)Mode::SERVER, (int)Mode::CLIENT);
    Mode mode;
    scanf("%d", &mode);

    zts_in_port_t localPort;
    if ((err = bind_to_port_in_range(sockfd, ZTS_AF_INET, ZTS_INADDR_ANY, 2000 + (int)mode, 2100, &localPort)) < 0) {
        printf("Error binding to interface (fd=%d, ret=%d, zts_errno=%d). Exiting.\n", sockfd, err, zts_errno);
        goto END;
    }
    else {
        printf("Socket bound to port %d\n", (int)localPort);
    }

    if (mode == Mode::SERVER) {
        for (int i = 0; i < 10; i += 1) {
            zts_sockaddr from;
            zts_socklen_t fromlen;
            auto byteCount = zts_recvfrom(sockfd, databuffer, DATABUFFER_LEN, 0, &from, &fromlen);
            if (byteCount < 0) {
                printf("recvfrom error (fd=%d, ret=%d, zts_errno=%d)\n", sockfd, byteCount, zts_errno);
                goto END;
            }
            else {
                printf("received %d bytes\n", byteCount);
            }
        }
    }
    else if (mode == Mode::CLIENT) {
        struct zts_sockaddr_in in4;
        in4.sin_port = htons(2001);
#if defined(_WIN32)
        zts_inet_pton(ZTS_AF_INET, "127.0.0.1", &(in4.sin_addr.S_addr));
#else
        zts_inet_pton(ZTS_AF_INET, "127.0.0.1", &(in4.sin_addr.s_addr));
#endif
        in4.sin_family = ZTS_AF_INET;

        for (int i = 0; i < 100; i += 1) {
            auto byteCount = zts_sendto(sockfd, databuffer, 100, 0, (struct zts_sockaddr*)&in4, sizeof(in4));
            if (byteCount < 0) {
                printf("sendto error (fd=%d, ret=%d, zts_errno=%d)\n", sockfd, byteCount, zts_errno);
                goto END;
            }
            else {
                printf("sent %d bytes\n", byteCount);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds{500});
        }
    }
    else {
        // Do nothing
    }

END:
    printf(">>> Closing the socket...");
    if ((err = zts_close(sockfd)) != ZTS_ERR_OK) {
        // ...
    }
#endif
}