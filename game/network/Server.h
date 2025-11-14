//
// Created by 7906200 on 11/14/2025.
//

#ifndef SERVER_H
#define SERVER_H

#include <string>

#define enet
#include "enet/enet.h"
#undef enet

class Server {
    ENetAddress address;
    ENetHost *server;
    ENetEvent event;

public:
    Server(std::string IPAddress = "127.0.0.1", int Port = 8080, int MaxClients = 32);
    void Start();
    void Stop();
};



#endif //SERVER_H
