//
// Created by 7906200 on 11/14/2025.
//

#include "Server.h"
#include "Player.h"
#include <iostream>
#include <raymath.h>
#include <thread>
#include <unordered_map>
#include "Packet.h"
#include "Utils.h"
#include "enet/enet.h"

using namespace std;

ENetHost* server = {0};
bool ServerRunning = true;
unordered_map<long, ENetPeer*> peers;

double LastSyncedTime = GetTimeUtils();

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

    long latest_player_id = 1;

    while (ServerRunning) {
        int active = enet_host_service(server, &server_event, 250);
        if (active > 0) {
            switch (server_event.type) {
                case ENET_EVENT_TYPE_CONNECT: {
                    printf("A new client connected from %x:%u.\n",  server_event.peer->address.host, server_event.peer->address.port);
                    Player* newPlayer = new Player();
                    newPlayer->PlayerID = latest_player_id;
                    server_event.peer->data = newPlayer;
                    peers[latest_player_id] = server_event.peer;
                    for (auto [name,peer] : peers) {
                        auto* p = static_cast<Player *>(peer->data);
                        if (p->PlayerID != newPlayer->PlayerID) {
                            Packet myPacket = {};
                            myPacket.type = PLAYER_JOIN;
                            myPacket.playerJoin.id = newPlayer->PlayerID;
                            myPacket.playerJoin.starting_location = {0, 0};
                            ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), ENET_PACKET_FLAG_RELIABLE);
                            enet_peer_send(peer, 0, packet);

                            myPacket = {};
                            myPacket.type = TIME_SYNC;
                            myPacket.timestamp = GetTimeUtils();
                            packet = enet_packet_create(&myPacket, sizeof(myPacket), 0);
                            enet_peer_send(server_event.peer, 0, packet);

                            myPacket = {};
                            myPacket.type = PLAYER_JOIN;
                            myPacket.playerJoin.id = p->PlayerID;
                            myPacket.playerJoin.starting_location = p->CurrentState.position;
                            packet = enet_packet_create(&myPacket, sizeof(myPacket), ENET_PACKET_FLAG_RELIABLE);
                            enet_peer_send(server_event.peer, 0, packet);
                        }
                    }
                    latest_player_id += 1;
                    std::cout << "added " << latest_player_id << "\n";
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE: {
                    Packet myPacket;
                    memcpy(&myPacket, server_event.packet->data, server_event.packet->dataLength);

                    switch (myPacket.type) {
                        case PLAYER_UPDATE: {
                            auto* player = reinterpret_cast<Player *>(server_event.peer->data);
                            myPacket.playerState.timestamp = GetTimeUtils();
                            player->LastState = player->CurrentState;
                            player->CurrentState = myPacket.playerState;
                            player->CurrentState.id = player->PlayerID;
                            player->LastState.id = player->PlayerID;
                            break;
                        }
                    }

                    enet_packet_destroy (server_event.packet);

                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    printf("disconnected.\n");
                    Player* oldPlayer = reinterpret_cast<Player *>(server_event.peer->data);
                    for (auto [name, peer] : peers) {
                        auto* p = static_cast<Player *>(peer->data);
                        if (p->PlayerID != oldPlayer->PlayerID) {
                            Packet myPacket;
                            myPacket.type = PLAYER_LEFT;
                            myPacket.playerLeft.id = oldPlayer->PlayerID;
                            ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), ENET_PACKET_FLAG_RELIABLE);
                            enet_peer_send(peer, 0, packet);
                        }
                    }
                    peers.erase(oldPlayer->PlayerID);
                    delete oldPlayer;
                    cout << "new size " << peers.size() << "\n";
                    break;
                }
                case ENET_EVENT_TYPE_NONE:
                    break;
            }
        }
        if (GetTimeUtils() - LastSyncedTime >= 5) {
            for (auto [id, peer] : peers) {
                Packet myPacket;
                myPacket.type = TIME_SYNC;
                myPacket.timestamp = GetTimeUtils();
                ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), 0);
                enet_peer_send(peer, 0, packet);
            }
            LastSyncedTime = GetTimeUtils();
        }
        if (peers.size() > 1) {

            for (auto [id, peer] : peers) {
                for (auto [other_id, other_peer] : peers) {
                    if (other_id != id) {
                        auto* player = reinterpret_cast<Player *>(other_peer->data);
                        Packet myPacket;
                        myPacket.type = PLAYER_UPDATE;
                        myPacket.playerState= player->CurrentState;
                        myPacket.playerState.timestamp = GetTimeUtils();
                        ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), 0);
                        enet_peer_send(peer, 0, packet);
                    }
                }
            }

        }

        enet_host_flush(server);
    }
}

void StopServer() {
    if (server != nullptr) {
        enet_host_destroy(server);
    }
    enet_deinitialize();
}
