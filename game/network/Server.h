//
// Created by 7906200 on 11/14/2025.
//

#ifndef SERVER_H
#define SERVER_H
#include <string>

#include "enet/enet.h"

bool StartServer(std::string IPAddress = "127.0.0.1", int Port = 5000, int MaxClients = 32);
void StopServer();

#endif //SERVER_H
