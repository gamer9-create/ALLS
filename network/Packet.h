//
// Created by lalit on 12/2/2025.
//

#ifndef ALLS_PACKET_H
#define ALLS_PACKET_H
#include "Player.h"

enum PacketType {
    NONE,
    PLAYER_UPDATE,
    PLAYER_JOIN,
    PLAYER_LEFT,
    PACKET_TYPE_END
};

#pragma pack(push, 1)
struct Packet {
    PacketType type = NONE;
    PlayerState playerState;
    PlayerJoin playerJoin;
    PlayerLeft playerLeft;
};
#pragma pack(pop)

#endif //ALLS_PACKET_H