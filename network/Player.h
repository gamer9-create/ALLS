//
// Created by lalit on 11/27/2025.
//

#ifndef ALLS_PLAYER_H
#define ALLS_PLAYER_H
#include <raymath.h>
#include <vector>

#pragma pack(push, 1)
struct PlayerJoin {
    long id = 0;
    Vector2 starting_location = {0, 0};
    double timestamp = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PlayerLeft {
    long id = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PlayerState {
    long id = 0;
    Vector2 position = {0,0};
    Vector2 velocity = {0,0};
    float speed = 0;
    float rotation = 0;
    double timestamp = 0;
};
#pragma pack(pop)

struct Player {
    long PlayerID;
    PlayerState CurrentState = PlayerState();
    PlayerState LastState = PlayerState();
    std::vector<double> UpdateTimes;
    double LastUpdateTime = 0;
    double AverageUpdateTime = 0;
    PlayerState LocalState = PlayerState();
    std::vector<PlayerState> PreviousPlayerStates;
};


#endif //ALLS_PLAYER_H