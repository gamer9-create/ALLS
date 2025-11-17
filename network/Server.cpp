//
// Created by 7906200 on 11/14/2025.
//

#include "Server.h"

#include <cstring>
#include <iostream>
#include <raymath.h>
#include <thread>
#include <unordered_map>
#include <vector>

#include "enet/enet.h"

ENetHost* server = {0};
bool ServerRunning = true;
std::unordered_map<enet_uint32, ENetPeer*> peers;
std::unordered_map<enet_uint32, Vector2> positions;

void SendPositions() {
    while (ServerRunning) {
        if (server != NULL) {
            for (auto [id, peer] : peers) {
                std::vector<Vector2> otherPlayerPositions;
                for (auto [other_id, other_peer] : peers) {
                    if (other_id != id && positions.contains(other_id)) {
                        otherPlayerPositions.push_back(positions[other_id]);
                    }
                }
                if (otherPlayerPositions.size() > 0) {
                    int siz = otherPlayerPositions.size() * 8;
                    std::cout << siz << std::endl;
                    byte position[siz];
                    std::cout << "did it?" << std::endl;
                    for (int i = 0; i < otherPlayerPositions.size(); i++) {
                        int idx = i * 8;
                        std::memcpy(position, &otherPlayerPositions[i]+idx, siz);
                        std::memcpy(position, &otherPlayerPositions[i]+idx+4, siz);
                    }
                    std::cout <<"DEAD" << std::endl;
                    std::cout << position << std::endl;
                    std::cout <<"DEAD?" << std::endl;
                    ENetPacket* packet = enet_packet_create(&position, sizeof(position), ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
                    std::cout <<"HOW" << std::endl;
                    enet_peer_send(peer, 0, packet);
                    std::cout <<"WTF" << std::endl;
                }
            }
        }
    }
}

void StartServer(std::string IPAddress, int Port, int MaxClients) {
    ServerRunning = true;
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

    std::thread t(SendPositions);
    t.detach();

    const char* s = "Client information";
    while (ServerRunning) {
        int active = enet_host_service(server, &server_event, 1000);
        if (active > 0) {
            switch (server_event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    printf("A new client connected from %x:%u.\n",  server_event.peer->address.host, server_event.peer->address.port);
                    peers[server_event.peer->connectID] = server_event.peer;
                    positions[server_event.peer->connectID]= {0,0};
                    //server_event.peer->data = &s;
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
