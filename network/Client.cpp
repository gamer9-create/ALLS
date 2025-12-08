//
// Created by lalit on 11/16/2025.
//

#include "Client.h"
#include "Utils.h"
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Packet.h"
#include "Player.h"
#include "enet/enet.h"

using namespace std;

std::atomic<bool> Running;

ENetHost* client = {0};
ENetPeer* peer = {0};
std::unordered_map<long, Player> client_players;

double ServerTime = 0;
double RealServerTime = 0;

double LastTime = 0;

void ClientThread(std::string IPAddress, int Port) {
    printf("yo everybody hush we startin the client\n");
    if (enet_initialize() != 0) {
        printf("An error occurred while initializing ENet.\n");
    } else {
        printf("enet init!\n");
    }
    atexit(enet_deinitialize);

    ENetEvent client_event;

    ENetAddress client_address = {0};

    client = enet_host_create(NULL, 1, 2, 0, 0);
    if (client == NULL) {
        printf("An error occurred while trying to create an ENet client host.\n");
    } else {
        printf("i like eating people\n");
    }

    printf("sussy baka!\n");
    enet_address_set_host(&client_address, IPAddress.c_str());
    printf("getting DEEP\n");
    client_address.port = Port;
    peer = enet_host_connect(client, &client_address, 2, 0);
    if (peer == NULL) {
        printf("No available peers for initiating an ENet connection.\n");
    }
    printf("SUSSED OUT\n");

    if (enet_host_service(client, &client_event, 5000) > 0 &&
      client_event.type == ENET_EVENT_TYPE_CONNECT) {
        puts("Connection to some.server.net:1234 succeeded.");
      } else {
          enet_peer_reset(peer);
          puts("Connection to some.server.net:1234 failed.");
      }
    printf("MAX RIZZ\n");

    while (Running.load(std::memory_order_relaxed)) {
        int active = enet_host_service(client, &client_event, 250);
        if (active > 0) {
            switch (client_event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    break;
                case ENET_EVENT_TYPE_RECEIVE: {

                    Packet packet;
                    memcpy(&packet, client_event.packet->data, client_event.packet->dataLength);
                    switch (packet.type) {
                        case PLAYER_JOIN: {
                            PlayerState s = {
                                packet.playerJoin.id,
                                packet.playerJoin.starting_location.x,
                                packet.playerJoin.starting_location.y,
                                0,0,
                                0,0,packet.playerJoin.timestamp
                            };
                            client_players[packet.playerJoin.id] = {
                                packet.playerJoin.id,
                                s,s,
                                std::vector<double>(), GetTimeUtils(), 0.1f, s, std::vector<PlayerState>(),

                            };
                            break;
                        }
                        case PLAYER_LEFT: {
                            if (client_players.contains(packet.playerLeft.id))
                                client_players.erase(packet.playerLeft.id);
                            break;
                        }
                        case PLAYER_UPDATE: {
                            PlayerState newPlayerState = packet.playerState;
                            PlayerState oldPlayerState;
                            if (client_players.contains(newPlayerState.id)) {
                                oldPlayerState = client_players.at(newPlayerState.id).CurrentState;
                                client_players[newPlayerState.id].PreviousPlayerStates.push_back(oldPlayerState);
                                if (client_players[newPlayerState.id].PreviousPlayerStates.size() > 5) {
                                    client_players[newPlayerState.id].PreviousPlayerStates.erase(
                                        client_players[newPlayerState.id].PreviousPlayerStates.begin());
                                }

                                client_players[newPlayerState.id].CurrentState = newPlayerState;
                                client_players[newPlayerState.id].LastState = oldPlayerState;

                                client_players[newPlayerState.id].UpdateTimes.push_back(
                                    GetTimeUtils() - client_players[newPlayerState.id].LastUpdateTime);
                                if (client_players[newPlayerState.id].UpdateTimes.size() > 5)
                                    client_players[newPlayerState.id].UpdateTimes.erase(
                                        client_players[newPlayerState.id].UpdateTimes.begin());
                                float AddUp = 0;
                                for (int i = 0; i < client_players[newPlayerState.id].UpdateTimes.size(); i++) {
                                    AddUp += client_players[newPlayerState.id].UpdateTimes[i];
                                }
                                if (client_players[newPlayerState.id].UpdateTimes.size() != 0 && client_players[
                                        newPlayerState.id].AverageUpdateTime != 0)
                                    client_players[newPlayerState.id].AverageUpdateTime =
                                            AddUp / client_players[newPlayerState.id].UpdateTimes.size();
                                client_players[newPlayerState.id].LastUpdateTime = GetTimeUtils();
                            }
                            break;
                        }
                    }

                    enet_packet_destroy(client_event.packet);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT:
                    break;
                case ENET_EVENT_TYPE_NONE:
                    break;
            }
        }

    }
}

void StartClient(std::string IPAddress, int Port) {
    Running.store(true, std::memory_order_relaxed);
    printf("Starting client...\n");
    std::thread t(ClientThread, IPAddress, Port);
    t.detach();
}

std::unordered_map<long, Player> *GetPlayers() {
    return &client_players;
}

void InterpolateServerTime(float dt) {
    ServerTime = lerp(ServerTime, RealServerTime, 0.1f * dt);
}

void UpdateState(PlayerState state) {
    if (client != nullptr && peer != nullptr) {
        Packet myPacket;
        myPacket.type = PLAYER_UPDATE;
        myPacket.playerState = state;
        ENetPacket* packet = enet_packet_create(&myPacket, sizeof(myPacket), 0);
        enet_peer_send(peer, 0, packet);
    }
}

void StopClient() {
    Running.store(false, std::memory_order_relaxed);
    Sleep(100);
    uint8_t disconnected = false;
    if (client != nullptr) {
        ENetEvent client_event;
        enet_peer_disconnect(peer, 0);
        while (enet_host_service(client, &client_event, 1000) > 0) {
            switch (client_event.type) {
                case ENET_EVENT_TYPE_RECEIVE:
                    enet_packet_destroy(client_event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    puts("Disconnection succeeded.");
                    disconnected = true;
                    break;
            }
        }
    }

    // Drop connection, since disconnection didn't successed
    if (peer != nullptr && !disconnected) {
        enet_peer_reset(peer);
    }

    if (client != nullptr) {
        enet_host_destroy(client);
    }
    enet_deinitialize();
}
