//
// Created by lalit on 11/16/2025.
//

#include "Client.h"
#include "raymath.h"
#include <array>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "enet/enet.h"

std::atomic<bool> Running;

ENetHost* client = {0};
ENetPeer* peer = {0};
std::vector<Vector2> client_positions;

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

    int number_of_floats = 0;
    std::vector<Vector2> floats;
    while (Running.load(std::memory_order_relaxed)) {
        int active = enet_host_service(client, &client_event, 500);
        if (active > 0) {
            switch (client_event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    number_of_floats = (int) (client_event.packet->dataLength / 4);
                    floats.clear();
                    for (int i = 0; i < number_of_floats/2; i++) {
                        float x, y;
                        int idx = i * 8;
                        std::memcpy(&x, client_event.packet->data + idx, 4);
                        std::memcpy(&y, client_event.packet->data + idx + 4, 4);
                        floats.push_back({x,y});
                    }
                    client_positions = floats;
                    enet_packet_destroy(client_event.packet);
                    break;
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

void UpdatePosition(float x, float y) {
    if (peer != nullptr && client != nullptr) {
        std::array<uint8_t, 8> position;
        position[0] = x;
        position[1] = y;
        auto bytes = std::bit_cast<std::array<uint8_t, 8>>(position);

        ENetPacket* packet = enet_packet_create(&bytes, sizeof(bytes), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
        //enet_packet_destroy(packet);
    }
}

std::vector<Vector2> GetPositions() {
    return client_positions;
}

void StopClient() {
    Running.store(false, std::memory_order_relaxed);
    uint8_t disconnected = false;
    if (client != nullptr) {
        ENetEvent client_event;
        while (enet_host_service(client, &client_event, 300) > 0) {
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