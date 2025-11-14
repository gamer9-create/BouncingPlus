//
// Created by 7906200 on 11/14/2025.
//

#include "Server.h"

#include <iostream>

Server::Server(std::string IPAddress, int Port, int MaxClients) {
    enet_initialize();

    enet_address_set_host_ip(&address, IPAddress.c_str());
    address.port = Port;

    server = enet_host_create(&address, MaxClients, 2, 0, 0);

    if (server == NULL) {
        std::cout << "Error creating server\n";
    }
}

void Server::Start() {
    /* Wait up to 1000 milliseconds for an event. (WARNING: blocking) */
    const char* s = "Client information";
    while (enet_host_service(server, &event, 1000) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                printf("A new client connected from %x:%u.\n",  event.peer->address.host, event.peer->address.port);
                /* Store any relevant client information here. */

                event.peer->data = &s;
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                printf("A packet of length %lu containing %s was received from %s on channel %u.\n",
                        event.packet->dataLength,
                        event.packet->data,
                        event.peer->data,
                        event.channelID);
                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy (event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("%s disconnected.\n", event.peer->data);
                /* Reset the peer's client information. */
                event.peer->data = NULL;
                break;

            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }
}

void Server::Stop() {
    enet_host_destroy(server);
    enet_deinitialize();

    delete server;
}
