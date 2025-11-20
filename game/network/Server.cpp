//
// Created by 7906200 on 11/14/2025.
//

#include "Server.h"
#include <iostream>
#include <unordered_map>

ENetAddress address;
ENetEvent event;
ENetHost* server;

bool StartServer(std::string IPAddress, int Port, int MaxClients) {
    enet_initialize();
    atexit(enet_deinitialize);

    enet_address_set_host_ip(&address, IPAddress.c_str());
    address.port = Port;

    server = enet_host_create(&address, MaxClients, 2, 0, 0);

    if (server == NULL) {
        std::cout << "Error creating server\n";
        return false;
    }

    std::cout << "Starting server...\n";

    std::unordered_map<enet_uint32, ENetPeer*> peers;

    const char* s = "Client information";
    while (true) {
        int active = enet_host_service(server, &event, 1000);
        if (active > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    printf("A new client connected from %x:%u.\n",  event.peer->address.host, event.peer->address.port);

                    peers[event.peer->connectID] = event.peer;
                    event.peer->data = &s;
                    std::cout << "added " << event.peer->connectID << "\n";
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    printf("A packet of length %lu containing %s was received from %s on channel %u.\n",
                            event.packet->dataLength,
                            event.packet->data,
                            event.peer->data,
                            event.channelID);
                    enet_packet_destroy (event.packet);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("%s disconnected.\n", event.peer->data);
                    /* Reset the peer's client information. */
                    event.peer->data = NULL;
                    peers.erase(event.peer->connectID);
                    break;

                case ENET_EVENT_TYPE_NONE:
                    break;
            }
        }

    }
    return true;
}

void StopServer() {
    enet_host_destroy(server);
    enet_deinitialize();
}
