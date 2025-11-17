//
// Created by 7906200 on 11/14/2025.
//

#include "Server.h"
#include <iostream>
#include <raymath.h>
#include <unordered_map>
#include "enet/enet.h"

ENetHost* server = {0};
bool Running = true;
std::unordered_map<enet_uint32, ENetPeer*> peers;
std::unordered_map<enet_uint32, Vector2> positions;

void SendPositions() {
    while (Running) {
        if (server != NULL) {
            for (auto [id, peer] : peers) {

                for (auto [other_id, other_peer] : peers) {
                    if (other_id != id) {

                    }
                }
            }
        }
    }
}

void StartServer(std::string IPAddress, int Port, int MaxClients) {
    Running = true;
    if (enet_initialize() != 0) {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
    }
    //atexit(enet_deinitialize);

    ENetAddress server_address = {0};
    ENetEvent server_event;

    enet_address_set_host_ip(&server_address, IPAddress.c_str());
    server_address.port = Port;

    server = enet_host_create(&server_address, MaxClients, 2, 0, 0);

    if (server == NULL) {
        std::cout << "Error creating server\n";
    }

    std::cout << "Starting server...\n";

    const char* s = "Client information";
    while (Running) {
        int active = enet_host_service(server, &server_event, 1000);
        if (active > 0) {
            switch (server_event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    printf("A new client connected from %x:%u.\n",  server_event.peer->address.host, server_event.peer->address.port);

                    peers[server_event.peer->connectID] = server_event.peer;
                    positions[server_event.peer->connectID]= {0,0};
                    server_event.peer->data = &s;
                    std::cout << "added " << server_event.peer->connectID << "\n";
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    float x, y;
                    std::memcpy(&x, server_event.packet->data, 4);
                    std::memcpy(&y, server_event.packet->data + 4, 4);
                    positions[server_event.peer->connectID]= {x,y};
                    enet_packet_destroy (server_event.packet);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("disconnected.\n");
                    /* Reset the peer's client information. */
                    server_event.peer->data = NULL;
                    peers.erase(server_event.peer->connectID);
                    positions.erase(server_event.peer->connectID);
                    break;

                case ENET_EVENT_TYPE_NONE:
                    break;
            }
        }
    }
}

void StopServer() {
    if (server != nullptr) {
        enet_host_destroy(server);
    }
    enet_deinitialize();
}
