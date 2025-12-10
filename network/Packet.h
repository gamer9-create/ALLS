//
// Created by lalit on 12/2/2025.
//

#ifndef ALLS_PACKET_H
#define ALLS_PACKET_H
#include "Player.h"

enum PacketType {
    PLAYER_UPDATE,
    PLAYER_JOIN,
    PLAYER_LEFT,
    TIME_SYNC,
};

#pragma pack(push, 1)
struct Packet {
    PacketType type;
    PlayerState playerState;
    PlayerJoin playerJoin;
    PlayerLeft playerLeft;
    double timestamp = -1;
};
#pragma pack(pop)

#endif //ALLS_PACKET_H